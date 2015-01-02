#include <framework/timerbusiness.h>

/*

		Class	CSendBusiness

*/
CSendBusiness::CSendBusiness()
{
	reset();
}

CSendBusiness::~CSendBusiness()
{
    
}

void CSendBusiness::reset()
{
    m_pSerial    = NULL;
    m_pCurrent    = NULL;
    m_pDAServer   = NULL;
}

void CSendBusiness::setSerial(ISerialPtr p)
{
    m_pSerial = p;
}

void CSendBusiness::setCurrent(CCurrentPtr p)
{
    m_pCurrent = p;
}

CCurrentPtr CSendBusiness::getCurrent()
{
    return m_pCurrent;
}

void CSendBusiness::setDAServer(CDAServerPtr p)
{
    m_pDAServer = p;
}   

CDAServerPtr CSendBusiness::getDAServer()
{
    return m_pDAServer;
}

void CSendBusiness::execute()
{
	if( NULL != m_pSerial && NULL != m_pCurrent ) 
	{
		IEndPointPtr pendpoint = m_pCurrent->getEndPoint();
		if( NULL == pendpoint )
		{   
			printf("CTimerBusiness::execute() Error: NULL == m_pCurrent->getEndPoint() \n");
			return;
		}
		m_pCurrent->sendSerial(m_pCurrent->getRequestID(),m_pCurrent->getServantName(),*this->m_pSerial.get());
	}
}

/*

		Class	CTimerBusiness

*/
CTimerBusiness::CTimerBusiness()
{
	reset();
}

CTimerBusiness::~CTimerBusiness()
{
}

void CTimerBusiness::reset()
{
	m_pTimeTask = NULL;
	m_pDAServer = NULL;
	m_pParam = NULL;
	m_pCurrent = NULL;
}

void CTimerBusiness::setDAServer(CDAServerPtr p)
{
	m_pDAServer = p;
}

CDAServerPtr CTimerBusiness::getDAServer()
{
	return m_pDAServer;
}

void CTimerBusiness::setTimeTask(ITimeTaskPtr TimeTask)
{
	this->m_pTimeTask = TimeTask;
}

void CTimerBusiness::setParam(void* pParam)
{
	this->m_pParam = pParam;
}

void CTimerBusiness::execute()
{
	m_pTimeTask->execute(m_pParam);
}

CCurrentPtr CTimerBusiness::getCurrent()
{
	return this->m_pCurrent;
}

void CTimerBusiness::setCurrent(CCurrentPtr pCurrent)
{
	m_pCurrent = pCurrent;
}

/*

		Class	CDAServerBusiness

*/
CDAServerBusiness::CDAServerBusiness()
{
}

CDAServerBusiness::~CDAServerBusiness()
{
}

void CDAServerBusiness::reset()
{
	m_pHandler = NULL;
	m_pDAServer = NULL;
	m_pCurrent = NULL;
	m_Type = DAOPER_END;
}

void CDAServerBusiness::execute()
{
	if ( m_pHandler == NULL || m_pDAServer == NULL )
		return ;
	switch ( this->m_Type )
	{
		case DAOPER_ONPROFUPDATE :
			m_pHandler->onProfileUpdate(*m_pDAServer.get());
			break;
	}
}

void CDAServerBusiness::setParams(CDAServerPtr pServer, CCurrentPtr pCurrent, IDAServerHandlerPtr pHandler, DAOPER Type)
{
	m_pHandler = pHandler;
	m_pDAServer = pServer;
	m_pCurrent = pCurrent;
	m_Type = Type;

}

CCurrentPtr CDAServerBusiness::getCurrent()
{
	return this->m_pCurrent;
}
