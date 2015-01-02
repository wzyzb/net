#ifndef _TCPCONNECTORIMPL_H_
#define _TCPCONNECTORIMPL_H_



#include "../public.h"
#include "netpublic.h"
#include "connector.h"
#include "connection.h"
#include "address.h"
#include "../util/refShare.h"

class CTCPConnectorImpl : public IConnector,
    public CRefShare
{
protected:
    CTCPConnectorImpl();

public:
    CTCPConnectorImpl(IAddressPtr pAddress);
    virtual ~CTCPConnectorImpl();

    virtual SOCKET getHandle();

    virtual int32 setMode(ECOMMMODE Mode);

    virtual IConnectionPtr getConnection();

    virtual int32 connect();

    virtual int32 reconnect();  

    virtual void close();

    const IAddressPtr& getConnectAddress() const;
    void incRef();
    bool decRef();

protected:
    int32 create(); 

private:
    SOCKET         m_Sock;
    ECOMMMODE      m_Mode;
    IAddressPtr    m_pAddress;
    IConnectionPtr m_pConnection;
};

#endif