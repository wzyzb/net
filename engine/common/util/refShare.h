#ifndef _REFSHARE_H_
#define _REFSHARE_H_

#include "automicOper.h"
#include "exception/otherException.h"

typedef void(*FreeRefFunc)(void*);

class  IRefShare
{
public:
    virtual ~IRefShare(){};
    virtual bool    decRef()=0;
    virtual void    incRef()=0;
};



class CRefShare:public IRefShare
{
private:
    atomic_t  m_nRefCount;
public:
    CRefShare();
    virtual ~CRefShare();
    CRefShare(const CRefShare&);
    CRefShare& operator=(const CRefShare&);

    void incRef();
    bool decRef();
    
    bool decRefEx(FreeRefFunc pFreeFunc);

    const atomic_t getRefCount()const {return m_nRefCount;}

    void CRefShare::setStackObject()
    {
        incRef();
    }
};


template<class C>
class TRefPointer
{
private :
    template<class T>
    class CInnerPointer : virtual public CRefShare
    {
    private :
        CInnerPointer(CInnerPointer& Right);
        CInnerPointer& operator=(CInnerPointer& Right);

    protected :
        friend class TRefPointer;
        T* m_pPointer;

    public :
        CInnerPointer() : m_pPointer(NULL) {}

        CInnerPointer(T* pObj) 
        {
            m_pPointer = pObj ;
        }

        ~CInnerPointer()  { delete m_pPointer ; }

        T* operator->()
        {
            if ( m_pPointer == NULL )  //ø’÷∏’Î≈◊“Ï≥£
                ThrowException<CNullPointException>("Null Point Exception");
            return m_pPointer;
        }

        const T* operator->() const
        {
            if ( m_pPointer == NULL ) //ø’÷∏’Î≈◊“Ï≥£
                ThrowException<CNullPointException>("Null Point Exception");
            return m_pPointer;
        }

        operator void*() const
        {
            return m_pPointer;
        }
    };

protected :
    CInnerPointer<C>* m_pRefObj;

public :
    TRefPointer() : m_pRefObj(NULL) { }

    TRefPointer(const TRefPointer& Right)
    { 
        m_pRefObj = Right.m_pRefObj;
        if ( m_pRefObj != NULL )
            m_pRefObj->incRef();
    }

    TRefPointer(C* pObj) 
    {
        if ( pObj != NULL )
        {
            m_pRefObj = new CInnerPointer<C>(pObj);
            m_pRefObj->incRef();
        }
        else
            m_pRefObj = NULL;
    }

    ~TRefPointer() 
    {
        if ( m_pRefObj != NULL ) 
            m_pRefObj->decRef();
    }

    TRefPointer& operator=(const TRefPointer& Right)
    {
        if ( m_pRefObj != NULL )
            m_pRefObj->decRef();
        m_pRefObj = Right.m_pRefObj;
        if ( m_pRefObj != NULL )
            m_pRefObj->incRef();
        return *this;
    }

    operator void*() const
    {
        if ( m_pRefObj == NULL )
            return NULL;
        return m_pRefObj->operator void *();
    }

    C* operator->()
    {
        if ( m_pRefObj == NULL )//ø’÷∏’Î≈◊“Ï≥£
            ThrowException<CNullPointException>("Null Point Exception");
        return m_pRefObj->operator->();
    }

    const C* operator->() const
    {
        if ( m_pRefObj == NULL )//ø’÷∏’Î≈◊“Ï≥£
            ThrowException<CNullPointException>("Null Point Exception");
        return m_pRefObj->operator ->();
    }
};

template <class T>
class CRefObject
{
protected :
    T* m_pRefObj;

public :
    CRefObject() : m_pRefObj(NULL) { }

    CRefObject(const CRefObject& Right)
    { 
        m_pRefObj = Right.m_pRefObj;
        if ( m_pRefObj != NULL )
        {
            m_pRefObj->incRef();
        }
    }

    CRefObject(T* pObj)
    {
        m_pRefObj = (T*)pObj;
        if ( m_pRefObj != NULL )
        {
            m_pRefObj->incRef();
        }
    }

    ~CRefObject() 
    {
        if ( m_pRefObj != NULL ) 
            m_pRefObj->decRef();
    }

    CRefObject& operator=(const CRefObject& Right)
    {
        if ( m_pRefObj != NULL )
            m_pRefObj->decRef();
        m_pRefObj = Right.m_pRefObj;
        if ( m_pRefObj != NULL )
        {
            m_pRefObj->incRef();
        }
        return *this;
    }

    CRefObject& operator=(T* pPointer)
    {
        if ( m_pRefObj != NULL )
            m_pRefObj->decRef();
        m_pRefObj = pPointer;
        if ( m_pRefObj != NULL )
        {
            m_pRefObj->incRef();
        }
        return *this;
    }

    operator void*() const
    {
        return m_pRefObj; 
    }

    T* operator->()
    {
        if ( m_pRefObj == NULL )
            ThrowException<CNullPointException>("Null Point Exception");
        return m_pRefObj;
    }

    const T* operator->() const
    {
        if ( m_pRefObj == NULL )
            ThrowException<CNullPointException>("Null Point Exception");
        return m_pRefObj;
    }

    T* get()
    {
        return m_pRefObj;
    }

    const T* get() const
    {
        return m_pRefObj;
    }

    static CRefObject<T> createInstance()
    {
        return CRefObject<T>(new T());
    }
};



#endif
