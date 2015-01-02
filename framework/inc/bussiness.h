#ifndef _BUSSINESS_H_
#define _BUSSINESS_H_

#include <framework/current.h>

class IBussiness : public CRefShare
{
public:
    IBussiness() { }
    virtual ~IBussiness() {  } 

    virtual CCurrentPtr getCurrent() = 0;
	virtual uint32 getServantName() = 0;
    
    virtual void execute() = 0;
};

typedef CRefObject<IBussiness> IBussinessPtr;

#endif 

