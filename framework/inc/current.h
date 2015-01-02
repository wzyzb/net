#ifndef _CURRENT_H_
#define _CURRENT_H_

#include <framework/endpoint.h>
#include <framework/session.h>
#include <framework/response.h>

class CDAServer;
typedef CRefObject<CDAServer> CDAServerPtr;

class CCurrent : public CRefShare
{
public:
	/**
	 * 构造函数
	 */
	CCurrent();
	/**
	 * 析构函数
	 */
	virtual ~CCurrent();
	/**
	 * 设置请求ID
	 */
	void setRequestID(uint32 nRequestID);
	/**
	 * 获取当前请求的id
	 */
	uint32 getRequestID();
	/**
	 *
	 */
	void setServantName(uint32 nServantName);
	/**
	 * 获取servant名
	 */
	uint32 getServantName();
	/**
	 * 设置物理端点的实例
	 */
	void setEndPoint(IEndPoint* pEndPoint);
	/**
	 * 此请求对应的物理端点
	 */
	IEndPointPtr getEndPoint();
	/**
	 * 设置会话
	 */
	void setSession(CSession* pSession);
	/**
	 * 此请求对应的会话标识
	 */
	CSessionPtr getSession();
	/**
	 * 返回应答包
	 */
	void sendResponse(IResponsePtr pResponse);
	/**
	 * 直接发送序列化包
	 */
	void sendSerial(uint32 nSerialNo,uint32 nServantName,ISerial& pSerial);
	/**
	 * 关闭当前会话（断开连接）
	 * 在当前业务完毕后关闭
	 */
	void closeSession();
	/**
	 * 是否需要关闭会话
	 */
	bool isCloseSession();
	/**
	 * 设置服务器
	 */
	void setDAServer(CDAServer* pServer);
	/**
	 * 重置数据
	 */
	void reset();
	/**
	 * 设置本地调用
	 */
	void setLocalCall();

private:
	bool m_bIsCallLocal;
	IEndPointPtr m_pEndPoint;
	CSessionPtr m_pSession;
	uint32 m_nRequestID;
	CDAServerPtr m_pDAServer;
	uint32 m_nServantName;
	bool m_bCloseSession;
};

#include <util/objectpool.h>
typedef CPoolObjectSharePtr<CCurrent> CCurrentPtr;

#endif

