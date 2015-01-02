#include <public.h>

#ifndef WINDOWS
#include <net/netactor/unix/linuxreactorconnectorimpl.h>

CLinuxReactorConnectorImpl::CLinuxReactorConnectorImpl(CLinuxReactorImpl* pReactor, CEpollBindDataPtr pBindData, 
                                                       IConnectorPtr pConnector) : m_pReactor(pReactor),
                                                                                   m_pBindData(pBindData.get()),
                                                                                   m_pConnector(pConnector)
{
    //m_pLinuxConnection = new CLinuxReactorConnectionImpl(pReactor, m_pBindData, m_pConnector->getConnection());
}

CLinuxReactorConnectorImpl::~CLinuxReactorConnectorImpl()
{
}

SOCKET CLinuxReactorConnectorImpl::getHandle()
{
    return m_pConnector->getHandle();    
}

int32 CLinuxReactorConnectorImpl::setMode(ECOMMMODE Mode)
{
    return m_pConnector->setMode(Mode);
}

IConnectionPtr CLinuxReactorConnectorImpl::getConnection()
{
	return m_pConnector->getConnection();
    //return m_pLinuxConnection.get();
}

int32 CLinuxReactorConnectorImpl::connect()
{
    return m_pConnector->connect();
}

int32 CLinuxReactorConnectorImpl::reconnect()
{
    return m_pConnector->reconnect();
}

void CLinuxReactorConnectorImpl::close()
{
    return m_pConnector->close();
}

void CLinuxReactorConnectorImpl::incRef()
{
     CRefShare::incRef();
}

bool CLinuxReactorConnectorImpl::decRef()
{
     return CRefShare::decRef();
}

const IAddressPtr& CLinuxReactorConnectorImpl::getConnectAddress() const
{
    return m_pConnector->getConnectAddress();
}

#endif

