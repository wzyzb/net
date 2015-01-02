#include "otherException.h"
#include "../public.h"
#include "stdlib.h"
/*

Class	CNullPointException

*/

CNullPointException::CNullPointException(const std::string& strErrMsg): CMyException(strErrMsg)
{
}

CNullPointException::~CNullPointException() throw()
{
}


//=====CThreadException ===============

CThreadException::CThreadException(const char* exp) throw ()
{
    buffer = NULL;
    if (exp != NULL)
    {
        int len = strlen(exp);
        buffer = (char*)malloc(len+1);
        memcpy(buffer,exp,len);
        buffer[len]=0;
    }
}

CThreadException::~CThreadException() throw ()
{
    free(buffer);
}

const char* CThreadException::what() const throw()
{
    return buffer;
}

CDateTimeException::CDateTimeException(const char* pMsg ) : CMyException(pMsg)
{
}

CDateTimeException::~CDateTimeException() throw()
{
}