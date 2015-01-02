#ifndef _BASELOCK_H_
#define _BASELOCK_H_


#ifdef WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x410
#endif
#include <windows.h>
#include <winbase.h>
#else
#include <pthread.h>
#include <signal.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef THREADHANDLE
#undef THREADHANDLE
#endif
#ifdef THREADID
#undef THREADID
#endif

#ifdef WIN32
#define MUTEXHANDLE HANDLE
#define CONDHANDLE HANDLE
#define THREADHANDLE HANDLE
#define THREADID unsigned int
#else
#define MUTEXHANDLE pthread_mutex_t
#define CONDHANDLE  pthread_cond_t
#define THREADHANDLE pthread_t
#define THREADID pthread_t
#endif

#include "tlobject.h"
#include "autoLock.h"

class CBaseLock:public ILock
{
private:
    unsigned int m_nLockCount;
    static int   m_serialno;
    MUTEXHANDLE  m_mutexObj;
    CONDHANDLE   m_condObj;
    char         m_name[100];
    CTlObject    m_Synchronizer;
    bool         m_bInitialized ;
    THREADID     m_OwnThread;
    CBaseLock(const CBaseLock& Other);
    CBaseLock& operator= (const CBaseLock& Other);
public:
    CBaseLock();
    ~CBaseLock();

    bool initialize();

    bool lock();

    bool unlock();

    void wait();

    void wait(long waitmillsseconds);

    void notifyAll();
    void notify();
};


#endif