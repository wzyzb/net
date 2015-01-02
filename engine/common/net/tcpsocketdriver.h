#ifndef _TCPSOCKETDRIVER_H_
#define _TCPSOCKETDRIVER_H_


#include "acceptor.h"
#include "connector.h"
#include "socketdriver.h"

class CTCPSocketDriver : public ISocketDriver
{
public:
	CTCPSocketDriver();
	virtual ~CTCPSocketDriver();

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