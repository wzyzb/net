#include <framework/servant.h>
#include <util/datetime.h>

/*

		Class	STATISTICS_INFO

*/
void STATISTICS_INFO::incExecute()
{
	uint32 now = (uint32)(CDateTime::current()/1000);
	TotalExecTimes ++;
	if ( this->LastExecTime != now )
	{
		LastExecTime = now;
		ExecTimesPerSecond = CurrentExecTimes;
		if ( this->ExecTimesPerSecond > PeekExecTimesPerSecond )
			PeekExecTimesPerSecond = ExecTimesPerSecond;
		CurrentExecTimes = 0;
	}
	CurrentExecTimes ++;
}

void STATISTICS_INFO::serial(char* pBuf,int BufSize)
{
	snprintf(pBuf,BufSize-1,"%d|%d|%d",ExecTimesPerSecond,PeekExecTimesPerSecond,TotalExecTimes);
}

/*

		Class	IServant

*/
void IServant::incExecuteTimes()
{
	this->m_StatisticsInfo.incExecute();
}

STATISTICS_INFO& IServant::getStatsticsInfo()
{
	return this->m_StatisticsInfo;
}


