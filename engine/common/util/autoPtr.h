#ifndef _AUTOPTR_H_
#define _AUTOPTR_H_

#include "otherException.h"

template<class T>
class CAutoPtr
{
protected :
	T* m_pPointer;

public :
	//简单的拷贝构造函数实现，如果发现赋值后智能指针不再保存
	//每次只有一个智能指针保存。
	CAutoPtr(const CAutoPtr& Other)
	{
		m_pPointer = ((CAutoPtr&)Other).release();
	}

	CAutoPtr& operator = (const CAutoPtr& Other)
	{
		if ( m_pPointer != NULL )
			delete m_pPointer;
		m_pPointer = ((CAutoPtr&)Other).release();
		return *this;
	}
	/**
	 * 构造函数
	 */
	CAutoPtr(T* Pointer)
	{
		m_pPointer = Pointer;
	}
	/**
	 * 析构函数
	 */
	virtual ~CAutoPtr()
	{
		if ( m_pPointer != NULL )
			delete m_pPointer;
	}
	/**
	 * 释放所保存的指针。
	 * @return T* 指针
	 */
	T* release()
	{
		T* pPointer = m_pPointer;
		m_pPointer = NULL;
		return pPointer;
	}
	/**
	 * 指针操作函数
	 */
	T* operator->()
	{
		//空指针异常
		if ( NULL == m_pPointer )
			throw CNullPointException("Null Pointer");
		return m_pPointer;
	}

	T* get()
	{
		return m_pPointer;
	}

	const T* get() const
	{
		return m_pPointer;
	}

	/**
	 * 指针操作函数
	 */
	const T* operator->() const
	{
		//空指针异常
		if ( NULL == m_pPointer )
			throw CNullPointException("Null Pointer");
		return m_pPointer;
	}

	operator void*() const
	{
		return m_pPointer; 
	}

};


#endif