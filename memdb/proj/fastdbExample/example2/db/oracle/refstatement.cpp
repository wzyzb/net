#include <db/oracle/oraclepublic.h>
#include <db/oracle/refconnection.h>
#include <db/oracle/refstatement.h>

avocado_oracle::CRefStatement::CRefStatement(oracle::occi::Connection* pConn,oracle::occi::Statement* pStmt/*,CRefConnection* pRefConn*/)
:m_pConn(pConn),m_pStmt(pStmt)//,m_pRefConn(pRefConn)
{
	/*
	if (this->m_pRefConn!=NULL)
	{
		this->m_pRefConn->incCount();
	}
	*/
}

avocado_oracle::CRefStatement::~CRefStatement()
{
	BEGIN_DBTRY
	//this->m_pConn->terminateStatement(this->m_pStmt);
	END_DBTRY
}

void avocado_oracle::CRefStatement::close()
{
	try
	{
		if (m_pConn!=NULL)
		{	
			this->m_pConn->terminateStatement(this->m_pStmt);
		}
	}
	catch (...)
	{
#ifdef WIN32
		//e;
#endif
	}
}

//增加引用个数
unsigned long avocado_oracle::CRefStatement::incCount()
{
	//this->m_pRefConn->incCount();
	return this->CRefObject::incCount();		
}

//减少引用个数
bool avocado_oracle::CRefStatement::decCount()
{
	--m_nRefCount;
	if (m_nRefCount==0)
	{
		this->close();		
		//this->m_pRefConn->decCount();
		delete this;
		return true;
	}
	//this->m_pRefConn->decCount();
	return true;
}
