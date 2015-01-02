#ifndef _MYEXCEPTION_H_
#define  _MYEXCEPTION_H_

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
    char buffer[256] = {0};
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

    int retval = vsnprintf(buffer,sizeof(buffer),fmt, list);

#else
  
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

class CMyException : public std::exception
{
protected :
    std::string m_ErrMsg;
    std::string m_stackInfo;

public :
    CMyException(const std::string& strErrMsg);
    virtual ~CMyException() throw();
    const char* what() const throw();

    const char* printStackTrace();
};



#endif