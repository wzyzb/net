#ifndef _TIMERBUSINESS_H_
#define _TIMERBUSINESS_H_

#include <framework/bussiness.h>
#include <framework/daserver.h>
#include <event/timeservice.h>

typedef CRefObject<CDAServer> CDAServerPtr;
/*
广播的发生包
*/
class CSendBusiness : public IBussiness
{
public:
    CSendBusiness(); 
    virtual ~CSendBusiness();
    
    virtual void execute();
	void reset();

protected:
    friend class CDAServer;

    void setSerial(ISerialPtr p);

    void        setCurrent(CCurrentPtr p);
    CCurrentPtr getCurrent();
	virtual uint32 getServantName(){return 0;}

    void         setDAServer(CDAServerPtr p);
    CDAServerPtr getDAServer();

private:
    ISerialPtr   m_pSerial;
    CCurrentPtr  m_pCurrent;
    CDAServerPtr m_pDAServer;
};

typedef CRefObject<CSendBusiness> CSendBusinessPtr;

/*
定时执行业务
*/
class CTimerBusiness : public IBussiness
{
protected :
	ITimeTaskPtr m_pTimeTask;
	CDAServerPtr m_pDAServer;
	void *m_pParam;
	CCurrentPtr  m_pCurrent;

public:
    CTimerBusiness(); 
    virtual ~CTimerBusiness();
    void reset();
    void execute();
	CCurrentPtr getCurrent();
	virtual uint32 getServantName(){return 0;}

protected:
	friend class CDAServer;
	void setDAServer(CDAServerPtr p);
	void setTimeTask(ITimeTaskPtr TimeTask);
	void setParam(void* pParam);
	CDAServerPtr getDAServer();
	//
	void setCurrent(CCurrentPtr pCurrent);
};

typedef CRefObject<CTimerBusiness> CTimerBusinessPtr;

enum DAOPER
{
	DAOPER_ONPROFUPDATE,
	DAOPER_ONSTOP,
	DAOPER_END
};
/*
DAServer的一些hook操作
*/
class CDAServerBusiness : public IBussiness
{
protected :
	IDAServerHandlerPtr m_pHandler;
	CDAServerPtr m_pDAServer;
	CCurrentPtr  m_pCurrent;
	DAOPER m_Type;

public:
	CDAServerBusiness(); 
	virtual ~CDAServerBusiness();
	void reset();
	void execute();
	CCurrentPtr getCurrent();
	virtual uint32 getServantName(){return 0;}

protected:
	friend class CDAServer;
	void setParams(CDAServerPtr pServer,CCurrentPtr pCurrent,IDAServerHandlerPtr pHandler,DAOPER Type);
};

typedef CRefObject<CDAServerBusiness> CDAServerBusinessPtr;

#endif 

