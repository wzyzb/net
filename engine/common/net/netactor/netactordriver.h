#ifndef _NETACTORDRIVER_H_
#define _NETACTORDRIVER_H_

#include "netactor.h"
#include "../../util/driver.h"
#include "../../util/refshare.h"



class INetActorDriver : public IDriver
{
public:
    INetActorDriver() {}
    virtual ~INetActorDriver() {}

    virtual INetActorPtr getNetActorInstance() = 0;
    const char* getDriverName() { return "NetActorDriver"; }
    static INetActorPtr getNetActor();
};


typedef CRefObject<INetActorDriver> INetActorDriverPtr;

#endif
