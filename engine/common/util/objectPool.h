#ifndef _OBJECTPOOL_H_
#define _OBJECTPOOL_H_

#include "lock/lightLock.h"
#include "autoLock.h"
#include "refShare.h"
#include "exception/otherException.h"
#include "singleton.h"
#include "stack"
#include "autoPtr.h"

template<class T>
class CPoolObjectAutoPtr;

template<class T>
class CPoolObjectSharePtr;


template<typename T,typename PTR=CPoolObjectAutoPtr<T> >
class CObjectPool : public CSingleTon<CObjectPool<T,PTR> >
{
protected:
    friend class CSingleTon<CObjectPool<T,PTR> >;
    CObjectPool(){}
    ~CObjectPool();
public:
    typedef CObjectPool<T,CPoolObjectSharePtr<T> >  share_pool;
    uint32  size() {return m_ObjStack.size();}
    PTR  alloc(const char* pInfo = "");
    void free( T* obj);
    void foreach(void (*pFunc)(T*,const char* pInfo));
private:
    CLightLock  m_Lock;
    stack<T*>   m_ObjStack;
#ifdef _DEBUG
    map<T*,const char*> m_PointerInfo;
#endif
};


template<class T,class PTR>
PTR CObjectPool<T,PTR>::alloc(const char* pInfo)
{
    CAutoLock autoloc(m_Lock);
    T* pobj = NULL;
    if (m_ObjStack.size() > 0)
    {
        pobj = m_ObjStack.top();
        m_ObjStack.pop();
    }
    else
    {
        pobj = new T;
    }
#ifdef _DEBUG
    m_PointerInfo[pobj] = pInfo;
#endif
    return PTR(pobj);
}

template<class T,class PTR>
void CObjectPool<T,PTR>::free(T* obj)
{
    obj->reset();
    CAutoLock autoloc(m_Lock);
    m_ObjStack.push(obj);
#ifdef _DEBUG
    typename map<T*,const char*>::iterator i = m_PointerInfo.find(obj);
    if ( i != m_PointerInfo.end() )
    {
        m_PointerInfo.erase(i);
    }
#endif
}

template<class T,class PTR>
void CObjectPool<T,PTR>::foreach(void (*pFunc)(T* p,const char*))
{
#ifdef _DEBUG
    CAutoLock autoloc(m_Lock);
    for ( typename map<T*,const char*>::iterator i = m_PointerInfo.begin() ; i != m_PointerInfo.end() ; i ++ )
    {
        (*pFunc)(i->first,i->second);
    }
#endif
}


//析构函数，释放所有内存
template<class T,class PTR>
CObjectPool<T,PTR>::~CObjectPool()
{
    CAutoLock autoloc(m_Lock);
    while ( m_ObjStack.size() > 0 )
    {
        delete m_ObjStack.top();
        m_ObjStack.pop();
    }
#ifdef _DEBUG
    m_PointerInfo.clear();
#endif
}

template<typename T>
class CPoolObjectAutoPtr:public CAutoPtr<T>
{
    CPoolObjectAutoPtr(T* P):CAutoPtr<T>(P){}
    ~CPoolObjectAutoPtr()
    {
        T* pointer = this->release();
        if ( pointer )
        {
            CObjectPool<T>::getInstance()->free(pointer);
        }
    }

    CPoolObjectAutoPtr& operator=(const CPoolObjectAutoPtr& Other)
    {
        T* pointer = this->release();
        if ( pointer != NULL )
            CObjectPool<T>::getInstance()->free(pointer);
        this->m_pPointer = ((CPoolObjectAutoPtr&)Other).release();
        return *this;
    }
};


template <class T>
class CPoolObjectSharePtr
{
protected :
	T* m_pRefObj;

public :
	static void free(void* pPointer)
	{
		if ( pPointer )
		{
			T* p = static_cast<T*>(pPointer);
			//放回内存池
			CObjectPool<T>::share_pool::getInstance()->free(p);
		}
	}

	CPoolObjectSharePtr() : m_pRefObj(NULL) { }

	CPoolObjectSharePtr(const CPoolObjectSharePtr& Right)
	{ 
		m_pRefObj = Right.m_pRefObj;
		if ( m_pRefObj != NULL )
		{
			m_pRefObj->incRef();
		}
	}

	CPoolObjectSharePtr(T* pObj) 
	{
		m_pRefObj = pObj ;
		if ( m_pRefObj != NULL )
		{
			m_pRefObj->incRef();
		}
	}

	~CPoolObjectSharePtr() 
	{
		if ( m_pRefObj != NULL ) 
			m_pRefObj->decRefEx(CPoolObjectSharePtr<T>::free);
	}

	CPoolObjectSharePtr& operator=(const CPoolObjectSharePtr& Right)
	{
		if ( m_pRefObj != NULL )
			m_pRefObj->decRefEx(CPoolObjectSharePtr<T>::free);
		m_pRefObj = Right.m_pRefObj;
		if ( m_pRefObj != NULL )
		{
			m_pRefObj->incRef();
		}
		return *this;
	}

	CPoolObjectSharePtr& operator=(T* pPointer)
	{
		if ( m_pRefObj != NULL )
			m_pRefObj->decRefEx(CPoolObjectSharePtr<T>::free);
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
		//空指针抛异常
		if ( m_pRefObj == NULL )
			ThrowException<CNullPointException>("Null Point Exception");
		return m_pRefObj;
	}

	const T* operator->() const
	{
		//空指针抛异常
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

	/**
	 * 生成对象
	 */
	static CPoolObjectSharePtr<T> createInstance(const char* pInfo = "")
	{
		return CPoolObjectSharePtr<T>(CObjectPool<T>::share_pool::getInstance()->alloc(pInfo));
	}
};

#endif