#include <stdlib.h>
#include <string.h>
#include <list>
#include <string>

#include "genglobal.h"
#include "codebuilder.h"
#include "cppgen.h"

typerecord g_CppTypeTable[] = 
{
	{"uint64", "uint64"},
	{"uint32", "uint32"},
	{"uint16", "uint16"},
	{"size_t", "uint16"},
	{"uint8", "uint8"},
	{"char", "uint8"},
	{"int64", "int64"},
	{"int32", "int32"},
	{"int16", "int16"},
	{"int8", "int8"},
	{"bytes", "CBytesBuffer"},
	{"bool", "bool"},
	{"std::string", "std::string"},
	{"string", "string"},
	{NULL, NULL}
};

typerecord g_CppSerialFuncTable[] =
{
	{"uint64", "writeuint64"},
	{"uint32", "writeuint32"},
	{"uint16", "writeuint16"},
	{"size_t", "writesize"},
	{"uint8", "writeuint8"},
	{"char", "writeuint8"},
	{"int64", "writeint64"},
	{"int32", "writeint32"},
	{"int16", "writeint16"},
	{"int8", "writeint8"},
	{"string", "writestring"},
	{"std::string", "writestring"},
	{NULL, NULL}
};

typerecord g_CppUnserialFuncTable[] =
{
	{"uint64", "readuint64"},
	{"uint32", "readuint32"},
	{"uint16", "readuint16"},
	{"size_t", "readsize"},
	{"uint8", "readuint8"},
	{"char", "readuint8"},
	{"int64", "readint64"},
	{"int32", "readint32"},
	{"int16", "readint16"},
	{"int8", "readint8"},
	{"string", "readstring"},
	{"std::string", "readstring"},
	{NULL, NULL}
};

CCPPGen::CCPPGen()
{
	m_pDesFile = NULL;
	m_Seperator = '/';
}

CCPPGen::~CCPPGen()
{
	release();
}

void CCPPGen::initialize(const char *desFile)
{
	char desfile[256] = {0};
	strcpy(desfile, desFile);
	for (size_t i = 0; i < strlen(desfile); i++)
	{
		if(desfile[i] == '\\')
			desfile[i] = m_Seperator;
	}
	m_strDesFile = desfile;

	if((m_pDesFile = fopen(m_strDesFile.c_str(), "w")) == NULL)
	{
		printf("initCpp|cannot open cpp desfile : %s\n", m_strDesFile.c_str());
		exit(1);
	}

	std::string strmarco = getMarco();
	fprintf(m_pDesFile, "#ifndef %s\n", strmarco.c_str());
	fprintf(m_pDesFile, "#define %s\n\n", strmarco.c_str());
	fprintf(m_pDesFile, "#include <public.h>\n");
	fprintf(m_pDesFile, "#include <framework/serial.h>\n");
	fprintf(m_pDesFile, "#include <framework/request.h>\n");
	fprintf(m_pDesFile, "#include <framework/response.h>\n");
	fprintf(m_pDesFile, "#include <framework/commonptl.h>\n\n");	
}

void CCPPGen::release()
{
	if(m_pDesFile)
		fclose(m_pDesFile);
	g_lstPrecompile.clear();
}

void CCPPGen::releaseError()
{
	release();
	remove(m_strDesFile.c_str());
}

void CCPPGen::writeVariable()
{
	for(size_t i = 0; i < endlist->count; ++i)
	{
		switch(endlist->pPair[i].identifierType)
		{
		case VARIABLE :
			fprintf(m_pDesFile, "\t%s %s;\n", transform(endlist->pPair[i].keyword, g_CppTypeTable), endlist->pPair[i].identifier);
			break;

		case CLASS :
			if(strcmp(tokens[i].keyword, "bytes") == 0 || strcmp(tokens[i].keyword, "CBytesBuffer") == 0)
			{
				memset(tokens[i].keyword, 0, sizeof(tokens[i].keyword));
				strcpy(tokens[i].keyword, "CBytesBuffer");
				fprintf(m_pDesFile, "\tCBytesBuffer %s;\n", tokens[i].identifier);
			}
			else
				fprintf(m_pDesFile, "\t%s %s;\n", tokens[i].keyword, tokens[i].identifier);
			break;

		case POINTER :
		case CLASSPOINTER :
			fprintf(m_pDesFile, "\t%s *%s;\n", tokens[i].keyword, tokens[i].identifier);    
			break;

		case CONTAINER :
		case CLASSCONTAINER :
			fprintf(m_pDesFile, "\t%s %s;\n", tokens[i].keyword, tokens[i].identifier);
			break;

		default :
			//fprintf(m_pDesFile, "\t%s %s[%d];\n", tokens[i].keyword, tokens[i].identifier, tokens[i].identifierType);
			errorExit("Unexpected identifier.\n");
			break;
		}//end of switch
	}//end of for
	fprintf(m_pDesFile, "\n");
}

void CCPPGen::serialFunc()
{
	fprintf(m_pDesFile, "\tvirtual bool serial(char *pOut, int &Length){return true;}\n");

	fprintf(m_pDesFile, "\tvirtual bool serial(IOutputStream& Output)\n\t{\n");
	for(size_t i = 0; i < endlist->count; ++i)
	{
		switch(tokens[i].identifierType)
		{
		case VARIABLE :
			fprintf(m_pDesFile, "\t\t%s(Output, %s);\n", transform(endlist->pPair[i].keyword, g_CppSerialFuncTable),
				endlist->pPair[i].identifier);
			break;

		case CLASS :
			if(strcmp(tokens[i].keyword, "CBytesBuffer") == 0)
			{
				fprintf(m_pDesFile, "\t\twritesize(Output, %s.getDataSize());\n", tokens[i].identifier);
				fprintf(m_pDesFile, "\t\tuint32 len%d = %s.getDataSize();\n", i, tokens[i].identifier);
				fprintf(m_pDesFile, "\t\tOutput.writeBytes(%s.c_str(), len%d);\n", tokens[i].identifier, i);
			}
			else
				fprintf(m_pDesFile, "\t\t%s.serial(Output);\n", tokens[i].identifier);
			break;

		case POINTER :
			fprintf(m_pDesFile,"\t\tfor(uint32 i = 0; i < %s; ++i)\n\t\t{\n",tokens[i-1].identifier);
			fprintf(m_pDesFile, "\t\t\t%s(Output, %s[i]);\n", transform(endlist->pPair[i].keyword, g_CppSerialFuncTable),
				endlist->pPair[i].identifier);
			fprintf(m_pDesFile, "\t\t}\n");
			break;

		case CLASSPOINTER :
			fprintf(m_pDesFile,"\t\tfor(uint32 i = 0; i < %s; ++i)\n\t\t{\n",tokens[i-1].identifier);
			fprintf(m_pDesFile,"\t\t\t%s[i].serial(Output);\n", tokens[i].identifier);
			fprintf(m_pDesFile, "\t\t}\n");
			break;

		case CONTAINER :
			{
				char typeBuf[64] = {0};
				char *pLBrack = NULL;
				char *pRBrack = NULL;
				pLBrack = strchr(tokens[i].keyword, '<');
				pRBrack = strchr(tokens[i].keyword, '>');
				++pLBrack;
				strncpy(typeBuf, pLBrack, pRBrack-pLBrack);
				fprintf(m_pDesFile, "\t\twritesize(Output, %s.size());\n", tokens[i].identifier);
				fprintf(m_pDesFile, "\t\t%s::iterator iter%d = %s.begin();\n", tokens[i].keyword, i, tokens[i].identifier);
				fprintf(m_pDesFile, "\t\tfor(; iter%d != %s.end(); ++iter%d)\n\t\t{\n", i, tokens[i].identifier, i);
				fprintf(m_pDesFile, "\t\t\t%s(Output, *iter%d);\n", transform(typeBuf, g_CppSerialFuncTable), i);
				fprintf(m_pDesFile, "\t\t}\n");
				//fprintf(m_pDesFile, "\t\t%s.clear();\n", tokens[i].identifier);
			}
			break;

		case CLASSCONTAINER :
			fprintf(m_pDesFile, "\t\twritesize(Output, %s.size());\n", tokens[i].identifier);
			fprintf(m_pDesFile, "\t\t%s::iterator iter_t%d;\n", tokens[i].keyword, i);
			fprintf(m_pDesFile, "\t\tfor(iter_t%d = %s.begin(); iter_t%d != %s.end(); ++iter_t%d)\n\t\t{\n",
				i, tokens[i].identifier, i, tokens[i].identifier, i);
			fprintf(m_pDesFile, "\t\t\titer_t%d->serial(Output);\n\t\t}\n", i);
			break;

		default:
			//fprintf(m_pDesFile, "\t\twritebytes(Output, (char*)%s, sizeof(%s));\n", tokens[i].identifier, tokens[i].identifier);
			errorExit("Unexpected identifier.\n");
			break;
		}//end of switch
	}//end of for

	fprintf(m_pDesFile, "\t\treturn true;\n\t}\n");
}

void CCPPGen::unserialFunc()
{
	fprintf(m_pDesFile, "\tvirtual bool unserial(const char *pIn, int Length){return true;}\n");

	fprintf(m_pDesFile, "\tvirtual bool unserial(IInputStream& Input)\n\t{\n");

	for(size_t i = 0; i < endlist->count; ++i)
	{
		switch(tokens[i].identifierType)
		{
		case VARIABLE :
			fprintf(m_pDesFile, "\t\t%s(Input, %s);\n", transform(endlist->pPair[i].keyword, g_CppUnserialFuncTable),
				endlist->pPair[i].identifier);
			break;

		case CLASS :
			if(strcmp(tokens[i].keyword, "CBytesBuffer") == 0)
			{
				fprintf(m_pDesFile, "\t\tuint32 size%d = 0;\n", i);
				fprintf(m_pDesFile, "\t\treadsize(Input, size%d);\n", i);
				fprintf(m_pDesFile, "\t\t%s.writeBytes(Input.c_str(), size%d);\n", tokens[i].identifier, i);
				fprintf(m_pDesFile, "\t\tInput.popBytes(size%d);\n", i);
			}
			else
				fprintf(m_pDesFile, "\t\t%s.unserial(Input);\n", tokens[i].identifier);
			break;

		case POINTER :
			fprintf(m_pDesFile, "\t\tif(%s > 0)\n\t\t{\n", tokens[i-1].identifier);
			fprintf(m_pDesFile, "\t\t\tif(%s != NULL)\n", tokens[i].identifier);
			fprintf(m_pDesFile, "\t\t\t\tdelete[] %s;\n", tokens[i].identifier);
			fprintf(m_pDesFile, "\t\t\t%s = new %s[%s];\n", tokens[i].identifier, tokens[i].keyword, tokens[i-1].identifier);
			fprintf(m_pDesFile,"\t\t\tfor(uint32 i = 0; i < %s; ++i)\n\t\t\t{\n",tokens[i-1].identifier);
			fprintf(m_pDesFile, "\t\t\t\t%s(Input, %s[i]);\n", transform(endlist->pPair[i].keyword, g_CppUnserialFuncTable),
				endlist->pPair[i].identifier);
			fprintf(m_pDesFile, "\t\t\t}\n\t\t}\n");
			break;

		case CLASSPOINTER :
			fprintf(m_pDesFile, "\t\tif(%s > 0)\n\t\t{\n", tokens[i-1].identifier);
			fprintf(m_pDesFile, "\t\t\tif(%s != NULL)\n", tokens[i].identifier);
			fprintf(m_pDesFile, "\t\t\t\tdelete[] %s;\n", tokens[i].identifier);
			fprintf(m_pDesFile, "\t\t\t%s = new %s[%s];\n", tokens[i].identifier, tokens[i].keyword, tokens[i-1].identifier);
			fprintf(m_pDesFile,"\t\t\tfor(uint32 i = 0; i < %s; ++i)\n\t\t\t{\n",tokens[i-1].identifier);
			fprintf(m_pDesFile,"\t\t\t\t%s[i].unserial(Input);\n", tokens[i].identifier);
			fprintf(m_pDesFile, "\t\t\t}\n\t\t}\n");
			break;

		case CONTAINER :
			{
				char typeBuf[64] = {0};
				char *pLBrack = NULL;
				char *pRBrack = NULL;
				pLBrack = strchr(tokens[i].keyword, '<');
				pRBrack = strchr(tokens[i].keyword, '>');
				++pLBrack;
				strncpy(typeBuf, pLBrack, pRBrack-pLBrack);
				fprintf(m_pDesFile, "\t\tuint32 nSize%d = 0;\n", i);
				fprintf(m_pDesFile, "\t\treadsize(Input, nSize%d);\n", i);
				fprintf(m_pDesFile, "\t\tfor(uint32 i = 0; i < nSize%d; ++i)\n\t\t{\n", i);
				fprintf(m_pDesFile, "\t\t\t%s a;\n\t\t\t%s(Input, a);\n\t\t\t%s.push_back(a);\n",
					transform(typeBuf, g_CppTypeTable), transform(typeBuf, g_CppUnserialFuncTable), endlist->pPair[i].identifier);
				fprintf(m_pDesFile, "\t\t}\n");
			}
			break;

		case CLASSCONTAINER :
			{
				char typeBuf[64] = {0};
				char *pLBrack = NULL;
				char *pRBrack = NULL;
				pLBrack = strchr(tokens[i].keyword, '<');
				pRBrack = strchr(tokens[i].keyword, '>');
				++pLBrack;
				strncpy(typeBuf, pLBrack, pRBrack-pLBrack);

				fprintf(m_pDesFile, "\t\tuint32 nSize%d = 0;\n", i);
				fprintf(m_pDesFile, "\t\treadsize(Input, nSize%d);\n", i);
				fprintf(m_pDesFile, "\t\tfor(uint32 i = 0; i < nSize%d; ++i)\n\t\t{\n", i);
				fprintf(m_pDesFile, "\t\t\t%s tmp;\n", typeBuf);
				fprintf(m_pDesFile, "\t\t\ttmp.unserial(Input);\n");
				fprintf(m_pDesFile, "\t\t\t%s.push_back(tmp);\n\t\t}\n", tokens[i].identifier);
			}
			break;

		default:
			errorExit("Unexpected identifier %s.\n", tokens[i].identifier);
			break;
		}//end of switch
	}//end of for

	fprintf(m_pDesFile, "\t\treturn true;\n\t}\n");
}

std::string CCPPGen::getMarco()
{
	char macroBuf[256] = {0};
	char *pMacro = NULL;

	macroBuf[0] = '_';

	if((pMacro = strrchr((char*)m_strDesFile.c_str(), m_Seperator)) != NULL)
	{
		strcpy(&macroBuf[1], pMacro+1);
	}
	else
		strcpy(&macroBuf[1], m_strDesFile.c_str());
	if((pMacro = strchr(macroBuf, '.')) != NULL)
		*pMacro = '_';
	strcat(macroBuf, "_");
	//转换为大写
	for(size_t i = 1; macroBuf[i] != 0; ++i)
	{
		if(islower(macroBuf[i]))
			macroBuf[i] -= 32;
	}
	return std::string(macroBuf);
}

void CCPPGen::generateConstuctor()
{
	fprintf(m_pDesFile, "\t%s()\n\t{\n", endlist->typeName);
	for(size_t i = 0; i < endlist->count; ++i)
	{
		if(tokens[i].identifierType == POINTER || tokens[i].identifierType == CLASSPOINTER)
		{
			fprintf(m_pDesFile, "\t\tif(%s != NULL)\n\t\t{\n", tokens[i].identifier);
			fprintf(m_pDesFile, "\t\t\tdelete[] %s;\n", tokens[i].identifier);
			fprintf(m_pDesFile, "\t\t\t%s = NULL;\n\t\t}\n", tokens[i].identifier);
		}
		else if(tokens[i].identifierType == CONTAINER || tokens[i].identifierType == CLASSCONTAINER)
			fprintf(m_pDesFile, "\t\t%s.clear();\n", tokens[i].identifier);
		else if(!strcmp(tokens[i].keyword,"uint8") || !strcmp(tokens[i].keyword, "uint16") || !strcmp(tokens[i].keyword,"uint32") ||
			!strcmp(tokens[i].keyword, "uint64") || !strcmp(tokens[i].keyword,"int8") || !strcmp(tokens[i].keyword, "int16") ||
			!strcmp(tokens[i].keyword,"int32") || !strcmp(tokens[i].keyword, "int64") || !strcmp(tokens[i].keyword, "size_t"))
		{
			fprintf(m_pDesFile, "\t\t%s = 0;\n", tokens[i].identifier);
		}
		else if(tokens[i].identifierType == CLASS && strcmp(tokens[i].keyword, "CBytesBuffer") == 0 )
		{
			fprintf(m_pDesFile, "\t\t%s.incRef();\n", tokens[i].identifier);
		}
	}
	fprintf(m_pDesFile, "\t}\n");
}

void CCPPGen::generateCopyConstructor()
{
	fprintf(m_pDesFile, "\t%s(const %s &t)\n\t{\n", endlist->typeName, endlist->typeName);
	for(size_t i = 0; i < endlist->count; ++i)
	{
		if(tokens[i].identifierType == VARIABLE)
			fprintf(m_pDesFile, "\t\t%s = t.%s;\n", tokens[i].identifier, tokens[i].identifier);
		else if(tokens[i].identifierType == CLASS)
		{
			if (strcmp(tokens[i].keyword, "CBytesBuffer"))
			{
				fprintf(m_pDesFile, "\t\t%s = t.%s;\n", tokens[i].identifier, tokens[i].identifier);
			} 
			else
			{
				fprintf(m_pDesFile, "\t\t%s.clear();\n", tokens[i].identifier);
				fprintf(m_pDesFile, "\t\t%s.writeBytes(t.%s);\n", tokens[i].identifier, tokens[i].identifier);
			}

		}
		else if(tokens[i].identifierType == CONTAINER || tokens[i].identifierType == CLASSCONTAINER)
			fprintf(m_pDesFile, "\t\t%s.assign(t.%s.begin(), t.%s.end());\n",
			tokens[i].identifier, tokens[i].identifier, tokens[i].identifier);
		else if(tokens[i].identifierType == POINTER || tokens[i].identifierType == CLASSPOINTER)
		{
			fprintf(m_pDesFile, "\t\tif(%s != NULL)\n", tokens[i].identifier);
			fprintf(m_pDesFile, "\t\t\tdelete[] %s;\n", tokens[i].identifier);
			fprintf(m_pDesFile, "\t\t%s = new %s[t.%s];\n", tokens[i].identifier, tokens[i].keyword, tokens[i-1].identifier);
			fprintf(m_pDesFile, "\t\tmemcpy((void*)%s, (void*)(t.%s), sizeof(%s[0])*%s);\n", tokens[i].identifier,
				tokens[i].identifier, tokens[i].identifier, tokens[i-1].identifier);
		}
		//数组
		//else if(tokens[i].identifierType > 0)
		//{
		//	fprintf(m_pDesFile, "\t\tfor(uint32 i = 0; i < %d; ++i)\n\t\t{\n", tokens[i].identifierType);
		//	fprintf(m_pDesFile, "\t\t\t%s[i] = t.%s[i];\n\t\t}\n", tokens[i].identifier, tokens[i].identifier);
		//}
	}
	fprintf(m_pDesFile, "\t}\n");
}

void CCPPGen::generateAssignmentConstructor()
{
	fprintf(m_pDesFile, "\t%s& operator = (const %s& t)\n\t{\n", endlist->typeName, endlist->typeName);
	for(size_t i = 0; i < endlist->count; ++i)
	{
		if(tokens[i].identifierType == VARIABLE)
			fprintf(m_pDesFile, "\t\t%s = t.%s;\n", tokens[i].identifier, tokens[i].identifier);
		else if(tokens[i].identifierType == CLASS)
		{
			if (strcmp(tokens[i].keyword, "CBytesBuffer"))
			{
				fprintf(m_pDesFile, "\t\t%s = t.%s;\n", tokens[i].identifier, tokens[i].identifier);
			} 
			else
			{
				fprintf(m_pDesFile, "\t\t%s.clear();\n", tokens[i].identifier);
				fprintf(m_pDesFile, "\t\t%s.writeBytes(t.%s);\n", tokens[i].identifier, tokens[i].identifier);
			}

		}
		else if(tokens[i].identifierType == CONTAINER || tokens[i].identifierType == CLASSCONTAINER)
			fprintf(m_pDesFile, "\t\t%s.assign(t.%s.begin(), t.%s.end());\n",
			tokens[i].identifier, tokens[i].identifier, tokens[i].identifier);
		else if(tokens[i].identifierType == POINTER || tokens[i].identifierType == CLASSPOINTER)
		{
			fprintf(m_pDesFile, "\t\tif(%s != NULL)\n", tokens[i].identifier);
			fprintf(m_pDesFile, "\t\t\tdelete[] %s;\n", tokens[i].identifier);
			fprintf(m_pDesFile, "\t\t%s = new %s[t.%s];\n", tokens[i].identifier, tokens[i].keyword, tokens[i-1].identifier);
			fprintf(m_pDesFile, "\t\tmemcpy((void*)%s, (void*)(t.%s), sizeof(%s[0])*%s);\n", tokens[i].identifier,
				tokens[i].identifier, tokens[i].identifier, tokens[i-1].identifier);
		}
		//else if(tokens[i].identifierType > 0)
		//{
		//	fprintf(m_pDesFile, "\t\tfor(uint32 i = 0; i < %d; ++i)\n\t\t{\n", tokens[i].identifierType);
		//	fprintf(m_pDesFile, "\t\t\t%s[i] = t.%s[i];\n\t\t}\n", tokens[i].identifier, tokens[i].identifier);
		//}
	}
	fprintf(m_pDesFile, "\t\treturn *this;\n\t}\n");
}

void CCPPGen::generateDestructor()
{
	fprintf(m_pDesFile, "\tvirtual ~%s()\n\t{\n", endlist->typeName);
	for(size_t i = 0; i < endlist->count; ++i)
	{
		if(tokens[i].identifierType == POINTER || tokens[i].identifierType == CLASSPOINTER)
		{
			fprintf(m_pDesFile, "\t\tif(%s != NULL)\n\t\t{\n", tokens[i].identifier);
			fprintf(m_pDesFile, "\t\t\tdelete[] %s;\n", tokens[i].identifier);
			fprintf(m_pDesFile, "\t\t\t%s = NULL;\n\t\t}\n", tokens[i].identifier);
		}
	}
}

void CCPPGen::generateBegin()
{
	for(std::list<std::string>::iterator iter = g_lstPrecompile.begin(); iter != g_lstPrecompile.end(); iter++)
		fprintf(m_pDesFile, "%s", iter->c_str());
	fprintf(m_pDesFile, "\n");
	g_lstPrecompile.clear();
}

void CCPPGen::generateFile()
{
	generateBegin();
	
	fprintf(m_pDesFile, "class %s : public %s\n{\npublic:\n", endlist->typeName, endlist->baseClass);
	writeVariable();
	serialFunc();
	unserialFunc();

	generateConstuctor();
	generateCopyConstructor();
	generateAssignmentConstructor();
	generateDestructor();

	fprintf(m_pDesFile, "\t}\n");
	fprintf(m_pDesFile, "};\n\n");
	fprintf(m_pDesFile, "typedef CRefObject<%s> %sPtr;\n\n", endlist->typeName, endlist->typeName);
}

void CCPPGen::generateEnd()
{
	fprintf(m_pDesFile, "#endif\n\n");

	if(m_pDesFile)
		fclose(m_pDesFile);

	g_lstPrecompile.clear();
}

void CCPPGen::writeEnumHead(const char *enumName)
{
	fprintf(m_pDesFile, "enum %s\n{\n", enumName);
}

void CCPPGen::writeEnumIV(const char *identifier, const char *value)
{
	if(value != NULL)
	{
		fprintf(m_pDesFile, "\t%s = %s,\n", identifier, value);
	}
	else
	{
		fprintf(m_pDesFile, "\t%s,\n", identifier);
	}
}

void CCPPGen::writeEnumIVI(const char *identifier, int value)
{
	fprintf(m_pDesFile, "\t%s = %d,\n", identifier, value);
}

void CCPPGen::writeEnumEnd()
{
	fprintf(m_pDesFile, "};\n\n");
}
