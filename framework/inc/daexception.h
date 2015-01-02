#ifndef _DAEXCEPTION_H_
#define _DAEXCEPTION_H_

#include <string>
#include <util/userexception.h>

/*
*/
void throwDAException(int32 nErrCode,const char* fmt,...);

class DLLAPI CDAException : public CCException
{
public :
	CDAException(int32 nErrCode,const std::string& ErrMsg);
	virtual ~CDAException() throw();
	int32 getErrCode() const;

protected :
	int32 m_nErrCode;
};

#endif
