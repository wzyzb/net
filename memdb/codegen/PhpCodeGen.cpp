#include "PhpCodeGen.h"

#include <stdlib.h>
#include <string.h>
#include <list>
#include <string>

#include "genglobal.h"
#include "codebuilder.h"
#include "cppgen.h"

typerecord g_PhpSerialFuncTable[] =
{
	{"uint32", "writeUint32"},
	{"uint16", "writeUint16"},
	{"uint8", "writeUint8"},
	{"int32", "writeUint32"},
	{"int16", "writeUint16"},
	{"int8", "writeUint8"},
	{"char", "writeUint8"},
	{"std::string", "writeString"},
	{"string", "writeString"},
	{NULL, NULL}
};

typerecord g_PhpUnsrialFuncTable[] = 
{
	{"uint32", "readUint32"},
	{"uint16", "readUint16"},
	{"uint8", "readUint8"},
	{"int32", "readUint32"},
	{"int16", "readUint16"},
	{"int8", "readUint8"},
	{"char", "readUint8"},
	{"std::string", "readString"},
	{"string", "readString"},
	{NULL, NULL}
};


PhpCodeGen::PhpCodeGen(void)
{
	m_pDesFile = NULL;
	m_cSeparator = '/';
}

PhpCodeGen::~PhpCodeGen(void)
{
    release();
}

void PhpCodeGen::initialize(const char *desFile)
{
	char desfile[256] = {0};
	strcpy(desfile, desFile);
	for (size_t i = 0; i < strlen(desfile); i++)
	{
		if(desfile[i] == '\\')
			desfile[i] = m_cSeparator;
	}
	m_strDesFile = desfile;

	if((m_pDesFile = fopen(m_strDesFile.c_str(), "w")) == NULL)
	{
		printf("initCpp|cannot open cpp desfile : %s\n", m_strDesFile.c_str());
		exit(1);
	}
}
void PhpCodeGen::release()
{
	if(m_pDesFile)
		fclose(m_pDesFile);
	g_lstPrecompile.clear();
}
void PhpCodeGen::releaseError()
{
	release();
}
void PhpCodeGen::generateBegin()
{
	fprintf(m_pDesFile, "<?php\n");
}
void PhpCodeGen::generateEnd()
{
	fprintf(m_pDesFile, "?>\n");
}
void PhpCodeGen::writeEnumHead(const char *enumName)
{

}

void PhpCodeGen::writeEnumIV(const char *identifier, const char *value)
{

}
void PhpCodeGen::writeEnumIVI(const char *identifier, int value)
{

}
void PhpCodeGen::writeEnumEnd()
{

}
void PhpCodeGen::generateFile()
{
	fprintf(m_pDesFile, "class %s : implements Serializer\n{\n", endlist->typeName);
	this->writeVariable();
	this->constructFunc();
	this->serialFunc();
	this->unserialFunc();

	fprintf(m_pDesFile, "}\n\n");
	
}

void PhpCodeGen::writeVariable()
{
	for(size_t i = 0; i < endlist->count; ++i)
	{
		fprintf(m_pDesFile, "\tvar $%s;\n", endlist->pPair[i].identifier);
	}
	fprintf(m_pDesFile, "\n");
}
void PhpCodeGen::constructFunc()
{
    fprintf(m_pDesFile, "\tpublic function __construct()\n\t{\n");

	for(size_t i = 0; i < endlist->count; ++i)
	{
        if(tokens[i].identifierType == CONTAINER || tokens[i].identifierType == CLASSCONTAINER)
		{
			fprintf(m_pDesFile, "\t\t$this->%s = array();\n", tokens[i].identifier);
		}
		else if(!strcmp(tokens[i].keyword,"uint8") || !strcmp(tokens[i].keyword, "uint16") || !strcmp(tokens[i].keyword,"uint32") ||
			!strcmp(tokens[i].keyword, "uint64") || !strcmp(tokens[i].keyword,"int8") || !strcmp(tokens[i].keyword, "int16") ||
			!strcmp(tokens[i].keyword,"int32") || !strcmp(tokens[i].keyword, "int64"))
		{
			fprintf(m_pDesFile, "\t\t$this->%s = 0;\n", tokens[i].identifier);
		}
		else if(!strcmp(tokens[i].keyword,"string") || !strcmp(tokens[i].keyword,"std::string") )
		{
			fprintf(m_pDesFile, "\t\t$this->%s = '';\n", tokens[i].identifier);
		}
	}

	fprintf(m_pDesFile, "\t}\n");
}


void PhpCodeGen::serialFunc()
{
	fprintf(m_pDesFile, "\tpublic function serialize($ServerProtocol)\n\t{\n");

	for(size_t i = 0; i < endlist->count; ++i)
	{
		switch(tokens[i].identifierType)
		{
		case VARIABLE :
			fprintf(m_pDesFile, "\t\t$ServerProtocol->%s($this->%s);\n", transform(endlist->pPair[i].keyword, g_PhpSerialFuncTable),
				endlist->pPair[i].identifier);
			break;
        case CLASSCONTAINER :
		case CONTAINER :
			{
			    char typeBuf[64] = {0};
				char *pLBrack = NULL;
				char *pRBrack = NULL;
				pLBrack = strchr(tokens[i].keyword, '<');
				pRBrack = strchr(tokens[i].keyword, '>');
				++pLBrack;
				strncpy(typeBuf, pLBrack, pRBrack-pLBrack);

				if(transform(typeBuf, g_PhpSerialFuncTable) == NULL)
				{
					fprintf(m_pDesFile, "\t\t$ServerProtocol->writeTypeArray($this->%s);\n",endlist->pPair[i].identifier);
				}
				else
				{
					fprintf(m_pDesFile, "\t\t$ServerProtocol->writeObjectArray($this->%s);\n",endlist->pPair[i].identifier);
				}
			    break;
			}
		default:
			errorExit("Unexpected identifier %s.\n", tokens[i].identifier);
			break;
		}//end of switch
	}//end of for

    fprintf(m_pDesFile, "\t}\n");
}
void PhpCodeGen::unserialFunc()
{
	fprintf(m_pDesFile, "\tpublic function unserialize($ServerProtocol)\n\t{\n");

	for(size_t i = 0; i < endlist->count; ++i)
	{
		switch(tokens[i].identifierType)
		{
		case VARIABLE :
			fprintf(m_pDesFile, "\t\t$ServerProtocol->%s($this->%s);\n", transform(endlist->pPair[i].keyword, g_PhpUnsrialFuncTable),
				endlist->pPair[i].identifier);
			break;
        case CLASSCONTAINER :
		case CONTAINER :
			{
			    char typeBuf[64] = {0};
				char *pLBrack = NULL;
				char *pRBrack = NULL;
				pLBrack = strchr(tokens[i].keyword, '<');
				pRBrack = strchr(tokens[i].keyword, '>');
				++pLBrack;
				strncpy(typeBuf, pLBrack, pRBrack-pLBrack);

				if(transform(typeBuf, g_PhpSerialFuncTable) == NULL)
				{
					fprintf(m_pDesFile, "\t\t$ServerProtocol->readTypeArray($this->%s);\n",endlist->pPair[i].identifier);
				}
				else
				{
					fprintf(m_pDesFile, "\t\t$ServerProtocol->readObjectArray($this->%s);\n",endlist->pPair[i].identifier);
				}
			    break;
			}
		default:
			errorExit("Unexpected identifier %s.\n", tokens[i].identifier);
			break;
		}//end of switch
	}//end of for

	fprintf(m_pDesFile, "\t}\n");
}