#ifndef _REFSTATEMENT_H_
#define _REFSTATEMENT_H_

#include <util/refobject.h>

namespace avocado_oracle
{
class CRefConnection;
/**
 * 关联statemnet
 * 此类保存occi::Statement的指针
 * 并维护occi::Statement的生命周期，只有在引用都被解除后，occi::Statement才会被关闭
 */
class CRefStatement:public CRefObject
{
	//在CResultSetImpl析构时，需要把CStatementImpl中指向自己的指针清空，所以，在这里需要访问m_pStmt
	friend class CResultSetImpl;
private:	
	oracle::occi::Connection* m_pConn;	
	oracle::occi::Statement* m_pStmt;
	//CRefConnection* m_pRefConn;

	void close();
public:
	//构造函数
	CRefStatement(oracle::occi::Connection* pConn,oracle::occi::Statement* pStmt/*,CRefConnection* pRefConn*/);
	
	//析构函数
	virtual ~CRefStatement();

	//增加引用个数
	unsigned long incCount();
	
	//减少引用个数
	bool decCount();	
};
}

#endif