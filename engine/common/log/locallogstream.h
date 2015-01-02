#ifndef _LOCALLOGSTREAM_H_
#define _LOCALLOGSTREAM_H_

#include "public.h"
#include "thread/thread.h"
#include  "lock/lightLock.h"
#include  "io/outputstream.h"
#include  "log.h"
#include "util/refshare.h"

class CLocalLogOutputStream : public IOutputStream,
							  public CRefShare,
							  public CRunnable
{
public :
	CLocalLogOutputStream(const char * pathname = "./log", const char *logfilename = "log");
	~CLocalLogOutputStream();
	int writeBytes(const char *pBuf, uint32 &Buflen);
	void incRef();
	bool decRef();
	/**
	 * 线程运行主函数
	 */
	void run();
	/*
	 * 初始化
	 */
	bool init();
	/**
	 * 停止
	 */
	void stop();
	/**
	 * 启动线程
	 */
	void start();

private :
	bool writeLog(LOGMSG& LogRequest);
	/**
	 * 写入日志文件
	 */
	void flushFile();

private :
	std::string m_strFile;
	std::string m_strPath;
	//写锁
	CLightLock m_WriteLock;
	//日志数据
	list<string> m_LogList;
	//线程
	CThread* m_pThread;
	//运行标识
	bool m_bRunFlag;
};

typedef CRefObject<CLocalLogOutputStream> CLocalLogOutputStreamPtr;
#endif

