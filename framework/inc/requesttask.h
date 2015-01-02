#ifndef _REQUESTTASK_H_
#define _REQUESTTASK_H_

#include <framework/bussiness.h>
#include <framework/daserver.h>

class CDAServer;
typedef CRefObject<CDAServer> CDAServerPtr;

class CRequestTask : public IBussiness
{
public:
    CRequestTask(); 
    virtual ~CRequestTask();
    
    virtual void execute();
	/**
	 * 重置各种变量
	 */
	void reset();

    void setServant(IServantPtr p);
    
    void setRequest(IRequestPtr p);

    void        setCurrent(CCurrentPtr p);
    CCurrentPtr getCurrent();
	void		   setServantName(uint32 nServantName);
	virtual uint32 getServantName();

    void         setDAServer(CDAServerPtr p);
    CDAServerPtr getDAServer();

protected:
	friend class CDAServer;

private:
    IServantPtr  m_pServantPtr;
    IRequestPtr  m_pRequest;
    CCurrentPtr  m_pCurrent;
    CDAServerPtr m_pDAServer;
	uint32 m_nServantName;
};

typedef CRefObject<CRequestTask> CRequestTaskPtr;

#endif 

