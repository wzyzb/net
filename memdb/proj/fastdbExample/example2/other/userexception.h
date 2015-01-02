#if !defined(_AVOEXCEPTION_H_)
#define _AVOEXCEPTION_H_

#ifdef WINDOWS
#pragma warning(disable:4996)
#endif


#include <exception>
#include <string>
#include <stdarg.h>
#include <stdio.h>

template <class T>
void ThrowException(const char* fmt,...)
{
	char buffer[200];
	va_list list;
	va_start(list,fmt);
#ifdef WIN32
	int retval = _vsnprintf(buffer,sizeof(buffer)-1,fmt,list);
	if ( retval > 0 )
	{
		buffer[retval] = 0;
	}
	else if ( retval == -1 )
	{
		buffer[sizeof(buffer)-1] = 0;
	}
#elif defined GCC
	//linuxï¿½ï¿½ï¿½Ô¶ï¿½ï¿½ï¿½ï¿½ï¿½0ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿?
	int retval = vsnprintf(buffer,sizeof(buffer),fmt, list);

/*
#ifdef _DEBUG
__asm__( 
"int3" 
);
#endif // _DEBUG
*/

#else
/*
#ifdef _DEBUG
__asm__( 
"int3" 
);
#endif // _DEBUG
*/
	int retval = vsnprintf(buffer,sizeof(buffer)-1,fmt,list);
	if ( retval > 0 )
	{
		buffer[retval] = 0;
	}
	else if ( retval == -1 )
	{
		buffer[sizeof(buffer)-1] = 0;
	}
#endif

	buffer[sizeof(buffer)-1] = 0 ;
	va_end(list);
	throw T(buffer);
}

class CCException : public std::exception
{
protected :
	std::string m_ErrMsg;
	std::string m_stackInfo;

public :
	CCException(const std::string& strErrMsg);
	virtual ~CCException() throw();
	const char* what() const throw();

	const char* printStackTrace();
};

#endif

