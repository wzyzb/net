#ifndef _SERIAL_H
#define _SERIAL_H

#include <io/inputstream.h>
#include <io/outputstream.h>
#include <util/refshare.h>

class ISerial : public CRefShare
{
public:
    ISerial() 
    {

    }    
    
    virtual ~ISerial()
    {

    }

    /**
     *
     */
    virtual bool serial(char* pOut, int &Length) = 0;

    /**
     *
     */
    virtual bool unserial(const char* pIn, int Length) = 0;
 
    /**
     *
     */
    virtual bool serial(IOutputStream& Output) = 0;

    /**
     *
     */
    virtual bool unserial(IInputStream& Input) = 0;
};

typedef CRefObject<ISerial> ISerialPtr;

#endif 
