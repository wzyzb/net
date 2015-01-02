#ifndef _DAO_H_
#define _DAO_H_
#include "public.h"
#include "userexception.h"
#include "datetime.h"
#include <list>
using namespace std;
#include "daodefs.h"
#include "daoimpl.h"
#include "tlobject.h"
#include "dbinterface.h"

class CDaoTransactionImpl;

/*
  数据访问对象
  查询条件
  支持name=? and param=? 的方式
*/
class CDao
{
protected :
	CDaoTransactionImpl* m_pTranImpl;
	IDaoQuery* m_pQuery;
	int m_nRowCount;

public :
	CDao();
	virtual ~CDao();
	bool find();
	bool next();
	bool update();
	bool insert();
	bool remove();
	bool prepare(const char* sql);
	void setInt(int Index,int32 nValue);
	void setInt64(int Index,int64 nValue);
	void setInt16(int Index,int16 nValue);
	void setString(int Index,const string& Value);
	void setDateTime(int Index,const CDateTime& Value);
	void setBytes(int Index,const void* pValue,int nLength);
	void setUInt8(int Index,uint8 Value);
	void setUInt16(int Index,uint16 Value);
	void setUInt32(int Index,uint32 Value);
	void setUInt64(int Index,uint64 Value);
	/**
	 * 获取条数
	 */
	int getRowCount() const;

	/**
	 * 获取对应的内存dao实现
	 * @return 对应的实例指针，生命周期由CDao管理
	 */
	virtual CMDBDaoImpl* getMDBDao() = 0 ;
	/**
	 * 获取对应的持久数据库dao实现
	 * @return 对应的实例指针，生命周期由CDao管理
	 */
	virtual CDBDaoImpl* getDBDao() = 0;
};

class CDaoException : public CCException
{
public :
	CDaoException(const string& Message);
	virtual ~CDaoException() throw();
};

class CDaoTransaction
{
protected :
	//当前的新事务
	CDaoTransactionImpl* m_pTranImpl;
	//上一级事务
	CDaoTransactionImpl* m_pOldTranImpl;
	//标识是否开启事务
	bool m_bHasBegin;

public :
	//默认启动cached的事务
	CDaoTransaction(TRANSACTION_TYPE TransType = TRANSACTION_SYNC);
	//
	~CDaoTransaction();
	/**
	 * 启动dao事务
	 */
	bool begin();
	/**
	 * 提交dao事务
	 */
	bool commit();
	/**
	 * 回滚dao事务
	 */
	bool rollback();
};

class CDaoTransactionImpl
{
protected :
	//事务类型
	TRANSACTION_TYPE m_TransType;

public :
	/**
	 * dao事务的构造函数
	 * @param TransType 事务类型，默认为同步型事务
	 */
	CDaoTransactionImpl(TRANSACTION_TYPE TransType = TRANSACTION_SYNC);
	//
	virtual ~CDaoTransactionImpl();
	/**
	 * 启动dao事务
	 */
	virtual bool begin() = 0;
	/**
	 * 提交dao事务
	 */
	virtual bool commit() = 0;
	/**
	 * 回滚dao事务
	 */
	virtual bool rollback() = 0;
	virtual bool update(CDao* pDao) = 0;
	virtual bool insert(CDao* pDao) = 0;
	virtual bool remove(CDao* pDao) = 0;
	virtual IDaoQuery* prepare(const char* Sql,CDao* pDao) = 0;
	/**
	 * 返回影响条数
	 */
	virtual int find(CDao* pDao) = 0;
	virtual bool next(CDao* pDao) = 0;
};

#endif

