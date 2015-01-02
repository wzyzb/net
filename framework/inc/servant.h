#ifndef _SERVANT_H_
#define _SERVANT_H_

#include <util/refshare.h>
#include <framework/request.h>
#include <framework/response.h>
#include <framework/current.h>


/*
统计信息
*/
typedef struct _t_statstics_info
{
	//总执行次数
	uint64 TotalExecTimes;
	//最新的每秒执行次数
	uint32 ExecTimesPerSecond;
	//峰值的每秒执行次数
	uint32 PeekExecTimesPerSecond;

	//当前计数器
	uint32 CurrentExecTimes;
	//上一次执行时间
	uint32 LastExecTime;

	_t_statstics_info()
	{
		memset(this,0,sizeof(struct _t_statstics_info));
	}

	/**
	 * 增加执行次数
	 */
	void incExecute();
	/**
	 * 序列化统计信息
	 */
	void serial(char* pBuff,int BufLen);
}STATISTICS_INFO;

class IServant : public IRefShare
{
public:
	/**
	 * 构造函数
	 */
    IServant() 
    {
    }
	/**
	 * 析构函数
	 */
    virtual ~IServant() {  }
    virtual IResponsePtr execute(IRequestPtr Request, CCurrentPtr pCurrent) = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual IRequestPtr createRequest() = 0;
    virtual void onOpen(IEndPointPtr pEndpoint) {  }
    virtual void onClose(IEndPointPtr pEndpoint, CSessionPtr Session) {  }

	/**
	 * 增加执行次数
	 */
    void incExecuteTimes();
	/**
	 * 获取统计信息
	 */
	STATISTICS_INFO& getStatsticsInfo();

private:
	STATISTICS_INFO m_StatisticsInfo;
};

typedef CRefObject<IServant> IServantPtr;

/*
模板方法，可以把任意类的任意方法变为Servant
*/
template <typename T>
class TServant : public IServant
{
public:
    TServant(T* pClass, IResponsePtr (T::*pfnExecute)(IRequestPtr Request, CCurrentPtr pCurrent), IRequestPtr (T::*pfnCreateRequest)() ) :
    m_pfnExecute(pfnExecute),  m_pfnCreateRequest(pfnCreateRequest)   
    {
        
    }
    
    virtual ~TServant() 
    {
        
    }

    virtual IResponsePtr execute(IRequestPtr Request, CCurrentPtr pCurrent)
    {
        return (m_pClass->*m_pfnExecute)(Request, pCurrent);
    }

    virtual void pause() = 0;
    virtual void stop() = 0;

    virtual IRequestPtr createRequest()
    {
        return (m_pClass->*m_pfnCreateRequest)();
    }

private:
    T* m_pClass;
    IResponsePtr (T::*m_pfnExecute)(IRequestPtr Request, CCurrentPtr pCurrent);
    IRequestPtr (T::*m_pfnCreateRequest)();
};

typedef struct _t_servantinfo
{
	//统计数据
	STATISTICS_INFO StatsticsInfo;
	//servant 的指针
	IServantPtr pServant;
}SERVANTINFO;

class CDAServer;

/*
Servant容器，用于额外提供Servant的运行环境
*/
class IServantContainer : public IRefShare
{
public :
	virtual ~IServantContainer() { }
	/**
	 * 增加servant
	 */
	virtual void addServant(uint32 Name, IServantPtr Servant) = 0;

	virtual IServantPtr getServant(uint32 Name) = 0;
	/**
	 * 初始化容器
	 */
	virtual bool init(CDAServer* pServer) = 0;
	/**
	 * 启动容器
	 */
	virtual void start() = 0;
	/**
	 * 停止环境
	 */
	virtual void stop() = 0;
	/**
	 * 是否有这个servant
	 */
	virtual bool hasServant(uint32 Name) = 0;
	/**
	 * 创建请求处理包
	 */
	virtual IRequestPtr createRequest(uint32 nServantName) = 0;
	/**
	 * 调用servant
	 */
	virtual void callServant(uint32 Name, IRequestPtr Request,CCurrentPtr pCurrent) = 0;
};

typedef CRefObject<IServantContainer> IServantContainerPtr;


#endif 

