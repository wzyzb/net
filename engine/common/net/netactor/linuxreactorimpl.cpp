#include <public.h>

#ifndef WINDOWS
#include <net/netactor/unix/linuxreactorimpl.h>
#include <net/netactor/readresult.h>
#include <net/netactor/writeresult.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <net/netactor/unix/linuxreactorconnectionimpl.h>
#include <net/netactor/unix/linuxreactorconnectorimpl.h>
#include <assert.h>
#include <io/bytesinputstream.h>
#include <util/datetime.h>

#define MAX_FDS      1024
#define RECV_BUFSIZE 128*1024
#ifdef _LINUXREACTORDEBUG
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT
#endif

CLinuxReactorImpl::CLinuxReactorImpl() 
{
    m_EpollWriteFd = 0;
    m_EpollReadFd = 0;
    memset(m_events, 0x00, sizeof(m_events));
}

CLinuxReactorImpl::~CLinuxReactorImpl()
{

}

int32 CLinuxReactorImpl::init()
{
	m_EpollReadFd = epoll_create(MAX_FDS);
	if ( -1 == m_EpollReadFd )
	{        
		return errno;
	}
	m_EpollWriteFd = epoll_create(MAX_FDS);
	if ( -1 == m_EpollWriteFd )
	{        
		return errno;
	}
	m_pThread = new THREAD_TYPE(this,&CLinuxReactorImpl::sendRun);
	m_pThread->init();
	return 0;
}

void CLinuxReactorImpl::sendRun(CLinuxReactorImpl* pPointer)
{
	struct epoll_event events[MAX_FDS],ev;
	while ( m_isLoop )
	{
		int nfds = epoll_wait(m_EpollWriteFd,events,MAX_FDS,500);
		if ( nfds == -1 )
		{
			if ( errno == EINTR )
				continue;
			break;
		}
		for ( int i = 0 ; i < nfds ; i ++ )
		{
			CEpollBindDataPtr pbinddata = NULL;
			IConnectionPtr pconnection = NULL;
			ev = events[i];
			{
				CAutoLock lock(m_BindDataLock);
				std::map<SOCKET, CEpollBindData*>::iterator iter = m_mapBindData.find(events[i].data.fd);
				if ( iter == m_mapBindData.end() )
				{
					DEBUG_PRINT("epoll_send|error|can't find binddata fd[%d]\n",ev.data.fd);
					epoll_ctl(m_EpollWriteFd, EPOLL_CTL_DEL, ev.data.fd, &ev);
					continue;
				}
				pbinddata = iter->second;
				pconnection = pbinddata->pConnection;
			}
			if ( pconnection == NULL )
			{
				DEBUG_PRINT("epoll_send|error|connection is null");
				epoll_ctl(m_EpollWriteFd, EPOLL_CTL_DEL, ev.data.fd, &ev);
				continue;
			}

			if( events[i].events & EPOLLOUT )
			{
				IConnectionPtr pconn = dynamic_cast<CLinuxReactorConnectionImpl*>(pconnection.get())->getRealConnection();

				bool issendagain = false;
				uint32 totalsendsize = 0;
				CAutoLock lock(pbinddata->m_SendBufLock);
				CBytesInputStream stream(pbinddata->SendBuffer->c_str(),pbinddata->SendBuffer->getDataSize());
				stream.setStackObject();
				//DEBUG_PRINT("%s:epoll_send|send|begin size=%d\n",CDateTime().asString("YYYY-MM-DD hh:mm:ss.nnn").c_str(),pbinddata->SendBuffer->getDataSize());
				while( stream.getDataSize() > 0  )
				{
					// 最多一次发 1024
					uint32 sendsize  = stream.getDataSize() > 1024 ? 1024 : stream.getDataSize();
					int errorno = pconn->getOutputStream()->writeBytes( stream.c_str(), sendsize );
					if ( errorno == 0 )
					{
						//发送成功
						totalsendsize += sendsize;
						stream.popBytes(sendsize);
						continue;
					}
					if( errorno == EAGAIN )
					{
						//发送失败，错误码为需要重试
						issendagain = true;
						DEBUG_PRINT("epoll_send|error|WriteMessage Error[%s] handle[%d]\n",strerror(errorno),pconn->getHandle());
						break;
					}
					break;
				} // while
				if ( totalsendsize > 0 )
				{
					pbinddata->SendBuffer->popBytes(totalsendsize);
				}
				//DEBUG_PRINT("%s:epoll_send|send|finish sendedsize=%d\n",CDateTime().asString("YYYY-MM-DD hh:mm:ss.nnn").c_str(),totalsendsize);

				if ( issendagain )
				{
					//ev.events = EPOLLOUT;
					//epoll_ctl(m_EpollWriteFd, EPOLL_CTL_MOD, ev.data.fd, &ev);
				}
				else
				{
					ev.events = 0;
					epoll_ctl(m_EpollWriteFd, EPOLL_CTL_MOD, ev.data.fd, &ev);
				}
				continue;
			}
			if ( events[i].events & EPOLLERR )
			{
				DEBUG_PRINT("epoll_send|error|catch epollerr event binddata[%d]\n",events[i].data.fd);
				struct epoll_event ev;
				ev.events = 0;
				epoll_ctl(m_EpollWriteFd, EPOLL_CTL_DEL, ev.data.fd, &ev);
				continue;
			}
			if ( events[i].events & EPOLLHUP )
			{
				DEBUG_PRINT("epoll_send|error|catch epollhup event binddata[%d]\n",events[i].data.fd);
				ev.events = 0;
				epoll_ctl(m_EpollWriteFd, EPOLL_CTL_DEL, ev.data.fd, &ev);
				continue;
			}
		}
	}
}

int32 CLinuxReactorImpl::run()
{
	char recvbuf[RECV_BUFSIZE] = {0};

	m_isLoop = true;
	struct epoll_event ev;
	//启动发送线程
	m_pThread->start();
	while ( m_isLoop )
	{
		//epoll wait 3秒
		int nfds = epoll_wait(m_EpollReadFd, m_events, MAX_FDS, 3000);
		if( nfds == -1 )
		{
			if( errno == EINTR )
				continue;
			stop();
			return errno;
		}

		for( int i = 0; i < nfds; i++ )
		{
			CEpollBindDataPtr pbinddata = NULL;
			ev = m_events[i];
			{
				CAutoLock lock(m_BindDataLock);
				std::map<SOCKET, CEpollBindData*>::iterator iter = m_mapBindData.find(m_events[i].data.fd);
				if ( iter == m_mapBindData.end() )
				{
					DEBUG_PRINT("epoll_read|error|can't find binddata fd[%d]\n",ev.data.fd);
					epoll_ctl(m_EpollReadFd, EPOLL_CTL_DEL, ev.data.fd, &ev);
					continue;
				}
				pbinddata = iter->second;
			}

			//Listen Socket
			if ( pbinddata->BindDataType == CEpollBindData::ACCEPTOR_TYPE )
			{
				if ( pbinddata->nNetHandleMask & NETHANDLER_MASK_ACCEPT )
				{
					IAcceptorPtr pAcceptor = pbinddata->pAcceptor;
		            
					// accept
					if( pAcceptor == NULL )
					{
						CNetResult Result;
						Result.setCode(-2);
						Result.setConnection(NULL);
						Result.setConnector(NULL);
						Result.setAcceptor(NULL);
						Result.setHandle(0); 
						pbinddata->pHandler->onAccept(Result, pbinddata->pParam);
					}
					else
					{
						CNetResult Result;
						Result.setCode(0);
						Result.setConnection(NULL);
						Result.setConnector(NULL);
						Result.setAcceptor(pAcceptor);
						Result.setHandle(0);
						pbinddata->pHandler->onAccept(Result, pbinddata->pParam);
					}
				}
				continue;
			}
			else if ( pbinddata->BindDataType == CEpollBindData::CONNECTION_TYPE )
			{
				if ( pbinddata->pConnection == NULL )
				{
					//如果已经没有连接，代表连已断开，不再做处理
					DEBUG_PRINT("epoll_wait|error|binddata[%08X]'s Connection is NULL io events[%d]\n",pbinddata.get(),m_events[i].events);
					continue;
				}
				if( m_events[i].events & EPOLLIN )
				{
					//读操作
					IConnectionPtr pconn = pbinddata->pConnection;
		            
					uint32 readbytes = sizeof(recvbuf);
					int errorno = pconn->getInputStream()->readBytes( recvbuf, readbytes );
					if ( errorno == EAGAIN )
					{
						continue;
					}
					if ( errorno != 0 || readbytes == 0 )
					{
						//连接断开
						unregistry( pconn );
						if ( pbinddata->nNetHandleMask & NETHANDLER_MASK_CLOSE )
						{
							CNetResult Result;
							Result.setCode(errno);
							Result.setConnection( pconn );
							Result.setConnector(pbinddata->pRawConnector);
							Result.setAcceptor(NULL);
							Result.setHandle(0);
							pbinddata->pHandler->onClose(Result, pbinddata->pParam);
						}
						DEBUG_PRINT("epoll_wait|Socket Close SOCKET:%d", pconn->getHandle());
						continue;
					}
					if ( pbinddata->nNetHandleMask & NETHANDLER_MASK_READ )
					{
						//成功读取数据
						CReadResult Result;
						Result.setCode(0);
						Result.setConnection(pconn);
						Result.setConnector(pbinddata->pRawConnector);
						Result.setAcceptor(NULL);
						Result.setHandle(0);
						Result.setData(recvbuf,readbytes);
						pbinddata->pHandler->onRead(Result, pbinddata->pParam);
					}
				}
				else if( m_events[i].events & EPOLLHUP ||  m_events[i].events & EPOLLERR )
				{
					//连接断开
					DEBUG_PRINT("epoll_wait|error|Socket Close SOCKET:%d", pbinddata->pConnection->getHandle());
					IConnectionPtr pconn = pbinddata->pConnection;
					unregistry( pbinddata->pConnection ); 

					if ( ( pbinddata->nNetHandleMask) & NETHANDLER_MASK_CLOSE )
					{
						CNetResult Result;
						Result.setCode(errno);
						Result.setConnection( pconn );
						Result.setConnector(pbinddata->pRawConnector);
						Result.setAcceptor(NULL);
						Result.setHandle(0);
						pbinddata->pHandler->onClose(Result, pbinddata->pParam);
					}
					continue;
				}
				if( m_events[i].events & EPOLLOUT )
				{
					IConnectionPtr pconn = ((CLinuxReactorConnectionImpl*)pbinddata->pConnection.get())->getRealConnection();

					bool issendagain = false;
					uint32 totalsendsize = 0;
					CAutoLock lock(pbinddata->m_SendBufLock);
					CBytesInputStream stream(pbinddata->SendBuffer->c_str(),pbinddata->SendBuffer->getDataSize());
					stream.setStackObject();
					while( stream.getDataSize() > 0  )
					{
						// 最多一次发 1024
						uint32 sendsize  = stream.getDataSize() > 1024 ? 1024 : stream.getDataSize();
						int errorno = pconn->getOutputStream()->writeBytes( stream.c_str(), sendsize );
						if ( errorno == 0 )
						{
							//发送成功
							totalsendsize += sendsize;
							stream.popBytes(sendsize);
							continue;
						}
						if( errorno == EAGAIN )
						{
							//发送失败，错误码为需要重试
							issendagain = true;
							DEBUG_PRINT("epoll_send|error|Connection Closed on Write[%s] handle[%d]\n",strerror(errorno),pconn->getHandle());
							break;
						}
						break;
					} // while
					if ( totalsendsize > 0 )
					{
						pbinddata->SendBuffer->popBytes(totalsendsize);
					}

					if ( issendagain )
					{
						pbinddata->nEpollevents |= EPOLLOUT;
					}
					else
					{
						pbinddata->nEpollevents &= ~EPOLLOUT;
					}
					ev.events = pbinddata->nEpollevents;
					epoll_ctl(m_EpollReadFd, EPOLL_CTL_MOD,ev.data.fd,&ev);
				}
			}
			else if ( pbinddata->BindDataType == CEpollBindData::CONNECTOR_TYPE )
			{
				IConnectorPtr pconnector = pbinddata->pRawConnector;

				//处理完毕
				//从epoll中删除，当进行registry的时候，会再重新加入到epoll中
				ev.events = 0;
				epoll_ctl(m_EpollReadFd, EPOLL_CTL_DEL, ev.data.fd, &ev);

				int resultcode = -1;
				if ( m_events[i].events == EPOLLOUT )
				{
					resultcode = 0;
				}
				CNetResult Result;
				Result.setCode(resultcode);
				Result.setConnection(NULL);
				Result.setConnector( pbinddata->pRawConnector );
				Result.setAcceptor(NULL);
				Result.setHandle(0); 
				if ( pbinddata->nNetHandleMask & NETHANDLER_MASK_OPEN && pbinddata->pHandler != NULL )
				{
					//由于注册时需要重用Connector，所以在此需要先保存。在调用onopen后再删除。
					if ( resultcode != 0 )
					{
						CAutoLock lock(m_BindDataLock);
						map<SOCKET,CEpollBindData*>::iterator iter = m_mapBindData.find(pconnector->getHandle());
						if ( iter != m_mapBindData.end() )
							m_mapBindData.erase(iter);
					}
					pbinddata->pHandler->onOpen(Result,pbinddata->pParam);
				}
				if ( pbinddata->decRef() && resultcode == 0 )
				{
					CAutoLock lock(m_BindDataLock);
					map<SOCKET,CEpollBindData*>::iterator iter = m_mapBindData.find(pconnector->getHandle());
					if ( iter != m_mapBindData.end() )
						m_mapBindData.erase(iter);
				}
			}
		}
	}
    return 0;
}

int32 CLinuxReactorImpl::stop()
{
	m_isLoop = false;
	close(m_EpollReadFd);
	close(m_EpollWriteFd);

	CAutoLock lock(m_BindDataLock);
	std::map<SOCKET, CEpollBindData*>::iterator iter;
	for( iter = m_mapBindData.begin(); iter != m_mapBindData.end(); iter++ )
	{
		if ( iter->second->pConnection != NULL )
			iter->second->pConnection->close();
		iter->second->decRef();
	}
	m_mapBindData.clear();
	return 0;
}

int32 CLinuxReactorImpl::addAcceptor(IAcceptorPtr pAcceptor, INetHandlerPtr Handler, void* Param)
{
    if ( pAcceptor == NULL )
        return EINVAL;
	pAcceptor->setMode(ASYNC_MODE);

	CEpollBindDataPtr pbinddata = CEpollBindDataPtr::createInstance();
    if( NULL == pbinddata )
    {
        return ENOMEM;
    }

	pbinddata->BindDataType = CEpollBindData::ACCEPTOR_TYPE;
	pbinddata->pAcceptor = pAcceptor;
	pbinddata->pHandler = Handler;
	pbinddata->pParam = Param;
	pbinddata->nEpollevents = EPOLLIN | EPOLLHUP | EPOLLERR;
	pbinddata->nNetHandleMask = NETHANDLER_MASK_ACCEPT;

	struct epoll_event ev;
	ev.data.fd = pAcceptor->getHandle();
	ev.events   = pbinddata->nEpollevents;

	std::pair<std::map<SOCKET, CEpollBindData*>::iterator, bool> iter_ret;
	{
		CAutoLock lock(m_BindDataLock);
		iter_ret = m_mapBindData.insert(std::make_pair<SOCKET, CEpollBindData*>( pAcceptor->getHandle(), pbinddata.get() ));
		if( !iter_ret.second )
			return EINVAL;
	}
	if( -1 == epoll_ctl(m_EpollReadFd, EPOLL_CTL_ADD, pAcceptor->getHandle(), &ev) )
	{
		CAutoLock lock(m_BindDataLock);
		m_mapBindData.erase(iter_ret.first);
		return errno;
	}
	pbinddata->incRef();
	return 0;
}

int32 CLinuxReactorImpl::removeAcceptor(IAcceptorPtr pAcceptor)
{
	if ( pAcceptor == NULL )
		return EINVAL;

	SOCKET sock = pAcceptor->getHandle();
	DEBUG_PRINT("removeAcceptor|socket(%d)\n",sock);
	CEpollBindDataPtr pbinddata = NULL;
	{
		CAutoLock lock(m_BindDataLock);
		std::map<SOCKET, CEpollBindData*>::iterator iter_find = m_mapBindData.find( sock );
		if( iter_find == m_mapBindData.end() )
		{
			DEBUG_PRINT("removeAcceptor|error|can't find[socket(%d)] binddata\n",sock);
			return -1;
		}
		pbinddata = iter_find->second;
		m_mapBindData.erase( iter_find );
	}

	struct epoll_event ev;
	ev.data.fd = sock;
	ev.events   = 0;
	//从epoll中删除对此socket事件的处理
	epoll_ctl(m_EpollReadFd, EPOLL_CTL_DEL, sock, &ev);
	pAcceptor->close();
	pbinddata->decRef();
	return 0;
}

int32 CLinuxReactorImpl::addConnector(IConnectorPtr pConnector,INetHandlerPtr Handler, void* Param)
{
    if ( pConnector == NULL )
    {
        printf("CLinuxReactorImpl::add()|Connector Error:pConnector == NULL \n");
        return EINVAL;
    }
	pConnector->setMode(ASYNC_MODE);
	int32 retcode = pConnector->connect();
	if ( retcode != 0 )
		return retcode;

	CEpollBindDataPtr pbinddata = CEpollBindDataPtr::createInstance();
    if( NULL == pbinddata )
    {
        return ENOMEM;
    }

	pbinddata->BindDataType  = CEpollBindData::CONNECTOR_TYPE;
	pbinddata->pRawConnector = pConnector;
	pbinddata->pHandler      = Handler;
	pbinddata->pParam        = Param;
	pbinddata->nEpollevents  = EPOLLOUT | EPOLLHUP | EPOLLERR;
	pbinddata->nNetHandleMask         = NETHANDLER_MASK_OPEN;

	//加入全局的map中处理
	std::pair<std::map<SOCKET, CEpollBindData*>::iterator, bool> iter_ret;
	{
		CAutoLock lock(m_BindDataLock);
		iter_ret = m_mapBindData.insert(std::make_pair<SOCKET, CEpollBindData*>( pConnector->getHandle(), pbinddata.get() ));
		if ( !iter_ret.second )
		{
			return EINVAL;
		}
	}
	struct epoll_event ev;
	ev.data.fd = pConnector->getHandle();
	ev.events   = pbinddata->nEpollevents;
	pbinddata->incRef();
	//加入epoll
	if( -1 == epoll_ctl(m_EpollReadFd, EPOLL_CTL_ADD, ev.data.fd, &ev) )
	{
		CAutoLock lock(m_BindDataLock);
		m_mapBindData.erase(iter_ret.first);
		pbinddata->decRef();
		return errno;
	}
	return 0;
}

int32 CLinuxReactorImpl::removeConnector(IConnectorPtr pConnector)
{
	if ( pConnector == NULL )
		return EINVAL;
	IConnectionPtr pconn = pConnector->getConnection();
	if ( pconn != NULL )
		return unregistry(pconn);

	SOCKET sock = pConnector->getHandle();
	DEBUG_PRINT("removeConnector|socket(%d)\n",sock);
	CEpollBindDataPtr pbinddata = NULL;
	{
		CAutoLock lock(m_BindDataLock);
		std::map<SOCKET, CEpollBindData*>::iterator iter_find = m_mapBindData.find( sock );
		if( iter_find == m_mapBindData.end() )
		{
			DEBUG_PRINT("removeConnector|error|can't find[socket(%d)] binddata\n",sock);
			return -1;
		}
		pbinddata = iter_find->second;
		m_mapBindData.erase( iter_find );
	}

	struct epoll_event ev;
	ev.data.fd = sock;
	ev.events   = 0;
	//从epoll中删除对此socket事件的处理
	epoll_ctl(m_EpollReadFd, EPOLL_CTL_DEL, sock, &ev);
	pConnector->close();
	//解除双向引用
	pbinddata->pConnection = NULL;
	pbinddata->decRef();
	return 0;
}

int32 CLinuxReactorImpl::registry(IConnectionPtr& pConnection, INetHandlerPtr Handler, uint32 nMask, void* Param)
{
	if( NULL == pConnection )
		return EINVAL;
	CEpollBindDataPtr pbinddata = NULL;
	map<SOCKET,CEpollBindData*>::iterator i;
	{
		CAutoLock lock(m_BindDataLock);
		i= m_mapBindData.find(pConnection->getHandle());
		//重用binddata，可以获取connector的引用
		if ( i == m_mapBindData.end() )
		{
			pbinddata = CEpollBindDataPtr::createInstance();
			if( pbinddata == NULL )
				return ENOMEM;
			std::pair<std::map<SOCKET, CEpollBindData*>::iterator, bool> iter_ret;
			iter_ret = m_mapBindData.insert(std::make_pair<SOCKET, CEpollBindData*>( pConnection->getHandle(), pbinddata.get() ));
			if( !iter_ret.second )
				return EINVAL;
			i = iter_ret.first;
		}
		else
		{
			pbinddata = (*i).second;
		}
	}
	
	pbinddata->Sock = pConnection->getHandle();
	pbinddata->BindDataType = CEpollBindData::CONNECTION_TYPE;
	pConnection = pbinddata->pConnection  = new CLinuxReactorConnectionImpl(this, pbinddata.get(), pConnection);
	pbinddata->pHandler = Handler;
	pbinddata->pParam = Param;
	pbinddata->nEpollevents = EPOLLIN | EPOLLHUP | EPOLLERR;
	pbinddata->nNetHandleMask = nMask;

	//加入EPOLL
	struct epoll_event ev;
	ev.data.fd = pbinddata->Sock;
	ev.events   = pbinddata->nEpollevents;
	pbinddata->incRef();
	//读取数据的epoll
	if( -1 == epoll_ctl(m_EpollReadFd, EPOLL_CTL_ADD, pbinddata->Sock, &ev) )
	{
		CAutoLock lock(m_BindDataLock);
		m_mapBindData.erase(i);
		pbinddata->decRef();
		return errno;
	}
	
	//写数据的epoll
	ev.events = 0;
	if( -1 == epoll_ctl(m_EpollWriteFd, EPOLL_CTL_ADD, pbinddata->Sock, &ev) )
	{
		CAutoLock lock(m_BindDataLock);
		m_mapBindData.erase(i);
		pbinddata->decRef();
		return errno;
	}
	DEBUG_PRINT("registry|handle[%d]|binddata[%08X]\n",pConnection->getHandle(),pbinddata.get());
	return 0;
}

int32 CLinuxReactorImpl::unregistry(IConnectionPtr& pConnection)
{
	if ( NULL == pConnection )
		return 0;
	SOCKET sock = pConnection->getHandle();
	DEBUG_PRINT("unregistry|socket(%d)\n",sock);
	CEpollBindDataPtr pbinddata = NULL;
	{
		CAutoLock lock(m_BindDataLock);
		std::map<SOCKET, CEpollBindData*>::iterator iter_find = m_mapBindData.find( sock );
		if( iter_find == m_mapBindData.end() )
		{
			DEBUG_PRINT("unregistry|error|can't find[socket(%d)] binddata\n",sock);
			return -1;
		}
		pbinddata = iter_find->second;
		m_mapBindData.erase( iter_find );
	}

	struct epoll_event ev;
	ev.data.fd = sock;
	ev.events   = 0;
	//从epoll中删除对此socket事件的处理
	epoll_ctl(m_EpollReadFd, EPOLL_CTL_DEL, sock, &ev);
	epoll_ctl(m_EpollWriteFd, EPOLL_CTL_DEL, sock, &ev);
	//pConnection->close();
	//解除双向引用
	pbinddata->pConnection = NULL;
	pbinddata->decRef();
	DEBUG_PRINT("unregistry|success|handle[%d]|binddata[%08X]\n",sock,pbinddata.get());
	return 0;
}

int32 CLinuxReactorImpl::asynWrite(IConnectionPtr& pConnection, CEpollBindData* pData, const char* pszBuf, uint32 nBufLen)
{
	CAutoLock lock(pData->m_SendBufLock);
	pData->SendBuffer->writeBytes(pszBuf, nBufLen);
	//pData->nEpollevents |= EPOLLOUT;
	struct epoll_event ev;
	ev.data.fd = pConnection->getHandle();
	//ev.events   = pData->nEpollevents;
	ev.events = EPOLLOUT;
	//epoll_ctl(m_EpollReadFd, EPOLL_CTL_MOD, pConnection->getHandle(), &ev);
	epoll_ctl(m_EpollWriteFd, EPOLL_CTL_MOD, pConnection->getHandle(), &ev);
	return 0;
}

#endif
