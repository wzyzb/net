#include "udpsocketdriver.h"
#include "udpsocketimpl.h"

CUDPSocketDriver::CUDPSocketDriver()
{

}

CUDPSocketDriver::~CUDPSocketDriver()
{

}

const char* CUDPSocketDriver::getClassName()
{
    return "CUDPSocketDriver";
}

IAcceptorPtr CUDPSocketDriver::createAcceptor(IAddressPtr pAddress)
{
    ThrowException<CMyException>("UDP Not Support Acceptor");
    return NULL;
    //return new CTCPAcceptorImpl(pAddress);	
}

IConnectorPtr CUDPSocketDriver::createConnector(IAddressPtr pAddress)
{
    return new CUDPConnectorImpl(pAddress);
}

IConnectionPtr CUDPSocketDriver::createConnection(SOCKET Handle,IAddressPtr& HostAddress,IAddressPtr& RemoteAddress)
{
    return new CUDPConnectionImpl(Handle,HostAddress,RemoteAddress);
}
