#if !defined(_MYSQLPUBLIC_H_)
#define _MYSQLPUBLIC_H_

#include "../../public.h"
#include "../../exception/myException.h"
#include "../../util/datetime.h"
#include <mysql.h>
#include "metadata.h"

//把普通日期转换成数据库日期
void convert(MYSQL_TIME& SQLTime,const CDateTime& DateTime);
//把数据库日期转换成普通日期
void convert(CDateTime& DateTime,const MYSQL_TIME& SQLTime);
//获取数据库的类型标识
enum_field_types getDBType(EDATATYPE nDataType);
//获取普通的类型标识
EDATATYPE getDataType(enum_field_types nSqlType);

#endif
