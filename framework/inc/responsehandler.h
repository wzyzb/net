#ifndef _RESPONSEHANDLER_H_
#define _RESPONSEHANDLER_H_

#include <framework/current.h>

class IResponseHandler : public IRefShare
{
public:
    IResponseHandler()
    {
        
    }

    virtual ~IResponseHandler()
    {
        
    }

    virtual void onResponse(IResponsePtr Response, CCurrentPtr pCurrent, void* Param) = 0;
   
    virtual void onTimeout(void* Param) = 0;

    virtual IResponsePtr createResponse() = 0;

};

typedef CRefObject<IResponseHandler> IResponseHandlerPtr;

template <typename T>
class TResponseHandler : public IResponseHandler, public CRefShare
{
public:
    TResponseHandler(T* pClass, 
                    void (T::*pfnOnResponse)(IResponsePtr Response, CCurrentPtr pCurrent, void* Param), 
                    IResponsePtr (T::*pfnCreateResponse)(),
                    void (T::*pfnOnTimeOut)(void* Param) ) :
                    m_pClass(pClass),
                    m_pfnOnResponse(pfnOnResponse),  
                    m_pfnCreateResponse(pfnCreateResponse), 
                    m_pfnOnTimeOut(pfnOnTimeOut)
    {
    }

    virtual ~TResponseHandler()
    {
    }

	virtual void onResponse(IResponsePtr Response, CCurrentPtr pCurrent, void* Param)
	{
		(m_pClass->*m_pfnOnResponse)(Response, pCurrent, Param);
	}

    virtual void onTimeout(void* Param)
	{
	    (m_pClass->*m_pfnOnTimeOut)(Param);	
	}

    virtual IResponsePtr createResponse()
	{
		return (m_pClass->*m_pfnCreateResponse)();	
	}

protected:
    void incRef()
    {
        CRefShare::incRef();
    }

    bool decRef()
    {
        return CRefShare::decRef();
    } 

private:
    T* m_pClass;
    void (T::*m_pfnOnResponse)(IResponsePtr Response, CCurrentPtr pCurrent, void* Param);
	IResponsePtr (T::*m_pfnCreateResponse)();
	void (T::*m_pfnOnTimeOut)(void* Param);
};

#endif 

