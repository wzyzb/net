#if !defined(_CCONNECTIONMANAGER_H_)
#define _CCONNECTIONMANAGER_H_

class CConnection;
class CConnectionPool;

/**
 * 数据库连接管理器
 * @doc 根据指定的连接字符串生成实际的数据库连接
 * <br> 由于所提供的方法都是静态方法，所以不需要实例化，构造函数为私有
 */
class CConnectionManager
{
private:
public:
	/**
	 * 获取数据库连接
	 * @param pConnectionURL 数据库名称或连接别名
	 * @param UserName 登陆用户名
	 * @param UserPwd 登陆密码
	 * @return CAutoPtr<CConnection> 数据库连接的智能指针
	 * @exception CSQLException 数据库连接失败，具体原因请根据sqlerrcode查阅数据库错误代码说明
	 */
	static CAutoPtr<CConnection> getConnection(const char* pConnectionURL,const char* UserName="",const char* UserPwd="");

	/**
	 * 获取数据库连接池
	 * @param pConnectionURL 数据库名称或连接别名
	 * @param nMaxConnections 连接池最大连接数
	 * @param UserName 登陆用户名
	 * @param UserPwd 登陆密码
	 * @return CAutoPtr<CConnection> 数据库连接池的智能指针
	 */
	static CAutoPtr <CConnectionPool> getConnectionPool(const char* pConnectionURL,int nMaxConnections,const char* UserName="",const char* UserPwd="");

private:

	CConnectionManager();
};

#endif 
