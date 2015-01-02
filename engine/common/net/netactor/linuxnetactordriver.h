#ifndef _LINUXNETACTORDRIVER_H_
#define _LINUXNETACTORDRIVER_H_

#include <net/netactor/netactordriver.h>
#include <util/refshare.h>

class CLinuxNetActorDriver : public INetActorDriver
{
public:
    CLinuxNetActorDriver();
    virtual ~CLinuxNetActorDriver();
    
    /**
     *
     */
    virtual const char* getDriverName();

    /**
     *
     */
    virtual const char* getClassName();

    virtual INetActorPtr getNetActorInstance();

};

#endif
