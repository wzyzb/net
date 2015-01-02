#include "mysqlpublic.h"

#include "../sqlexception.h"
#include "../dbpublic.h"

void convert(MYSQL_TIME& SQLTime,const CDateTime& DateTime)
{
	memset(&SQLTime,0,sizeof(MYSQL_TIME));
	SQLTime.year = DateTime.getYear();
	SQLTime.month = DateTime.getMonth();
	SQLTime.day = DateTime.getDay();
	SQLTime.hour = DateTime.getHour();
	SQLTime.minute = DateTime.getMinute();
	SQLTime.second = DateTime.getSecond();
}

void convert(CDateTime& DateTime,const MYSQL_TIME& SQLTime)
{
	/*
	char buffer[40];
	sprintf(buffer,"%04d-%02d-%02d %02d:%02d:%02d",SQLTime.year,SQLTime.month,SQLTime.day,SQLTime.hour,SQLTime.minute,SQLTime.second);
	DateTime.parse(buffer,"YYYY-MM-DD hh:mm:ss");
	*/
	if ( SQLTime.year == 0 || SQLTime.month == 0 || SQLTime.day == 0 )
		DateTime = CDateTime((uint64)0);
	else
	{
		DateTime.init(SQLTime.year,SQLTime.month,SQLTime.day,SQLTime.hour,SQLTime.minute,SQLTime.second,0);
	}
}

/*
enum EDATATYPE
{
	TYPE_INT,
	TYPE_STRING,
	TYPE_DATE,
	TYPE_TIME,
	TYPE_TIMESTAMP,
	TYPE_LONG64,
	TYPE_DOUBLE,
};
*/

//类型转换表
const int types[][2] = {
	{MYSQL_TYPE_VAR_STRING,TYPE_STRING},
	{MYSQL_TYPE_STRING,TYPE_STRING},
	{MYSQL_TYPE_LONG,TYPE_INT},
	{MYSQL_TYPE_LONGLONG,TYPE_LONG64},
	{MYSQL_TYPE_DOUBLE,TYPE_DOUBLE},
	{MYSQL_TYPE_FLOAT,TYPE_DOUBLE},
	{MYSQL_TYPE_DATETIME,TYPE_DATE},
	{MYSQL_TYPE_DATE,TYPE_DATE},
	{MYSQL_TYPE_TIME,TYPE_TIME},
	{MYSQL_TYPE_DATETIME,TYPE_TIMESTAMP},
	{MYSQL_TYPE_TIMESTAMP,TYPE_TIMESTAMP},
	{MYSQL_TYPE_TINY,TYPE_CHAR},
	{MYSQL_TYPE_BLOB,TYPE_BLOB},
	{MYSQL_TYPE_TINY_BLOB,TYPE_BLOB},
//	{MYSQL_TYPE_BIT,TYPE_BLOB},
	{MYSQL_TYPE_LONG_BLOB,TYPE_BLOB},
	{MYSQL_TYPE_MEDIUM_BLOB,TYPE_BLOB},
	{MYSQL_TYPE_SHORT,TYPE_SHORT},
	};

enum_field_types getDBType(EDATATYPE nDataType)
{
	for ( size_t i = 0 ; i < sizeof(types) / sizeof(int[2]) ; i ++ )
	{
		if ( types[i][1] == nDataType )
			return (enum_field_types)types[i][0];
	}
	ThrowSQLException<CSQLException>(ERROR_DATABASE_INVALIDDATATYPE,"DBType Un Supported[%d]",nDataType);
	//没意义的返回，只是不让编译时出warning而已。
	return MYSQL_TYPE_GEOMETRY;
}

EDATATYPE getDataType(enum_field_types nSqlType)
{
	for ( size_t i = 0 ; i < sizeof(types) / sizeof(int[2]) ; i ++ )
	{
		if ( types[i][0] == nSqlType )
			return (EDATATYPE)types[i][1];
	}
	ThrowSQLException<CSQLException>(ERROR_DATABASE_INVALIDDATATYPE,"DataType Un Supported[%d]",nSqlType);
	//没意义的返回，只是不让编译时出warning而已。
	return TYPE_UNKNOW;
}

