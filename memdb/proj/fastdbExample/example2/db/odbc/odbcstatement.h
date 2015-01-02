#ifndef _MY_ODBCSTATEMENT_H_
#define _MY_ODBCSTATEMENT_H_
#include "odbcconnection.h"
namespace my_odbc
{
	class COdbcConnection;
	class COdbcStatement : public CStatement
	{
	protected:
		//连接处理实例
		COdbcConnection* m_pConn;
		//sql的语句
		std::string m_strSql;
		//mysql的preparedstatement的句柄
		HSTMT m_pStmt;

		map<int,odbc_bind *> m_mapParams;//结果选项
		//总参数个数
		int m_nParamCount;

	protected :
		/**
		 * 根据预提交语句，设置参数属性
		 */
		virtual void prepareParams(){};
		/**
		 * 检查索引号是否合法
		 * @param nIndex int 索引号
		 */
		virtual void validIndex(int nIndex);

	public:
		/**
		 * 构造函数
		 */
		COdbcStatement(COdbcConnection& Conn);
		/**
		 * 析构函数
		 */
		~COdbcStatement();

		virtual int getTimeZoneOffset(bool Throw=false);

		virtual const char* getSQL() const;
		/**
		 * 获取mysql auto_increment自动插入后的自动生成值
		 * @return ULong64 返回的值
		 */
		virtual long64 getLastInsertID();
		//*********************CStatement的接口函数*******************************
		/**
		 * 关闭资源
		 */
		virtual void close();
		/**
		 * 统一接口，获取插入的id值
		 * @param pParam 输入参数，对mysql没用
		 * @return Long64 返回的撒值
		 */
		virtual long64 getInsertID(void* pParam);
		/**
		 * 执行查询SQL并返回结果集
		 */
		virtual CAutoPtr<CResultSet> executeQuery(const char* pSQL);
		/**
		 * 执行更新等语句并返回影响的行数
		 * @param pSQL 提交运行的sql语句
		 * @throws CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
		 */
		virtual int executeUpdate(const char* pSQL);
		/**
		 * 执行等语句并返回是否操作成功
		 * @param pSQL 提交运行的sql语句
		 * @throws CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
		 */
		virtual bool execute(const char* pSQL);
		// 执行当前的SQL语句
		//
		virtual bool execute();
		// 执行当前查询
		//
		virtual CAutoPtr<CResultSet> executeQuery();

		// 执行当前的更新SQL操作
		// 返回执行影响的总行数
		//
		virtual int executeUpdate();

		// 清空所有参数的值
		//
		virtual void clearParameters();

		//
		virtual void saveBytes(const char* strColumnName, const unsigned char* Value,
			int nLength,bool SetSequence=false,string SequenceName="",
			bool SetPkName=false,string PkName="");
		//

		virtual void setByte(int nParamIndex, char Value);
		//
		virtual void setBytes(int nParamIndex, const unsigned char* Value, int nLength);

		//
		virtual void setBoolean(int nParamIndex, bool Value);

		//
		virtual void setString(int nParamIndex, const std::string& Value);

		//
		virtual void setInt(int nParamIndex, int Value);

		//
		virtual void setDouble(int nParamIndex, double Value);

		//
		virtual void setNull(int nParamIndex, EDATATYPE nEDATATYPE);

		//
		virtual void setDate(int nParamIndex, CDateTime Value);

		//
		virtual void setTime(int nParamIndex, CDateTime Value);

		//
		virtual void setTimestamp(int nParamIndex, CDateTime Value);

		//
		virtual void setLong64(int nParamIndex, long64 Value); 	 
		/**
		 * 预处理sql
		 */
		virtual void prepare(const char* pSql);
		/**
		 * 获取当前的错误号
		 * @return int 错误号
		 */
		virtual int getErrNo();
		/**
		 * 获取当前的错误描述
		 * @return char* 字符串的错误描述
		 */
		virtual const char* getError();


		HSTMT getStmt() const;

		void clearResultSet();
	private:
		//写入大对象
		void writeBinaryData();
		COdbcResultSet *m_pUsedResultSet;
	};
}
#endif