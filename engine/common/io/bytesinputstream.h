#ifndef _BYTESINPUTSTREAM_H_
#define _BYTESINPUTSTREAM_H_

#include "inputstream.h"
#include "util/refShare.h"
#include "bytesbuffer.h"

class CBytesInputStream : public IInputStream,
						  public CRefShare
{
private :
	const char* m_pBuffer;
	const char* m_pCurrent;
	int m_nBufLen;
public :
	CBytesInputStream(const char* pBuffer,int BufLen);
	~CBytesInputStream();
    int readBytes(char *pBuf, uint32& Buflen);
	int readBytes(CBytesBuffer& Buffer);
    int popBytes(uint32 BytesToPop);
    uint32 getDataSize() const;
    const char* c_str();
	void incRef();
	bool decRef();
	void reset(const char* pBuffer,int BufLen);
};

#endif
