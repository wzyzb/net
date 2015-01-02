#ifndef _REACTOR_H_
#define _REACTOR_H_

#include "netactor.h"

class IReactor : public INetActor 
{
public:
    IReactor() 
    {

    }

    virtual ~IReactor()
    {
    }    
};

typedef CRefObject<IReactor> IReactorPtr;

#endif

