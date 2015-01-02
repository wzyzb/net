#include <db/oracle/oracleinterface.h>
#include <db/oracle/connectionimpl.h>
#include <db/connectionmanager.h>
#include <db/oracle/refconnection.h>

CAutoPtr<CConnection> CConnectionManager::getConnection(const char* pConnectionURL,const char* UserName,const char* UserPwd)
{
	return new avocado_oracle::CConnectionImpl(UserName,UserPwd,pConnectionURL);	
}
