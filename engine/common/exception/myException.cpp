#include "myException.h"

#ifdef WIN32
#include <Windows.h>
#include <dbghelp.h>
#pragma comment( lib, "dbghelp.lib" )

extern void windowsCallstack(CONTEXT *context, std::string& stackInfo);
#ifndef GET_CURRENT_CONTEXT
#define GET_CURRENT_CONTEXT(c, contextFlags) \
    do { \
    memset(&c, 0, sizeof(CONTEXT)); \
    c.ContextFlags = contextFlags; \
    __asm    call x \
    __asm x: pop eax \
    __asm    mov c.Eip, eax \
    __asm    mov c.Ebp, ebp \
    __asm    mov c.Esp, esp \
    } while(0);
#endif

#else
extern void linuxCallstack(std::string& stackInfo);

#endif


CMyException::CMyException(const std::string& strErrMsg): m_ErrMsg(strErrMsg)
{
#ifdef WIN32
    m_stackInfo = " ";
    CONTEXT curcontext;
    GET_CURRENT_CONTEXT(curcontext, CONTEXT_CONTROL);
    if(!SymInitialize( GetCurrentProcess(), NULL, TRUE ) )
    {
        printf( "Init windows dbghelp error.\n" );
        exit(EXIT_FAILURE);
    }
    windowsCallstack(&curcontext, m_stackInfo);

    if( !SymCleanup( GetCurrentProcess() ) )
    {
        printf( "Cleanup windows dbghelp error.\n" );
        exit(EXIT_FAILURE);
    }
#else
    linuxCallstack(m_stackInfo);
#endif
}

CMyException::~CMyException() throw()
{
}

const char* CMyException::what() const throw()
{
    return m_ErrMsg.c_str();
}


const char* CMyException::printStackTrace()
{
    return m_stackInfo.c_str();
}


