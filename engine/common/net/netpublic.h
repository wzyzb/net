#ifndef _NETPUBLIC_H_
#define _NETPUBLIC_H_

#include <public.h>

enum EERRCODE
{
	//系统错误
	E_SYSTEM = -2,
	E_INVAL = -3,
	E_NOMEM = -4,
};

enum ECOMMMODE
{
    ASYNC_MODE,
    SYNC_MODE
};

//公共函数
/**
 * 设置socket的阻塞模式
 */
int32 setSocketMode(SOCKET Sock, ECOMMMODE Mode);
/**
 * 打印二进制信息
 * @param pBuf 要打印的缓冲区起始地址
 * @param nSize 缓冲区的大小
 * @param pContext 打印的标识，会在打印前后把这段标识打印出来，方便检索
 */
void printfBinBuffer(const char* pBuf, uint32 nSize, const char* pContext = "");

#define DEFAULT_BACKLOG 1024

#ifdef WINDOWS
	#include <winsock2.h>
	typedef int socklen_t;
	#define RETURN_ERROR return WSAGetLastError()
	#define SHUT_RDWR SD_BOTH
#else
	#include <sys/types.h> // for sockaddr_in
	#include <arpa/inet.h> // for inet_addr()
	#include <netinet/in.h> // for netinet/in.h
	#include <sys/epoll.h>
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define RETURN_ERROR return errno
#endif

#endif
