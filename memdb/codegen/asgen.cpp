#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>

#include "genglobal.h"
#include "codebuilder.h"
#include "asgen.h"

typerecord g_AsTypeTable[] = 
{
	{"uint32", "uint"},
	{"uint16", "int"},
	{"size_t", "int"},
	{"uint8", "int"},
	{"char", "int"},
	{"int32", "int"},
	{"int16", "int"},
	{"int8", "int"},
	{"bytes", "int"},
	{"bool", "Boolean"},
	{"std::string", "String"},
	{"string", "String"},
	{NULL, NULL}
};

typerecord g_AsSerialFuncTable[] =
{
	{"uint32", "writeUnsignedInt"},
	{"uint16", "writeUnsignedShort"},
	{"size_t", "writeUnsignedShort"},
	{"uint8", "writeByte"},
	{"int32", "writeInt"},
	{"int16", "writeShort"},
	{"int8", "writeByte"},
	{NULL, NULL}
};

typerecord g_AsUnsrialFuncTable[] = 
{
	{"uint32", "readUnsignedInt"},
	{"uint16", "readUnsignedShort"},
	{"size_t", "readUnsignedShort"},
	{"uint8", "readByte"},
	{"char", "readByte"},
	{"int32", "readInt"},
	{"int16", "readShort"},
	{"int8", "readByte"},
	{NULL, NULL}
};

CASGen::CASGen()
{
	m_pDesFile = NULL;
	m_cSeparator = '/';
}

CASGen::~CASGen()
{
	release();
}

void CASGen::initialize(const char *desFile)
{
	//¹¹Ôì´æ´¢Â·¾¶
	m_strPath = desFile;
	std::string::size_type index = m_strPath.find_last_of(m_cSeparator);
	if (index != std::string::npos)
	{
		m_strPath = m_strPath.substr(0, index + 1);
	}
	else
		m_strPath = "";
}

void CASGen::release()
{
	if(m_pDesFile)
		fclose(m_pDesFile);
	g_importVec.clear();
}

void CASGen::releaseError()
{
	release();
}

void CASGen::writeVariable()
{
	for(size_t i = 0; i < endlist->count; ++i)
	{
		switch(endlist->pPair[i].identifierType)
		{
		case VARIABLE:	
			fprintf(m_pDesFile, "\t\tpublic var %s : %s;\n", endlist->pPair[i].identifier,
				transform(endlist->pPair[i].keyword, g_AsTypeTable));
			break;
		case CONTAINER:
			if(strncmp(endlist->pPair[i].keyword, "vector", 6) == 0 ||
				strncmp(endlist->pPair[i].keyword, "std::vector", 11) == 0)
				fprintf(m_pDesFile, "\t\tpublic var %s : Array;\n", endlist->pPair[i].identifier);
			break;
		default :
			printf("\ntodo : %s %s\n\n", endlist->pPair[i].keyword, endlist->pPair[i].identifier);
			break;
		}
	}

}

void CASGen::serialFunc()
{
	fprintf(m_pDesFile, "\t\tpublic function send(ServantName : int");// ): void\n\t\t{\n");
	for (size_t i = 0; i < endlist->count; ++i)
	{
		fprintf(m_pDesFile, ",%s:%s", endlist->pPair[i].identifier,
			transform(endlist->pPair[i].keyword, g_AsTypeTable));
	}
	fprintf(m_pDesFile, "): void\n\t\t{\n");
	fprintf(m_pDesFile, "\t\t\tvar dataPack : DataPack = new DataPack(ServantName);\n");
	fprintf(m_pDesFile, "\t\t\tvar body : BytesArray = dataPack.createDataBody();\n");
	for(size_t i = 0; i < endlist->count; ++i)
	{
		switch(endlist->pPair[i].identifierType)
		{
		case VARIABLE:
			if(strcmp(endlist->pPair[i].keyword, "string") == 0 ||
				strcmp(endlist->pPair[i].keyword, "std::string") == 0)
			{
				fprintf(m_pDesFile, "\t\t\tvar tmpBa%d:ByteArray = DataUtil.writeUTF(%s);\n", i, endlist->pPair[i].identifier);
				fprintf(m_pDesFile, "\t\t\tbody.writeBytes(tmpBa%d, 0, tmpBa%d.length);\n", i, i);
			}
			else
				fprintf(m_pDesFile, "\t\t\tbody.%s(%s);\n", transform(endlist->pPair[i].keyword, g_AsSerialFuncTable),
				endlist->pPair[i].identifier);
			break;
		case CONTAINER:
			printf("\ntodo : %s %s\n\n", endlist->pPair[i].keyword, endlist->pPair[i].identifier);
			break;
		default :
			printf("\ntodo : %s %s\n\n", endlist->pPair[i].keyword, endlist->pPair[i].identifier);
			break;
		}
	}
	fprintf(m_pDesFile, "\t\t\tGatawayServiceProxy.getInstance().send(dataPack);\n");
	fprintf(m_pDesFile, "\t\t}\n\n");
}

void CASGen::unserialFunc()
{
	fprintf(m_pDesFile, "\t\tpublic override function onSuccess(body : ByteArray) : void\n\t\t{\n");
	for(size_t i = 0; i < endlist->count; ++i)
	{
		switch(endlist->pPair[i].identifierType)
		{
		case VARIABLE:
			if(strcmp(endlist->pPair[i].keyword, "string") == 0 ||
				strcmp(endlist->pPair[i].keyword, "std::string") == 0)
				fprintf(m_pDesFile, "\t\t\t%s = DataUtil.readUTF(body);\n", endlist->pPair[i].identifier);
			else
				fprintf(m_pDesFile, "\t\t\t%s = body.%s();\n", endlist->pPair[i].identifier,
				transform(endlist->pPair[i].keyword, g_AsUnsrialFuncTable));
			break;
		case CONTAINER:
			if(strncmp(endlist->pPair[i].keyword, "vector", 6) == 0 ||
				strncmp(endlist->pPair[i].keyword, "std::vector", 11) == 0)
			{
				fprintf(m_pDesFile, "\t\t\t%s = [];\n", endlist->pPair[i].identifier);
				fprintf(m_pDesFile, "\t\t\tvar size:Size = new Size();\n");
				fprintf(m_pDesFile, "\t\t\tsize.readByBody(body);\n");
				fprintf(m_pDesFile, "\t\t\tvar len:int = size.size();\n");
				fprintf(m_pDesFile, "\t\t\tfor(var i:int = 0; i < len; ++i){\n");

				char itemType[64] = {0};
				char *pbitem = strchr(endlist->pPair[i].keyword, '<') + 1;
				strncpy(itemType, pbitem, strchr(endlist->pPair[i].keyword, '>') - pbitem);
				if(strcmp(endlist->pPair[i].keyword, "string") == 0 ||
					strcmp(endlist->pPair[i].keyword, "std::string") == 0)
					fprintf(m_pDesFile, "\t\t\t\tvar tmp:String = DataUtil.readUTF(body);\n");
				else
					fprintf(m_pDesFile, "\t\t\t\tvar tmp:%s = body.%s();\n",transform(itemType, g_AsTypeTable),
					transform(itemType, g_AsUnsrialFuncTable));

				fprintf(m_pDesFile, "\t\t\t\t%s.push(tmp);\n\t\t\t}\n", endlist->pPair[i].identifier);
			}
			break;
		default :
			printf("\ntodo : %s %s\n\n", endlist->pPair[i].keyword, endlist->pPair[i].identifier);
			break;
		}
	}
	fprintf(m_pDesFile, "\t\t}\n\n");

}

void CASGen::generateFile()
{
	std::string strfilename = m_strPath + endlist->typeName + ".as";

	coverOrNot(strfilename.c_str());
	
	if((m_pDesFile = fopen(strfilename.c_str(), "w")) == NULL)
	{
		printf("CASGen::generateFile|cannot open asdes file : %s\n", strfilename.c_str());
		exit(1);
	}

	if(strstr(endlist->typeName, "Request"))
	{
		fprintf(m_pDesFile, "package mmo.net.sendStroage\n{\n");

		fprintf(m_pDesFile, "\timport flash.utils.ByteArray;\n");
		fprintf(m_pDesFile, "\timport mmo.net.NetConst;\n");
		fprintf(m_pDesFile, "\timport mmo.net.core.GatawayServiceProxy;\n");
		fprintf(m_pDesFile, "\timport mmo.utils.DataPack;\n");

		for(size_t i = 0; i < g_importVec.size(); ++i)
			fprintf(m_pDesFile, "\t%s", g_importVec[i].c_str());
		fprintf(m_pDesFile, "\n");

		fprintf(m_pDesFile, "\tpublic class %s\n\t{\n", endlist->typeName);
		//writeAsVariable();
		//fprintf(m_pDesFile, "\t\tprotected var dataPack : DataPack;\n\n");
		serialFunc();
		fprintf(m_pDesFile, "\t}\n}\n\n");
	}
	else
	{
		fprintf(m_pDesFile, "package mmo.net.response\n{\n");		
		fprintf(m_pDesFile, "\timport flash.utils.ByteArray;\n");
		fprintf(m_pDesFile, "\timport mmo.net.events.SceneNetEvent;\n");
		fprintf(m_pDesFile, "\timport mmo.net.model.Size;\n");
		fprintf(m_pDesFile, "\timport mmo.net.response.base.BaseResponse;\n");
		fprintf(m_pDesFile, "\timport mmo.net.response.base.IResponse;\n");
		fprintf(m_pDesFile, "\timport mmo.trigger.NetEventTrigger;\n");
		for(size_t i = 0; i < g_importVec.size(); ++i)
			fprintf(m_pDesFile, "\t%s", g_importVec[i].c_str());
		fprintf(m_pDesFile, "\n");

		fprintf(m_pDesFile, "\tpublic class %s extends BaseResponse implements IResponse\n\t{\n", endlist->typeName);

		writeVariable();

		//constructor
		fprintf(m_pDesFile, "\t\tpublic function %s()\n\t\t{\n", endlist->typeName);
		fprintf(m_pDesFile, "\t\t\tsuper();\n\t\t}\n\n");

		unserialFunc();

		fprintf(m_pDesFile, "\t}\n}\n\n");
	}
	g_importVec.clear();
	if (m_pDesFile)
	{
		fclose(m_pDesFile);
		m_pDesFile = NULL;
	}
}

void CASGen::writeEnumHead(const char *enumName)
{
	fprintf(m_pDesFile, "package mmo.net\n{\n");
	fprintf(m_pDesFile, "\tpublic class %s\n\t{", enumName);
}

void CASGen::writeEnumIV(const char *identifier, const char *value)
{
	if(value != NULL)
	{
		fprintf(m_pDesFile, "\n\t\tpublic static const %s : int = %s;", identifier, value);
	}
	else
	{
		fprintf(m_pDesFile, "\n\t\tpublic static const %s : int;", identifier);
	}
}

void CASGen::writeEnumIVI(const char *identifier, int value)
{
	fprintf(m_pDesFile, "\n\t\tpublic static const %s : int = %d;", identifier, value);
}

void CASGen::writeEnumEnd()
{
	fprintf(m_pDesFile, "\t}\n};\n");
}
