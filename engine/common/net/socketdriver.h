#ifndef _SOCKETDRIVER_H_
#define _SOCKETDRIVER_H_




#include "../util/driver.h"
#include "acceptor.h"
#include "connector.h"

class ISocketDriver : public IDriver
{
public:
    ISocketDriver(){}
    virtual ~ISocketDriver(){}

    virtual IAcceptorPtr  createAcceptor(IAddressPtr pAddr) = 0;
    virtual IConnectorPtr createConnector(IAddressPtr pAddr) = 0;

    virtual IConnectionPtr createConnection(SOCKET Handle,IAddressPtr& HostAddress,IAddressPtr& remoteAddress) = 0;

    virtual const char* getDriverName() { return "SocketDriver";}
};

typedef CRefObject<ISocketDriver> ISocketDriverPtr;

















#endif