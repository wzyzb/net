#include "public.h"

#ifdef WIN32
#include <iostream>
#include <sstream>
#include <windows.h>
#include <dbghelp.h>

#pragma comment( lib, "dbghelp.lib" )

#else
#include <execinfo.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#endif

#ifndef NULL
#define NULL 0
#endif

#ifdef WIN32
void windowsCallstack(CONTEXT* context, std::string& stackInfo)
{
    STACKFRAME sf;
    memset(&sf, 0, sizeof(STACKFRAME));
    sf.AddrPC.Offset = context->Eip;
    sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrStack.Offset = context->Esp;
    sf.AddrStack.Mode = AddrModeFlat;
    sf.AddrFrame.Offset = context->Ebp;
    sf.AddrFrame.Mode = AddrModeFlat;
    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();

    for( ; ; )
    {
        if( !StackWalk(IMAGE_FILE_MACHINE_I386, 
            hProcess,
            hThread, 
            &sf, 
            context, 
            0, 
            SymFunctionTableAccess, 
            SymGetModuleBase, 0 ) )
        {
            break;
        }

        if( sf.AddrFrame.Offset == 0 )
        {
            break;
        }
        BYTE symbolBuffer[ sizeof( SYMBOL_INFO ) + 1024 ];

        PSYMBOL_INFO pSymbol = ( PSYMBOL_INFO ) symbolBuffer;

        pSymbol->SizeOfStruct = sizeof( symbolBuffer );
        pSymbol->MaxNameLen = 1024;

        DWORD64 symDisplacement = 0;
        if( SymFromAddr( hProcess, sf.AddrPC.Offset, 0, pSymbol ) )
        {
            stackInfo.append( pSymbol->Name ); 
            stackInfo.append( ":  ");
        }
        else
        {
            stackInfo.append( "can't find the function!\n" );
            printf( "SymFromAdd failed!\n" );
            break;
        }

        IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };
        DWORD dwLineDisplacement;

        if( SymGetLineFromAddr( hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo ) )
        {
            std::stringstream tmpss;
            tmpss << "line " << lineInfo.LineNumber << ":  ";
            stackInfo.append( tmpss.str() );
            stackInfo.append(lineInfo.FileName);
        }
        else
        {
            printf( "SymGetLineFromAddr failed!\n" );
            break;
        }
        stackInfo.append("\n"); 
    }
    
}
#else
void linuxCallstack(std::string &stackInfo)
{
    int nptrs;
#define SIZE 100
    void *buffer[100];
    char **strings;

    nptrs = backtrace(buffer, SIZE);

    strings = backtrace_symbols(buffer, nptrs);

    for(int i = 0; i  < nptrs; ++i)
    {
        stackInfo.append(strings[i]);
        stackInfo.append("\n");
    }

    if (strings == NULL)
    {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }
    free(strings);
}
#endif
