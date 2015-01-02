#include "locallogstream.h"

#include "util/dir.h"
#include "util/datetime.h"
#include "io/bytesinputstream.h"

/*

		CLocalLogOutputStream

*/

CLocalLogOutputStream::CLocalLogOutputStream(const char *pathname, const char *logfilename)
{
	m_strPath = pathname;
	m_strFile = logfilename;
	m_pThread = NULL;
	m_bRunFlag = false;
}

CLocalLogOutputStream::~CLocalLogOutputStream()
{
	stop();
	delete m_pThread;
}

bool CLocalLogOutputStream::init()
{
	//创建目录
	CDir::MakeDirectory(m_strPath.c_str());
	//初始化线程
	m_pThread = CThreadFactory::createThread(*this);
	//m_pThread->setDeamon(true);
	return true;
}

void CLocalLogOutputStream::start()
{
	if ( m_bRunFlag )
		return ;
	m_bRunFlag = true;
	m_pThread->start();
}

void CLocalLogOutputStream::stop()
{
	m_bRunFlag = false;
}

void CLocalLogOutputStream::run()
{
	while ( m_bRunFlag )
	{
		CThread::sleep(100);
		flushFile();
	}
	flushFile();
}

void CLocalLogOutputStream::flushFile()
{
	char buffer[1024] = {0};
	list<string> tmplist;
	m_WriteLock.lock();
	tmplist = this->m_LogList;
	m_LogList.clear();
	m_WriteLock.unlock();
	if ( tmplist.size() == 0 )
		return ;
	CDateTime datetime;
	sprintf(buffer,"%s/%s.%s",m_strPath.c_str(),m_strFile.c_str(),datetime.asString("YYYYMMDD").c_str());
	FILE *logfile = NULL;
	if ( ( logfile = fopen(buffer, "a") ) == NULL )
	{
		printf("Open LogFile == NULL : %s\n", buffer);
		printf("error code %d -- %s\n", errno, strerror(errno));
		return ;
	}
	for ( list<string>::iterator i = tmplist.begin() ; i != tmplist.end() ; i ++ )
	{
		printf("%s\n",(*i).c_str());
		fprintf(logfile,"%s\n",(*i).c_str());
	}
	fclose(logfile);
}

int CLocalLogOutputStream::writeBytes(const char *pBuf, uint32 &Buflen)
{
	LOGMSG logrequest;
	unserialLogMsg(pBuf,Buflen,logrequest);
	writeLog(logrequest);
	//如果日志线程未运行，则直接写入文件
	if ( !m_bRunFlag )
	{
		flushFile();
	}
	return 0;
}

bool CLocalLogOutputStream::writeLog(LOGMSG& LogRequest)
{
	PLOGMSG request = &LogRequest;
	std::string strlevel;
	switch ( request->level )
	{
	case LOG_FATAL:
		strlevel = "FATAL";
		break;
	case LOG_ERROR:
		strlevel = "ERROR";
		break;
	case LOG_WARNING:
		strlevel = "WARNING";
		break;
	case LOG_INFO:
		strlevel = "INFO";
		break;
	case LOG_DEBUG:
		strlevel = "DEBUG";
		break;
	default :
		break;
	}

	char buffer[4096] = {0};
	snprintf(buffer,4095,"%s:[%d][%s][%s]", request->datetime,
				request->pid,strlevel.c_str(),request->message);
	CAutoLock lock(m_WriteLock);
	m_LogList.push_back(buffer);
	return true;
}

void CLocalLogOutputStream::incRef()
{
	CRefShare::incRef();
}

bool CLocalLogOutputStream::decRef()
{
    return CRefShare::decRef();
	//return ;
}

