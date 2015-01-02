#ifndef _DAODEFS_H_
#define _DAODEFS_H_


enum PARAM_OPERMODE
{
	OPER_EQUAL= 0x01,
	OPER_NOEQL,
	OPER_LT,
	OPER_LET,
	OPER_BT,
	OPER_BET,
	OPER_IN,
	OPER_BETWEEN,
	OPER_LIKE,
	OPER_UNKNOW
};

enum VALUE_TYPE
{
	VALUE_STRING,
	VALUE_NUMBER,
	VALUE_PARAM,
};

enum ORDER_MODE
{
	ORDER_ASC,
	ORDER_DESC
};

enum EXPRESS_JOINMODE
{
	JOIN_AND = 1,
	JOIN_OR,
};

//事务类型
enum TRANSACTION_TYPE
{
	//缓冲
	TRANSACTION_SYNC = 0x0001,
	//数据库
	TRANSACTION_DB_ONLY = 0x0002,
	//数据库
	TRANSACTION_CACHE_ONLY = 0x0003,
};
//数据库连接的类型
enum DBCONN_TYPE
{
	//主库
	DBCONN_MASTER = 0,
	//从库
	DBCONN_SLAVE = 1
};

//当前的数据库环境
enum DBENV
{
	//操作内存数据库 -- 默认
	DBENV_MDB = 0,
	//操作dbms数据库
	DBENV_DB,
	//操作从库的数据库
	DBENV_SDB,
	DBENV_END
};

//
enum EDAOACTION
{
	ACTION_UNKNOW = 0,
	ACTION_UPDATE,
	ACTION_DELETE,
	ACTION_INSERT,
	ACTION_END
};

#endif
