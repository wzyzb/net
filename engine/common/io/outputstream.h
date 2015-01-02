#ifndef _IOUTPUTSTREAM_H_
#define _IOUTPUTSTREAM_H_

#include "public.h"
#include "util/refshare.h"

class IOutputStream : public IRefShare
{
public:
	virtual ~IOutputStream() {};
	virtual int writeBytes( const char* pBuf, uint32& Buflen) = 0;
};

typedef CRefObject<IOutputStream> IOutputStreamPtr;

#endif
