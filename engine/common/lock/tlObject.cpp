#include "tlObject.h"

CTlObject::CTlObject()
{
#ifdef _WIN32
    this->m_TLKey = 0xFFFFFFFF;
#else
#endif
}

CTlObject::~CTlObject()
{
#ifdef _WIN32
    if ( this->m_TLKey != 0xFFFFFFFF )
        TlsFree(this->m_TLKey);
#else
    pthread_key_delete(this->m_TLKey);
#endif
}

bool CTlObject::initialize()
{
#ifdef _WIN32
    this->m_TLKey = TlsAlloc();
    if ( m_TLKey == 0xFFFFFFFF )
        return false;
#else
    if ( pthread_key_create(&this->m_TLKey,NULL) )
        return false;
#endif
    return true;
}

void* CTlObject::getValue()
{
#ifdef _WIN32
    return TlsGetValue(m_TLKey);
#else
    return pthread_getspecific(m_TLKey);
#endif
}

bool  CTlObject::setValue(void* pValue)
{
#ifdef _WIN32
    return ( TRUE == TlsSetValue(this->m_TLKey,pValue) );
#else
    return ( 0 == pthread_setspecific(this->m_TLKey,pValue) );
#endif
}