#include <framework/current.h>
#include <framework/daserver.h>
#include <util/objectpool.h>

CCurrent::CCurrent()
{
	reset();
}

CCurrent::~CCurrent()
{
}

void CCurrent::sendSerial(uint32 nSerialNo,uint32 nServantName,ISerial& pSerial)
{
	CPoolObjectSharePtr<CBytesBuffer> sendbuf = CObjectPool<CBytesBuffer>::share_pool::getInstance()->alloc();
	pSerial.serial(*sendbuf.get());
	this->m_pEndPoint->send(nSerialNo, nServantName, *sendbuf.get());
}

void CCurrent::sendResponse(IResponsePtr pResponse)
{
	if( !m_bIsCallLocal )
	{
		//非本地调用，发送到网络上
		CPoolObjectSharePtr<CBytesBuffer> sendbuf = CObjectPool<CBytesBuffer>::share_pool::getInstance()->alloc();
		pResponse->serial(*sendbuf.get());
		this->m_pEndPoint->response(m_nRequestID, m_nServantName, *sendbuf.get());
	}
	else
	{
		m_pDAServer->response(m_nRequestID, pResponse);
	}
}

IEndPointPtr CCurrent::getEndPoint()
{
    return m_pEndPoint;    
}

CSessionPtr CCurrent::getSession()
{
    return m_pSession;
}

uint32 CCurrent::getRequestID()
{
    return m_nRequestID;
}

uint32 CCurrent::getServantName()
{
    return m_nServantName;
}

void CCurrent::setServantName( uint32 nName )
{
	m_nServantName = nName;
}

void CCurrent::closeSession()
{
	m_bCloseSession = true;
}

bool CCurrent::isCloseSession()
{
	return m_bCloseSession;
}

void CCurrent::reset()
{
	m_pDAServer = NULL ;
	m_nRequestID = 0;
	m_nServantName = 0;
	m_pEndPoint = NULL;
	m_pSession = NULL;
	m_bIsCallLocal = false;
	m_bCloseSession = false;
}

void CCurrent::setLocalCall()
{
	m_bIsCallLocal =  true;
}

void CCurrent::setDAServer(CDAServer *pServer)
{
	m_pDAServer = pServer;
}

void CCurrent::setEndPoint(IEndPoint *pEndPoint)
{
	m_pEndPoint = pEndPoint;
}

void CCurrent::setSession(CSession *pSession)
{
	m_pSession = pSession;
}

void CCurrent::setRequestID(uint32 nRequestID)
{
	this->m_nRequestID = nRequestID;
}
