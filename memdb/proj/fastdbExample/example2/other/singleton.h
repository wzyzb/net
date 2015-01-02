#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <util/allocator.h>

/*
单例类实现，所有需要实现单例模式的类，都需要继承或使用此类。
*/
template <class T,class Alloc = CSystemAllocator>
class CSingleton
{
private :
	static T* m_pInstance;
protected :
	/**
	 * 构造函数
	 *
	 */
	CSingleton() {}


	/**
	 * 析构函数
	 *
	 */
	virtual ~CSingleton() {}

	CSingleton& operator = ( const CSingleton& other );
public :
	
	/**
	 * 获取单例对象
	 * @return T
	 */
	static T* getInstance()
	{
		if ( m_pInstance == NULL )
		{
			CAllocator *pAlloc = Alloc::getInstance();
			//由于只是单例，无须从内存中获取。
			if ( pAlloc == NULL )
			{
				m_pInstance = new T();
			}
			else
			{
				m_pInstance = (T*) pAlloc->allocate(sizeof(T));
				m_pInstance = new(m_pInstance)T();
			}
		}
		return m_pInstance;
	}

	/**
	 * 强制释放单例资源
	 */
	static void destroy()
	{
		if ( m_pInstance )
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}
};

template<class T,class Alloc>
T* CSingleton<T,Alloc>::m_pInstance = NULL;

#endif
