#ifndef _BINLOG_H_
#define _BINLOG_H_

/*
二进制日志，用于数据的调试
*/
class CBinLog
{
public :
	/**
	 * 设置二进制日志的路径，如果不设置，默认为进程的当前工作目录
	 */
	static void setBinLogPath(const char* pPath);
	/**
	 * 设置binlog的默认名
	 */
	static void setBinLogName(const char* pName);
	/**
	 * 写人二进制文件
	 * @param pData 要写入的数据
	 * @param DataSize 要写入的数据大小
	 * @param pLogName 写入的文件名，如果不设置，则取setBinLogName的设置
	 */
	static void writeBinLog(const char* pData,int DataSize,const char* pLogName = NULL);
	/**
	 * 以文本形式打印出二进制的内容
	 * @param pData 要打印的二进制内容
	 * @param DataSize 二进制数据的大小，不允许超过1024个字节
	 * @return const char* 转换后的内容
	 */
	static const char* printBinText(const char* pData,int DataSize);
};

#endif
