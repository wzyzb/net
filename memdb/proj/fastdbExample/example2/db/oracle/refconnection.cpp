#include <db/oracle/oraclepublic.h>
#include <db/oracle/refconnection.h>

avocado_oracle::CRefConnection::CRefConnection(oracle::occi::Environment* pEnv,oracle::occi::Connection* pConn):m_pEnv(pEnv),m_pConn(pConn)
{
	
}

avocado_oracle::CRefConnection::~CRefConnection()
{
	if (this->m_pEnv != NULL)
	{
		try
		{
		
			if (this->m_pConn != NULL)
			{
				this->m_pEnv->terminateConnection(this->m_pConn);
				this->m_pConn = NULL;
			}
		
			Environment::terminateEnvironment(this->m_pEnv);
			this->m_pEnv = NULL;
		}
		catch (oracle::occi::SQLException& e)
		{
#ifdef WIN32
			e;
#endif
		}
		
	}
}

