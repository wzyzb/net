#ifndef _SESSIONHANDLER_H_
#define _SESSIONHANDLER_H_

#include <framework/session.h>

class ISessionHandler : public IRefShare
{
public:
    ISessionHandler()
    {
        
    }

    virtual ~ISessionHandler()
    {
        
    }

    virtual void onSessionClose(CSessionPtr pSession) = 0;
   
};

typedef CRefObject<ISessionHandler> ISessionHandlerPtr;

#endif 

