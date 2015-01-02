#include "readresult.h"

CReadResult::CReadResult()
{
	m_pData = NULL;
	m_nDataSize = 0;
}

CReadResult::~CReadResult()
{
}

uint32 CReadResult::getSize()
{
    return m_nDataSize;
}

const char* CReadResult::c_str()
{
    return m_pData;
}

void CReadResult::setData(const char* pData, uint32 nDataSize)
{
	m_pData = pData;
	m_nDataSize = nDataSize;
}

void CReadResult::setData(CBytesBuffer& DataBuffer)
{
	m_pData = DataBuffer.c_str();
	m_nDataSize = DataBuffer.getDataSize();
}

