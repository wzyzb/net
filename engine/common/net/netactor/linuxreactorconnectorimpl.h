#ifndef _LINUXREACTORCONNECTORIMPL_H_
#define _LINUXREACTORCONNECTORIMPL_H_

#include <net/netactor/reactor.h>
#include <map>
#include <util/singleton.h>
#include <net/connection.h>
#include <io/inputstream.h>
#include <io/outputstream.h>
#include <net/netactor/unix/linuxreactorimpl.h>
#include <net/netactor/unix/linuxreactorconnectionimpl.h>

#define EVNET_COUNT 1024

class CLinuxReactorConnectorImpl : public IConnector, 
                                   public CRefShare 
{
public:
    CLinuxReactorConnectorImpl(CLinuxReactorImpl* pReactor, CEpollBindDataPtr lpBindData, IConnectorPtr pConnector);
    virtual ~CLinuxReactorConnectorImpl();

    virtual SOCKET getHandle();

    virtual int32 setMode(ECOMMMODE Mode);

    virtual IConnectionPtr getConnection();

    virtual int32 connect();

    virtual int32 reconnect();

    virtual void close();

    void incRef();
    bool decRef();
    const IAddressPtr& getConnectAddress() const;

protected :
	CLinuxReactorConnectorImpl();
	IConnectorPtr      m_pConnector;
	CLinuxReactorImpl* m_pReactor; 
	CEpollBindData*  m_pBindData; 
	CLinuxReactorConnectionImplPtr m_pLinuxConnection;
};

typedef CRefObject<CLinuxReactorConnectorImpl> CLinuxReactorConnectorImplPtr;

#endif

