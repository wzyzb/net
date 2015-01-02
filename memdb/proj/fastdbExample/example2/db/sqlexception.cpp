// SQLException.cpp: implementation of the CSQLException class.
//
//////////////////////////////////////////////////////////////////////
#include <public.h>
#include <exception>
#include <string>
#include <db/sqlexception.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSQLException::CSQLException(int nSQLErrCode, const std::string& strErrDesc) : CCException(strErrDesc),m_nSQLErrCode(nSQLErrCode),m_SQLError(nSQLErrCode, strErrDesc)
{

}

CSQLException::~CSQLException() throw()
{

}

const char* CSQLException::what()
{
	char buff[2048];
	snprintf(buff,sizeof(buff)-1," errcode=%d , errmsg=%s",this->m_SQLError.getSQLErrorCode(),this->m_SQLError.getSQLDesc().c_str());
	this->m_ErrMsg = buff;
	return this->m_ErrMsg.c_str();
}

const CSQLError& CSQLException::getSQLError()
{
	return m_SQLError;
}
