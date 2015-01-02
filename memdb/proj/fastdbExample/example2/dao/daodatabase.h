#ifndef _DAODATABASE_H_
#define _DAODATABASE_H_
#include "dao.h"
#include "daoimpl.h"

class CDaoDatabase
{
protected  :
	//线程本地变量
	CTlObject m_LocalTransaction;
	CTlObject m_LocalDBEnv;
public :
	virtual ~CDaoDatabase();
	/**
	 * 创建事务
	 * @param TransTye 事务类型，详见枚举值定义daodefs.h
	 */
	virtual CDaoTransactionImpl* createTransaction(TRANSACTION_TYPE TransType) = 0;
	/**
	 * 获取当前的事务
	 */
	CDaoTransactionImpl* getLocalTransaction();
	/**
	 * 设置当前线程的事务－－返回旧的线程事务
	 */
	CDaoTransactionImpl* setLocalTransaction(CDaoTransactionImpl* pTransaction);
	/**
	 * 设置数据库环境
	 * @param EnvType 要设置的数据库类型，详见枚举值定义daodefs.h
	 */
	void setLocalDBEnv(DBENV EnvType);
	/**
	 * 获取当前的数据库类型
	 * @return DBENV 数据库类型
	 */
	DBENV getLocalDBEnv();
	/**
	 * 初始化必要的资源
	 */
	bool initialize();

	/**
	 * 获取数据库实例
	 * @return 数据库实例
	 */
	static CDaoDatabase& getInstanceImpl();
};

#endif
