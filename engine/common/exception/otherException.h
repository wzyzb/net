#ifndef _NULLPOINTEXCEPTION_H_
#define _NULLPOINTEXCEPTION_H_

#include "myException.h"
/*
*/

class CNullPointException : public CMyException
{
public :
    CNullPointException(const std::string& strErrMsg);
    virtual ~CNullPointException() throw();
};


class CThreadException : public std::exception
{
private :
    char* buffer;

public :
    CThreadException(const char* exp) throw ();
    ~CThreadException() throw ();
    const char* what() const throw();
    CThreadException operator = (CThreadException& other);
};


class CDateTimeException : public CMyException
{
public :
    CDateTimeException(const char* pMsg);
    ~CDateTimeException() throw();
};

#endif