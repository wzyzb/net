#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <iostream>
using namespace std;
#include <util/stringfun.h>
#include <db/oracle/oraclepublic.h>
#include <db/oracle/connectionimpl.h>
#include <db/oracle/resultsetimpl.h>
#include <db/oracle/callablestatementimpl.h>

using namespace avocado_oracle;

CCallableStatementImpl::CCallableStatementImpl()
	:m_pStmt(NULL),m_Sql(""),m_iCurrentIndex(0),
	m_iResultsetCount(0),m_ResultsetIndex(NULL)
{
}

CCallableStatementImpl::~CCallableStatementImpl()
{
	if (this->m_ResultsetIndex!=NULL)
	{
		delete this->m_ResultsetIndex;
	}
	this->close();
	//¼õÉÙÒýÓÃ
	if (m_pRefStmt != NULL)
	{
		m_pRefStmt->decCount();
	}
}

bool CCallableStatementImpl::execute()
{
	try
	{
		if( this->m_pStmt->execute() == oracle::occi::Statement::UNPREPARED )
			return false;		
	}
	catch (oracle::occi::SQLException& e)
	{
		throw CSQLException(e.getErrorCode(), e.what());  
	}	
	return true;
}

void CCallableStatementImpl::close()
{
	if (this->m_pStmt!=NULL)
	{
		//this->m_pConn->getConnection()->terminateStatement(this->m_pStmt);		
		//m_pStmt = NULL;
	}
}

bool CCallableStatementImpl::registerResultset(int nIndex)
{
	BEGIN_DBTRY;
	m_iResultsetCount++;

	//this->m_iCurrentIndex = nIndex;
	if (this->m_ResultsetIndex==NULL)
	{
		this->m_ResultsetIndex = new std::list<int>();
	}
	this->m_ResultsetIndex->push_back(nIndex);
	this->m_pStmt->registerOutParam(nIndex, OCCICURSOR);
	END_DBTRY;
	return true;
}

bool CCallableStatementImpl::registerReturn(EDATATYPE nEDATATYPE)
{
	BEGIN_DBTRY
	int nParamIndex = 1;
	m_pStmt->registerOutParam(nParamIndex, avocado_oracle::toOcciType(nEDATATYPE));
	return true;
	END_DBTRY
}

//
void CCallableStatementImpl::setByte(int nParamIndex, char Value, bool isRegOut)
{
	BEGIN_DBTRY
	//if(isRegOut)
	//	m_pStmt->registerOutParam(nParamIndex, OCCICHAR);
	this->setBytes(nParamIndex, (const char*)&Value, 1, isRegOut);
	END_DBTRY
}

/*
//
void CCallableStatementImpl::setBoolean(int nParamIndex, bool Value, bool isRegOut)
{
	BEGIN_DBTRY
	//if(isRegOut)		
	//	m_pStmt->registerOutParam(nParamIndex, OCCIBOOL);
	char cc;
	Value ? cc=0xf: cc=0x0;
	this->setByte(nParamIndex, cc, isRegOut);
	END_DBTRY
}
*/

//
void CCallableStatementImpl::setBytes(int nParamIndex, const char* Value, int nLength, bool isRegOut)
{
	BEGIN_DBTRY
	if(isRegOut)		
		m_pStmt->registerOutParam(nParamIndex, OCCIBYTES, nLength);//BYTES_MAX_SIZE

	Environment * env = m_pConn->getEnvironment();
	Bytes bt( (unsigned char *)Value, nLength, 0, env );
	m_pStmt->setBytes(nParamIndex, bt);	
	END_DBTRY
}

//
void CCallableStatementImpl::setString(int nParamIndex, const std::string& Value, bool isRegOut)
{
	BEGIN_DBTRY
	if(isRegOut)
		m_pStmt->registerOutParam(nParamIndex, OCCISTRING, STRING_MAX_SIZE);
	//OCCI_STD_NAMESPACE::string sValue = Value;
	string tmp = Value;
	if (tmp.size()==0)
	{
		string tmp = " ";
	}
	m_pStmt->setString(nParamIndex, tmp);
	END_DBTRY
}

//
void CCallableStatementImpl::setDouble(int nParamIndex, double Value, bool isRegOut)
{
	BEGIN_DBTRY
	if(isRegOut)
		m_pStmt->registerOutParam(nParamIndex, OCCIDOUBLE);
	m_pStmt->setDouble(nParamIndex, Value) ;
	END_DBTRY
}

//
void CCallableStatementImpl::setInt(int nParamIndex, int Value, bool isRegOut)
{
	BEGIN_DBTRY
	if(isRegOut)
		m_pStmt->registerOutParam(nParamIndex, OCCIINT);
	m_pStmt->setInt (nParamIndex, Value) ;
	END_DBTRY
}

//
void CCallableStatementImpl::setNull(int nParamIndex, EDATATYPE nEDATATYPE, bool isRegOut)
{
	BEGIN_DBTRY
	//isRegOut;
	m_pStmt->setNull (nParamIndex, avocado_oracle::toOcciType(nEDATATYPE));
	END_DBTRY
}

//
void CCallableStatementImpl::setDate(int nParamIndex, CDateTime Value, bool isRegOut)
{
	BEGIN_DBTRY
	if(isRegOut)
		m_pStmt->registerOutParam(nParamIndex, OCCIDATE);// or OCCIDATE?

	Environment * env = m_pConn->getEnvironment();
	Date dat( env, Value.getYear(), Value.getMonth(), Value.getDay(),
		Value.getHour(), Value.getMinute(), Value.getSecond() );
	m_pStmt->setDate(nParamIndex, dat);	
	END_DBTRY
}

//
void CCallableStatementImpl::setTime(int nParamIndex, CDateTime Value, bool isRegOut)
{
	this->setDate(nParamIndex, Value, isRegOut) ;
/*	BEGIN_DBTRY
	if(isRegOut)
		m_pStmt->registerOutParam(nParamIndex, OCCIDATE);

	
	Environment * env = m_pConn->getEnvironment();
	Date dat( env, Value.getYear(), Value.getMonth(), Value.getDay(),
		Value.getHour(), Value.getMinute(), Value.getSecond() );
	m_pStmt->setDate(nParamIndex, dat);	
	END_DBTRY*/
}

//
void CCallableStatementImpl::setTimestamp(int nParamIndex, CDateTime Value, bool isRegOut)
{
	BEGIN_DBTRY
	if(isRegOut)
		m_pStmt->registerOutParam(nParamIndex, OCCITIMESTAMP);

	int tzhour = 0, tzmin = 0; //timezone hour = 8
	Environment * env = m_pConn->getEnvironment();
	Timestamp ts( env, Value.getYear(), Value.getMonth(), Value.getDay(),
		Value.getHour(), Value.getMinute(), Value.getSecond(), Value.getMillSecond(), tzhour, tzmin);
	m_pStmt->setTimestamp(nParamIndex, ts);
	END_DBTRY
}

//
void CCallableStatementImpl::setLong64(int nParamIndex, Long64 Value, bool isRegOut)
{
	BEGIN_DBTRY
		if(isRegOut)
		{
			//m_pStmt->registerOutParam(nParamIndex, OCCINUMBER);
		}

		Environment * env = m_pConn->getEnvironment();
		//char buf[20];
		//_i64toa(Value,buf,10);
		oracle::occi::Number num(1);
		//string tmp = buf;
		string tmp = Int64ToStr(Value);
		num.fromText(env,tmp,"99999999999999999999");
		this->m_pStmt->setNumber(nParamIndex, num);
		//string tmp1 = num.toText(env,"999999999999999");
		//cout<<"test fromText:"<<tmp1<<endl;
	END_DBTRY	
}

//
bool CCallableStatementImpl::hasMoreResultsets()
{	
	if(m_iResultsetCount>0)
	{		
		return true;
	}		
	return false;	
}

//
CAutoPtr<CResultSet> CCallableStatementImpl::getNextResultSet()
{
	BEGIN_DBTRY
	if(m_iResultsetCount>0)
	{ 
		m_iResultsetCount--;

		this->m_iCurrentIndex++;
		CResultSetImpl* rsi = new CResultSetImpl();
		rsi->m_pRefStmt = this->m_pRefStmt;
		rsi->m_pConn = this->m_pConn;
		this->m_pRefStmt->incCount();
		std::list<int>::iterator itor = this->m_ResultsetIndex->begin();
		for (int i=1;i<this->m_iCurrentIndex;i++)
		{
			itor++;
		}
		int index = *itor;
		rsi->m_pSet = m_pStmt->getCursor(index);
		CAutoPtr<CResultSet> autoptr(rsi);
		return autoptr;		 
	}
	else
	{
		this->m_iCurrentIndex = 0;
		this->m_ResultsetIndex->clear();
	}
	END_DBTRY
	return NULL;
}

//
char CCallableStatementImpl::getByte(int nIndex)
{
	BEGIN_DBTRY
	Bytes bt = m_pStmt->getBytes(nIndex);
	if( bt.isNull() )
		return '\0';

	return bt.byteAt(0);
	END_DBTRY
}

//
char CCallableStatementImpl::getByte(const char* pColumnName)
{
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"fetch data by column name not supported!");
	//BEGIN_DBTRY
	//return getResultSetImpl( m_pStmt->getResultSet() )->getByte(pColumnName) ;
	//END_DBTRY
}

//
bool CCallableStatementImpl::getBytes(int nIndex, char* pOutBuffer, int& nLength)
{
	BEGIN_DBTRY
	Bytes bt = m_pStmt->getBytes(nIndex);
	if( bt.isNull() )
		return false;

	bt.getBytes( (unsigned char *)pOutBuffer, bt.length() );
	nLength = bt.length();
	return true;
	END_DBTRY
}

//
bool CCallableStatementImpl::getBytes(const char* pColumnName, char* pOutBuffer, int& nLength)
{
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"fetch data by column name not supported!");
}

/*
//
bool CCallableStatementImpl::getBoolean(int nIndex)
{
	BEGIN_DBTRY
	Bytes bt = m_pStmt->getBytes(nIndex);
	if( bt.isNull() )
		return false;
	if( bt.byteAt(0) &0xff ) //0-based index	
		return true;
	return false;
	END_DBTRY
}
*/

/*
//
bool CCallableStatementImpl::getBoolean(const char* pColumn)
{
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"fetch data by column name not supported!");
}
*/

//
std::string CCallableStatementImpl::getString(int nIndex)
{
	BEGIN_DBTRY
		if (m_pStmt->isNull(nIndex))
		{
			return DEFAULT_STRING;
		}

		string tmp = m_pStmt->getString(nIndex).c_str();

		if (tmp == " ")
		{
			tmp = "";
		}
	
		return tmp;
	END_DBTRY
}

//
std::string CCallableStatementImpl::getString(const char* pColumn)
{
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"fetch data by column name not supported!");
}

//
int CCallableStatementImpl::getInt(int nIndex)
{
	BEGIN_DBTRY
	return m_pStmt->getInt(nIndex);
	END_DBTRY
}

//
int CCallableStatementImpl::getInt(const char* pColumn)
{
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"fetch data by column name not supported!");
}

//
double CCallableStatementImpl::getDouble(int nIndex)
{
	BEGIN_DBTRY
	return m_pStmt->getDouble(nIndex);
	END_DBTRY
}

//
double CCallableStatementImpl::getDouble(const char* pColumn)
{
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"fetch data by column name not supported!");
}

//
bool CCallableStatementImpl::isNull(int nIndex)
{
	BEGIN_DBTRY
	return m_pStmt->isNull(nIndex);
	END_DBTRY
}

//
bool CCallableStatementImpl::isNull(const char* pColumn)
{
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"fetch data by column name not supported!");
}

//
CDateTime CCallableStatementImpl::getDate(int nIndex)
{
	BEGIN_DBTRY
	int year = 0, misecond = 0, tzone = 0; 
	unsigned int month, day, hour, minute,second;
	Date dt = m_pStmt->getDate(nIndex);	
	dt.getDate(year, month, day, hour, minute, second);
	return CDateTime(year, month, day, hour, minute,second, misecond, tzone);
	END_DBTRY
}

//
CDateTime CCallableStatementImpl::getDate(const char* pParamName)
{
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"fetch data by column name not supported!");
}

//
CDateTime CCallableStatementImpl::getTime(int nIndex)
{
	BEGIN_DBTRY
	return this->getDate(nIndex); 
	END_DBTRY
}

//
CDateTime CCallableStatementImpl::getTime(const char* pParamName)
{
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"fetch data by column name not supported!");
}

//
CDateTime CCallableStatementImpl::getTimestamp(int nIndex)
{
	BEGIN_DBTRY		
		
		Timestamp tmStamp = m_pStmt->getTimestamp(nIndex);	
	
		CDateTime rtn;
		string tmp = tmStamp.toText("yyyy-mm-dd hh24:mi:ss ff3",0);
		rtn.parse(tmp.c_str(),"YYYY-MM-DD hh:mm:ss 000");
		return rtn;
		
	END_DBTRY
}

//
CDateTime CCallableStatementImpl::getTimestamp(const char* pColumn)
{
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"fetch data by column name not supported!");
}

//
Long64 CCallableStatementImpl::getLong64(int nIndex)
{
	BEGIN_DBTRY
		Environment * env = m_pConn->getEnvironment();
		oracle::occi::Number num = this->m_pStmt->getNumber(nIndex);
		//Long64 rtn = _atoi64(num.toText(env,"99999999999999999999").c_str());
		Long64 rtn = atol64(num.toText(env,"99999999999999999999").c_str());
		return rtn;
	END_DBTRY	
}

//
Long64 CCallableStatementImpl::getLong64(const char* pParamName)
{
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"fetch data by column name not supported!");
}

//
void CCallableStatementImpl::prepare(const char* pSql)
{
	BEGIN_DBTRY
		m_Sql = pSql;	
		m_Sql = StrReplaceAddIndex(m_Sql,"?",":p");
		this->m_pStmt->setSQL(m_Sql);
	END_DBTRY
}
