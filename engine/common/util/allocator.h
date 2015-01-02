#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

#include <stdio.h>

class CAllocator
{
protected:
    CAllocator(){}
    virtual ~CAllocator(){};

public:
    virtual void* allocate(size_t nSize)=0;
    virtual void  free(void* pPointer)=0;
    virtual void* realloc(void* pPointer,size_t nSize)=0;
};

class CSystemAllocator : public CAllocator 
{
public :
    static CAllocator* getInstance()
    {
        return NULL;
    }
};


#endif