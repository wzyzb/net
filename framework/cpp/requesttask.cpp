#include <framework/requesttask.h>

CRequestTask::CRequestTask()
{
	reset();
}

CRequestTask::~CRequestTask()
{
    
}

void CRequestTask::reset()
{
    m_pServantPtr = NULL;    
    m_pRequest    = NULL;
    m_pCurrent    = NULL;
    m_pDAServer   = NULL;
	m_nServantName = 0;
}

void CRequestTask::setServant(IServantPtr p)
{
    m_pServantPtr = p;
}

void CRequestTask::setRequest(IRequestPtr p)
{
    m_pRequest = p;
}

void CRequestTask::setCurrent(CCurrentPtr p)
{
    m_pCurrent = p;
}

CCurrentPtr CRequestTask::getCurrent()
{
    return m_pCurrent;
}

void CRequestTask::setServantName(uint32 nServantName)
{
	m_nServantName = nServantName;
}

uint32 CRequestTask::getServantName()
{
	return m_nServantName;
}

void CRequestTask::setDAServer(CDAServerPtr p)
{
    m_pDAServer = p;
}   

CDAServerPtr CRequestTask::getDAServer()
{
    return m_pDAServer;
}

void CRequestTask::execute()
{
	if( NULL != m_pServantPtr && NULL != m_pRequest && NULL != m_pCurrent ) 
	{
		m_pServantPtr->incExecuteTimes();
		IResponsePtr pResponse = m_pServantPtr->execute(m_pRequest, m_pCurrent);
		if( pResponse != NULL )
		{
			m_pCurrent->sendResponse(pResponse);
		}
		if ( m_pCurrent->isCloseSession() && m_pCurrent->getSession() != NULL )
		{
			m_pDAServer->closeSession(m_pCurrent->getSession()->getSessionID());
		}
	}
}
