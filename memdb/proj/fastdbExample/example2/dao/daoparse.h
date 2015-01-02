#ifndef _DAOPARSE_H_
#define _DAOPARSE_H_

#include <public.h>
#include "daodefs.h"
#include <string>
#include <list>
using namespace std;
#include "userexception.h"
#include "datetime.h"

struct _t_expression;
struct _t_orderlist;
struct _t_statement;

typedef struct _t_dao_order_field
{
	string fieldname;
	ORDER_MODE ordermode;
}dao_order_field;

typedef struct _t_dao_order
{
	list<dao_order_field> order_fields;
}dao_order;

typedef struct _t_dao_bind_param
{
	EDATATYPE ParamType;
	union
	{
		uint8 utinyint;
		uint16 usmallint;
		uint32 unumber;
		uint64 ubigint;
		int32 number;
		int64 bigint;
		char* pdata;
	}Data;
	uint32 DataSize;
	 _t_dao_bind_param()
	{
		memset(this,0,sizeof(struct _t_dao_bind_param));
	}
	_t_dao_bind_param(const struct _t_dao_bind_param& Other)
	{
		memcpy(this,&Other,sizeof(struct _t_dao_bind_param));
		if ( ( ParamType == TYPE_STRING || ParamType == TYPE_BLOB ) )
		{
			this->Data.pdata = (char*)malloc(this->DataSize);
			memcpy((void*)this->Data.pdata,(void*)Other.Data.pdata,DataSize);
		}
	}
	struct _t_dao_bind_param& operator = (const struct _t_dao_bind_param& Other)
	{
		memcpy(this,&Other,sizeof(struct _t_dao_bind_param));
		if ( ( ParamType == TYPE_STRING || ParamType == TYPE_BLOB ) )
		{
			this->Data.pdata = (char*)malloc(this->DataSize);
			memcpy(this->Data.pdata,Other.Data.pdata,DataSize);
		}
		return *this;
	}
	~_t_dao_bind_param()
	{
		if ( ( ParamType == TYPE_STRING || ParamType == TYPE_BLOB) && Data.pdata != NULL )
			free(Data.pdata);
	}
}dao_bind_param;


typedef struct _t_dao_exp_param
{
	bool isBindParam;
	EDATATYPE datatype;
	int32 datasize;
	union
	{
		char* str;
		int64 number;
		void* pBindData;
	}value;

	_t_dao_exp_param()
	{
		memset(this,0,sizeof(struct _t_dao_exp_param));
	}
	~_t_dao_exp_param()
	{
		if ( ( datatype == TYPE_STRING || datatype == TYPE_BLOB ) && value.str != NULL )
			free(value.str);
	}
	_t_dao_exp_param(const struct _t_dao_exp_param& Other)
	{
		memcpy(this,&Other,sizeof(struct _t_dao_exp_param));
		if ( ( datatype == TYPE_STRING || datatype == TYPE_BLOB ) )
		{
			this->value.str = (char*)malloc(this->datasize);
			memcpy(this->value.str,Other.value.str,datasize);
		}
	}

	struct _t_dao_exp_param& operator = (const struct _t_dao_exp_param& Other)
	{
		memcpy(this,&Other,sizeof(struct _t_dao_exp_param));
		if ( ( datatype == TYPE_STRING || datatype == TYPE_BLOB ) )
		{
			this->value.str = (char*)malloc(this->datasize);
			memcpy(this->value.str,Other.value.str,datasize);
		}
		return *this;
	}
}dao_exp_param;

typedef struct _t_dao_expression
{
	string name;
	PARAM_OPERMODE compmode;
	list<dao_exp_param> params;
	EXPRESS_JOINMODE joinmode;
	_t_dao_expression()
	{
		//memset(this,0,sizeof(struct _t_dao_expression));
	}
}dao_expression;

typedef struct _t_dao_statement
{
	list<dao_expression> expressions;
	dao_order order;
}dao_statement;

//Order 关键字
class CDaoOrderBy
{
public :
	dao_order m_Order;
public :
	CDaoOrderBy();
	void addOrderField(const string& FieldName,ORDER_MODE Mode);
};

//表达式
class CDaoExpression
{
public :
	dao_expression m_Expression;
public :
	CDaoExpression();
	CDaoExpression(const string& FieldName);
	CDaoExpression& operator = (const string& FieldName);
	CDaoExpression& compare(PARAM_OPERMODE CompareMode);
	CDaoExpression& addBindParam();
	CDaoExpression& addInt(int32 Value);
	CDaoExpression& addString(const string& Value);
	CDaoExpression& addInt64(int64 Value);
	CDaoExpression& addInt16(int16 Value);
	CDaoExpression& addDateTime(const CDateTime& Value);
};

struct _t_daoqueryst;

class CDaoSQLParser
{
public :
	string m_SQL;
	dao_statement m_Statement;
	int m_nBindParamCount;

protected :
	void copyExpression(list<dao_expression>& Expressions,const struct _t_expression* pExpression);
	void copyOrderList(dao_order& Order,const struct _t_orderlist* pOrderList);
	void copyStatement(dao_statement& Stmt,const _t_statement* pStmt);

private :
	CDaoSQLParser(const CDaoSQLParser& Other);

public :
	CDaoSQLParser();
	~CDaoSQLParser();
	CDaoSQLParser& operator = (const CDaoSQLParser& Other);
	void prepare(const char* pSQL);
	//
	CDaoSQLParser& And(CDaoExpression& Expression);
	CDaoSQLParser& Or(CDaoExpression& Expression);
	CDaoSQLParser& Orderby(CDaoOrderBy& Order);
	CDaoSQLParser& operator = (CDaoExpression& Expression);
	
	/**
	 * 获取绑定参数的个数
	 */
	int32 getBindParamCount();
	void clear();
};


#endif

