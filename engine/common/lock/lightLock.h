#ifndef _LIGHTLOCK_H_
#define _LIGHTLOCK_H_

#include "autoLock.h"
#include "public.h"

class CLightLock : public ILock
{
protected:
#ifdef _WIN32
    CRITICAL_SECTION m_Lock;
#elif _USE_SPINLOCK
    pthread_spinlock_t m_Lock;
#else
    pthread_mutex_t m_Lock;
#endif
public:
    CLightLock();
    ~CLightLock();

    bool lock();
    bool unlock();

};

#endif