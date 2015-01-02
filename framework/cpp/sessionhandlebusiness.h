#ifndef _SESSIONHANDLEBUSINESS_H_
#define _SESSIONHANDLEBUSINESS_H_

#include <framework/bussiness.h>
#include <framework/sessionhandler.h>

class CSessionHandleBusiness : public IBussiness
{
private :
	CSessionPtr m_pSession;
	ISessionHandlerPtr m_pSessionHandler;
public :
	CSessionHandleBusiness(CSessionPtr pSession,ISessionHandlerPtr pHandler);
	~CSessionHandleBusiness();
    CCurrentPtr getCurrent();
	virtual uint32 getServantName(){return 0;}
	void execute();
};

#endif
