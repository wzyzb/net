#include "../../public.h"
#include "../sqlexception.h"
#include "../../util/autoptr.h"
#include "connectionimpl.h"
using namespace mysql;

#include "mysqlpublic.h"
#include "statementimpl.h"
#include "callablestatementimpl.h"
#include "../dbpublic.h"

/*
#include <public.h>
#include <db/sqlexception.h>
#include <util/autoptr.h>
#include "connectionimpl.h"
using namespace mysql;

#include "mysqlpublic.h"
#include "statementimpl.h"
#include "callablestatementimpl.h"
#include <db/dbpublic.h>
*/

#define VALIDCONN if ( NULL == m_pConn || ! m_bConnected ) \
					throw CSQLException(ERROR_DATABASE_INVALIDOPER,"Connection Not Connected");
/*

		Class	CConnectionImpl

*/
CConnectionImpl::CConnectionImpl(const string& Host,unsigned short Port,const string& DataBase,
								 const string& UserName,const string& Password) : m_UserName(UserName),m_PassWord(Password),m_Database(DataBase),m_Host(Host),m_nPort(Port)
{
	m_bConnected = false;
	m_pConn = NULL;
	m_bAutoCommit = true;
}

CConnectionImpl::~CConnectionImpl()
{
	close();
}

CAutoPtr <CConnection> CConnectionImpl::clone()
{
	if ((this->m_UserName=="")||(this->m_PassWord=="")||(this->m_Host==""))
	{
		throw CSQLException(ERROR_DATABASE_INVALIDPROTOCOL, "Connection param error while calling CConnectionImpl::clone");  
	}

	CAutoPtr<CConnectionImpl> conn = new CConnectionImpl(m_Host.c_str(),m_nPort,m_Database.c_str(),m_UserName.c_str(),m_PassWord.c_str());
	conn->connect();
	return conn.release();
}

void CConnectionImpl::close()
{
	if ( m_pConn != NULL )
	{
		mysql_close(m_pConn);
		m_pConn = NULL;
	}
	m_bConnected = false;
}

void CConnectionImpl::keepActive()
{
	if ( m_pConn != NULL )
		mysql_ping(m_pConn);
}

bool CConnectionImpl::commit()
{
	VALIDCONN;
	return ( 0 == mysql_commit(m_pConn) );
}

bool CConnectionImpl::rollback()
{
	VALIDCONN;
	return ( 0 == mysql_rollback(m_pConn) );
}

bool CConnectionImpl::connect()
{
	close();
	m_pConn = mysql_init(NULL);
	if ( NULL == m_pConn )
		return false;
	my_bool reconnflag = true;
	mysql_options(m_pConn,MYSQL_OPT_RECONNECT,&reconnflag);
	m_bConnected = ( NULL != mysql_real_connect(m_pConn,m_Host.c_str(),m_UserName.c_str(),m_PassWord.c_str(),m_Database.c_str(),m_nPort,NULL,0) );
	if ( m_bConnected )
	{
		this->setAutoCommit(m_bAutoCommit);
		//…Ë÷√Œ™UTF8±‡¬Î
		mysql_set_character_set(m_pConn,"utf8");
	}
	return m_bConnected;
}

bool CConnectionImpl::isClosed()
{
	if ( ! m_bConnected )
		return true;
	if ( 0 == mysql_ping(m_pConn) )
		return false;
	return true;
}

void CConnectionImpl::setAutoCommit(bool bAutoCommit)
{
	VALIDCONN;
	mysql_autocommit(m_pConn,bAutoCommit);
	m_bAutoCommit = bAutoCommit;
}

bool CConnectionImpl::getAutoCommit()
{
	VALIDCONN;
	return m_bAutoCommit;
}

void CConnectionImpl::setTransactionIsolation(int nLevel)
{
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"Not Support");
}

bool CConnectionImpl::begin()
{
	VALIDCONN;
	return ( 0 == mysql_real_query(m_pConn,"begin",sizeof("begin")) );
}

int CConnectionImpl::getErrNo()
{
	//VALIDCONN;
	return mysql_errno(m_pConn);
}

const char* CConnectionImpl::getError()
{
	//VALIDCONN;
	return mysql_error(m_pConn);
}

CAutoPtr<CStatement> CConnectionImpl::createStatement()
{
	return new CRefStatement(*new CStatementImpl(*this));
}

CAutoPtr<CStatement> CConnectionImpl::prepareStatement(const char* pSQL)
{
	CStatementImpl* pstmt = new CStatementImpl(*this);
	CAutoPtr<CStatement> stmt = new CRefStatement(*pstmt);
	stmt->prepare(pSQL);
	return stmt;
}

CAutoPtr<CCallableStatement> CConnectionImpl::prepareCall(const char* pSQL)
{
	/*
	CAutoPtr<CCallableStatement> stmt = new CCallableStatementImpl(*this);
	stmt->prepare(pSQL);
	return stmt;
	*/
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"Not Support");
	return NULL;
}

