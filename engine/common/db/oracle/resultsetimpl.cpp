#ifdef WIN32
#pragma warning(disable : 4786)
#endif

#include <algorithm>
#ifdef WIN32
#else
#include <ctype.h>
#define _strdup(a) strdup(a)
#endif
#include <db/oracle/oraclepublic.h>
#include <db/oracle/connectionimpl.h>
#include <db/oracle/statementimpl.h>
#include <db/oracle/resultsetimpl.h>

#define BEGIN_RESULTSET \
if (this->m_pSet != NULL) \
{\

#define END_RESULTSET \
}\
else\
{\
	throw CSQLException(ERROR_DATABASE_INVALIDOPER,"resultset was closed");\
}\

  //抛出空指针异常
#define NULLVALUEEXCEPTOIN ThrowSQLException<CSQLException>(ERROR_DATABASE_NULLVALUE,"%s[%d]NULL VALUE",getColumnName(nIndex).c_str(),nIndex)

#define RSCHECKNULL(bNullThrow,DEFAULT) { \
			if ( bNullThrow ) \
				NULLVALUEEXCEPTOIN; \
			else \
				return DEFAULT; \
		}

//检查是否为空并处理抛出异常
#define CHECKNULL(bNullThrow,DEFAULT)	if (  m_pSet->isNull(nIndex) ) RSCHECKNULL(bNullThrow,DEFAULT);

using namespace avocado_oracle;

//
CResultSetImpl::CResultSetImpl() : m_pSet(NULL),m_pRefStmt(NULL),m_pConn(NULL),m_pStmt(NULL)
{
}

CResultSetImpl::~CResultSetImpl()	
{
	this->close();	
}

//
void CResultSetImpl::close()
{
	//ResultSet::getStatement() returns a pointor of Statement
	try
	{
		
		if (this->m_pSet != NULL)
		{
			this->m_pSet->getStatement()->closeResultSet(m_pSet);
			this->m_pSet = NULL;

			if (this->m_pRefStmt != NULL)
			{
				
				
				this->m_pRefStmt->decCount();
				this->m_pRefStmt = NULL;
			}
			
			if (this->m_pStmt != NULL)
			{
				if (this->m_pStmt->m_pRst == this)
				{
					this->m_pStmt->m_pRst = NULL;
				}
			}
		}	
	}
	catch (oracle::occi::SQLException& e)
	{
		printf("CResultSetImpl::close oracle::occi::SQLException: %d-%s",e.getErrorCode(),e.getMessage());
	}
}

//
int CResultSetImpl::getColumnCount() const
{
	BEGIN_DBTRY
		BEGIN_RESULTSET
			return	m_pSet->getColumnListMetaData().size();
		END_RESULTSET	
	END_DBTRY
}

EDATATYPE CResultSetImpl::getColumnDataType(int nIndex)
{
	BEGIN_DBTRY
		BEGIN_RESULTSET
			vector<MetaData> v1 = m_pSet->getColumnListMetaData();
			if(nIndex > v1.size())
				throw CSQLException(-6,"invalid column or column exceeds!");

			//oracle::occi::Type vtype = v1[nIndex - 1].getAttributeType(nIndex);
			int vtype = ( v1[nIndex - 1].getInt(MetaData::ATTR_DATA_TYPE) );
			int precision = ( v1[nIndex - 1].getInt(MetaData::ATTR_PRECISION) );
			int scale = ( v1[nIndex - 1].getInt(MetaData::ATTR_SCALE) );
			
			//cout<<"in type"<<vtype<<";"<<precision<<";"<<scale<<endl;

			if ((vtype==OCCI_SQLT_NUM)&&(scale>0))
			{
				vtype = oracle::occi::OCCIDOUBLE;
			}
			if ((vtype==OCCI_SQLT_NUM)&&(precision>16))
			{
				//由于occi中没有定义long64,暂时用OCCIUNSIGNED_INT来替换
				vtype = oracle::occi::OCCIUNSIGNED_INT;
			}
			return avocado_oracle::toOracleType( (oracle::occi::Type)vtype );
		END_RESULTSET
	END_DBTRY
}

//
std::string CResultSetImpl::getColumnName(int nIndex) const
{
	BEGIN_DBTRY
		BEGIN_RESULTSET
			vector<MetaData> v1 = m_pSet->getColumnListMetaData();
			if(nIndex > v1.size())
				return "\0";
			
			return std::string ( v1[nIndex - 1].getString(MetaData::ATTR_NAME).c_str() );
		END_RESULTSET
	END_DBTRY
}

//
bool CResultSetImpl::next()
{	
	BEGIN_DBTRY		
	BEGIN_RESULTSET
		if( m_pSet->next() == ResultSet::END_OF_FETCH )
			return false;
		return true;	
	END_RESULTSET
	END_DBTRY
}

bool CResultSetImpl::getFields() const
{
	BEGIN_DBTRY
		BEGIN_RESULTSET
			//get column list
			vector<MetaData> v1 = m_pSet->getColumnListMetaData();
			for(int i=0; i < v1.size(); i++)
			{
				string strColName = v1[i].getString(MetaData::ATTR_NAME);
				transform(strColName.begin(), strColName.end(), strColName.begin(), toupper);
				m_Fields[strColName] = i + 1;
			}//for
		END_RESULTSET
	END_DBTRY
	return true;
}

//
int CResultSetImpl::findColumn(const char* pColumnName) const
{
	if ( m_Fields.size() == 0 )
		getFields();
	char* p = _strdup(pColumnName);
	{
		int len = strlen(p);
		for ( int i=0 ; i<len ; i++ )
			p[i] = toupper(p[i]);
	}
	map<string,int>::iterator i = this->m_Fields.find(p);
	free(p);
	if ( i == m_Fields.end() )
		return -1;
	return i->second;
}

int CResultSetImpl::getBytes(int nIndex,unsigned char* pOutBuffer,int nBuffSize,int nCharCount,bool bNullThrow,const char* pDefault,unsigned int nOffset) const
{
	int rtn=0;
	BEGIN_DBTRY
		BEGIN_RESULTSET
			Clob clob = this->m_pSet->getClob(nIndex);
			
			if (clob.isNull())
			{
				if (bNullThrow)
				{
					NULLVALUEEXCEPTOIN;
				}
				else
				{
					int size = strlen(pDefault);
					if ( nBuffSize < size )
						size = nBuffSize;
					memcpy(pOutBuffer,pDefault,size);
					rtn = size;
				}
			}

			if ((nCharCount*3 > nBuffSize))
			{
				ThrowSQLException<CSQLException>(ERROR_GETBYTES_BUFFOVERFLOW,"nCharCount*3 < nBuffSize while getBytes nCharCount=%d,nBuffSize=%d",nCharCount,nBuffSize);
			}
			
			rtn = clob.read(nCharCount,pOutBuffer,nBuffSize,nOffset);
			
			return rtn;
		END_RESULTSET
	END_DBTRY		
return rtn;


}

//
int CResultSetImpl::getBytes(const char* pColumnName,unsigned char* pOutBuffer,int nBuffSize,int nCharCount,bool bNullThrow,const char* pDefault,unsigned int nOffset) const
{
	return getBytes( findColumn(pColumnName), pOutBuffer,nBuffSize, nCharCount,bNullThrow,pDefault,nOffset);
}

//
std::string CResultSetImpl::getString(int nIndex,bool bNullThrow,const char* pDefault) const
{
	BEGIN_DBTRY
		BEGIN_RESULTSET
		CHECKNULL(bNullThrow,pDefault);
		string tmp = m_pSet->getString(nIndex);
		if (tmp == " ")
		{
			tmp = "";
		}
		return tmp;
		END_RESULTSET
	END_DBTRY
}

//
std::string CResultSetImpl::getString(const char* pColumn,bool bNullThrow,const char* pDefault) const
{
	//BEGIN_DBTRY
	return getString( findColumn(pColumn) ,bNullThrow,pDefault);
	//END_DBTRY
}

//
int CResultSetImpl::getInt(int nIndex,bool bNullThrow,int nDefault) const
{
	BEGIN_DBTRY
		BEGIN_RESULTSET
		CHECKNULL(bNullThrow,nDefault);
		return m_pSet->getInt(nIndex);
		END_RESULTSET
	END_DBTRY
}

//
int CResultSetImpl::getInt(const char* pColumn,bool bNullThrow,int nDefault) const
{
	return getInt( findColumn(pColumn) , bNullThrow,nDefault);
}

//
double CResultSetImpl::getDouble(int nIndex,bool bNullThrow,double dDefault) const
{
	BEGIN_DBTRY
		BEGIN_RESULTSET
		CHECKNULL(bNullThrow,dDefault);
		return m_pSet->getDouble(nIndex);
		END_RESULTSET
	END_DBTRY
}

//
double CResultSetImpl::getDouble(const char* pColumn,bool bNullThrow,double dDefault) const
{
	return getDouble( findColumn(pColumn) ,bNullThrow,dDefault);
}

//
bool CResultSetImpl::isNull(int nIndex) const
{
	BEGIN_DBTRY
		BEGIN_RESULTSET
			return this->m_pSet->isNull(nIndex);
		END_RESULTSET
	END_DBTRY
}

//
bool CResultSetImpl::isNull(const char* pColumn) const
{
	return isNull( findColumn(pColumn) );
}

//
CDateTime CResultSetImpl::getDate(int nIndex,bool bNullThrow,const CDateTime& Default) const
{
	BEGIN_DBTRY
		BEGIN_RESULTSET
		CHECKNULL(bNullThrow,Default);
		int year = 0, tzone = 0; 
		unsigned int month, day, hour, minute, second, misecond=0;
		Date dt = m_pSet->getDate(nIndex);	
		dt.getDate(year, month, day, hour, minute, second);
		return CDateTime(year, month, day, hour, minute, second, misecond, tzone);
		END_RESULTSET
	END_DBTRY
}

//
CDateTime CResultSetImpl::getDate(const char* pColumn,bool bNullThrow,const CDateTime& Default) const
{
	return getDate( findColumn(pColumn),bNullThrow,Default );
}

//
CDateTime CResultSetImpl::getTime(int nIndex,bool bNullThrow,const CDateTime& Default) const
{
	return this->getDate(nIndex,bNullThrow,Default) ;
}

//
CDateTime CResultSetImpl::getTime(const char* pColumn,bool bNullThrow,const CDateTime& Default) const
{
	return this->getDate(pColumn,bNullThrow,Default) ;
}

//
CDateTime CResultSetImpl::getTimestamp(int nIndex,bool bNullThrow,const CDateTime& Default) const
{
	BEGIN_DBTRY
		BEGIN_RESULTSET
		CHECKNULL(bNullThrow,Default);
		Timestamp tmStamp = m_pSet->getTimestamp(nIndex);	
	
		CDateTime rtn;
		rtn.parse(tmStamp.toText("yyyy-mm-dd hh24:mi:ss xff",0).c_str(),"YYYY-MM-DD hh:mm:ss 000");
		return rtn;
		END_RESULTSET
	END_DBTRY
}

//
CDateTime CResultSetImpl::getTimestamp(const char* pColumn,bool bNullThrow,const CDateTime& Default) const
{
	return getTimestamp( findColumn(pColumn),bNullThrow,Default);
}

//
Long64 CResultSetImpl::getLong64(int nIndex,bool bNullThrow,Long64 nDefault) const
{
	BEGIN_DBTRY
		BEGIN_RESULTSET
		Environment * env = m_pConn->getEnvironment();
		CHECKNULL(bNullThrow,nDefault);
		oracle::occi::Number num = m_pSet->getNumber(nIndex);
		OCCI_STD_NAMESPACE::string tmp = num.toText(env,"99999999999999999999");
		Long64 rtn = atol64(tmp.c_str());
		return rtn;
		END_RESULTSET
	END_DBTRY	
}

//
Long64 CResultSetImpl::getLong64(const char* pParamName,bool bNullThrow,Long64 nDefault) const
{
	return getLong64( findColumn(pParamName),bNullThrow,nDefault );
}

