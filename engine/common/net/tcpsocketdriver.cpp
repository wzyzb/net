#include "tcpsocketdriver.h"
#include "tcpacceptorimpl.h"
#include "tcpconnectorimpl.h"
#include  "tcpconnectionimpl.h"

CTCPSocketDriver::CTCPSocketDriver()
{

}

CTCPSocketDriver::~CTCPSocketDriver()
{

}

const char* CTCPSocketDriver::getClassName()
{
    return "CTCPSocketDriver";
}

IAcceptorPtr CTCPSocketDriver::createAcceptor(IAddressPtr pAddress)
{
    return new CTCPAcceptorImpl(pAddress);	
}

IConnectorPtr CTCPSocketDriver::createConnector(IAddressPtr pAddress)
{
    return new CTCPConnectorImpl(pAddress);
}

IConnectionPtr CTCPSocketDriver::createConnection(SOCKET Handle,IAddressPtr& HostAddress,IAddressPtr& RemoteAddress)
{
    return new CTCPConnectionImpl(Handle,HostAddress,RemoteAddress);
}