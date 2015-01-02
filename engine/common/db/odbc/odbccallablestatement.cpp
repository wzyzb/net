#include "odbccallablestatement.h"
#include "odbcconnection.h"
#include "odbcresultset.h"
#include "odbcstatement.h"

#define FIND_OUTPUT_PARAMS(OUTSTRING)	\
	validIndex( nIndex ); \
	odbc_bind * pOdbcBind = NULL;\
	map<int,odbc_bind *>::iterator itMap;	\
	itMap = m_mapParams.find( nIndex ); \
	if( itMap == m_mapParams.end() )\
	{\
		ThrowSQLException<CSQLException>( 0 , "Can't find index %d" , nIndex);\
	}\
	else{\
		pOdbcBind = itMap->second;\
	}\
	if( pOdbcBind->nInOut != SQL_PARAM_OUTPUT\
		&& pOdbcBind->nInOut != SQL_PARAM_INPUT_OUTPUT )\
	{\
		ThrowSQLException<CSQLException>( 0 , \
				OUTSTRING" %d is not output value!\n" ,\
				nIndex );\
	}

using namespace my_odbc;
COdbcCallableStatement
	::COdbcCallableStatement(COdbcConnection& Conn)
	:m_pStmt(NULL),m_nParamCount(0),m_pUsedResultSet(NULL),m_nResultsetCount(0)
{
	//分配STMT
	SQLRETURN nRet = SQLAllocHandle(SQL_HANDLE_STMT, Conn.m_pConn, &m_pStmt );
	CHECK_CONN_AND_THROW( nRet , Conn.m_pConn , "create statement error" );
}
COdbcCallableStatement::~COdbcCallableStatement()
{
	try
	{
		close();
	}
	catch(...)
	{
	}
};
bool COdbcCallableStatement::registerResultset(int nIndex)
{
	VALIDSTMT;
	return true;
}
bool COdbcCallableStatement::registerReturn(EDATATYPE nEDATATYPE)
{
	VALIDSTMT;
	return true;
}
bool COdbcCallableStatement::execute()
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
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "COdbcCallableStatement::execute" );
	return true;	
}

void COdbcCallableStatement::close()
{
	VALIDSTMT;
	clearParameters();
	SQLRETURN nRet = ::SQLFreeStmt(m_pStmt, SQL_DROP);
	CHECK_CONN_AND_THROW( nRet , m_pStmt , "close statement error" );
}

void COdbcCallableStatement::setBytes(int nParamIndex, const char* Value, int nLength, bool isRegOut/* =false */)
{
	VALIDSTMT;
	CHECK_PARAMS;
	pOdbcBind->nType = SQL_C_BINARY;
	if( isRegOut )
	{
		pOdbcBind->nInOut = SQL_PARAM_OUTPUT;
		pOdbcBind->setSize( SQL_DESC_LENGTH );
	}
	else
	{
		pOdbcBind->nInOut = SQL_PARAM_INPUT;
		pOdbcBind->setString( Value , nLength );
	}
	pOdbcBind->setData( Value , nLength );
	pOdbcBind->nPcbValues = SQL_NTS;
	SQLRETURN nRet = SQLBindParameter( this->m_pStmt , nParamIndex, pOdbcBind->nInOut,  
		SQL_C_CHAR,   SQL_LONGVARCHAR, pOdbcBind->getSize() , 0, pOdbcBind->getData() , 0 ,
		&pOdbcBind->nPcbValues );   
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "COdbcStatement::setBytes" );
	pOdbcBind->nPcbValues =  SQL_LEN_DATA_AT_EXEC(0);
}

void COdbcCallableStatement::setByte(int nParamIndex, char Value, bool isRegOut/* =false */)
{
	setInt( nParamIndex , Value , isRegOut );
}

void COdbcCallableStatement::setString(int nParamIndex, const std::string& Value, bool isRegOut/* =false */)
{
	VALIDSTMT;
	CHECK_PARAMS;
	pOdbcBind->nType = TYPE_STRING;
	if( isRegOut )
	{
		pOdbcBind->nInOut = SQL_PARAM_OUTPUT;
		pOdbcBind->setSize( SQL_DESC_LENGTH );
	}
	else
	{
		pOdbcBind->nInOut = SQL_PARAM_INPUT;
		pOdbcBind->setString( Value.c_str() , Value.length() );
	}
	pOdbcBind->nPcbValues = 0;
	SQLRETURN nRet = SQLBindParameter( m_pStmt ,(SQLUSMALLINT)nParamIndex , 
			pOdbcBind->nInOut , SQL_C_CHAR , SQL_VARCHAR , SQL_DESC_LENGTH , 0 ,
			pOdbcBind->getString() , pOdbcBind->getSize() , &pOdbcBind->nPcbValues 
			);
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "COdbcCallableStatement::setString" );
}
		
void COdbcCallableStatement::setDate(int nParamIndex, CDateTime Value, bool isRegOut/* =false */)
{
	setTimestamp( nParamIndex , Value , isRegOut );
}

void COdbcCallableStatement::setDouble(int nParamIndex, double Value, bool isRegOut/* =false */)
{
	VALIDSTMT;
	CHECK_PARAMS;
	pOdbcBind->nType = TYPE_DOUBLE;
	if( isRegOut )
	{
		pOdbcBind->nInOut = SQL_PARAM_OUTPUT;
	}
	else
	{
		pOdbcBind->nInOut = SQL_PARAM_INPUT;
	}
	pOdbcBind->setData( &Value , sizeof( Value ));
	SQLRETURN nRet = SQLBindParameter( m_pStmt ,(SQLUSMALLINT)nParamIndex , 
		pOdbcBind->nInOut , SQL_C_DOUBLE , SQL_DOUBLE , 0 , 0 ,
		pOdbcBind->getData() , 0 , 0 );
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "COdbcCallableStatement::setDouble Error" );
}

void COdbcCallableStatement::setInt(int nParamIndex, int Value, bool isRegOut/* =false */)
{
	VALIDSTMT;
	CHECK_PARAMS;
	pOdbcBind->nType = TYPE_INT;
	if( isRegOut )
	{
		pOdbcBind->nInOut = SQL_PARAM_OUTPUT;
	}
	else
	{
		pOdbcBind->nInOut = SQL_PARAM_INPUT;
	}
	pOdbcBind->setData( &Value , sizeof( Value ));
	SQLRETURN nRet = SQLBindParameter( m_pStmt ,(SQLUSMALLINT)nParamIndex , 
		pOdbcBind->nInOut , SQL_C_LONG , SQL_INTEGER , 0 , 0 ,
		pOdbcBind->getData() , 0 , 0 );
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "COdbcCallableStatement::setInt Error" );
}

void COdbcCallableStatement::setNull(int nParamIndex, EDATATYPE nEDATATYPE, bool isRegOut/* =false */)
{
	VALIDSTMT;
}

void COdbcCallableStatement::setTime(int nParamIndex, CDateTime Value, bool isRegOut/* =false */)
{
	setTimestamp( nParamIndex , Value , isRegOut );
}		

void COdbcCallableStatement::setTimestamp(int nParamIndex, CDateTime Value, bool isRegOut/* =false */)
{
	VALIDSTMT;
	CHECK_PARAMS;
	pOdbcBind->nType = TYPE_TIMESTAMP;
	if( isRegOut )
	{
		pOdbcBind->nInOut = SQL_PARAM_OUTPUT;
	}
	else
	{
		pOdbcBind->nInOut = SQL_PARAM_INPUT;
	}
	TIMESTAMP_STRUCT Date;
	convert( Date , Value );
	pOdbcBind->setData( &Date , sizeof( Date ) );
	SQLRETURN nRet = SQLBindParameter( m_pStmt ,(SQLUSMALLINT)nParamIndex , 
		pOdbcBind->nInOut , SQL_C_TIMESTAMP , SQL_TIMESTAMP , 23 , 0 ,
		pOdbcBind->getData() , 0 , 0 );
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "COdbcCallableStatement::setTimestamp" );
}
		
void COdbcCallableStatement::setLong64(int nParamIndex, Long64 Value, bool isRegOut/* =false */)
{
	VALIDSTMT;
	CHECK_PARAMS;
	pOdbcBind->nType = TYPE_LONG64;
	if( isRegOut )
	{
		pOdbcBind->nInOut = SQL_PARAM_OUTPUT;
	}
	else
	{
		pOdbcBind->nInOut = SQL_PARAM_INPUT;
	}
	pOdbcBind->setData( &Value , sizeof( Value ));
	SQLRETURN nRet = SQLBindParameter( m_pStmt ,(SQLUSMALLINT)nParamIndex , 
		pOdbcBind->nInOut , SQL_C_SBIGINT , SQL_BIGINT , 0 , 0 ,
		pOdbcBind->getData() , 0 , 0 );
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "COdbcCallableStatement::setLong64 Error" );
}

bool COdbcCallableStatement::hasMoreResultsets()
{
	VALIDSTMT;
	int nRet = SQLMoreResults( this->m_pStmt );
	if( SQL_NO_DATA == nRet )
	{
		return false;
	}
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "COdbcCallableStatement::getNextResultSet" );
	return true;
}
		
CAutoPtr<CResultSet> COdbcCallableStatement::getNextResultSet()
{
	VALIDSTMT;
	if(m_nResultsetCount != 0)
	{
		int nRet = SQLMoreResults( this->m_pStmt );
		CHECK_STMT_AND_THROW( nRet , m_pStmt , "COdbcCallableStatement::getNextResultSet" );
	}
	CAutoPtr<COdbcResultSet> rs( new COdbcResultSet(*this) );
	rs->buildResultSet();
	m_nResultsetCount ++;
	return rs.release();
}

char COdbcCallableStatement::getByte(int nIndex)
{
	return getInt( nIndex );
}
		
char COdbcCallableStatement::getByte(const char* pColumnName)
{
	VALIDSTMT;
	return 0;
}
		
bool COdbcCallableStatement::getBytes(int nIndex, char* pOutBuffer, int& nLength)
{
	VALIDSTMT;
	return false;
}
		
bool COdbcCallableStatement::getBytes(const char* pColumnName, char* pOutBuffer, int& nLength)
{
	VALIDSTMT;
	return false;
}

std::string COdbcCallableStatement::getString(int nIndex)
{
	VALIDSTMT;
	FIND_OUTPUT_PARAMS("COdbcCallableStatement::getString");
	switch( pOdbcBind->nType )
	{
	case TYPE_STRING:
		return string( pOdbcBind->getString() , pOdbcBind->nPcbValues );
	}
	return "";
}

std::string COdbcCallableStatement::getString(const char* pColumn)
{
	VALIDSTMT;
	return "";
}

int COdbcCallableStatement::getInt(int nIndex)
{
	VALIDSTMT;
	FIND_OUTPUT_PARAMS("COdbcCallableStatement::getInt");
	switch( pOdbcBind->nType )
	{
	case TYPE_INT:
		return *(int*)pOdbcBind->getData();
	case TYPE_LONG64:
		return *(long64*)pOdbcBind->getData();
	case TYPE_DOUBLE:
		return *(double*)pOdbcBind->getData();
	}
	return 0;
}

int COdbcCallableStatement::getInt(const char* pColumn)
{
	VALIDSTMT;
	return 0;
}
		
double COdbcCallableStatement::getDouble(int nIndex)
{
	VALIDSTMT;
	FIND_OUTPUT_PARAMS("COdbcCallableStatement::getDouble");
	switch( pOdbcBind->nType )
	{
	case TYPE_INT:
		return *(int*)pOdbcBind->getData();
	case TYPE_LONG64:
		return *(long64*)pOdbcBind->getData();
	case TYPE_DOUBLE:
		return *(double*)pOdbcBind->getData();
	}
	return 0;
}
		

double COdbcCallableStatement::getDouble(const char* pColumn)
{
	VALIDSTMT;
	return 0;
}
		
bool COdbcCallableStatement::isNull(int nIndex)
{
	VALIDSTMT;
	FIND_OUTPUT_PARAMS("COdbcCallableStatement::isNull");
	return pOdbcBind->nLength == SQL_NULL_DATA;
}
		
bool COdbcCallableStatement::isNull(const char* pColumn)
{
	VALIDSTMT;
	return false;
}
		
CDateTime COdbcCallableStatement::getDate(int nIndex)
{
	VALIDSTMT;
	FIND_OUTPUT_PARAMS("COdbcCallableStatement::getDate");
	return getTimestamp( nIndex );
}
		
CDateTime COdbcCallableStatement::getDate(const char* pParamName)
{
	VALIDSTMT;
	return CDateTime();
}
		
CDateTime COdbcCallableStatement::getTime(int nIndex)
{
	VALIDSTMT;
	return getTimestamp( nIndex );
}

CDateTime COdbcCallableStatement::getTime(const char* pParamName)
{
	VALIDSTMT;
	return CDateTime();
}
	
CDateTime COdbcCallableStatement::getTimestamp(int nIndex)
{
	VALIDSTMT;
	FIND_OUTPUT_PARAMS("COdbcCallableStatement::getTimestamp");
	CDateTime date;
	switch( pOdbcBind->nType )
	{
	case TYPE_DATE:
	case TYPE_TIME:
	case TYPE_TIMESTAMP:
		convert( date , *(TIMESTAMP_STRUCT*)pOdbcBind->getData() );
		break;
	}
	return date;
}		

CDateTime COdbcCallableStatement::getTimestamp(const char* pColumn)
{
	VALIDSTMT;
	return CDateTime();
}
		
Long64 COdbcCallableStatement::getLong64(int nIndex)
{
	VALIDSTMT;
	FIND_OUTPUT_PARAMS("COdbcCallableStatement::getLong64");
	switch( pOdbcBind->nType )
	{
	case TYPE_INT:
		return *(int*)pOdbcBind->getData();
	case TYPE_LONG64:
		return *(long64*)pOdbcBind->getData();
	case TYPE_DOUBLE:
		return *(double*)pOdbcBind->getData();
	}
	return 0;
}
		
Long64 COdbcCallableStatement::getLong64(const char* pParamName)
{
	VALIDSTMT;
	return 0;
}
		
void COdbcCallableStatement::prepare(const char* pSql)
{
	VALIDSTMT;
	//执行清除操作
	clearParameters();
	SQLRETURN nRet = SQLPrepare( m_pStmt , (SQLCHAR *) pSql , SQL_NTS );
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "SQLPrepare error" );
	SQLSMALLINT nCount = 0;
	nRet = SQLNumParams( m_pStmt ,  &nCount );
	CHECK_STMT_AND_THROW( nRet , m_pStmt , "SQLNumParams error" );
	m_nParamCount = nCount;
}

void COdbcCallableStatement::clearParameters()
{
	map<int,odbc_bind *>::iterator itMap;	
	for( itMap = m_mapParams.begin() ; itMap != m_mapParams.end() ; itMap++ )
	{
		delete itMap->second;
	}
	m_mapParams.clear();
	if( m_pUsedResultSet != NULL )
	{
		m_pUsedResultSet->close();
		m_pUsedResultSet = NULL;
	}
	m_nResultsetCount = 0;
}
void COdbcCallableStatement::validIndex(int nIndex)
{
	if ( nIndex <= 0 || nIndex > m_nParamCount )
		throw CSQLException(ERROR_DATABASE_INDEXOUT,"Index Out Of Bounds");
}
void COdbcCallableStatement::clearResultSet()
{
	m_pUsedResultSet = NULL;
}
void COdbcCallableStatement::writeBinaryData()
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
