#ifndef _RESPONSETASK_H_
#define _RESPONSETASK_H_

#include <framework/bussiness.h>
#include <framework/daserver.h>
#include <framework/response.h>

class CResponseTask : public IBussiness
{
private:
	uint32 m_nServantName;
	CDAServerPtr m_pServer;
	CCurrentPtr m_pCurrent;
	IResponsePtr m_pResponse;
	IResponseHandlerPtr m_pResponseHandler;
	void *m_pParam;

public:
	CResponseTask();

	virtual ~CResponseTask();
	/**
	 *
	 */
	void reset();

	virtual void execute();
	/**
	 *
	 */
	void setParam(IResponseHandlerPtr pResponseHandler,IResponsePtr pResponse,CCurrentPtr pCurrent,void* pParam,CDAServerPtr pServer);
	virtual CCurrentPtr getCurrent();
	void setServantName(uint32 nServantName);
	virtual uint32 getServantName();

protected:

};

typedef CRefObject<CResponseTask> CResponseTaskPtr;

#endif 

