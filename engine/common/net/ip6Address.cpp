#include "ip6Address.h"
#include "string.h"

CIP6Address::CIP6Address(const char* IP, uint16 Port)
{
    /*memset(&m_addr, 0x00, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;
    m_addr.sin6_port = htons(Port);
    inet_pton(AF_INET6,IP,&m_addr.sin6_addr);
    
    memset(m_IP,0x00,sizeof(m_IP));
    strncpy(m_IP,IP,strlen(m_IP));*/
}

const char* CIP6Address::getIP() const
{
    return m_IP;
}

uint16 CIP6Address::getPort() const
{
    //return ntohs(m_addr.sin6_port);
    return 0;
}

const struct sockaddr_in* CIP6Address::getsockaddr() const
{
    return (struct sockaddr_in*) &m_addr;
}