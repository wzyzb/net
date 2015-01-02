#ifndef _COMMONPTL_H_
#define _COMMONPTL_H_

#include <io/outputstream.h>
#include <io/inputstream.h>
#include <string>
#include <public.h>

DLLAPI void writestring(IOutputStream& Output, std::string str);
DLLAPI void readstring(IInputStream& Input, std::string& str);

DLLAPI void writeuint8(IOutputStream& Output, uint8 n);
DLLAPI void writeint8(IOutputStream& Output, int8 n);
DLLAPI void readuint8(IInputStream& Input, uint8& n);
DLLAPI void readint8(IInputStream& Input, int8& n);

DLLAPI void writeuint16(IOutputStream& Output, uint16 n);
DLLAPI void writeint16(IOutputStream& Output, int16 n);
DLLAPI void readuint16(IInputStream& Input, uint16& n);
DLLAPI void readint16(IInputStream& Input, int16& n);

DLLAPI void writeuint32(IOutputStream& Output, uint32 n);
DLLAPI void writeint32(IOutputStream& Output, int32 n);
DLLAPI void readuint32(IInputStream& Input, uint32& n);
DLLAPI void readint32(IInputStream& Input, int32& n);

DLLAPI void writeuint64(IOutputStream& Output, uint64 n);
DLLAPI void writeint64(IOutputStream& Output, int64 n);
DLLAPI void readuint64(IInputStream& Input, uint64& n);
DLLAPI void readint64(IInputStream& Input, int64& n);

DLLAPI void writesize(IOutputStream& Output, uint32 n);
DLLAPI bool readsize(IInputStream& Input, uint32& n);

DLLAPI void writebytes(IOutputStream& Output, const char* pBuf, uint32 buflen);
DLLAPI void readbytes(IInputStream& Input, char* pszBuf, uint32 readlen);

DLLAPI bool checkPacket(const char* pszBuf, uint32 len, bool bSendCheck=false);

#endif 

