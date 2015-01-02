#include "autoLock.h"
#include <stdlib.h>


CAutoLock::CAutoLock(ILock& baseLock)
{
    if (baseLock.lock())
    {
        m_lock=&baseLock;
    }
    else
    {
        m_lock=NULL;
    }
}

CAutoLock::~CAutoLock()
{
    if(m_lock!=NULL)
    {
        m_lock->unlock();
        m_lock=NULL;
    }
}