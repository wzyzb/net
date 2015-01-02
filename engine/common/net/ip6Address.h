#ifndef _IP6ADDRESS_H_
#define _IP6ADDRESS_H_

#include "address.h"

class CIP6Address : public IAddress
{
public:
    CIP6Address(const char* IP, uint16 Port);
    virtual ~CIP6Address(){};

    virtual const char* getIP() const;
    virtual uint16 getPort() const;
    virtual const struct sockaddr_in* getsockaddr() const;

private:
    CIP6Address();
    struct sockaddr_in m_addr;
    char m_IP[32];
};

#endif
