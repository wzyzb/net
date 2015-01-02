#include "netresult.h"

CNetResult::CNetResult()
{
    m_nCode = 0;
    m_Socket = INVALID_SOCKET;
}

CNetResult::~CNetResult()
{
	
}

int32 CNetResult::getCode() const
{
    return m_nCode;
}

void CNetResult::setCode( int32 nCode )
{
    m_nCode = nCode;
}

const char* CNetResult::getDesc()
{
	m_strDesc = strerror( getCode() );    
	return m_strDesc.c_str();
}

IConnectionPtr CNetResult::getConnection() const
{
    return m_pConnection;
}

void CNetResult::setConnection(IConnectionPtr p)
{
    m_pConnection = p;
}

IConnectorPtr CNetResult::getConnector() const
{
    return m_pConnector;
}

void CNetResult::setConnector(IConnectorPtr p)
{
    m_pConnector = p;
}

IAcceptorPtr CNetResult::getAcceptor() const
{
    return m_pAcceptor;
}

void CNetResult::setAcceptor(IAcceptorPtr p)
{
    m_pAcceptor = p;
}

SOCKET CNetResult::getHandle() const
{
    return m_Socket;
}

void CNetResult::setHandle(SOCKET Sock)
{
    m_Socket = Sock;
}

