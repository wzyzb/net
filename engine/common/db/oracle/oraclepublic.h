#ifndef _ORACLEDBPUBLIC_H_
#define _ORACLEDBPUBLIC_H_

#ifdef WIN32
#pragma warning(disable:4250)
#pragma warning(disable:4244)
#pragma warning(disable:4715)
#pragma warning(disable:4150) // Warning ****************
#endif

#include <common.h>
#include <util/autoptr.h>
#include <util/userexception.h>
#include <util/datetime.h>
#include <db/dbpublic.h>
#include <db/sqlexception.h>
#define WIN32COMMON
#include <occi.h>
using namespace oracle::occi;
#include <db/oracle/refstatement.h>
namespace avocado_oracle
{
//
#define BYTES_MAX_SIZE	512
#define STRING_MAX_SIZE 512

/*
 *	测试OCCI DB 封装类的 用例宏
 *  @x 测试的函数名，出错时显示
 */
#define BEGIN_TEST_TRY(x) \
	try \
{ \

#define END_TEST_TRY(x) \
} \
catch (CSQLException& e) \
{ \
	printf("catch a CSQLException - Method:" #x "(), %s\n", e.what()); \
} \
catch (oracle::occi::SQLException& e) \
{ \
	printf("Error - catch a oracle::occi::SQLException - Method:" #x "(), %s;\n", e.what()); \
	CU_ASSERT(false); \
} \
catch (std::exception& e) \
{ \
	printf("Error - catch a std::exception - Method:" #x "(), %s\n", e.what()); \
	CU_ASSERT(false); \
} \
catch (...) \
{ \
    printf("Error - catch a unhandle exception - Method:" #x "();\n"); \
	CU_ASSERT(false); \
} \

/*
 *捕获oracle异常，抛出内部异常的宏
 */
#define BEGIN_DBTRY \
try \
{\

#define END_DBTRY \
}\
catch (oracle::occi::SQLException& e)\
{\
	throw CSQLException(e.getErrorCode(), e.what());\
}\

// database的类型
/*
class SQLTYPE
{
public:
	static const int SQL_INT;
	static const int SQL_VARCHAR;
	static const int SQL_DATE;
	static const int SQL_TIME;
	static const int SQL_TIMESTAMP;
};
*/

//framework数据类型与oracle数据类型的转换
oracle::occi::Type toOracleType(EDATATYPE nType);

//OCCI类型与framework类型的转换
EDATATYPE toOracleType(oracle::occi::Type oraType);

//framework数据类型与OCCI类型的转换
oracle::occi::Type toOcciType(EDATATYPE nType);

//OCCI类型与db包类型的转换
EDATATYPE toOcciType(oracle::occi::Type oraType);


//Bytes 与 Long64 的类型转换
Long64 bytesToLong64(Bytes& bt);

/*
 *
 *字符串大小写转换
 *
const char* strToUpper(const char* pSrc);
const char* strtoLower(const char* pSrc);
*/
}
#endif 
