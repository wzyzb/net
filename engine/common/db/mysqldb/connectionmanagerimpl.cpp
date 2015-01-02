#include "../../public.h"
#include "../../util/autoptr.h"
#include "connectionimpl.h"
#include "../connectionmanager.h"

#include "../sqlexception.h"
#include "../dbpublic.h"

/*
#include <public.h>
#include <util/autoptr.h>
#include "connectionimpl.h"
#include <db/connectionmanager.h>

#include <db/sqlexception.h>
#include <db/dbpublic.h>
*/

using namespace mysql;

void parseProtocol(std::vector<string>& Params,const char* pConnectionURL)
{
	const char* p,*begin;;
	begin = p = pConnectionURL;
	while ( *p )
	{
		if ( ':' == (*p) || '/' == (*p) )
		{
			//
			if ( p - begin > 0 )
			{
				Params.push_back(string(begin,p-begin));
			}
			begin = p + 1;
		}
		p++;
	}
	if ( p - begin > 1 )
	{
		Params.push_back(string(begin,p-begin));
	}
}

//url - mysql://host:port/database
CAutoPtr<CConnection> CConnectionManager::getConnection(const char* pConnectionURL,const char* UserName,const char* UserPwd)
{
	vector<string> params;
	parseProtocol(params,pConnectionURL);
	if ( params.size() < 2 )
		throw CSQLException(ERROR_DATABASE_INVALIDPROTOCOL,"Unknow Protocol");
	vector<string>::iterator iparam = params.begin();
	if ( strcasecmp((*iparam++).c_str(),"mysql") != 0 )
		throw CSQLException(ERROR_DATABASE_INVALIDPROTOCOL,"Unknow Protocol");
	string host = (*iparam++);
	string database;
	short port = 0;
	//»ñÈ¡port¿Ú
	if ( iparam != params.end() && isdigit((iparam->c_str()[0])) )
		port = atoi((*iparam++).c_str());
	if ( iparam != params.end() )
		database = (*iparam);
	CAutoPtr<CConnectionImpl> conn = new CConnectionImpl(host.c_str(),port,database.c_str(),UserName,UserPwd);
	if ( !conn->connect() )
		throw CSQLException(conn->getErrNo(),conn->getError());
	return conn.release();
}
