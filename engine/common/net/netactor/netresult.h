#ifndef _NETRESULT_H_
#define _NETRESULT_H_

#include "../../public.h"
#include "../../io/bytesbuffer.h"
#include "../connection.h"
#include "../connector.h"
#include "../acceptor.h"
#include "../../util/refshare.h"
#include <string>

class CNetResult
{
public:
	/**
	 * 构造函数
	 */
	CNetResult();
	/**
	 * 析构函数
	 */
	virtual ~CNetResult();
	/**
	 * 获取结果码
	 */
	int32 getCode() const;
	/**
	 * 获取结果描述
	 */
	const char* getDesc();
	/**
	 * 获取当前的连接
	 */
	IConnectionPtr getConnection() const;
	/**
	 * 获取当前的远程连接
	 */
	IConnectorPtr getConnector() const;
	/**
	 * 获取当前的acceptor
	 */
	IAcceptorPtr getAcceptor() const;
	/**
	 * 获取socket句柄
	 */
	SOCKET getHandle() const;

	/**
	 * 设置结果码
	 */
	void setCode(int32);
	/**
	 * 设置物理连接
	 */
	void setConnection(IConnectionPtr p);
	/**
	 * 设置远程连接
	 */
	void setConnector(IConnectorPtr p);
	/**
	 * 设置acceptor
	 */
	void setAcceptor(IAcceptorPtr p);
	/**
	 * 设置socket句柄
	 */
	void setHandle(SOCKET Sock);

protected :
	int32       m_nCode;
	std::string  m_strDesc;
	IConnectionPtr m_pConnection;
	IConnectorPtr  m_pConnector;
	IAcceptorPtr   m_pAcceptor;
	SOCKET       m_Socket;
};

#endif

