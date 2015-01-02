#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <framework/serial.h>
#include <framework/endpoint.h>
#include <framework/session.h>
#include <framework/context.h>

class IRequest : public ISerial
{
public:
    IRequest()
    {
//        m_bIsCritical = false;
    }

    virtual ~IRequest()
    {
        
    }

/*
    virtual CEndPointPtr getEndPoint()
    {
        return m_pEndPoint;
    }
    virtual CSessionPtr getSession()
    {
        return m_pSession;
    }

    virtual CContextPtr getGlobalContext()
    {
        return m_pContext;
    }
    */

//    virtual bool serial(char* pOut, int &Length) = 0;

//    virtual bool unserial(const char* pIn, int Length) = 0;

//    virtual bool serial(IOutputStreamPtr Output) = 0;

//    virtual bool unserial(IInputStreamPtr Input) = 0;

//    void setCritical(bool b)
//    {
//        m_bIsCritical = b;
//    }

private:
//    CEndPointPtr m_pEndPoint;
//    CSessionPtr  m_pSession;
//    CContextPtr  m_pContext;
//    bool         m_bIsCritical;
};

typedef CRefObject<IRequest> IRequestPtr;

#endif 

