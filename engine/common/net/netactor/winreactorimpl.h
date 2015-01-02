#ifndef _REACTORIMPL_H_
#define _REACTORIMPL_H_


#include "../../util/singleton.h"
#include "netactordriver.h"
#include "../../thread/thread.h"
#include "../../lock/baselock.h"
#include "reactor.h"
#include "../../util/threadpool.h"
#include "../../public.h"

#ifdef WINDOWS

class CWinReactorThread : public CRunnable
{
protected :
	bool m_bRunFlag ;
	SOCKET m_nListenSocket;
	map<SOCKET,void*> m_Clients;
	HANDLE& m_hCompletionHandle;
	CBaseLock m_ClientLock;
	CThread* m_pThread;
public :
	CWinReactorThread(HANDLE& hCompletionHandle);
	~CWinReactorThread();
	void run();
	void addToListen(SOCKET nConnector,void* pClient);
	void stop();
	void start();
	bool init();
};

class CWinReactorImpl : public IReactor,
						public CSingleTon<CWinReactorImpl> 
{
protected :
	//完成端口句柄
	HANDLE m_hCompletionHandle;
	//运行标志
	bool m_bRunFlag;
	//
	map<SOCKET,void*> m_ClientData;
	CWinReactorThread m_Thread;
	typedef TThreadPool<CWinReactorImpl> THREADPOOL_TYPE;
	//线程池
	THREADPOOL_TYPE m_ThreadPool;
	//线程数
	int m_nThreadCount;
	CLightLock m_ClientDataLock;
protected :
	/**
	 * 清除资源
	 */
	void clear();
	/**
	 * 完成端口操作
	 */
	void iocp(CWinReactorImpl* pReactor = NULL);
public :
	CWinReactorImpl();
	~CWinReactorImpl();
	int32 init();
	int32 run();
	int32 stop();
	int32 addAcceptor(IAcceptorPtr Acceptor, INetHandlerPtr Handler, void* Param);
	int32 addConnector(IConnectorPtr Connector, INetHandlerPtr Handler, void* Param);
	int32 removeConnector(IConnectorPtr pConnector);
	int32 removeAcceptor(IAcceptorPtr pAcceptor);
	int32 registry(IConnectionPtr& pConnection, INetHandlerPtr Handler, uint32 nMask, void* Param);
	int32 unregistry(IConnectionPtr& pConnection);
};

class CWinReactorDriver : public INetActorDriver
{
public :
	CWinReactorDriver();
	~CWinReactorDriver();
	const char* getClassName();
	INetActorPtr getNetActorInstance();
};

#endif

#endif
