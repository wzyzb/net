#ifndef _CRESULTSETIMPL_H_
#define _CRESULTSETIMPL_H_

#include <db/resultset.h>
#include "statementimpl.h"

// 查询返回的结果集
namespace mysql
{
	class CStatementImpl;
	class CCallableStatementImpl;
	//普通结果集
	class CResultSetImpl : public CResultSet
	{
	protected :
		//
		MYSQL_RES* m_pMetaData;
		//
		CConnectionImpl* m_pConn;
		//
		int m_nFieldCount;
		//字段名－索引对应map
		map<string,int> m_Fields;
		//结果字段集
		CResultData* m_pResultData;
		//结果行数据
		MYSQL_ROW m_Row;
	protected :
		/**
		 * 检查索引是否越界，如果是，则抛出异常
		 */
		void validIndex(int nIndex) const;
	public:
		CResultSetImpl(CConnectionImpl& Conn);
		//构造函数

		//析构函数
		~CResultSetImpl();

		// 关闭结果集。
		// 释放所有资源
		//
		void close();
		/**
		 * 初始化参数
		 * @return bool 初始化成功标识
		 */
		bool init();

		/**
		 *
		 */
		int getColumnCount() const;
		
		/**
		 *
		 */
		EDATATYPE getColumnDataType(int nIndex);

		/**
		 *
		 */
		std::string getColumnName(int nIndex) const;

		/**
		 * 结果集移动到下一行
		 * 返回是否有下一行
		 */
		bool next();

		// 根据列名查找列的索引ID
		// 如果找不到则返回-1
		//
		int findColumn(const char* pColumnName) const;

		/**
		 *
		 */
		char getByte(int nIndex,bool bNullThrow = true,char Default = 0) const;

		/**
		 *
		 */
		char getByte(const char* pColumnName,bool bNullThrow = true,char Default = 0) const;

		/*
		//
		bool getBoolean(int nIndex);

		//
		bool getBoolean(const char* pColumn);
		*/
		/**
		 *
		 */
		std::string getString(int nIndex,bool bNullThrow = true,const char* pDefault = DEFAULT_STRING) const;

		/**
		 *
		 */
		std::string getString(const char* pColumn,bool bNullThrow = true,const char* pDefault = DEFAULT_STRING) const;

		/**
		 *
		 */
		int getInt(int nIndex,bool bNulLThrow = true,int nDefault= DEFAULT_INT) const;

		/**
		 *
		 */
		int getInt(const char* pColumn,bool bNulLThrow = true,int nDefault= DEFAULT_INT) const;

		/**
		 *
		 */
		double getDouble(int nIndex,bool bNulLThrow = true,double dDefault= DEFAULT_DOUBLE) const;

		/**
		 *
		 */
		double getDouble(const char* pColumn,bool bNulLThrow = true,double dDefault= DEFAULT_DOUBLE) const;

		/**
		 *
		 */
		bool isNull(int nIndex) const;

		/**
		 *
		 */
		bool isNull(const char* pColumn) const;

		/**
		 *
		 */
		CDateTime getDate(int nIndex,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const;

		/**
		 *
		 */
		CDateTime getDate(const char* pColumn,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const;

		/**
		 *
		 */
		CDateTime getTime(int nIndex,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const;

		/**
		 *
		 */
		CDateTime getTime(const char* pColumn,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const;

		/**
		 *
		 */
		CDateTime getTimestamp(int nIndex,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const;

		/**
		 *
		 */
		CDateTime getTimestamp(const char* pColumn,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const;

		/**
		 *
		 */
		int64 getInt64(int nIndex,bool bNullThrow = true,int64 nDefault = DEFAULT_LONG64) const;

		/**
		 *
		 */
		int64 getInt64(const char* pParamName,bool bNullThrow = true,int64 nDefault = DEFAULT_LONG64) const;
		int getBytes(int nIndex,unsigned char* pOutBuffer,int nBuffSize,int nCharCount,bool bNullThrow = true,const char* pDefault = DEFAULT_STRING,unsigned int nOffset = 1) const;
		int getBytes(const char* pColumnName,unsigned char* pOutBuffer,int nBuffSize,int nCharCount,bool bNullThrow = true,const char* pDefault = DEFAULT_STRING,unsigned int nOffset = 1) const;
	};
	/*
	  preparestatement的结果集
	*/
	class CPrepareResultSetImpl : public CResultSet
	{
	protected :
		MYSQL_RES* m_pMetaData;
		CRefStatement* m_pStmt;
		//结果字段集
		CResultData* m_pResultData;
		//结果字段集
		MYSQL_BIND* m_pResult;
		//字段总数
		int m_nFieldCount;
		//字段名－索引对应map
		map<string,int> m_Fields;

	protected :
		/**
		 * 检查索引是否越界，如果是，则抛出异常
		 */
		void validIndex(int nIndex) const;
		/**
		 * 清除参数值
		 */
		void cleanParam(MYSQL_BIND& pBind);
	public:
		//构造函数
		CPrepareResultSetImpl(CStatementImpl& Stmt);
		//析构函数
		~CPrepareResultSetImpl();
		/**
		 * 初始化必须的资源
		 */
		bool init();
		// 关闭结果集。
		// 释放所有资源
		//
		void close();

		/**
		 *
		 */
		int getColumnCount() const;
		/**
		 *
		 */
		EDATATYPE getColumnDataType(int nIndex);

		/**
		 *
		 */
		std::string getColumnName(int nIndex) const;

		// 结果集移动到下一行
		// 返回是否有下一行
		//
		bool next();

		// 根据列名查找列的索引ID
		// 如果找不到则返回-1
		//
		int findColumn(const char* pColumnName) const;

		//
		char getByte(int nIndex,bool bNullThrow = true,char Default = 0) const;

		//
		char getByte(const char* pColumnName,bool bNullThrow = true,char Default = 0) const;

		/*
		//
		bool getBoolean(int nIndex);

		//
		bool getBoolean(const char* pColumn);
		*/
		//
		std::string getString(int nIndex,bool bNullThrow = true,const char* pDefault = DEFAULT_STRING) const;

		//
		std::string getString(const char* pColumn,bool bNullThrow = true,const char* pDefault = DEFAULT_STRING) const;

		//
		int getInt(int nIndex,bool bNulLThrow = true,int nDefault= DEFAULT_INT) const;

		//
		int getInt(const char* pColumn,bool bNulLThrow = true,int nDefault= DEFAULT_INT) const;

		//
		double getDouble(int nIndex,bool bNulLThrow = true,double dDefault= DEFAULT_DOUBLE) const;

		//
		double getDouble(const char* pColumn,bool bNulLThrow = true,double dDefault= DEFAULT_DOUBLE) const;

		//
		bool isNull(int nIndex) const;

		//
		bool isNull(const char* pColumn) const;

		//
		CDateTime getDate(int nIndex,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const;

		//
		CDateTime getDate(const char* pColumn,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const;

		//
		CDateTime getTime(int nIndex,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const;

		//
		CDateTime getTime(const char* pColumn,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const;

		//
		CDateTime getTimestamp(int nIndex,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const;

		//
		CDateTime getTimestamp(const char* pColumn,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const;

		//
		int64 getInt64(int nIndex,bool bNullThrow = true,int64 nDefault = DEFAULT_LONG64) const;

		//
		int64 getInt64(const char* pParamName,bool bNullThrow = true,int64 nDefault = DEFAULT_LONG64) const;
		int getBytes(int nIndex,unsigned char* pOutBuffer,int nBuffSize,int nCharCount,bool bNullThrow = true,const char* pDefault = DEFAULT_STRING,unsigned int nOffset = 1) const;
	    int getBytes(const char* pColumnName,unsigned char* pOutBuffer,int nBuffSize,int nCharCount,bool bNullThrow = true,const char* pDefault = DEFAULT_STRING,unsigned int nOffset = 1) const;
	};
}

#endif
