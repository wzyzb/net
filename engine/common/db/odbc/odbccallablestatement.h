#ifndef _ODBCCALLABLESTATEMENT_H_
#define _ODBCCALLABLESTATEMENT_H_
#include "../dbinterface.h"
#include "odbcpublic.h"
#include "odbccallablestatement.h"
class CCallableStatement;
namespace my_odbc
{
	class COdbcConnection;
	class COdbcStatement;
	class COdbcResultSet;
	class COdbcCallableStatement
		:public CCallableStatement
	{
	public:
		COdbcCallableStatement(COdbcConnection& Conn);
		virtual ~COdbcCallableStatement();
		
		/**
		* 注册存储过程返回值为结果集
		* @param	nIndex: 列索引ID
		* @return	布尔型，注册成功为true，否则为false
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual bool registerResultset(int nIndex);
		
		/**
		* 注册函数返回值为结果集
		* @param	nEDATATYPE: 返回值的类型
		* @return	布尔型，注册成功为true，否则为false
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual bool registerReturn(EDATATYPE nEDATATYPE);
		
		/**
		* 执行存储过程或函数
		* @param  无
		* @return 布尔型，成功执行返回true，否则返回false
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual bool execute();
		
		/**
		* 关闭当前执行SQL语句的Statement，
		* 如果需要执行SQL语句，则要重新创建Statement，
		* @param  无
		* @return 无
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual void close();
		
		/**
		* 设置SQL语句参数索引ID的值
		* @param	nParamIndex:	索引ID
		*			Value:			要设置的值，为char型
		*			isRegOut:		是否注册为输出
		* @return	无
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual void setByte(int nParamIndex, char Value, bool isRegOut=false);
		
		/**
		* 设置SQL语句参数索引ID的值
		* @param	nParamIndex:	索引ID
		*			Value:			要设置的值，为布尔型
		*			isRegOut:		是否注册为输出
		* @return	无
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		//virtual void setBoolean(int nParamIndex, bool Value, bool isRegOut=false)=0;
		
		/**
		* 设置SQL语句参数索引ID的值
		* @param	nParamIndex:	索引ID
		*			Value:			要设置的值，为字节序列指针
		*			nLength:		字节序列的长度
		*			isRegOut:		是否注册为输出
		* @return	无
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual void setBytes(int nParamIndex, const char* Value, int nLength, bool isRegOut=false);
		
		/**
		* 设置SQL语句参数索引ID的值
		* @param	nParamIndex:	索引ID
		*			Value:			要设置的值，为字符串
		*			isRegOut:		是否注册为输出
		* @return	无
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual void setString(int nParamIndex, const std::string& Value, bool isRegOut=false);
		
		/**
		* 设置SQL语句参数索引ID的值
		* @param	nParamIndex:	索引ID
		*			Value:			要设置的值，为CDateTime型
		*			isRegOut:		是否注册为输出
		* @return	无
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual void setDate(int nParamIndex, CDateTime Value, bool isRegOut=false);
		
		/**
		* 设置SQL语句参数索引ID的值
		* @param	nParamIndex:	索引ID
		*			Value:			要设置的值，为浮点型
		*			isRegOut:		是否注册为输出
		* @return	无
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual void setDouble(int nParamIndex, double Value, bool isRegOut=false);
		
		/**
		* 设置SQL语句参数索引ID的值
		* @param	nParamIndex:	索引ID
		*			Value:			要设置的值，为整型
		*			isRegOut:		是否注册为输出
		* @return	无
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual void setInt(int nParamIndex, int Value, bool isRegOut=false);
		
		/**
		* 清空SQL语句参数索引ID的值
		* @param	nParamIndex:	索引ID
		*			nEDATATYPE:		要清空的参数类型
		*			isRegOut:		是否注册为输出
		* @return	无
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual void setNull(int nParamIndex, EDATATYPE nEDATATYPE, bool isRegOut=false);
		
		/**
		* 设置SQL语句参数索引ID的值
		* @param	nParamIndex:	索引ID
		*			Value:			要设置的值，为CDateTime型
		*			isRegOut:		是否注册为输出
		* @return	无
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual void setTime(int nParamIndex, CDateTime Value, bool isRegOut=false);
		
		/**
		* 设置SQL语句参数索引ID的值
		* @param	nParamIndex:	索引ID
		*			Value:			要设置的值，为CDateTime型
		*			isRegOut:		是否注册为输出
		* @return	无
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual void setTimestamp(int nParamIndex, CDateTime Value, bool isRegOut=false);
		
		/**
		* 设置SQL语句参数索引ID的值
		* @param	nParamIndex:	索引ID
		*			Value:			要设置的值，为Long64型，即64位整型
		*			isRegOut:		是否注册为输出
		* @return	无
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual void setLong64(int nParamIndex, Long64 Value, bool isRegOut = false);
		
		/**
		* 是否还有更多结果集
		* 用于判断存储过程和函数
		* @param	无
		* @return	布尔型，有结果集则返回true，否则返回false
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual bool hasMoreResultsets();
		
		/**
		* 获取下一个结果集
		* @param	无
		* @return	指向类CResultSet的CAutoPtr智能指针
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual CAutoPtr<CResultSet> getNextResultSet();
		
		/**
		* 返回参数索引ID的值	
		* @param	nIndex: 索引ID
		* @return	类型为char
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual char getByte(int nIndex);
		
		/**
		* 根据列名返回值	
		* @param	pColumnName: 列名（字段名）
		* @return	类型为char
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual char getByte(const char* pColumnName);
		
		/**
		* 返回参数索引ID的值	
		* @param	nIndex:		索引ID
		*			pOutBuffer: 返回字节序列的地址
		*			nLength:	字节序列的长度
		* @return	返回值类型为布尔型
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual bool getBytes(int nIndex, char* pOutBuffer, int& nLength);
		
		/**
		* 根据列名返回值
		* @param	pColumnName: 列名（字段名）
		*			pOutBuffer:  返回字节序列的地址
		*			nLength:	 字节序列的长度
		* @return	返回值类型为布尔型
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual bool getBytes(const char* pColumnName, char* pOutBuffer, int& nLength);
		
		/**
		* 返回参数索引ID的值
		* @param	nIndex: 索引ID
		* @return	返回值类型为布尔型，成功为true，否则为false
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		//virtual bool getBoolean(int nIndex)=0;
		
		/**
		* 根据列名返回值
		* @param	pColumn: 列名（字段名）
		* @return	返回值类型为布尔型，成功为true，否则为false
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		//virtual bool getBoolean(const char* pColumn)=0;
		
		/**
		* 返回参数索引ID的值	
		* @param	nIndex: 索引ID
		* @return	返回值类型为字符串
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual std::string getString(int nIndex);
		
		/**
		* 根据列名返回值
		* @param	pColumn: 列名（字段名）
		* @return	返回值类型为字符串
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual std::string getString(const char* pColumn);
		
		/**
		* 返回参数索引ID的值	
		* @param	nIndex: 索引ID
		* @return	返回值类型为整型
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual int getInt(int nIndex);
		
		/**
		* 根据列名返回值
		* @param	pColumn: 列名（字段名）
		* @return	返回值类型为整型
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual int getInt(const char* pColumn);
		
		
		/**
		* 返回参数索引ID的值	
		* @param	nIndex: 索引ID
		* @return	返回值类型为浮点型
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual double getDouble(int nIndex);
		
		/**
		* 根据列名返回值
		* @param	pColumn: 列名（字段名）
		* @return	返回值类型为浮点型
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual double getDouble(const char* pColumn);
		
		/**
		* 判断参数索引ID的值是否为空（NULL）
		* @param	nIndex: 索引ID
		* @return	返回值类型为布尔型，如果为空则返回true，否则为false
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual bool isNull(int nIndex);
		
		/**
		* 根据列名判断值是否为空（NULL）
		* @param	pColumn: 列名（字段名）
		* @return	返回值类型为布尔型，如果为空则返回true，否则为false
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual bool isNull(const char* pColumn);
		
		/**
		* 返回参数索引ID的值	
		* @param	nIndex: 索引ID
		* @return	返回值类型为CDateTime
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual CDateTime getDate(int nIndex);
		
		/**
		* 根据列名返回值
		* @param	pParamName: 列名（字段名）
		* @return	返回值类型为CDateTime
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual CDateTime getDate(const char* pParamName);
		
		/**
		* 返回参数索引ID的值	
		* @param	nIndex: 索引ID
		* @return	返回值类型为CDateTime
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual CDateTime getTime(int nIndex);
		
		/**
		* 根据列名返回值
		* @param	pParamName: 列名（字段名）
		* @return	返回值类型为CDateTime
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual CDateTime getTime(const char* pParamName);
		
		/**
		* 返回参数索引ID的值	
		* @param	nIndex: 索引ID
		* @return	返回值类型为CDateTime
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual CDateTime getTimestamp(int nIndex);
		
		/**
		* 根据列名返回值
		* @param	pColumn: 列名（字段名）
		* @return	返回值类型为CDateTime
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual CDateTime getTimestamp(const char* pColumn);
		
		/**
		* 返回参数索引ID的值	
		* @param	nIndex: 索引ID
		* @return	返回值类型为Long64，64位整型
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual Long64 getLong64(int nIndex);
		
		/**
		* 根据列名返回值
		* @param	pParamName: 列名（字段名）
		* @return	返回值类型为Long64
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual Long64 getLong64(const char* pParamName);
		
		/**
		* 预执行SQL语句
		* 执行后可以设置参数值，
		* @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
		*/
		virtual void prepare(const char* pSql);
	public:
		HSTMT getStmt(){ return m_pStmt; }
		void clearResultSet();
	private:
		virtual void clearParameters();
		/**
		 * 检查索引号是否合法
		 * @param nIndex int 索引号
		 */
		virtual void validIndex(int nIndex);
		//写入大对象
		void writeBinaryData();
	private:
		HSTMT m_pStmt;
		SQLSMALLINT m_nParamCount;
		map<int,odbc_bind *> m_mapParams;//结果选项
		COdbcResultSet *m_pUsedResultSet;
		bool m_nResultsetCount;
	};
}
#endif