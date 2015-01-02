#ifndef _CSTATEMENTIMPL_H_
#define _CSTATEMENTIMPL_H_

#include <db/statement.h>

namespace avocado_oracle
{
class CConnectonImpl;
class CRefStatement;
class CResultSetImpl;
	// 处理带？参数的sql语句
	//
	class CStatementImpl : public CStatement
	{
	friend class CConnectionImpl;

	//在CResultSetImpl析构时，需要把CStatementImpl中指向自己的指针清空，所以，在这里需要访问m_pStmt
	friend class CResultSetImpl;	
	  protected:
		CConnectionImpl* m_pConn;
		CRefStatement* m_pRefStmt;
		oracle::occi::Statement*  m_pStmt;
		std::string               m_strErrMsg;
		std::string m_Sql;
		CResultSetImpl* m_pRst;
		map<int,Clob> m_Clobs;

		//构造函数
		CStatementImpl();

	  public:
		//析构函数
		~CStatementImpl();

			//
		void close();

		int getTimeZoneOffset(bool Throw=false);

		// 获取自增值
		//
		Long64 getInsertID(void* pParam);

		// 执行查询SQL并返回结果集
		CAutoPtr<CResultSet> executeQuery(const char* pSQL);

		/**
		 * 执行更新等语句并返回影响的行数
		 * @param pSQL 提交运行的sql语句
		 * @throws CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
		 */
		int executeUpdate(const char* pSQL);

		//
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

		//此方法,oracle版目前不支持
		void setByte(int nParamIndex, char Value);

		void saveBytes(const char* strColumnName, const unsigned char* Value, int nLength,bool SetSequence=false,string SequenceName="",bool SetPkName=false,string PkName="");

		//更新大对象字段前，未对statement执行executequery，则抛出异常
		void setBytes(int nParamIndex, const unsigned char* Value, int nLength);

		//
		//void setBoolean(int nParamIndex, bool Value);

		//
		void setString(int nParamIndex, const std::string& Value);

		//
		void setInt(int nParamIndex, int Value);

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
		void setLong64(int nParamIndex, Long64 Value); 	 

		void prepare(const char* pSql);

		const char* getSQL() const;

		string getInsertTableName();
	};
}// namespace avocado_oracle


#endif
