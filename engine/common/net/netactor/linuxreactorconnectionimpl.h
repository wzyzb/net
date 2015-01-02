#ifndef _LINUXREACTORCONNECTIONIMPL_H_
#define _LINUXREACTORCONNECTIONIMPL_H_

#include <net/netactor/reactor.h>
#include <map>
#include <util/singleton.h>
#include <net/connection.h>
#include <io/inputoutputstream.h>
#include <net/netactor/unix/linuxreactorimpl.h>

#define EVNET_COUNT 1024

class CLinuxReactorConnectionImpl : public IConnection, 
                                    public IInputOutputStream,
                                    public CRefShare 
{
public:
    CLinuxReactorConnectionImpl(CLinuxReactorImpl* pReactor, CEpollBindData* pBindData, IConnectionPtr pConnection);
    virtual ~CLinuxReactorConnectionImpl();

    IConnectionPtr getRealConnection();

    virtual int32 setMode(ECOMMMODE Mode);
    virtual IInputStreamPtr  getInputStream();
    virtual IOutputStreamPtr getOutputStream();

    virtual void close();

    virtual int readBytes(char *pBuf, uint32& Buflen);
    virtual int popBytes(uint32 BytesToPop);
    virtual uint32 getDataSize() const;
    virtual const char* c_str();

    virtual int writeBytes( const char* pBuf, uint32& Buflen); 

    void incRef();
    bool decRef();

		
    virtual SOCKET getHandle() const;
    virtual IAddressPtr getHostAddress();
    virtual IAddressPtr getRemoteAddress();
    int32 setBufSize(int32 Size);
	/**
	 * »ñÈ¡epollµÄkey
	 */
	CEpollBindData* getKey();

private:
    IConnectionPtr     m_pConnection;
    CLinuxReactorImpl* m_pReactor; 
    CEpollBindDataPtr  m_pBindData;   
};

typedef CRefObject<CLinuxReactorConnectionImpl> CLinuxReactorConnectionImplPtr;

#endif

