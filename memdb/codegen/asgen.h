#include <vector>
#include <string>

#include "codegen.h"

extern std::vector<std::string> g_importVec;

class CASGen : public ICodeGen
{
public :
	CASGen();
	virtual ~CASGen();

	//初始化
	virtual void initialize(const char *desFile);
	//释放资源
	virtual void release();
	//发生错误时释放资源
	virtual void releaseError();
	//开始生成文件
	virtual void generateBegin(){}
	//生成文件
	virtual void generateFile();
	//结束生成文件
	virtual void generateEnd(){}

	//todo
	void writeEnumHead(const char *enumName);
	void writeEnumIV(const char *identifier, const char *value);
	void writeEnumIVI(const char *identifier, int value);
	void writeEnumEnd();

private :
	void writeVariable();
	void serialFunc();
	void unserialFunc();

private :
	FILE* m_pDesFile;
	//路径
	std::string m_strPath;
	//分隔符
	char m_cSeparator;
};