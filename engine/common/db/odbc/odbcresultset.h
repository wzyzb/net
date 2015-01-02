#ifndef _MY_ODBCRESULTSET_H_
#define _MY_ODBCRESULTSET_H_
namespace my_odbc
{
	class COdbcStatement;
	class COdbcCallableStatement;
	class COdbcConnection;
	//普通结果集
	class COdbcResultSet : public CResultSet
	{
		friend class my_odbc::COdbcStatement;
		friend class my_odbc::COdbcCallableStatement;
	private:
		COdbcStatement *m_pStmt;
		COdbcCallableStatement *m_pCallStmt;
		HSTMT m_Stmt;//用来保存STMT
		//
		int m_nFieldCount;
		//字段名－索引对应map
		map<string,int> m_Fields;
		//MYSQL_ROW m_Row;
		map<int,odbc_bind *> m_mapResultData;//结果选项
	protected :
		/**
		 * 检查索引是否越界，如果是，则抛出异常
		 */
		void validIndex(int nIndex);
		//清空绑定的值
		void clear();
	private:
		/**
		* 生成结果集
		*/
		void buildResultSet(void);
	public:
		COdbcResultSet(COdbcStatement& stmt);
		COdbcResultSet(COdbcCallableStatement& stmt);
		//构造函数

		//析构函数
		~COdbcResultSet();

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
		
		EDATATYPE getColumnDataType(int nIndex);
		/**
		 *
		 */
		string getColumnName(int nIndex) const;

		/**
		 * 结果集移动到下一行
		 * 返回是否有下一行
		 */
		bool next();

		// 根据列名查找列的索引ID
		// 如果找不到则返回-1
		//
		virtual int findColumn(const char* pColumnName) const;

		/**
		 *
		 */
		virtual char getByte(int nIndex,bool bNullThrow = true,char Default = 0) const;

		/**
		 *
		 */
		virtual char getByte(const char* pColumnName,bool bNullThrow = true,char Default = 0) const;

		/**
		 *
		 */
		virtual int getBytes(int nIndex,unsigned char* pOutBuffer,
					int nBuffSize,int nCharCount,bool bNullThrow = true,
					const char* pDefault = DEFAULT_STRING) const;

		/**
		 *
		 */
		virtual int getBytes(const char* pColumnName,unsigned char* pOutBuffer,
					int nBuffSize,int nCharCount,bool bNullThrow = true,
					const char* pDefault = DEFAULT_STRING) const;
		
		//
		virtual bool getBoolean(int nIndex) const;

		//
		virtual bool getBoolean(const char* pColumn) const;
		
		/**
		 *
		 */
		virtual string getString(int nIndex,bool bNullThrow = true,const char* pDefault = DEFAULT_STRING) const;

		/**
		 *
		 */
		virtual string getString(const char* pColumn,bool bNullThrow = true,const char* pDefault = DEFAULT_STRING) const;

		/**
		 *
		 */
		virtual int getInt(int nIndex,bool bNulLThrow = true,int nDefault= DEFAULT_INT) const;

		/**
		 *
		 */
		virtual int getInt(const char* pColumn,bool bNulLThrow = true,int nDefault= DEFAULT_INT) const;

		/**
		 *
		 */
		virtual double getDouble(int nIndex,bool bNulLThrow = true,double dDefault= DEFAULT_DOUBLE) const;

		/**
		 *
		 */
		virtual double getDouble(const char* pColumn,bool bNulLThrow = true,double dDefault= DEFAULT_DOUBLE) const;

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
		long64 getLong64(int nIndex,bool bNullThrow = true,long64 nDefault = DEFAULT_LONG64) const;

		/**
		 *
		 */
		long64 getLong64(const char* pParamName,bool bNullThrow = true,long64 nDefault = DEFAULT_LONG64) const;
	};
}
#endif