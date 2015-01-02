#pragma once
#include "codegen.h"
#include <vector>
#include <string>

class PhpCodeGen :
	public ICodeGen
{
public:
	PhpCodeGen(void);
	virtual ~PhpCodeGen(void);

	virtual void initialize(const char *desFile);
	virtual void release();

	virtual void releaseError();
	virtual void generateBegin();
	virtual void generateFile();
	virtual void generateEnd();

	virtual void writeEnumHead(const char *enumName);
	virtual void writeEnumIV(const char *identifier, const char *value);
	virtual void writeEnumIVI(const char *identifier, int value);
	virtual void writeEnumEnd();

private :
	void writeVariable();
	void serialFunc();
	void unserialFunc();

    void constructFunc();

private :
	FILE* m_pDesFile;
	std::string m_strDesFile;
	char m_cSeparator;
};
