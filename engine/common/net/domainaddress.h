#ifndef _DOMAINADDRESS_H_
#define _DOMAINADDRESS_H_

#include "netpublic.h"
#include "address.h"

/*
域名地址
*/
class CDomainAddress : public IAddress
{
public:
	CDomainAddress();
	virtual ~CDomainAddress();
	/**
	 * 初始化域名，转换为IP
	 * @param Domain 域名，最大允许199个字节
	 * @param nPort 端口
	 * @return int32 转换结果，0代表成功，其他值代表错误码
	 */
	int32 init(const char* Domain,uint16 nPort);

	virtual const char* getIP() const;
	virtual uint16 getPort() const;
	virtual const struct sockaddr_in* getsockaddr() const;
	/**
	 * 获取一个包含IP、PORT的字符串描述（具有唯一性）
	 * @return const char* 字符串描述
	 */
	const char* getString() const;

private:
	struct sockaddr_in m_addr;
	char m_Address[100];
	char m_IP[20];
	//域名，最大199个字节
	char m_Domain[200];
};


#endif
