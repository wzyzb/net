#ifndef _IP4ADDRESS_H_
#define _IP4ADDRESS_H_

#include "netpublic.h"
#include "address.h"

class CIP4Address : public IAddress
{
public:
    CIP4Address(const char* IP, uint16 Port);
    virtual ~CIP4Address();

    virtual const char* getIP() const;
    virtual uint16 getPort() const;
    virtual const struct sockaddr_in* getsockaddr() const;
	/**
	 * 获取一个包含IP、PORT的字符串描述（具有唯一性）
	 * @return const char* 字符串描述
	 */
	const char* getString() const;

private:
	CIP4Address();
	struct sockaddr_in m_addr;
	char m_Address[50];
	char m_IP[20];
};

#endif

