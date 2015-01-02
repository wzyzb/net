#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include  "netpublic.h"
#include  "../io/inputstream.h"
#include  "../io/outputstream.h"
#include  "address.h"
#include  "../util/refshare.h"

/*
IConnection可能存在多继承，所以只能继承IRefShare这个接口
*/
class IConnection : public IRefShare
{
public:
	IConnection() 
	{
		m_Socket = INVALID_SOCKET; 
		m_pHostAddr   = NULL;
		m_pRemoteAddr = NULL;
	}

	virtual ~IConnection() { }
	
	/**
	 * 设置同步、异步模式
	 */
	virtual int32 setMode(ECOMMMODE Mode) = 0;
	/**
	 * 获取输入流
	 */
	virtual IInputStreamPtr  getInputStream() = 0;  
	/**
	 * 获取输出流
	 */
	virtual IOutputStreamPtr getOutputStream() = 0;
	/**
	 * 关闭连接
	 */
	virtual void close() = 0;
	/**
	 * 获取socket句柄
	 */
	virtual SOCKET getHandle() const { return m_Socket; } 
	/**
	 * 获取本机的地址
	 */
	virtual IAddressPtr getHostAddress() { return m_pHostAddr; }
	/**
	 * 获取远端的地址
	 */
	virtual IAddressPtr getRemoteAddress() { return m_pRemoteAddr; }
	/**
	 * 设置Socket缓冲区的大小
	 */
	virtual int32 setBufSize(int32 nBufSize) = 0;

protected :
	SOCKET      m_Socket;
	IAddressPtr m_pHostAddr;
	IAddressPtr m_pRemoteAddr;
};

typedef CRefObject<IConnection> IConnectionPtr;

#endif
