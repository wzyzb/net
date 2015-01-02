#include "writeresult.h"

CWriteResult::CWriteResult(uint32 nBytesToSend,uint32 nBytesTransferred) :
m_nBytesToSend(nBytesToSend),m_nBytesTransferred(nBytesTransferred)
{
}

CWriteResult::~CWriteResult()
{
}

uint32 CWriteResult::getBytesToSend()
{
    return m_nBytesToSend;
}

uint32 CWriteResult::getBytesTransferred()
{
    return m_nBytesTransferred;
}

