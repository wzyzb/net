// µœ÷¿‡
#include "../public.h"
#include "poolconnautoptr.h"
#include "connectionpool.h"
#include "connectionmanager.h"

CAutoPtr <CConnectionPool> CConnectionManager::getConnectionPool(const char* pConnectionURL,int nMaxConnections,const char* UserName,const char* UserPwd)
{
	CConnectionPool* conpool = new CConnectionPool(pConnectionURL,nMaxConnections,UserName,UserPwd);
	CAutoPtr<CConnectionPool> autoptr(conpool);
	return autoptr;
}

