#include "mysqlpublic.h"
#include <util/autoptr.h>
#include "statementimpl.h"

#include "connectionimpl.h"
#include "resultsetimpl.h"
#include <db/sqlexception.h>
#include <db/dbpublic.h>

using namespace mysql;
#define THROWEXCEPTION throw CSQLException(getErrNo(),m_SQL + " : " + getError());
#define EXECUTE(a) if ( 0 != a ) \
			THROWEXCEPTION;
#define VALIDSTMT if ( NULL == m_pStmt ) \
			throw CSQLException(ERROR_DATABASE_INVALIDOPER,"Not Prepared");
/*

		Class	CStatementImpl

*/
CStatementImpl::CStatementImpl(CConnectionImpl& Conn)
{
	m_pConn = &Conn;
	this->m_pStmt = NULL;
	m_pParams = NULL;
	m_nParamCount = 0;
}

CStatementImpl::~CStatementImpl()
{
	close();
}

void CStatementImpl::close()
{
	if ( m_pStmt != NULL )
	{
		mysql_stmt_close(m_pStmt);
		m_pStmt = NULL;
	}
	this->destroyParams();
}

uint64 CStatementImpl::getLastInsertID()
{
	if ( m_pStmt != NULL )
		return mysql_stmt_insert_id(m_pStmt);
	return mysql_insert_id(m_pConn->m_pConn);
}

uint64 CStatementImpl::getInsertID(void* pParam)
{
	return getLastInsertID();
}

int CStatementImpl::executeUpdate(const char* pSQL)
{
	m_SQL = pSQL;
	EXECUTE(mysql_real_query(m_pConn->m_pConn,pSQL,strlen(pSQL)));
	return (int)mysql_affected_rows(m_pConn->m_pConn);
}

bool CStatementImpl::execute(const char* pSQL)
{
	m_SQL = pSQL;
	EXECUTE(mysql_real_query(m_pConn->m_pConn,pSQL,strlen(pSQL)));
	return (0 != mysql_affected_rows(m_pConn->m_pConn));
}

CAutoPtr<CResultSet> CStatementImpl::executeQuery(const char* pSQL)
{
	m_SQL = pSQL;
	EXECUTE(mysql_real_query(m_pConn->m_pConn,pSQL,strlen(pSQL)));
	CAutoPtr<CResultSetImpl> result = new CResultSetImpl(*this->m_pConn);
	if ( result->init() )
		return result.release();
	throw CSQLException(ERROR_DATABASE_INVALIDOPER,"Result Execute Error");
}

void CStatementImpl::prepare(const char* pSQL)
{
	m_SQL = pSQL;
	if ( m_pStmt != NULL )
		close();
	m_pStmt = mysql_stmt_init(m_pConn->m_pConn);
	if ( m_pStmt == NULL )
		THROWEXCEPTION;
	EXECUTE(mysql_stmt_prepare(m_pStmt,pSQL,strlen(pSQL)));
	prepareParams();
}

bool CStatementImpl::execute()
{
	VALIDSTMT;
	EXECUTE(mysql_stmt_execute(m_pStmt));
	return (0 != mysql_stmt_affected_rows(m_pStmt));
}

int CStatementImpl::executeUpdate()
{
	VALIDSTMT;
	EXECUTE(mysql_stmt_bind_param(m_pStmt,m_pParams));
	EXECUTE(mysql_stmt_execute(m_pStmt));
	my_ulonglong affected_rows = mysql_stmt_affected_rows(m_pStmt);
	return (int)affected_rows;
}

CAutoPtr<CResultSet> CStatementImpl::executeQuery()
{
	VALIDSTMT;
	EXECUTE(mysql_stmt_bind_param(m_pStmt,m_pParams));
	EXECUTE(mysql_stmt_execute(m_pStmt));
	CAutoPtr<CPrepareResultSetImpl> result = new CPrepareResultSetImpl(*this);
	result->init();
	return result.release();
}

//*************set params*************************
/*
void CStatementImpl::setBoolean(int nParamIndex,bool Value)
{
	setByte(nParamIndex,Value?1:0);
}
*/

void CStatementImpl::setByte(int nParamIndex,char Value)
{
	VALIDSTMT;
	validIndex(nParamIndex);
	MYSQL_BIND* bind = &m_pParams[nParamIndex-1];//.MYSQL_BIND();
	bind->buffer_type = MYSQL_TYPE_TINY;
	bind->buffer = malloc(sizeof(char));
	*((unsigned char*)bind->buffer) = (unsigned char)Value;
	//EXECUTE(mysql_stmt_bind_param(m_pStmt,bind));
}

void CStatementImpl::setBytes(int nParamIndex,const char* Value,int nLength)
{
	VALIDSTMT;
	validIndex(nParamIndex);
	MYSQL_BIND* bind = &m_pParams[nParamIndex-1];//.MYSQL_BIND();
	bind->buffer_type = MYSQL_TYPE_BLOB;
	//bind->length = new unsigned long;
	//*bind->length = Value.length() + 1;
	bind->buffer_length = nLength;
	bind->buffer = malloc(nLength);
	memcpy(bind->buffer,Value,nLength);
	//这个只能用于bind以后才行
	//EXECUTE(mysql_stmt_send_long_data(m_pStmt,nParamIndex-1,Value,nLength));
}

void CStatementImpl::setDate(int nParamIndex,CDateTime Value)
{
	VALIDSTMT;
	validIndex(nParamIndex);
	MYSQL_BIND* bind = &m_pParams[nParamIndex-1];
	//MYSQL_BIND* bind = m_pParams[nParamIndex-1].MYSQL_BIND();
	bind->buffer_type = MYSQL_TYPE_DATE;
	bind->buffer = malloc(sizeof(MYSQL_TIME));
	convert(*(MYSQL_TIME*)bind->buffer,Value);
	bind->length = 0;
	//EXECUTE(mysql_stmt_bind_param(m_pStmt,bind));
}

void CStatementImpl::setTime(int nParamIndex,CDateTime Value)
{
	VALIDSTMT;
	validIndex(nParamIndex);
	MYSQL_BIND* bind = &m_pParams[nParamIndex-1];//.MYSQL_BIND();
	bind->buffer_type = MYSQL_TYPE_TIME;
	bind->buffer = malloc(sizeof(MYSQL_TIME));
	convert(*(MYSQL_TIME*)bind->buffer,Value);
	((MYSQL_TIME*)bind->buffer)->year = 0;
	((MYSQL_TIME*)bind->buffer)->month = 0;
	((MYSQL_TIME*)bind->buffer)->day = 0;
	bind->length = 0;
	//EXECUTE(mysql_stmt_bind_param(m_pStmt,bind));
}

void CStatementImpl::setTimestamp(int nParamIndex,CDateTime Value)
{
	VALIDSTMT;
	validIndex(nParamIndex);
	MYSQL_BIND* bind = &m_pParams[nParamIndex-1];//.MYSQL_BIND();
	bind->buffer_type = MYSQL_TYPE_TIMESTAMP;
	bind->buffer = malloc(sizeof(MYSQL_TIME));
	convert(*(MYSQL_TIME*)bind->buffer,Value);
	bind->length = 0;
	//EXECUTE(mysql_stmt_bind_param(m_pStmt,bind));
}

void CStatementImpl::setDouble(int nParamIndex,double Value)
{
	VALIDSTMT;
	validIndex(nParamIndex);
	MYSQL_BIND* bind = &m_pParams[nParamIndex-1];//.MYSQL_BIND();
	bind->buffer_type = MYSQL_TYPE_DOUBLE;
	bind->buffer = malloc(sizeof(double));
	*(double*)bind->buffer = Value;
	//EXECUTE(mysql_stmt_bind_param(m_pStmt,bind));
}

void CStatementImpl::setInt(int nParamIndex,int Value)
{
	VALIDSTMT;
	validIndex(nParamIndex);
	MYSQL_BIND* bind = &m_pParams[nParamIndex-1];//.MYSQL_BIND();
	bind->buffer_type = MYSQL_TYPE_LONG;
	bind->buffer = malloc(sizeof(int));
	*(int*)bind->buffer = Value;
	bind->length = 0;
	//EXECUTE(mysql_stmt_bind_param(m_pStmt,bind));
}

void CStatementImpl::setUInt(int nParamIndex,unsigned int Value)
{
	VALIDSTMT;
	validIndex(nParamIndex);
	MYSQL_BIND* bind = &m_pParams[nParamIndex-1];//.MYSQL_BIND();
	bind->buffer_type = MYSQL_TYPE_LONG;
	bind->buffer = malloc(sizeof(int));
	*(unsigned int*)bind->buffer = Value;
	bind->length = 0;
	bind->is_unsigned = 1;
	//EXECUTE(mysql_stmt_bind_param(m_pStmt,bind));
}


void CStatementImpl::setInt64(int nParamIndex,int64 Value)
{
	VALIDSTMT;
	validIndex(nParamIndex);
	MYSQL_BIND* bind = &m_pParams[nParamIndex-1];//.MYSQL_BIND();
	bind->buffer_type = MYSQL_TYPE_LONGLONG;
	bind->buffer = malloc(sizeof(int64));
	*(int64*)bind->buffer = Value;
	//EXECUTE(mysql_stmt_bind_param(m_pStmt,bind));
}

void CStatementImpl::setUInt64(int nParamIndex,uint64 Value)
{
	VALIDSTMT;
	validIndex(nParamIndex);
	MYSQL_BIND* bind = &m_pParams[nParamIndex-1];//.MYSQL_BIND();
	bind->buffer_type = MYSQL_TYPE_LONGLONG;
	bind->buffer = malloc(sizeof(int64));
	*(uint64*)bind->buffer = Value;
	bind->is_unsigned = 1;
	//EXECUTE(mysql_stmt_bind_param(m_pStmt,bind));
}
void CStatementImpl::setNull(int nParamIndex,EDATATYPE nEDATATYPE)
{
	VALIDSTMT;
	validIndex(nParamIndex);
	MYSQL_BIND* bind = &m_pParams[nParamIndex-1];//.MYSQL_BIND();
	bind->buffer_type = getDBType(nEDATATYPE);
	bind->is_null = new char;
	*bind->is_null = 1;
	//EXECUTE(mysql_stmt_bind_param(m_pStmt,bind));
}

void CStatementImpl::setString(int nParamIndex,const string& Value)
{
	VALIDSTMT;
	validIndex(nParamIndex);
	MYSQL_BIND* bind = &m_pParams[nParamIndex-1];//.MYSQL_BIND();
	bind->buffer_type = MYSQL_TYPE_VAR_STRING;
	//bind->length = new unsigned long;
	//*bind->length = Value.length() + 1;
	bind->buffer_length = Value.length();
	bind->buffer = malloc(Value.length() + 1);
	sprintf((char*)bind->buffer,"%s",Value.c_str());
	//EXECUTE(mysql_stmt_bind_param(m_pStmt,bind));
}

//*******************CStatementImpl Funcs***************************
void CStatementImpl::clearParameters()
{
	for ( int i = 1 ; i <= m_nParamCount ; i ++ )
	{
		clearParam(i);
	}
}

//********************other funcs****************
int CStatementImpl::getErrNo()
{
	if ( m_pStmt != NULL )
		return mysql_stmt_errno(m_pStmt);
	return this->m_pConn->getErrNo();
}

const char* CStatementImpl::getError()
{
	if ( m_pStmt != NULL )
		return mysql_stmt_error(m_pStmt);
	return this->m_pConn->getError();
}

const char* CStatementImpl::getSQL() const
{
	return this->m_SQL.c_str();
}

void CStatementImpl::prepareParams()
{
	destroyParams();
	m_nParamCount = mysql_stmt_param_count(m_pStmt);
	if ( m_nParamCount > 0 )
	{
		m_pParams = new MYSQL_BIND[m_nParamCount];
		memset(m_pParams,0,sizeof(MYSQL_BIND)*m_nParamCount);
	}
}

void CStatementImpl::destroyParams()
{
	if ( m_pParams != NULL )
	{
		for ( int i = 0 ; i < this->m_nParamCount ; i ++ )
			cleanParam(m_pParams[i]);
		delete[] m_pParams;
	}
	m_pParams = NULL;
}

void CStatementImpl::cleanParam(MYSQL_BIND& Param)
{
	if ( NULL != Param.buffer )
	{
		free(Param.buffer);
		Param.buffer = NULL;
	}
	if ( NULL != Param.length )
	{
		delete Param.length;
		Param.length = NULL;
	}
	if ( NULL != Param.is_null )
	{
		delete Param.is_null;
		Param.is_null = NULL;
	}
}

void CStatementImpl::clearParam(int nIndex)
{
	//由于参数可能重用，释放没需要的参数
	cleanParam(m_pParams[nIndex - 1]);//.clear();
}

void CStatementImpl::validIndex(int nIndex)
{
	if ( nIndex > m_nParamCount || nIndex <= 0 )
		throw CSQLException(ERROR_DATABASE_INDEXOUT,"Index Out Of Bounds");
	clearParam(nIndex);
}

int CStatementImpl::getTimeZoneOffset(bool Throw)
{
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"Not Support");
	return -1;
}
void CStatementImpl::saveBytes(const char* strColumnName, const unsigned char* Value, int nLength,bool SetSequence,string SequenceName,bool SetPkName,string PkName)
{
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"Not Support");
}

void CStatementImpl::setBytes(int nParamIndex, const unsigned char* Value, int nLength)
{
	VALIDSTMT;
	validIndex(nParamIndex);
	MYSQL_BIND* bind = &m_pParams[nParamIndex-1];//.MYSQL_BIND();
	bind->buffer_type = MYSQL_TYPE_BLOB;
	bind->buffer_length = nLength;
	bind->buffer = malloc(nLength);
	memcpy(bind->buffer,Value,nLength);
}

/*

		Class	CRefStatement

*/
CRefStatement::CRefStatement(CStatementImpl& Stmt)
{
	m_pStmtImpl = &Stmt;
	m_pStmtImpl->incRef();
}

CRefStatement::~CRefStatement()
{
	m_pStmtImpl->decRef();
}

void CRefStatement::clearParameters()
{
	m_pStmtImpl->clearParameters();
}

void CRefStatement::close()
{
	m_pStmtImpl->close();
}

bool CRefStatement::execute()
{
	return m_pStmtImpl->execute();
}

bool CRefStatement::execute(const char* pSQL)
{
	return m_pStmtImpl->execute(pSQL);
}

CAutoPtr<CResultSet> CRefStatement::executeQuery()
{
	return m_pStmtImpl->executeQuery();
}

CAutoPtr<CResultSet> CRefStatement::executeQuery(const char* pSQL)
{
	return m_pStmtImpl->executeQuery(pSQL);
}

int CRefStatement::executeUpdate()
{
	return m_pStmtImpl->executeUpdate();
}

int CRefStatement::executeUpdate(const char* pSQL)
{
	return m_pStmtImpl->executeUpdate(pSQL);
}

uint64 CRefStatement::getInsertID(void* pParam)
{
	return m_pStmtImpl->getInsertID(pParam);
}

int CRefStatement::getErrNo()
{
	return m_pStmtImpl->getErrNo();
}

const char* CRefStatement::getError()
{
	return m_pStmtImpl->getError();
}

const char* CRefStatement::getSQL() const
{
	return m_pStmtImpl->getSQL();
}

void CRefStatement::prepare(const char* pSql)
{
	m_pStmtImpl->prepare(pSql);
}

void CRefStatement::setByte(int nParamIndex,char Value)
{
	m_pStmtImpl->setByte(nParamIndex,Value);
}

void CRefStatement::setBytes(int nParamIndex,const char* Value,int nLength)
{
	m_pStmtImpl->setBytes(nParamIndex,Value,nLength);
}


void CRefStatement::setDate(int nParamIndex,CDateTime Value)
{
	m_pStmtImpl->setDate(nParamIndex,Value);
}

void CRefStatement::setTime(int nParamIndex,CDateTime Value)
{
	m_pStmtImpl->setTime(nParamIndex,Value);
}

void CRefStatement::setTimestamp(int nParamIndex,CDateTime Value)
{
	m_pStmtImpl->setTimestamp(nParamIndex,Value);
}

void CRefStatement::setDouble(int nParamIndex,double Value)
{
	m_pStmtImpl->setDouble(nParamIndex,Value);
}

void CRefStatement::setInt(int nParamIndex,int Value)
{
	m_pStmtImpl->setInt(nParamIndex,Value);
}

void CRefStatement::setUInt(int nParamIndex,unsigned int Value)
{
	m_pStmtImpl->setUInt(nParamIndex,Value);
}

void CRefStatement::setInt64(int nParamIndex,int64 Value)
{
	m_pStmtImpl->setInt64(nParamIndex,Value);
}

void CRefStatement::setUInt64(int nParamIndex,uint64 Value)
{
	m_pStmtImpl->setUInt64(nParamIndex,Value);
}

void CRefStatement::setNull(int nParamIndex,EDATATYPE nEDATATYPE)
{
	m_pStmtImpl->setNull(nParamIndex,nEDATATYPE);
}

void CRefStatement::setString(int nParamIndex,const string& Value)
{
	m_pStmtImpl->setString(nParamIndex,Value);
}

int CRefStatement::getTimeZoneOffset(bool Throw)
{
	return m_pStmtImpl->getTimeZoneOffset(Throw);
}
void CRefStatement::saveBytes(const char* strColumnName, const unsigned char* Value, int nLength,bool SetSequence,string SequenceName,bool SetPkName,string PkName)
{
	m_pStmtImpl->saveBytes(strColumnName, Value, nLength,SetSequence,SequenceName,SetPkName,PkName);
}

void CRefStatement::setBytes(int nParamIndex, const unsigned char* Value, int nLength)
{
	m_pStmtImpl->setBytes(nParamIndex,Value,nLength);
}
