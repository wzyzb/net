#include "udpsocketimpl.h"

/*

		Class	CUDPConnectionImpl

*/
CUDPConnectionImpl::CUDPConnectionImpl(SOCKET Sock,IAddressPtr HostAddress,IAddressPtr RemoteAddress)
{
	m_bIsClosed = false;
	m_Socket = Sock;
	m_pHostAddr = HostAddress;
	m_pRemoteAddr = RemoteAddress;
}

CUDPConnectionImpl::~CUDPConnectionImpl()
{
	close();
	//CLOSE_SOCKET(m_Socket);
}

int32 CUDPConnectionImpl::setMode(ECOMMMODE Mode)
{
	return setSocketMode(m_Socket,Mode);
}

IInputStreamPtr CUDPConnectionImpl::getInputStream()
{
    return this;
}

IOutputStreamPtr CUDPConnectionImpl::getOutputStream()
{
    return this;
}

void CUDPConnectionImpl::close()
{
	m_bIsClosed = true;
	//shutdown(m_Socket, SHUT_RDWR);
}

int CUDPConnectionImpl::readBytes(char *pBuf, uint32& Buflen)
{
	struct sockaddr_in client_addr;
	memset(&client_addr,0,sizeof(client_addr));
	socklen_t client_len = sizeof(client_addr);
	int retval = recvfrom(m_Socket, pBuf, Buflen,0,(struct sockaddr*)&client_addr,&client_len);
	//socket已被关闭
	if ( 0 == retval )
		return -1;
	if ( SOCKET_ERROR == retval )
		RETURN_ERROR;
	Buflen = retval;
	return 0;
}

void CUDPConnectionImpl::incRef()
{
	CRefShare::incRef();
}

bool CUDPConnectionImpl::decRef()
{
    return CRefShare::decRef();
	//return ;
}

int CUDPConnectionImpl::popBytes(uint32 BytesToPop)
{
    return 0;
}

uint32 CUDPConnectionImpl::getDataSize() const
{
    return 0;
}

const char* CUDPConnectionImpl::c_str()
{
    return NULL;
}

int CUDPConnectionImpl::writeBytes( const char* pBuf, uint32& Buflen)
{
	int retval = sendto(m_Socket, pBuf, Buflen,0,(struct sockaddr*)this->m_pRemoteAddr->getsockaddr(),sizeof(*this->m_pRemoteAddr->getsockaddr()));
	if ( SOCKET_ERROR == retval )
		RETURN_ERROR;
	Buflen = retval;
	return 0;
}

int32 CUDPConnectionImpl::setBufSize(int32 nBufSize)
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

/*

		Class	CUDPConnectorImpl

*/

CUDPConnectorImpl::CUDPConnectorImpl(IAddressPtr pAddress) : m_pAddress(pAddress)
{
    m_Sock        = INVALID_SOCKET;
    m_Mode        = SYNC_MODE;
    m_pConnection = NULL;
}

CUDPConnectorImpl::~CUDPConnectorImpl()
{
	close();
}

int32 CUDPConnectorImpl::create()
{
	m_Sock = socket(AF_INET, SOCK_DGRAM, 0);
	if( INVALID_SOCKET == m_Sock )
	{
		RETURN_ERROR;
	}
	return 0;
}

SOCKET CUDPConnectorImpl::getHandle()
{
    return m_Sock;
}

int32 CUDPConnectorImpl::setMode(ECOMMMODE Mode)
{
    m_Mode = Mode;
	return 0;
}

IConnectionPtr CUDPConnectorImpl::getConnection()
{
    return m_pConnection;
}

int32 CUDPConnectorImpl::connect()
{
    return reconnect();
}

int32 CUDPConnectorImpl::reconnect()
{
	//保证资源被释放
	close();
	if ( NULL == m_pAddress )
	{
		return -1;
	}

	uint32 retval = 1;

	// 创建SOCKET
	retval = create();
	if ( retval != 0 )
	{
		return retval;
	}

	retval = setSocketMode(m_Sock,m_Mode);
	if ( retval != 0 )
	{
		return retval;
	}

	m_pConnection = new CUDPConnectionImpl(m_Sock,getSockAddr(m_Sock),m_pAddress);
	if ( m_pConnection == NULL )
	{
		close();
		return -1;
	}

	return 0;
}

void CUDPConnectorImpl::close()
{
	if ( m_pConnection != NULL )
		m_pConnection->close();
	shutdown(m_Sock, SHUT_RDWR);
	m_Sock = INVALID_SOCKET;
}

const IAddressPtr& CUDPConnectorImpl::getConnectAddress() const
{
	return m_pAddress;
}

void CUDPConnectorImpl::incRef()
{
	CRefShare::incRef();
}

bool CUDPConnectorImpl::decRef()
{
    return CRefShare::decRef();
	//return ;
}

