#include <string>
#include <util/stringfun.h>
#include <util/lightlock.h>
#include "odbcpublic.h"
#include "odbcstatement.h"
#include "odbcconnection.h"
#include "odbccallablestatement.h"
using namespace my_odbc;

//#define _DECLARE_MSSQL_

SQLHENV COdbcConnection::m_pEven = SQL_NULL_HENV; 
int COdbcConnection::m_ConnCount = 0;
CLightLock COdbcConnection::m_CountLock;

CAutoPtr<CConnection> CConnectionManager::getConnection(const char* pConnectionURL,
												const char* UserName/* ="" */,
												const char* UserPwd/* ="" */)
{
	CAutoPtr<CConnection> conn( 
					new COdbcConnection( pConnectionURL , 1743 , 
					UserName , UserPwd ) 
					);
	conn->connect();
	return conn;
}

void parseOdbcProtocol( vector<string>& Params,const char* pConnectionURL)
{
	const char* p,*begin;;
	begin = p = pConnectionURL;
	while ( *p )
	{
		if ( ':' == (*p) || '/' == (*p) )
		{
			//
			if ( p - begin > 0 )
			{
				Params.push_back(string(begin,p-begin));
			}
			begin = p + 1;
		}
		p++;
	}
	if ( p - begin > 1 )
	{
		Params.push_back(string(begin,p-begin));
	}
}

#define VALIDCONN if ( SQL_NULL_HENV == m_pConn || ! m_bConnected ) \
				throw CSQLException(ERROR_DATABASE_INVALIDOPER,"Connection Not Connected");

COdbcConnection::COdbcConnection()
	:m_ConnStr(""),m_nPort(1743),m_UserName(""),
				m_PassWord(""),m_pConn(SQL_NULL_HDBC),m_bConnected(false),
				m_bAutoCommit(true)
{
}

COdbcConnection::COdbcConnection(const std::string& ConnStr,unsigned short Port ,
								 const std::string& UserName ,const std::string& Password )
				:m_ConnStr(ConnStr),m_nPort(1743),m_UserName(UserName),
				m_PassWord(Password),m_pConn(SQL_NULL_HDBC),m_bConnected(false),
				m_bAutoCommit(true)
{

}


COdbcConnection::~COdbcConnection()
{
	try
	{
		close();
	}
	catch(...)
	{
	}
}

CAutoPtr<CConnection> COdbcConnection::clone()
{
	if (this->m_ConnStr=="")
	{
		throw CSQLException(ERROR_DATABASE_INVALIDPROTOCOL, 
					"Connection param error while calling CConnectionImpl::clone");  
	}
	CAutoPtr<CConnection> conn( new COdbcConnection(m_ConnStr.c_str(),
						m_nPort,m_UserName.c_str(),m_PassWord.c_str()) );
	conn->connect();
	return conn;
}

void COdbcConnection::close()
{
	if( !m_bConnected )
		return;
	if ( m_pConn != SQL_NULL_HENV )
	{
		//rollback();
		SQLDisconnect(m_pConn);
		SQLFreeConnect(m_pConn);
		m_pConn = SQL_NULL_HENV;	
	}
	m_bConnected = false;
	CAutoLightLock alock( m_CountLock );
	m_ConnCount --;
	if( 0 == m_ConnCount )
	{
		COdbcConnection::finalize();
	}
}

void COdbcConnection::keepActive()
{
	//return true;
}

bool COdbcConnection::commit()
{
	VALIDCONN;
	SQLRETURN nRet = SQLEndTran( SQL_HANDLE_DBC , m_pConn , SQL_COMMIT );
	CHECK_CONN_AND_THROW( nRet , m_pConn , "commit error" );
	setAutoCommit(true);
	return false;
}

bool COdbcConnection::rollback()
{
	VALIDCONN;
	SQLRETURN nRet = SQLEndTran( SQL_HANDLE_DBC , m_pConn , SQL_ROLLBACK );
	CHECK_CONN_AND_THROW( nRet , m_pConn , "SQLEndTran Error" );
	setAutoCommit(true);
	return true;
}

bool COdbcConnection::connect()
{
	close();
	{
		CAutoLightLock aLock( m_CountLock );
		if( 0 == m_ConnCount )
		{
			COdbcConnection::initialize();
		}
		m_ConnCount ++ ;
	}
	SQLRETURN nRet = SQLAllocHandle(SQL_HANDLE_DBC,m_pEven,&m_pConn); 
	CHECK_EVN_AND_THROW( nRet ,  m_pEven , "create conn error");
	
#ifndef _DECLARE_MSSQL_
	nRet =SQLConnect(m_pConn, (uchar_t *)m_ConnStr.c_str() , (SWORD)m_ConnStr.length(),
					(uchar_t *)m_UserName.c_str(), (SWORD)m_UserName.length(),
					(uchar_t *)m_PassWord.c_str(), (SWORD)m_PassWord.length()
				  );
#else
	SQLCHAR szOutConn[255];
	SQLSMALLINT   nConnTimeOut = 0;
	nRet = SQLDriverConnect(m_pConn, NULL,
		(SQLTCHAR*) m_ConnStr.c_str() , SQL_NTS , 
		szOutConn	,	sizeof( szOutConn ), 
		&nConnTimeOut, SQL_DRIVER_COMPLETE
		);
#endif
	//检查连接是否成功
	try
	{
		CHECK_CONN_AND_THROW( nRet ,  m_pConn , "conncet error");
	}
	catch(...)
	{
		SQLFreeConnect(m_pConn);
		//释放资源
		CAutoLightLock aLock( m_CountLock );
		if( 0 == --m_ConnCount )
		{
			COdbcConnection::finalize();
		}
		throw;
	}
	nRet = SQLSetConnectAttr( m_pConn , 
				SQL_ATTR_TXN_ISOLATION  , 
				(SQLPOINTER)SQL_TXN_READ_UNCOMMITTED ,
				0 );
	//设置连接属性
	try
	{
		CHECK_CONN_AND_THROW( nRet ,  m_pConn , "conncet error");
	}
	catch(...)
	{
		//释放资源
		SQLFreeConnect(m_pConn);
		CAutoLightLock aLock( m_CountLock );
		if( 0 == --m_ConnCount )
		{
			COdbcConnection::finalize();
		}
		throw;
	}
	m_bConnected = true;
	return true;
}
bool COdbcConnection::reconnect()
{
	return connect();
}
bool COdbcConnection::isClosed()
{
	return !m_bConnected;
}

void COdbcConnection::setAutoCommit(bool bAutoCommit)
{
	VALIDCONN;
	long nCommitType = 0;
	if( bAutoCommit )
	{
		nCommitType = SQL_AUTOCOMMIT_ON;
	}
	else
	{
		nCommitType = SQL_AUTOCOMMIT_OFF;
	}
	SQLRETURN nRet = SQLSetConnectAttr( m_pConn , SQL_ATTR_AUTOCOMMIT , (SQLPOINTER)nCommitType , 0 );
	CHECK_CONN_AND_THROW( nRet ,  m_pConn , "setAutoCommit error");
	m_bAutoCommit = bAutoCommit;
}

bool COdbcConnection::getAutoCommit()
{
	VALIDCONN;
	return m_bAutoCommit;
}

void COdbcConnection::setTransactionIsolation(int nLevel)
{
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"Not Support");
}

bool COdbcConnection::begin()
{
	setAutoCommit(false);
	return true;
}

int COdbcConnection::getErrNo() const
{
	VALIDCONN;
	return GetLastError();
}

const char* COdbcConnection::getError() const
{
	VALIDCONN;
	return "";
}

CAutoPtr<CStatement> COdbcConnection::createStatement()
{
	CAutoPtr<CStatement> stmt(new COdbcStatement(*this));
	return stmt;
}

CAutoPtr<CStatement> COdbcConnection::prepareStatement(const char* pSQL)
{
	CAutoPtr<CStatement> stmt(new COdbcStatement(*this));
	stmt->prepare(pSQL);
	return stmt;
}

CAutoPtr<CCallableStatement> COdbcConnection::prepareCall(const char* pSQL)
{
	CAutoPtr<CCallableStatement> stmt( new COdbcCallableStatement(*this) );
	//string strSql= StrReplaceAddIndex( pSQL , "?" , ":p");	
	stmt->prepare( pSQL );
	return stmt;
}

bool COdbcConnection::setEncoding(const char * pEncode/* = "gb2312"*/)
{
	VALIDCONN;
//	return mysql_set_character_set(m_pConn, pEncode);
	return false;
}

bool COdbcConnection::setConnectString(const char * pConnString , const char * pUserName , const char * pUserPwd)
{
//	vector<string> params;
//	parseOdbcProtocol(params,pConnString);
//	if ( params.size() < 2 )
//		throw CSQLException(ERROR_DATABASE_INVALIDPROTOCOL,"Unknow Protocol");
//	vector<string>::iterator iparam = params.begin();
//	if ( stricmp((*iparam++).c_str(),"odbc") != 0 )
//		throw CSQLException(ERROR_DATABASE_INVALIDPROTOCOL,"Unknow Protocol");
	m_ConnStr = pConnString;
	m_UserName = pUserName;
	m_PassWord = pUserPwd;
	return true;
}

void COdbcConnection::initialize()
{
	if( SQL_NULL_HENV != m_pEven )
		return ;
	SQLRETURN nRet = 0;
	nRet = SQLAllocHandle(SQL_HANDLE_ENV,NULL, &m_pEven); 
	CHECK_EVN_AND_THROW( nRet ,  m_pEven , "initialize ENV error");

	//通知odbc驱动程序管理器，这是一个基于odbc3.0版本的应用程序 
	nRet = SQLSetEnvAttr(m_pEven,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,SQL_IS_INTEGER);
	//nRet = SQLSetEnvAttr(m_pEven, SQL_ATTR_CONNECTION_POOLING,(SQLPOINTER)SQL_CP_ONE_PER_DRIVER,SQL_IS_INTEGER);
	CHECK_EVN_AND_THROW( nRet ,  m_pEven , "initialize ENV error");

	//分配得到连接句柄，并由此和数据源进行连接 
	//CConnectionManager::registProtocol("odbc",new COdbcConnection);
}
void COdbcConnection::finalize()
{
	if( SQL_NULL_HENV == m_pEven )
		return ;
	SQLRETURN nRet = SQLFreeHandle(SQL_HANDLE_ENV,m_pEven);
	m_pEven = SQL_NULL_HENV;
	CHECK_EVN_AND_THROW( nRet ,  m_pEven , "finalize ENV error");
}
