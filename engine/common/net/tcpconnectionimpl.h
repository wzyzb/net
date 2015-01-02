#ifndef _TCPCONNECTIONIMPL_H_
#define _TCPCONNECTIONIMPL_H_


#include "../public.h"
#include "netpublic.h"
#include "connection.h"
#include "ip4address.h"
#include "../io/inputoutputstream.h"

class CTCPConnectionImpl : public IConnection, 
    public IInputOutputStream,
    public CRefShare
{
protected :
    bool m_bIsClosed;
public:
    CTCPConnectionImpl(SOCKET Sock,IAddressPtr HostAddress,IAddressPtr RemoteAddress);
    virtual ~CTCPConnectionImpl();

    virtual int32 setMode(ECOMMMODE Mode);
    virtual IInputStreamPtr  getInputStream();
    virtual IOutputStreamPtr getOutputStream();
    virtual void close();
    virtual int32 setBufSize(int32 nBufSize);

    virtual int readBytes(char *pBuf, uint32& Buflen);
    //int readBytes(CBytesBuffer& Buffer);
    virtual int popBytes(uint32 BytesToPop);
    virtual uint32 getDataSize() const;
    virtual const char* c_str();
    virtual int writeBytes( const char* pBuf, uint32& Buflen);
    void incRef();
    bool decRef();
};



#endif