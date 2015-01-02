#ifndef _CODEGEN_H_
#define _CODEGEN_H_

typedef struct _t_t_typerecord 
{
	const char *srctype;
	const char *destype;
}typerecord;

class ICodeGen
{
public :
	ICodeGen(){}
	virtual ~ICodeGen(){}

	//初始化
	virtual void initialize(const char *desFile) = 0;
	//释放资源
	virtual void release() = 0;
	//发生错误时释放资源
	virtual void releaseError() = 0;
	//开始生成文件
	virtual void generateBegin() = 0;
	//生成文件
	virtual void generateFile() = 0;
	//结束生成文件
	virtual void generateEnd() = 0;

	//to do delete
	virtual void writeEnumHead(const char *enumName) = 0;
	virtual void writeEnumIV(const char *identifier, const char *value) = 0;
	virtual void writeEnumIVI(const char *identifier, int value) = 0;
	virtual void writeEnumEnd() = 0;
};

#endif
