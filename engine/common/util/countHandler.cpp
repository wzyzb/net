#include "countHandler.h"
#include "objectMgr.h"

#include <strstream>

CCountHandler::CCountHandler(const std::string& name)
:_name(name)
{
    CObjectManager::registCountHandler(this);
}

CCountHandler::~CCountHandler()
{
    CObjectManager::removeCountHandler(this);
}

void CCountHandler::print(IIOHandler* handle)
{
    std::strstream stream;
    handle->write(stream.str(),stream.pcount(),NULL);
    stream.freeze(false);
}
