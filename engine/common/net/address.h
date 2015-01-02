#ifndef _ADDRESS_H_
#define _ADDRESS_H_

#include "netpublic.h"
#include "../util/refshare.h"

class IAddress
{
public:
    IAddress() {}
    virtual ~IAddress() {}
	/**
	 * 获取IP
	 */
    virtual const char* getIP() const = 0;
	/**
	 * 获取端口
	 */
    virtual uint16 getPort() const = 0;
	/**
	 * 获取sockaddr的结构
	 */
    virtual const struct sockaddr_in* getsockaddr() const = 0;
	/**
	 * 获取一个包含IP、PORT的字符串描述（具有唯一性）
	 * @return const char* 字符串描述
	 */
	virtual const char* getString() const = 0;
};

typedef TRefPointer<IAddress> IAddressPtr;

/**
 * 获取socket的本机地址
 */
IAddressPtr getSockAddr(SOCKET Sock);
/**
 * 获取socket的对端地址
 */
IAddressPtr getPeerSockAddr(SOCKET Sock);

#endif

