#ifndef _RESPONSE_H_
#define _RESPONSE_H_

#include <framework/serial.h>

class IResponse : public ISerial
{
public:
    IResponse()
    {
    }
    
    virtual ~IResponse()
    {
        
    }
};

typedef CRefObject<IResponse> IResponsePtr;

#endif 

