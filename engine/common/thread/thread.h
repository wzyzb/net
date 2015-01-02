#ifndef _THREAD_H_
#define _THREAD_H_

#ifdef _WIN32
#pragma warning (disable:4786)
#endif

#include "vector"
#include "map"
#include "errno.h"
#include "exception"
using namespace std;

#ifdef _WIN32

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x410
#endif

#include "windows.h"
#include "winbase.h"

#else

#include <pthread.h>
#include <signal.h>
#include <sched.h>

#endif


#ifndef PTHREAD_THREADS_MAX
#define PTHREAD_THREADS_MAX  1000
#endif

#ifdef STARTROUTINE
#undef STARTROUTINE
#endif

#ifdef THREADHANDLE
#undef THREADHANDLE
#endif

#ifdef THREADID
#undef THREADID
#endif

#ifdef MUTEXHANDLE
#undef MUTEXHANDLE
#endif

#ifdef _WIN32
#define STARTROUTINE unsigned __stdcall
#define THREADHANDLE HANDLE
#define THREADID     unsigned int
#define MUTEXHANDLE  HANDLE
#define CONDHANDLE   HANDLE
#define TSD          DWORD

#else
#define STARTROUTINE void*
#define THREADHANDLE pthread_t
#define THREADID     pthread_t
#define MUTEXHANDLE  pthread_mutex_t
#define CONDHANDLE   pthread_cond_t
#define TSD          pthread_key_t

#endif

STARTROUTINE ThreadCall(void* param);


/*pthread_join 的返回 
ESRCH
描述:
没有找到与给定的线程 ID 相对应的线程。

EDEADLK
描述:
将出现死锁，如一个线程等待其本身，或者线程 A 和线程 B 互相等待。

EINVAL
描述:
与给定的线程 ID 相对应的线程是分离线程。
*/
#ifndef ESRCH 
#define ESRCH 3
#endif

class CRunnable  //线程抽象
{
public:
    virtual ~CRunnable()=0;
    virtual void run() = 0;
};

class CThreadFactory;
class CRunThread;


class CThread
{
    friend class CThreadFactory;
private:
    CRunnable*      m_runnable;
    void*           m_pParamter;
    THREADHANDLE    m_hThread;
    THREADID        m_pThreadId;
    bool            m_bDeamon;
    CRunThread*     m_RunThread;
    CThread(const CThread&);
    CThread& operator=(const CThread&);
protected:
    CThread(CRunnable* runnable);
    bool            m_bAutoDel;
public:
    ~CThread();

    bool                   start();
    bool                   stop();

    int                    getPriority();
    bool                   setPriority(int nPriority);
    bool                   setMaxPriority();
    bool                   setMinPriority();

    static  void            yield();

    THREADHANDLE            getThreadHandle();
    static THREADHANDLE     getCurrentThread();
    static THREADID         getCurrentThreadId();

#ifdef sleep
#undef sleep
#endif
    static  void            sleep(long mSeconds);
    bool                    isDeamon();
    void                    setDeamon(bool bDeamon);
    bool                    isRunning();
private:
    void                    freeResource();
};

class   CRunThread
{
    friend class CThread;
public:
    bool m_AutoDeleted;
    CThread* m_thHandle;
    CRunnable* m_Runnable;
protected:
    CRunThread(CRunnable* runnable,CThread* m_Handle,bool m_AutoDeleted = false);
    ~CRunThread();
};

class CThreadFactory
{
    friend class CThread;
private :
    static int m_nThreadCount;
    static THREADHANDLE m_ThreadArray[PTHREAD_THREADS_MAX];
private:
    static bool m_bInitialized;
public:
    static bool initialize();
    static bool finalize();

    static CThread* createThread(CRunnable& runnable,bool AutoDel = false);
    static int  getThreadCount();
};








#endif