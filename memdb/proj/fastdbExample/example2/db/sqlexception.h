// SQLException.h: interface for the CSQLException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SQLEXCEPTION_H)
#define SQLEXCEPTION_H

#include <util/userexception.h>

class CSQLError
{
public:
	/**
	 * 初始化 CSQLError
	 * @param nSQLErrCode DataBase异常的错误码, 范围: 0x00010000 ~ 0x00019999, 前缀: ERROR_DATABASE_
	 * @param strErrDesc   DataBase错误的具体描述
	 */
	CSQLError (int nSQLErrCode, const std::string& strErrDesc)
		      : m_nSQLErrCode(nSQLErrCode),
		        m_strSQLDesc(strErrDesc)
	{

	}

	/**
	 * 获取DataBase错误码 
	 * @return int DataBase异常的错误码, 范围: 0x00010000 ~ 0x00019999, 前缀: ERROR_DATABASE_
	 */
	int getSQLErrorCode()  const  { return m_nSQLErrCode; } 

	/**
	 * 获取DataBase错误的具体描述 
	 * @return std::string DataBase错误的具体描述
	 */
	std::string getSQLDesc() const  { return m_strSQLDesc; }

private:
	int         m_nSQLErrCode;    
	std::string m_strSQLDesc;    
};

/**
 * DB模块异常类，用于定义DB相关的异常
 * @doc DB模块在实现上，会捕获所有数据库抛出的异常，然后抛出DB模块内部定义的异常
 */
class CSQLException : public CCException
{
public:
	/**
	 * 初始化 CSQLException
	 * @param nSQLErrCode DataBase异常的错误码, 范围: 0x00010000 ~ 0x00019999, 前缀: ERROR_DATABASE_
	 * @param strErrMsg   DataBase错误的具体描述
	 */
	CSQLException(int nSQLErrCode, const std::string& strErrMsg);

	/**
	 * 析构函数
	 */
	virtual ~CSQLException() throw();
	
	/**
	 * 初始化 CSQLException
	 * @return const char* DataBase错误的具体描述
	 */
	const char* what();

	/*
	 *	返回一个 CSQLError, 包含 DataBase错误信息
	 *  @return CSQLError 包含 DataBase错误信息
	 */
	const CSQLError& getSQLError();

private:
	int m_nSQLErrCode;
	CSQLError m_SQLError;
};

template <class T>
void ThrowSQLException(int nSQLErrCode,const char* fmt,...)
{
	char buffer[1024];
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
	//linux下自动增加0在最后面
	int retval = vsnprintf(buffer,sizeof(buffer),fmt, list);
#else
	int retval = vsnprintf(buffer,sizeof(buffer)-1,fmt,list);
	if ( retval > 0 && (retval< sizeof(buffer)-1) )
	{
		buffer[retval] = 0;
	}
	else
	{
		buffer[sizeof(buffer)-1] = 0;
	}
#endif
	buffer[sizeof(buffer)-1] = 0 ;
	va_end(list);
	throw T(nSQLErrCode,buffer);
}

#endif // !defined(SQLEXCEPTION_H)

