#ifndef _TXLOGROTOCOL_H_
#define _TXLOGROTOCOL_H_

#include <framework/protocol.h>

#ifdef WINDOWS
#pragma pack(push,1)
#else
#pragma pack(push,1)
#endif

#ifdef WINDOWS
#pragma pack(pop)
#else
#pragma pack(pop)
#endif

class CTXLogProtocol : public IProtocol,
					public CRefShare
{
public :
	/**
	 * 构造函数
	 */
	CTXLogProtocol();
	/**
	 * 析构函数
	 */
	~CTXLogProtocol();
	/**
	 * 开始进行协议分析
	 * @param Buffer CBytesBuffer缓冲的数据，会把其中的数据读走
	 * @return void 不做任何返回
	 */
	//void begin(CBytesBuffer& Buffer);
	/**
	 * 是否有下一个包
	 * @param Input IInputStream 输入的数据
	 * @param Body 协议体的内容
	 * @return PROTOCOL_RETCODE PRC_SUCCESS-有下一个包，PRC_ERROR-包分析错误，PRC_NOPACK--没有完整的包
	 */
	PROTOCOL_RETCODE next(IInputStream& Input,uint32& nSerialNo,uint32& nServantName,IOutputStream& Body);
	/**
	 * 打包协议
	 * @return int 0－－成功，-1－－失败
	 */
	PROTOCOL_RETCODE pack(uint32 nSerialNo,uint32 nServantName,const CBytesBuffer& Body,IOutputStream& Output);
	/**
	 * 获取请求协议的应答命令
	 * @param nServantName 请求协议的命令名
	 * @param uint32 应答协议的命令名
	 */
	uint32 getResponseName(uint32 nServantName);
	/**
	 * 是否应答协议
	 */
	bool isResponse(uint32 nServantName);
	/**
	 * 是否心跳协议，如果是，直接跳过，无需处理
	 */
	bool isHeartBeat(uint32 nServantName);
	/**
	 * 根据应答或请求命令，获得请求的命令号
	 * @param nServantName 请求命令
	 * @return uint32 命令号
	 */
	uint32 getServantName(uint32 nServantName);
	/**
	 * 发送心跳信息
	 */
	PROTOCOL_RETCODE packHeartBeat(IOutputStream& Output);
	/**
	 * 创建实例
	 */
	IProtocol* newInstance();


	/**
	 *
	 */
	void incRef();
	/**
	 *
	 */
	bool decRef();
};

#endif
