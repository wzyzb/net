#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include "allocator.h"

template<typename T,typename Alloc = CSystemAllocator>
class CSingleTon
{
private:
    static T*  m_instance;
protected:
    CSingleTon(){};
    virtual ~CSingleTon(){};

    CSingleTon& operator=(const CSingleTon&);
    CSingleTon(const CSingleTon&);
public:
    static T* getInstance()
    {
        if ( m_instance == NULL )
        {
            CAllocator* pAlloc = Alloc::getInstance();
            if ( pAlloc == NULL )
            {
                m_instance = new T();
            }
            else
            {
                m_instance = (T*) pAlloc->allocate(sizeof(T));
                m_instance = new(m_instance)T();
            }
        }
        return m_instance;
    }

    static void destroy()
    {
        if ( m_instance )
        {
            delete m_instance;
            m_instance = NULL;
        }
    }

};

template<class T,class Alloc>
T* CSingleTon<T,Alloc>::m_instance = NULL;


#endif