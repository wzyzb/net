#ifndef _REFCONNECTION_H_
#define _REFCONNECTION_H_

#include <util/refobject.h>

class oracle::occi::Statement;
namespace avocado_oracle
{
/**
 * 关联connection
 * 此类保存occi::Connection的指针
 * 并维护occi::Connection的生命周期，只有在引用都被解除后，occi::Connection才会被关闭
 */
class CRefConnection:public CRefObject
{
private:
	oracle::occi::Environment* m_pEnv;
	oracle::occi::Connection* m_pConn;		
public:
	//构造函数
	CRefConnection(oracle::occi::Environment* pEnv,oracle::occi::Connection* pConn);
	

	//析构函数
	virtual ~CRefConnection();
};
}

#endif
