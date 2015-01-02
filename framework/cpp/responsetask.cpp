#include <framework/responsetask.h>
#include <framework/debuginfo.h>

CResponseTask::CResponseTask()
{
	reset();
}

CResponseTask::~CResponseTask()
{
}

void CResponseTask::setParam(IResponseHandlerPtr pResponseHandler,IResponsePtr pResponse,CCurrentPtr pCurrent,void *pParam,CDAServerPtr pServer)
{
	m_pServer = pServer;
	m_pParam = pParam;
	m_pCurrent = pCurrent;
	m_pResponse = pResponse;
	m_pResponseHandler = pResponseHandler;
}

void CResponseTask::reset()
{
	m_nServantName = 0;
	m_pServer = NULL;
	m_pCurrent = NULL;
	m_pResponse = NULL;
	m_pResponseHandler = NULL;
	m_pParam = NULL;
}

CCurrentPtr CResponseTask::getCurrent()
{
	return m_pCurrent;
}

void CResponseTask::setServantName(uint32 nServantName)
{
	m_nServantName = nServantName;
}

uint32 CResponseTask::getServantName()
{
	return m_nServantName;
}

void CResponseTask::execute()
{
	if ( m_pResponseHandler != NULL )
		m_pResponseHandler->onResponse( m_pResponse, m_pCurrent, m_pParam );
	if ( m_pCurrent != NULL && m_pCurrent->isCloseSession() && m_pCurrent->getSession() != NULL )
	{
		m_pServer->closeSession(m_pCurrent->getSession()->getSessionID());
	}
}

