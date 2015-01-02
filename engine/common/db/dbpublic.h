#ifndef _DBPUBLIC_H_
#define _DBPUBLIC_H_

//错误代码定义,抛出CSQLException时，使用以下代码做errcode

#include "../public.h"

#ifdef WIN32
#pragma warning(disable:4150) // Warning ****************
#endif
#include "../util/autoptr.h"
#include "../exception/myException.h"
#include "../util/datetime.h"
#include "sqlexception.h"

enum DATABSE_ERROR
{
	//非法操作
	ERROR_DATABASE_INVALIDOPER = 0x00010001,
	//不支持当前的方法
	ERROR_DATABASE_UNSUPPORT = 0x00010002, 
	//连接协议无效
	ERROR_DATABASE_INVALIDPROTOCOL = 0x00010003,
	//数据类型错误
	ERROR_DATABASE_INVALIDDATATYPE = 0x00010004, 
	//索引越界
	ERROR_DATABASE_INDEXOUT = 0x00010005, 
	//数据库已关闭
	ERROR_DATABASE_CLOSED = 0x00010006, 
	//公共错误
	ERROR_DATABASE_COMMON = 0x00010007, 
	//企图获取空值的字段
	ERROR_DATABASE_NULLVALUE = 0x00010008,
	//建立数据库连接不成功
	ERROR_DATABASE_NOTCONNECT = 0x00010009,
	//数据库连接池满
	ERROR_DATABASE_POOLFULL = 0x00010010,
	//更新大对象字段前，未对statement执行executequery
	ERROR_DATABASE_LOBUPDATE = 0x00010011,
	//读取大对象时，缓存溢出
	ERROR_GETBYTES_BUFFOVERFLOW = 0x00010012,
};

#ifdef _ORACLE_
	#define _SYSDATE_ "sysdate"
#elif _MYSQL_
	#define _SYSDATE_ "sysdate()"
#else
	#define _SYSDATE_ "sysdate()"
#endif

#endif

