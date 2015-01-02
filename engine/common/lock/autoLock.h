#ifndef _MYLOCK_H_
#define _MYLOCK_H_

class ILock
{
protected:
    ILock(){}
public:
    virtual ~ILock(){}
    virtual bool lock()=0;
    virtual bool unlock()=0;
};

class CAutoLock
{
protected:
    ILock*  m_lock;
public:
    CAutoLock(ILock& lock);
    ~CAutoLock();
private:
    CAutoLock(const CAutoLock&);
    CAutoLock& operator=(const CAutoLock&);
};

#endif