#include "../public.h"
#include "../thread/thread.h"
#include "../lock//baselock.h"
#include "dbpublic.h"
#include "sqlexception.h"
#include "poolconnautoptr.h"
#include "connection.h"
#include "connectionmanager.h"
#include "poolconnection.h"
#include "connectionpool.h"

/*
#include <public.h>
#include <util/thread.h>
#include <util/baselock.h>
#include <db/dbpublic.h>
#include <db/sqlexception.h>
#include <db/poolconnautoptr.h>
#include <db/connection.h>
#include <db/connectionmanager.h>
#include <db/poolconnection.h>
#include <db/connectionpool.h>
*/
//
CConnectionPool::CConnectionPool(const char* pURL, int nMaxConnections,const char* pUsr,const char* pUsrPwd)
:m_nMaxConnections(nMaxConnections),m_nConnectionCount(0),m_nIdleConnectionCount(0),
m_Url(pURL),m_Usr(pUsr),m_UsrPwd(pUsrPwd),m_nLastConnID(0)
{	
	m_pConnections = new CPoolConnection*[nMaxConnections];
	for ( unsigned short i = 0 ; i < nMaxConnections ; i ++ )
	{
		m_pConnections[i] = NULL;
	}
	m_nMaxConnections = nMaxConnections;
	m_nConnectionCount = 0;
	m_nIdleConnectionCount= 0;
	this->m_BaseLock = new CBaseLock();
	m_BaseLock->initialize();
}

//
CConnectionPool::~CConnectionPool()
{
	for ( unsigned short i = 0 ; i < this->m_nMaxConnections; i ++ )
	{
		if ( m_pConnections[i] != NULL )
			delete m_pConnections[i];
	}
	delete[] m_pConnections;

	if (this->m_BaseLock != NULL)
	{
		delete m_BaseLock;
	}
}

//
void CConnectionPool::putConnection(CPoolConnection* pConn)
{
	CAutoLock lock(*this->m_BaseLock);

	if ( ( pConn->m_pConnectionPool == this )&&(pConn->m_bIsIdle==false) )
	{
		pConn->m_bIsIdle = true;		
		m_nIdleConnectionCount ++;
		pConn->m_nOwnThreadId = 0;
		this->m_BaseLock->notifyAll();	
	}	
}

const CPoolConnection* CConnectionPool::getIdleConnection()
{
	CPoolConnection* pPoolConn = NULL;
	if ( m_nIdleConnectionCount > 0 )
	{
		/*
		for ( unsigned short i = 0 ; i < this->m_nConnectionCount ; i ++ )
		{
			pPoolConn = m_pConnections[i];
			if ((pPoolConn==NULL)||( !pPoolConn->m_bIsIdle ))
				continue;
			pPoolConn->m_bIsIdle = false;
			m_nIdleConnectionCount--;			
			break;
		}
		*/

		int count=0;

		int i=m_nLastConnID+1;
		if (i==m_nConnectionCount)
		{
			i=0;
		}		

		pPoolConn = m_pConnections[i];

		//轮流法获取空闲连接，保证连接池中的连接被轮流使用
		while((pPoolConn==NULL)||(!pPoolConn->m_bIsIdle))
		{	
			//printf("getIdleConn m_nLastConnID=%d i=%d\n",m_nLastConnID,i);

			i++;
			count++;

			//当达到最大连接下标，从0开始找空闲连接
			if (i==m_nConnectionCount)
			{
				i=0;
			}

			//避免本方法外的bug导致的死循环，所有的连接都被查过后，还是没找到可用连接则跳出循环
			if (count>m_nConnectionCount)
			{
				break;
			}
			
			pPoolConn = m_pConnections[i];
		}

		//获取到空闲的连接
		if (pPoolConn!=NULL)
		{
			pPoolConn->m_bIsIdle = false;
			m_nIdleConnectionCount--;
			m_nLastConnID = i;
			
			//printf("%s m_nLastConnID=%d\n","getIdleConn",m_nLastConnID);
		}
	}

	//设置为自动commit
	if ( pPoolConn != NULL )
	{
		if( !pPoolConn->isClosed() )
			pPoolConn->setAutoCommit(true);
	}

	return pPoolConn;
}

//
CPoolConnAutoPtr<CPoolConnection> CConnectionPool::getConnection(long nWaitMillis)
{
	long tmp =0;
	bool bHasIdleConn=false;
	CPoolConnection* pPoolConn = NULL;
	
	{
		CAutoLock lock(*this->m_BaseLock);		
		pPoolConn = (CPoolConnection*)getIdleConnection();
		if ( pPoolConn == NULL )
		{
			//无连接则等待
			if ((this->m_nConnectionCount >= this->m_nMaxConnections) && nWaitMillis)
			{
				//printf("wait for conn\n");
				while ( (pPoolConn == NULL)||(0<nWaitMillis)||(tmp < nWaitMillis) )
				{					
					if (nWaitMillis>0)
					{
						//printf("%s ThreadId=%d tmp=%d nWaitMillis=%d\n","nWaitMillis>0",CThread::getCurrentThreadId(),tmp,nWaitMillis);
						//nWaitMillis>0,超时不等待，每半秒取一次空闲连接
						tmp += 500;
						this->m_BaseLock->wait(500);
					}
					else
					{
						//nWaitMillis<0,无限等待，每秒取一次空闲连接
						this->m_BaseLock->wait(1000);
					}

					pPoolConn = (CPoolConnection*)getIdleConnection();

					if (pPoolConn != NULL)
					{
						//printf("wait but got idle conn\n");
						bHasIdleConn = true;
						break;
						//CPoolConnAutoPtr<CPoolConnection> poolconn(pPoolConn);
						//this->m_BaseLock->unlock();	
						//return poolconn;
					}

					if ((tmp >= nWaitMillis)&&(nWaitMillis>0))
					{
						break;
					}
				}

				if ((tmp >= nWaitMillis)&&(!bHasIdleConn))
				{
					//this->m_BaseLock->unlock();	
					throw CSQLException(ERROR_DATABASE_NOTCONNECT,"timeout when getConnection by CConnectionPool!");
				}
			}
			//无连接不等待
			else 
			{
				//连接池未满，创建一条新的连接
				if ( this->m_nConnectionCount < this->m_nMaxConnections )
				{
					try
					{
						CAutoPtr<CConnection> ptr = CConnectionManager::getConnection(this->m_Url.c_str(),this->m_Usr.c_str(),this->m_UsrPwd.c_str());
						CConnection* pConn = ptr.release();					
						pPoolConn = new CPoolConnection(pConn,this);
						m_pConnections[m_nConnectionCount] = pPoolConn;
						m_nConnectionCount ++;
						pPoolConn->m_bIsIdle = false;				
					}
					catch (CSQLException& e)
					{
						//this->m_BaseLock->unlock();
						string errmsg = e.what();
						errmsg = "getConnection by CConnectionPool error! " + errmsg;
						throw CSQLException(ERROR_DATABASE_NOTCONNECT,errmsg);
					}
					
					//printf("getConn m_nConnectionCount=%d\n",m_nConnectionCount);
					CPoolConnAutoPtr<CPoolConnection> poolconn(pPoolConn);
					//this->m_BaseLock->unlock();	
					return poolconn;					
				}
				else
				{
					//连接池已满				
					throw CSQLException(ERROR_DATABASE_POOLFULL,"getConnection by CConnectionPool error! no connection available!");
				}
			}
		}
		//有可用连接
		else
		{
			bHasIdleConn = true;		
		}
	}

	if (bHasIdleConn)
	{
		//检查数据库连接是否在活动状态，不是的话需要尝试重连，重连不成功则需要做取出连接的反操作
		try
		{
			if (pPoolConn->isClosed())
			{
				pPoolConn->close();
				pPoolConn->connect();
			}	
		}
		catch (CSQLException& )
		{
			pPoolConn->putConnection();
			throw;
			return NULL;
		}

		CPoolConnAutoPtr<CPoolConnection> poolconn(pPoolConn);
		return poolconn;
	}	
	
	return NULL;
}

void CConnectionPool::keepActive()
{
	//最小连接数判断
	{
		
	}

	//心跳处理
	if ( m_nIdleConnectionCount > 0 )
	{
		CPoolConnection* pPoolConn = NULL;
		for ( unsigned short i = 0 ; i < this->m_nConnectionCount ; i ++ )
		{
			//取出空闲连接
			{
				CAutoLock lock(*this->m_BaseLock);	
			
				pPoolConn = m_pConnections[i];
				if ((pPoolConn==NULL)||( !pPoolConn->m_bIsIdle ))
				{
					continue;
				}
				else
				{
					pPoolConn = (CPoolConnection*)getIdleConnection();		
				}
			}
			try
			{
				//已断开连接则重连，否则发送心跳包
				if ( pPoolConn->isClosed() )
					pPoolConn->connect();
				else
					pPoolConn->keepActive();
				//放回空闲连接
				pPoolConn->putConnection();
			}
			catch (exception& e)
			{	}
		}
	}
}

