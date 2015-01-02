#ifndef _MYSQL_DAOIMPL_H_
#define _MYSQL_DAOIMPL_H_

#include <public.h>
#include <db/dbinterface.h>
#include <dao/daoimpl.h>

namespace dao_mysql
{
class CMysqlDaoImpl : public CDBDaoImpl
{
protected :
	//查询详细记录
	CStatementPtr m_QueryStmt;
	//查询总条数
	CStatementPtr m_QueryCountStmt;
	CResultSetPtr m_Resultset;
	CPoolConnectionPtr m_Conn;
	//表名，默认在构造函数中m_TableName = "tb_table"
	const char* m_TableName;

public :
	CMysqlDaoImpl();
	virtual ~CMysqlDaoImpl();
	/**
	 * 按条件查询
	 * @return int 返回结果集的总数
	 */
	bool prepareDB(const char* Sql,CPoolConnectionPtr& Conn);
	//
	void setQInt(int Index,int32 nValue);
	void setQInt64(int Index,int64 nValue);
	void setQInt16(int Index,int16 nValue);
	void setQString(int Index,const string& Value);
	void setQDateTime(int Index,const CDateTime& Value);
	void setQBytes(int Index,const void* pValue,int nLength);
	void setQUInt8(int Index,uint8 Value);
	void setQUInt16(int Index,uint16 Value);
	void setQUInt32(int Index,uint32 Value);
	void setQUInt64(int Index,uint64 Value);
	int findDB();
	bool nextDB();
	bool compare(const CDBDaoImpl* pOther) const;
	//----派生类必须实现的函数----
	//数据库相关的操作函数
	virtual void readDB(CResultSet& Set) = 0;
	/**
	 * 检查是否相同的数据库记录
	 * @return bool 如果是相同的数据库，则返回true
	 */
	virtual bool check(const CMysqlDaoImpl* pOther) const = 0;
};

class CMysqlDBDaoTransactionImpl : public CDBDaoTransactionImpl
{
protected :
	//数据库连接池
	CConnectionPool* m_pConnPool;
	//更新的连接
	CPoolConnectionPtr m_pUpdateConn;

public :
	CMysqlDBDaoTransactionImpl(CConnectionPool& ConnPool);
	~CMysqlDBDaoTransactionImpl();
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
	/**
	 */
	void update(CDBDaoImpl* pDao);
	void insert(CDBDaoImpl* pDao);
	void remove(CDBDaoImpl* pDao);
	void prepare(const char* Sql,CDBDaoImpl* pDao);
};

}

#endif
