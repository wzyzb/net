#ifndef _UDPSOCKETDRIVER_H_
#define _UDPSOCKETDRIVER_H_


#include "acceptor.h"
#include "connector.h"
#include "socketdriver.h"

/*
UDP SocketÇý¶¯
*/
class CUDPSocketDriver : public ISocketDriver
{
public:
	CUDPSocketDriver();
	virtual ~CUDPSocketDriver();
        
	/**
	 *
	 */
	virtual const char* getClassName();
	    
	/**
	 *
	 */
	virtual IAcceptorPtr createAcceptor(IAddressPtr pAddr);

	/**
	 *
	 */
	virtual IConnectorPtr createConnector(IAddressPtr pAddr);

	/**
	 *
	 */
	virtual IConnectionPtr createConnection(SOCKET Handle,IAddressPtr& HostAddress,IAddressPtr& RemoteAddress);
};

#endif