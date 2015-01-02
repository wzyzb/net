#ifndef _CSTATEMENTIMPL_H_
#define _CSTATEMENTIMPL_H_

#include <mysql.h>
#include <db/statement.h>
#include <util/refshare.h>

namespace mysql
{
	class CConnectionImpl;
	// 处理带？参数的sql语句
	//
	class CStatementImpl : public CStatement,public CRefShare
	{
	protected:
		friend class CPrepareResultSetImpl;
		//连接处理实例
		CConnectionImpl* m_pConn;
		//sql的语句
		std::string m_strSql;
		//mysql的preparedstatement的句柄
		MYSQL_STMT* m_pStmt;
		//所有的sql参数
		//CMetaData* m_pParams;
		MYSQL_BIND* m_pParams;
		//总参数个数
		int m_nParamCount;
		//
		std::string m_SQL;

	protected :
		/**
		 * 根据预提交语句，设置参数属性
		 */
		void prepareParams();
		/**
		 * 检查索引号是否合法
		 * @param nIndex int 索引号
		 */
		void validIndex(int nIndex);
		/**
		 * 删除所有的参数
		 */
		void destroyParams();
		/**
		 * 清空参数
		 * @param nIndex 索引号，从1开始，必须保证索引号有效
		 */
		void clearParam(int nIndex);
		/**
		 *
		 */
		void cleanParam(MYSQL_BIND& Param);

	public:
		/**
		 * 构造函数
		 */
		CStatementImpl(CConnectionImpl& Conn);
		/**
		 * 析构函数
		 */
		~CStatementImpl();
		/**
		 * 获取mysql auto_increment自动插入后的自动生成值
		 * @return uint64 返回的值
		 */
		uint64 getLastInsertID();
		//*********************CStatement的接口函数*******************************
		/**
		 * 关闭资源
		 */
		void close();
		/**
		 * 统一接口，获取插入的id值
		 * @param pParam 输入参数，对mysql没用
		 * @return int64 返回的撒值
		 */
		uint64 getInsertID(void* pParam);
		/**
		 * 执行查询SQL并返回结果集
		 */
		CAutoPtr<CResultSet> executeQuery(const char* pSQL);
		/**
		 * 执行更新等语句并返回影响的行数
		 * @param pSQL 提交运行的sql语句
		 * @throws CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
		 */
		int executeUpdate(const char* pSQL);
		/**
		 * 执行等语句并返回是否操作成功
		 * @param pSQL 提交运行的sql语句
		 * @throws CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
		 */
		bool execute(const char* pSQL);
		// 执行当前的SQL语句
		//
		bool execute();
		// 执行当前查询
		//
		CAutoPtr<CResultSet> executeQuery();

		// 执行当前的更新SQL操作
		// 返回执行影响的总行数
		//
		int executeUpdate();

		// 清空所有参数的值
		//
		void clearParameters();

		//
		void setByte(int nParamIndex, char Value);

		//
		void setBytes(int nParamIndex, const char* Value, int nLength);

		//
		//void setBoolean(int nParamIndex, bool Value);

		//
		void setString(int nParamIndex, const std::string& Value);

		//
		void setInt(int nParamIndex, int Value);
		void setUInt(int nParamIndex, unsigned int Value);

		//
		void setDouble(int nParamIndex, double Value);

		//
		void setNull(int nParamIndex, EDATATYPE nEDATATYPE);

		//
		void setDate(int nParamIndex, CDateTime Value);

		//
		void setTime(int nParamIndex, CDateTime Value);

		//
		void setTimestamp(int nParamIndex, CDateTime Value);

		//
		void setInt64(int nParamIndex,int64 Value);
		void setUInt64(int nParamIndex,uint64 Value);
		/**
		 * 预处理sql
		 */
		void prepare(const char* pSql);
		/**
		 * 获取当前的错误号
		 * @return int 错误号
		 */
		int getErrNo();
		/**
		 * 获取当前的错误描述
		 * @return char* 字符串的错误描述
		 */
		const char* getError();
		
		/** 
		 * 获取sql语句
		 * @return sql语句
		 * @note 当离开statement的作用域后，返回指针指向的内存会被释放
		 */
		const char* getSQL() const;
		int getTimeZoneOffset(bool Throw=false);
		void saveBytes(const char* strColumnName, const unsigned char* Value, int nLength,bool SetSequence=false,string SequenceName="",bool SetPkName=false,string PkName="");
		void setBytes(int nParamIndex, const unsigned char* Value, int nLength);
	};

	//采用引用计数的方式保护内部数据
	class CRefStatement : public CStatement
	{
	private :
		//proxy模式处理
		friend class CPrepareResultSetImpl;
		CStatementImpl* m_pStmtImpl;
	public :
		CRefStatement(CStatementImpl& Stmt);
		~CRefStatement();
		/**
		 * 关闭资源
		 */
		void close();
		/**
		 * 统一接口，获取插入的id值
		 * @param pParam 输入参数，对mysql没用
		 * @return int64 返回的撒值
		 */
		uint64 getInsertID(void* pParam);
		/**
		 * 执行查询SQL并返回结果集
		 */
		CAutoPtr<CResultSet> executeQuery(const char* pSQL);
		/**
		 * 执行更新等语句并返回影响的行数
		 * @param pSQL 提交运行的sql语句
		 * @throws CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
		 */
		int executeUpdate(const char* pSQL);
		/**
		 * 执行等语句并返回是否操作成功
		 * @param pSQL 提交运行的sql语句
		 * @throws CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
		 */
		bool execute(const char* pSQL);
		// 执行当前的SQL语句
		//
		bool execute();
		// 执行当前查询
		//
		CAutoPtr<CResultSet> executeQuery();

		// 执行当前的更新SQL操作
		// 返回执行影响的总行数
		//
		int executeUpdate();

		// 清空所有参数的值
		//
		void clearParameters();

		//
		void setByte(int nParamIndex, char Value);

		//
		void setBytes(int nParamIndex, const char* Value, int nLength);

		//
		//void setBoolean(int nParamIndex, bool Value);

		//
		void setString(int nParamIndex, const std::string& Value);

		//
		void setInt(int nParamIndex, int Value);
		void setUInt(int nParamIndex, unsigned int Value);

		//
		void setDouble(int nParamIndex, double Value);

		//
		void setNull(int nParamIndex, EDATATYPE nEDATATYPE);

		//
		void setDate(int nParamIndex, CDateTime Value);

		//
		void setTime(int nParamIndex, CDateTime Value);

		//
		void setTimestamp(int nParamIndex, CDateTime Value);

		//
		void setInt64(int nParamIndex, int64 Value);
		void setUInt64(int nParamIndex, uint64 Value);
		/**
		 * 预处理sql
		 */
		void prepare(const char* pSql);
		/**
		 * 获取当前的错误号
		 * @return int 错误号
		 */
		int getErrNo();
		/**
		 * 获取当前的错误描述
		 * @return char* 字符串的错误描述
		 */
		const char* getError();
		
		/** 
		 * 获取sql语句
		 * @return sql语句
		 * @note 当离开statement的作用域后，返回指针指向的内存会被释放
		 */
		const char* getSQL() const;
		int getTimeZoneOffset(bool Throw=false);
		void saveBytes(const char* strColumnName, const unsigned char* Value, int nLength,bool SetSequence=false,string SequenceName="",bool SetPkName=false,string PkName="");
		void setBytes(int nParamIndex, const unsigned char* Value, int nLength);
	};

}// namespace avocado_mysql


#endif
