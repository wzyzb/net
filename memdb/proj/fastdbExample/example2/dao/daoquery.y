%{
#include <stdlib.h>
#include <stdio.h>
#include <dao/daostruct.h>

extern int yylex();
statement* g_pStmt;


void yyerror(const char * str)
{
	throwException("yyerror [%s]\n",str);
}

%}

%union
{
	void* pointer;
	expression* pexp;
	statement* pstmt;
	valuelist* pval;
	int enumeration;
	int digit;
	char* str;
}

%type <pointer> order identifiers order_identifier
%type <pexp> expression
%type <pstmt> statement
%type <pval> values value
%type<enumeration> join
%token<str> IDENTIFIER
%token<digit> NUMBER
%token<enumeration> OPIN
%token<enumeration> AND
%token<enumeration> OR
%token<enumeration> OPBETWEEN
%token<enumeration> OPSGL
%token<enumeration> ORDER
%token<enumeration> ORDERBY
%token<enumeration> DESC
%token<enumeration> ASC
%token<str> STRING

%%
start : statement 
{
	g_pStmt = $1;
}
| statement order
{
	$1->porder = $2;
	g_pStmt = $1;
}
| order
{
	g_pStmt = dao_new_statement();
	g_pStmt->porder = $1;
	g_pStmt->expressions = NULL;
}
;

order : ORDERBY identifiers
{
	struct _t_orderlist* p = dao_new_orderlist();
	p->fields = $2;
	$$ = p;
}
;

identifiers : order_identifier
{
	$$=$1;
}
| order_identifier ',' identifiers
{
	orderfield* p = $1;
	p->next = $3;
	$$ = p;
}
;

order_identifier : IDENTIFIER
{
	orderfield* p = dao_new_orderfield();
	p->str = $1;
	p->ordermode = ASC;
	p->next = NULL;
	$$ = p;
}
| IDENTIFIER ORDER
{
	orderfield* p = dao_new_orderfield();
	p->str = $1;
	p->ordermode = $2;
	p->next = NULL;
	$$ = p;
}
;

statement : expression 
{
	statement* p = dao_new_statement();
	p->expressions = $1;
	p->porder = NULL;
	$$=p;
}
| expression join statement
{
	$1->right = $3->expressions;
	$3->expressions = $1;
	$1->joinmode = $2;
	$$=$3;
}
| ''
{
	$$ = NULL;
}
;

expression : IDENTIFIER  OPSGL value
{
	$$ = dao_new_expression();
	$$->left = dao_new_param();
	$$->left->name = $1;
	$$->left->paramopermode = $2;
	$$->left->values = $3;
	$$->right = NULL;
	$$->joinmode = 0;
}
| IDENTIFIER OPIN '(' values ')'
{
	$$ = dao_new_expression();
	$$->left = dao_new_param();
	$$->left->name = $1;
	$$->left->paramopermode = $2;
	$$->left->values = $4;
	$$->right = NULL;
	$$->joinmode = 0;
}
| IDENTIFIER OPBETWEEN value AND value
{
	$$ = dao_new_expression();
	$$->left = dao_new_param();
	$$->left->name = $1;
	$$->left->paramopermode = $2;
	$3->pnext = $5;
	$$->left->values = $3;
	$$->right = NULL;
	$$->joinmode = 0;
}
;

values : value 
{
	$$=$1;
}
| value ',' values
{
	$1->pnext = $3;
	$$=$1;
}
;

value : '?'
{
	struct _t_valuelist* p = dao_new_valuelist();
	p->valuetype=VALUE_PARAM;
	p->pnext = NULL;
	$$=p;
}
| NUMBER
{
	struct _t_valuelist* p = dao_new_valuelist();
	p->valuetype=VALUE_NUMBER;
	p->value.num=$1;
	p->pnext = NULL;
	$$=p;
}
| STRING
{
	struct _t_valuelist* p = dao_new_valuelist();
	p->valuetype=VALUE_STRING;
	p->value.str=$1;
	p->pnext = NULL;
	$$=p;
}
;

join : AND
{
	$$=JOIN_AND;
}
| OR
{
	$$=JOIN_OR;
}
;
%%



