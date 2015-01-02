#ifndef _MY_ODBCCONNECTION_H_
#define _MY_ODBCCONNECTION_H_
#include "./odbcpublic.h"
#include <util/lightlock.h>
namespace my_odbc 
{
	class COdbcConnection:
		public CConnection
	{
		friend class COdbcStatement;
		friend class COdbcResultSet;
		friend class COdbcCallableStatement;
	protected :
		SQLHDBC m_pConn;
		//用户名
		std::string m_UserName;
		//密码
		std::string m_PassWord;
		//连接地址
		std::string m_ConnStr;
		//连接端口
		unsigned short m_nPort;
		//连接标识
		bool m_bConnected;
		//自动提交标识
		bool m_bAutoCommit;

	public :

		SQLHDBC getConnHandle(){ return m_pConn; }
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
		COdbcConnection(const std::string& Host,unsigned short Port = 0,
					const std::string& UserName = "",const std::string& Password = "");
		//构造函数
		COdbcConnection();
		//析构函数
		virtual ~COdbcConnection();
		//克隆
		virtual CAutoPtr<CConnection> clone();
		//创建可执行的SQLStatement，用于执行普通的SQL语句 
		CAutoPtr<CStatement> createStatement();
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
		//重新连接数据库
		bool reconnect();
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
		/**
		* 设置字符集编码
		* @pEncode 客户端
		* @retrun 字符集编码方式
		*/
		virtual bool setEncoding(const char * pEncode = "gb2312");
		/**
		* @param pConnString连接字符串
		* @param 成功返回TRUE失败返回FALSE
		*/
		virtual bool setConnectString(const char * pConnString , const char * pUserName , const char * pUserPwd);
		//************
		/**
		 * 获取mysql的错误号
		 * @return int 错误号
		 */
		virtual int getErrNo() const;
		/**
		 * 获取mysql的错误的字符串描述
		 * @return char* 字符串描述
		 */
		virtual const char* getError() const;
	public:
		//mysql c api
		static CLightLock m_CountLock;
		static int m_ConnCount;
		static SQLHENV m_pEven; 
		static void initialize();
		static void finalize();
	};
}
#endif