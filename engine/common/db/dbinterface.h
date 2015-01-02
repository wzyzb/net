#ifndef _DBINTERFACE_H_
#define _DBINTERFACE_H_

#include "dbpublic.h"

//抽象类
#include "resultset.h"
#include "statement.h"
#include "callablestatement.h"
#include "connection.h"
#include "sqlexception.h"

//连接管理
#include "connectionmanager.h"

//连接池
#include "poolconnautoptr.h"
#include "connectionpool.h"
#include "poolconnection.h"

typedef CAutoPtr<CStatement> CStatementPtr;
typedef CAutoPtr<CConnection> CConnectionPtr;
typedef CAutoPtr<CResultSet> CResultSetPtr;
typedef CPoolConnAutoPtr<CPoolConnection> CPoolConnectionPtr;

#endif
