#include "tcpconnectionimpl.h"
#include "../io/bytesbuffer.h"

CTCPConnectionImpl::CTCPConnectionImpl(SOCKET Sock,IAddressPtr HostAddress,IAddressPtr RemoteAddress)
{
    m_bIsClosed = false;
    m_Socket = Sock;
    m_pHostAddr = HostAddress;
    m_pRemoteAddr = RemoteAddress;
}
//禁用Nagle算法
//	int flag = 1;
//	int ret = setsockopt(Sock,IPPROTO_TCP,TCP_NODELAY,(const char*)&flag,sizeof(flag));
//	if ( ret == -1 )
//	{
//		printf("set nodelay error\n");
//	}

CTCPConnectionImpl::~CTCPConnectionImpl()
{
    close();
    //最后才回收句柄
    CLOSE_SOCKET(m_Socket);
}

int32 CTCPConnectionImpl::setMode(ECOMMMODE Mode)
{
    return setSocketMode(m_Socket,Mode);
}

IInputStreamPtr CTCPConnectionImpl::getInputStream()
{
    return this;
}

IOutputStreamPtr CTCPConnectionImpl::getOutputStream()
{
    return this;
}

void CTCPConnectionImpl::close()
{
    SOCKET nclientfd = m_Socket;
    if ( nclientfd == INVALID_SOCKET )
        return;
    if ( m_bIsClosed )
        return ;
    m_bIsClosed = true;
    //设置为不经历TIME_WAIT
    linger ling;
    ling.l_onoff = 1;
    ling.l_linger = 0;
    setsockopt(nclientfd,SOL_SOCKET,SO_LINGER,(const char*)&ling,sizeof(linger));
    shutdown(nclientfd, SHUT_RDWR);
}

int CTCPConnectionImpl::readBytes(char *pBuf, uint32& Buflen)
{
    int retval = recv(m_Socket, pBuf, Buflen,0);
    //socket已被关闭
    if ( 0 == retval )
        return -1;
    if ( SOCKET_ERROR == retval )
        RETURN_ERROR;
    Buflen = retval;
    return 0;
}

void CTCPConnectionImpl::incRef()
{
    CRefShare::incRef();
}

bool CTCPConnectionImpl::decRef()
{
    return CRefShare::decRef();
    //return ;
}

int CTCPConnectionImpl::popBytes(uint32 BytesToPop)
{
    return 0;
}

uint32 CTCPConnectionImpl::getDataSize() const
{
    return 0;
}

const char* CTCPConnectionImpl::c_str()
{
    return NULL;
}

int CTCPConnectionImpl::writeBytes( const char* pBuf, uint32& Buflen)
{
    int retval = send(m_Socket, pBuf, Buflen,0);
    if ( SOCKET_ERROR == retval )
        RETURN_ERROR;
    Buflen = retval;
    return 0;
}

int32 CTCPConnectionImpl::setBufSize(int32 nBufSize)
{
    if ( -1 == setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (const char*)&nBufSize, sizeof(int)) )
    {
        RETURN_ERROR;
    }

    if ( -1 == setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (const char*)&nBufSize, sizeof(int)) )
    {
        RETURN_ERROR;
    }
    return 0;
}
