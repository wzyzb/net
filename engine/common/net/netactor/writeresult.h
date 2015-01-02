#ifndef _WRITERESULT_H_
#define _WRITERESULT_H_

#include "../../public.h"
#include "netresult.h"

class CWriteResult : public CNetResult
{
public:
    CWriteResult(uint32 nBytesToSend,uint32 nBytesTransferred);
    virtual ~CWriteResult();

    uint32 getBytesToSend();
    uint32 getBytesTransferred();

private:
    uint32 m_nBytesToSend;
    uint32 m_nBytesTransferred;
};

#endif
