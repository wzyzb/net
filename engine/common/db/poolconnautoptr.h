#if !defined(_POOLCONNAUTOPTR_H_)
#define _POOLCONNAUTOPTR_H_
#include "../util/autoptr.h"
#include "../thread/thread.h"

/**
 * 数据库连接智能指针
 */
template<class T>
class CPoolConnAutoPtr:public CAutoPtr<T>	
{
public :
	//简单的拷贝构造函数实现，如果发现赋值后智能指针不再保存
	//每次只有一个智能指针保存。
	CPoolConnAutoPtr(const CPoolConnAutoPtr& Other):CAutoPtr<T>((CAutoPtr<T>&)Other)
	{
		//CAutoPtr::CAutoPtr((CAutoPtr&)Other);
		//m_pPointer = Other.m_pPointer;
		//((CPoolConnAutoPtr&)Other).m_pPointer = NULL;
	}

	/**
	 * 构造函数
	 */
	CPoolConnAutoPtr():CAutoPtr<T>(NULL)
	{
		//m_pPointer = NULL;
	}

	/**
	 * 构造函数
	 */
	CPoolConnAutoPtr(T* Pointer):CAutoPtr<T>(Pointer)
	{		
		//CAutoPtr::m_pPointer = Pointer;
		if ( Pointer != NULL )
			Pointer->m_nOwnThreadId = CThread::getCurrentThreadId();
	}

	CPoolConnAutoPtr& operator = (const CPoolConnAutoPtr& Other)
	{
		//return CAutoPtr::operator =(Other);
		if ( this->m_pPointer != NULL )
			this->m_pPointer->putConnection();
		this->m_pPointer = Other.m_pPointer;
		((CPoolConnAutoPtr&)Other).m_pPointer = NULL;
		return *this;
	}

	/**
	 * 析构函数
	 */
	virtual ~CPoolConnAutoPtr()
	{
		if ( this->m_pPointer != NULL )
		{
			((T*)this->m_pPointer)->putConnection();
			this->m_pPointer = NULL;
		}
	}

};

#endif
