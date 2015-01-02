#include "public.h"
#include <string>
#ifdef WINDOWS
#include <time.h>
#else
#include <sys/time.h>
#endif
#include "datetime.h"


/*

		Common Function

*/
extern "C" {
bool parseYMD(struct tm* Time,unsigned long* Millis , const char Ch,const char* Value,int Index,int Length,int Count)
{
	char buf[5];
	int value = 0;
	if ( Index + Count > Length )
		return false;
	switch ( Ch )
	{
		case 'Y' :
			{
				//int year = 0;
				if ( Count == 2 || Count == 4 )
				{
					memcpy(buf,Value+Index,Count);
					buf[Count] = 0;
					value = atoi(buf);
				}
				else
					return false;
				//输入两位永远是表示2000年开始的
				if ( Count == 2 )
					value += 2000;
				if ( value >= 1970 )
					value -= 1900;
				else
					return false;
				Time->tm_year = value;
			}
			break;
		case 'M' :
			{
				if ( Count > 2 )
					return false;
				memcpy(buf,Value+Index,Count);
				buf[Count] = 0;
				value = atoi(buf);
				if ( value > 12 || value <= 0 )
					return false;
				Time->tm_mon = value-1;
			}
			break;
		case 'D' :
			{
				if ( Count > 2 )
					return false;
				memcpy(buf,Value+Index,Count);
				buf[Count] = 0;
				value = atoi(buf);
				if ( value > 31 || value <= 0 )
					return false;
				Time->tm_mday = value;
			}
			break;
		case 'h' :
			{
				if ( Count > 2 )
					return false;
				memcpy(buf,Value+Index,Count);
				buf[Count] = 0;
				value = atoi(buf);
				if ( value > 23 || value < 0 )
					return false;
				Time->tm_hour = value;
			}
			break;
		case 'm' :
			{
				if ( Count > 2 )
					return false;
				memcpy(buf,Value+Index,Count);
				buf[Count] = 0;
				value = atoi(buf);
				if ( value > 59 || value < 0 )
					return false;
				Time->tm_min = value;
			}
			break;
		case 's' :
			{
				if ( Count > 2 )
					return false;
				memcpy(buf,Value+Index,Count);
				buf[Count] = 0;
				value = atoi(buf);
				if ( value > 59 || value < 0 )
					return false;
				Time->tm_sec = value;
			}
			break;
		case 'n' :
			{
				if ( Count > 3 )
					return false;
				memcpy(buf,Value+Index,Count);
				buf[Count] = 0;
				value = atoi(buf);
				if ( value < 0 )
					return false;
				*Millis = value;
			}
			break;
		default :
			return false;
	}
	return true;
}

bool parseIntervalYMD(int64* nTime,const char Ch,const char* Value,int Index,int Length ,int Count)
{
	char buf[5];
	int value = 0;
	if ( Index + Count > Length )
		return false;
	switch ( Ch )
	{
		case 'D' :
			{
				if ( Count > 2 )
					return false;
				memcpy(buf,Value+Index,Count);
				buf[Count] = 0;
				value = atoi(buf);
				if ( value < 0 )
					return false;
				*nTime += value * 24 * 3600 * 1000 ;
			}
			break;
		case 'h' :
			{
				if ( Count > 2 )
					return false;
				memcpy(buf,Value+Index,Count);
				buf[Count] = 0;
				value = atoi(buf);
				if ( value >= 24 || value < 0 )
					return false;
				*nTime += value * 3600 * 1000 ;
			}
			break;
		case 'm' :
			{
				if ( Count > 2 )
					return false;
				memcpy(buf,Value+Index,Count);
				buf[Count] = 0;
				value = atoi(buf);
				if ( value >= 60 || value < 0 )
					return false;
				*nTime += value * 60 * 1000 ;
			}
			break;
		case 's' :
			{
				if ( Count > 2 )
					return false;
				memcpy(buf,Value+Index,Count);
				buf[Count] = 0;
				value = atoi(buf);
				if ( value >= 60 || value < 0 )
					return false;
				*nTime += value * 1000 ;
			}
			break;
		case 'n' :
			{
				if ( Count > 3 )
					return false;
				memcpy(buf,Value+Index,Count);
				buf[Count] = 0;
				value = atoi(buf);
				if ( value < 0 )
					return false;
				*nTime += value;
			}
			break;
		default :
			return false;
	}
	return true;
}
}

/*

		Class	CDateTime

*/
time_t CDateTime::g_Time = 0;
uint64 CDateTime::g_nTimeSpan = 0;

uint64 CDateTime::current(bool bReal)
{
	if ( g_Time == 0 || bReal )
	{
#ifdef WINDOWS
		return GetTickCount();
#else
		struct timeval now;
		gettimeofday(&now,NULL);
		uint64 retval = now.tv_sec;
		return (retval * 1000 + now.tv_usec / 1000);
#endif
	}
	return g_nTimeSpan;
}

void CDateTime::tickCount()
{
#ifdef WINDOWS
	SYSTEMTIME current;
	GetLocalTime(&current);
	struct tm ltm;
	memset(&ltm, 0, sizeof(ltm));
	ltm.tm_year = current.wYear - 1900;
	ltm.tm_mon = current.wMonth - 1;
	ltm.tm_mday = current.wDay;
	ltm.tm_hour = current.wHour;
	ltm.tm_min = current.wMinute;
	ltm.tm_sec = current.wSecond;
	g_Time = mktime(&ltm);
	uint64 temptime = g_Time;
	g_nTimeSpan = temptime * 1000 + current.wMilliseconds;
#else
	struct timeval current;
	gettimeofday(&current,NULL);
	g_Time = current.tv_sec;
	uint64 temptime = g_Time;
	g_nTimeSpan = temptime * 1000 + current.tv_usec / 1000;
#endif
}

CDateTime::CDateTime(bool bReal)
{
	m_nTimeZone = 0;
	m_nShowZone = 0;
	if ( g_Time == 0 || bReal )
	{
#ifdef WINDOWS
	SYSTEMTIME current;
	GetLocalTime(&current);
	init(current.wYear,current.wMonth,current.wDay,current.wHour,current.wMinute,current.wSecond,current.wMilliseconds);
#else
	struct timeval current;
	gettimeofday(&current,NULL);
	m_Time = current.tv_sec;
	m_nTimeSpan = m_Time;
	m_nTimeSpan = m_nTimeSpan * 1000 + current.tv_usec / 1000;
#endif
	}
	else
	{
		m_Time = g_Time;
		m_nTimeSpan = g_nTimeSpan;
	}
}

CDateTime::CDateTime(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond,int nMillSeconds,short nTimeZone)
{
	init(nYear,nMonth,nDay,nHour,nMinute,nSecond,nMillSeconds,nTimeZone);
}

bool CDateTime::init(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond,int nMillSeconds,short nTimeZone)
{
	struct tm ltm;
	memset(&ltm, 0, sizeof(ltm));
	ltm.tm_year = nYear - 1900;
	ltm.tm_mon = nMonth - 1;
	ltm.tm_mday = nDay;
	ltm.tm_hour = nHour;
	ltm.tm_min = nMinute;
	ltm.tm_sec = nSecond;
	m_Time = mktime(&ltm);
	//非法判断
	if ( nMillSeconds < 0 || nMillSeconds > 999 ||  m_Time == -1 )
	{
		m_nTimeZone = 0;
		m_nShowZone = 0;
		m_nTimeSpan = 0;
		m_Time = 0;
		return false;
	}
	m_nTimeSpan = m_Time;
	m_nTimeSpan = m_nTimeSpan * 1000 + nMillSeconds;
	m_nTimeZone = m_nShowZone = nTimeZone;
	return true;
}

CDateTime::CDateTime(const CDateTime& Other)
{
	this->m_nTimeZone = Other.m_nTimeZone;
	this->m_nTimeSpan = Other.m_nTimeSpan;
	this->m_nShowZone = Other.m_nShowZone;
	m_Time = (long)(m_nTimeSpan / 1000);
}

CDateTime::CDateTime(uint64 nMillSeconds)
{
	this->m_nTimeSpan = nMillSeconds;
	m_Time = (long)(m_nTimeSpan / 1000);
	m_nTimeZone = 0;
	m_nShowZone = 0;
}

const CDateTime& CDateTime::operator = (uint64 MillSeconds)
{
    m_nTimeZone = 0;
    m_nShowZone = 0;
	this->m_nTimeSpan = MillSeconds;
	this->m_Time = (long)(m_nTimeSpan/1000);
	return *this;
}

int CDateTime::getYear() const
{
    tm* ptm = localtime(&m_Time);
    if( ptm == NULL )
        throw CDateTimeException("CDateTime Invalid Time!");

	return ptm->tm_year+1900;
}

int CDateTime::getMonth() const
{
    tm* ptm = localtime(&m_Time);
    if( ptm == NULL )
        throw CDateTimeException("CDateTime Invalid Time!");

	return ptm->tm_mon + 1;
}

int CDateTime::getDay() const
{
    tm* ptm = localtime(&m_Time);
    if( ptm == NULL )
        throw CDateTimeException("CDateTime Invalid Time!");

	return ptm->tm_mday;
}

int CDateTime::getHour() const
{
    tm* ptm = localtime(&m_Time);
    if( ptm == NULL )
        throw CDateTimeException("CDateTime Invalid Time!");

	return ptm->tm_hour;
}

int CDateTime::getMinute() const
{
    tm* ptm = localtime(&m_Time);
    if( ptm == NULL )
        throw CDateTimeException("CDateTime Invalid Time!");

	return ptm->tm_min;
}

int CDateTime::getSecond() const
{
    tm* ptm = localtime(&m_Time);
    if( ptm == NULL )
        throw CDateTimeException("CDateTime Invalid Time!");

	return ptm->tm_sec;
}

int CDateTime::getDayOfWeek() const
{
    tm* ptm = localtime(&m_Time);
    if( ptm == NULL )
        throw CDateTimeException("CDateTime Invalid Time!");

	return ptm->tm_wday;
}

int CDateTime::getMillSecond() const
{
	return (int)(m_nTimeSpan % 1000 );
}

int CDateTime::getYday() const
{
    tm* ptm = localtime(&m_Time);
    if( ptm == NULL )
        throw CDateTimeException("CDateTime Invalid Time!");

	return ptm->tm_yday;
}

CInterval CDateTime::operator -(const CDateTime& Other) const
{
	int64 interval;
	if ( this->m_nTimeZone == Other.m_nTimeZone )
		interval = this->m_nTimeSpan - Other.m_nTimeSpan;
	else
	{
		interval = this->m_nTimeSpan + this->getTimeZoneMills() - Other.m_nTimeSpan - Other.getTimeZoneMills();
	}
	return CInterval(interval);
}

CDateTime CDateTime::operator -(const CInterval& Interval) const
{
	CDateTime datetime(*this);
	datetime -= Interval;
	return datetime;
}

CDateTime CDateTime::operator +(const CInterval& Interval) const
{
	CDateTime datetime(*this);
	datetime += Interval;
	return datetime;
}

const CDateTime& CDateTime::operator -=(const CInterval& Interval)
{
	m_nTimeSpan -= Interval.m_nTimeSpan;
	m_Time = (long)(m_nTimeSpan / 1000);
	return *this;
}

const CDateTime& CDateTime::operator +=(const CInterval& Interval)
{
	m_nTimeSpan += Interval.m_nTimeSpan;
	m_Time = (long)(m_nTimeSpan / 1000);
	return *this;
}

bool CDateTime::operator==(const CDateTime& Other) const
{
	if ( this->m_nTimeZone == Other.m_nTimeZone )
		return this->m_nTimeSpan == Other.m_nTimeSpan;
	else
		return ( this->m_nTimeSpan + getTimeZoneMills() ) == Other.m_nTimeSpan + Other.getTimeZoneMills();
}

bool CDateTime::operator !=(const CDateTime& Other) const
{
	if ( this->m_nTimeZone == Other.m_nTimeZone )
		return this->m_nTimeSpan != Other.m_nTimeSpan;
	else
		return ( this->m_nTimeSpan + getTimeZoneMills() ) != Other.m_nTimeSpan + Other.getTimeZoneMills();
}

bool CDateTime::operator <= (const CDateTime& Other) const
{
	if ( this->m_nTimeZone == Other.m_nTimeZone )
		return this->m_nTimeSpan <= Other.m_nTimeSpan;
	else
		return ( this->m_nTimeSpan + getTimeZoneMills() ) <= Other.m_nTimeSpan + Other.getTimeZoneMills();
}

bool CDateTime::operator < (const CDateTime& Other) const
{
	if ( this->m_nTimeZone == Other.m_nTimeZone )
		return this->m_nTimeSpan < Other.m_nTimeSpan;
	else
		return ( this->m_nTimeSpan + getTimeZoneMills() ) < Other.m_nTimeSpan + Other.getTimeZoneMills();
}

bool CDateTime::operator >(const CDateTime& Other) const
{
	if ( this->m_nTimeZone == Other.m_nTimeZone )
		return this->m_nTimeSpan > Other.m_nTimeSpan;
	else
		return ( this->m_nTimeSpan + getTimeZoneMills() ) > Other.m_nTimeSpan + Other.getTimeZoneMills();
}

bool CDateTime::operator >=(const CDateTime& Other) const
{
	if ( this->m_nTimeZone == Other.m_nTimeZone )
		return this->m_nTimeSpan >= Other.m_nTimeSpan;
	else
		return ( this->m_nTimeSpan + getTimeZoneMills() ) >= Other.m_nTimeSpan + Other.getTimeZoneMills();
}

long CDateTime::getTimeZoneMills() const
{
	return m_nTimeZone * 1000 * 3600;
}

void CDateTime::setShowTimeZone(short TimeZone)
{
	this->m_nShowZone = TimeZone;
}

void CDateTime::parse(const char* Value, const char* Format)
{
	//m_nTimeSpan = 0;
	//m_Time = time(0);
	int len = length(Format);
	int vallen = length(Value);
	int index = 0;
	struct tm ltm;
    tm* ptm = localtime(&m_Time);
    if ( ptm != NULL )
        memcpy(&ltm,ptm,sizeof(struct tm));
	//memset(&ltm,0,sizeof(struct tm));
	unsigned long millis = this->getMillSecond();
	for ( index = 0 ; index < len ; )
	{
		char ch = Format[index];
		switch ( ch )
		{
			case 'Y' :
			case 'M' :
			case 'D' :
			case 'h' :
			case 'm' :
			case 's' :
			case 'n' :
				{
					int count = 1;
					while ( Format[++index] == ch )
						count++;
					//分解
					if ( !parseYMD(&ltm,&millis,ch,Value,index-count,vallen,count) )
						throw CDateTimeException("CDateTime Format Error!");
				};
			break;
			default :
				index ++;
		}
	}
	this->m_Time = mktime(&ltm);
	if ( m_Time == -1 )
		throw CDateTimeException("CDateTime Format Error!");
	m_nTimeSpan = m_Time;
	m_nTimeSpan = m_nTimeSpan * 1000 + millis;
}

std::string CDateTime::asString(const char* Format) const
{
	char buf[100];
	sprintf(buf,"%s",Format);
	int len = length(Format);
	int index = 0;
	time_t showtime;
	if ( this->m_nTimeZone != m_nShowZone )
		showtime = m_Time + ( m_nShowZone - m_nTimeZone) * 3600;
	else
		showtime = m_Time;

    struct tm *ptm = localtime(&showtime);
    if ( ptm == NULL )
        throw CDateTimeException("CDateTime asString Error!");

	struct tm time = *ptm;
	unsigned long mills = (long)(m_nTimeSpan % 1000);
	for ( index = 0 ; index < len ; )
	{
		char ch = Format[index];
		switch ( ch )
		{
			case 'Y' :
			case 'M' :
			case 'D' :
			case 'h' :
			case 'm' :
			case 's' :
			case 'n' :
				{
					int count = 1;
					while ( Format[++index] == ch )
						count++;
					//分解
					if ( !asYMD(&time,mills,ch,buf,index-count,count,sizeof(buf)) )
						throw CDateTimeException("CDateTime Format Error!");
				};
			break;
			default :
				index ++;
		}
	}
	return std::string(buf);
}

bool CDateTime::asYMD(struct tm* pTime,unsigned long Mills,const char Ch,char* Buf,int Index,int Count,int BufSize) const
{
	if ( Index + Count >= BufSize )
		return false;
	char change;
	char format[5];
	sprintf(format,"%%0%dd",Count);
	change = Buf[Index+Count];
	switch ( Ch )
	{
		case 'Y' :
			if ( Count == 2 )
				sprintf(Buf+Index,format,((pTime->tm_year+1900) % 100));
			else if ( Count == 4 )
				sprintf(Buf+Index,format,pTime->tm_year+1900);
			else
				return false;
			break;
		case 'M' :
			if ( Count > 2 )
				return false;
			sprintf(Buf+Index,format,pTime->tm_mon+1);
			break;
		case 'D' :
			if ( Count > 2 )
				return false;
			sprintf(Buf+Index,format,pTime->tm_mday);
			break;
		case 'h' :
			if ( Count > 2 )
				return false;
			sprintf(Buf+Index,format,pTime->tm_hour);
			break;
		case 'm' :
			if ( Count > 2 )
				return false;
			sprintf(Buf+Index,format,pTime->tm_min);
			break;
		case 's' :
			if ( Count > 2 )
				return false;
			sprintf(Buf+Index,format,pTime->tm_sec);
			break;
		case 'n' :
			if ( Count > 3 )
				return false;
			sprintf(Buf+Index,format,Mills);
			break;
		default :
			return false;
	}
	Buf[Index+Count] = change;
	return true;
}

bool CDateTime::check() const
{
    tm* ptm = localtime(&m_Time);
    if ( ptm == NULL )
        return false;
    else
        return true;
}

uint64 CDateTime::getTotalMills() const
{
	return this->m_nTimeSpan;
}

int32 CDateTime::getTotalSecs() const
{
	return (int32)m_Time;
}


/*

		Class	CInterval

*/
CInterval::CInterval(int nDays, int nHours, int nMinutes, int nSeconds,int nMillSeconds)
{
	this->m_nTimeSpan = nMillSeconds;
	this->m_nTimeSpan += nSeconds * 1000;
	this->m_nTimeSpan += nMinutes * 60 * 1000;
	this->m_nTimeSpan += nHours * 60 * 60 * 1000;
	this->m_nTimeSpan += nDays * 24 * 60 * 60 * 1000;
}

CInterval::CInterval(const CInterval& Other)
{
	m_nTimeSpan = Other.m_nTimeSpan;
}

CInterval::CInterval(const int64 MillSeconds)
{
	m_nTimeSpan = MillSeconds;
}
/*
CInterval::CInterval(const long& MillSeconds)
{
	m_nTimeSpan = MillSeconds;
}
*/
int CInterval::getDays() const
{
	return (long)(m_nTimeSpan / (24 * 60 * 60 * 1000));
}

int CInterval::getTotalSeconds() const
{
	return (long)(m_nTimeSpan / 1000);
}

int CInterval::getTotalMinutes() const
{
	return (long)(m_nTimeSpan / (60 * 1000));
}

int CInterval::getTotalHours() const
{
	return (long)(m_nTimeSpan / (60 * 60 * 1000));
}

int64 CInterval::getTotalMills() const
{
	return m_nTimeSpan;
}

int CInterval::getSecondsOfDay() const
{
	return getTotalSeconds() - getTotalMinutes() * 60 ;
}

int CInterval::getMinutesOfDay() const
{
	return getTotalMinutes() - getTotalHours() * 60 ;
}

int CInterval::getHoursOfDay() const
{
	return getTotalHours() - getDays() * 24 ;
}

int CInterval::getMillsOfDay() const
{
	return (long)(m_nTimeSpan - getTotalSeconds() * 1000);
}

CInterval CInterval::operator -(const CInterval& Other) const
{
	CInterval result(*this);
	result -= Other;
	return result;
}

CInterval CInterval::operator +(const CInterval& Other) const
{
	CInterval result(*this);
	result += Other;
	return result;
}

const CInterval& CInterval::operator -=(const CInterval& Other)
{
	this->m_nTimeSpan -= Other.m_nTimeSpan;
	return *this;
}

const CInterval& CInterval::operator +=(const CInterval& Other)
{
	this->m_nTimeSpan += Other.m_nTimeSpan;
	return *this;
}

bool CInterval::operator ==(const CInterval& Other) const
{
	return this->m_nTimeSpan == Other.m_nTimeSpan;
}

bool CInterval::operator !=(const CInterval& Other) const
{
	return this->m_nTimeSpan != Other.m_nTimeSpan;
}

bool CInterval::operator <(const CInterval& Other) const
{
	return this->m_nTimeSpan < Other.m_nTimeSpan;
}

bool CInterval::operator <=(const CInterval& Other) const
{
	return this->m_nTimeSpan <= Other.m_nTimeSpan;
}

bool CInterval::operator >(const CInterval& Other) const
{
	return this->m_nTimeSpan > Other.m_nTimeSpan;
}

bool CInterval::operator >=(const CInterval& Other) const
{
	return this->m_nTimeSpan >= Other.m_nTimeSpan;
}

void CInterval::parse(const char* Value,const char* Format)
{
	m_nTimeSpan = 0;
	int len = length(Format);
	int vallen = length(Value);
	int index = 0;
	for ( index = 0 ; index < len ; )
	{
		char ch = Format[index];
		switch ( ch )
		{
			case 'D' :
			case 'h' :
			case 'm' :
			case 's' :
			case 'n' :
				{
					int count = 1;
					while ( Format[++index] == ch )
						count++;
					//分解
					if ( !parseIntervalYMD(&m_nTimeSpan,ch,Value,index-count,vallen,count) )
						throw CDateTimeException("Interval Format Error!");
				};
			break;
			default :
				index ++;
		}
	}
}

std::string CInterval::asString(const char* Format) const
{
	char buf[100];
	sprintf(buf,"%s",Format);
	int len = length(Format);
	int index = 0;
	for ( index = 0 ; index < len ; )
	{
		char ch = Format[index];
		switch ( ch )
		{
			case 'D' :
			case 'h' :
			case 'm' :
			case 's' :
			case 'n' :
				{
					int count = 1;
					while ( Format[++index] == ch )
						count++;
					//分解
					if ( !asYMD(ch,buf,index-count,count,sizeof(buf)) )
						throw CDateTimeException("Interval Format Error!");
				};
			break;
			default :
				index ++;
		}
	}
	return std::string(buf);
}

bool CInterval::asYMD(const char Ch,char* Buf,int Index,int Count,int BufSize) const
{
	if ( Index + Count >= BufSize )
		return false;
	char change;
	char format[5];
	sprintf(format,"%%0%dd",Count);
	change = Buf[Index+Count];
	switch ( Ch )
	{
		case 'D' :
			if ( Count > 2 )
				return false;
			sprintf(Buf+Index,format,this->getDays());
			break;
		case 'h' :
			if ( Count > 2 )
				return false;
			sprintf(Buf+Index,format,this->getHoursOfDay());
			break;
		case 'm' :
			if ( Count > 2 )
				return false;
			sprintf(Buf+Index,format,this->getMinutesOfDay());
			break;
		case 's' :
			if ( Count > 2 )
				return false;
			sprintf(Buf+Index,format,this->getSecondsOfDay());
			break;
		case 'n' :
			if ( Count > 3 )
				return false;
			sprintf(Buf+Index,format,this->getMillsOfDay());
			break;
		default :
			return false;
	}
	Buf[Index+Count] = change;
	return true;
}
