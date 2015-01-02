#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include "netpublic.h"
#include "address.h"
#include "connection.h"
#include "../util/refshare.h"

class IConnector : public IRefShare
{
public:
	
	virtual ~IConnector() { }

	virtual SOCKET getHandle() = 0;
	
    virtual int32 setMode(ECOMMMODE Mode) = 0;
	
    virtual IConnectionPtr getConnection() = 0;
	
    virtual int32 connect() = 0;
	virtual int32 reconnect() = 0;
	
    virtual void  close() = 0;

	virtual const IAddressPtr& getConnectAddress() const = 0;//获取连接的地址
};

typedef CRefObject<IConnector> IConnectorPtr;

#endif
