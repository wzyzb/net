#include "lightLock.h"


#ifdef _WIN32
CLightLock::CLightLock()
{
    InitializeCriticalSectionAndSpinCount(&m_Lock,1000);
}

CLightLock::~CLightLock()
{
    DeleteCriticalSection(&m_Lock);
}

bool CLightLock::lock()
{
    EnterCriticalSection(&m_Lock);
    return true;
}   

bool CLightLock::unlock()
{
    LeaveCriticalSection(&m_Lock);
    return true;
}

#elif _USE_SPINLOCK
CLightLock::CLightLock()
{
    pthread_spin_init(&m_Lock,PTHREAD_PROCESS_PRIVATE);
}

CLightLock::~CLightLock()
{
    pthread_spin_destroy(&m_Lock);
}

bool CLightLock::lock()
{
    pthread_spin_lock(&m_Lock);
    return true;
}   

bool CLightLock::unlock()
{
    pthread_spin_unlock(&m_Lock);
    return true;
}

#else
CLightLock::CLightLock()
{
    pthread_mutex_init(&m_Lock,NULL);
}

CLightLock::~CLightLock()
{
   pthread_mutex_destroy(&m_Lock);
}

bool CLightLock::lock()
{   
    pthread_mutex_lock(&m_Lock);
    return true;
}   

bool CLightLock::unlock()
{
    pthread_mutex_unlock(&m_Lock);
    return true;
}

#endif