#ifndef _PROACTOR_H_
#define _PROACTOR_H_

#include "netactor.h"

class IProactor : public INetActor
{
public:
    IProactor() {}
    virtual ~IProactor() {}

    virtual void run() = 0;
    virtual IProactor* Instance() = 0;
};

#endif
