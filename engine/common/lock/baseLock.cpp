#include "baseLock.h"
#include "stdio.h"
#include "myException.h"


#ifdef _WIN32
#pragma warning(disable:4996)
#endif

int CBaseLock::m_serialno = 0;


CBaseLock::CBaseLock()
{
    m_nLockCount=0;
    this->m_bInitialized = false;
#ifdef WIN32
    sprintf(m_name,"PID[%d]::Synchronizer%d",GetCurrentProcessId(),m_serialno++);
#else
    sprintf(m_name,"PID[%d]::Synchronizer%d",getpid(),m_serialno++);
#endif

#ifdef WIN32
    m_mutexObj = NULL;
    m_condObj = NULL;
    m_OwnThread = NULL;
#endif
}


CBaseLock::~CBaseLock()
{
#ifdef WIN32
    if (m_condObj != NULL)
    {
        SetEvent(m_condObj);
    }
#else
    pthread_cond_broadcast(&this->m_condObj);
#endif

#ifdef WIN32
    if ( m_condObj != NULL )
    {
        CloseHandle(m_condObj);
    }
    if ( m_mutexObj != NULL )
    {
        CloseHandle(m_mutexObj);
    }
#else
    pthread_cond_destroy(&m_condObj);
    pthread_mutex_destroy(&m_mutexObj);
#endif
}

bool CBaseLock::initialize()
{
    if ( !m_Synchronizer.initialize() )
    {
        return false;
    }
   
#ifdef WIN32
    m_mutexObj = OpenMutex(MUTEX_ALL_ACCESS,false,m_name);
    if ( m_mutexObj != NULL )
    {
        CloseHandle(m_mutexObj);
        return false;
    }
    m_mutexObj = CreateMutex(NULL,false,m_name);
    if ( m_mutexObj == NULL )
    {
        return false;
    }

    strcat(m_name,"EVENT");
    m_condObj = OpenEvent(EVENT_ALL_ACCESS,false,m_name);
    if ( m_condObj != NULL )
    {
        CloseHandle(m_condObj);
        return false;
    }
    m_condObj = CreateEvent(NULL,true,false,m_name);
    if ( m_condObj == NULL )
    {
        CloseHandle(m_mutexObj);
        return false;
    }
#else
    int retval = 0;
    if ( (retval = pthread_mutex_init(&m_mutexObj,NULL)) != 0 )
    {
        return false;
    }
    if ( (retval = pthread_cond_init(&m_condObj,NULL) ) != 0 )
    {
        pthread_mutex_destroy(&m_mutexObj);
        return false;
    }
#endif
    m_bInitialized = true;
    return true;
}

bool CBaseLock::lock()
{
    if ( !this->m_bInitialized)
    {
        //
        ThrowException<CMyException>("CBaseLock::lock this->m_bInitialized = false,%s",m_name);
        return false;
    }
    int state = 0;
    int* p = (int*)m_Synchronizer.getValue();

    if ( p )
    {
        m_nLockCount++;
        return true;
    }
#ifdef WIN32
    if ( WaitForSingleObject(this->m_mutexObj,INFINITE) == WAIT_FAILED)
    {
        return false;
    }
    state = 1; //为什么设置 为 1
#else
    int retval = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&state);
    retval = pthread_mutex_lock(&this->m_mutexObj);
#endif
    p = (int*) (state + 1);
    m_Synchronizer.setValue(p);
    m_nLockCount++;

#ifdef _DEBUG
    //获取当前线程的id
#endif

    return true;
}

bool CBaseLock::unlock()
{
    if ( !this->m_bInitialized)
    {
        //
        ThrowException<CMyException>("CBaseLock::unlock this->m_bInitialized = false,%s",m_name);
        return false;
    }

    int *p = (int*)m_Synchronizer.getValue();
    if (!p)
    {
        return false;
    }
    long state = (long)p-1;
    if ((--m_nLockCount)>0)
    {
        return false;
    }

    m_Synchronizer.setValue(0);

#ifdef WIN32
    if ( !ReleaseMutex(this->m_mutexObj))
    {
        //这里需要输出打印log
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
    memset(&m_OwnThread,0,sizeof(m_OwnThread)); //pthread_t 是 unsigned long型
#endif
#endif
    return true;
}

void CBaseLock::notify()
{
    int* p =(int*)m_Synchronizer.getValue();
    if (!p)
    {
        //这里要加个log
        return ;
    }
#ifdef WIN32
    PulseEvent(m_condObj);
#else
    pthread_cond_signal(&this->m_condObj);
#endif
}

void CBaseLock::notifyAll()
{
    int* p = (int*)m_Synchronizer.getValue();
    if (!p)
    {
        //这里后面要加个log
        return;
    }
#ifdef  WIN32
    SetEvent(m_condObj);
#else
    pthread_cond_broadcast(&this->m_condObj);
#endif
}

void CBaseLock::wait()
{
    if ( !this->m_bInitialized)
    {
        //这里后面要加个log
        return;
    }
    int *p=(int*)m_Synchronizer.getValue();
    if (!p)
    {
        //这里后面要加个log
        return;
    }
#ifdef WIN32
    if ( !ResetEvent(m_condObj) )
    {
        //log
    }
    
    int waitcount = m_nLockCount;
    m_nLockCount = 0 ;
   
    m_Synchronizer.setValue(0);
    SignalObjectAndWait(m_mutexObj,m_condObj,INFINITE,false);
    this->lock();
    m_nLockCount = waitcount;

#else
    int waitcount = m_nLockCount;
    m_nLockCount = 0 ;
    pthread_cond_wait(&this->m_condObj,&this->m_mutexObj);
    m_nLockCount = waitcount;
#endif
}

void CBaseLock::wait(long waitmillsseconds)
{
    if ( ! this->m_bInitialized )
    {
        //log
        return ;
    }
    int *p = (int*)m_Synchronizer.getValue();
    if ( !p )
    {
        //log
        return ;
    }
#ifdef WIN32
   
        if ( !ResetEvent(m_condObj) )
        {
            //log
        }
        int waitcount = m_nLockCount;
        m_nLockCount = 0 ;
    
        m_Synchronizer.setValue(0);
        SignalObjectAndWait(m_mutexObj,m_condObj,waitmillsseconds,false);
        
        this->lock();
        m_nLockCount = waitcount;
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

    pthread_cond_timedwait(&this->m_condObj,&this->m_mutexObj,&delay);

    m_nLockCount = waitcount;
#endif
}

