#ifndef _DBINTERFACE_H_
#define _DBINTERFACE_H_

#include <db/dbpublic.h>

//抽象类
#include <db/resultset.h>
#include <db/statement.h>
#include <db/callablestatement.h>
#include <db/connection.h>
#include <db/sqlexception.h>

//连接管理
#include <db/connectionmanager.h>

//连接池
#include <db/poolconnautoptr.h>
#include <db/connectionpool.h>
#include <db/poolconnection.h>

typedef CAutoPtr<CStatement> CStatementPtr;
typedef CAutoPtr<CConnection> CConnectionPtr;
typedef CAutoPtr<CResultSet> CResultSetPtr;
typedef CPoolConnAutoPtr<CPoolConnection> CPoolConnectionPtr;

#endif
