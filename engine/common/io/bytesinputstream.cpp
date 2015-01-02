#include "bytesinputstream.h"
#include "bytesbuffer.h"

/*

		Class	CBytesInputStream

*/
CBytesInputStream::CBytesInputStream(const char* pBuf,int nBufLen) : m_pBuffer(pBuf),m_nBufLen(nBufLen),m_pCurrent(pBuf)
{
}

CBytesInputStream::~CBytesInputStream()
{
}

void CBytesInputStream::reset(const char* pBuf,int nBufLen)
{
	m_pBuffer = m_pCurrent = pBuf;
	m_nBufLen = nBufLen;
}

int CBytesInputStream::readBytes(char *pBuf, uint32& Buflen)
{
	uint32 copylen = m_nBufLen - (m_pCurrent - m_pBuffer);
	if ( Buflen < copylen )
		copylen = Buflen;
	memcpy(pBuf,m_pCurrent,copylen);
	Buflen = copylen;
	m_pCurrent += copylen;
	return 0;
}

int CBytesInputStream::readBytes(CBytesBuffer& Buffer)
{
	uint32 copylen = m_nBufLen - (m_pCurrent - m_pBuffer);
	Buffer.writeBytes(m_pCurrent,copylen);
	m_pCurrent += copylen;
	return 0;
}

int CBytesInputStream::popBytes(uint32 BytesToPop)
{
	uint32 copylen = m_nBufLen - (m_pCurrent - m_pBuffer);
	if ( copylen > BytesToPop )
		copylen = BytesToPop;
	m_pCurrent += copylen;
	return 0;
}

uint32 CBytesInputStream::getDataSize() const
{
	return m_nBufLen - (m_pCurrent - m_pBuffer);
}

const char* CBytesInputStream::c_str()
{
	return m_pCurrent;
}

void CBytesInputStream::incRef()
{
	CRefShare::incRef();
}

bool CBytesInputStream::decRef()
{ 
    return CRefShare::decRef();
	//return;
}

