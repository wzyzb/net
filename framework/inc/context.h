#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include <framework/serial.h>

class DLLAPI CContext : public ISerial
{
public:
    void set(const char* Name, IRefShare* Value);
    IRefShare* get(const char* Name);

    /**
     *
     */
    virtual bool serial(char* pOut, int &Length);

    /**
     *
     */
    virtual bool unserial(const char* pIn, int Length);

    /**
     *
     */
    virtual bool serial(IOutputStreamPtr Output);

    /**
     *
     */
    virtual bool unserial(IInputStreamPtr Input);

};

typedef CRefObject<CContext> CContextPtr;

#endif

