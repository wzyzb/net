#include "netpublic.h"

int32 setSocketMode(SOCKET s,ECOMMMODE Mode)
{
    if (Mode == ASYNC_MODE)
    {
#ifdef WINDOWS
        u_long mode = 1;
        if (0 != ioctlsocket(s,FIONBIO,&mode))
        {
            RETURN_ERROR ;
        }
#else
        int flags = fcntl(s,F_GETFL,0);
        if ( -1 == flags )
        {
            RETURN_ERROR;
        }
        flags |= O_NONBLOCK;
        if (-1 == fcntl(s,F_SETFL,flags))
        {
            RETURN_ERROR;
        }
#endif
    }
    else
    {
#ifdef WINDOWS
        u_long mode = 0;
        if (0 != ioctlsocket(s,FIONBIO,&mode))
        {
            RETURN_ERROR ;
        }
#else
        int flags = fcntl(s,F_GETFL,0);
        if ( -1 == flags )
        {
            RETURN_ERROR;
        }
        flags |= ~O_NONBLOCK;
        if (-1 == fcntl(s,F_SETFL,flags))
        {
            RETURN_ERROR;
        }
#endif
    }
    return 0;
}

void printfBinBuffer(const char* pszBuf,uint32 nSize,const char* pszContext)
{
    printf("Trace Buffer:%s Begin \n",pszContext);
    for (int i=0;i<nSize;i++)
    {
        printf("0x%02X ",(unsigned char)pszBuf[i]);
    }
    printf("\nTrace Buffer:%s End \n", pszContext);
}