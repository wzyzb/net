#if !defined(_BASELOCK_H_)
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
#include "mylock.h"

/*
线程锁
*/
class CBaseLock : public ILock
{
private :
	//锁计数器
	unsigned int m_nLockCount;
	static int serialno;
	MUTEXHANDLE m_mutexObj;
	CONDHANDLE  m_condObj;
	//唯一标识的名字
	char name[100];
	//标识当前线程是否已经获得同步资源
	//线程数据－－标识当前线程是否已经获得同步资源
	CTlObject m_Synchronizer;
	bool bInitialized ;
	CBaseLock(const CBaseLock& Other);
	CBaseLock& operator= (const CBaseLock& Other);
	THREADID m_OwnThread;

public :
	CBaseLock();
	virtual ~CBaseLock();
	//初始化所有资源
	bool initialize();
	//对同步资源上锁－－并对每个线程创建一个指针
	bool lock();
	//对同步资源解锁－－并对每个线程自己的指针进行删除
	bool unlock();
	//让当前线程进入等待
	void wait();
	//让线程进入等待状态，并当时间(单位：毫秒)到达后苏醒
	void wait(long waitmillsseconds);
	//唤醒所有正在等待的线程
	void notifyAll();
	//唤醒指定的线程
	void notify();
};


#endif
