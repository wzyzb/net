#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <public.h>
#include <io/outputstream.h>
#include <io/bytesbuffer.h>
#include <util/refshare.h>

enum PROTOCOL_RETCODE
{
	PRC_ERROR = -1,
	PRC_SUCCESS = 0,
	PRC_NOMOREPACK
};

class IProtocol : public IRefShare
{
public :
	virtual ~IProtocol() {}
	/**
	 * 打包协议
	 * @return PROTOCOL_RETCODE PRC_SUCCESS－－成功，PRC_ERROR－－失败
	 */
	virtual PROTOCOL_RETCODE pack(uint32 nSerialNo,uint32 nServantName,const CBytesBuffer& Body,IOutputStream& Output) = 0;
	//保持与数据库操作的相像性
	/**
	 * 是否有下一个包
	 * @param Input IInputStream 输入的数据
	 * @param Body 协议体的内容
	 * @return PROTOCOL_RETCODE PRC_SUCCESS-有下一个包，PRC_ERROR-包分析错误，PRC_NOPACK--没有完整的包
	 */
	virtual PROTOCOL_RETCODE next(IInputStream& Input,uint32& nSerialNo,uint32& nServantName,IOutputStream& Body) = 0;
	/**
	 * 获取请求协议的应答命令
	 * @param nServantName 请求协议的命令名
	 * @param uint32 应答协议的命令名
	 */
	virtual uint32 getResponseName(uint32 nServantName) = 0;
	/**
	 * 是否应答协议
	 */
	virtual bool isResponse(uint32 nServantName) = 0;
	/**
	 * 是否心跳协议，如果是，直接跳过，无需处理
	 */
	virtual bool isHeartBeat(uint32 nServantName) = 0;
	/**
	 * 发送心跳信息
	 */
	virtual PROTOCOL_RETCODE packHeartBeat(IOutputStream& Output) = 0;
	/**
	 * 根据应答或请求命令，获得请求的命令号
	 * @param nServantName 请求命令
	 * @return uint32 命令号
	 */
	virtual uint32 getServantName(uint32 nServantName) = 0;
	//-----理论上，应该放入单独的实例生成类中-----
	/**
	 * 创建实例
	 */
	virtual IProtocol* newInstance() = 0;
};

typedef CRefObject<IProtocol> IProtocolPtr;

class CProtocolFactory
{
private :
	//存储所有的空间
	static map<string,IProtocolPtr> g_Protocols;
	CProtocolFactory();

public :
	/**
	 * 根据协议名，创建对应的协议
	 */
	static IProtocolPtr createProtocol(const char* pProtocolName);
	/**
	 * 注册协议
	 */
	static void regProtocol(const char* pProtocolName,IProtocolPtr pProtocol);
};

#endif
