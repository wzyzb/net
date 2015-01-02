#include "objectMgr.h"
#include "countHandler.h"

CObjectManager CObjectManager::_initManager;

CObjectManager::CObjectManager()
{
    CObjectManager::getSingletonLock();
    CObjectManager::getAtomicLock();
    CObjectManager::getCountHandlersLock();
    CObjectManager::getCountHandlers();
}

CObjectManager::~CObjectManager()
{

}

CLightLock& CObjectManager::getSingletonLock()
{
    static  CLightLock lock;
    return lock;
}

CLightLock& CObjectManager::getAtomicLock()
{
    static CLightLock lock;
    return lock;
}

CLightLock& CObjectManager::getCountHandlersLock()
{
    static  CLightLock lock;
    return lock;
}

std::list<CCountHandler*>& CObjectManager::getCountHandlers()
{
    static std::list<CCountHandler*>    countHandlersList;
    return countHandlersList;
}

void CObjectManager::print(IIOHandler* handle)
{
    CAutoLock l(getCountHandlersLock());
    std::list<CCountHandler*>& listCountHandler = getCountHandlers();
    std::list<CCountHandler*>::iterator iter;
    for (iter=listCountHandler.begin();iter != listCountHandler.end(); iter++)
    {
        (*iter)->print(handle);
    }
}

void CObjectManager::registCountHandler(CCountHandler* handler)
{
    CAutoLock l( getCountHandlersLock() );
    getCountHandlers().push_back( handler );
}

void CObjectManager::removeCountHandler( CCountHandler* handler )
{

    CAutoLock l( getCountHandlersLock() );
    getCountHandlers().remove( handler );
}

void CObjectManager::clearCountHandlers()
{
    CAutoLock l( getCountHandlersLock() );
    getCountHandlers().clear();
}
