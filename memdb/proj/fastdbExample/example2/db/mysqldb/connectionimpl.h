#ifndef _CCONNECTIONIMPL_H_
#define _CCONNECTIONIMPL_H_

#include <mysql.h>
#include <db/connection.h>

class CConnectionManager;

namespace mysql
{
	class CStatementImpl;
	class CResultsetImpl;
	class CConnectionImpl : public CConnection
	{
		friend class CStatementImpl;
		friend class CResultSetImpl;
		friend class CCallableStatementImpl;
	protected :
		//mysql c api
		MYSQL* m_pConn;
		//用户名
		string m_UserName;
		//密码
		string m_PassWord;
		//数据库名
		string m_Database;
		//连接地址
		string m_Host;
		//连接端口
		unsigned short m_nPort;
		//连接标识
		bool m_bConnected;
		//自动提交标识
		bool m_bAutoCommit;

	public :
		//构造函数,根据url读取,以后扩展时用
		//CConnectionImpl(const char* pConnectionURL);
		//构造函数
		/**
		 * 构造函数
		 * @param Host string 主机地址
		 * @param Port short 主机端口
		 * @param DataBase string 数据库名
		 * @param UserName 用户名
		 * @param PassWord 密码
		 */
		CConnectionImpl(const string& Host,unsigned short Port = 0,const string& DataBase = "",const string& UserName = "",const string& Password = "");

		//析构函数
		virtual ~CConnectionImpl();
		//克隆
		CAutoPtr <CConnection> clone();
		//创建可执行的SQLStatement，用于执行普通的SQL语句 
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
		// 检查数据库连接是否已关闭
		bool isClosed();
		//预执行SQL，可用于带参数SQL的语句
		CAutoPtr<CStatement> prepareStatement(const char* pSQL);
		//调用存储过程    
		CAutoPtr<CCallableStatement> prepareCall(const char* pSQL);
		// 设置事务级别：
		// 所有级别参考TRANSACTION_XXX的枚举值
		void setTransactionIsolation(int nLevel);
		//保存连接
		void keepActive();
		//************
		/**
		 * 获取mysql的错误号
		 * @return int 错误号
		 */
		int getErrNo();
		/**
		 * 获取mysql的错误的字符串描述
		 * @return char* 字符串描述
		 */
		const char* getError();
	};
}// namespace avocado_mysql

#endif
