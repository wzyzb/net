#ifndef _TCPACCEPTORIMPL_H_
#define _TCPACCEPTORIMPL_H_




#include "netpublic.h"
#include "acceptor.h"
#include "connection.h"

class CTCPAcceptorImpl : public IAcceptor
{
protected:
    CTCPAcceptorImpl();
public:
    CTCPAcceptorImpl(IAddressPtr pAddress);
    virtual ~CTCPAcceptorImpl();

    SOCKET          getHandle();

    int32           setMode(ECOMMMODE Mode);
    ECOMMMODE       getMode();

    int32           listen(int BackLog);
    IConnectionPtr  accept();

    void            close();
    const IAddressPtr&  getListenAddress() const;

protected:
    int32 create();

private:
    SOCKET      m_ListenSock;
    ECOMMMODE   m_Mode;
    IAddressPtr m_pAddress;
};





















#endif