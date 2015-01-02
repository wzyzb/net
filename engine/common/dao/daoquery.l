%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dao/daostruct.h>
#include "daoquery.y.h"

int yywrap() { return 1; }
int checklen(const char* pStr)
{
	const char* p = pStr;
	while ( *p != 0 )
		p ++;
	return p - pStr;
}

%}

digit [0-9]
letter [_a-zA-Z%]
space [ \t]
keyword "and"|"or"
identifier {letter}({letter}|{digit})*({space}*[\[]{space}*([1-9]{digit}*{space}*)?[\]])*
notation "?"
number {digit}+
%%
{space} /* ignore comment {return SPACE;}*/
"and" {yylval.enumeration=JOIN_AND;return AND;}
"or" {yylval.enumeration=JOIN_OR;return OR;}
{number} { yylval.digit = atoi(yytext); return NUMBER;}
"=" { yylval.enumeration=OPER_EQUAL;return OPSGL;}
"!="|"<>" { yylval.enumeration=OPER_NOEQL;return OPSGL;}
"<" { yylval.enumeration=OPER_LT;return OPSGL;}
"<=" { yylval.enumeration=OPER_LET;return OPSGL;}
">" { yylval.enumeration=OPER_BT;return OPSGL;}
">=" { yylval.enumeration=OPER_BET;return OPSGL;}
"like" { yylval.enumeration=OPER_LIKE;return OPSGL;}
"?" return '?';
"," return ',';
"order"{space}+"by" { return ORDERBY; }
"in" { yylval.enumeration=OPER_IN; return OPIN; }
"between" { yylval.enumeration=OPER_BETWEEN; return OPBETWEEN; }
"desc" { yylval.enumeration = ORDER_DESC;return ORDER; }
"asc" { yylval.enumeration = ORDER_ASC;return ORDER; }
["']{identifier}["'] { yylval.str=strdup(yytext+1); yylval.str[checklen(yylval.str)-1]=0;return STRING; }
{identifier} { yylval.str=strdup(yytext);return IDENTIFIER;}
"(" return '(';
")" return ')';

%%
YY_BUFFER_STATE g_state;
void set_scan_string(const char* str)
{
	g_state = yy_scan_string(str);
}

void free_scan_string()
{
	yy_delete_buffer(g_state);
}