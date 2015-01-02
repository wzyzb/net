#ifndef _AUTOMICOPER_H_
#define _AUTOMICOPER_H_


#if (defined(__linux) || defined(__FreeBSD__)) && (defined(__i386) || defined(__x86_64)) && !defined(__ICC)
#   define HAS_ATOMIC_FUNCTIONS

struct atomic_t
{
    volatile int counter;
    inline operator int()
    {
        return counter;
    }
    inline void operator =( int i )
    {
        counter = i ;
    }
    inline atomic_t operator ++(int i )
    {
        counter ++;
        return *this;
    }
    inline atomic_t operator --( )
    {
        counter --;
        return *this;
    }
};


inline void atomic_set(atomic_t* v, int i)
{
    v->counter = i;
}

inline void atomic_inc(atomic_t *v)
{
    __asm__ __volatile__(
        "lock ; incl %0"
        :"=m" (v->counter)
        :"m" (v->counter));
}


inline int atomic_dec_and_test(atomic_t *v)
{
    unsigned char c;
    __asm__ __volatile__(
        "lock ; decl %0; sete %1"
        :"=m" (v->counter), "=qm" (c)
        :"m" (v->counter) : "memory");
    return c != 0;
}


inline int atomic_exchange_add( atomic_t* v , int i )
{
    int tmp = i;
    __asm__ __volatile__(
        "lock ; xadd %0,(%2)"
        :"+r"(tmp), "=m"(v->counter)
        :"r"(v), "m"(v->counter)
        : "memory");
    return tmp;
}

inline int atomic_compare_exchange( atomic_t* v , int old , int newv )
{
    int prev;
    __asm__ __volatile__("lock; cmpxchgl %1,%2"
        : "=a"(prev)
        : "r"(newv), "m" (v->counter),  "0"(old)
        : "memory");
    return prev;
}

#elif defined(_WIN32)
#include <windows.h>

typedef LONG atomic_t;

#define atomic_set(REF,X)               ((*REF) = (X))
#define atomic_exchange_add(REF,X)      (int)InterlockedExchangeAdd((REF), (X))
#define atomic_inc(REF)                 (int)InterlockedIncrement((REF))
#define atomic_dec_and_test(REF)        (InterlockedDecrement((REF)) == 0)
#define atomic_compare_exchange(REF,OLD,NEW)  InterlockedCompareExchange(REF,NEW,OLD)

#else
typedef int atomic_t;

inline void atomic_set(atomic_t* v, int i)
{
    CObjectManager::getAtomicLock().lock();
    *v = i;
    CObjectManager::getAtomicLock().unlock();
}

inline void atomic_inc(atomic_t *v)
{
    CObjectManager::getAtomicLock().lock();
    *v ++ ;
    CObjectManager::getAtomicLock().unlock();
}


inline int atomic_dec_and_test(atomic_t *v)
{
    CObjectManager::getAtomicLock().lock();
    bool ret = ( -- *v == 0 );
    CObjectManager::getAtomicLock().unlock();
    return ret;
}


inline int atomic_exchange_add( atomic_t* v , int i )
{
    CObjectManager::getAtomicLock().lock();
    int ret = *v;
    *v = *v + i;
    CObjectManager::getAtomicLock().unlock();
    return ret;
}

inline int atomic_compare_exchange( atomic_t* v , int old , int newv )
{
    CObjectManager::getAtomicLock().lock();
    int ret = *v;
    if( ret == old )
    {
        *v = newv;
    }
    CObjectManager::getAtomicLock().unlock();
    return ret;
}

#endif

class CAtomic
{
public:
    explicit CAtomic( int i = 0 )
    {
        _count = i;
    }
    virtual ~CAtomic()
    {

    }

    inline int operator ++ ()
    {
        int ret = atomic_exchange_add( &_count , 1 );
        return ++ret;
    }

    inline int operator ++ ( int )
    {
        return atomic_exchange_add( &_count , 1  );
    }

    inline int operator -- ()
    {
        int ret = atomic_exchange_add( &_count , -1 );
        return ret - 1;
    }

    inline int operator -- ( int )
    {
        return atomic_exchange_add( &_count , -1 );
    }

    inline int operator += ( int i )
    {
        int ret = atomic_exchange_add( &_count , i );
        return ret + i;
    }

    inline int operator -= ( int i )
    {
        int ret = atomic_exchange_add( &_count , -i );
        return ret - i;
    }

    inline operator int() const
    {
        return atomic_exchange_add( const_cast<atomic_t*>(&_count) , 0 );
    }

    inline void operator = ( int i )
    {
        atomic_set( &_count , i );
    }

    inline int cmpExchange( int old , int newv )
    {
        return atomic_compare_exchange( &_count , old , newv );
    }
private:
    CAtomic operator = ( CAtomic& );
    CAtomic( const CAtomic& );
    atomic_t _count;
};

#endif