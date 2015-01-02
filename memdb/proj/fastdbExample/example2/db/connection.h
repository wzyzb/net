#ifndef _CCONNECTION_H_
#define _CCONNECTION_H_

class CStatement;
class CCallableStatement;

/**
 * 数据库连接类
 * @doc 实现数据库连接功能。
 * <BR> 用于创建运行sql语句的CStatement；创建可调用存储过程、函数的CCallableStatement。
 * <BR> 用于维护事务，提供事务的建立、提交、回滚操作。默认情况下，每个sql语句是自己提交的。
 * <BR> 此类为抽象类，不能实例化，可以通过CConnectionManager::getConnection方法获取它的智能指针。
 */
class CConnection
{
  public:
	  virtual ~CConnection(){};

    /**
	 * 创建CStatement，用于执行普通的SQL语句
	 * @return CAutoPtr<CStatement> Statement智能指针	
	 * @exception CSQLException 创建CStatement失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */    
    virtual CAutoPtr < CStatement > createStatement()=0;

	/**
	 * 设置自动提交事务属性
	 * @param bAutoCommit 是否自动提交（true:是；false:否）
	 */    
    virtual void setAutoCommit(bool bAutoCommit)=0;

    /**
	 * 获取当前连接是否自动提交事务
	 * @return 是否自动提交（true:是；false:否）
	 */
    virtual bool getAutoCommit()=0;

	/**    
	 * 显式启动事务
	 * @return 启动是否成功（true:成功；false:失败）
	 */    
    virtual bool begin()=0;

	/**
	 * 提交事务
     * @return 提交是否成功（true:成功；false:失败）
	 * @exception CSQLException 提交失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual bool commit()=0;

	/**
	 * 回滚事务    
	 * @return 回滚是否成功（true:成功；false:失败）
	 * @exception CSQLException 回滚失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual bool rollback()=0;

    /**
	 * 连接数据库
	 * @return 启动是否成功（true:成功；false:失败）
	 * @exception CSQLException 数据库连接失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual bool connect()=0;

    /**
	 * 关闭数据库连接
	 * @return 关闭是否成功（true:成功；false:失败）
	 * @exception CSQLException 数据库连接关闭失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual void close()=0;

    /**
	 * 检查数据库连接是否已关闭
	 * @return 数据库连接是否已关闭（true:是；false:否）
	 * @exception CSQLException 检查数据库连接关闭失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */    
    virtual bool isClosed()=0;

	/**
	 * 创建准备了SQL语句的CStatement，可用于需要设置参数的SQL语句    
	 * @return CAutoPtr<CStatement> CStatement智能指针
	 * @exception CSQLException 创建准备了SQL语句的CStatement失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual CAutoPtr<CStatement> prepareStatement(const char* pSQL)=0;

    
	/**
	 * 创建准备了SQL语句的CCallableStatement，可用于调用存储过程、函数或运行更新类的sql语句    
	 * @return CAutoPtr<CCallableStatement> CCallableStatement智能指针
	 * @exception CSQLException 创建准备了SQL语句的CCallableStatement失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual CAutoPtr< CCallableStatement > prepareCall(const char* pSQL)=0;

    // 设置事务级别：
    // 所有级别参考TRANSACTION_XXX的枚举值    
    virtual void setTransactionIsolation(int nLevel)=0;

	/**
	 * 保持连接，如果连接已经断开，则抛出异常
	 * @exception CSQLException 保持连接失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */  
	virtual void keepActive()=0;

	/**
	 * 数据库连接克隆，即获取连接参数相同的另一个连接
	 * @return CAutoPtr<CConnection> CConnection智能指针
	 * @exception CSQLException
	 */
	virtual CAutoPtr<CConnection> clone()=0;
	
	static const int TRANSACTION_NONE;
    
    static const int TRANSACTION_READ_COMMITTED;
    
    static const int TRANSACTION_READ_UNCOMMITTED;
    
    static const int TRANSACTION_REPEATABLE_READ;
    
    static const int TRANSACTION_SERIALIZABLE;
};

/*
const int CConnection::TRANSACTION_NONE=1;
const int CConnection::TRANSACTION_READ_COMMITTED=2;
const int CConnection::TRANSACTION_READ_UNCOMMITTED=3;
const int CConnection::TRANSACTION_REPEATABLE_READ=4;
const int CConnection::TRANSACTION_SERIALIZABLE=5;
*/

#endif 
