#include <public.h>
#include <db/sqlexception.h>
#include <util/datetime.h>
#include <util/thread.h>
#include <util/baselock.h>
#include <db/dbpublic.h>
#include <db/poolconnautoptr.h>
#include <db/connection.h>
#include <db/connectionpool.h>
#include <db/poolconnection.h>
#include <db/statement.h>
#include <db/callablestatement.h>
//
CPoolConnection::CPoolConnection(CConnection* pConnection,CConnectionPool* pConnectionPool)
:m_pConnection(pConnection),m_pConnectionPool(pConnectionPool),m_bIsIdle(true)
{
	
}

void CPoolConnection::putConnection()
{
	if (!this->m_bIsIdle)
	{
		try
		{
			//如果rollback失败，则断开连接，让服务器端进行回滚。
			if ( !this->m_pConnection->rollback() )
			{
				m_pConnection->close();
			}
		}
		catch (exception& e)
		{  }
		this->m_pConnectionPool->putConnection(this);	
	}
}

void CPoolConnection::close()
{
	//printf("CPoolConnection::close");
	m_pConnection->close();
}

CPoolConnection::~CPoolConnection()
{
	if (this->m_pConnection!=NULL)
	{
		delete this->m_pConnection;
	}
}


CAutoPtr <CStatement> CPoolConnection::createStatement()
{
	return this->m_pConnection->createStatement();	
}


CAutoPtr<CStatement> CPoolConnection::prepareStatement(const char* pSQL)
{	
	return this->m_pConnection->prepareStatement(pSQL);	
}


CAutoPtr< CCallableStatement > CPoolConnection::prepareCall(const char* pSQL)
{	
	return this->m_pConnection->prepareCall(pSQL);	
}


void CPoolConnection::setAutoCommit(bool AutoCommit)
{	
	this->m_pConnection->setAutoCommit(AutoCommit);
}


bool CPoolConnection::getAutoCommit()
{
	return this->m_pConnection->getAutoCommit();
}


bool CPoolConnection::begin()
{	
	if (!this->m_bIsIdle)
		return this->m_pConnection->begin();
	return false;
}


bool CPoolConnection::commit()
{
	return this->m_pConnection->commit();
}


bool CPoolConnection::rollback()
{
	//for test only
	//return false;
	return this->m_pConnection->rollback();
}


bool CPoolConnection::connect()
{
	return this->m_pConnection->connect();
}

bool CPoolConnection::isClosed()
{
	if (!this->m_bIsIdle)
		return this->m_pConnection->isClosed();
	else
		return true;
}


void CPoolConnection::setTransactionIsolation(int nLevel)
{
	this->m_pConnection->setTransactionIsolation(nLevel);
}

void CPoolConnection::keepActive()
{
	this->m_pConnection->keepActive();
}

CAutoPtr<CConnection> CPoolConnection::clone()
{
	throw CSQLException(ERROR_DATABASE_UNSUPPORT,"CPoolConnection::clone is not supported");
}

