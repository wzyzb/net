#include "ip4Address.h"


CIP4Address::CIP4Address( const char* IP, uint16 Port )
{
    memset(&m_addr, 0x00, sizeof(m_addr));
    m_addr.sin_family      = AF_INET;
    m_addr.sin_port        = htons((short)(Port));
    m_addr.sin_addr.s_addr = inet_addr(IP);
    snprintf(m_IP,sizeof(m_IP)-1,"%s",IP);
    m_IP[sizeof(m_IP)-1] = 0;
    snprintf(m_Address,sizeof(m_Address),"%s:%u",IP,Port);
    m_Address[sizeof(m_Address)-1] = 0;
}

CIP4Address::CIP4Address()
{
}

CIP4Address::~CIP4Address()
{

}

const char* CIP4Address::getIP() const
{
    return m_IP;
}

uint16 CIP4Address::getPort() const
{
    return ntohs(m_addr.sin_port);
}

const struct sockaddr_in* CIP4Address::getsockaddr() const
{
    return &m_addr;
}

const char* CIP4Address::getString() const
{
    return m_Address;
}

