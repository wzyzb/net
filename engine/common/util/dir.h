#ifndef _DIR_H_
#define _DIR_H_
#include <string>
using namespace std;
#ifdef WIN32
//#include <winbase.h>
#include <windows.h>
#include <direct.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

#ifdef WIN32
#define DIR_HANDLE HANDLE
#define STAT_DATA WIN32_FIND_DATA
#else
#define DIR_HANDLE DIR*
#define STAT_DATA struct stat
#endif

#ifndef ulong64
typedef struct _tULong64
{
	unsigned long HighFileSize;
	unsigned long LowFileSize;
}ulong64;
#endif

enum EDIRTYPE
{
	DIR_INEXISTENCE,
	DIR_FILE,
	DIR_DIRECTORY,
};

//操作目录的类，用于实现查询某个目录下的文件名
class CDir
{
private :
	bool bOpened;
	bool bNext;
	string strDirectory;
	string strFileName;
	DIR_HANDLE dp;
	STAT_DATA fp;

public :
	CDir();
	~CDir();
	//打开指定的目录
	bool OpenDirectory(const char* pathname);
	//查询是否还有文件
	bool HasNextFile();
	//获取下一个文件的名字－不包含路径
	string NextFile();
	//返回当前文件是否是目录
	bool isDirectory();
	//返回当前文件是否是文件
	bool isFile();
	//返回当前文件所在目录
	string GetDirectory();
	//获取当前文件的大小
	ulong64 GetFileSize();
	//判断类型
	static int GetNodeType(const char* pathname);
	//递归创建目录
	static void MakeDirectory(const char *pathname);
	void Close();
};

class CFileFilter
{
private :
	string filematchname;

public :
	CFileFilter(const string& filter);
	~CFileFilter();
	bool isMatch(const string& filename);
};
#endif
