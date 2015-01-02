#include <framework/endpointtask.h>
#include <util/drivermanager.h>
#include <zwencryptor.h>
#include <framework/zwprotocol.h>

//ÅÐ¶ÏÊÇ·ñIPµØÖ·
bool CEndPointTask::isIP(const char* pHost)
{
    return ( INADDR_NONE != inet_addr(pHost) );
}

void CEndPointTask::setCurrent( CCurrentPtr p )
{
    m_pCurrent = p;
}

CCurrentPtr CEndPointTask::getCurrent()
{
    return m_pCurrent;
}

void CEndPointTask::setServantName( uint32 nServantName )
{
    m_nServantName = nServantName;
}

uint32 CEndPointTask::getServantName()
{
    return m_nServantName;
}

void CEndPointTask::setDAServer( CDAServerPtr p )
{
    m_pDAServer = p;
}

CDAServerPtr CEndPointTask::getDAServer()
{
    return m_pDAServer;
}

void CEndPointTask::execute()
{
    if ( getPort() == 0 || getHost() == "" )
    {
        throw CDAException(-1, "CDAServer::initService|Invalid host or port!");
        return ;
    }   

    if ( ! isIP(getHost().c_str()) )
    {
        string hostip = m_pDAServer->getProfile().getString("machine settings",getHost().c_str(),"");
        if ( hostip == "" )
            log_fatal("no machine[%s]'s setting",getHost().c_str());
        else
            setHost( hostip );
    }
	//todo
    //m_pDAServer->addEndpoint( getHost(),getPort(),IEndPoint::REMOTE_ENDPOINT,getUseEncrypt(),512, IEndPointHandler::MASK_OPEN|IEndPointHandler::MASK_CLOSE, getEndPointHandler() );
}

void CEndPointTask::setHost( const std::string& strHost )
{
    m_strHost = strHost;
}

std::string CEndPointTask::getHost()
{
    return m_strHost;
}

void CEndPointTask::setPort( uint16 nPort )
{
    m_nPort = nPort;
}

uint16 CEndPointTask::getPort()
{
    return m_nPort;
}

void CEndPointTask::setUseEncrypt( bool b )
{
    m_bUseEncrypt = b;
}

bool CEndPointTask::getUseEncrypt()
{
    return m_bUseEncrypt;
}

void CEndPointTask::setMask( uint32 nMask )
{
    m_nMask = nMask;
}

uint32 CEndPointTask::getMask()
{
    return m_nMask;
}

void CEndPointTask::setEndPointHandler( IEndPointHandlerPtr pEndPointHandler )
{
    m_pEndPointHandler = pEndPointHandler;
}

IEndPointHandlerPtr CEndPointTask::getEndPointHandler()
{
    return m_pEndPointHandler;
}

CEndPointTask::CEndPointTask()
{
	m_pDAServer = NULL;
	m_pParam = NULL;
}

CEndPointTask::~CEndPointTask()
{

}

void CEndPointTask::setParam( void* Param )
{
	m_pParam = Param;
}

void* CEndPointTask::getParam()
{
	return m_pParam;
}
