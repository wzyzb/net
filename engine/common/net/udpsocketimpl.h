#ifndef _UDPSOCKETIMPL_H_
#define _UDPSOCKETIMPL_H_

#include "acceptor.h"
#include "connector.h"
#include "connection.h"
#include "../io/inputoutputstream.h"

/*
class CUDPConnection
UDP连接
*/
class CUDPConnectionImpl : public IConnection, 
                        public IInputOutputStream,
						public CRefShare
{
protected :
	bool m_bIsClosed;

public:
	/**
	 * 构造函数
	 */
	CUDPConnectionImpl(SOCKET Sock,IAddressPtr HostAddress,IAddressPtr RemoteAddress);
	/**
	 * 析构函数
	 */
	virtual ~CUDPConnectionImpl();
	/**
	 * 设置同步、异步模式
	 */
	int32 setMode(ECOMMMODE Mode);
	/**
	 * 获取输入流
	 */
	IInputStreamPtr  getInputStream();
	/**
	 * 获取输出流
	 */
	IOutputStreamPtr getOutputStream();
	/**
	 * 关闭连接
	 */
	void close();
	/**
	 * 设置Socket缓冲区的大小
	 */
	int32 setBufSize(int32 nBufSize);

    virtual int readBytes(char *pBuf, uint32& Buflen);
	//int readBytes(CBytesBuffer& Buffer);
    virtual int popBytes(uint32 BytesToPop);
    virtual uint32 getDataSize() const;
    virtual const char* c_str();
    virtual int writeBytes( const char* pBuf, uint32& Buflen);
	void incRef();
	bool decRef();
};

/*
UDP Connector
*/
class CUDPConnectorImpl : public IConnector,
						  public CRefShare
{
protected:
	CUDPConnectorImpl();
	int32 create();

public:
	CUDPConnectorImpl(IAddressPtr pAddress);
	virtual ~CUDPConnectorImpl();

	virtual SOCKET getHandle();

	virtual int32 setMode(ECOMMMODE Mode);

	virtual IConnectionPtr getConnection();

	virtual int32 connect();

	virtual int32 reconnect();  

	virtual void close();

	const IAddressPtr& getConnectAddress() const;
	void incRef();
	bool decRef();

private:
    SOCKET         m_Sock;
    ECOMMMODE      m_Mode;
    IAddressPtr    m_pAddress;
    IConnectionPtr m_pConnection;
};

/*
UDP Acceptor
*/
class CUDPAcceptorImpl : public IAcceptor
{
protected:
    CUDPAcceptorImpl();

public:
    CUDPAcceptorImpl(IAddressPtr pAddress);
    virtual ~CUDPAcceptorImpl();

    SOCKET getHandle();

    int32 setMode(ECOMMMODE Mode);

	ECOMMMODE getMode();

    int32 listen(int BackLog);

    IConnectionPtr accept();

    void close();
	const IAddressPtr& getListenAddress() const;

private:
    SOCKET      m_ListenSock;
    ECOMMMODE   m_Mode;
    IAddressPtr m_pAddress;
};

#endif

