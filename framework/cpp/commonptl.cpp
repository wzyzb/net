#include <framework/commonptl.h>

void writestring(IOutputStream& Output, std::string str)
{
    if ( str.length() >= 0xFF )
    {
        uint16 nStrLen = str.length() <= 0xFFFF ? str.length() : 0xFFFF;
        uint8 nValue = 0xFF;
		uint32 nSize = 1;
		Output.writeBytes((const char*)&nValue, nSize);
		nSize = 2;
        Output.writeBytes((const char*)&nStrLen, nSize);
       
        nSize = nStrLen;
        Output.writeBytes(str.c_str(), nSize); 
    }
    else
    {
        uint8 nStrLen = str.length();

        uint32 nSize = 1;
        Output.writeBytes((const char*)&nStrLen, nSize);

        nSize = nStrLen;
        Output.writeBytes(str.c_str(), nSize);
    }
}

void readstring(IInputStream& Input, std::string& str)
{
	uint16 nValue = 0;
	uint32 nSize = 0;

	nSize = 1;
	Input.readBytes((char*)&nValue, nSize);

	if( nValue == 0xFF )
	{
		nSize = 2;
		Input.readBytes((char*)&nValue, nSize);
	}

	char szBuf[65536] = {0};
	nSize = nValue;
	Input.readBytes(szBuf, nSize);
	szBuf[nSize] = 0;
	str = szBuf;
}

void writeuint8(IOutputStream& Output, uint8 n)
{
    uint32 nSize = sizeof(n);
    Output.writeBytes((const char*)&n, nSize);
}
void writeint8(IOutputStream& Output, int8 n)
{
    uint32 nSize = sizeof(n);
    Output.writeBytes((const char*)&n, nSize);
}
               
void readuint8(IInputStream& Input, uint8& n)
{
    uint32 nSize = sizeof(n);
    Input.readBytes((char*)&n, nSize);
}

void readint8(IInputStream& Input, int8& n)
{
    uint32 nSize = sizeof(n);
    Input.readBytes((char*)&n, nSize);
}


void writeuint16(IOutputStream& Output, uint16 n)
{
    uint32 nSize = sizeof(n);
    Output.writeBytes((const char*)&n, nSize);
}

void writeint16(IOutputStream& Output, int16 n)
{
    uint32 nSize = sizeof(n);
    Output.writeBytes((const char*)&n, nSize);
}
void readuint16(IInputStream& Input, uint16& n)
{
    uint32 nSize = sizeof(n);
    Input.readBytes((char*)&n, nSize);
}
void readint16(IInputStream& Input, int16& n)
{
    uint32 nSize = sizeof(n);
    Input.readBytes((char*)&n, nSize);
}

void writeuint32(IOutputStream& Output, uint32 n)
{
    uint32 nSize = sizeof(n);
    Output.writeBytes((const char*)&n, nSize);
}
void writeint32(IOutputStream& Output, int32 n)
{
    uint32 nSize = sizeof(n);
    Output.writeBytes((const char*)&n, nSize);
}

void writeuint64(IOutputStream& Output, uint64 n)
{
    uint32 nSize = sizeof(n);
    Output.writeBytes((const char*)&n, nSize);
}

void writeint64(IOutputStream& Output, int64 n)
{
    uint32 nSize = sizeof(n);
    Output.writeBytes((const char*)&n, nSize);
}

void readuint32(IInputStream& Input, uint32& n)
{
    uint32 nSize = sizeof(n);
    Input.readBytes((char*)&n, nSize);
}
void readint32(IInputStream& Input, int32& n)
{
    uint32 nSize = sizeof(n);
    Input.readBytes((char*)&n, nSize);
}

void readuint64(IInputStream& Input, uint64& n)
{
    uint32 nSize = sizeof(n);
    Input.readBytes((char*)&n, nSize);
}

void readint64(IInputStream& Input, int64& n)
{
    uint32 nSize = sizeof(n);
    Input.readBytes((char*)&n, nSize);
}

void writesize(IOutputStream& Output, uint32 n)
{
    if( n < 255 )
    {
        uint8  nValue = n;
        uint32 nSize  = sizeof(nValue);
        Output.writeBytes((const char*)&nValue, nSize);
    }
    else if( n < 65535 )
    {
        uint32 nSize = 0;        

        uint8  nFlag = 0xFF;
        nSize = sizeof(nFlag);
        Output.writeBytes((const char*)&nFlag, nSize);

        uint16 nValue = n;
        nSize  = sizeof(nValue);
        Output.writeBytes((const char*)&nValue, nSize);
    }
    else
    {
        uint32 nSize = 0;        

        uint8  nFlagInt8 = 0xFF;
        nSize = sizeof(nFlagInt8);
        Output.writeBytes((const char*)&nFlagInt8, nSize);

        uint16  nFlagInt16 = 0xFFFF;
        nSize = sizeof(nFlagInt16);
        Output.writeBytes((const char*)&nFlagInt16, nSize);

        uint32 nValue = n;
        nSize  = sizeof(nValue);
        Output.writeBytes((const char*)&nValue, nSize);
    }
}

bool readsize(IInputStream& Input, uint32& n)
{   
    if( Input.getDataSize() < sizeof(uint8) )
        return false;

    uint8  nFlagUInt8 = 0;
    uint32 nSize = 0;

    nSize = sizeof(nFlagUInt8);
    Input.readBytes((char*)&nFlagUInt8, nSize);

    if ( 0xFF == nFlagUInt8 )
    {
        if( Input.getDataSize() < sizeof(uint16) )
            return false;

        uint16 nFlagUint16 = 0;

        nSize = sizeof(nFlagUint16);
        Input.readBytes((char*)&nFlagUint16, nSize);
        
        if ( 0xFFFF == nFlagUint16 )
        {
            uint32 nValue = 0;

            nSize = sizeof(nValue);
            Input.readBytes((char*)&nValue, nSize);

            n = nValue;
        }
        else
        {
            n = nFlagUint16;
        }

        return true;
    }
    else
    {
        n = nFlagUInt8;

        return true;
    }

}

void writebytes(IOutputStream& Output, const char* pBuf, uint32 buflen)
{
    Output.writeBytes(pBuf, buflen);
}

void readbytes(IInputStream& Input, char* pszBuf, uint32 readlen)
{
    Input.readBytes(pszBuf, readlen);
}

void printfBuf(const char* pszBuf, uint32 len)
{
    printf("TRACE checkPacket buf -- begin\n");
    
    for( uint32 i = 0; i < len; i++ )
    {
        printf("0x%02x ", (unsigned char)pszBuf[i]);
    }

    printf("TRACE checkPacket buf -- end\n");
}

bool checkPacket(const char* pszBuf, uint32 len, bool bSendCheck)
{
    if( (unsigned char)pszBuf[0] != 0xFF )   
    {   
        printf("if( (unsigned char)pszBuf[0] != 0xFF ) \n");
        printfBuf(pszBuf, len);
        return false;
    }

    int nPos = sizeof(uint8) + sizeof(uint32) + sizeof(uint32);
    
    // ¼ì²é check sum ×Ö¶Î
    uint32 nCheckSum = 0;
    memcpy(&nCheckSum, pszBuf + nPos, sizeof(uint32));
    
    if( nCheckSum != 0x78563412 && nCheckSum != 0 )
    {
        printf("if( nCheckSum != 0x78563412 && nCheckSum != 0 ) \n");
        printfBuf(pszBuf, len);
        return false;
    }

    nPos += sizeof(uint32);

    uint16 nFlag = 0;
    memcpy(&nFlag, pszBuf + nPos, sizeof(uint16));
    
    if( nFlag != 0 )
    {
        printf("if( nFlag != 0 ) \n");
        printfBuf(pszBuf, len);
        return false;
    }

    nPos += sizeof(uint16);

    uint16 nSize = 0;
    uint8 nSizeFlag = 0;
    memcpy(&nSizeFlag, pszBuf + nPos, sizeof(uint8));

    if( nSizeFlag == 0xFF )
    {
        nPos += sizeof(uint8);
        memcpy(&nSize, pszBuf + nPos, sizeof(uint16));
        nPos += sizeof(uint16);
    }
    else
    {
        nSize = nSizeFlag;
        nPos += sizeof(uint8);
    }

    uint32 nBufSize = 0;
    memcpy(&nBufSize, pszBuf + nPos, sizeof(uint32));
    nPos += sizeof(uint32);

    if( int(nPos + nBufSize) > len )
    {
        printf("if( (nPos + nBufSize) < len nPos:%d nBufSize:%d len:%d)\n", nPos, nBufSize, len);
        printfBuf(pszBuf, len);
        return false;
    }

    if( bSendCheck )
    {
        if( (nPos + nBufSize) != len )
        {
            printf("if( (nPos + nBufSize) != len ) \n");
            printfBuf(pszBuf, len);
            return false;
        }
    }

    for(int i = 0; i < nBufSize; i++)
    {
        if( pszBuf[nPos + i] != 0x61 )
        {
            printf("if( pszBuf[nPos + i] != 0x61 [%d])\n", nPos + i);
            printfBuf(pszBuf, len);
            return false;
        }
    }
    
    return true;
}

