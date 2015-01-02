#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <ctype.h>
#include <stddef.h>
#include <exception>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

#include "dir.h"
#include "codebuilder.h"
#include "genglobal.h"

#ifdef __cplusplus
extern "C"{
#endif

pTList endlist = NULL;
typeList listHead;
TToken tokens[TOKEN_SIZE];
static FILE *pfin = NULL;
extern "C"{
size_t tokenCount = 0;
int linenum = 1;
int fileType;
int classType;
char errfile[256] = {0};
}

static char baseclass[64] = {0};
static char extendclass[64] = {0};
extern "C" FILE *yyin;

int fileStatus = MAKEFILE;

//==================public==================================
char pwd[256] = {0};

std::list<std::string> g_lstPrecompile;
std::vector<std::string> g_importVec;
std::list<ICodeGen*> g_lstCodeGen;

void errorExit(char *errorStr, ...);
const char* transform(const char *keyword, typerecord *pTable)
{
	for (int i = 0; pTable[i].srctype != NULL; i++)
	{
		if (strcmp(keyword, pTable[i].srctype) == 0)
		{
			return pTable[i].destype;
		}
	}

	errorExit("Type cover error : %s.\n", keyword);
	return "";
}

//文件已存在，是否覆盖
void coverOrNot(const char *filename)
{
	//char tag = 'n', tmpchar;
	//if(DIR_INEXISTENCE != CDir::GetNodeType(filename))
	//{
	//	do
	//	{
	//		printf("File '%s' exist. Cover it anyway?(y or n)", filename);
	//		tag = getchar();
	//		while((tmpchar = getchar()) != '\n' && tmpchar != EOF);
	//		if(tag == 'y' || tag == 'Y')
	//			break;
	//		else if(tag == 'n' || tag == 'N')
	//			exit(1);
	//		printf("Please answer y or n.\n");
	//	}while(1);
	//}
}
//==================end of public===========================
//===============typelist function==========================
void initList()
{
    listHead.count = 0;
    listHead.pPair = NULL;
    listHead.next = NULL;
    endlist = &listHead;
}

pTList findType(const char *typeName)
{
    pTList plist = listHead.next;
    while(plist)
    {
        if(!strcmp(plist->typeName, typeName))
            return plist;
        plist = plist->next;
    }
    return NULL;
}

void addType(const char *classname)
{
	size_t i = 0;
    if(findType(classname))
	{
		//重复定义
		if(fileStatus == MAKEFILE)
        	errorExit("addType|Error in line : %d : Type '%s' exist.\n",linenum,classname);
		//重复包含
		else
		{
			tokenCount = 0;
			return ;
		}
	}
    
    printf("addtype -- %s\n",classname);
    ++listHead.count;
    endlist->next = (pTList)malloc(sizeof(typeList));
    endlist = endlist->next;
    strcpy(endlist->typeName, classname);
	strncpy(endlist->baseClass, baseclass, 64);
	strncpy(endlist->extendClass, extendclass, 64);
    endlist->count = tokenCount;
    endlist->next = NULL;
    endlist->pPair = (pToken)malloc(tokenCount * sizeof(TToken));

    for(i = 0; i < tokenCount; ++i)
    {
        strcpy(endlist->pPair[i].keyword, tokens[i].keyword);
        strcpy(endlist->pPair[i].identifier, tokens[i].identifier);
        endlist->pPair[i].identifierType = tokens[i].identifierType;
    }
}

void addPrecompile(const char *precompile)
{
	g_lstPrecompile.push_back(string(precompile));
}

void addImport(const char *import)
{
	g_importVec.push_back(string(import));
}

void release()
{
	if(pfin != NULL)
	{
		fclose(pfin);
		pfin = NULL;
	}
	linenum = 1;
	tokenCount = 0;
	memset(pwd, 0, sizeof(pwd));
    pTList delList = listHead.next;
    while(delList)
    {
        endlist = delList->next;
        free(delList->pPair);
        free(delList);
        delList = endlist;
    }
	listHead.next = NULL;
	endlist = NULL;
	for (std::list<ICodeGen*>::iterator iter = g_lstCodeGen.begin(); iter != g_lstCodeGen.end(); iter++)
	{
		(*iter)->release();
	}
	g_lstCodeGen.clear();
}
//=============================================================================

//===========public function==========================================
void errorExit(char *errorStr, ...)
{
    char errorMsg[256] = {0};
    if(pfin != NULL)
	{
        fclose(pfin);
		pfin = NULL;
	}
	
	for (std::list<ICodeGen*>::iterator iter = g_lstCodeGen.begin(); iter != g_lstCodeGen.end(); iter++)
	{
		(*iter)->releaseError();
	}
	g_lstCodeGen.clear();
	//remove("codebuilder.ini");

    va_list args;
    va_start(args, errorStr);
    vsprintf(errorMsg, errorStr, args);
    va_end(args);

	if(strlen(errfile))
    	fprintf(stderr, "%s:%s",errfile, errorMsg);
	else
		fprintf(stderr, "%s", errorMsg);

	yyexit();
    exit(1);
}

void getNewLine()
{
    ++linenum;
}
//============================================================================

//===========syntax function=========================================
int isRepeat(const char *identifier)
{
    size_t i = 0;
    for(; i < tokenCount; ++i)
    {
        if(!strcmp(identifier, tokens[i].identifier))
            return 1;
    }
    return 0;
}

void setBaseClass(const char *pBaseclass)
{
	strcpy(baseclass, pBaseclass);
}

void setExtendClass(const char *pExtendclass)
{
	strcpy(extendclass, pExtendclass);
}

void addKIPair(char *keyword, char *identifier, int type)
{
    pTList plist = NULL;
    size_t i = 0; 

    if(isRepeat(identifier))
        errorExit("addKIPair|Error in line : %d : Redefine Identifier '%s'\n", linenum, identifier);

    if(tokenCount >= TOKEN_SIZE )
        errorExit("addKIPair|Error : tokenCount >= TOKEN_SIZE");

	//expand the inline
    if(strcmp(keyword, "inline") == 0)
    {
		if((plist = findType(identifier)) == NULL)
			errorExit("addKIPair|Error in line : %d : Unkown Type '%s'\n", linenum, identifier);
		printf("%s--%s\n",keyword,identifier);
		for(i = 0; i < plist->count; ++i)
			addKIPair(plist->pPair[i].keyword, plist->pPair[i].identifier, plist->pPair[i].identifierType);
        
    }
    else
    {
		if((type == POINTER || type == CLASSPOINTER) &&
			(!tokenCount || (strcmp(tokens[tokenCount-1].keyword,"uint16") && strcmp(tokens[tokenCount-1].keyword, "size_t"))))
			errorExit("addKIPair|Error in line : %d : Lack size.\n", linenum);

		strncpy(tokens[tokenCount].keyword, keyword, KEYWORDLEN);
		strncpy(tokens[tokenCount].identifier, identifier, IDENTIFIERLEN);
		tokens[tokenCount].identifierType = type;
		++tokenCount;
    }
}

//==============================Enum===============================
void writeEnumHead(const char *enumName)
{
	for (std::list<ICodeGen*>::iterator iter = g_lstCodeGen.begin(); iter != g_lstCodeGen.end(); iter++)
	{
		(*iter)->writeEnumHead(enumName);
	}
}

void writeEnumIV(const char *identifier, const char *value)
{
	for (std::list<ICodeGen*>::iterator iter = g_lstCodeGen.begin(); iter != g_lstCodeGen.end(); iter++)
	{
		(*iter)->writeEnumIV(identifier, value);
	}
}

void writeEnumIVI(const char *identifier, int value)
{
	for (std::list<ICodeGen*>::iterator iter = g_lstCodeGen.begin(); iter != g_lstCodeGen.end(); iter++)
	{
		(*iter)->writeEnumIVI(identifier, value);
	}
}

void writeEnumEnd()
{
	for (std::list<ICodeGen*>::iterator iter = g_lstCodeGen.begin(); iter != g_lstCodeGen.end(); iter++)
	{
		(*iter)->writeEnumEnd();
	}
}
//=============================================================================

//==============writefile function===================================

void writeFile()
{
	for (std::list<ICodeGen*>::iterator iter = g_lstCodeGen.begin(); iter != g_lstCodeGen.end(); iter++)
	{
		(*iter)->generateFile();
	}
	tokenCount = 0;
}

//=============================================================================

void yyerror(char *s)
{
    errorExit("yyerror|Error in line : %d : %s\n", linenum, s);
}

void makeupFile(char *srcFile)
{
    if((pfin = fopen(srcFile, "r")) == NULL)
    {
        printf("makeupFile|cannot open srcfile : %s\n", srcFile);
        exit(1);
    }

	strcpy(errfile, srcFile);

	for (std::list<ICodeGen*>::iterator iter = g_lstCodeGen.begin(); iter != g_lstCodeGen.end(); iter++)
	{
		(*iter)->generateBegin();
	}

    yyin = pfin;
    initList();
	newfile(srcFile, MAKEFILE);
	fileStatus = MAKEFILE;
    yyparse();

	for (std::list<ICodeGen*>::iterator iter = g_lstCodeGen.begin(); iter != g_lstCodeGen.end(); iter++)
	{
		(*iter)->generateEnd();
	}

	//release source
    release();
}

void makeup(const char *program, char *srcPath, char *desPath, char *asPath)
{
	CDir dir;
	char *phfile = NULL;
	char *pasfile = NULL;

	switch(CDir::GetNodeType(srcPath))
	{
	case DIR_DIRECTORY :
		{
			if(desPath != NULL && DIR_INEXISTENCE == CDir::GetNodeType(desPath))
			{
				CDir::MakeDirectory(desPath);
			}
			if(asPath != NULL && DIR_INEXISTENCE == CDir::GetNodeType(asPath))
			{
				CDir::MakeDirectory(asPath);
			}

			if(!dir.OpenDirectory(srcPath))
			{
				errorExit("makeup|Directory '%s' not exist.\n", srcPath);
				exit(1);
			}

			//保存当前解析目录
			memset(pwd, 0, sizeof(pwd));
			strcpy(pwd, srcPath);
			char seperator = '/';
			if(pwd[strlen(pwd) - 1] != seperator)
				pwd[strlen(pwd)] = seperator;

			string filename;
			char infilebuf[256] = {0};
			char outfilebuf[256] = {0};
			char asfilebuf[256] = {0};
			char *position = NULL;
			int location = 0;
			CFileFilter filter("*.idl");
			CFileFilter filterdir(".*");

			while(dir.HasNextFile())
			{
				filename = dir.NextFile();
				if((dir.isFile() && !filter.isMatch(filename)) || filterdir.isMatch(filename) || filename == "." || filename == "..")
					continue;

				phfile = NULL;
				pasfile = NULL;
				memset(infilebuf, 0, sizeof(infilebuf));
				memset(outfilebuf, 0, sizeof(outfilebuf));
				memset(asfilebuf, 0, sizeof(asfilebuf));

				strcpy(infilebuf, srcPath);
				infilebuf[strlen(infilebuf)] = seperator;
				strcat(infilebuf, filename.c_str());
				if(desPath != NULL)
				{
					strcpy(outfilebuf, desPath);
					outfilebuf[strlen(outfilebuf)] = seperator;
					strcat(outfilebuf, filename.c_str());
					phfile = outfilebuf;
				}
				if (asPath != NULL)
				{
					strcpy(asfilebuf, asPath);
					asfilebuf[strlen(asfilebuf)] = seperator;
					strcat(asfilebuf, filename.c_str());
					pasfile = asfilebuf;
				}

				if(dir.isFile())
				{
					//strcpy(errfile, infilebuf);				
					if(phfile != NULL && (position = strrchr(phfile, '.')) != NULL)
					{
						*(position+1) = 'h';
						*(position+2) = '\0';
					}

					buildFork(program, infilebuf, phfile, pasfile);
				}
				else if(dir.isDirectory())
				{				
					makeup(program, infilebuf, phfile, pasfile);
				}
				else
				{
					printf("unexpected file : '%s'.\n", infilebuf);
					exit(1);
				}
				//还原当前解析目录
				memset(pwd, 0, sizeof(pwd));
				strcpy(pwd, srcPath);
				if(pwd[strlen(pwd) - 1] != seperator)
					pwd[strlen(pwd)] = seperator;
			}//end of while
		}
		break;
	case DIR_FILE :
		errorExit("makeup|srcPath must be directory.\n");
		break;
	case DIR_INEXISTENCE :
		errorExit("makeup|'%s' not exist.\n", srcPath);
		break;
	default :
		break;
	}//end of switch
}

void buildFork(const char *program, char *srcFile, char *desFile, char *asFile)
{
#ifdef WIN32
	
	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo;
	memset(&siStartInfo,0,sizeof(STARTUPINFO));

	// Set up members of STARTUPINFO structure.
	siStartInfo.cb = sizeof(STARTUPINFO); 
	siStartInfo.lpReserved = NULL;
	siStartInfo.lpReserved2 = NULL; 
	siStartInfo.cbReserved2 = 0;
	siStartInfo.lpDesktop = NULL; 
	siStartInfo.dwFlags = 0;

	//设置参数
	string  strCmdLine = program;
	strCmdLine += " -b ";
	strCmdLine += srcFile;
	strCmdLine += " ";
	if(desFile != NULL)
	{
		strCmdLine += "-s ";
		strCmdLine += desFile;
		strCmdLine += " ";
	}
	if(asFile != NULL)
	{
		strCmdLine += "-c ";
		strCmdLine += asFile;
		strCmdLine += " ";
	}
	strCmdLine += pwd;
	char cmd[1024];
	sprintf(cmd,"%s",strCmdLine.c_str());

	// Create the child process.
	CreateProcess(
	NULL,
	cmd,
	NULL, // process security attributes
	NULL, // primary thread security attributes
	0, // handles are inherited
	0, // creation flags
	NULL, // use parent's environment
	NULL, // use parent's current directory
	&siStartInfo, // STARTUPINFO pointer
	&piProcInfo); // receives PROCESS_INFORMATION

	// Wait for the processs to finish
	DWORD rc = WaitForSingleObject(
	piProcInfo.hProcess, // process handle
	INFINITE); 
	
#else
	pid_t pid;
	//获取程序工作路径
	char programPath [256] = {0};
	if( getcwd(programPath, 256) == 0)
	{
		printf("getcwd error.\n");
		exit(1);
	}
	strcat(programPath, ++program);
	//创建新的进程
	if((pid = vfork()) < 0)
	{
		printf("buidFork error.\n");
		exit(1);
	}
	else if(pid == 0)
	{
		if(desFile != NULL && asFile != NULL)
		{
			if( execl(programPath, program, "-b", srcFile, "-s", desFile, "-c", asFile, pwd, NULL) < 0 )
			{
				printf("execl '%s' error!\n", programPath);
				exit(1);
			}
		}
		else if(desFile != NULL)
		{
			if( execl(programPath, program, "-b", srcFile, "-s", desFile, pwd, NULL) < 0 )
			{
				printf("execl '%s' error!\n", programPath);
				exit(1);
			}
		}
		else if(asFile != NULL)
		{
			if( execl(programPath, program, "-b", srcFile, "-c", asFile, pwd, NULL) < 0 )
			{
				printf("execl '%s' error!\n", programPath);
				exit(1);
			}
		}
		else
		{
			printf("error!");
			exit(1);
		}
		
		
		//makeupFile(srcFile,desFile);
	}
	else
		wait(NULL);
#endif
}

#ifdef __cplusplus
}
#endif
