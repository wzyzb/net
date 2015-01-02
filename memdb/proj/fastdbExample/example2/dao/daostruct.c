#include <stdlib.h>
#include <stdio.h>
#include <dao/daostruct.h>

void dao_free_valuelist(struct _t_valuelist* pValues)
{
	if ( pValues == NULL )
		return ;
	if ( pValues->pnext != NULL )
		dao_free_valuelist(pValues->pnext);
	if ( pValues->valuetype == VALUE_STRING )
		free(pValues->value.str);
	free(pValues);
}

void dao_free_param(struct _t_param* pParam)
{
	if ( pParam == NULL )
		return ;
	if ( pParam->values != NULL )
		dao_free_valuelist(pParam->values);
	free(pParam->name);
	free(pParam);
}

void dao_free_orderlist(struct _t_orderlist* pOrderList)
{
	if ( pOrderList == NULL )
		return ;
	dao_free_orderfield(pOrderList->fields);
	free(pOrderList);
}

void dao_free_orderfield(struct _t_orderfield* pOrderField)
{
	if ( pOrderField == NULL )
		return ;
	if ( pOrderField->next != NULL )
		dao_free_orderfield(pOrderField->next);
	free(pOrderField);
}

void dao_free_expression(struct _t_expression* pExpression)
{
	if ( pExpression == NULL )
		return ;
	dao_free_param(pExpression->left);
	if ( pExpression->right != NULL )
		dao_free_expression(pExpression->right);
	free(pExpression);
}

void dao_free_statement(struct _t_statement* pStatement)
{
	if ( pStatement == NULL )
		return ;
	dao_free_expression(pStatement->expressions);
	dao_free_orderlist(pStatement->porder);
	free(pStatement);
}

struct _t_statement* dao_new_statement()
{
	return (struct _t_statement*) malloc(sizeof(struct _t_statement));
	throwException("yyparse -- memory not enought");
	return NULL;
}

struct _t_expression* dao_new_expression()
{
	return (struct _t_expression*) malloc(sizeof(struct _t_expression));
	throwException("yyparse -- memory not enought");
	return NULL;
}

struct _t_orderlist* dao_new_orderlist()
{
	return (struct _t_orderlist*) malloc(sizeof(struct _t_orderlist));
	throwException("yyparse -- memory not enought");
	return NULL;
}

struct _t_orderfield* dao_new_orderfield()
{
	return (struct _t_orderfield*) malloc(sizeof(struct _t_orderfield));
	throwException("yyparse -- memory not enought");
	return NULL;
}

struct _t_param* dao_new_param()
{
	return (struct _t_param*) malloc(sizeof(struct _t_param));
	throwException("yyparse -- memory not enought");
	return NULL;
}
struct _t_valuelist* dao_new_valuelist()
{
	return (struct _t_valuelist*) malloc(sizeof(struct _t_valuelist));
	throwException("yyparse -- memory not enought");
	return NULL;
}
