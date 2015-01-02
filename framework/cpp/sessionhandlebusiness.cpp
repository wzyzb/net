#include "sessionhandlebusiness.h"
CSessionHandleBusiness::CSessionHandleBusiness(CSessionPtr pSession,ISessionHandlerPtr pHandler) : m_pSession(pSession) , m_pSessionHandler(pHandler)
{
}

CSessionHandleBusiness::~CSessionHandleBusiness()
{
}

CCurrentPtr CSessionHandleBusiness::getCurrent()
{
	return NULL;
}

void CSessionHandleBusiness::execute()
{
	m_pSessionHandler->onSessionClose(m_pSession);
}

