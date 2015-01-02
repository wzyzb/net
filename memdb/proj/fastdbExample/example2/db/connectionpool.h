#ifndef _CCONNECTIONPOOL_H_
#define _CCONNECTIONPOOL_H_

template <class T>
class CPoolConnAutoPtr;
class CConnection;
class CConnectionManager;
class CPoolConnection;

/**
 * 数据库连接池
 * @doc 以集合的方式维护数据库连接。
 * <BR> 通常，中间件服务器在启动时，会建立一个数据库连接池，中间件服务DLL在需要时，可以在池中获取数据库连接来完成数据库操作。
 * <BR> 数据库连接池采用如下策略来管理连接
 * <BR> 
 */
class CConnectionPool
{
friend class CConnectionManager;
friend class CPoolConnection;
  public:
    /**
     * 从连接池中获取数据库连接，如果连接池中已经没有可用的数据库连接，而且连接数到达最大数，则抛出异常
     * @param nWaitMillis 等待的毫秒数，如果等待时间到达，则抛出异常，=0表示马上返回，<0表示无限等待，>0表示等待的毫秒数
	 * @return CAutoPtr<CPoolConnection> 数据库连接的智能指针
	 * @exception CSQLException 从连接池中获取数据库连接失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    CPoolConnAutoPtr<CPoolConnection> getConnection(long nWaitMillis);    

    /**
	 * 析构函数
	 */
    ~CConnectionPool();
	/**
	 * 保证数据库连接的激活状态
	 * 
	 */
	void keepActive();

  protected:

    //释放指定的连接
    void putConnection(CPoolConnection* pConn);

	//获取空闲的连接
	const CPoolConnection* getIdleConnection();

	//通过URL生成连接池
    CConnectionPool(const char* pURL, int nMaxConnections,const char* pUsr="",const char* pUsrPwd="");

  private:
    //
    CPoolConnection** m_pConnections;

    //
    //CConnectionManager* m_pConnectionManager;

    //最大的连接数
    int m_nMaxConnections;

	//最小连接数
	int m_nMinConnections;	

	//当前的连接数
	int m_nConnectionCount;

	//当前空闲的连接数
	int m_nIdleConnectionCount;

	//最后一次被取出的空闲连接的下标
	int m_nLastConnID;

	//数据库别名
	string m_Url;
	
	//用户名
	string m_Usr;

	//密码
	string m_UsrPwd;	

	CBaseLock* m_BaseLock;
};
#endif
