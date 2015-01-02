#include "mysqlpublic.h"
#include "../../util/autoptr.h"
#include "connectionimpl.h"
#include "statementimpl.h"
#include "resultsetimpl.h"
#include "../dbpublic.h"
#include "../sqlexception.h"
#include "../../public.h"
#ifdef WIN32
#else
#include <ctype.h>
#define _strdup(a) strdup(a)
#endif

//最大的大对象大小－－最大允许128K
#define MAXBLOBSIZE 128*1024

using namespace mysql;

#define THROWEXCEPTION throw CSQLException(m_pStmt->getErrNo(),m_pStmt->getError());

#define EXECUTE(a) if ( 0 != a ) \
			THROWEXCEPTION;

//检查ROW是否有效
#define VALIDROW if ( m_Row == NULL ) \
		throw CSQLException(ERROR_DATABASE_INVALIDOPER,"Row Not Exist");

//非法类型异常
#define INVALIDTYPEEXCEPTION \
		ThrowSQLException<CSQLException>(ERROR_DATABASE_INVALIDDATATYPE,"%s[%d] Invalid Data Type",getColumnName(nIndex).c_str(),nIndex);

#ifdef _DEBUG
//检查类型是否正确
#define CHECKTYPE(a) 	if ( m_pResultData[nIndex-1].m_nDataType != a ) \
		INVALIDTYPEEXCEPTION

#define CHECKTYPE2(a,b) 	if ( m_pResultData[nIndex-1].m_nDataType != a && m_pResultData[nIndex-1].m_nDataType != b ) \
		INVALIDTYPEEXCEPTION

#define CHECKTYPE3(a,b,c) 	if ( m_pResultData[nIndex-1].m_nDataType != a && m_pResultData[nIndex-1].m_nDataType != b && m_pResultData[nIndex-1].m_nDataType != c ) \
		INVALIDTYPEEXCEPTION

#define CHECKTYPE4(a,b,c,d) 	if ( m_pResultData[nIndex-1].m_nDataType != a && m_pResultData[nIndex-1].m_nDataType != b && m_pResultData[nIndex-1].m_nDataType != c && m_pResultData[nIndex-1].m_nDataType != d ) \
		INVALIDTYPEEXCEPTION
#else
#define CHECKTYPE(a) 	{}

#define CHECKTYPE2(a,b) {}

#define CHECKTYPE3(a,b,c) {}

#define CHECKTYPE4(a,b,c,d) {}
#endif

//抛出空指针异常
#define NULLVALUEEXCEPTOIN ThrowSQLException<CSQLException>(ERROR_DATABASE_NULLVALUE,"%s[%d]NULL VALUE",getColumnName(nIndex).c_str(),nIndex)

#define RSCHECKNULL(bNullThrow,DEFAULT) { \
			if ( bNullThrow ) \
				NULLVALUEEXCEPTOIN; \
			else \
				return DEFAULT; \
		}

//检查是否为空并处理抛出异常
#define CHECKNULL(bNullThrow,DEFAULT)	if ( m_Row[nIndex-1] == NULL ) RSCHECKNULL(bNullThrow,DEFAULT);

//针对bytes的特殊处理
//#define GETBYTES(bNullThrow,DEFAULT)	CHECKNULL(bNullThrow,DEFAULT);return NULL;

/*

		Class	CResultSetImpl

*/

CResultSetImpl::CResultSetImpl(CConnectionImpl& Conn)
{
	this->m_pMetaData = NULL;
	m_pConn = &Conn;
	m_nFieldCount = 0;
	m_Row = NULL;
}

CResultSetImpl::~CResultSetImpl()
{
	if ( m_pResultData != NULL )
	{
		delete[] m_pResultData;
		m_pResultData = NULL;
	}
	close();
}

void CResultSetImpl::close()
{
	if ( m_pMetaData != NULL )
	{
		mysql_free_result(m_pMetaData);
		m_pMetaData = NULL;
	}
}

bool CResultSetImpl::init()
{
	m_pMetaData = mysql_use_result(m_pConn->m_pConn);
	if ( m_pMetaData == NULL )
		return false;
	m_nFieldCount = mysql_num_fields(m_pMetaData);
	this->m_Fields.clear();
	if ( m_nFieldCount > 0 )
	{
		m_pResultData = new CResultData[m_nFieldCount];
		MYSQL_FIELD* fields = mysql_fetch_fields(m_pMetaData);
		for ( int i = 0 ; i < m_nFieldCount ; i ++ )
		{
			MYSQL_FIELD* field = &fields[i];
			CResultData* pData = &m_pResultData[i];
			int len = strlen(field->name);
			for ( int k=0 ; k<len ; k++ )
				field->name[k] = toupper(field->name[k]);
			//pData->m_ColumnName = strupr(field->name);
			m_Fields[field->name] = i+1;
			pData->m_nDataType = getDataType(field->type);
		}
	}
	return true;
}

int CResultSetImpl::findColumn(const char* pColumnName) const
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
		return -1;
	return i->second;
}

string CResultSetImpl::getColumnName(int nIndex) const
{
	validIndex(nIndex);
	return m_pResultData[nIndex-1].m_ColumnName;
}

void CResultSetImpl::validIndex(int nIndex) const
{
	if ( m_pMetaData == NULL )
		throw CSQLException(ERROR_DATABASE_INVALIDOPER,"Result Not Open");
	if ( nIndex <= 0 || nIndex > this->m_nFieldCount )
		throw CSQLException(ERROR_DATABASE_INDEXOUT,"Index Out of Bounds");
}

int CResultSetImpl::getColumnCount() const
{
	return m_nFieldCount;
}

EDATATYPE CResultSetImpl::getColumnDataType(int nIndex)
{
	validIndex(nIndex);
	return m_pResultData[nIndex-1].m_nDataType;
}

bool CResultSetImpl::next()
{
	if ( m_pMetaData == NULL )
		throw CSQLException(ERROR_DATABASE_INVALIDOPER,"Result Closed");
	m_Row = mysql_fetch_row(m_pMetaData);
	if ( m_Row == NULL )
		return false;
	return true;
}

//************get funcs****************
bool CResultSetImpl::isNull(const char* pColumn) const
{
	return isNull(findColumn(pColumn));
}

bool CResultSetImpl::isNull(int nIndex) const
{
	validIndex(nIndex);
	VALIDROW;
	return ( m_Row[nIndex-1] == NULL );
}

char CResultSetImpl::getByte(const char* pColumn,bool bNullThrow,char Default) const
{
	return getByte(findColumn(pColumn),bNullThrow,Default);
}

char CResultSetImpl::getByte(int nIndex,bool bNullThrow,char Default) const
{
	validIndex(nIndex);
	VALIDROW;
	CHECKTYPE(TYPE_CHAR);
	CHECKNULL(bNullThrow,Default);
	return atoi(m_Row[nIndex-1]);
}

int CResultSetImpl::getBytes(const char* pColumnName,unsigned char* pOutBuffer,int nBuffSize,int nCharCount,bool bNullThrow /* = true */,const char* pDefault /* = DEFAULT_STRING */,unsigned int nOffset /* = 1 */) const
{
	return getBytes(findColumn(pColumnName),pOutBuffer,nBuffSize,nCharCount,bNullThrow,pDefault,nOffset);
}

int CResultSetImpl::getBytes(int nIndex,unsigned char* pOutBuffer,int nBuffSize,int nCharCount,bool bNullThrow /* = true */,const char* pDefault /* = DEFAULT_STRING */,unsigned int nOffset /* = 1 */) const
{
	validIndex(nIndex);
	VALIDROW;
	CHECKTYPE(TYPE_BLOB);
	int size = 0;
	const char* pvalue = NULL;
	if ( m_Row[nIndex-1] == NULL )
	{
		if ( bNullThrow || pDefault == NULL )
			NULLVALUEEXCEPTOIN;
		pvalue = pDefault;
		size = strlen(pvalue);
	}
	else
	{
		size = mysql_fetch_lengths(m_pMetaData)[nIndex-1];
		pvalue = m_Row[nIndex-1];
	}
	if ( size > nBuffSize )
		throw CSQLException(-7,"blob size exceeded");
	memcpy(pOutBuffer,pvalue,size);
	return size;
}

CDateTime CResultSetImpl::getDate(const char* pColumn,bool bNulLThrow,const CDateTime& Default) const
{
	return getDate(findColumn(pColumn),bNulLThrow,Default);
}

CDateTime CResultSetImpl::getDate(int nIndex,bool bNulLThrow,const CDateTime& Default) const
{
	validIndex(nIndex);
	VALIDROW;
	CHECKTYPE2(TYPE_DATE,TYPE_TIMESTAMP);
	CHECKNULL(bNulLThrow,Default);
	CDateTime time;
	time.parse(m_Row[nIndex-1],"YYYY-MM-DD");
	return time;
}

CDateTime CResultSetImpl::getTime(const char* pColumn,bool bNulLThrow,const CDateTime& Default) const
{
	return getTime(findColumn(pColumn),bNulLThrow,Default);
}

CDateTime CResultSetImpl::getTime(int nIndex,bool bNullThrow,const CDateTime& Default) const
{
	validIndex(nIndex);
	VALIDROW;
	CHECKTYPE2(TYPE_TIME,TYPE_TIMESTAMP);
	CHECKNULL(bNullThrow,Default);
	CDateTime time;
	time.parse(m_Row[nIndex-1],"hh:mm:ss");
	return time;
}

CDateTime CResultSetImpl::getTimestamp(const char* pColumn,bool bNullThrow,const CDateTime& Default) const
{
	return getTimestamp(findColumn(pColumn),bNullThrow,Default);
}

CDateTime CResultSetImpl::getTimestamp(int nIndex,bool bNullThrow,const CDateTime& Default) const
{
	validIndex(nIndex);
	VALIDROW;
	CHECKTYPE3(TYPE_TIMESTAMP,TYPE_TIME,TYPE_DATE);
	CHECKNULL(bNullThrow,Default);
	CDateTime time;
	time.parse(m_Row[nIndex-1],"YYYY-MM-DD hh:mm:ss");
	return time;
}

double CResultSetImpl::getDouble(const char* pColumn,bool bNullThrow,double dDefault) const
{
	return getDouble(findColumn(pColumn),bNullThrow,dDefault);
}

double CResultSetImpl::getDouble(int nIndex,bool bNullThrow,double dDefault) const
{
	validIndex(nIndex);
	VALIDROW;
	CHECKTYPE(TYPE_DOUBLE);
	CHECKNULL(bNullThrow,dDefault);
	return atof(m_Row[nIndex-1]);
}

string CResultSetImpl::getString(const char* pColumn,bool bNullThrow,const char* pDefault) const
{
	return getString(findColumn(pColumn),bNullThrow,pDefault);
}

string CResultSetImpl::getString(int nIndex,bool bNullThrow,const char* pDefault) const
{
	validIndex(nIndex);
	VALIDROW;
	CHECKTYPE2(TYPE_STRING,TYPE_BLOB);
	CHECKNULL(bNullThrow,pDefault);
	return m_Row[nIndex-1];
}

int CResultSetImpl::getInt(const char* pColumn,bool bNullThrow,int nDefault) const
{
	return getInt(findColumn(pColumn),bNullThrow,nDefault);
}

int CResultSetImpl::getInt(int nIndex,bool bNullThrow,int nDefault) const
{
	validIndex(nIndex);
	VALIDROW;
	CHECKTYPE(TYPE_INT);
	CHECKNULL(bNullThrow,nDefault);
	return atoi(m_Row[nIndex-1]);
}

int64 CResultSetImpl::getInt64(const char* pColumn,bool bNullThrow,int64 nDefault) const
{
	return getInt64(findColumn(pColumn),bNullThrow,nDefault);
}

int64 CResultSetImpl::getInt64(int nIndex,bool bNullThrow,int64 nDefault) const
{
	validIndex(nIndex);
	VALIDROW;
	CHECKTYPE(TYPE_LONG64);
	CHECKNULL(bNullThrow,nDefault);
	return atol64(m_Row[nIndex-1]);
}


/*

		Class	CPrepareResultSetImpl

*/

CPrepareResultSetImpl::CPrepareResultSetImpl(CStatementImpl& Stmt)
{
	m_pStmt = new CRefStatement(Stmt);
	this->m_pMetaData = NULL;
	m_pResultData = NULL;
	m_nFieldCount = 0;
}

CPrepareResultSetImpl::~CPrepareResultSetImpl()
{
	if ( m_pMetaData != NULL )
	{
		mysql_free_result(m_pMetaData);
		m_pMetaData = NULL;
	}
	close();
}

bool CPrepareResultSetImpl::init()
{
	m_pMetaData = mysql_stmt_result_metadata(m_pStmt->m_pStmtImpl->m_pStmt);
	if ( m_pMetaData == NULL )
		THROWEXCEPTION;
	m_nFieldCount = mysql_num_fields(m_pMetaData);
	this->m_Fields.clear();
	if ( m_nFieldCount > 0 )
	{
		m_pResultData = new CResultData[m_nFieldCount];
		m_pResult = new MYSQL_BIND[m_nFieldCount];
		memset(m_pResult,0,sizeof(MYSQL_BIND)*m_nFieldCount);
		MYSQL_FIELD* fields = mysql_fetch_fields(m_pMetaData);
		for ( int i = 0 ; i < m_nFieldCount ; i ++ )
		{
			MYSQL_FIELD* field = &fields[i];
			CResultData* pData = &m_pResultData[i];
			pData->m_ColumnName = field->name;
			m_Fields[pData->m_ColumnName] = i+1;
			MYSQL_BIND* bind = &m_pResult[i];
			//memset(bind,0,sizeof(MYSQL_BIND));
			bind->buffer_type = field->type;
			bind->is_null = new char;
			bind->length = new unsigned long;
			EDATATYPE type = getDataType(field->type);
			switch ( type )
			{
				case TYPE_CHAR :
				case TYPE_INT :
					bind->buffer = malloc(sizeof(int));
					break;
				case TYPE_LONG64 :
					bind->buffer = malloc(sizeof(int64));
					break;
				case TYPE_DOUBLE :
					bind->buffer = malloc(sizeof(double));
					break;
				case TYPE_DATE :
					bind->buffer = malloc(sizeof(MYSQL_TIME));
					break;
				case TYPE_TIME :
					bind->buffer = malloc(sizeof(MYSQL_TIME));
					break;
				case TYPE_TIMESTAMP :
					bind->buffer = malloc(sizeof(MYSQL_TIME));
					break;
				case TYPE_STRING :
					bind->buffer = malloc(field->length+1);
					bind->buffer_length = field->length;
					break;
				case TYPE_BLOB :
					{
						unsigned int allocsize = MAXBLOBSIZE;
						if ( allocsize > field->length )
							allocsize = field->length;
						bind->buffer = malloc(allocsize);
						bind->buffer_length = allocsize;
					}
				default:
					break;
			}
			pData->m_nDataType = type;
		}
		EXECUTE(mysql_stmt_bind_result(m_pStmt->m_pStmtImpl->m_pStmt,m_pResult));
	}
	return true;
}

bool CPrepareResultSetImpl::next()
{
	if ( NULL == m_pStmt )
		throw CSQLException(ERROR_DATABASE_INVALIDOPER,"Result Closed");
	int retval = mysql_stmt_fetch(m_pStmt->m_pStmtImpl->m_pStmt);
	if ( retval == 1 )
		throw CSQLException(m_pStmt->getErrNo(),m_pStmt->getError());
	if ( retval == MYSQL_NO_DATA )
	{
		close();
		return false;
	}
	return true;
}

void CPrepareResultSetImpl::cleanParam(MYSQL_BIND& Bind)
{
	if ( Bind.is_null != NULL )
	{
		delete Bind.is_null;
		Bind.is_null = NULL;
	}
	if ( Bind.length != NULL )
	{
		delete Bind.length;
		Bind.length = NULL;
	}
	if ( Bind.buffer != NULL )
	{
		free(Bind.buffer);
		Bind.buffer = NULL;
	}
}

void CPrepareResultSetImpl::close()
{
	if ( m_pStmt != NULL )
	{
		m_pStmt->close();
		delete m_pStmt;
		m_pStmt = NULL;
	}
	if ( m_pResultData != NULL )
	{
		delete[] m_pResultData;
		m_pResultData = NULL;
	}
	if ( m_pResult != NULL )
	{
		for ( int i = 0 ; i < m_nFieldCount ; i ++ )
		{
			cleanParam(m_pResult[i]);
		}
		delete[] m_pResult;
		m_pResult = NULL;
	}
}

int CPrepareResultSetImpl::findColumn(const char* pColumnName) const
{
	map<string,int>::const_iterator i = this->m_Fields.find(pColumnName);
	if ( i == m_Fields.end() )
		return -1;
	return i->second;
}

string CPrepareResultSetImpl::getColumnName(int nIndex) const
{
	validIndex(nIndex);
	return m_pResultData[nIndex-1].m_ColumnName;
}

void CPrepareResultSetImpl::validIndex(int nIndex) const
{
	if ( nIndex <= 0 || nIndex > this->m_nFieldCount )
		throw CSQLException(ERROR_DATABASE_INDEXOUT,"Index Out of Bounds");
}

int CPrepareResultSetImpl::getColumnCount() const
{
	return m_nFieldCount;
}

EDATATYPE CPrepareResultSetImpl::getColumnDataType(int nIndex)
{
	validIndex(nIndex);
	return m_pResultData[nIndex-1].m_nDataType;
}

//*****************get funcs***********************
/*
bool CPrepareResultSetImpl::getBoolean(const char* pColumn)
{
	return getBoolean(this->findColumn(pColumn));
}

bool CPrepareResultSetImpl::getBoolean(int nIndex)
{
	validIndex(nIndex);
	bool retval = false;
	MYSQL_BIND* bind = &m_pResult[nIndex-1];
	if ( bind->buffer_type != MYSQL_TYPE_TINY )
		throw CSQLException(0x03,"Invalid Data Type");
	return *(char*)bind->buffer
	return retval;
}
*/

#define RSCHECKTYPE(a) if ( bind->buffer_type != a ) \
		INVALIDTYPEEXCEPTION;

#define RSCHECKTYPE2(a,b) if ( bind->buffer_type != a && bind->buffer_type != b ) \
		INVALIDTYPEEXCEPTION;

#define RSCHECKTYPE3(a,b,c) if ( bind->buffer_type != a && bind->buffer_type != b && bind->buffer_type != c ) \
		INVALIDTYPEEXCEPTION;

#define RSCHECKTYPE4(a,b,c,d) if ( bind->buffer_type != a && bind->buffer_type != b && bind->buffer_type != c && bind->buffer_type != d ) \
		INVALIDTYPEEXCEPTION;

#define PRSCHECKNULL(bThrow,DEFAULT) if ( *bind->is_null ) RSCHECKNULL(bNullThrow,DEFAULT);

char CPrepareResultSetImpl::getByte(const char* pColumnName,bool bNullThrow,char Default) const
{
	return getByte(findColumn(pColumnName),bNullThrow,Default);
}

char CPrepareResultSetImpl::getByte(int nIndex,bool bNullThrow,char Default) const
{
	validIndex(nIndex);
	MYSQL_BIND* bind = &m_pResult[nIndex-1];
	RSCHECKTYPE(MYSQL_TYPE_TINY);
	PRSCHECKNULL(bNullThrow,Default);
	return *(char*)bind->buffer;
}

int CPrepareResultSetImpl::getBytes(const char* pColumnName,unsigned char* pOutBuffer,int nBuffSize,int nCharCount,bool bNullThrow /* = true */,const char* pDefault /* = DEFAULT_STRING */,unsigned int nOffset /* = 1 */) const
{
	return getBytes(findColumn(pColumnName),pOutBuffer,nBuffSize,nCharCount,bNullThrow,pDefault,nOffset);
}

int CPrepareResultSetImpl::getBytes(int nIndex,unsigned char* pOutBuffer,int nBuffSize,int nCharCount,bool bNullThrow /* = true */,const char* pDefault /* = DEFAULT_STRING */,unsigned int nOffset /* = 1 */) const
{
	validIndex(nIndex);
	MYSQL_BIND* bind = &m_pResult[nIndex-1];
	RSCHECKTYPE(MYSQL_TYPE_BLOB);
	const void* pvalue = NULL;
	unsigned int size = 0;
	//如果为空，则返回失败。
	if ( *bind->is_null )
	{
		if ( bNullThrow || pDefault == NULL )
			throw CSQLException(ERROR_DATABASE_NULLVALUE,"NULL VALUE");
		pvalue = pDefault;
		size = strlen(pDefault);
	}
	else
	{
		size = nBuffSize;
		if ( size > *bind->length )
			size = *bind->length;
		else
			throw CSQLException(-7,"blob size exceeded");
		pvalue = bind->buffer;
	}
	memcpy(pOutBuffer,pvalue,size);
	return size;
}

CDateTime CPrepareResultSetImpl::getDate(const char* pColumn,bool bNullThrow,const CDateTime& Default) const
{
	return getDate(findColumn(pColumn),bNullThrow,Default);
}

CDateTime CPrepareResultSetImpl::getDate(int nIndex,bool bNullThrow,const CDateTime& Default) const
{
	CDateTime time;
	validIndex(nIndex);
	MYSQL_BIND* bind = &m_pResult[nIndex-1];
	RSCHECKTYPE3(MYSQL_TYPE_DATE,MYSQL_TYPE_TIMESTAMP,MYSQL_TYPE_DATETIME);
	PRSCHECKNULL(bNullThrow,Default);
	convert(time,(*(MYSQL_TIME*)bind->buffer));
	return time;
}

CDateTime CPrepareResultSetImpl::getTime(const char* pColumn,bool bNullThrow,const CDateTime& Default) const
{
	return getTime(findColumn(pColumn),bNullThrow,Default);
}

CDateTime CPrepareResultSetImpl::getTime(int nIndex,bool bNullThrow,const CDateTime& Default) const
{
	CDateTime time;
	validIndex(nIndex);
	MYSQL_BIND* bind = &m_pResult[nIndex-1];
	RSCHECKTYPE3(MYSQL_TYPE_TIME,MYSQL_TYPE_TIMESTAMP,MYSQL_TYPE_DATETIME);
	PRSCHECKNULL(bNulLThrow,Default);
	convert(time,(*(MYSQL_TIME*)bind->buffer));
	return time;
}

CDateTime CPrepareResultSetImpl::getTimestamp(const char* pColumn,bool bNullThrow,const CDateTime& Default) const
{
	return getTimestamp(findColumn(pColumn),bNullThrow,Default);
}

CDateTime CPrepareResultSetImpl::getTimestamp(int nIndex,bool bNullThrow,const CDateTime& Default) const
{
	CDateTime time;
	validIndex(nIndex);
	MYSQL_BIND* bind = &m_pResult[nIndex-1];
	RSCHECKTYPE4(MYSQL_TYPE_TIMESTAMP,MYSQL_TYPE_DATETIME,MYSQL_TYPE_DATE,MYSQL_TYPE_TIME);
	PRSCHECKNULL(bNullThrow,Default);
	convert(time,(*(MYSQL_TIME*)bind->buffer));
	return time;
}

bool CPrepareResultSetImpl::isNull(const char* pColumn) const
{
	return isNull(findColumn(pColumn));
}

bool CPrepareResultSetImpl::isNull(int nIndex) const
{
	validIndex(nIndex);
	MYSQL_BIND* bind = &m_pResult[nIndex-1];
	return 0 != *bind->is_null;
}

string CPrepareResultSetImpl::getString(const char* pColumn,bool bNullThrow,const char* pDefault) const
{
	return getString(findColumn(pColumn),bNullThrow,pDefault);
}

string CPrepareResultSetImpl::getString(int nIndex,bool bNullThrow,const char* pDefault) const
{
	validIndex(nIndex);
	MYSQL_BIND* bind = &m_pResult[nIndex-1];
	RSCHECKTYPE3(MYSQL_TYPE_STRING,MYSQL_TYPE_VAR_STRING,MYSQL_TYPE_BLOB);
	PRSCHECKNULL(bNullThrow,pDefault);
	if ( !*bind->is_null )
		((char*)bind->buffer)[*bind->length] = 0;
	return (char*)bind->buffer;
}

int64 CPrepareResultSetImpl::getInt64(const char* pColumn,bool bNullThrow,int64 nDefault) const
{
	return getInt64(findColumn(pColumn),bNullThrow,nDefault);
}

int64 CPrepareResultSetImpl::getInt64(int nIndex,bool bNullThrow,int64 nDefault) const
{
	validIndex(nIndex);
	MYSQL_BIND* bind = &m_pResult[nIndex-1];
	RSCHECKTYPE(MYSQL_TYPE_LONGLONG);
	PRSCHECKNULL(bNulLThrow,nDefault);
	return *(my_ulonglong*)(bind->buffer);
}

double CPrepareResultSetImpl::getDouble(const char* pColumn,bool bNullThrow,double dDefault) const
{
	return getDouble(findColumn(pColumn),bNullThrow,dDefault);
}

double CPrepareResultSetImpl::getDouble(int nIndex,bool bNullThrow,double dDefault) const
{
	validIndex(nIndex);
	MYSQL_BIND* bind = &m_pResult[nIndex-1];
	RSCHECKTYPE(MYSQL_TYPE_DOUBLE);
	PRSCHECKNULL(bNulLThrow,dDefault);
	return *(double*)(bind->buffer);
}

int CPrepareResultSetImpl::getInt(const char* pColumn,bool bNullThrow,int nDefault) const
{
	return getInt(findColumn(pColumn),bNullThrow,nDefault);
}

int CPrepareResultSetImpl::getInt(int nIndex,bool bNullThrow,int nDefault) const
{
	validIndex(nIndex);
	MYSQL_BIND* bind = &m_pResult[nIndex-1];
	RSCHECKTYPE3(MYSQL_TYPE_LONG,MYSQL_TYPE_TINY,MYSQL_TYPE_INT24);
	PRSCHECKNULL(bNulLThrow,nDefault);
	switch (*bind->length)
	{
		case 1 :
			return ((char*)bind->buffer)[0];
		case 2 :
			return *(short*)(bind->buffer);
	}
	return *(int*)(bind->buffer);
}

