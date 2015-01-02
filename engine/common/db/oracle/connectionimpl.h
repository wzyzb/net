#ifndef _CCONNECTIONIMPL_H_
#define _CCONNECTIONIMPL_H_

#include <occi.h>
#include <db/connection.h>

class CConnectionManager;

namespace avocado_oracle
{
	class CConnectionImpl : public CConnection
	{
		friend class CConnectionManager;
	protected:
		//occi
		oracle::occi::Environment* m_pEnv;
		oracle::occi::Connection* m_pConn;
//		CRefConnection* m_pRefConn;
		OCCI_STD_NAMESPACE::string m_UsrName;
		OCCI_STD_NAMESPACE::string m_UsrPwd;
		OCCI_STD_NAMESPACE::string m_Database;
		bool m_AutoCommit;
		bool m_AutoCommitTmp;
		bool m_bConnected;
		int m_BeginCount;

	public:
		enum ETranctionLevel
		{
			TRANSACTION_NONE ,
			TRANSACTION_READ_COMMITTED , 
			TRANSACTION_READ_UNCOMMITTED ,
			TRANSACTION_REPEATABLE_READ ,
			TRANSACTION_SERIALIZABLE ,
		};
		
		//构造函数,根据url读取,以后扩展时用
		//CConnectionImpl(const char* pConnectionURL);
		//构造函数
		CConnectionImpl(string UsrName,string UsrPwd,string DataBase);

		//析构函数
		~CConnectionImpl();

		//数据库连接克隆
		CAutoPtr<CConnection> clone();

		// 创建可执行的SQLStatement，用于执行普通的SQL语句
		CAutoPtr <CStatement> createStatement();

		// 设置自动提交事务属性
    
		void setAutoCommit(bool bAutoCommit);

		// 获取当前连接是否自动提交事务
    
		bool getAutoCommit();

		// 显式启动事务
    
		bool begin();

		// 执行事务
    
		bool commit();

		// 回滚事务
    
		bool rollback();

		// 连接数据库
		bool connect();

		// 关闭数据库连接
    
		void close();

		bool checkConnect(std::string MethodName);

		// 检查数据库连接是否已关闭
    
		bool isClosed();

		// 预执行SQL，可用于带参数SQL的语句
		CAutoPtr<CStatement> prepareStatement(const char* pSQL);

		//测试数据库连接专用方法
		bool testStatement();
		

		// 调用存储过程    
		CAutoPtr<CCallableStatement> prepareCall(const char* pSQL);

		

		// 设置事务级别：
		// 所有级别参考TRANSACTION_XXX的枚举值
    
		void setTransactionIsolation(int nLevel);

		//保存连接
		void keepActive();

		// 中止SQL语句，若需执行SQL语句需要重新调用createStatement

		bool terminateStatement(oracle::occi::Statement*  m_pStmt);

		//获取创建当前Connection的Environment

		oracle::occi::Environment* getEnvironment();

		//获取Connection
		oracle::occi::Connection* getConnection();
	};
}// namespace avocado_oracle 


#endif 
