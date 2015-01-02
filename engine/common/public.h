#ifndef _PUBLIC_H_
#define _PUBLIC_H_


//编译选项
//是否支持多线程，0－不支持，1－支持
#define HAS_MULTITHREAD 1

#include <stdio.h>
#include <signal.h>
#include <memory.h>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <assert.h>
#include <sys/stat.h>
using namespace std;

//允许的最大路径长度
#define _MAX_PATH_LENGTH 512

#if !defined(SIZEOF_LONG) && (defined(_L64) || defined(_LP64) || defined(__osf__))
#define SIZEOF_LONG 8
#endif

//公共类型
#if defined(_WIN32)||defined(_WIN64)
#define WINDOWS 1
#pragma warning (disable : 4786)
#pragma warning (disable : 4503)
#pragma warning (disable : 4996)
#include <winsock2.h>
#include <time.h>
#define CLOSE_SOCKET(a) closesocket(a);
#ifndef int64
typedef __int64 int64;
#endif
#ifndef int32
typedef __int32 int32;
#endif
#ifndef int16
typedef __int16 int16;
#endif
#ifndef int8
typedef char int8;
#endif
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
typedef unsigned __int16 uint16;
typedef unsigned __int8 uint8;
#define snprintf _snprintf
#define	atol64(a) _atoi64(a)
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define stat _stat
#ifdef _USE_DLL_
#if defined(_WINDLL) // inside DLL
#   define DLLAPI   __declspec(dllexport)
#else // outside DLL
#   define DLLAPI   __declspec(dllimport)
#endif
#else
#define DLLAPI
#endif

#else
#define DLLAPI
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#define CLOSE_SOCKET(a) ::close(a);
#ifndef SOCKET
typedef int SOCKET;
#endif

#if SIZEOF_LONG == 8
#ifndef int32
typedef int int32;
#endif
#ifndef int64
typedef long int64;
#endif
typedef unsigned long uint64;
typedef unsigned int uint32;
#else
#ifndef int32
typedef long int32;
#endif
#ifndef int64
typedef long long int64;
#endif
typedef unsigned long long uint64;
typedef unsigned long uint32;
#endif
#ifndef int16
typedef short int16;
#endif
#ifndef int8
typedef char int8;
#endif
typedef unsigned short uint16;
typedef unsigned char uint8;
#define	atol64(a) atoll(a)
#endif

#define MAX_UINT8  0xFF
#define MAX_UINT16 0xFFFF
#define MAX_UINT32 0xFFFFFFFF
#define MAX_UINT64 0xFFFFFFFFFFFFFFFFLL

#ifndef NULL
#define NULL 0
#endif
#ifndef SOMAXCONN
#define SOMAXCONN 100
#endif
//针对fastdb的定义
#define INT8_IS_DEFINED 1
#ifdef __cplusplus
extern "C" {
#endif
    inline int length(const char* szStr)
    {
        const char* p = szStr;
        while ( *p++ != 0 )
            continue;
        return p - szStr - 1;

    }

    inline char* trimRight(char* toTrim)
    {
        char* p = toTrim + length(toTrim) - 1;
        while ( (*p) == ' ' )
        {
            (*p--) = 0;
        }
        return toTrim;
    }

    inline char* trimLeft(char* toTrim)
    {
        char* p = toTrim;
        while ( (*p) == ' ' )
        {
            (*p++) = 0;
        }
        return p;
    }

    inline char* trim(char* toTrim)
    {
        trimRight(toTrim);
        return trimLeft(toTrim);
    }
#ifdef __cplusplus
}
#endif

enum EDATATYPE
{
    TYPE_INT,
    TYPE_STRING,
    TYPE_DATE,
    TYPE_TIME,
    TYPE_TIMESTAMP,
    TYPE_LONG64,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_BLOB,
    TYPE_SHORT,
    //无符号
    TYPE_UINT8,
    TYPE_UINT16,
    TYPE_UINT32,
    TYPE_UINT64,
    TYPE_UNKNOW=0xFF
};

#define DEFAULT_STRING "DEFAULT_NONE"
#define DEFAULT_INT -999
#define DEFAULT_DOUBLE -999.0
#define DEFAULT_DATETIME CDateTime(1970,1,1,0,0,0,0)
#define DEFAULT_LONG64 -999


#endif