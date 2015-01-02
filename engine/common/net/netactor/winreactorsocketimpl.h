#ifndef _REACTORSOCKETIMPL_H_
#define _REACTORSOCKETIMPL_H_

#include "../../public.h"
#include "../netpublic.h"
#include "../acceptor.h"
#include "../connector.h"
#include <mswsock.h>
#include "../connection.h"
#include "nethandler.h"
#include "../../io/bytesbuffer.h"
#include "../../lock/lightlock.h"

//单IO操作数据
typedef struct _t_per_io_data
{
	//完成端口结构
	OVERLAPPED overlapped;
	//一个数据缓冲区,用于WSASend/WSARecv中的第二个参数
	WSABUF buffer;
	//操作类型,可以为SEND/RECV两种
	enum OPER_TYPE
	{
		OPER_ACCEPT,
		OPER_SEND,
		OPER_RECV
	}opertype;
	DWORD dwFlags;
}PER_IO_DATA,*LPPER_IO_DATA;


typedef struct _t_binddata : public CRefShare
{
	enum
	{
		CONNECTOR_TYPE,
		ACCEPTOR_TYPE,
		CONNECTION_TYPE
	} BindDataType;
	//实际的连接器
	IConnectorPtr pRawConnector;
	//连接器
	IConnectorPtr  pConnector;
	//Acceptor
	IAcceptorPtr   pAcceptor;
	//物理连接
	IConnectionPtr pConnection;
	//存放外部的参数
	void* pParam;
	//Net Handle的mask
	uint32 nMask;
	//网络操作实例
	INetHandlerPtr pHandler;
	int32 nBeginConnect;
	//完成端口句柄
	HANDLE hCompletionHandle;
	//Io数据
	PER_IO_DATA IoData;
	//接收缓冲
	CBytesBufferPtr RecvBuffer;
	//发送缓冲
	CBytesBufferPtr SendBuffer;

	_t_binddata() : pConnector(NULL),pAcceptor(NULL),pConnection(NULL),pHandler(NULL)
	{
		nMask = 0;
		pParam = NULL;
		hCompletionHandle = INVALID_HANDLE_VALUE;
	}

	~_t_binddata()
	{

	}
}BIND_DATA,*LPBIND_DATA;

typedef CRefObject<BIND_DATA> BIND_DATAPTR;

class CWinReactorAcceptorImpl : public IAcceptor
{
protected :
	IAcceptorPtr m_pAcceptor;
	//Accept进来的客户端socket
	SOCKET m_nClientSocket;
	//可用于完成端口的Accept方法
	LPFN_ACCEPTEX m_pFunAcceptEx;
	//获取地址的缓冲区
	char m_AcceptBuff[100];
public :
	CWinReactorAcceptorImpl(IAcceptorPtr pAcceptor);
	~CWinReactorAcceptorImpl();
	SOCKET getHandle();
	int32 setMode(ECOMMMODE Mode);
	ECOMMMODE getMode();
	int32 listen(int BackLog);
	IConnectionPtr accept();
	void close();
	/*
	 * 创建AcceptEx所需的client socket
	 */
	SOCKET createClientSocket();
	/**
	 *
	 */
	int32 IocpAccept(LPOVERLAPPED pOverLapped);
	const IAddressPtr& getListenAddress() const;
};

class CWinReactorConnectionImpl : public IConnection,
								  public IOutputStream,
								  public CRefShare
{
protected :
	//发送锁
	CLightLock m_SendLock;
	//接收缓冲
	CBytesBufferPtr m_RecvBuffer;
	PER_IO_DATA m_RecvIoData;
	//发送缓冲
	CBytesBufferPtr m_SendBuffer;
	PER_IO_DATA m_SendIoData;
	//底层的连接
	IConnectionPtr m_pConnection;
	//读取的KEY
	BIND_DATAPTR m_Key;
	//是否在发送中的标识
	bool m_bIsSending;
public :
	CWinReactorConnectionImpl(IConnectionPtr pConnection,BIND_DATAPTR Key);
	~CWinReactorConnectionImpl();
	int32 setMode(ECOMMMODE Mode);
	IInputStreamPtr  getInputStream();
	IOutputStreamPtr getOutputStream();
	void* getKey() { return m_Key; }
	/**
	 * 写入数据
	 */
	int writeBytes( const char* pBuf, uint32& Buflen);
	/**
	 * 完成发送
	 */
	void finishSend(uint32 SendSize);
	/**
	 * 完成读取
	 */
	void finishRecv(uint32 SendSize);
	void close();
	/**
	 * 触发完成端口的写事件，保证按顺序发送数据
	 */
	int32 IocpSend();
	/**
	 * 触发完成端口的读事件
	 */
	int32 IocpRecv();
	SOCKET getHandle() const;
	IAddressPtr getHostAddress();
	IAddressPtr getRemoteAddress();
	/**
	 * 读取接收队列的缓冲
	 */
	CBytesBufferPtr& getRecvBuffer();
    //void setParam(void* pParam);
    //void* getParam();
	/**
	 * 设置Socket缓冲区的大小
	 */
	int32 setBufSize(int32 nBufSize);
	void incRef();
	bool decRef();
};

class CWinReactorConnectorImpl : public IConnector,
								 public CRefShare
{
protected :
	IConnectorPtr m_pConnector;
public :
	CWinReactorConnectorImpl(IConnectorPtr pConnector);
	~CWinReactorConnectorImpl();
	SOCKET getHandle();
	int32 setMode(ECOMMMODE Mode);
	IConnectionPtr getConnection();
	int32 connect();
	int32 reconnect();
	void close();
	void incRef();
	bool decRef();
	const IAddressPtr& getConnectAddress() const;
};


#endif
