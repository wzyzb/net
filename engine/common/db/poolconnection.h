#ifndef _CPOOLCONNECTION_H_
#define _CPOOLCONNECTION_H_

class CConnectionPool;
/**
 * 池数据库连接
 * @doc 实现数据库连接的所有功能，并且提供在数据库连接池中维护的功能
 * <br> 此类不能被实例化，必须通过CConnectionPool的getConnection方法来获得它的智能指针
 */
class CPoolConnection:public CConnection
{
public:

	/**
	 * 创建CStatement，用于执行普通的SQL语句
	 * @return CAutoPtr<CStatement> Statement智能指针
	 * @exception CSQLException 创建CStatement失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    CAutoPtr <CStatement> createStatement();

    /**
	 * 设置自动提交事务属性
	 * @param bAutoCommit 是否自动提交（true:是；false:否）
	 */    
    void setAutoCommit(bool bAutoCommit);

    /**
	 * 获取当前连接是否自动提交事务
	 * @return 是否自动提交（true:是；false:否）
	 */    
    bool getAutoCommit();

    /**    
	 * 显式启动事务
	 * @return 启动是否成功（true:成功；false:失败）
	 */    
    bool begin();

    /**
	 * 提交事务
     * @return 提交是否成功（true:成功；false:失败）
	 * @exception CSQLException 提交失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    bool commit();

    /**
	 * 回滚事务    
	 * @return 回滚是否成功（true:成功；false:失败）
	 * @exception CSQLException 回滚失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    bool rollback();

    /**
	 * 连接数据库
	 * @return 启动是否成功（true:成功；false:失败）
	 * @exception CSQLException 数据库连接失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    bool connect();

    /**
	 * 关闭数据库连接
	 * @return 关闭是否成功（true:成功；false:失败）
	 * @exception CSQLException 数据库连接关闭失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */    
    void close();

    /**
	 * 检查数据库连接是否已关闭
	 * @return 数据库连接是否已关闭（true:是；false:否）
	 * @exception CSQLException 检查数据库连接关闭失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    bool isClosed();

    /**
	 * 创建准备了SQL语句的CStatement，可用于需要设置参数的SQL语句    
	 * @return CAutoPtr<CStatement> 准备了sql语句的Statement智能指针
	 * @exception CSQLException 创建准备了SQL语句的CStatement失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    CAutoPtr<CStatement> prepareStatement(const char* pSQL);

    /**
	 * 创建准备了SQL语句的CCallableStatement，可用于调用存储过程、函数或运行更新类的sql语句    
	 * @return CAutoPtr<CCallableStatement>
	 * @exception CSQLException 创建准备了SQL语句的CCallableStatement失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    CAutoPtr<CCallableStatement> prepareCall(const char* pSQL);

    /**
	 * 设置事务级别,由于occi不支持，所以位实现
	 */    
    void setTransactionIsolation(int nLevel);

	/**
	 * 保持连接，如果连接已经断开，则抛出异常
	 * @exception CSQLException 保持连接失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */    
	void keepActive();

	/**
	 * 析构函数
	 */
    ~CPoolConnection();

	/**
	 * 克隆
	 * CPoolConnection类不支持此方法，调用此方法会得到异常CSQLException 
	 */
	CAutoPtr<CConnection> clone();

protected:
friend class CConnectionPool;
friend class CPoolConnAutoPtr<CPoolConnection>;
	CPoolConnection(CConnection* pConnection,CConnectionPool* pConnectionPool);

	void putConnection();
	//
	CConnection* m_pConnection;

	//
	CConnectionPool* m_pConnectionPool;

	bool m_bIsIdle;
public : 
	//
	unsigned int m_nOwnThreadId;
};



#endif
