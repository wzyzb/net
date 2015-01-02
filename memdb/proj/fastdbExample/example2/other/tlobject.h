#if !defined(_TLOBJECT_H_)
#define _TLOBJECT_H_

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

/*
线程本地变量
*/
class CTlObject
{
private :
	//线程本地变量的key
	TSD m_TlKey;
public :
	/**
	 *
	 */
	CTlObject();
	/**
	 *
	 */
	~CTlObject();
	/**
	 *
	 */
	bool initialize();
	/**
	 *
	 */
	void* getValue();
	/**
	 *
	 */
	bool setValue(void*);
};

#endif

