#ifndef _FRAMEPUBLIC_H_
#define _FRAMEPUBLIC_H_

#include <public.h>
#include <util/getopt.h>
#include <io/log.h>
#include <io/binlog.h>
#include <util/profile.h>

extern DLLAPI CLogPtr g_Log;
#ifdef WIN32
#define log_debug(fmt, ...) g_Log->debug(fmt, __VA_ARGS__)
#define log_error(fmt, ...) g_Log->error(fmt, __VA_ARGS__)
#define log_fatal(fmt, ...) g_Log->fatal(fmt, __VA_ARGS__)
#define log_info(fmt, ...) g_Log->info(fmt, __VA_ARGS__)
#define log_warning(fmt, ...) g_Log->warning(fmt, __VA_ARGS__)
#else
#define log_debug(fmt, ...) g_Log->debug(fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) g_Log->error(fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...) g_Log->fatal(fmt, ##__VA_ARGS__)
#define log_info(fmt, ...) g_Log->info(fmt, ##__VA_ARGS__)
#define log_warning(fmt, ...) g_Log->warning(fmt, ##__VA_ARGS__)
#endif

extern const struct option default_opts[];

ELOGLEVEL getLogLevel(const char* LogLevel);

/*
  处理命令行参数
  @return int 返回获取的参数个数
*/
int parseCmdArgs(int argc,char** argv,const struct option opts[]=default_opts);

/**
 * 获取命令行参数
 * @param nOpt 短参数名
 * @param Value 参数值，输出参数
 * @return bool 是否获取成功，如果失败，则Value的值为NULL
*/
bool getCmdArgs(int nOpt,const char*& Value);

/**
 * 初始化业务框架包
 * 初始化日志(log)，初始化配置文件(profile)
 */
bool initFrameWorkLib(const char* ProcessName);
/*
 初始化日志，必须在parseCmdArgs后调用，否则会返回失败
 会使用参数-l所指定的作为日志路径
*/
bool initFrameWorkLog(const char* ProcessName);


/**
 * 释放业务框架包，回收所有资源
 */
void uninitFrameWorkLib();

/**
 * 获取配置文件
 */
CProfile* getFrameWorkProf();
/**
 * 初始化配置文件
 */
bool initFrameWorkProf();
/**
 * 重新装载配置文件，必须先调用initFrameWorkProf
 */
bool reloadFrameWorkProf();
/**
 * 是否配置文件有更新
 */
bool isFrameWorkProfUpdated();

#endif
