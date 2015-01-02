#include <public.h>
#include <binlog.h>

/*

		Class	CBinLog

*/
namespace binlog
{
	char g_BinLogPath[_MAX_PATH_LENGTH] = "./log";
	char g_BinLogName[_MAX_PATH_LENGTH] = "debug";
}

void CBinLog::setBinLogPath(const char *pPath)
{
	snprintf(binlog::g_BinLogPath,sizeof(binlog::g_BinLogPath)-1,"%s",pPath);
}

void CBinLog::setBinLogName(const char* pName)
{
	snprintf(binlog::g_BinLogName,sizeof(binlog::g_BinLogName)-1,"%s",pName);
}

const char* CBinLog::printBinText(const char *pData, int DataSize)
{
	static char BinText[1024*2+1] = {0};
	int j = 0;
	for ( int i = 0 ; i < DataSize && j < sizeof(BinText) - 1 ; i ++ , j += 2 )
	{
		unsigned char tmp = *(pData+i);
		unsigned char tmp1 = tmp >> 4;
		tmp = tmp&0xF;
		if ( tmp1 < 10 )
			BinText[j] = tmp1 +'0';
		else
			BinText[j] = tmp1 + 'A' - 10;
		if ( tmp < 10 )
			BinText[j+1] = tmp + '0';
		else
			BinText[j+1] = tmp + 'A' - 10;
	}
	BinText[j] = 0;
	return BinText;
}

void CBinLog::writeBinLog(const char *pData, int DataSize, const char *pLogName)
{
	char filename[2*_MAX_PATH_LENGTH] = {0};
	if ( pLogName != NULL )
		snprintf(filename,sizeof(filename)-1,"%s/%s.%s.binlog",binlog::g_BinLogPath,binlog::g_BinLogName,pLogName);
	else
		snprintf(filename,sizeof(filename)-1,"%s/%s.binlog",binlog::g_BinLogPath,binlog::g_BinLogName);
	FILE *logfile = NULL;
	if ( ( logfile = fopen(filename, "ab") ) == NULL )
		return ;
	fwrite(pData,sizeof(char),DataSize,logfile);
	fclose(logfile);
}
