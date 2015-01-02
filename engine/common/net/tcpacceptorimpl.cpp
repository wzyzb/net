#include "tcpacceptorimpl.h"
#include "tcpconnectionimpl.h"
#include "ip4address.h"
#include "socketdriver.h"
#include "drivermanager.h"

CTCPAcceptorImpl::CTCPAcceptorImpl( IAddressPtr pAddress ) : m_pAddress( pAddress )
{
    m_ListenSock = -1;
    m_Mode       = SYNC_MODE;
}

CTCPAcceptorImpl::~CTCPAcceptorImpl()
{
	close();
}

int32 CTCPAcceptorImpl::create()
{
	m_ListenSock = socket(AF_INET, SOCK_STREAM, 0);
	if ( INVALID_SOCKET == m_ListenSock )
	{
		RETURN_ERROR;
	}
	return 0;
}

SOCKET CTCPAcceptorImpl::getHandle()
{
    return m_ListenSock;
}

int32 CTCPAcceptorImpl::setMode(ECOMMMODE Mode)
{
    m_Mode = Mode;
    return 0;
}

int32 CTCPAcceptorImpl::listen(int BackLog)
{
    uint32 nResult = 1;
    
    // 创建 SOCKET
    nResult = create();
    if ( nResult != 0 )
    {
        return nResult;
    }

    // 设置SOCKET的阻塞模式 (阻塞/非阻塞)
    nResult = setSocketMode( getHandle(),  m_Mode );
    if ( nResult != 0 )
    {
        return nResult;
    }

    // ListenSocket 允许端口复用
    int nOn = 1;
#ifdef WINDOWS
    if ( -1 == setsockopt(getHandle(), SOL_SOCKET, SO_REUSEADDR, (const char*)&nOn, sizeof(nOn)) )
	{
		return WSAGetLastError();
	}
#else
	if ( -1 == setsockopt(getHandle(), SOL_SOCKET, SO_REUSEADDR, &nOn, sizeof(nOn)) )
    {
        RETURN_ERROR;
    }
#endif

    if ( m_pAddress == NULL )
    {
        return -1;
    }

    if( -1 == bind(getHandle(), (struct sockaddr*)m_pAddress->getsockaddr(), sizeof(struct sockaddr)) )
    {
        RETURN_ERROR;
    }

	int recvsize = 1024*32;
	if ( -1 == setsockopt(getHandle(), SOL_SOCKET, SO_SNDBUF, (const char*)&recvsize, sizeof(int)) )
	{
		RETURN_ERROR;
	}

    if( -1 == ::listen(getHandle(), BackLog) )
    {
        RETURN_ERROR;
    }
    else
    {
        return 0;
    }
}


IConnectionPtr CTCPAcceptorImpl::accept()
{
	struct sockaddr_in clientaddr;
	memset(&clientaddr, 0x00, sizeof(clientaddr));
	socklen_t len = sizeof(struct sockaddr);

	int clientfd = ::accept(getHandle(), (struct sockaddr*)&clientaddr, &len);
	if ( -1 == clientfd )
	{
		return NULL;
	}
	//获取远程地址
	IAddressPtr remoteaddress = new CIP4Address(inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	//创建TCPConnection的实例
	IConnectionPtr conn(new CTCPConnectionImpl(clientfd,m_pAddress,remoteaddress));
	if ( conn == NULL )
	{
		CLOSE_SOCKET(clientfd);
		return NULL;
	}
	// 设置SOCKET的阻塞模式 (阻塞/非阻塞)
	if ( 0 != conn->setMode(getMode()) )
	{
		conn->close();
		return NULL;
	}
	return conn;
}

void CTCPAcceptorImpl::close()
{
    shutdown(getHandle(), SHUT_RDWR);
    CLOSE_SOCKET(getHandle());
}

ECOMMMODE CTCPAcceptorImpl::getMode()
{
	return m_Mode;
}

const IAddressPtr& CTCPAcceptorImpl::getListenAddress() const
{
	return m_pAddress;
}