#include "public.h"
#include "log.h"
#include "util/objectpool.h"

inline void serialInt(int Value,CBytesBuffer& Buffer)
{
	uint32 len = sizeof(Value);
	Buffer.writeBytes((const char*)&Value,len);
}

inline bool unserialInt(const char* pData,int DataSize,int& Index,int& Value)
{
	const char* p = pData + Index;
	Index += sizeof(Value);
	if ( Index > DataSize )
		return false;
	memcpy(&Value,p,sizeof(Value));
	return true;
}

inline void serialString(const string& Value,CBytesBuffer& Buffer)
{
	uint32 len = Value.length()+1;
	uint32 mylen = sizeof(len);
	Buffer.writeBytes((const char*)&len,mylen);
	Buffer.writeBytes(Value.c_str(),len);
}

inline bool unserialString(const char* pData,int DataSize,int& Index,string& Value)
{
	const char* p = pData + Index;
	uint32 stringlen = 0;
	Index += sizeof(stringlen);
	if ( Index > DataSize )
		return false;
	memcpy(&stringlen,p,sizeof(stringlen));
	p = pData + Index;
	Index += stringlen;
	if ( Index > DataSize )
		return false;
	Value = p;
	//由于有时候出现string 的len错误，这里不做检查
	//if ( Value.length() != stringlen-1 )
	//	return false;
	return true;
}

inline void serialString(const char* Value,int DataSize,CBytesBuffer& Buffer)
{
	uint32 len = DataSize+1;
	uint32 mylen = sizeof(len);
	Buffer.writeBytes((const char*)&len,mylen);
	Buffer.writeBytes(Value,len);
}

inline bool unserialString(const char* pData,int DataSize,int& Index,const char*& Value)
{
	const char* p = pData + Index;
	uint32 stringlen = 0;
	Index += sizeof(stringlen);
	if ( Index > DataSize )
		return false;
	memcpy(&stringlen,p,sizeof(stringlen));
	p = pData + Index;
	Index += stringlen;
	if ( Index > DataSize )
		return false;
	Value = p;
	return true;
}

void serialLogMsg(const LOGMSG& LogMsg,CBytesBuffer& Buffer)
{
	serialString(LogMsg.datetime,sizeof(LogMsg.datetime)-1,Buffer);
	serialInt(LogMsg.level,Buffer);
	serialString(LogMsg.process,Buffer);
	serialInt(LogMsg.pid,Buffer);
	serialString(LogMsg.servername,Buffer);
	serialString(LogMsg.message,strlen(LogMsg.message),Buffer);
}

bool unserialLogMsg(const char* pData,int DataSize,LOGMSG& LogMsg)
{
	int index = 0;
	const char* p = NULL;
	if ( !unserialString(pData,DataSize,index,p) )
		return false;
	memcpy(LogMsg.datetime,p,sizeof(LogMsg.datetime));
	if ( !unserialInt(pData,DataSize,index,LogMsg.level) )
		return false;
	if ( !unserialString(pData,DataSize,index,LogMsg.process) )
		return false;
	if ( !unserialInt(pData,DataSize,index,LogMsg.pid) )
		return false;
	if ( !unserialString(pData,DataSize,index,LogMsg.servername) )
		return false;
	if ( !unserialString(pData,DataSize,index,LogMsg.message) )
		return false;
	return true;
}


/*

		Class	CLog

*/
#define BUF_SIZE 2048

#ifdef WIN32
#define WRITE_VAR_LOG \
	va_list list;\
	va_start(list,Format);\
	int retval = _vsnprintf(buffer,sizeof(buffer)-1,Format,list);\
	if ( retval > 0 )\
	{\
		buffer[retval] = 0;\
	}\
	else if ( retval == -1 )\
	{\
		buffer[sizeof(buffer)-1] = 0;\
	}\
	va_end(list);
#elif GCC
#define WRITE_VAR_LOG \
	va_list list;\
	va_start(list,Format);\
	int retval = vsnprintf(buffer,sizeof(buffer),Format, list);\
	buffer[sizeof(buffer)-1] = 0 ;\
	va_end(list);
#else
#define WRITE_VAR_LOG \
	va_list list;\
	va_start(list,Format);\
	int retval = vsnprintf(buffer,sizeof(buffer)-1,Format,list);\
	if ( retval > 0 )\
	{\
		buffer[retval] = 0;\
	}\
	else if ( retval == -1 )\
	{\
		buffer[sizeof(buffer)-1] = 0;\
	}\
	va_end(list);
#endif

CLog::CLog(string processname, string servername)
{
	this->m_LogMsg.process = processname;
	m_LogMsg.servername = servername;
#ifdef WINDOWS
	m_LogMsg.pid = (uint16)GetCurrentProcessId();
#else
	m_LogMsg.pid = getpid();
#endif
	m_nLevel = LOG_DEBUG;
	//m_pBuffer = CBytesBufferPtr::createInstance(__FUNCTION__);
}

CLog::~CLog()
{
}

void CLog::fatal(const char* Format,...)
{
	//CAutoLock loc(m_Lock);
	if ( LOG_FATAL > m_nLevel )
		return ;
	char buffer[BUF_SIZE];
	WRITE_VAR_LOG;
	writeMessage(LOG_FATAL,buffer);
}

void CLog::error(const char* Format,...)
{
	//CAutoLock loc(m_Lock);
	if ( LOG_ERROR > m_nLevel )
		return ;

	char buffer[BUF_SIZE];
	WRITE_VAR_LOG;
	writeMessage(LOG_ERROR,buffer);
}

void CLog::warning(const char* Format,...)
{
	//CAutoLock loc(m_Lock);
	if ( LOG_WARNING > m_nLevel )
		return ;

	char buffer[BUF_SIZE];
	WRITE_VAR_LOG;
	writeMessage(LOG_WARNING,buffer);
}

void CLog::info(const char* Format,...)
{
	//CAutoLock loc(m_Lock);
	if ( LOG_INFO > m_nLevel )
		return ;
	char buffer[BUF_SIZE];
	WRITE_VAR_LOG;
	writeMessage(LOG_INFO,buffer);
}

void CLog::debug(const char* Format,...)
{
	//CAutoLock loc(m_Lock);
	if ( LOG_DEBUG > m_nLevel )
		return ;
	char buffer[BUF_SIZE];
	WRITE_VAR_LOG;
	writeMessage(LOG_DEBUG,buffer);
}

void CLog::addOutput(IOutputStreamPtr Output)
{
	m_vOutputStream.push_back(Output);
}

void CLog::setLogLevel(ELOGLEVEL level)
{
	m_nLevel = level;
}

uint8 CLog::getLogLevel()
{
	return m_nLevel;
}

void CLog::writeMessage(ELOGLEVEL nLogLevel,const char* pMessage)
{
	LOGMSG logmsg;
	logmsg = m_LogMsg;
	sprintf(logmsg.datetime,"%s",CDateTime().asString("YYYY-MM-DD hh:mm:ss.nnn").c_str());
	logmsg.level = nLogLevel;
	logmsg.message = pMessage;
	//m_pBuffer->clear();
	//由于可能在遍历bytesbuffer对象池的时候打印日志，所以不能使用对象池
	CBytesBuffer buffer;
	buffer.setStackObject();
	serialLogMsg(logmsg,buffer);
	uint32 datasize = buffer.getDataSize();
	for ( OUTPUT_VEC::iterator iter = m_vOutputStream.begin(); iter != m_vOutputStream.end(); ++iter)
	{
		//serial为多次调用outputstream的writeBytes方法，所以需要增加同步处理
		//保证一次就能push出去，减少同步的需要
		(*iter)->writeBytes(buffer.c_str(),datasize);
	}
}
