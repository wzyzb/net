#include "thread.h"
#include "stdio.h"
#include "../exception/otherException.h"

#ifdef WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

CRunnable::~CRunnable()
{

}


//===============CThread=================

CThread::CThread(CRunnable* runnable)
{
    this->m_runnable = runnable;
#ifdef  WIN32
    this->m_hThread = NULL;
#else
    m_hThread   = 0;
#endif
    this->m_bDeamon = false;
    this->m_pParamter = NULL;
    this->m_RunThread = new CRunThread(runnable,this);
}

CThread::~CThread()
{
    this->freeResource();
    if (this->m_RunThread != NULL)
    {
        delete  m_RunThread;
    }
#ifdef WIN32
    if ( m_hThread == NULL )
    {
        CloseHandle(this->m_hThread);
    }
#else
    if ( m_hThread == 0 )
    {
        //join完之后，不需要再detach
        //pthread_detach(this->m_hThread);
    }
#endif
        return ;
}

bool  CThread::start()
{
#ifdef WIN32
    if ( this->m_hThread != NULL )
    {
        return false;
    }
#endif

    this->m_RunThread->m_AutoDeleted = this->m_bAutoDel;

#ifdef WIN32
    this->m_hThread = (HANDLE)_beginthreadex(NULL,0,ThreadCall,this->m_RunThread,0,&m_pThreadId);
#else
    this->m_pThreadId = pthread_create(&this->m_hThread,NULL,ThreadCall,this->m_RunThread);
    this->m_pThreadId = this->m_hThread;
#endif

    if ( ! m_bDeamon )
    {
        if ( CThreadFactory::m_nThreadCount >= PTHREAD_THREADS_MAX )
            throw new CThreadException("被创建的线程数已经达到本版本最大数目，不能再进行创建");
        for ( int i = 0 ; i < CThreadFactory::m_nThreadCount ; i ++ )
        {
            if ( CThreadFactory::m_ThreadArray[i] == 0 )
            {
                CThreadFactory::m_ThreadArray[i] = this->getThreadHandle();
                return true;
            }
        }
        CThreadFactory::m_ThreadArray[CThreadFactory::m_nThreadCount++] = this->getThreadHandle();
    }
    else
    {
#ifndef WIN32
        pthread_detach(m_hThread);
#endif
    }
    return true;
}

bool CThread::stop()
{
#ifdef  WIN32
    BOOL    retval = TerminateThread(this->m_hThread,-1);
    return  retval;
#else
    int     retval = pthread_cancel(this->m_hThread);
    return  retval == 0 ? true : false;
#endif
}


THREADHANDLE    CThread::getCurrentThread()
{
#ifdef WIN32
    HANDLE  th;
    if (DuplicateHandle(GetCurrentProcess(),GetCurrentThread(),GetCurrentProcess(),
        &th,THREAD_ALL_ACCESS,false,DUPLICATE_SAME_ACCESS))
    {
        return th;
    }
    else
        return NULL;
#else
    return  pthread_self();
#endif
}

THREADID    CThread::getCurrentThreadId()
{
#ifdef WIN32
    return ::GetCurrentThreadId();
#else
    return pthread_self();
#endif
}

void  CThread::sleep(long millSec)
{
#ifdef WIN32
    Sleep(millSec);
#else
    struct timespec delay,rem;
    delay.tv_sec = (int)(millSec / 1000);
    //纳秒--10负9次方
    delay.tv_nsec = (millSec % 1000)*1000000;
    //	pthread_delay_np(&delay);
    nanosleep(&delay,&rem);
#endif
}

THREADHANDLE CThread::getThreadHandle()
{
    return this->m_hThread;
}

void CThread::yield()
{
#ifdef WIN32
    Sleep(10);
#elif _AIX_PTHREADS_D7
    pthread_yield();
#endif
}

STARTROUTINE ThreadCall(void *parm)
{
#ifdef LINUX
/* linux下线程特别对信号(SIGINT/SIGTERM)进行单独屏蔽，
	否则经常出现线程在wait的时候对信号处理时出现一些内存异常*/
	sigset_t nmask,omask;
	sigemptyset(&nmask);
	sigaddset(&nmask,SIGINT);
	sigaddset(&nmask,SIGTERM);
	pthread_sigmask(SIG_BLOCK,&nmask,&omask);
#endif
	int retcode = 0;
	try
	{
		((CRunThread*)parm)->m_Runnable->run();
	}
	catch (exception* e)
	{
		delete e;
		retcode = -100;
	}
	if ( ((CRunThread*)parm)->m_AutoDeleted )
	{
		delete ((CRunThread*)parm)->m_Runnable;
		delete ((CRunThread*)parm)->m_thHandle;
	}
#ifdef WIN32
		_endthreadex(retcode);
		return retcode;
#else
		return (void*)retcode;
#endif
}

int CThread::getPriority()
{
    int priority;
#ifdef WIN32
    priority = GetThreadPriority(this->m_hThread);
#else
    struct sched_param param;
    int policy;
    if ( pthread_getschedparam(m_hThread,&policy,&param) )
        return -1;
    priority = param.sched_priority;
#endif
    return priority;
}

bool CThread::setPriority(int nPriority)
{
    bool success = false;
#ifdef WIN32
    if ( SetThreadPriority(m_hThread,nPriority) == TRUE )
        success = true;
#else
    struct sched_param param;
    int policy;
    if ( pthread_getschedparam(m_hThread,&policy,&param) )
        return false;
    param.sched_priority = nPriority;
    if ( pthread_setschedparam(m_hThread,policy,&param) == 0 )
        success = true;
#endif
    return success;
}

bool CThread::setMaxPriority()
{
    bool success = false;
#ifdef WIN32
    if ( SetThreadPriority(m_hThread,THREAD_PRIORITY_TIME_CRITICAL) == TRUE )
        success = true;
#else
    struct sched_param param;
    int policy;
    if ( pthread_getschedparam(m_hThread,&policy,&param) )
        return false;
    param.sched_priority = sched_get_priority_max(policy);
    if ( pthread_setschedparam(m_hThread,policy,&param) == 0 )
        success = true;
#endif
    return success;
}

bool CThread::setMinPriority()
{
    bool success = false;
#ifdef WIN32
    if ( SetThreadPriority(m_hThread,THREAD_PRIORITY_IDLE) == TRUE )
        success = true;
#else
    struct sched_param param;
    int policy;
    if ( pthread_getschedparam(m_hThread,&policy,&param) )
        return false;
    param.sched_priority = sched_get_priority_min(policy);
    if ( pthread_setschedparam(m_hThread,policy,&param) == 0 )
        success = true;
#endif
    return success;
}


bool CThread::isDeamon()
{
    return m_bDeamon;
}

void CThread::setDeamon(bool bDeamon)
{
    this->m_bDeamon = bDeamon;
}

bool CThread::isRunning()
{
#ifdef WIN32
    DWORD ExitCode;
    if ( GetExitCodeThread(this->m_hThread,&ExitCode) )
    {
        if ( ExitCode == STILL_ACTIVE )
            return true;
    }
#else
    struct sched_param param;
    int policy = SCHED_OTHER;
    int retval = pthread_getschedparam(this->getThreadHandle(),&policy,&param);
    if ( retval != ESRCH )
        return true;
#endif
    return false;
}

//删除同步变量中的指针，删除线程工厂中的数组变量
void CThread::freeResource()
{
	//释放线程工厂保存线程句柄的线程数组资源
	if ( !this->m_bDeamon )
	{
		for ( int i = 0 ; i < CThreadFactory::m_nThreadCount ; i ++ )
		{
			if ( this->getThreadHandle() == CThreadFactory::m_ThreadArray[i] )
			{
				CThreadFactory::m_ThreadArray[i] = 0;
				break;
			}
		}
	}
}


//==================CThreadFactory=======================
int     CThreadFactory::m_nThreadCount = 0;
bool CThreadFactory::m_bInitialized = false;
THREADHANDLE CThreadFactory::m_ThreadArray[PTHREAD_THREADS_MAX];

bool CThreadFactory::initialize()
{
    m_nThreadCount = 0;
    m_bInitialized = true;
    return true;
}

//等待所有线程结束
bool CThreadFactory::finalize()
{
    m_bInitialized = false;
#ifdef WIN32
    for ( int i = 0 ; i < m_nThreadCount ; i ++ )
        if ( m_ThreadArray[i] != 0 )
            WaitForSingleObject(m_ThreadArray[i],INFINITE);
#else
    for ( int i = 0 ; i < m_nThreadCount ; i ++ )
        if ( m_ThreadArray[i] != 0 )
            pthread_join(m_ThreadArray[i],NULL);
#endif
    return true;
}

//创建一新线程并运行与记录其线程数据
CThread* CThreadFactory::createThread(CRunnable& runnable,bool AutoDel)
{
    if ( !m_bInitialized )
        return NULL;
    if ( m_nThreadCount > PTHREAD_THREADS_MAX )
        return NULL;
    CThread* thread = new CThread(&runnable);
    thread->m_bAutoDel  = AutoDel;
    return thread;
}

int  CThreadFactory::getThreadCount()
{
    return m_nThreadCount;
}

//=============CRunThread================

CRunThread::CRunThread(CRunnable* runnable,CThread* m_Handle,bool m_AutoDeleted)
{
    this->m_Runnable = runnable;
    this->m_thHandle = m_Handle;
    this->m_AutoDeleted = m_AutoDeleted;
}

CRunThread::~CRunThread()
{
}