#include <list>
#include <string>

#include "codegen.h"

extern std::list<std::string> g_lstPrecompile;

class CCPPGen : public ICodeGen
{
public :
	CCPPGen();
	virtual ~CCPPGen();

	//初始化
	virtual void initialize(const char *desFile);
	//释放资源
	virtual void release();
	//发生错误时释放资源
	virtual void releaseError();
	//开始生成文件
	virtual void generateBegin();
	//生成文件
	virtual void generateFile();
	//结束生成文件
	virtual void generateEnd();

	//todo
	void writeEnumHead(const char *enumName);
	void writeEnumIV(const char *identifier, const char *value);
	void writeEnumIVI(const char *identifier, int value);
	void writeEnumEnd();

private :
	void writeVariable();
	void serialFunc();
	void unserialFunc();
	std::string getMarco();
	void generateConstuctor();
	void generateCopyConstructor();
	void generateAssignmentConstructor();
	void generateDestructor();

private :
	FILE* m_pDesFile;
	std::string m_strDesFile;
	//分割符
	char m_Seperator;
};
