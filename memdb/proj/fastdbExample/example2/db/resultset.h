#ifndef _CRESULTSET_H_
#define _CRESULTSET_H_

/**
 * 结果集类
 * @doc 维护查询返回的结果集
 */
class CResultSet
{
  public:
	virtual ~CResultSet(){};

    /**
	 * 关闭结果集。
     * 建议在resultset使用完毕后显式调用此方法来释放资源，提高程序性能
	 * @param	无
	 * @return	无
	 * @exception CSQLException 关闭失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual void close()=0;

	/**
	 * 返回结果集的列的个数。
	 * @param	无
	 * @return	结果集的列的个数
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual int getColumnCount() const=0;

	/**
	 * 返回结果集中相应列索引的列名
	 * 如果索引非法，则返回空字符串
	 * @param	nIndex，列索引
	 * @return	列名（字段名）,以大写字母返回
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual std::string getColumnName(int nIndex) const=0;

	/**
	 * 返回结果集中相应列索引的数据类型
	 * 如果索引非法，则抛出异常（code:-6）
	 * @param	nIndex，列索引
	 * @return	列的数据类型
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
	virtual EDATATYPE getColumnDataType(int nIndex)=0;

    /**
	 * 结果集移动到下一行
	 * @param	无
	 * @return	布尔型，（true：存在下一行记录，false：无下一行记录）
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual bool next()=0;

	/**
	 * 根据列名查找列的索引ID
	 * @param	pColumnName		列名（字段名）
	 * @return	返回列的索引ID，失败时返回-1
	 * @exception CSQLException 查找失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual int findColumn(const char* pColumnName) const =0;

   /**
	 * 根据列名读取大对象值	
	 * @param	pColumnName:要返回值的列名
	 * @param	pOutBuffer:	存放大对象的buff
	 * @param	nBuffSize:	存放大对象的buff的大小
	 * @param   nCharCount  需要读的字符数
	 * @param	bNullThrow	如果当前值为空，是否抛出异常，默认为TRUE
	 * @param	pDefault	如果bNullThrow为false，则当当前值为空则返回此默认值
	 * @param   nOffset     开始读大对象的位置
	 * @return	读到的大对象的字节数(相当与strlen(char*),如果是全是中文，GBK的情况下，实际的字符数就是return/2)
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 * @note 由于数据库采用UTF8字符集，所有nBuffSize的大小必须>实际字符数*3，如果nBuffSize大小不足，会抛出异常
	   调用示例参见teststatement.cpp testclob方法
	    sql = "select id,value from testclob";
		stmt3->prepare(sql.c_str());
		CAutoPtr<CResultSet> rst = stmt3->executeQuery();

		unsigned char testclob[40960]="";
		memset(testclob,0,40960);
		int buffsize = sizeof(testclob);		
		rst->next();		
		int gotcount = rst->getBytes(2,testclob,buffsize,buffsize/3);
		//如果需要控制读取的字符数，可以这么调用，例如只读前1000个字符
		//int gotcount = rst->getBytes(2,testclob,buffsize,1000);
		rst->close();
	 */
    virtual int getBytes(int nIndex,unsigned char* pOutBuffer,int nBuffSize,int nCharCount,bool bNullThrow = true,const char* pDefault = DEFAULT_STRING,unsigned int nOffset = 1) const =0;

	/**
	 * 根据列名读取大对象值	
	 * @param	pColumnName:要返回值的列名
	 * @param	pOutBuffer:	存放大对象的buff
	 * @param	nBuffSize:	存放大对象的buff的大小
	 * @param   nCharCount  需要读的字符数
	 * @param	bNullThrow	如果当前值为空，是否抛出异常，默认为TRUE
	 * @param	pDefault	如果bNullThrow为false，则当当前值为空则返回此默认值
	 * @return	读到的大对象的字节数(相当与strlen(char*),如果是全是中文，GBK的情况下，实际的字符数就是return/2)
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明	 
	 */
    virtual int getBytes(const char* pColumnName,unsigned char* pOutBuffer,int nBuffSize,int nCharCount,bool bNullThrow = true,const char* pDefault = DEFAULT_STRING,unsigned int nOffset = 1) const =0;

    /**
	 * 返回结果集中索引ID的值	
	 * @param	nIndex:		索引ID
	 * @return	布尔型
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    //virtual bool getBoolean(int nIndex)=0;

    /**
	 * 根据列名返回结果集的值	
	 * @param	pColumn:	要返回值的列名
	 * @return	布尔型
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    //virtual bool getBoolean(const char* pColumn)=0;

	/**
	 * 返回结果集中索引ID的值	
	 * @param	nIndex:		索引ID
	 * @param	bNullThrow	如果当前值为空，是否抛出异常，默认为TRUE
	 * @param	pDefault	如果bNullThrow为false，则当当前值为空则返回此默认值
	 * @return	字符串
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual std::string getString(int nIndex,bool bNulLThrow = true,const char* pDefault= DEFAULT_STRING) const =0;

    /**
	 * 根据列名返回结果集的值	
	 * @param	pColumn:	要返回值的列名
	 * @param	bNullThrow	如果当前值为空，是否抛出异常，默认为TRUE
	 * @param	pDefault	如果bNullThrow为false，则当当前值为空则返回此默认值
	 * @return	字符串
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual std::string getString(const char* pColumn,bool bNulLThrow = true,const char* pDefault= DEFAULT_STRING) const =0;

    /**
	 * 根据索引读取int值	
	 * @param	nIndex:		索引ID
	 * @param	bNullThrow	如果当前值为空，是否抛出异常，默认为TRUE
	 * @param	nDefault	如果bNullThrow为false，则当当前值为空则返回此默认值
	 * @return	整型
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual int getInt(int nIndex,bool bNulLThrow = true,int nDefault= DEFAULT_INT) const =0;

    /**
	 * 根据列名读取int值	
	 * @param	pColumn:	要返回值的列名
	 * @param	bNullThrow	如果当前值为空，是否抛出异常，默认为TRUE
	 * @param	nDefault	如果bNullThrow为false，则当当前值为空则返回此默认值
	 * @return	整型
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual int getInt(const char* pColumn,bool bNulLThrow = true,int nDefault= DEFAULT_INT) const =0;

    /**
	 * 根据索引读取Double值	
	 * @param	nIndex:		索引ID
	 * @param	bNullThrow	如果当前值为空，是否抛出异常，默认为TRUE
	 * @param	dDefault	如果bNullThrow为false，则当当前值为空则返回此默认值
	 * @return	double
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual double getDouble(int nIndex,bool bNulLThrow = true,double dDefault= DEFAULT_DOUBLE) const =0;

    /**
	 * 根据列名返回结果集的值	
	 * @param	pColumn:	要返回值的列名
	 * @param	bNullThrow	如果当前值为空，是否抛出异常，默认为TRUE
	 * @param	dDefault	如果bNullThrow为false，则当当前值为空则返回此默认值
	 * @return	double
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual double getDouble(const char* pColumn,bool bNulLThrow = true,double dDefault= DEFAULT_DOUBLE) const =0;

    /**
	 * 判断结果集中索引ID对应的字段是否为NULL
	 * @param	nIndex: 索引ID
	 * @return	布尔型，允许为空则返回true，否则false
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual bool isNull(int nIndex) const =0;

    /**
	 * 根据列名判断结果集中对应的字段是否为NULL
	 * @param	pColumn:	要判断值的列名
	 * @return	布尔型，允许为空则返回true，否则false
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual bool isNull(const char* pColumn) const =0;

    /**
	 * 返回结果集中索引ID的值	
	 * @param	nIndex: 索引ID
	 * @param	bNullThrow	如果当前值为空，是否抛出异常，默认为TRUE
	 * @param	Default	如果bNullThrow为false，则当当前值为空则返回此默认值
	 * @return	CDateTime
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual CDateTime getDate(int nIndex,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const =0;

    /**
	 * 根据列名返回结果集的值	
	 * @param	pColumn:	要返回值的列名
	 * @param	bNullThrow	如果当前值为空，是否抛出异常，默认为TRUE
	 * @param	Default	如果bNullThrow为false，则当当前值为空则返回此默认值
	 * @return	CDateTime
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual CDateTime getDate(const char* pColumn,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const =0;

    /**
	 * 返回结果集中索引ID的值	
	 * @param	nIndex: 索引ID
	 * @param	bNullThrow	如果当前值为空，是否抛出异常，默认为TRUE
	 * @param	Default	如果bNullThrow为false，则当当前值为空则返回此默认值
	 * @return	CDateTime
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual CDateTime getTime(int nIndex,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const =0;

    /**
	 * 根据列名返回结果集的值	
	 * @param	pColumn:	要返回值的列名
	 * @param	bNullThrow	如果当前值为空，是否抛出异常，默认为TRUE
	 * @param	Default	如果bNullThrow为false，则当当前值为空则返回此默认值
	 * @return	CDateTime
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual CDateTime getTime(const char* pColumn,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const =0;

    /**
	 * 返回结果集中索引ID的值	
	 * @param	nIndex: 索引ID
	 * @param	bNullThrow	如果当前值为空，是否抛出异常，默认为TRUE
	 * @param	Default	如果bNullThrow为false，则当当前值为空则返回此默认值
	 * @return	CDateTime
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual CDateTime getTimestamp(int nIndex,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const =0;

    /**
	 * 根据列名返回结果集的值	
	 * @param	pColumn:	要返回值的列名
	 * @param	bNullThrow	如果当前值为空，是否抛出异常，默认为TRUE
	 * @param	Default	如果bNullThrow为false，则当当前值为空则返回此默认值
	 * @return	CDateTime
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual CDateTime getTimestamp(const char* pColumn,bool bNullThrow = true,const CDateTime& Default = DEFAULT_DATETIME) const =0;

    /**
	 * 返回结果集中索引ID的值	
	 * @param	nIndex: 索引ID
	 * @param	bNullThrow	如果当前值为空，是否抛出异常，默认为TRUE
	 * @param	nDefault	如果bNullThrow为false，则当当前值为空则返回此默认值
	 * @return	int64，64位整形值
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual int64 getInt64(int nIndex,bool bNullThrow = true,int64 nDefault = DEFAULT_LONG64) const =0;

    /**
	 * 根据列名返回结果集的值	
	 * @param	pParamName:	要返回值的列名
	 * @param	bNullThrow	如果当前值为空，是否抛出异常，默认为TRUE
	 * @param	nDefault	如果bNullThrow为false，则当当前值为空则返回此默认值
	 * @return	int64，64位整形值
	 * @exception CSQLException 操作失败，具体原因请根据sqlcode查阅数据库错误代码说明
	 */
    virtual int64 getInt64(const char* pParamName,bool bNullThrow = true,int64 nDefault = DEFAULT_LONG64) const =0;

};



#endif /* CRESULTSET_H_HEADER_INCLUDED_BB3264A8 */
