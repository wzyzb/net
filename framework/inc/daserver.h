#ifndef _DASERVER_H_
#define _DASERVER_H_

#include <framework/framepublic.h>
#include <framework/context.h>
#include <framework/endpoint.h>
#include <net/netactor/nethandler.h>
#include <framework/request.h>
#include <framework/response.h>
#include <framework/servant.h>
#include <map>
#include <net/netactor/reactor.h>
#include <net/netactor/nethandler.h>
#include <net/netactor/netactordriver.h>
#include <event/timeservice.h>
#include <net/connection.h>
#include <framework/serial.h>
#include <framework/responsehandler.h>
#include <list>
#include <vector>
#include <framework/sessionhandler.h>
#include <framework/current.h>
#include <framework/bussiness.h>
#include <framework/commonptl.h>
#include <event/timeservice.h>
#include <util/activeobject.h> // TActiveObject
#include <framework/daexception.h>

#include <net/driver/tcpsocketdriver.h>
#include <net/driver/ip4address.h>
#include <net/netactor/reactor.h>
#include <util/profile.h>
#include <util/datetime.h>
#include <util/lightlock.h>
#include <util/tlobject.h>

// Ret Header Ret Code
#ifndef SERVANT_NOTFOUND
    #define SERVANT_NOTFOUND 0x00000004
#endif

#define HANDLER_TIMEOUT_INFINITE -1

#define PREFIX_RESPONSE 0X80000000
//获取 Servant Name 的前缀
#define GET_SERVANT_PREFIX(ServantName) ( ( ( ServantName ) >> 16 ) & 0xFFFF )

/*

Class	CEmptyRequest

*/
class CEmptyRequest : public IRequest
{
public:

    virtual bool serial(char *pOut, int &Length){ return true; }
    virtual bool serial(IOutputStreamPtr Output) { return true;	}
    virtual bool unserial(const char *pIn, int Length) { return true; }
    virtual bool unserial(IInputStreamPtr Input) {	return true; }
};

/*
 EndPoint信息管理器
*/
class DLLAPI CEndPointManager 
{
private:
	typedef list<CSTLRefObject<IEndPoint> > ENDPOINT_LIST;
	// 两个 map 保存的内容相同, 为了在不同的情况下提高查找的效率
	std::map<uint32,ENDPOINT_LIST> m_mapServantInfo_name; // 采用 ServantName  作为主键保存的 map
	std::map<std::string,IEndPointPtr> m_mapServantInfo_ip;   // 采用 ip:port 形式 作为主键保存的 map
	CLightLock m_Lock;
public:
	CEndPointManager();
	~CEndPointManager();
	/**
	 * 加入新的EndPoint
	 * @param pEndPoint 要加入的EndPoint
	 */
	void registry(IEndPointPtr pEndPoint);
	/**
	 * 为EndPointb注册ServantName前缀
	 * @param pEndPoint 处理的EndPoint，如果EndPoint已存在，则不会重复保存
	 * @param nServantNamePrefix 
	 */
	void registry(IEndPointPtr pEndPoint,uint32 nServantNamePrefix);
	/**
	 * 删除指定endpoint的信息
	 */
	void unregistry(const char* pEndPointName);

	/**
	 * 根据EndPointName查找对应的EndPoint
	 */
	IEndPointPtr find(const char* pEndPointName);
	/**
	 * 根据ServantName查找对应的EndPoint
	 */
	IEndPointPtr find(uint32 nServantNamePrefix);
	/**
	 * 检查这个endpoint是否有这个prefix
	 * @retur bool 是否存在
	 */
	bool isEndPointPrefix(uint32 nServantNamePrefix,IEndPointPtr pEndPoint);
};

class CResponseTask;
//环境堆栈
typedef std::stack<CCurrentPtr> CURRENT_STACK;

//DAServer的处理
class IDAServerHandler : public IRefShare
{
public :
	/**
	 * 服务器停止
	 */
	virtual void onServerStop() = 0;
	/**
	 * 配置文件更新
	 */
	virtual void onProfileUpdate(CDAServer& Server) = 0;
};

typedef CRefObject<IDAServerHandler> IDAServerHandlerPtr;

class DLLAPI CDAServer : public INetHandler, public CRunnable
{
protected:
	friend class CResponseTask;
	//是否已完成资源的初始化
	bool m_bIsInited;
	// Server的配置文件
	CProfile* m_pProfile;
	//命令控制端口
	int m_nSvrPortArgc;
	//定时服务
	CTimeService m_TimeSvr;
	//流水号
	int32 m_nSerialNo;
	//网络模型
	IReactorPtr m_pReactor;
	//本地的servant
	std::map<uint32, SERVANTINFO> m_mapServants;
	//侦听端口允许执行的servant前缀
	std::map<uint16, std::vector<uint16> > m_mapServantPrefixs;
	//servant的容器
	vector<IServantContainerPtr> m_ServantContainers;
	//空闲超时，单位：毫秒
	int32 m_nIdleTimeOut;
	//servant执行时间，单位：毫秒
	uint64 m_nServantExecuteTime;

	//-------------超时重连--------------------
	//超时事务相关的锁
	CBaseLock m_TimeoutLock;
	//超时事务－－重连列表
	typedef list<CSTLRefObject<IEndPoint> > ENDPOINT_LIST;
	ENDPOINT_LIST m_ReConnList;
	//超时计数器
	uint32 m_nTimeoutCount;
	//当前时间
	uint32 m_nCurrentTime;

	//---------------网络相关的变量-------------------
	// 连接信息, 每个连接只有一个 Session & RecvBuf
	typedef struct _t_connection_data
	{
		//对应的物理连接
		IConnectionPtr pConnection;
		//会话信息
		CSessionPtr pSession;
		//服务端点
		IEndPointPtr pEndPoint;
		//最近读取时间，单位：毫秒
		uint64 nLastReadTime;
	}CONNECTION_DATA, *LPCONNECTION_DATA;
	//保存所有客户端的连接 <SOCKET, CONNECTION>
	//保存所有的会话信息，key=SESSIONID
	std::map<uint32, LPCONNECTION_DATA> m_Sessions;
	//会话处理器
	typedef list<CSTLRefObject<ISessionHandler> > SESSIONHANDLER_LIST;
	SESSIONHANDLER_LIST m_SessionHandlers;

	//EndPoint的保存以及各种信息
	CEndPointManager m_EndPointManager;

	typedef struct _t_handle_info
	{
		IEndPointHandlerPtr pEndPointHandler;
		uint32 nMask;
		_t_handle_info()
		{
			nMask = 0;
			pEndPointHandler = NULL;
		}
	}HANDLE_INFO;
	//对部署于某些servant的
	std::map<uint32,HANDLE_INFO> m_ServantHandlers;
	typedef struct _t_endpoint_info
	{
		LPCONNECTION_DATA pConnData;
		HANDLE_INFO HandleInfo;
	}ENDPOINT_INFO,*LPENDPOINT_INFO;
	//资源保护锁
	CLightLock m_Lock;
	//endpointname-handler
	std::map<string,ENDPOINT_INFO> m_EndPoints;

	//-------------业务线程相关变量--------------
	//业务线线程
	TActiveObject<CDAServer, void*> m_Thread;
	//保护连接数据的锁
	CLightLock m_ConnDataLock;
	//业务队列同步锁
	CBaseLock m_BusinessLock;
	//业务队列(所有业务处理都需要放入此队列，由业务线程处理)
	typedef std::list<CSTLRefObject<IBussiness> > BUSINESS_LIST;
	BUSINESS_LIST m_lstBusiness;
	//业务线程运行标识
	bool m_bIsRun;
	typedef struct _t_RESPONSE_DATA
	{
		uint32 nServantName;
		IResponseHandlerPtr	pResponseHandle;
		void* pParam;
		CCurrentPtr pCurrent;
		//对response的超时检查
		CDateTime BeginTime;
		//超时限制，单位－秒
		uint32 nTimeOut;
	}RESPONSE_DATA, *LPRESPONSE_DATA;
	//保存所有的应答与对应的处理句柄 RequestID, LPRESPONSE_DATA
	std::map<uint32, RESPONSE_DATA> m_mapResponsees;
	//时间处理
	IDAServerHandlerPtr m_pServerHandler;

	//servant的current堆栈
	// 调用 Servant.execute() 之前将Current 保存到堆栈
	//如果多线程调用callservant，会导致current混乱，需要使用本地变量来区分
	CTlObject m_Currents;

protected:
    //----------------INetHandler 接口函数--------------------------
    virtual void onRead(CReadResult& result, void* Param);
    virtual void onWrite(CWriteResult& result, void* Param);
    virtual void onOpen(CNetResult& result, void* Param);
    virtual void onClose(CNetResult& result, void* Param);
    virtual void onAccept(CNetResult& result, void* Param);
    //---------------类内部调用函数---------------------
    /**
     * 业务线程处理函数
     */
    void doBusiness(void* pParam);
	/**
	 * 获取sessionid
	 * @param nSessionID 输出参数，得到的sessionid
	 * @return bool 是否获取成功
	 */
	bool getSessionID(uint32& nSessionID);
	/**
	 * 配置文件有变更
	 */
	void onProfileUpdate();

protected:
	/**
	 * 生成requestID
	 */
    int32 makeRequestID();
	/**
	 * 判断端口port是否处理servant前缀的请求
	 */
	bool isLegalServantPrefix(uint16 port, uint16 servantprefix);
    /**
     * 应答servant找不到的错误信息，回复包包含[回复头], [回复头]的ErrorCode为 NotFoundServant
     */
    bool respondNotFoundServant(uint32 nSerialno, uint32 nServantName, IEndPointPtr pEndPoint);
    /**
     * 处理回复包
     */ 
    virtual bool processResponse(uint32 nSerialNo, uint32 nServantName, LPCONNECTION_DATA pConnData, IInputStream& pBody);
    /**
     * 处理请求包
     */
    bool processRequest(uint32 nSerialNo, uint32 nServantName, LPCONNECTION_DATA pConnData, IInputStream& pBody);
	/**
	 *  初始化 DAServer 的所有服务，包括初始化监听的服务(Acceptor)，远程服务(Connector)
	 * @return bool 是否成功
	 */
	bool initService();
    /**
     * 执行定时事件
     */
    int executeTimeTask( void *pParam );
	/**
	 * 超时处理
	 */
	int onTimeout(void *pParam);
	/**
	 * 定时任务完成
	 */
	int finishTimeout(void *pParam);
	/**
	 * 检查是否有业务超时
	 */
	int checkResponseTimeout(void *pParam);
	/**
	 * 放入到业务队列来处理
	 */
	void putBusiness(IBussinessPtr pBusiness);
	/**
	 * 放入批量的业务数据
	 */
	void putBusinessList(BUSINESS_LIST& BusinessList);
	/**
	 * 获取Current
	 * @param nSessionID 如果为0，代表根据当前堆栈信息获取current，如果不为0，则获取指定session的信息来创建current
	 */
	CCurrentPtr createCurrent(uint32 nSessionID,uint32 nRequestID,uint32 nServantName);
	/**
	 * 添加endpoint，并设置对应的处理器
	 * @param Address 地址
	 * @param nPort 端口
	 * @param EndPointType endpoint类型，详见IEndPoint::EENDPOINT_TYPE的枚举值
	 * @param SockType endpoint的链接类型，详见IEndPoint::EENDPOINT_SOCKTYPE的枚举值
	 * @param isUseEncrypt 是否使用加密
	 * @return IEndPointPtr 增加成功后，返回对应的指针，如果返回NULL，则证明增加失败
	 */
	IEndPointPtr addEndpoint(const string& Address,uint16 nPort,IEndPoint::EENDPOINT_TYPE EndPointType,
		IEndPoint::EENDPOINT_SOCKTYPE SockType,IProtocolPtr pProtocol,IEncryptorPtr pEncryptor = NULL,
		int nNetBuffSize = 512,uint32 nMask = 0,IEndPointHandlerPtr pEndPointHandler = NULL);
	/**
	 * 移除endpoint，并且删除对应的所有处理
	 * @return IEndPointPtr 删除成功后，返回对应的指针，如果返回NULL，证明删除失败（无此endpoint）
	 */
	IEndPointPtr removeEndpoint(const string& Address,uint16 nPort,IEndPoint::EENDPOINT_TYPE EndPointType);

	CDAServer();
	friend class CRefObject<CDAServer>;
	friend class CRefObject<CCurrent>;
	friend class CCurrent;
	friend class CRequestTask;
	friend class CDBServantService;

public:
	virtual ~CDAServer();

	//--------------------初始化接口-----------------------------
	/**
	 * 初始化函数, 所有函数调用之前必须先调用 init()
	 */
	bool init();

	/**
	 *
	 */
	bool uninit();

	/**
	 * 开始运行 DAServer
	 */    
	virtual void run();

	/**
	 * 停止运行 DAServer
	 */
	virtual void stop();

	/**
	 * 添加 Servant
	 * @param ContainerID 容器id，这个id是addServantContainer的返回值，为0代表加入到daserver自身的servant容器中
	 */
	void addServant(uint32 Name, IServantPtr Servant,int ContainerID = 0);

	IServantPtr getServant(uint32 Name,int ContainerID = 0);

	/**
	 * 添加servant运行容器
	 * @return int 如果>0，则为container的id，如果<0，代表失败
	 */
	int addServantContainer(IServantContainerPtr pServantContainer);

	/**
	 *  添加 EndPoint,并处理对应的事件
	 * @param EndPoint 服务端点
	 * @param nMask ，要处理的事件标识，
	 * @param pEndPointHandler 事件处理句柄
	 */ 
	//void addEndpoint(IEndPointPtr EndPoint,uint32 nMask = 0,IEndPointHandlerPtr pEndPointHandler = NULL);

	/**
	 * 侦听指定servant的endpoint操作
	 * @param nServantNamePrefix servantname前缀
	 * @param IEndPointHandlerPtr 处理器
	 * @return 
	 */
	void registry(uint32 nServantNamePrefix,uint32 nMask,IEndPointHandlerPtr pEndPointHandler);

	/**
	 *  添加定时任务
	 */
	TASKHANDLE addTimeTask(ITimeTaskPtr TimerTask, int32 nDelay, int32 nInterval, void *Param, int16 nRepeatTimes);    

	//------------------------------提供给业务Servant调用的接口--------------------------------------
	/**
	 * 发给指定的客户端
	 * 
	 */
	void send(uint32 SessionID, uint32 nServantName, ISerialPtr pMessage,IResponseHandlerPtr pResponseHandler = NULL,uint32 nTimeOut = 0, void* Param = NULL);

	/**
	 * 向所有客户端广播消息
	 */
	void broadcast(uint32 nName, ISerialPtr Message);
	/**
	 * 广播给所有拥有此ServantName的EndPoint
	 */
	//void broadcastServant(uint32 nServantName, IRequestPtr Request, IResponseHandlerPtr Response);

	/**
	 * call by servant
	 * @param nTimeout －－ 超时时间，如果为0，则为默认值－10
	 * @param nSessionID -- 使用某个session作为callservant的session，如果为0，则取当前的session
	 */
	void callServant(uint32 Name, IRequestPtr Request, IResponseHandlerPtr Response, uint32 nTimeOut, void* Param, const char* pszEndPoint,uint32 nSessionID = 0);
	// 提供给业务Servant调用的接口 -- end

	/**
	 * 获取string类型的配置数据
	 */
	std::string getSetting(const std::string& strSessionName, const std::string& strKeyName, const std::string& strDefValue);
	/**
	 * 获取uint32类型的配置数据
	 */
    uint32 getSettingUint32(const std::string& strSessionName, const std::string& strKeyName, const std::string& strDefValue);

	/**
	 * 获取当前进程配置文件，已经把额外的配置文件也一起合并了。
	 */
	CProfile& getProfile();
	/**
	 * 注册对session的管理
	 */
	void addSessionHandler(ISessionHandlerPtr pSessionHandler);
	/**
	 * 关闭session的连接（马上关闭）
	 * @param nSessionID －－ 会话id
	 */
	void closeSession(uint32 nSessionID);
	/**
	 * 设置服务器的事件处理器
	 */
	void setServerHandler(IDAServerHandlerPtr pHandler);
	/**
	 * 读取配置文件
	 */
	bool loadProfile();
	//---------------servant container调用----------
    /**
     * 本地调用时处理回复, 由 CCurrent及ServantContainer调用
     */
    bool response(uint32 nRequestID, IResponsePtr pResponse);
	/**
	 * 获取当前线程的栈对象
	 */
	CURRENT_STACK& getCurrentStack();
	/**
	 * 删除当前线程的堆栈
	 */
	void destoryCurrentStack();

	/**
	 * 增加远程调用的节点
	 */
	bool addCallEndpoint(const string& Address,uint16 nPort,const string& ServantList,IProtocolPtr pProtocol = NULL,bool isUseEncrypt = false,int nNetBuffSize = 512);
	/**
	 * 删除远程调用的节点
	 */
	bool removeCallEndpoint(const string& Address,uint16 nPort);

	//获取sessionID
	bool getSessionIDByEndpointName(string strName,uint32& nSessionID);
};

typedef CRefObject<CDAServer> CDAServerPtr;

/*

Class	CEmptyResponse

*/
class CEmptyResponse : public IResponse
{
public:
	uint32 m_nRetCode;

	virtual bool serial(char *Output, int &Length){return true;}
	virtual bool serial(IOutputStream& Output)
	{
		writeuint32(Output, m_nRetCode);
		return true;
	}
	virtual bool unserial(const char *Input, int Length){return true;}
	virtual bool unserial(IInputStream& Input)
	{
		readuint32(Input, m_nRetCode);
		return true;
	}
	CEmptyResponse()
	{
		m_nRetCode = -1;
	}
	CEmptyResponse(const CEmptyResponse &t)
	{
		m_nRetCode = t.m_nRetCode;
	}
	CEmptyResponse& operator = (const CEmptyResponse& t)
	{
		m_nRetCode = t.m_nRetCode;
		return *this;
	}
	virtual ~CEmptyResponse()
	{
	}
};


#endif 
