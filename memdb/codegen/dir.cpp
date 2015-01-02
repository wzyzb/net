//#ifdef _MSC_VER
//#include "stdafx.h"
//#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#ifdef WIN32
#pragma warning (disable:4800)
#include <winsock.h>
#include <io.h>
#endif
#include "dir.h"
#include <iostream>
/*
		class CDir
*/
CDir::CDir()
{
	this->bOpened = false;
	this->bNext = false;
#ifdef WIN32
	dp = INVALID_HANDLE_VALUE;
#else
	dp = NULL;
#endif
}

CDir::~CDir()
{
	Close();
}

bool CDir::OpenDirectory(const char* pathname)
{
	this->strDirectory = pathname;
	if ( strDirectory[strDirectory.length()-1] != '\\' &&
		strDirectory[strDirectory.length()-1] != '/' )
	{
#ifdef WIN32
		strDirectory += "\\";
#else
		strDirectory += "/";
#endif
	}
#ifdef WIN32
	dp = FindFirstFile((strDirectory + "*").c_str(),&this->fp);
	if ( dp == INVALID_HANDLE_VALUE )
		return false;
#else
	dp = opendir(strDirectory.c_str());
	if ( dp == NULL )
		return false;
#endif
	this->bOpened = true;
	return true;
}

bool CDir::HasNextFile()
{
	if ( !bOpened )
		return false;
	this->strFileName = "";
#ifdef WIN32
	this->bNext = FindNextFile(dp,&fp);
	if ( bNext )
		strFileName = fp.cFileName;
#else
	struct dirent *dirp;
	dirp = readdir(dp);
	if ( dirp == NULL )
		bNext = false;
	else
	{
		bNext =true;
		strFileName = dirp->d_name;
		char filename[500];
		sprintf(filename,"%s%s",strDirectory.c_str(),strFileName.c_str());
		stat(filename,&fp);
	}
#endif
	return bNext;
}

string CDir::NextFile()
{
	return this->strFileName;
}

string CDir::GetDirectory()
{
	return this->strDirectory;
}

bool CDir::isDirectory()
{
#ifdef WIN32
	return ( fp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY );
#else
	return ( S_ISDIR(fp.st_mode) );
#endif
}

bool CDir::isFile()
{
#ifdef WIN32
	return !( fp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY );
#else
	return ( S_ISREG(fp.st_mode) );
#endif
}

ulong64 CDir::GetFileSize()
{
	ulong64 filesize;
	filesize.HighFileSize = 0;
	filesize.LowFileSize = 0;
#ifdef WIN32
	filesize.HighFileSize = fp.nFileSizeHigh;
	filesize.LowFileSize = fp.nFileSizeLow;
#else
	filesize.LowFileSize = fp.st_size & 0xFFFFFFFF;
#ifdef _64BIT_
	filesize.HighFileSize = fp.st_size >> 8 ;
#endif
#endif
	return filesize;
}

int CDir::GetNodeType(const char* pathname)
{
	STAT_DATA statbuf;
#ifdef WIN32
	DIR_HANDLE fHandle;
	fHandle = FindFirstFile(pathname, &statbuf);
	if(fHandle == INVALID_HANDLE_VALUE)
		return DIR_INEXISTENCE;
	if(statbuf.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return DIR_DIRECTORY;
	else
		return DIR_FILE;
#else
	if(lstat(pathname, &statbuf) == -1)
		return DIR_INEXISTENCE;
	if(S_ISREG(statbuf.st_mode))
		return DIR_FILE;
	if(S_ISDIR(statbuf.st_mode))
		return DIR_DIRECTORY;
#endif
}

//递归创建目录
void CDir::MakeDirectory(const char *pathname)
{
	const char *p = pathname;
	char pathBuf[1024] = {0};
#ifdef WIN32
	char separator = '\\';
#else
	char separator = '/';
#endif
	for(p = strchr(p, separator); p != NULL; p = strchr(p, separator))
	{
		strncpy(pathBuf, pathname, p-pathname);
#ifdef WIN32
		_mkdir(pathBuf);
#else
		mkdir(pathBuf, 777);
#endif
		++p;
	}
#ifdef WIN32
	_mkdir(pathname);
#else
	mkdir(pathname, 777);
#endif
}

void CDir::Close()
{
#ifdef WIN32
	if ( dp != INVALID_HANDLE_VALUE )
		FindClose(dp);
	dp = INVALID_HANDLE_VALUE;
#else
	if ( dp != NULL )
		closedir(dp);
	dp = NULL;
#endif
	bOpened = false;
}

/*
		class CFileFilter
		?表示任意一个字节
		*表示任意长度的字节
*/
CFileFilter::~CFileFilter()
{
}

CFileFilter::CFileFilter(const string& filter)
{
	filematchname = filter;
}

bool CFileFilter::isMatch(const string& filename)
{
	int oldstate = -1;
	size_t iFilterBeginIndex = 0,iFileNameIndex =0;
	char curch=0,prevch=0,firstch = 0;
	for ( iFilterBeginIndex = 0 ; iFilterBeginIndex < filematchname.length(); iFilterBeginIndex++)
	{
		if ( iFileNameIndex >= filename.length() )
			return false;
		curch = filematchname[iFilterBeginIndex];
		switch ( curch )
		{
			case '?' :
				iFileNameIndex++;
				break;
			case '*' :
				oldstate = iFilterBeginIndex;
				break;
			default :
				if ( prevch == '*' || prevch == 0 )
					firstch = curch;
				if ( curch != filename[iFileNameIndex] )
				{
					if ( oldstate == -1 )
						return false;
					else
					{
						if ( firstch == filename[iFileNameIndex] )
							iFileNameIndex --;
						iFilterBeginIndex = oldstate;
					}
				}
				iFileNameIndex++;
		}
		prevch = curch;
	}
	//出现文件名还有剩余长度的情况
	if ( iFileNameIndex != filename.length() )
	{
		//如果后面是*的匹配则认为完全正确
		if ( curch == '*' )
			return true;
		//如果匹配规则中曾经出现*号则进行反向匹配
		else if ( oldstate != -1 )
		{
			iFileNameIndex = filename.length() - 1;
			//从文件名后进行匹配
			for ( iFilterBeginIndex--;iFilterBeginIndex >= 0;iFilterBeginIndex--,iFileNameIndex-- )
			{
				curch = filematchname[iFilterBeginIndex];
				switch ( curch )
				{
					case '?' :
						break;
					case '*' :
						return true;
					default :
						if ( curch != filename[iFileNameIndex] )
							return false;
				}
			}
			return false;
		}
		//否则认为匹配错误
		else
			return false;
	}
	return true;
}

