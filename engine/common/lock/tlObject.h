#ifndef _TLOBJECT_H
#define _TLOBJECT_H

#ifdef WIN32
#if (_WIN32_WINNT<0x410) || !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x410
#endif
#include <windows.h>
#include <winbase.h>
#else
#include <pthread.h>
#include <signal.h>
#include <sched.h>
#endif

#ifdef WIN32
#define TSD DWORD
#else
#define TSD pthread_key_t
#endif


class CTlObject
{
private:
    TSD  m_TLKey;
public:
    CTlObject();
    ~CTlObject();

    bool initialize();

    void* getValue();

    bool  setValue(void*);

};

#endif