#include <public.h>
#include <framework/daexception.h>

void throwDAException(int32 ErrCode,const char* fmt,...)
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
	throw CDAException(ErrCode,buffer);
}

/*

		Class	CDAException

*/
CDAException::CDAException(int32 nErrCode,const std::string& ErrMsg) : CCException(ErrMsg) , m_nErrCode(nErrCode)
{
}

int32 CDAException::getErrCode() const
{
	return m_nErrCode;
}

CDAException::~CDAException() throw()
{
}
