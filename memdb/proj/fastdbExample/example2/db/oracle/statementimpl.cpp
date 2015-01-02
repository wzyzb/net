//#include <iostream>
//using namespace std;
#include <util/stringfun.h>
#include <db/oracle/oraclepublic.h>
#include <db/oracle/resultsetimpl.h>
#include <db/oracle/connectionimpl.h>
#include <db/oracle/statementimpl.h>

#ifndef WIN32
	#include <ctype.h>
#endif

#include <iostream>
using namespace std;

using namespace avocado_oracle;

#define END_DBSTMTTRY \
}\
catch (oracle::occi::SQLException& e)\
{\
	ThrowSQLException<CSQLException>(e.getErrorCode(),"%s,sql=%s",e.what(),this->m_Sql.c_str());\
}\



CStatementImpl::CStatementImpl()
 :m_pStmt(NULL),m_Sql(""),m_pRefStmt(NULL),m_pRst(NULL)
{
	
}

CStatementImpl::~CStatementImpl()
{
	if (m_pRefStmt != NULL)
	{
		m_pRefStmt->decCount();
	}
	close();
}

//
CAutoPtr<CResultSet> CStatementImpl::executeQuery(const char* pSQL)
{
	BEGIN_DBTRY
		CResultSetImpl* rsi = new CResultSetImpl();
		CAutoPtr<CResultSet> autoptr(rsi);

		this->prepare(pSQL);
		rsi->m_pSet = m_pStmt->executeQuery();	

		this->m_pRst = rsi;
		rsi->m_pRefStmt = this->m_pRefStmt;
		rsi->m_pConn = this->m_pConn;
		rsi->m_pStmt = this;
		this->m_pRefStmt->incCount();					
		
		return autoptr;		
	END_DBSTMTTRY
	
	return NULL;
}

//
int CStatementImpl::executeUpdate(const char* pSQL)
{
	int rtn;
	BEGIN_DBTRY
		this->prepare(pSQL);
		rtn = this->m_pStmt->executeUpdate();
		if (this->m_pConn->getAutoCommit())
		{
			this->m_pConn->commit();
		}
		return rtn;
	END_DBSTMTTRY
	return -1;
}

//
bool CStatementImpl::execute(const char* pSQL)
{
	BEGIN_DBTRY
		this->prepare(pSQL);
		if( this->m_pStmt->execute() == oracle::occi::Statement::UNPREPARED )
			return false;
		if (this->m_pConn->getAutoCommit())
		{
			this->m_pConn->commit();
		}
	END_DBSTMTTRY
	return true;
}

//
bool CStatementImpl::execute()
{
	if (m_Sql == "")
	{
		throw CSQLException(ERROR_DATABASE_INVALIDOPER,"please prepare sql first!");
	}
	BEGIN_DBTRY	
		if( this->m_pStmt->execute() == oracle::occi::Statement::UNPREPARED )
			return false;
		if (this->m_pConn->getAutoCommit())
		{
			this->m_pConn->commit();
		}

	END_DBSTMTTRY
	
	return true;
}

//
CAutoPtr<CResultSet> CStatementImpl::executeQuery()
{
	if (m_Sql == "")
	{
		throw CSQLException(ERROR_DATABASE_INVALIDOPER,"please prepare sql first!");
	}
	BEGIN_DBTRY
		CResultSetImpl* rsi = new CResultSetImpl();	
		CAutoPtr<CResultSet> autoptr(rsi);
		rsi->m_pSet = m_pStmt->executeQuery();
		m_pRst = rsi;		
		rsi->m_pRefStmt = this->m_pRefStmt;
		rsi->m_pConn = this->m_pConn;
		rsi->m_pStmt = this;
		this->m_pRefStmt->incCount();
			
			
		return autoptr;		
	END_DBSTMTTRY
	
	return NULL;
}

//
int CStatementImpl::executeUpdate()
{
	if (m_Sql == "")
	{
		throw CSQLException(ERROR_DATABASE_INVALIDOPER,"please prepare sql first!");
	}

	int rtn;
	try
	{
		rtn = this->m_pStmt->executeUpdate();
		if (this->m_pConn->getAutoCommit())
		{
			this->m_pConn->commit();
		}
		return rtn;
//END_DBSTMTTRY

	}
catch (oracle::occi::SQLException& e)
{
	ThrowSQLException<CSQLException>(e.getErrorCode(),"%s,sql=%s",e.what(),this->m_Sql.c_str());
}
catch (exception& ex)
{
	ThrowSQLException<CSQLException>(ERROR_DATABASE_CLOSED,"%s,sql=%s",ex.what(),this->m_Sql.c_str());
}
catch (...)
{
	ThrowSQLException<CSQLException>(-1,"%s,sql=%s","catch ...",this->m_Sql.c_str());
}
	return -1;
}

//
void CStatementImpl::close()
{
	if (this->m_pStmt!=NULL)
	{
		//this->m_pConn->terminateStatement(this->m_pStmt);
		//m_pStmt = NULL;
	}	

	this->m_Clobs.clear();
}

int CStatementImpl::getTimeZoneOffset(bool Throw)
{
	BEGIN_DBTRY
	int rtn=0;
	bool issigned = false;
	
	std::string sql = "select dbtimezone from dual";		
	this->m_pStmt->setSQL(sql.c_str());
	oracle::occi::ResultSet* rset = this->m_pStmt->executeQuery();
	
	if (rset->next())
	{		
		std::string tmp = rset->getString(1);		
		//tmp = "-05:00";
		if (tmp[0]=='-')
		{
			issigned = true;			
			//cout<<"timezone="<<tmp.c_str()<<endl;
		}
		tmp = tmp.substr(1,tmp.size()-1);
		CDateTime dttimezone;
		dttimezone.parse(tmp.c_str(),"hh:mm");
		rtn = dttimezone.getHour();

		if (issigned)
		{
			rtn = -rtn;
		}

		this->m_pStmt->closeResultSet(rset);
		return rtn;
	}
	else
	{
		this->m_pStmt->closeResultSet(rset);
		throw CAvoException("CStatementImpl::getTimeZoneOffset error!");
	}
	
	
	
	return rtn;
	END_DBSTMTTRY;
}

//
Long64 CStatementImpl::getInsertID(void* pParam)
{
	BEGIN_DBTRY
	Long64 rtn=0;
	std::string tmp = (char *)pParam;
	tmp += ".CURRVAL";
	std::string sql = "select " + tmp + " from dual";		
	this->m_pStmt->setSQL(sql.c_str());
	oracle::occi::ResultSet* rset = this->m_pStmt->executeQuery();
	
	if (rset->next())
	{
		Environment * env = m_pConn->getEnvironment();
		oracle::occi::Number num = rset->getNumber(1);
		std::string tmp = num.toText(env,"99999999999999999999");
		//rtn = _atoi64(tmp.c_str()); 
		rtn = atol64(tmp.c_str());
	}
	
	this->m_pStmt->closeResultSet(rset);
	
	return rtn;
	END_DBSTMTTRY;
}

void CStatementImpl::clearParameters()
{
}

//
void CStatementImpl::setByte(int nParamIndex, char Value)
{
	//setXXX method works well?
	BEGIN_DBTRY
		//Environment * env = Environment::createEnvironment (Environment::OBJECT);	
		Environment * env = m_pConn->getEnvironment();
		Bytes bt( (unsigned char *)&Value, 1, 0, env );
		m_pStmt->setBytes(nParamIndex, bt);
		//Environment::terminateEnvironment (env);
	//this->setBytes(nParamIndex, &Value, 1);
	//this->m_pStmt->setDataBuffer(nParamIndex,&Value,oracle::occi::OCCI_SQLT_CHR,1,NULL);			
	END_DBSTMTTRY
}

string CStatementImpl::getInsertTableName()
{
	string sql = this->m_Sql;
	int pos;

#ifdef WIN32	
	char *pcopy;
    pcopy = _strupr( _strdup( sql.c_str() ) );
	sql = pcopy;
	free( pcopy );
#else
	std::string tmp=sql;
	for(unsigned int i=0; i< tmp.length(); i++)
	{
		tmp[i]= toupper(tmp[i]);
	}	
	sql = tmp;
#endif

	if ((pos = sql.find("INSERT INTO "))==string::npos)
	{
		return "";	
	}
		
	sql = sql.substr(pos + 12,32);

	pos = sql.find(" ");

	string tablename= sql.substr(0,pos);

	return tablename;
}

void CStatementImpl::saveBytes(const char* strColumnName,const unsigned char* Value, int nLength,bool SetSequence,string SequenceName,bool SetPkName,string PkName)
{
	ResultSet *rset = NULL;
	try
	{
		string seqname	= "S_"+this->getInsertTableName();
		
		if (SetSequence)
		{
			seqname = SequenceName;
		}

		string pkname = "id";
		if (SetPkName)
		{
			pkname = PkName;
		}
		 
		int id = this->getInsertID((void *)seqname.c_str());

		string sql = string("select ")+strColumnName+" from " + this->getInsertTableName() + " where "+ pkname +" =:p1 for update";
		this->m_pStmt->setSQL(sql);
		this->m_pStmt->setInt(1,id);
				
		rset = this->m_pStmt->executeQuery();
		
		if (rset->next())
		{
			Clob clob = rset->getClob(1);
			clob.write(nLength,(unsigned char*)Value,nLength);
			this->m_pStmt->executeUpdate();						
		}
		this->m_pStmt->closeResultSet(rset);
	}
	catch (oracle::occi::SQLException& e)
	{
		if (rset!=NULL)
		{
			this->m_pStmt->closeResultSet (rset);
		}		
		ThrowSQLException<CSQLException>(e.getErrorCode(),"%s,sql=%s",e.what(),this->m_Sql.c_str());
	}
}

//
void CStatementImpl::setBytes(int nParamIndex, const unsigned char* Value, int nLength)
{
	try
	{
		Clob clob = this->m_pRst->m_pSet->getClob(nParamIndex);
		clob.write(nLength,(unsigned char*)Value,nLength);					
	}
	catch (oracle::occi::SQLException& e)
	{	
		ThrowSQLException<CSQLException>(e.getErrorCode(),"%s,sql=%s",e.what(),this->m_Sql.c_str());
	}
	catch (...)
	{
		ThrowException<CAvoException>("CStatementImpl::setBytes exception ...,sql=%s",this->m_Sql.c_str());
	}	
}

//
/*
void CStatementImpl::setBoolean(int nParamIndex, bool Value)
{
	BEGIN_DBTRY
		char c;
		Value ? c=0xf: c=0x0;
		this->setByte(nParamIndex, c);
	END_DBSTMTTRY
}
*/
//
void CStatementImpl::setString(int nParamIndex, const std::string& Value)
{
	BEGIN_DBTRY
	//OCCI_STD_NAMESPACE::string sValue = Value;
	string tmp = Value;
	if (tmp.size()==0)
		tmp = " ";
	m_pStmt->setString(nParamIndex, tmp);
	END_DBSTMTTRY
}

//
void CStatementImpl::setInt(int nParamIndex, int Value)
{
	BEGIN_DBTRY
	m_pStmt->setInt (nParamIndex, Value) ;
	END_DBSTMTTRY
}

//
void CStatementImpl::setDouble(int nParamIndex, double Value)
{
	BEGIN_DBTRY
	m_pStmt->setDouble(nParamIndex, Value) ;
	END_DBSTMTTRY
}

//
void CStatementImpl::setNull(int nParamIndex, EDATATYPE nDataType)
{
	BEGIN_DBTRY
	m_pStmt->setNull (nParamIndex, toOcciType(nDataType));
	END_DBSTMTTRY
	/*
	switch(nSqlType)
	{
	case SQL_INT:
		m_pStmt->setNull (nParamIndex, OCCIINT);
		break;
	case SQL_VARCHAR:
		m_pStmt->setNull (nParamIndex, OCCICHAR);
		break;
	case SQL_DATE:
		m_pStmt->setNull (nParamIndex, OCCIDATE);
		break;
	case SQL_TIME:
		m_pStmt->setNull (nParamIndex, OCCIDATE);
		break;
	case SQL_TIMESTAMP:
		m_pStmt->setNull (nParamIndex, OCCITIMESTAMP);
		break;	
	default:
		break;
	}*/	
}

//
void CStatementImpl::setDate(int nParamIndex, CDateTime Value)
{
	BEGIN_DBTRY
		//Environment * env = Environment::createEnvironment (Environment::OBJECT);
		Environment * env = m_pConn->getEnvironment();
		Date dat(env, Value.getYear(), Value.getMonth(), Value.getDay(),
			Value.getHour(), Value.getMinute(), Value.getSecond());
		//Environment::terminateEnvironment (env);
		m_pStmt->setDate(nParamIndex, dat);		
	END_DBSTMTTRY
}

//
void CStatementImpl::setTime(int nParamIndex, CDateTime Value)
{
	this->setDate(nParamIndex, Value)  ;
}

//
void CStatementImpl::setTimestamp(int nParamIndex, CDateTime Value)
{
	BEGIN_DBTRY
		int tzhour = 0, tzmin = 0; //timezone hour = 8
		//Environment * env = Environment::createEnvironment (Environment::OBJECT);
		Environment * env = m_pConn->getEnvironment();
		Timestamp ts( env, Value.getYear(), Value.getMonth(), Value.getDay(), \
				Value.getHour(), Value.getMinute(), Value.getSecond(), \
				Value.getMillSecond() , tzhour, tzmin); 
		m_pStmt->setTimestamp(nParamIndex, ts);
		//Environment::terminateEnvironment (env);
	END_DBSTMTTRY
}

//
void CStatementImpl::setLong64(int nParamIndex, Long64 Value)
{
	BEGIN_DBTRY			
		Environment * env = m_pConn->getEnvironment();
		//char buf[20];
		//_i64toa(Value,buf,10);
		//string tmp = buf;
		string tmp = Int64ToStr(Value);
		
		oracle::occi::Number num(1);
		num.fromText(env,tmp,"99999999999999999999");
		

		this->m_pStmt->setNumber(nParamIndex, num);

		//string tmp1 = num.toText(env,"9999999999999999999");
		//cout<<"test fromText:"<<tmp1<<endl;
	END_DBSTMTTRY	
}

void CStatementImpl::prepare(const char* pSql)
{
	BEGIN_DBTRY
		if (m_pRst != NULL)
		{
			m_pRst->close();
			m_pRst = NULL;
		}
		
		m_Sql = pSql;
		m_Sql = StrReplace(m_Sql,"?",":p");		
		this->m_pStmt->setSQL(m_Sql);
	END_DBSTMTTRY
}

const char* CStatementImpl::getSQL() const
{
	return this->m_Sql.c_str();
}
