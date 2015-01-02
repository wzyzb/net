#ifndef _OBJECTMGR_H_
#define _OBJECTMGR_H_

#include "public.h"
#include "lock/lightLock.h"
#include "handler.h"

class CAtomic;
class CCountHandler;


class CObjectManager
{
private:
    CObjectManager();
    virtual ~CObjectManager();

public:
    static void print(IIOHandler* handle);

    static CLightLock& getSingletonLock();

    static CLightLock& getAtomicLock();

    static void registCountHandler(CCountHandler* handler);
    static void removeCountHandler(CCountHandler* handler);

    static void clearCountHandlers();
public:

private:
    static CLightLock&  getCountHandlersLock();
    static std::list<CCountHandler*>& getCountHandlers();
private:
    static CObjectManager _initManager;
};

class CAutoRun
{
public:
    template<typename T>
    CAutoRun(T& t)
    {
        t();
    }
    CAutoRun(const CAutoRun&)
    {

    }
};



#endif