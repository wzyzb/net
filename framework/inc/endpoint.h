#ifndef _ENDPOINT_H_
#define _ENDPOINT_H_

#include <net/acceptor.h>
#include <net/connector.h>
#include <net/connection.h>
#include <protocol/encryptor.h>
#include <protocol/compressor.h>
#include <util/refshare.h>
#include <io/bytesbuffer.h>
#include <framework/protocol.h>

#include <io/bytesinputstream.h>

/*
  端点，用于封装通信、协议的处理
*/
class DLLAPI IEndPoint : public CRefShare
{
public:
	enum EENDPOINT_TYPE
	{
		//本地端点
		LOCAL_ENDPOINT,
		//远程端点
		REMOTE_ENDPOINT
	};
	enum EENDPOINT_SOCKTYPE
	{
		//TCP连接
		SOCKTYPE_TCP,
		//UDP连接
		SOCKTYPE_UDP
	};

	enum ECONNECT_STATUS
	{
		//等待HTTP
		STATUS_WAITHTTP,
		//握手成功
		STATUS_SUCCESS,
	};
	/**
	 * 静态函数，获取endpoint的唯一名称
	 */
	static string getName(const string& Address,uint16 nPort,IEndPoint::EENDPOINT_TYPE nType);

	IEndPoint();
	virtual ~IEndPoint();

	/**
	 * 设置通信的加密方式
	 */
	void setEncryptor(IEncryptorPtr Encryptor) { m_pEncryptor = Encryptor; }
	IEncryptorPtr getEncryptor() { return m_pEncryptor; }

	/**
	 * 设置通信的压缩模式
	 */
	void setComprossor(ICompressorPtr Compressor) { m_pCompressor = Compressor; }
	ICompressorPtr getComprossor() { return m_pCompressor; }
	/**
	 * 设置Endpoint使用的协议类型
	 */
	void setProtocol(IProtocolPtr pProtocol) { m_pProtocol = pProtocol ; }
	IProtocolPtr getProtocol() { return m_pProtocol; }

	/**
	 * 获取Endpoint类型－
	 */
	EENDPOINT_TYPE getType();
	/**
	 * 设置Endpoint类型
	 */
	void setType(EENDPOINT_TYPE Type);

	/**
	 * Endpoint的真实通信连接
	 */
	void setConnection(IConnectionPtr Connection);
	/**
	 * 获取endpoint的通信连接
	 */
	IConnectionPtr getConnection();
	/**
	 * 发送信息
	 * @param nSerialNo 消息流水号
	 * @param nServantName 协议命令
	 * @param pBody 消息内容
	 */
	void send(uint32 nSerialNo,uint32 nServantName,CBytesBuffer& pBody);
	/**
	 * 发送心跳信息
	 */
	void sendHeartBeat();
	/**
	 * 应答信息
	 * @param nSerialNo 请求消息的流水号
	 * @param nServantName 请求协议命令
	 * @param pBody 应答的消息内容
	 */
	void response(uint32 nSerialNo,uint32 nServantName,CBytesBuffer& pBody);
	/**
	 * 读取信息
	 * @param nSerialNo 输出参数，消息流水号
	 * @param nServantName 输出参数，协议命令
	 * @return CBytesBufferPtr 消息内容，如果没有可读消息，则返回NULL
	 */
	CBytesBufferPtr next(uint32 &nSerialNo,uint32 &nServantName);

	void setAcceptor(IAcceptorPtr Acceptor);
	IAcceptorPtr getAcceptor();

	void setConnector(IConnectorPtr Connector);
	IConnectorPtr getConnector();
	/**
	 * 设置当前endpoint的网络缓冲区大小，单位：KB
	 */
	void setNetBufferSize(int NetBuffer);
	/**
	 * 获取当前endpoint的网络缓冲区大小，单位：KB
	 */
	int getNetBufferSize() const;
	/**
	 * 获取地址
	 */
	const char* getAddress() const;

	/**
	 * 写入读取到信息
	 * @param pData  数据
	 * @param DataLen 数据长度
	 */
	void writeRecvData(const char* pData,uint32 DataLen);

	/**
	 * 获取endpoint的唯一名称
	 */
	const char* getName();

	/**
	 *
	 */
	void setParam( void* pParam );
	void* getParam();

private:
	IAcceptorPtr   m_pAcceptor;
	IConnectorPtr  m_pConnector;
	IConnectionPtr m_pConnection;
	//地址
	IAddressPtr m_pAddress;
	//加密实例
	IEncryptorPtr  m_pEncryptor;
	//压缩实例
	ICompressorPtr m_pCompressor;
	//业务协议
	IProtocolPtr m_pProtocol;
	//读取缓冲区
	CBytesBufferPtr m_pRecvBuf;
	//发送缓冲区
	CBytesBufferPtr m_pSendBuf;
	//HTTP连接处理缓冲区
	CBytesBufferPtr m_pHttpBuf;
	//输入流
	CBytesInputStream m_InputStream;
	//当前的连接状态
	uint8 m_nStatus;
	//endpoint的类型
	EENDPOINT_TYPE m_nType;
	//名称
	char m_Name[50];
	//网络缓冲区大小
	int m_nNetBufferSize;
	//
	CLightLock m_Lock;
	//
	void* m_pParam;
};

typedef CRefObject<IEndPoint> IEndPointPtr;

class DLLAPI IEndPointHandler : public IRefShare
{
public :
	enum
	{
//		MASK_READ = 0x01,
		MASK_OPEN = 0x02,
		MASK_CLOSE = 0x04,
		MASK_ACCEPT = 0x08,
//		MASK_WRITE = 0x10,
        MASK_NOTRECONNECT = 0x20
	};
	virtual ~IEndPointHandler() = 0;

	/**
	 * 读事件
	 * @param RetCode int32 事件的返回值，0－－成功，-1－－失败
	 * @param EndPoint 产生此事件的EndPoint
	 * @return int32 处理结果，0－－处理成功，-1－－处理失败
	 */
	virtual int32 onRead(int32 RetCode,IEndPointPtr EndPoint) = 0;
	/**
	 * 写事件
	 * @param RetCode int32 事件的返回值，0－－成功，-1－－失败
	 * @param EndPoint 产生此事件的EndPoint
	 * @return int32 处理结果，0－－处理成功，-1－－处理失败
	 */
	virtual int32 onWrite(int32 RetCode,IEndPointPtr EndPoint) = 0;
	/**
	 * 关闭事件
	 * @param RetCode int32 事件的返回值，0－－成功，-1－－失败
	 * @param EndPoint 产生此事件的EndPoint
	 * @return int32 处理结果，0－－处理成功，-1－－处理失败
	 */
	virtual int32 onClose(int32 RetCode,IEndPointPtr EndPoint) = 0;
	/**
	 * 连接建立事件
	 * @param RetCode int32 事件的返回值，0－－成功，-1－－失败
	 * @param EndPoint 产生此事件的EndPoint
	 * @return int32 处理结果，0－－处理成功，-1－－处理失败
	 */
	virtual int32 onOpen(int32 RetCode,IEndPointPtr EndPoint) = 0;
	/**
	 * 连接建立事件
	 * @param RetCode int32 事件的返回值，0－－成功，-1－－失败
	 * @param EndPoint 产生此事件的EndPoint
	 * @return int32 处理结果，0－－处理成功，-1－－处理失败
	 */
	virtual int32 onAccept(int32 RetCode,IEndPointPtr EndPoint) = 0;
};

typedef CRefObject<IEndPointHandler> IEndPointHandlerPtr;

template <typename T>
class TEndPointHandler : public IEndPointHandler, public CRefShare
{
public:
    TEndPointHandler(T* pClass, 
        int32 (T::*pfnOnRead)(int32 ,IEndPointPtr),
        int32 (T::*pfnOnWrite)(int32 ,IEndPointPtr),
        int32 (T::*pfnOnClose)(int32 ,IEndPointPtr),
        int32 (T::*pfnOnOpen)(int32 ,IEndPointPtr),
        int32 (T::*pfnOnAccept)(int32 ,IEndPointPtr)
        ) :
        m_pClass(pClass),
        m_pfnOnRead(pfnOnRead),
        m_pfnOnWrite(pfnOnWrite),
        m_pfnOnClose(pfnOnClose),
        m_pfnOnOpen(pfnOnOpen),
        m_pfnOnAccept(pfnOnAccept)
    {
    }

    virtual ~TEndPointHandler()
    {
    }

    virtual int32 onRead(int32 RetCode,IEndPointPtr EndPoint)
    {
        if ( m_pfnOnRead != NULL )
            return (m_pClass->*m_pfnOnRead)( RetCode, EndPoint );
        else
            return 0;
    }

    virtual int32 onWrite(int32 RetCode,IEndPointPtr EndPoint)
    {
        if ( m_pfnOnWrite != NULL )
            return (m_pClass->*m_pfnOnWrite)( RetCode, EndPoint );
        else
            return 0;
    }
	
    virtual int32 onClose(int32 RetCode,IEndPointPtr EndPoint)
    {
        if ( m_pfnOnClose != NULL )
            return (m_pClass->*m_pfnOnClose)( RetCode, EndPoint );
        else
            return 0;
    }
	
    virtual int32 onOpen(int32 RetCode,IEndPointPtr EndPoint)
    {
        if ( m_pfnOnOpen != NULL )
            return (m_pClass->*m_pfnOnOpen)( RetCode, EndPoint );
        else
            return 0;
    }

	virtual int32 onAccept(int32 RetCode,IEndPointPtr EndPoint)
    {
        if ( m_pfnOnAccept != NULL )
            return (m_pClass->*m_pfnOnAccept)( RetCode, EndPoint );
        else
            return 0;
    }

protected:
    void incRef()
    {
        CRefShare::incRef();
    }

    bool decRef()
    {
        return CRefShare::decRef();
    } 

private:
    T* m_pClass;

    int32 (T::*m_pfnOnRead)(int32,IEndPointPtr);
    int32 (T::*m_pfnOnWrite)(int32,IEndPointPtr);
    int32 (T::*m_pfnOnClose)(int32,IEndPointPtr);
    int32 (T::*m_pfnOnOpen)(int32,IEndPointPtr);
    int32 (T::*m_pfnOnAccept)(int32,IEndPointPtr);
};

#endif 

