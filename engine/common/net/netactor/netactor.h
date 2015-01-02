#ifndef _NETACTOR_H_
#define _NETACTOR_H_

#include "../acceptor.h"
#include "../connector.h"
#include "nethandler.h"
#include "../util/refshare.h"

class INetActor : public CRefShare
{
public:
    INetActor()
    {

    }

    virtual ~INetActor() 
    {
    
    }

    /**
     *
     */
    virtual int32 init() = 0;
       
    /**
     *
     */
    virtual int32 run() = 0;

    /**
     *
     */
    virtual int32 stop() = 0;

    /**
     *
     */
    virtual int32 addAcceptor(IAcceptorPtr Acceptor, INetHandlerPtr Handler, void* Param) = 0;

    /**
     *
     */
    virtual int32 addConnector(IConnectorPtr Connector, INetHandlerPtr Handler, void* Param) = 0;
	/**
	 * 删除连接器
	 */
	virtual int32 removeConnector(IConnectorPtr Connector) = 0;
	/**
	 * 删除侦听器
	 */
	virtual int32 removeAcceptor(IAcceptorPtr Acceptor) = 0;
    /**
     * 注册connection
	 * @param pConnection 要注册的连接，输入输出参数，会被修改为注册后的合法连接
	 * @return int32 是否成功，0－－成功，其他－系统错误码
     */
    virtual int32 registry(IConnectionPtr& pConnection, INetHandlerPtr Handler, uint32 nMask, void* Param) = 0;
    /**
     *
     */
    virtual int32 unregistry(IConnectionPtr& pConnection) = 0;
};

typedef CRefObject<INetActor> INetActorPtr;


#endif
