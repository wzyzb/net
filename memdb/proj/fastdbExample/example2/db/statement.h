#ifndef _CSTATEMENT_H_
#define _CSTATEMENT_H_

class CResultSet;
/**
 * SQL语句声明类
 * @doc 实现运行SQL语句的功能
 */
class CStatement
{
public:
	virtual ~CStatement() {};
	/**
	 * 关闭当前执行SQL语句的实例
	 * @param	无
	 * @return	无
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual void close()=0;

	/**
	 * 获取数据库的时区偏移量，例如北京时间返回8,美国东部时间返回-5
	 * @param Throw 当取不到时区时是否抛异常，不抛异常的话返回GMT/UTC(格林尼治时间)
	 * @return 当前时区与UTC的偏移量                                                                    
	 * @exception CAvoException 取时区偏移量异常
	 */	
	virtual int getTimeZoneOffset(bool Throw=false)=0;

	/**
	 * 获取插入前的自增值
	 * @param 如果在获取自增值时需要传入参数则使用此参数，例如oracle需要传入序列名	
	 * @return	自增值
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual uint64 getInsertID(void* pParam)=0;

	/**
	 * 执行查询SQL并返回结果集
	 * <br>对于使用同一个Cstatement读取两个以上结果集的情况，在下一次查询prepare前，需要先close上一个结果集
	 * @param	pSQL 提交运行的sql语句
	 * @return	指向结果集的智能指针
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual CAutoPtr<CResultSet> executeQuery(const char* pSQL)=0;

	/**
	 * 执行更新等语句并返回影响的行数
	 * @param	pSQL 提交运行的sql语句
	 * @return	受影响的行数
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual int executeUpdate(const char* pSQL)=0;

	/**
	 * 执行SQL语句
	 * @param	pSQL 提交运行的sql语句
	 * @return	布尔型，成功为true，否则为false
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual bool execute(const char* pSQL)=0;    

	/**
	 * 执行当前的SQL语句
	 * 由于不带参数，SQL语句应该由前面的prepare()等函数设定
	 * @param	无
	 * @return	布尔型，成功为true，否则为false
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual bool execute()=0;

	/**
	 * 预处理SQL语句
	 * 通常SQL语句需要设定参数时，调用此函数
	 * <br>对于使用同一个Cstatement读取两个以上结果集的情况，在下一次查询prepare前，需要先close上一个结果集
	 * @param	pSQL 提交运行的sql语句
	 * @return	无
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual void prepare(const char* pSql)=0;

	/** 
	 * 获取sql语句
	 * @return sql语句
	 * @note 当离开statement的作用域后，返回指针指向的内存会被释放
	 */
	virtual const char* getSQL() const =0;

	/**
	 * 执行当前SQL查询
	 * 返回结果集
	 * @param	无
	 * @return	指向结果集的智能指针
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual CAutoPtr<CResultSet> executeQuery()=0;

	/**
	 * 执行当前的更新SQL操作
	 * 返回执行影响的总行数
	 * @param	无
	 * @return	受影响的总行数
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual int executeUpdate()=0;

	/**
	 * 清空所有参数的值
	 * 如需执行SQL语句，需要重新设定参数值
	 * @param	无
	 * @return	无
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual void clearParameters()=0;

	/**
	 * 设置SQL语句参数索引ID的值
	 * @param	nParamIndex:	索引ID
	 *			Value:			要设置的值，为char型
	 * @return	无
	 * @exception CSQLException 设置失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual void setByte(int nParamIndex, char Value)=0;

	/**
	 * insert into table的情况下保存大对象值
	 * @param strColumnName 大对象字段名
	 * @param Value 大对象值
	 * @param nLength 传输的值的长度
	 * @param SetSequence 是否设置序列名（用于取自增值）
	 * @param SequenceName 序列名
	 * @param SetPkName 是否设置主键字段名（用于取自增值）
	 * @param PkName 主键字段名
	 * @exception CSQLException 保存大对象失败，具体原因请根据errcode查阅数据库错误代码说明
	 * @note 参见teststatement.cpp的testclob方法
 		conn->begin();
 		sql = "insert into testclob (value) values (EMPTY_CLOB())";
		stmt->prepare(sql.c_str());
		stmt->executeUpdate();
		
		unsigned char tmp[100]="a very log string";
		int length = strlen((const char *)tmp);
		stmt->saveBytes("value",tmp,length);
		conn->commit();
	 */	
	virtual void saveBytes(const char* strColumnName, const unsigned char* Value, int nLength,bool SetSequence=false,string SequenceName="",bool SetPkName=false,string PkName="")=0;

	/**
	 * update table的情况下设置大对象值
	 * @param	nParamIndex:	索引ID
	 *			Value:			要设置的值，为字节序列指针
	 *			nLength:		字节序列的长度
	 * @return	无
	 * @exception	CSQLException 设置大对象失败，具体原因请根据errcode查阅数据库错误代码说明
	 * @note
	 * 针对oracle版，实现对大对象字段的更新，需要使用select for update语句来更新大对象
	 * 更新步骤：
	 * stmt->prepare("select ... for update");
	 * stmt->executeQuery();
	 * stmt->setBytes();
	 * stmt->executeUpdate();
	 * 如果在setBytes前没调用executeQuery,则抛出错误码为ERROR_DATABASE_LOBUPDATE的异常
	 */
	virtual void setBytes(int nParamIndex, const unsigned char* Value, int nLength)=0;

	/**
	 * 设置SQL语句参数索引ID的值
	 * @param	nParamIndex:	索引ID
	 *			Value:			要设置的值，为布尔型
	 * @return	无
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	//virtual void setBoolean(int nParamIndex, bool Value)=0;

	/**
	 * 设置SQL语句参数索引ID的值
	 * @param	nParamIndex:	索引ID
	 *			Value:			要设置的值，为字符串
	 * @return	无
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual void setString(int nParamIndex, const std::string& Value)=0;

	/**
	 * 设置SQL语句参数索引ID的值
	 * @param	nParamIndex:	索引ID
	 *			Value:			要设置的值，为整型
	 * @return	无
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual void setInt(int nParamIndex, int Value) = 0;
	/**
	 * 设置SQL语句参数索引ID的值
	 * @param	nParamIndex:	索引ID
	 *			Value:			要设置的值，为整型
	 * @return	无
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual void setUInt(int nParamIndex, unsigned int Value) = 0;

	/**
	 * 设置SQL语句参数索引ID的值
	 * @param	nParamIndex:	索引ID
	 *			Value:			要设置的值，为浮点型
	 * @return	无
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual void setDouble(int nParamIndex, double Value)=0;

	/**
	 * 清空SQL语句参数索引ID的值
	 * @param	nParamIndex:	索引ID
	 *			nEDATATYPE:		要清空的参数类型
	 * @return	无
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual void setNull(int nParamIndex, EDATATYPE nEDATATYPE)=0;

	/**
	 * 设置SQL语句参数索引ID的值
	 * @param	nParamIndex:	索引ID
	 *			Value:			要设置的值，为CDateTime型
	 * @return	无
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual void setDate(int nParamIndex, CDateTime Value)=0;

	/**
	 * 设置SQL语句参数索引ID的值
	 * @param	nParamIndex:	索引ID
	 *			Value:			要设置的值，为CDateTime型
	 * @return	无
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual void setTime(int nParamIndex, CDateTime Value)=0;

	/**
	 * 设置SQL语句参数索引ID的值
	 * @param	nParamIndex:	索引ID
	 *			Value:			要设置的值，为CDateTime型
	 * @return	无
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual void setTimestamp(int nParamIndex, CDateTime Value)=0;

	/**
	 * 设置SQL语句参数索引ID的值
	 * @param	nParamIndex:	索引ID
	 *			Value:			要设置的值，为int64型，即64位整型
	 * @return	无
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual void setInt64(int nParamIndex, int64 Value)=0; 

	/**
	 * 设置SQL语句参数索引ID的值
	 * @param	nParamIndex:	索引ID
	 *			Value:			要设置的值，为int64型，即64位整型
	 * @return	无
	 * @exception	CSQLException sql语句运行失败，具体原因请根据errcode查阅数据库错误代码说明
	 */
	virtual void setUInt64(int nParamIndex, uint64 Value)=0; 

};

#endif
