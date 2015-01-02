#ifndef _NETHANDLER_H_
#define _NETHANDLER_H_

#include "netresult.h"
#include "readresult.h"
#include "writeresult.h"
#include "../../util/refshare.h"

#define NETHANDLER_MASK_READ   (0x01 << 0)
#define NETHANDLER_MASK_WRITE  (0x01 << 1)
#define NETHANDLER_MASK_OPEN   (0x01 << 2)
#define NETHANDLER_MASK_CLOSE  (0x01 << 3)
#define NETHANDLER_MASK_ACCEPT (0x01 << 4)

class DLLAPI INetHandler : public CRefShare
{
public:
    virtual ~INetHandler()  { }
    virtual void onRead(CReadResult& Result, void* Param) = 0;
    virtual void onWrite(CWriteResult& Result, void* Param) = 0;
    virtual void onOpen(CNetResult& Result, void* Param) = 0;
    virtual void onClose(CNetResult& Result, void* Param) = 0;
    virtual void onAccept(CNetResult& Result, void* Param) = 0;
};

typedef CRefObject<INetHandler> INetHandlerPtr;

#endif

