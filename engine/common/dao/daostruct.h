#ifndef _DAOSTRUCT_H_
#define _DAOSTRUCT_H_

#include "daodefs.h"
#ifdef __cplusplus
extern "C" {
#endif
#define dbdatetime uint32

void throwException(const char* fmt,...);

typedef struct _t_valuelist
{
	int valuetype;
	union
	{
		char* str;
		int num;
	}value;
	struct _t_valuelist* pnext;
}valuelist;

typedef struct _t_orderfield
{
	char* str;
	int ordermode;
	struct _t_orderfield *next;
}orderfield;

typedef struct _t_param
{
	char* name;
	int paramopermode;
	valuelist* values;
}param;

typedef struct _t_orderlist
{
	orderfield* fields;
}orderlist;

typedef struct _t_expression
{
	param* left;
	int joinmode;
	struct _t_expression* right;
}expression;

typedef struct _t_statement
{
	expression* expressions;
	struct _t_orderlist* porder;
}statement;

void dao_free_statement(struct _t_statement* pStatement);
void dao_free_expression(struct _t_expression* pExpression);
void dao_free_orderlist(struct _t_orderlist* pOrderList);
void dao_free_param(struct _t_param* pParam);
void dao_free_valuelist(struct _t_valuelist* pValues);
void dao_free_orderlist(struct _t_orderlist* pValues);
void dao_free_orderfield(struct _t_orderfield* pValues);

struct _t_statement* dao_new_statement();
struct _t_expression* dao_new_expression();
struct _t_orderlist* dao_new_orderlist();
struct _t_orderfield* dao_new_orderfield();
struct _t_param* dao_new_param();
struct _t_valuelist* dao_new_valuelist();

#ifdef __cplusplus
}
#endif

#endif

