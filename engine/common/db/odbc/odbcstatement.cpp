#define _USE_STL_
#include "odbcpublic.h"
#include "odbcresultset.h"
#include "odbcstatement.h"
#include <util/stringfun.h>
using namespace my_odbc;


#define _M_TEXTSIZE_   12000



COdbcStatement::COdbcStatement(COdbcConnection& Conn)
{
	m_pConn = &Conn;
	this->m_pStmt = NULL;
	this->m_pUsedResultSet = NULL;
	SQLRETURN nRet = SQLAllocHandle(SQL_HANDLE_STMT, Conn.m_pConn, &m_pStmt );
	CHECK_CONN_AND_THROW( nRet , Conn.m_pConn , "create statement error" );
}

COdbcStatement::~COdbcStatement()
{
	try
	{
		close();
	}
	catch(...)
	{
	}
}
/**
* 添加
*/
int COdbcStatement::getTimeZoneOffset(bool Throw)
{
	return 0;
}
const char* COdbcStatement::getSQL() const
{
	return NULL;
}
void COdbcStatement::saveBytes(const char* strColumnName, const unsigned char* Value,
							   int nLength,bool SetSequence,string SequenceName,
							   bool SetPkName,string PkName)
{
}
void COdbcStatement::close()
{
	VALIDSTMT;
	clearParameters();
	if( m_pUsedResultSet != NULL )
	{
		m_pUsedResultSet->close();
		m_pUsedResultSet = NULL;
	}
	SQLRETURN nRet = ::SQLFreeStmt(m_pStmt, SQL_DROP);
	CHECK_CONN_AND_THROW( nRet , m_pStmt , "close statement error" );
	m_pStmt = SQL_NULL_HSTMT;
}

long64 COdbcStatement::getLastInsertID()
{
	VALIDSTMT;
#ifdef _MSSQL_SERVER_
	long64 ret = 0;
	CAutoPtr<CResultSet> rs= this->executeQuery("select @@IDENTITY");
	if( rs->next() )
	{
		if( rs->isNull( 1 ) )
		{
			ThrowSQLException<CSQLException>( 0 , 
					"[%s][%s]", 
					"COdbcStatement::getLastInsertID():" ,
					"is not insert anyrow befor!" );
		}
		ret = rs->getLong64( 1 );
	}
	else
	{
		ThrowSQLException<CSQLException>( 0 , 
					"[%s][%s]", 
					"COdbcStatement::getLastInsertID():" ,
					"sql error" );
	}
#endif
	return ret;
}

long64 COdbcStatement::getInsertID(void* pParam)
{
#ifdef _MSSQL_SERVER_
	return getLastInsertID();
#endif
}

int COdbcStatement::executeUpdate(const char* pSQL)
{
	VALIDSTMT;
	this->prepare( pSQL );
	return this->executeUpdate();
}

bool COdbcStatement::execute(const char* pSQL)
{
	COdbcStatement::executeUpdate(pSQL);
	return true;
}

CAutoPtr<CResultSet> COdbcStatement::executeQuery(const char* pSQL)
{
	VALIDSTMT;
	this->prepare( pSQL );
	return executeQuery();
}

void COdbcStatement::prepare(const char* pSQL)
{
	VALIDSTMT;
	if( m_pUsedResultSet != NULL )
	{
		m_pUsedResultSet->close();
		m_pUsedResultSet = NULL;
	}
	//删除绑定参数
	clearParameters();
	SQLRETURN nRet = SQLPrepare( m_pStmt , (SQLCHAR *) pSQL , SQL_NTS );
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "SQLPrepare error" );
	SQLSMALLINT nCount = 0;
	nRet = SQLNumParams( m_pStmt ,  &nCount );
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "SQLNumParams error" );
	m_nParamCount = nCount;
}

bool COdbcStatement::execute()
{
	VALIDSTMT;
	SQLRETURN nRet = SQLExecute(m_pStmt);
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "execute error" );
	return true;
}

int COdbcStatement::executeUpdate()
{
	VALIDSTMT;
	SQLRETURN nRet = SQLExecute(m_pStmt);
	if( SQL_NEED_DATA == nRet)
	{
		writeBinaryData();
		//SQLFreeStmt( m_pStmt, SQL_RESET_PARAMS);
		//SQLFreeStmt( m_pStmt, SQL_CLOSE);
		nRet = SQL_SUCCESS;
	}
	CHECK_STMT_AND_THROW( nRet ,  m_pStmt, "executeUpdate error" );
	SQLINTEGER nCount ;
	nRet = SQLRowCount( m_pStmt , &nCount );
	CHECK_STMT_AND_THROW( nRet ,  m_pStmt, "executeUpdate getrowcount error");
	return nCount;
}

CAutoPtr<CResultSet> COdbcStatement::executeQuery()
{
	VALIDSTMT;
	SQLRETURN nRet = SQLExecute( m_pStmt );
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "prepare error" );
	CAutoPtr<COdbcResultSet> rs(new COdbcResultSet(*this));
	rs->buildResultSet();
	this->m_pUsedResultSet = rs.operator->();
	CAutoPtr<CResultSet> rsret(rs.release());
	return rsret;
}

//*************set params*************************
void COdbcStatement::setBoolean(int nParamIndex,bool Value)
{
	setByte(nParamIndex,Value?1:0);
}

void COdbcStatement::setByte(int nParamIndex,char Value)
{
	CHECK_PARAMS;
	setInt( nParamIndex ,  Value );
}

void COdbcStatement::setBytes(int nParamIndex,const unsigned char* Value,int nLength)
{
	VALIDSTMT;
	CHECK_PARAMS;
	pOdbcBind->nType = SQL_C_BINARY;
	pOdbcBind->setData( Value , nLength );
	pOdbcBind->nPcbValues = SQL_NTS;
	SQLRETURN nRet = SQLBindParameter( this->m_pStmt , nParamIndex, SQL_PARAM_INPUT,  
		SQL_C_CHAR,   SQL_LONGVARCHAR, pOdbcBind->getSize() , 0, pOdbcBind->getData() , 0 ,
		&pOdbcBind->nPcbValues );   
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "COdbcStatement::setBytes" );
	pOdbcBind->nPcbValues =  SQL_LEN_DATA_AT_EXEC(0);
}

void COdbcStatement::setDate(int nParamIndex,CDateTime Value)
{
	setTimestamp( nParamIndex , Value );
}

void COdbcStatement::setTime(int nParamIndex,CDateTime Value)
{
	setTimestamp( nParamIndex , Value );
}

void COdbcStatement::setTimestamp(int nParamIndex,CDateTime Value)
{
	VALIDSTMT;
	CHECK_PARAMS;
	TIMESTAMP_STRUCT DateTime;
	convert( DateTime , Value );
	pOdbcBind->nType = SQL_C_TYPE_TIMESTAMP;
	pOdbcBind->setData( &DateTime , sizeof( TIMESTAMP_STRUCT ));
	SQLRETURN nRet = SQLBindParameter( m_pStmt ,(SQLUSMALLINT)nParamIndex , 
		SQL_PARAM_INPUT , SQL_C_TYPE_TIMESTAMP , SQL_TYPE_TIMESTAMP , 23 , 0 ,
		pOdbcBind->getData() , 0 , 0 );
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "setTimestamp Error" );
}

void COdbcStatement::setDouble(int nParamIndex,double Value)
{
	VALIDSTMT;
	CHECK_PARAMS;
	pOdbcBind->nType = SQL_C_DOUBLE;
	pOdbcBind->setData( &Value , sizeof( double ));
	SQLRETURN nRet = SQLBindParameter( m_pStmt ,(SQLUSMALLINT)nParamIndex , 
		SQL_PARAM_INPUT , SQL_C_DOUBLE , SQL_C_DOUBLE , 0 , 0 ,
		pOdbcBind->getData() , 0 , 0 );
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "setDouble Error" );
}

void COdbcStatement::setInt(int nParamIndex,int Value)
{
	VALIDSTMT;
	CHECK_PARAMS;
	pOdbcBind->nType = SQL_C_LONG;
	pOdbcBind->setData( &Value , sizeof( int ));
	SQLRETURN nRet = SQLBindParameter( m_pStmt ,(SQLUSMALLINT)nParamIndex , 
		SQL_PARAM_INPUT , SQL_C_LONG , SQL_INTEGER , 0 , 0 ,
		pOdbcBind->getData() , 0 , 0 );
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "setInt Error" );
}

void COdbcStatement::setLong64(int nParamIndex,long64 Value)
{
	VALIDSTMT;
	CHECK_PARAMS;
	pOdbcBind->nType = SQL_C_SBIGINT;
	pOdbcBind->setData( &Value , sizeof( long64 ));
	SQLRETURN nRet = SQLBindParameter( m_pStmt ,(SQLUSMALLINT)nParamIndex , 
		SQL_PARAM_INPUT , SQL_C_SBIGINT , SQL_BIGINT , 0 , 0 ,
		pOdbcBind->getData() , 0 , 0 );
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "setLong64 Error" );
}

void COdbcStatement::setNull(int nParamIndex,EDATATYPE nEDATATYPE)
{
	VALIDSTMT;
	CHECK_PARAMS;
	int nCType = getOdbcCType( nEDATATYPE );
	int nSQLType = getOdbcSQLType( nEDATATYPE );
	int nColDef = (INT)getColDef( nEDATATYPE );
	if( -1 == nCType )
	{
		throw CSQLException( 0 , "COdbcStatement::setNull Unkown Type");
	}
	if( -1 == nSQLType )
	{
		throw CSQLException( 0 , "COdbcStatement::setNull Unkown Type");
	}
	pOdbcBind->nPcbValues = SQL_NULL_DATA;
	//pOdbcBind->setData( &nNullPoint , sizeof(nNullPoint) );
	SQLRETURN nRet = SQLBindParameter( m_pStmt ,  nParamIndex , 
			SQL_PARAM_INPUT, 
			nCType , nSQLType, 
			nColDef, 0, 0, 
			0, &pOdbcBind->nPcbValues
			);
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "setNull Error" );
}

void COdbcStatement::setString(int nParamIndex,const string& Value)
{
	VALIDSTMT;
	CHECK_PARAMS;
	pOdbcBind->nType = SQL_C_CHAR;
	pOdbcBind->setString( Value.c_str() , Value.length() );
	SQLRETURN nRet = SQLBindParameter( m_pStmt ,(SQLUSMALLINT)nParamIndex , 
		SQL_PARAM_INPUT , SQL_C_CHAR , SQL_VARCHAR , pOdbcBind->getSize() , 0 ,
		pOdbcBind->getString() , 0 , 0 );
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "setString Error" );
}

//********************other funcs****************
int COdbcStatement::getErrNo()
{
	return 0;
}

const char* COdbcStatement::getError()
{
	return NULL;
}


void COdbcStatement::validIndex(int nIndex)
{
	if ( nIndex <= 0 || nIndex > m_nParamCount )
		throw CSQLException(ERROR_DATABASE_INDEXOUT,"Index Out Of Bounds");
}

void COdbcStatement::clearParameters()
{
	map<int,odbc_bind *>::iterator itMap;	
	for( itMap = m_mapParams.begin() ; itMap != m_mapParams.end() ; itMap++ )
	{
		delete itMap->second;
	}
	m_mapParams.clear();
}

HSTMT COdbcStatement::getStmt() const 
{ 
	VALIDSTMT;
	return m_pStmt; 
}
void COdbcStatement::clearResultSet()
{
	m_pUsedResultSet = NULL;
}
void COdbcStatement::writeBinaryData()
{
	VALIDSTMT;
	map<int,odbc_bind *>::iterator itMap;
	SQLPOINTER	pToken = (SQLPOINTER)-1;
	SQLRETURN nRetCode = - 1;
	for( itMap = m_mapParams.begin() ; itMap != m_mapParams.end() ; itMap++ )
	{		
		int nParamIndex = itMap->first;
		odbc_bind * bind = itMap->second;
		if( SQL_C_BINARY == bind->nType )
		{
			nRetCode = SQLParamData( m_pStmt , &pToken );
			if( nRetCode == SQL_NEED_DATA )
			{
				nRetCode = SQLPutData( m_pStmt , bind->getData() , bind->getSize() );
				CHECK_STMT_AND_THROW( nRetCode , m_pStmt , "writeBinaryData Error" );
			}
		}
	}
	nRetCode = SQLParamData( m_pStmt , &pToken );
	CHECK_STMT_AND_THROW( nRetCode , m_pStmt , "writeBinaryData Error" );
}
