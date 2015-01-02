#ifndef _LINUXREACTORIMPL_H_
#define _LINUXREACTORIMPL_H_

#include <net/netactor/reactor.h>
#include <net/netpublic.h>
#include <map>
#include <util/singleton.h>
#include <io/bytesbuffer.h>
#include <util/baselock.h>
#include <util/refshare.h>
#include <util/lightlock.h>
#include <util/thread.h>
#include <util/activeobject.h>

#define EVNET_COUNT 1024

class CEpollBindData : public CRefShare
{
public:
    CEpollBindData()
    {
		SendBuffer = CBytesBufferPtr::createInstance(__FUNCTION__);
		reset();
    }

    virtual ~CEpollBindData()
    {
		reset();
    }

    enum
    {
        CONNECTOR_TYPE,
        ACCEPTOR_TYPE,
        CONNECTION_TYPE
    } BindDataType;

	void reset()
	{
		pRawConnector = NULL;
		pAcceptor   = NULL;
		pConnection = NULL;
		pHandler = NULL;
		pParam = NULL;
		nEpollevents = 0;
		nNetHandleMask = 0;
		SendBuffer->reset();
	}

	//连接句柄，也是唯一标识
	SOCKET Sock;
	//
	IConnectorPtr pRawConnector;
	//acceptor
	IAcceptorPtr   pAcceptor;
	//实际的物理连接
	IConnectionPtr pConnection;
	//发送的所队列
	CLightLock m_SendBufLock;
	//网络事件处理器
	INetHandlerPtr pHandler;
	//网络事件处理的mask
	uint32 nNetHandleMask;
	//携带的参数
	void* pParam;
	//发送的buffer
	CBytesBufferPtr SendBuffer;
	//epoll 要处理的事件
	uint32 nEpollevents;
};

typedef CRefObject<CEpollBindData> CEpollBindDataPtr;


class CLinuxReactorImpl : public IReactor,
                          public CSingleton<CLinuxReactorImpl>
{
protected :
	/**
	 * 单独的发送线程
	 */
	void sendRun(CLinuxReactorImpl* pPointer);

public:
    CLinuxReactorImpl();
    virtual ~CLinuxReactorImpl();

    /**
     *
     */
    virtual int32 init();

    /**
     *
     */
    virtual int32 run();    

    /** 
     *
     */
    virtual int32 stop();

    /**
     *
     */
    virtual int32 addAcceptor(IAcceptorPtr Acceptor, INetHandlerPtr Handler, void* Param);
	/**
	 *
	 */
	virtual int32 removeAcceptor(IAcceptorPtr Acceptor);

    /**
     *
     */
    virtual int32 addConnector(IConnectorPtr Connector, INetHandlerPtr Handler, void* Param);
	/**
	 *
	 */
	virtual int32 removeConnector(IConnectorPtr Connector);

    /**
     *
     */
    virtual int32 registry(IConnectionPtr& pConnection, INetHandlerPtr Handler, uint32 nMask,void* Param);

	/**
	 *
	 */
	virtual int32 unregistry(IConnectionPtr& pConnection);
	/**
	 * 利用epoll的异步写
	 */
    int32 asynWrite(IConnectionPtr& pConnection, CEpollBindData* pData, const char* pszBuf, uint32 nBufLen);
    
    //int32 AsynRead(IConnectionPtr& pConnection, char* pszBuf, uint32 nBufLen); 
protected:
	CLightLock m_BindDataLock;

	//读操作的完成端口
	int m_EpollReadFd;
	//写操作的完成端口
	int m_EpollWriteFd;
	struct epoll_event m_events[EVNET_COUNT];
    std::map<SOCKET, CEpollBindData*> m_mapBindData;
	bool m_isLoop;
	typedef TActiveObject<CLinuxReactorImpl> THREAD_TYPE;
	//发送线程
	THREAD_TYPE* m_pThread;
};

#endif

