#include "odbcpublic.h"
#include "odbcresultset.h"
#include "odbcstatement.h"
#include "odbccallablestatement.h"

#define FINDCOLUMNBYNAME long nColumn;	\
	try		\
	{	\
		nColumn = findColumn(pColumn);	\
	}	\
	catch(CSQLException&)	\
	{	\
		if(!bNullThrow)\
			return Default;	\
		throw;\
	}

#define VALIDINDEXODBC  map<int,odbc_bind *>::const_iterator itMap;	\
	itMap = m_mapResultData.find( nIndex );	\
	if( itMap == m_mapResultData.end() ) \
	{	\
		ThrowSQLException<CSQLException>( ERROR_DATABASE_INDEXOUT , "column %d is out of index",nIndex);	\
	}	\
	odbc_bind * bind = itMap->second;


#define VALIDINDEANDCHECKNULLODBC  map<int,odbc_bind *>::const_iterator itMap;	\
	itMap = m_mapResultData.find( nIndex );	\
	if( itMap == m_mapResultData.end() ) \
	{	\
		if( bNullThrow ) \
		{\
			ThrowSQLException<CSQLException>( 0 , "column(%d) is out of index" , nIndex );\
		}\
		else\
		{\
			return Default;\
		}\
	}\
	odbc_bind * bind = itMap->second; \
	if( bind->nLength == SQL_NULL_DATA ) \
	{ \
		if( bNullThrow ) \
		{\
			ThrowSQLException<CSQLException>( 0 , "column(%d) is null" , nIndex );\
		}\
		else\
		{\
			return Default;\
		}\
	}
#define _INVIDLE_RST_STMT_(PARAM) \
	if( NULL == m_Stmt )\
	{\
		ThrowSQLException<CSQLException>( 0 , PARAM" Stmt is null");\
	}

using namespace my_odbc;
COdbcResultSet::COdbcResultSet(COdbcStatement& stmt)
	:m_pStmt(NULL),m_Stmt(NULL),m_pCallStmt(NULL)
{
	m_pStmt = &stmt;
	m_Stmt = stmt.getStmt();;
}
COdbcResultSet::COdbcResultSet(COdbcCallableStatement& stmt)
	:m_pStmt(NULL),m_Stmt(NULL),m_pCallStmt(NULL)
{
	m_pCallStmt = &stmt;
	m_Stmt = stmt.getStmt();;
}
COdbcResultSet::~COdbcResultSet()
{
	try
	{
		close();
	}
	catch(...)
	{

	}
}


void COdbcResultSet::validIndex(int nIndex)
{
	VALIDINDEXODBC;
}

void COdbcResultSet::close()
{
	clear();
}

bool COdbcResultSet::init()
{
	return false;
}

int COdbcResultSet::getColumnCount() const
{
	return m_Fields.size();
}

string COdbcResultSet::getColumnName(int nIndex) const
{
	map<string,int>::const_iterator iter;
	for( iter = m_Fields.begin() ; iter != m_Fields.end() ; iter++ )
	{
		if( iter->second == nIndex )
		{
			return iter->first;
		}
	}
	ThrowSQLException<CSQLException>( 0  , "getColumnName index(%d) is out" , nIndex );
	return "";
}

bool COdbcResultSet::next()
{
	_INVIDLE_RST_STMT_("COdbcResultSet::next");
	int nRet = SQLFetch( m_Stmt );
	bool bRet = true;
	switch( nRet )
	{
		//case SQL_SUCCESS:
		//case SQL_SUCCESS_WITH_INFO:
		//case SQL_STILL_EXECUTING:
		case SQL_NO_DATA:
			bRet = false;
			break;
		case SQL_ERROR:
		case SQL_INVALID_HANDLE:
			getErrorMsgAndThrow( SQL_HANDLE_STMT , m_Stmt , "COdbcResultSet::next");
			break;
			
	}
	return bRet;
}

int COdbcResultSet::findColumn(const char* pColumnName) const
{
	char* p = _strdup(pColumnName);
	{
		int len = strlen(p);
		for ( int i=0 ; i<len ; i++ )
			p[i] = toupper(p[i]);
	}
	map<string,int>::const_iterator i = this->m_Fields.find(p);
	free(p);
	if ( i == m_Fields.end() )
		ThrowSQLException<CSQLException>( 0  , "don't have the %s",pColumnName);
	return i->second;
}

char COdbcResultSet::getByte(int nIndex,bool bNullThrow ,char Default ) const
{
//	map<int,odbc_bind *>::iterator itMap;
//	if( itMap = m_mapResultData.find( nIndex ) )
//	{
//		ThrowSQLException<CSQLException>( ERROR_DATABASE_INDEXOUT , "column %d is out of index",nIndex);
//	}
//	odbc_bind * bind = itMap->second;
	return 0;
}

char COdbcResultSet::getByte(const char* pColumnName,bool bNullThrow,char Default) const
{
	return 0;
}
int COdbcResultSet::getBytes(int nIndex,unsigned char* pOutBuffer,
								 int nBuffSize,int nCharCount,bool bNullThrow,
								 const char* Default) const
{
	int nSize=0;
	try
	{
		_INVIDLE_RST_STMT_("COdbcResultSet::getBytes");
		VALIDINDEXODBC;
		switch( bind->nType )
		{
			case SQL_LONGVARCHAR:
			case SQL_BINARY:
				break;
			default:
				ThrowSQLException<CSQLException>( 0 , "COdbcResultSet::getBytes column %d is not binary value" , nIndex);
		}
		SQLRETURN nRet=SQL_SUCCESS;
		SQLINTEGER nDataSize = 0;
		//得到数据长度
		PBYTE pBuf = pOutBuffer;
		
		nRet = SQLGetData( m_Stmt ,  nIndex , SQL_C_BINARY, pBuf , 0 , &nDataSize );
		CHECK_STMT_AND_THROW( nRet , m_Stmt , "COdbcResultSet::getBytes");
		//判断数据长度
		if( nBuffSize < nDataSize )
		{
			ThrowSQLException<CSQLException>( 0 , "COdbcResultSet::getBytes:column %d data over buff " , nIndex );
		}
		//获取完整数据
 		SQLINTEGER nRetLen = 0;
		nRet = SQLGetData( m_Stmt, nIndex, SQL_C_BINARY , pBuf, nDataSize , &nRetLen );
		CHECK_STMT_AND_THROW( nRet , m_Stmt , "COdbcResultSet::getBytes");
		if( nRetLen != nDataSize )
		{
			ThrowSQLException<CSQLException>( 0 , "COdbcResultSet::getBytes:column %d not get all data" , nIndex);
		}
		nSize = nDataSize;
	}
	catch(CSQLException&)
	{
		if( bNullThrow )
		{
			throw;
		}
		else
		{
			int size = strlen(Default);
			size = _MIN( size , nBuffSize );
			size = _MAX( size , 0 );
			memcpy(pOutBuffer,Default,size);
			nSize = size;
		}
	}
	return nSize;
}


/**
*
*/
int COdbcResultSet::getBytes(const char* pColumnName,unsigned char* pOutBuffer,
									  int nBuffSize,int nCharCount,bool bNullThrow ,
									  const char* pDefault) const
{
	int nColumn;
	int nSize = 0;
	try
	{
		nColumn = findColumn(pColumnName);
	}
	catch(CSQLException&)
	{
		if( bNullThrow )
		{
			throw;
		}
		else
		{
			int size = strlen(pDefault);
			size = _MIN( size , nBuffSize );
			size = _MAX( size , 0 );
			memcpy(pOutBuffer,pDefault,size);
			nSize = size;
			return nSize;
		}
	}
	return getBytes( nColumn , pOutBuffer,nBuffSize,
						nCharCount,bNullThrow,pDefault);
}

bool COdbcResultSet::getBoolean(int nIndex) const
{
	return false;
}

bool COdbcResultSet::getBoolean(const char* pColumn) const
{
	return false;
}

string COdbcResultSet::getString(int nIndex,bool bNullThrow ,const char* Default ) const
{
	try
	{
		VALIDINDEANDCHECKNULLODBC;
		if( ( SQL_CHAR != bind->nType ) &&
			( SQL_VARCHAR != bind->nType ) )
			ThrowSQLException<CSQLException>( 0 , "column %d is not string value" , nIndex);
		string str(bind->getString());
		//bind->reset();
		return str;
	}
	catch(CSQLException&)
	{
		if( bNullThrow )
			throw;
	}
	return Default;
}

string COdbcResultSet::getString(const char* pColumn,bool bNullThrow ,const char* Default ) const
{
	try
	{
		FINDCOLUMNBYNAME;
		return getString( nColumn , bNullThrow , Default );
	}
	catch(CSQLException&)
	{
		if( bNullThrow )
			throw;
	}
	return Default;
}

int COdbcResultSet::getInt(int nIndex,bool bNullThrow ,int Default ) const
{
	int nRet = Default;
	try
	{
		VALIDINDEANDCHECKNULLODBC;
		if( bind->nLength == SQL_NULL_DATA )
		{
			if( bNullThrow )
			{
				ThrowSQLException<CSQLException>( 0 , "COdbcResultSet::getInt:column(%d) is null" , nIndex );
			}
			else
			{
				return Default;
			}
		}
		switch( bind->nType )
		{
		case SQL_INTEGER:
			nRet = *(long *)bind->getData(); 
			break;
		case SQL_REAL:
		case SQL_DOUBLE:
		case SQL_DECIMAL:
			nRet = *(double*)bind->getData();
			break;
		case SQL_BIGINT:
		case SQL_NUMERIC:
			nRet = *(long64*)bind->getData();
			break;
		case SQL_CHAR:
			nRet = *(char*)bind->getData();
			break;
		case SQL_TINYINT:
		case SQL_SMALLINT:
			nRet = *(short*)bind->getData();
			break;
		default:
			ThrowSQLException<CSQLException>( 0 , "column %d is not num value" ,nIndex);
			break;
		}
		return *((int*)bind->getData());
	}
	catch(CSQLException&)
	{
		if( bNullThrow )
			throw;
	}
	return nRet;
}

int COdbcResultSet::getInt(const char* pColumn,bool bNullThrow ,int Default ) const
{
	FINDCOLUMNBYNAME;
	return getInt( nColumn , bNullThrow , Default );
}

double COdbcResultSet::getDouble(int nIndex,bool bNullThrow ,double Default ) const
{
	double drRet = Default;
	try
	{
		VALIDINDEANDCHECKNULLODBC;
		switch(  bind->nType )
		{
		case SQL_DOUBLE:
		case SQL_DECIMAL:
		case SQL_REAL:
		case SQL_FLOAT:
			drRet = *((double*)bind->getData());
			break;
		default:
			drRet = getInt( nIndex , bNullThrow , Default );
		}
	}
	catch(CSQLException&)
	{
		if( bNullThrow )
			throw;
	}
	return drRet;
}
double COdbcResultSet::getDouble(const char* pColumn,bool bNullThrow ,double Default ) const
{
	try
	{
		FINDCOLUMNBYNAME;
		return getDouble( nColumn , bNullThrow , Default );
	}
	catch(CSQLException&)
	{
		if( bNullThrow )
			throw;
	}
	return Default;
}

bool COdbcResultSet::isNull(int nIndex) const
{
	VALIDINDEXODBC;
	return bind->nLength == SQL_NULL_DATA;
}

bool COdbcResultSet::isNull(const char* pColumn) const
{
	bool bNullThrow = true;
	bool Default = false;
	FINDCOLUMNBYNAME;
	return isNull( nColumn );
}

CDateTime COdbcResultSet::getDate(int nIndex,bool bNullThrow ,const CDateTime& Default ) const
{
	return getTimestamp( nIndex , bNullThrow , Default);
}

CDateTime COdbcResultSet::getDate(const char* pColumn,bool bNullThrow ,const CDateTime& Default ) const
{
	FINDCOLUMNBYNAME;
	return getDate( nColumn , bNullThrow , Default );
}
CDateTime COdbcResultSet::getTime(int nIndex,bool bNullThrow ,const CDateTime& Default ) const
{
	try
	{
		VALIDINDEANDCHECKNULLODBC;
		return getTimestamp( nIndex , bNullThrow , Default);
	}
	catch(CSQLException&)
	{
		if( bNullThrow )
			throw;
	}
	return Default;
}
CDateTime COdbcResultSet::getTime(const char* pColumn,bool bNullThrow ,const CDateTime& Default ) const
{
	FINDCOLUMNBYNAME;
	return getTime( nColumn , bNullThrow , Default );
}
CDateTime COdbcResultSet::getTimestamp(int nIndex,bool bNullThrow ,const CDateTime& Default ) const
{
	try
	{
		VALIDINDEANDCHECKNULLODBC;
		if( SQL_TYPE_TIMESTAMP != bind->nType )
			ThrowSQLException<CSQLException>( 0 , "column %d is not timestamp value" , nIndex);
		CDateTime datetime;
		switch( bind->nType )
		{
		case SQL_TYPE_TIMESTAMP:
			convert( datetime , *((TIMESTAMP_STRUCT* )bind->getData() ) );
			break;
		case SQL_TYPE_TIME:
			convert( datetime , *((TIME_STRUCT* )bind->getData() ) );
			break;
		case SQL_TYPE_DATE:
			convert( datetime , *((DATE_STRUCT* )bind->getData() ) );
			break;
		default:
			ThrowSQLException<CSQLException>( 0 , "column %d is not timestamp value" , nIndex);
			break;
		}
		return datetime;
	}
	catch(CSQLException&)
	{
		if( bNullThrow )
			throw;
	}
	return Default;
}
CDateTime COdbcResultSet::getTimestamp(const char* pColumn,bool bNullThrow ,const CDateTime& Default ) const
{
	FINDCOLUMNBYNAME;
	return getTimestamp( nColumn , bNullThrow , Default );
}
long64 COdbcResultSet::getLong64(int nIndex,bool bNullThrow ,long64 Default ) const
{
	long64 l64Ret = Default;
	try
	{
		VALIDINDEANDCHECKNULLODBC;
		switch(  bind->nType )
		{
		case SQL_BIGINT:
		case SQL_NUMERIC:
			l64Ret = *((long64*)bind->getData());
			break;
		default:
			l64Ret = getInt( nIndex , bNullThrow , Default );
		}
	}
	catch(CSQLException&)
	{
		if( bNullThrow )
			throw;
	}
	return l64Ret;
}
long64 COdbcResultSet::getLong64(const char* pColumn,bool bNullThrow ,long64 Default ) const
{
	FINDCOLUMNBYNAME;
	return getLong64( nColumn , bNullThrow , Default );
}
void COdbcResultSet::clear()
{
	if( m_pStmt == NULL )
	{
		return;
	}
	map<int,odbc_bind *>::iterator itMap ;
	for( itMap =  m_mapResultData.begin() ; itMap != m_mapResultData.end() ; )
	{
		delete itMap->second;
		m_mapResultData.erase( itMap++ );
	}
	m_mapResultData.clear();
	m_Fields.clear();
	if( NULL != m_pStmt )
	{
		m_pStmt->clearResultSet();
		SQLRETURN nRet = SQLFreeStmt( m_Stmt , SQL_RESET_PARAMS );
		CHECK_STMT_AND_THROW(nRet , m_Stmt , "COdbcResultSet::clear:SQLFreeStmt Error" );
		nRet=SQLCloseCursor( m_Stmt );
		CHECK_STMT_AND_THROW(nRet , m_Stmt , "COdbcResultSet::clear:SQLCloseCursor Error" );
		m_pStmt = NULL;
		m_Stmt = NULL;
	}
	if( NULL != m_pCallStmt )
	{
		m_pCallStmt->clearResultSet();
		SQLRETURN nRet = SQLFreeStmt( m_Stmt , SQL_RESET_PARAMS );
		CHECK_STMT_AND_THROW(nRet , m_Stmt , "COdbcResultSet::clear:SQLFreeStmt Error" );
		nRet=SQLCloseCursor( m_Stmt );
		CHECK_STMT_AND_THROW(nRet , m_Stmt , "COdbcResultSet::clear:SQLCloseCursor Error" );
		m_pCallStmt = NULL;
		m_Stmt = NULL;
	}
}
EDATATYPE COdbcResultSet::getColumnDataType(int nIndex)
{
	VALIDINDEXODBC;
	EDATATYPE type = getAvoDataTypeByOdbcType(bind->nType);
	if( -1 == type )
	{
		ThrowSQLException<CSQLException>( ERROR_DATABASE_INDEXOUT , "COdbcResultSet::getColumnDataType:unkown type:nIndex",nIndex);
	}
	return type;
}

/**
* 生成结果集
*/
void COdbcResultSet::buildResultSet(void)
{
	if( NULL == m_Stmt )
	{
		ThrowSQLException<CSQLException>( 0 , "COdbcResultSet::buildResultSet stmt is null" );
	}
	SQLSMALLINT nRet;
	SQLSMALLINT nColsCount = 0;
	nRet = SQLNumResultCols( this->m_Stmt , &nColsCount );
	CHECK_STMT_AND_THROW( nRet , m_Stmt , "executeQuery" );
	SQLCHAR szColumn[256] = "";
	SQLSMALLINT nActualLen;
	SODBCFieldInfo odbcFieldInfo;
	CAutoPtr<odbc_bind> odbcBind = NULL;
	//CAutoPtr<COdbcResultSet> rs(new COdbcResultSet(*this));
	for( SQLSMALLINT i = 1 ; i <= nColsCount ; i ++ )
	{
		nRet = SQLDescribeCol( m_Stmt, i,
			odbcFieldInfo.m_szName, sizeof( odbcFieldInfo.m_szName ), 
			&nActualLen,
			&odbcFieldInfo.m_nSQLType,
			&odbcFieldInfo.m_nPrecision,
			&odbcFieldInfo.m_nScale,
			&odbcFieldInfo.m_nNullability);
		CHECK_STMT_AND_THROW( nRet , m_Stmt , "executeQuery SQLDescribeCol" );
		odbcBind = new odbc_bind();
		odbcBind->nType = odbcFieldInfo.m_nSQLType;
		odbcBind->nScale = odbcFieldInfo.m_nScale;
		switch( odbcBind->nType )
		{
		case SQL_CHAR:
			odbcBind->setSize( odbcFieldInfo.m_nPrecision );
			nRet = SQLBindCol( m_Stmt , i , SQL_C_CHAR , odbcBind->getData() , odbcBind->getSize() , &odbcBind->nLength );
			CHECK_STMT_AND_THROW( nRet , m_Stmt , "SQLBindCol Error");
			break;
		case SQL_DOUBLE:
		case SQL_REAL:
		case SQL_FLOAT:
		case SQL_DECIMAL:
			odbcBind->setSize( odbcFieldInfo.m_nPrecision );
			nRet = SQLBindCol( m_Stmt , i , SQL_C_DOUBLE , odbcBind->getData() , odbcBind->getSize() , &odbcBind->nLength );
			CHECK_STMT_AND_THROW( nRet , m_Stmt , "SQLBindCol Error");
			break;
		case SQL_INTEGER:
			nRet = SQLBindCol( m_Stmt , i , SQL_C_LONG , odbcBind->getData() , 0 , &odbcBind->nLength );
			CHECK_STMT_AND_THROW( nRet , m_Stmt , "SQLBindCol Error");
			break;
		case SQL_TYPE_DATE:
		case SQL_TYPE_TIME:
		case SQL_TYPE_TIMESTAMP:
			nRet = SQLBindCol( m_Stmt , i , SQL_C_TYPE_TIMESTAMP , odbcBind->getData() , 0 , &odbcBind->nLength );
			CHECK_STMT_AND_THROW( nRet , m_Stmt , "SQLBindCol Error");
			break;
		case SQL_VARCHAR:
			odbcBind->setSize( odbcFieldInfo.m_nPrecision );
			nRet = SQLBindCol( m_Stmt , i , SQL_C_CHAR , odbcBind->getData() , odbcBind->getSize() , &odbcBind->nLength );
			CHECK_STMT_AND_THROW( nRet , m_Stmt , "SQLBindCol Error");
			break;
		case SQL_TINYINT:
		case SQL_SMALLINT:
			nRet = SQLBindCol( m_Stmt , i , SQL_C_TINYINT , odbcBind->getData() , odbcBind->getSize() , &odbcBind->nLength );
			CHECK_STMT_AND_THROW( nRet , m_Stmt , "SQLBindCol Error");
			break;
		case SQL_LONGVARCHAR:
		case SQL_BINARY:
			//odbcBind->setSize( 0 );
			//nRet = SQLBindCol( m_Stmt , i , SQL_C_CHAR , NULL , 0 , &odbcBind->nLength );
			//CHECK_STMT_AND_THROW( nRet , m_Stmt , "SQLBindCol Error");
			break;
		case SQL_BIGINT:
		case SQL_NUMERIC:
			nRet = SQLBindCol( m_Stmt , i , SQL_C_SBIGINT , odbcBind->getData() , odbcBind->getSize() , &odbcBind->nLength );
			CHECK_STMT_AND_THROW( nRet , m_Stmt , "SQLBindCol Error");
			break;
		default:
			throw CSQLException( 0 , "executeQuery Not Support DataType");
			break;
		}
		m_mapResultData[i] = odbcBind.release();
		for( int j = 0 ; j < strlen( (char*)odbcFieldInfo.m_szName ) ; j++ )
		{
			odbcFieldInfo.m_szName[j] = toupper( odbcFieldInfo.m_szName[j] );
		}
		m_Fields[string((char *)odbcFieldInfo.m_szName)] =  i ;
	}
	CHECK_STMT_AND_THROW( nRet , m_Stmt , "executeQuery" );
}
