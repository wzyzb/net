%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "codebuilder.h"

extern int linenum;
extern int fileStatus;
extern int tokenCount;
extern char errfile[256];

static char tmplateBuf[128] = {0};
%}

%union{
    int size;
    char keyword[64];
    char identifier[128];
	char enumline[256];
}

%token<size> NUMBER
%token<keyword> KEYWORD
%token<keyword> INLINE
%token<keyword> TEMPLATE
%token<identifier> IDENTIFIER
%token<identifier> HEXADECIMAL
%token<enumline> ENUMLINE

%token TYPEDEF
%token STRUCT
%token ENUM
%token PUBLIC
%token EXTENDS
%token LBRACE
%token RBRACE
%token LBRACKET
%token RBRACKET
%token LANGLEBRACKET
%token RANGLEBRACKET
%token COMMAN
%token SEMICOLON
%token ASTERISK
%token COLON
%token EQUAL

%start program

%%

program
    : 
    | struct
      program
	| enum
      program
    ;

struct
    : head
      tokenList
      over
    {
		if(fileStatus == MAKEFILE)
			writeFile();
		else
		{
			tokenCount = 0;
		}
	}
    ;

head
    : TYPEDEF STRUCT IDENTIFIER LBRACE
	{
		setBaseClass("ISerial");
		setExtendClass("BaseObj");
	}
	| TYPEDEF STRUCT IDENTIFIER COLON PUBLIC IDENTIFIER LBRACE
	{
		setBaseClass($6);
	}
	| TYPEDEF STRUCT IDENTIFIER COLON EXTENDS IDENTIFIER LBRACE
	{
		setExtendClass($6);
	}
	| TYPEDEF STRUCT IDENTIFIER COLON PUBLIC IDENTIFIER EXTENDS IDENTIFIER LBRACE
	{
		setBaseClass($6);
		setExtendClass($8);
	}
	| TYPEDEF STRUCT IDENTIFIER COLON EXTENDS IDENTIFIER PUBLIC IDENTIFIER LBRACE
	{
		setExtendClass($6);
		setBaseClass($8);
	}
    ;

over
    : RBRACE identifiers SEMICOLON
    ;

identifiers
    : IDENTIFIER
    {
		addType($1);
	}
    | ASTERISK IDENTIFIER
    {
		addType($2);
	}
    | identifiers COMMAN IDENTIFIER
    {
		addType($3);
	}
    | identifiers COMMAN ASTERISK IDENTIFIER
    {
		addType($4);
	}
    ;

tokenList
    :
    | kipair
      tokenList
    ;

kipair
    : KEYWORD IDENTIFIER SEMICOLON
    {
		addKIPair($1, $2, VARIABLE);
	}

    | KEYWORD IDENTIFIER LBRACKET RBRACKET SEMICOLON
    {
		addKIPair($1, $2, POINTER);
	}

    | KEYWORD IDENTIFIER LBRACKET NUMBER RBRACKET SEMICOLON
    {
		addKIPair($1, $2, $4);
	}

    | INLINE IDENTIFIER SEMICOLON
    {
		if(findType($2))
			addKIPair($1, $2, INLINE);
		else
		{
			errorExit("bison|Error in line : %d : Unkown Type '%s'\n", linenum, $1);
		}
	}

    | IDENTIFIER IDENTIFIER SEMICOLON
    {
		if(findType($1) || strcmp($1, "CBytesBuffer") == 0 || strcmp($1, "bytes") == 0)
			addKIPair($1, $2, CLASS);
		else
		{
			errorExit("bison|Error in line : %d : Unkown Type '%s'\n", linenum, $1);
		}
	}

    | IDENTIFIER IDENTIFIER LBRACKET RBRACKET SEMICOLON
    {
		if(findType($1))
			addKIPair($1, $2, CLASSPOINTER);
		else
		{
			errorExit("bison|Error in line : %d : Unkown Type '%s'\n", linenum, $1);
		}
	}

    | IDENTIFIER IDENTIFIER LBRACKET NUMBER RBRACKET SEMICOLON
    {
		if(findType($1))
			addKIPair($1, $2, $4);
		else
		{
			errorExit("bison|Error in line : %d : Unkown Type '%s'\n", linenum, $1);
		}
	}

	| TEMPLATE LANGLEBRACKET KEYWORD RANGLEBRACKET IDENTIFIER SEMICOLON
	{
		memset(tmplateBuf, 0, sizeof(tmplateBuf));
		sprintf(tmplateBuf, "%s<%s>", $1, $3);
		addKIPair(tmplateBuf, $5, CONTAINER);
	}

	| TEMPLATE LANGLEBRACKET IDENTIFIER RANGLEBRACKET IDENTIFIER SEMICOLON
	{
		if(findType($3))
		{
			memset(tmplateBuf, 0, sizeof(tmplateBuf));
			sprintf(tmplateBuf, "%s<%s>", $1, $3);
			addKIPair(tmplateBuf, $5, CLASSCONTAINER);
		}
		else
		{
			errorExit("bision|Error in line : %d : Unkown Type '%s'\n", linenum, $3);
		}
	}
    ;

enum
	: enumhead
	  enumList
	  RBRACE SEMICOLON
	{
		if(fileStatus == MAKEFILE)
			writeEnumEnd();
	}
	;

enumhead
	: ENUM IDENTIFIER LBRACE
	{
		if(fileStatus == MAKEFILE)
			writeEnumHead($2);
	}
	;

enumList
	:
	| enumkv
	  enumList
	;

enumkv
	: IDENTIFIER EQUAL HEXADECIMAL COMMAN
	{
		if(fileStatus == MAKEFILE)
			writeEnumIV($1, $3);
	}
    | IDENTIFIER EQUAL NUMBER COMMAN
	{
		if(fileStatus == MAKEFILE)
			writeEnumIVI($1, $3);
	}
	| IDENTIFIER COMMAN
	{
		if(fileStatus == MAKEFILE)
			writeEnumIV($1, NULL);
	}
	;

%%

