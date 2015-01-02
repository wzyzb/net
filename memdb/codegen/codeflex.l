%option noyywrap
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codebuilder.h"
#include "codebison.h"

#define WRITE_CPPFILE //if(WRITECPP == 1 && fileStatus == MAKEFILE)fprintf(pfout, "%s", yytext);
#define WRITE_ASFILE

//extern FILE *pfout;
extern FILE *asout;
extern int linenum;
int isNota = 0;
extern int fileStatus;
extern int isEnd;
extern char errfile[256];
extern char pwd[256];
extern int WRITECPP;
extern int WRITEAS;

typedef struct _t_bufstack
{
	struct _t_bufstack *prev; //pevious entry
	YY_BUFFER_STATE bs; //saved buffer
	int lineno; //savaed line number
	char *filename; //name of this file
	int fileStatus; //标识文件状态
	FILE *f;
}BufStack, *PBufStack;

PBufStack curbs = NULL;

static void dealHead(char *str)
{
	char *p = NULL;
	char *pfind = NULL;
	char tmpbuf[256] = {0};
	char pathbuf[256] = {0};
	char filebuf[256] = {0};
	FILE *tmpfile;
	
	strcpy(pathbuf, str);
	
	//如果 include 了 idl,将后缀改为 .h
	if((p = strstr(pathbuf, ".idl")) != NULL)
	{
		*(++p) = 'h';
		++p;
		if((pfind = strchr(pathbuf, '<')) != NULL)
			strcpy(p,">\n");
		else if((pfind = strchr(pathbuf, '\"')) != NULL)
			strcpy(p, "\"\n");
		else
			errorExit("flex::dealHead|Error in line : %d : \"include\" syntax error.\n",linenum);
		*(p+2) = '\0';
	}
	
	//保存预编译选项
	if(fileStatus == MAKEFILE)
	{
		addPrecompile(pathbuf);
		getNewLine();
	}

	//如果不是头文件预编译，则退出
	if((p = strstr(pathbuf, ".h")) == NULL)
	{
		return ;
	}
	

	//生成依赖关系
	if((p = strchr(pathbuf, '<')) == NULL)
		p = strchr(pathbuf, '\"');
	++p;
	pfind = strrchr(pathbuf, '.');
	strncpy(tmpbuf, p, pfind-p);
	strcat(tmpbuf, ".idl");
	strcpy(filebuf, pwd);
	strcat(filebuf, tmpbuf);
	if(newfile(filebuf, fileStatus) == 0)
	{
		if((tmpfile = fopen("codebuilder.ini", "r")) == NULL)
		{
			perror("flex::dealHead|codebuilder.ini");
			errorExit("flex::dealHead|Can't open %s\n", filebuf);
		}
		//读临时文件的include路径
		else
		{
			while(1)
			{
				memset(filebuf, 0, sizeof(filebuf));
				if(fgets(filebuf, sizeof(filebuf), tmpfile) == NULL)
				{
					errorExit("flex::dealHead|%s not found.\n", tmpbuf);
				}
				filebuf[strlen(filebuf) - 1] = 0;
				strcat(filebuf, tmpbuf);
				if(newfile(filebuf, fileStatus))
					break;
			}//end of while(true)
			fclose(tmpfile);
		}
	}//end of if
	fileStatus = MAKEDEPENDENCE;
}

int newfile(char *fn, int status)
{
	FILE *f = fopen(fn, "r");		
	PBufStack bs = (PBufStack)malloc(sizeof(BufStack));
	if(f == NULL)
		return 0;
	if(bs == NULL )
	{
		perror("malloc");
		return 0;
	}

	if(curbs)
		curbs->lineno = linenum;
	bs->prev = curbs;
	
	bs->bs = yy_create_buffer(f, YY_BUF_SIZE);
	bs->f = f;
	bs->filename = fn;

	//保存前一文件状态
	bs->fileStatus = status;
	yy_switch_to_buffer(bs->bs);
	curbs = bs;
	strcpy(errfile, fn);
	linenum = 1;
	return 1;
}

int popfile()
{
	PBufStack bs = curbs;
	PBufStack prevbs;
	
	if(!bs)
		return 0;

	//恢复前一文件状态
	fileStatus = bs->fileStatus;
	fclose(bs->f);
	yy_delete_buffer(bs->bs);

	curbs = prevbs = bs->prev;
	free(bs);

	if(!prevbs)
		return 0;
		
	yy_switch_to_buffer(prevbs->bs);
	linenum = curbs->lineno;
	strcpy(errfile, curbs->filename);
	return 1;
}

void yyexit()
{
	PBufStack bs = curbs;
	
	while(bs != NULL)
	{
		fclose(bs->f);
		yy_delete_buffer(bs->bs);
		curbs = bs->prev;
		free(bs);
		if(!curbs)
			break;
		yy_switch_to_buffer(curbs->bs);
		bs = curbs;
	}
}

%}

digit [0-9]
letter [_a-zA-Z]
space [ \t]
newline [\n]
identifier {letter}({letter}|{digit})*
keyword "char"|"size_t"|"std::string"|"string"|"int"|"uint8"|"uint16"|"uint32"|"uint64"|"int8"|"int16"|"int32"|"int64"
template "std::vector"|"std::list"|"vector"|"list"|"map"|"std::map"
inline "inline"
hexadecimal "0x".{digit}+
number [0]|([1-9]{digit}*)
precompile [#].*[\n]
import "import".*[\n]
wrongid {digit}+{identifier}
singlenota "//".*"\n"
beginnota [/][*]
endnota [*][/]
%%

{newline} {getNewLine();}

"typedef" {if(isNota){WRITE_CPPFILE; WRITE_ASFILE;}
    else{return TYPEDEF;}}

"struct" {if(isNota){WRITE_CPPFILE; WRITE_ASFILE;}
    else{return STRUCT;}}

"enum" {if(isNota){WRITE_CPPFILE; WRITE_ASFILE;}
	else{return ENUM;}}

"public" {if(isNota){WRITE_CPPFILE; WRITE_ASFILE;}
	else{return PUBLIC;}}

{keyword} {if(!isNota){strcpy(yylval.keyword, yytext);return KEYWORD;}
    else{WRITE_CPPFILE; WRITE_ASFILE;}}

{inline} {if(!isNota){strcpy(yylval.keyword, yytext);return INLINE;}
    else{WRITE_CPPFILE; WRITE_ASFILE;}}

{template} {if(!isNota){strcpy(yylval.keyword, yytext);return TEMPLATE;}
	else{WRITE_CPPFILE; WRITE_ASFILE;}}

{identifier} {if(!isNota){strcpy(yylval.identifier, yytext);return IDENTIFIER;}
    else{WRITE_CPPFILE; WRITE_ASFILE;}}

{number} {if(!isNota){yylval.size=atoi(yytext);return NUMBER;}
    else{WRITE_CPPFILE; WRITE_ASFILE;}}

{hexadecimal} {if(!isNota){strcpy(yylval.identifier, yytext);return HEXADECIMAL;}
	else{WRITE_CPPFILE; WRITE_ASFILE;}}

{precompile} {if(!isNota)dealHead(yytext);
	else{WRITE_CPPFILE; WRITE_ASFILE;};getNewLine();}

{import} {if(!isNota)addImport(yytext);
	else{WRITE_CPPFILE; WRITE_ASFILE;}getNewLine();}

"," {if(!isNota){return COMMAN;}    
    else{WRITE_CPPFILE; WRITE_ASFILE;}}

";" {if(!isNota){return SEMICOLON;}    
    else{WRITE_CPPFILE; WRITE_ASFILE;}}

"[" {if(!isNota){return LBRACKET;}    
    else{WRITE_CPPFILE; WRITE_ASFILE;}}

"]" {if(!isNota){return RBRACKET;}    
    else{WRITE_CPPFILE; WRITE_ASFILE;}}

"{" {if(!isNota){return LBRACE;}    
    else{WRITE_CPPFILE; WRITE_ASFILE;}}

"}" {if(!isNota){return RBRACE;}    
    else{WRITE_CPPFILE; WRITE_ASFILE;}}

"<" {if(!isNota){return LANGLEBRACKET;}    
    else{WRITE_CPPFILE; WRITE_ASFILE;}}

">" {if(!isNota){return RANGLEBRACKET;}    
    else{WRITE_CPPFILE; WRITE_ASFILE;}}

"*" {if(!isNota){return ASTERISK;}    
    else{WRITE_CPPFILE; WRITE_ASFILE;}}

":" {if(!isNota){return COLON;}
	else{WRITE_CPPFILE; WRITE_ASFILE;}}

"=" {if(!isNota){return EQUAL;}
	else{WRITE_CPPFILE; WRITE_ASFILE;}}

{space}* {if(isNota){WRITE_CPPFILE; WRITE_ASFILE;}}

{wrongid} {if(!isNota){fprintf(stderr, "Error in line : %d : WrongID '%s'\n", linenum, yytext);}
    else{WRITE_CPPFILE; WRITE_ASFILE;}}

{singlenota} {WRITE_CPPFILE; WRITE_ASFILE; getNewLine();}

{beginnota} {isNota = 1; WRITE_CPPFILE; WRITE_ASFILE;}

{endnota} {isNota = 0; WRITE_CPPFILE; WRITE_ASFILE;}

. {if(isNota){WRITE_CPPFILE; WRITE_ASFILE;}}

<<EOF>> {if(!popfile())yyterminate();}

%%

