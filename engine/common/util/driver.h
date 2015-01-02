#ifndef _DRIVER_H_
#define _DRIVER_H_

#include "refShare.h"

class IDriver : public CRefShare
{
public:
    IDriver(){}
    virtual ~IDriver(){}

    virtual const char* getDriverName()=0;
    virtual const char* getClassName()=0;
};

typedef CRefObject<IDriver> IDriverPtr;

#endif