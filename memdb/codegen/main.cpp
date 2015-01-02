#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <vector>
#include <ctype.h>
#include <stddef.h>
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
#include "cppgen.h"
#include "asgen.h"
#include "PhpCodeGen.h"

#ifdef __cplusplus
extern "C"{
#endif

int usage(int argc,char** argv)
{
	printf("%s Usage : [indir -s outdir -c outdir] | [infile -s -c]\n",argv[0]);
	return 1;
}

//将 -I 选项保持到临时文件 codebuilder.ini
void savePath(int argc, char *argv[])
{
	char seperator = '/';
	FILE *tmpfile;
	if((tmpfile = fopen("codebuilder.ini", "w")) == NULL)
	{
		perror("savePath|codebuilder.ini");
		exit(1);
	}

	//save "-I" option
	for(int i = 0; i < argc; ++i)
	{
		if(argv[i][0] == '-' && argv[i][1] == 'I')
		{
			if(argv[i][strlen(argv[i]) - 1] != seperator)
				fprintf(tmpfile, "%s%c\n", &argv[i][2], seperator);
			else
				fprintf(tmpfile, "%s\n", &argv[i][2]);
		}
	}  
	fclose(tmpfile);
}

int main(int argc, char *argv[])
{
	char *phfile = NULL;
	char *pasfile = NULL;

	if ( argc < 3 )
		return usage(argc,argv);

	//fork 进程
	if(strcmp(argv[1], "-b") == 0)
	{
		strcpy(pwd, argv[argc-1]);
		pwd[strlen(argv[argc-1])] = '\0';
		for(int i = 3; i < argc-2; i += 2)
		{
			if(strcmp(argv[i], "-s") == 0)
			{
				g_lstCodeGen.push_back(new CCPPGen());
				g_lstCodeGen.back()->initialize(argv[i+1]);
			}
			else if(strcmp(argv[i], "-c") == 0)
			{
				g_lstCodeGen.push_back(new CASGen());
				g_lstCodeGen.back()->initialize(argv[i+1]);
			}
			else if(strcmp(argv[i], "-p") == 0)
			{
				g_lstCodeGen.push_back(new PhpCodeGen());
				g_lstCodeGen.back()->initialize(argv[i+1]);
			}
			//else
			//	printf("%s\n", argv[i]);
		}

		makeupFile(argv[2]);
	}
	else
	{
		//保存 -I 选项
		savePath(argc, argv);

		switch(CDir::GetNodeType(argv[1]))
		{
		case DIR_FILE :
			{
				char *position = NULL;
				if(argc < 3)
					return usage(argc,argv);

				for(int i = 2; i < argc; ++i)
				{
					if(strcmp(argv[i], "-s") == 0)
					{
						char desfile[256] = {0};
						strcpy(desfile, argv[1]);
						if((position = strrchr(desfile, '.')) != NULL)
						{			
							*(position+1) = 'h';
							*(position+2) = '\0';					
						}
						g_lstCodeGen.push_back(new CCPPGen());
						g_lstCodeGen.back()->initialize(desfile);
					}
					//todo
					else if(strcmp(argv[i], "-c") == 0)
					{
						g_lstCodeGen.push_back(new CASGen());
						g_lstCodeGen.back()->initialize(argv[1]);
					}
				}

				makeupFile(argv[1]);
			}
			break;
		case DIR_DIRECTORY :
			{
				phfile = NULL;
				pasfile = NULL;
				for(int i = 2; i < argc-1; i += 2)
				{
					if(strcmp(argv[i], "-s") == 0)
					{
						phfile = argv[i+1];
					}
					else if(strcmp(argv[i], "-c") == 0)
					{
						pasfile = argv[i+1];
					}
					//else
					//	printf("%s\n", argv[i]);
				}
				makeup(argv[0], argv[1], phfile, pasfile);
			}
			break;
		case DIR_INEXISTENCE :
			errorExit("main|Error : '%s' inexistence.\n", argv[1]);
			break;
		default :
			break;
		}

		//remove("codebuilder.ini");
	}

	for (std::list<ICodeGen*>::iterator iter = g_lstCodeGen.begin(); iter != g_lstCodeGen.end(); iter++)
	{
		delete *iter;
	}
	g_lstCodeGen.clear();
	return 0;
}

#ifdef __cplusplus
}
#endif