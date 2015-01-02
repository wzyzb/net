#ifndef _READRESULT_H_
#define _READRESULT_H_

#include "netresult.h"
#include "../../io/bytesbuffer.h"

class CReadResult : public CNetResult
{
public:
    CReadResult();
    virtual ~CReadResult();

    uint32 getSize();
    const char* c_str();
	
	/**
	 * 设置可读取的数据
	 */
    void setData(const char* pData, uint32 nDataSize);

	/**
	 * 设置可读取的数据
	 */
	void setData(CBytesBuffer& DataBuffer);

private:
	const char* m_pData;
	uint32 m_nDataSize;
};

#endif
