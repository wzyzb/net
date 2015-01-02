#include <util/baselock.h>

#include <util/thread.h>
#include <stdio.h>

#ifdef _WIN32
#pragma warning(disable:4996)
#endif

////////////////////////////////////////////////////////////////////////////
//
//
//				class CBaseLock
//
//
////////////////////////////////////////////////////////////////////////////
int CBaseLock::serialno = 0;
CBaseLock::CBaseLock()
{
	m_nLockCount = 0;
	this->bInitialized = false;
#ifdef WIN32
	sprintf(name,"PID[%d]::Synchronizer%d",GetCurrentProcessId(),serialno++);
#else
	sprintf(name,"PID[%d]::Synchronizer%d",getpid(),serialno++);
#endif
#ifdef WIN32
	this->m_condObj = NULL;
	this->m_mutexObj = NULL;
	this->m_OwnThread = NULL;
#endif
}

//初始化所有资源锁－－互斥变量、条件变量(仅对UNIX系统有效)
bool CBaseLock::initialize()
{
	//创建线程私有变量数据
	if ( ! m_Synchronizer.initialize() )
	{
#ifdef _THDEBUG
#ifdef WIN32
		xprint("Create TSD Object Failed[%d]",GetLastError());
#else
		xprint("Create TSD Object Failed");
#endif
#endif
		return false;
	}

#ifdef WIN32
	//检查是否已经存在同名的MUTEX对象
	m_mutexObj = OpenMutex(MUTEX_ALL_ACCESS,false,name);
	if ( m_mutexObj != NULL )
	{
#ifdef _THDEBUG
		xprint("WINDOWX Mutex Object Already exists\n");
#endif
		CloseHandle(m_mutexObj);
		return false;
	}
	//参数false 表明并不是马上获得Mutex对象的访问权，
	//其他线程需要取得访问权的时候必须用wait function才能获得
	m_mutexObj = CreateMutex(NULL,false,name);
	if ( m_mutexObj == NULL )
	{
#ifdef _THDEBUG
		xprint("Create WINDOWS Mutex Object Failed[%d]",GetLastError());
#endif
		return false;
	}

	//生成新的EVENT唯一标识名
	strcat(name,"EVENT");
	//检查是否已经存在同名的EVENT对象
	m_condObj = OpenEvent(EVENT_ALL_ACCESS,false,name);
	if ( m_condObj != NULL )
	{
#ifdef _THDEBUG
		xprint("WINDOW Event Object Already exists\n");
#endif
		CloseHandle(m_condObj);
		return false;
	}

	//第一参数true 表明所有EVENT对象被获取后不会马上标识成nonsignaled，要调用ResetEvent()才可以
	//第二个参数false表明创建后的状态是nonsignaled
	m_condObj = CreateEvent(NULL,true,false,name);
	if ( m_condObj == NULL )
	{
#ifdef _THDEBUG
		xprint("Create WINDOWS Mutex Object Failed[%d]",GetLastError());
#endif
		return false;
	}

//end of WIN32 INITIALIZED
#else
	int retval = 0 ;
	if ( ( retval = pthread_mutex_init(&m_mutexObj,NULL) ) != 0 )
	{
#ifdef _THDEBUG
		xprint("Create UNIX Mutex Object Failed[%d]",retval);
#endif
		return false;
	}
	if ( ( retval = pthread_cond_init(&m_condObj,NULL) ) != 0 )
	{
#ifdef _THDEBUG
		xprint("Create UNIX Condition Object Failed[%d]",retval);
#endif
		pthread_mutex_destroy(&m_mutexObj);
		return false;
	}
#endif
	this->bInitialized = true;
	return true;
}

//析构函数，关闭资源锁对象
CBaseLock::~CBaseLock()
{
#ifdef WIN32
	if ( m_condObj != NULL )
		SetEvent(m_condObj);
#else
	pthread_cond_broadcast(&this->m_condObj);
#endif

#ifdef WIN32
	if ( m_condObj != NULL )
		CloseHandle(m_condObj);
	if ( m_mutexObj != NULL )
		CloseHandle(m_mutexObj);
#else
	pthread_cond_destroy(&m_condObj);
	pthread_mutex_destroy(&m_mutexObj);
#endif
}

//对本对象上锁－－即开始进行同步操作
bool CBaseLock::lock()
{
	if ( !this->bInitialized )
	{
		//printf("name lock error[%s]\n",name);
		throw CThreadException("No Initialize--Lock Error");
	}
	int state = 0;

	int* p = (int*)m_Synchronizer.getValue();
	//如果存在则证明曾经获取过锁资源，不再进行获取
	if ( p )
	{
		//增加计数
		m_nLockCount ++;
		return true;
	}

//*****************获取同步锁资源***************************
#ifdef WIN32
	if ( WaitForSingleObject(this->m_mutexObj,INFINITE) == WAIT_FAILED )
	{
	#ifdef _THDEBUG
			xprint("Lock Error[%d]",GetLastError());
	#endif
		return false;
	}
	state = 1;
#else
		int retval = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&state);
		retval = pthread_mutex_lock(&this->m_mutexObj);
#endif

#ifdef _THDEBUG
	#ifdef WIN32
		xprint("Lock Mutex[%d]",CThread::getCurrentThreadId());
	#endif
#endif

//**********************对线程本地变量进行赋值***********************
	p = (int*)(state+1);
	m_Synchronizer.setValue(p);
	m_nLockCount++;
#ifdef _DEBUG
	m_OwnThread = CThread::getCurrentThreadId();
#endif
	return true;
}

//对本对象解锁－－即同步操作完毕
bool CBaseLock::unlock()
{
	if ( !this->bInitialized )
		throw CThreadException("UnLock Error--Not Initialize");
//*****************获取线程本地变量并进行资源清除************************
	int *p = (int*)m_Synchronizer.getValue();
	if ( !p )
	{
		return false;
	}
	long state = (long)p-1;
	//计数为0才允许解锁
	if ( (--m_nLockCount) > 0 )
		return false;
	m_Synchronizer.setValue(0);

#ifdef _THDEBUG
	xprint("Release Mutex[%d]",CThread::getCurrentThreadId());
#endif

#ifdef WIN32
	if ( ! ReleaseMutex(this->m_mutexObj) )
	{
	#ifdef _THDEBUG
			xprint("Release Mutex Error[%d]",GetLastError());
	#endif
		return false;
	}
#else
	pthread_mutex_unlock(&this->m_mutexObj);
	pthread_setcancelstate(state,NULL);
	pthread_testcancel();
#endif

#ifdef _DEBUG
	#ifdef WIN32
		m_OwnThread = NULL;
	#else
		memset(&m_OwnThread,0,sizeof(m_OwnThread));
	#endif
#endif
	return true;
}

void  CBaseLock::notify()
{
	int *p = (int*)m_Synchronizer.getValue();
	if ( !p )
		throw CThreadException("Notify1 Wrong No in Synchronize Mode");

#ifdef WIN32
	PulseEvent(m_condObj);
#else
	pthread_cond_signal(&this->m_condObj);
#endif
}

void CBaseLock::notifyAll()
{
	int *p = (int*)m_Synchronizer.getValue();
	if ( !p )
		throw CThreadException("NotifyAll1 Wrong No in Synchronize Mode");

#ifdef WIN32
	SetEvent(m_condObj);
#else
	pthread_cond_broadcast(&this->m_condObj);
#endif
}

void CBaseLock::wait()
{
	if ( ! this->bInitialized )
		throw CThreadException("Wait Wrong No in Synchronize Mode");

	int *p = (int*)m_Synchronizer.getValue();
	if ( !p )
		throw CThreadException("Wait1 Wrong No in Synchronize Mode");

#ifdef WIN32
	HANDLE th = CThread::getCurrentThread();
	if ( th != NULL )
	{
	#ifdef _THDEBUG
			xprint("Current Handle[%d],Id[%d]",GetCurrentThread(),th);
	#endif
		if ( !ResetEvent(m_condObj) )
		{
	#ifdef _THDEBUG
			xprint("Reset Event Error[%d]",WSAGetLastError());
	#endif
		}
		//wait的时候进行统一的解锁
		int waitcount = m_nLockCount;
		m_nLockCount = 0 ;
		//因为后面会重新调用lock，所以这里需要把tsd变为0
		m_Synchronizer.setValue(0);
		SignalObjectAndWait(m_mutexObj,m_condObj,INFINITE,false);
		//关闭句柄资源
		CloseHandle(th);
		//重新获得资源
		this->lock();
		m_nLockCount = waitcount;
	}
	#ifdef _THDEBUG
		else
			xprint("Duplite Handle Error[%d]",GetLastError());
	#endif
#else//end of ifdef WIN32
	int waitcount = m_nLockCount;
	m_nLockCount = 0 ;
	#ifdef _THDEBUG
		xprint("Condition Wait in UNIX ");
		if (pthread_cond_wait(&this->m_condObj,&this->m_mutexObj) == 0 )
			xprint("Wait Unix Error [%d] ",errno);
	#else
		pthread_cond_wait(&this->m_condObj,&this->m_mutexObj);
	#endif//end of ifdef _THDEBUG
	m_nLockCount = waitcount;
#endif
}

void CBaseLock::wait(long waitmillsseconds)
{
	if ( ! this->bInitialized )
		throw CThreadException("Wait Wrong No in Synchronize Mode");
	int *p = (int*)m_Synchronizer.getValue();
	if ( !p )
		throw CThreadException("WaitTime1 Wrong No in Synchronize Mode");
#ifdef WIN32
	HANDLE th = CThread::getCurrentThread();
	if ( th != NULL )
	{
	#ifdef _THDEBUG
			xprint("Current Handle[%d],Id[%d]",GetCurrentThread(),th);
	#endif
		if ( !ResetEvent(m_condObj) )
		{
	#ifdef _THDEBUG
			xprint("Reset Event Error[%d]",WSAGetLastError());
	#endif
		}
		int waitcount = m_nLockCount;
		//所以不允许嵌套调用lock/unlock的时候调用了wait这种方法，会很危险。
		m_nLockCount = 0 ;
		//
		m_Synchronizer.setValue(0);
		SignalObjectAndWait(m_mutexObj,m_condObj,waitmillsseconds,false);
		//关闭句柄资源
		CloseHandle(th);
		//重新获得资源
		this->lock();
		m_nLockCount = waitcount;
	}
	#ifdef _THDEBUG
		else
			xprint("Duplite Handle Error[%d]",GetLastError());
	#endif
#else//end of ifdef WIN32
	int waitcount = m_nLockCount;
	m_nLockCount = 0 ;
	struct timespec delay;
	//获取当前机器时间
	if ( !clock_gettime(CLOCK_REALTIME,&delay) )
	{
		delay.tv_sec += (int)(waitmillsseconds / 1000);
		delay.tv_nsec += (waitmillsseconds % 1000)*1000000;
	}
	else
	{
		delay.tv_sec = time(NULL) + (int)(waitmillsseconds / 1000);
		//纳秒--10负9次方
		delay.tv_nsec = (waitmillsseconds % 1000)*1000000;
	}
	#ifdef _THDEBUG
		xprint("Condition Wait in UNIX ");
		if (pthread_cond_timedwait(&this->m_condObj,&this->m_mutexObj,&delay) == 0 )
		{
			perror("test");
			xprint("Wait Unix Error [%d] ",errno);
		}
	#else
		pthread_cond_timedwait(&this->m_condObj,&this->m_mutexObj,&delay);
	#endif//end of ifdef _THDEBUG
	m_nLockCount = waitcount;
#endif
}

