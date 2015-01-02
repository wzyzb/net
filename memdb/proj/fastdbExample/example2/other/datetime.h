#if !defined(_DATETIME_H_)
#define _DATETIME_H_

#ifdef WINDOWS
#include <time.h>
#else
#include <sys/time.h>
#endif

inline bool checkTotolSec( long nSecs )
{
    time_t time;
    time = nSecs;
    if( localtime( &time ) == NULL )
        return false;
    else
        return true;
}

class CCException;
/**
 * usage :
 * 需要include <userexception.h>
*/
class CDateTimeException : public CCException
{
public :
	CDateTimeException(const char* pMsg);
	~CDateTimeException() throw();
};

class CInterval;

//默认的时间格式
static char DEFAULT_DATETIME_FORMAT[50]="YYYY-MM-DD hh:mm:ss";

const char YMD_DATETIME_FORMAT[50] = "YYYY-MM-DD";

//默认的时间间隔格式
static char DEFAULT_INTERVAL_FORMAT[50]="DD hh:mm:ss.nnn";

// 时间处理封装类
// 精确到秒
// 增加时区设置，但目前没有实现
class CDateTime
{
private :
	//时区，格林威治时间为0，1表示大一个小时的时区
	//当前时间所采用的时区，
	short m_nTimeZone;
	//显示时间所采用的时区
	short m_nShowZone;
	//当前时间的毫秒数，1970-1-1日到当前的总毫秒数
	uint64 m_nTimeSpan;
	//缓冲时间
	time_t m_Time;
	//用于提高获取当前时间的效率，当此值不为0，则CDatetime的默认构造函数不会直接通过系统api获取当前时间，而是直接使用此值
	static time_t g_Time;
	static uint64 g_nTimeSpan;

private :
	/**
	 * 获取此时区差转换成毫秒数
	 */
	long getTimeZoneMills() const;
	/**
	 * 根据格式化写入到缓冲中
	 */
	bool asYMD(struct tm* pTime,unsigned long Mills,const char Ch,char* Buf,int Index,int Count,int BufSize) const;

public :
	// 按照规定的格式转换为字符串。
	// format：
	// Y－－年
	// M－－月
	// D－－日
	// h－－时
	// m－－分
	// s－－秒
	// n－－毫秒
	std::string asString(const char* Format = DEFAULT_DATETIME_FORMAT) const;

	//默认构造函数，日期为当前时间
	/**
	 * @param bReal 是否取实时的机器时间，一般不建议设置为true
	 */
	CDateTime(bool bReal = false);

	// 给定时间的构造函数。
	// nYear，nMonth，nDay，nHour，nMinute，nSeconds，nMillSeconds的设置必须是合法的日期，否则可能在后面的操作中抛出异常
	// nYear必须大于1970，
	CDateTime(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond,int nMillSeconds,short nTimeZone = 0);

	// 拷贝构造函数
	CDateTime(const CDateTime& Other);

	//根据总毫秒数，初始化当前时间
	CDateTime(uint64 nMillSeconds);
	
	// 获取年份
	int getYear() const;

	// 获取以年计算的月份
	int getMonth() const;

	// 获取以月计算的天数
	int getDay() const;

	// 获取以天计算的小时数
	int getHour() const;

	// 获取以天计算的分数
	int getMinute() const;

	// 获取以天计算的秒数
	int getSecond() const;

	// 获取星期几
	//0~6，0－－星期天，1－－星期一...
	int getDayOfWeek() const;

	//获取以天计算的毫秒数
	int getMillSecond() const;

	//获取以年计算的天数
	int getYday() const;
	
	/**
	 * 公用的初始化函数
	 */
	bool init(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond,int nMillSeconds,short nTimeZone = 0);
    /**
     *@检查日期是否非法
     */
    bool check() const;

	CInterval operator -(const CDateTime& Other) const;

	CDateTime operator -(const CInterval& Interval) const;

	CDateTime operator +(const CInterval& Interval) const;

	const CDateTime& operator -=(const CInterval& Interval);

	const CDateTime& operator +=(const CInterval& Interval);
	
	const CDateTime& operator = (uint64 MillSeconds);

	bool operator==(const CDateTime& Other) const;

	bool operator !=(const CDateTime& Other) const;

	bool operator <=(const CDateTime& Other) const;

	bool operator <(const CDateTime& Other) const;

	bool operator >(const CDateTime& Other) const;

	bool operator >=(const CDateTime& Other) const;

	/** 分解日期格式的Valu字符串，转换为时间的类型
	 * Format可自定义格式为：
	 * Y－－年 ，可以采用2位或4位的表示法，
	 *           对于2位的表示法，永远认为是2000以后的年份，如79则表示2079而不是1979。
	 *            所以建议尽量全部采用4位表示法
	 * M－－月
	 * D－－日
	 * h－－时
	 * m－－分
	 * s－－秒
	 * n－－毫秒
	 * 输入格式不是标准 的日期格式，则抛出异常
	*/
	void parse(const char* Value, const char* Format = DEFAULT_DATETIME_FORMAT);
	/**
	 * 设置显示的时区
	 * 不设置则默认为保存记录的时区一致
	 */
	void setShowTimeZone(short TimeZone);
	/**
	 * 全局函数，更新g_Time为当前时间，当需要使用全局函数时时，此为函数必须被定时调用。
	 */
	static void tickCount();
	
	/**
	 * 获取总的毫秒数
	 */
	uint64 getTotalMills() const;
	/**
	 * 获取总得秒数
	 */
	int32 getTotalSecs() const;
	/**
	 * 静态函数，获取当前的毫秒数
	 * @param bReal 获取真实的秒数
	 */
	static uint64 current(bool bReal = false);
};

// 时间间隔类，完善处理时间的操作
// 精确到秒
class CInterval
{
private :
	friend class CDateTime;
	//时间间隔，单位毫秒。64位记录
	int64 m_nTimeSpan;

private :
	/**
	 * 根据格式化写入到缓冲中
	 */
	bool asYMD(const char Ch,char* Buf,int Index,int Count,int BufSize) const;

public:
	// 构造函数。
	// 大于0代表时间轴往前，小于0代表时间轴往后
	// nDays－－天数上的总间隔
	// nHours－－小时上的总间隔
	// nMinutes－－分钟上的总间隔
	// nSeconds－－秒上的总间隔
	// nMillSeconds－－毫秒上的间隔
	// 例如输入参数为：(10,-24,0,0)
	// 实际上为时间间隔9天。
	CInterval(int nDays, int nHours, int nMinutes, int nSeconds,int nMillSeconds);
	//拷贝构造函数
	CInterval(const CInterval& Other);
	//根据总毫秒数构造函数
	CInterval(const int64 MillSeconds);

	// 获取总的间隔天数
	int getDays() const;

	// 获取总的间隔秒数
	int getTotalSeconds() const;

	// 获取总的分钟数
	int getTotalMinutes() const;

	// 获取总的小时数
	int getTotalHours() const;
	// 获取总的毫秒数
	int64 getTotalMills() const;

	// 获取时间间隔以天计算的秒数：
	// 例如时间间隔为2天10小时1分12秒，则返回12
	int getSecondsOfDay() const;

	// 获取时间间隔以天计算的分钟数：
	// 例如时间间隔为2天10小时1分12秒，则返回1
	int getMinutesOfDay() const;

	// 获取时间间隔以天计算下的总小时数：
	// 例如时间间隔为2天10小时1分12秒，则返回2
	int getHoursOfDay() const;

	// 获取时间间隔以天计算下的总毫秒数：
	// 例如时间间隔为2天10小时1分12秒200毫秒，则返回200
	int getMillsOfDay() const;

	// 重载-操作符，计算两个时间间隔的相差
	CInterval operator -(const CInterval& Other) const;

	// 重载＋操作符，计算两个时间间隔之和
	CInterval operator +(const CInterval& Other) const;

	// 重载自减操作符，
	const CInterval& operator -=(const CInterval& Other);

	// 重载自加操作符
	const CInterval& operator +=(const CInterval& Other);

	// 重载==操作符
	// 比较两个时间间隔是否一致
	bool operator ==(const CInterval& Other) const;

	// 重载!=操作符
	// 比较两个时间间隔的大小
	bool operator !=(const CInterval& Other) const;

	// 重载<操作符
	// 比较两个时间间隔的大小
	bool operator <(const CInterval& Other) const;

	// 重载<=操作符
	// 比较两个时间间隔的大小
	bool operator <=(const CInterval& Other) const;

	// 重载>操作符
	// 比较两个时间间隔的大小
	bool operator >(const CInterval& Other) const;

	// 重载>=操作符
	// 比较两个时间间隔的大小
	bool operator >=(const CInterval& Other) const;

	/**
	 * 分解日期格式的字符串，转换为时间间隔类型
	 * 输入格式不是标准 的日期格式，则抛出异常，如：小时必须在[0,23]区域内
	 */
	void parse(const char* Value,const char* Format = DEFAULT_INTERVAL_FORMAT);
	/**
	 * 按照指定的格式生成字符串形式
	 */
	std::string asString(const char* Format = DEFAULT_INTERVAL_FORMAT) const;

};
#endif
