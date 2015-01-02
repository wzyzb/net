#include <public.h>

#ifndef WINDOWS
#include <net/netactor/unix/linuxnetactordriver.h>
#include <net/netactor/unix/linuxreactorimpl.h>
#include <net/address.h>
#include <net/connection.h>

CLinuxNetActorDriver::CLinuxNetActorDriver()
{

}

CLinuxNetActorDriver::~CLinuxNetActorDriver()
{

}
    
const char* CLinuxNetActorDriver::getDriverName()
{
    return "NetActorDriver";
}

const char* CLinuxNetActorDriver::getClassName()
{
    return "CLinuxNetActorDriver";
}

INetActorPtr CLinuxNetActorDriver::getNetActorInstance()
{
    return CLinuxReactorImpl::getInstance();
}
#endif
