#ifndef _CODEBUILDER_H_
#define _CODEBUILDER_H_

#define VARIABLE -1
#define POINTER -2
#define ARRAY -3
#define CLASS -4
#define CLASSPOINTER -5
#define CONTAINER -6
#define CLASSCONTAINER -7
#define MYINLINE -8

#define TOKEN_SIZE 1000
#define KEYWORDLEN 64
#define IDENTIFIERLEN 128

#define MAKEFILE 0
#define MAKEDEPENDENCE 1

#define REQUESTPACK 0
#define RESPONEPACK 1

#ifdef __cplusplus
extern "C"{
#endif

typedef struct _t_token
{
    char keyword[KEYWORDLEN];
    char identifier[IDENTIFIERLEN];
    int identifierType;
}TToken, *pToken;

typedef struct _t_typelist
{
    size_t count;
    char typeName[64];
	char baseClass[64];
	char extendClass[64];
    pToken pPair;
    struct _t_typelist *next;
}typeList, *pTList;

extern int fileStatus;
extern char pwd[256];

//Óï·¨Ê÷
extern typeList listHead;
extern pTList endlist;
extern TToken tokens[TOKEN_SIZE];
extern size_t tokenCount;

//Çå³ý flex
void yyexit();

void errorExit(char *errorStr, ...);

void release();

void getNewLine();

void initList();

pTList findType(const char *typeName);

void addType(const char *classname);

void addPrecompile(const char *precompile);

void addImport(const char *import);

void releaseList();

int isRepeat(const char *identifier);

void setBaseClass(const char *baseclass);

void setExtendClass(const char *extendclass);

void addKIPair(char *keyword, char *identifier, int type);

void writeFile();

void writeEnumHead(const char *enumName);

void writeEnumIV(const char *identifier, const char *value);

void writeEnumIVI(const char *identifier, int value);

void writeEnumEnd();

void makeupFile(char *srcFile);

void makeup(const char *program, char *srcPath, char *desPath, char *asPath);

int newfile(char *fn, int status);

int popfile();

void buildFork(const char *program, char *srcFile, char *desFile, char *asFile);

//flex and bison
int yyflex(void);
int yyparse(void);
void yyerror(char*);

#ifdef __cplusplus
}
#endif

#endif

