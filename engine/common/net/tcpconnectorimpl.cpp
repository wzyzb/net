#include "tcpconnectorimpl.h"
#include "tcpconnectionimpl.h"

CTCPConnectorImpl::CTCPConnectorImpl(IAddressPtr pAddress) : m_pAddress(pAddress)
{
    m_Sock        = INVALID_SOCKET;
    m_Mode        = SYNC_MODE;
    m_pConnection = NULL;
}

CTCPConnectorImpl::~CTCPConnectorImpl()
{
    close();
}

int32 CTCPConnectorImpl::create()
{
    m_Sock = socket(AF_INET, SOCK_STREAM, 0);

    if( INVALID_SOCKET == m_Sock )
    {
        RETURN_ERROR;
    }
    return 0;
}

SOCKET CTCPConnectorImpl::getHandle()
{
    return m_Sock;
}

int32 CTCPConnectorImpl::setMode(ECOMMMODE Mode)
{
    m_Mode = Mode;
    return 0;
}

IConnectionPtr CTCPConnectorImpl::getConnection()
{
    return m_pConnection;
}

int32 CTCPConnectorImpl::connect()
{
    return reconnect();
}

int32 CTCPConnectorImpl::reconnect()
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

    retval = setSocketMode(getHandle(),m_Mode);
    if ( retval != 0 )
    {
        return retval;
    }

    retval = ::connect(getHandle(), (struct sockaddr*)m_pAddress->getsockaddr(), sizeof(sockaddr_in));
    if ( 0 != retval  )
    {
        //异步模式
        if( m_Mode == ASYNC_MODE )
        {
#ifdef WINDOWS
            int errcode = WSAGetLastError();
            if ( errcode != WSAEWOULDBLOCK )
                return errcode;
#else
            if ( errno != EINPROGRESS )
                return errno;
#endif
        }
        else
            RETURN_ERROR;
    }

    m_pConnection = new CTCPConnectionImpl(getHandle(),getSockAddr(getHandle()),m_pAddress);
    if ( m_pConnection == NULL )
    {
        close();
        return -1;
    }

    //	if ( 0 != m_pConnection->setBufSize(10*1024) )
    //	{
    //		m_pConnection->close();
    //		return -1;
    //	}
    return 0;
}

void CTCPConnectorImpl::close()
{
    //防止Connection的析构时再次close
    if ( m_pConnection != NULL )
        m_pConnection->close();
}

const IAddressPtr& CTCPConnectorImpl::getConnectAddress() const
{
    return m_pAddress;
}

void CTCPConnectorImpl::incRef()
{
    CRefShare::incRef();
}

bool CTCPConnectorImpl::decRef()
{
    return CRefShare::decRef();
    //return ;
}
