#include <dao/fastdb/fastdbdaoimpl.h>
#include <util/tlobject.h>
using namespace dao_fastdb;
#include <cli.h>
#ifndef max
#define max(a,b) ( a > b ? a : b )
#endif

#ifdef WIN32

#else
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ACCESS_PERMISSION_MASK 0666
#endif

const char* getJoinStr(int JoinMode)
{
	switch ( JoinMode )
	{
	case JOIN_AND :
		return "and";
	case JOIN_OR :
		return "or";
	}
	return "";
}

const char* getCompStr(PARAM_OPERMODE OperMode)
{
	switch ( OperMode )
	{
	case OPER_EQUAL :
		return "=";
	case OPER_LT :
		return "<";
	case OPER_LET :
		return "<=";
	case OPER_BT :
		return ">";
	case OPER_BET :
		return ">=";
	case OPER_NOEQL :
		return "!=";
	case OPER_BETWEEN :
		return " between ";
	case OPER_IN :
		return " in (";
	case OPER_LIKE :
		return "like";
	}
	return "";
}

/*
	Class	CDaoQueryComplierImpl

*/
CFastDBDaoQueryImpl::CFastDBDaoQueryImpl()
{
	this->m_isCompiled = false;
	m_isFastDBCompiled = false;
}

CFastDBDaoQueryImpl::~CFastDBDaoQueryImpl()
{
	clear();
}

void CFastDBDaoQueryImpl::clear()
{
	for (  vector<dao_fastdb::fdbparam>::iterator i = m_Conditions.begin(); i != m_Conditions.end() ; i ++ )
		delete (*i).Param;
	m_Conditions.clear();
	m_Query.reset();
	m_Params.clear();
	m_isCompiled = false;
	m_isFastDBCompiled = false;
}

void CFastDBDaoQueryImpl::compile(const dao_statement& Statement,const char* pSQL)
{
	clear();
	int nParamIndex = 0;
	char sql[100];
	int prevjoinmode = -1;
	if ( Statement.order.order_fields.size() != 0 )
	{
		//组织order
		m_Order = "order by";
		bool isfirst = true;
		for ( list<dao_order_field>::const_iterator i = Statement.order.order_fields.begin() ; i != Statement.order.order_fields.end() ; i ++ )
		{
			if ( isfirst )
			{
				isfirst = false;
				sprintf(sql," %s %s",(*i).fieldname.c_str(),(*i).ordermode==ORDER_ASC?" asc":" desc");
			}
			else
			{
				sprintf(sql,",%s %s",(*i).fieldname.c_str(),(*i).ordermode==ORDER_ASC?" asc":" desc");
			}
			m_Order += sql;
		}
	}
	for ( list<dao_expression>::const_iterator i = Statement.expressions.begin() ; i != Statement.expressions.end() ; i ++ )
	{
		if ( (*i).compmode == OPER_BETWEEN || (*i).compmode == OPER_IN )
			ThrowException<CDaoException>("dao_fastdb::CDaoQueryUtil not support between keyword");
		sprintf(sql," %s %s %s ",getJoinStr(prevjoinmode),(*i).name.c_str(),getCompStr((*i).compmode));
		fdbparam param;
		param.Param = new dao_bind_param();
		param.sql = sql;
		prevjoinmode = (*i).joinmode;
		list<dao_exp_param>::const_iterator j = (*i).params.begin();
		if ( (*j).isBindParam )
		{
			param.Param->ParamType = TYPE_UNKNOW;
			nParamIndex ++;
			m_Params.push_back(param.Param);
		}
		else
		{
			switch ( (*j).datatype )
			{
			case TYPE_INT:
				param.Param->ParamType = TYPE_INT;
				param.Param->Data.number = (*j).value.number;
				break;
			case TYPE_STRING:
				param.Param->ParamType = TYPE_STRING;
				param.Param->Data.pdata = (char*)malloc((*j).datasize);
				param.Param->DataSize = (*j).datasize;
				memcpy(param.Param->Data.pdata,(*j).value.str,param.Param->DataSize);
				break;
			}
		}
		m_Conditions.push_back(param);
	}
	m_isCompiled = true;
}

int32 CFastDBDaoQueryImpl::getBindParamCount()
{
	return this->m_Params.size();
}

dbQuery& CFastDBDaoQueryImpl::getQuery()
{
	if ( this->m_isFastDBCompiled )
	{
		return m_Query;
	}
	bool first = true;
	//
	for (  vector<fdbparam>::iterator i = m_Conditions.begin(); i != m_Conditions.end() ; i ++ )
	{
		if ( first )
		{
			m_Query = (*i).sql.c_str();
			first = false;
		}
		else
			m_Query ,(*i).sql.c_str();
		switch ( (*i).Param->ParamType )
		{
		case TYPE_STRING :
		case TYPE_BLOB :
			m_Query,(*i).Param->Data.pdata;
			break;
		case TYPE_LONG64 :
			m_Query,(*i).Param->Data.bigint;
			break;
		case TYPE_INT :
		case TYPE_TIMESTAMP :
		case TYPE_SHORT :
			m_Query,(*i).Param->Data.number;
			break;
		//fastdb使用了引用，所以不能用强制转换
		case TYPE_UINT8 :
			m_Query,(*i).Param->Data.utinyint;
			break;
		case TYPE_UINT16 :
			m_Query,(*i).Param->Data.usmallint;
			break;
		case TYPE_UINT32 :
			m_Query,(*i).Param->Data.unumber;
			break;
		case TYPE_UINT64 :
			m_Query,(*i).Param->Data.ubigint;
			break;
		default :
			ThrowException<CDaoException>("UnSupport Type[%d]",(*i).Param->ParamType);
		}
	}
	if ( m_Order.length() > 0 )
	{
		//增加order的处理
		m_Query,m_Order.c_str();
	}
	m_isFastDBCompiled = true;
	return m_Query;
}

void CFastDBDaoQueryImpl::setInt(int Index,int32 nValue)
{
	dao_bind_param* p = getBindParam(Index);
	if ( p->ParamType == TYPE_BLOB ||  p->ParamType == TYPE_STRING )
	{
		free(p->Data.pdata);
		m_isFastDBCompiled = false;
	}
	p->ParamType = TYPE_INT;
	p->Data.number = nValue;
}

void CFastDBDaoQueryImpl::setInt64(int Index,int64 nValue)
{
	dao_bind_param* p = getBindParam(Index);
	if ( p->ParamType == TYPE_BLOB ||  p->ParamType == TYPE_STRING )
	{
		free(p->Data.pdata);
		m_isFastDBCompiled = false;
	}
	p->ParamType = TYPE_LONG64;
	p->Data.bigint = nValue;
}

void CFastDBDaoQueryImpl::setInt16(int Index,int16 nValue)
{
	dao_bind_param* p = getBindParam(Index);
	if ( p->ParamType == TYPE_BLOB ||  p->ParamType == TYPE_STRING )
	{
		free(p->Data.pdata);
		m_isFastDBCompiled = false;
	}
	p->ParamType = TYPE_INT;
	p->Data.number = nValue;
}

void CFastDBDaoQueryImpl::setUInt8(int Index,uint8 nValue)
{
	dao_bind_param* p = getBindParam(Index);
	if ( p->ParamType == TYPE_BLOB ||  p->ParamType == TYPE_STRING )
	{
		free(p->Data.pdata);
		m_isFastDBCompiled = false;
	}
	p->ParamType = TYPE_UINT8;
	p->Data.utinyint = nValue;
}

void CFastDBDaoQueryImpl::setUInt16(int Index,uint16 nValue)
{
	dao_bind_param* p = getBindParam(Index);
	if ( p->ParamType == TYPE_BLOB ||  p->ParamType == TYPE_STRING )
	{
		free(p->Data.pdata);
		m_isFastDBCompiled = false;
	}
	p->ParamType = TYPE_UINT16;
	p->Data.usmallint = nValue;
}

void CFastDBDaoQueryImpl::setUInt32(int Index,uint32 nValue)
{
	dao_bind_param* p = getBindParam(Index);
	if ( p->ParamType == TYPE_BLOB ||  p->ParamType == TYPE_STRING )
	{
		free(p->Data.pdata);
		m_isFastDBCompiled = false;
	}
	p->ParamType = TYPE_UINT32;
	p->Data.unumber = nValue;
}

void CFastDBDaoQueryImpl::setUInt64(int Index,uint64 nValue)
{
	dao_bind_param* p = getBindParam(Index);
	if ( p->ParamType == TYPE_BLOB ||  p->ParamType == TYPE_STRING )
	{
		free(p->Data.pdata);
		m_isFastDBCompiled = false;
	}
	p->ParamType = TYPE_UINT64;
	p->Data.ubigint = nValue;
}

void CFastDBDaoQueryImpl::setString(int Index,const string& Value)
{
	dao_bind_param* p = getBindParam(Index);
	if ( p->DataSize <= Value.length() )
	{
		if ( p->ParamType == TYPE_BLOB ||  p->ParamType == TYPE_STRING )
			free(p->Data.pdata);
		p->DataSize = Value.length()+1;
		p->Data.pdata = (char*)malloc(max(p->DataSize,1024));
		//重新分配了内存，需重新组织dbQuery
		this->m_isFastDBCompiled = false;
	}
	p->ParamType = TYPE_STRING;
	uint32 copylen = min( ((uint32)p->DataSize-1), ((uint32)Value.length()) );
	memcpy(p->Data.pdata,Value.c_str(),copylen);
	((char*)p->Data.pdata)[copylen] =0;
}

void CFastDBDaoQueryImpl::setDateTime(int Index,const CDateTime& Value)
{
	dao_bind_param* p = getBindParam(Index);
	if ( p->ParamType == TYPE_BLOB ||  p->ParamType == TYPE_STRING )
	{
		free(p->Data.pdata);
		m_isFastDBCompiled = false;
	}
	p->ParamType = TYPE_TIMESTAMP;
	p->Data.number = Value.getTotalMills()/1000;
}

void CFastDBDaoQueryImpl::setBytes(int Index,const void* pValue,int nLength)
{
	dao_bind_param* p = getBindParam(Index);
	if ( p->DataSize <= nLength )
	{
		if ( p->ParamType == TYPE_BLOB ||  p->ParamType == TYPE_STRING )
			free(p->Data.pdata);
		p->DataSize = nLength+1;
		p->Data.pdata = (char*)malloc(max(p->DataSize,1024));
		m_isFastDBCompiled = false;
	}
	p->ParamType = TYPE_BLOB;
	memcpy(p->Data.pdata,pValue,p->DataSize-1);
	((char*)p->Data.pdata)[p->DataSize-1] =0;
}

dao_bind_param* CFastDBDaoQueryImpl::getBindParam(int nIndex)
{
	if ( nIndex > m_Params.size() || nIndex <= 0 )
		ThrowException<CDaoException>("Invalid Index ");
	return m_Params[nIndex-1];
	/*
	list<dao_bind_param*>::iterator i = m_Params.begin();
	while ( nIndex > 1 )
	{
		nIndex --;
		i++;
	}
	return (*i);
	*/
}

/*

		Class	CFastDBDaoTransactionImpl

*/
CFastDBDaoTransactionImpl::CFastDBDaoTransactionImpl(CFastDBDaoDatabaseImpl &Database)
{
	m_nSessionID = Database.m_nSessionID;
	m_bHasBegin = false;
}

CFastDBDaoTransactionImpl::~CFastDBDaoTransactionImpl()
{
	rollback();
}

bool CFastDBDaoTransactionImpl::begin()
{
	if ( m_bHasBegin )
		return false;
	cli_attach(m_nSessionID);
	m_bHasBegin = true;
	return true;
}

bool CFastDBDaoTransactionImpl::commit()
{
	if ( ! m_bHasBegin )
		return false;
	m_bHasBegin = false;
	cli_commit(m_nSessionID);
	cli_detach(m_nSessionID,cli_commit_on_detach);
	return true;
}

bool CFastDBDaoTransactionImpl::rollback()
{
	if ( ! m_bHasBegin )
		return true;
	cli_abort(m_nSessionID);
	cli_detach(m_nSessionID,cli_commit_on_detach);
	m_bHasBegin = false;
	return true;
}

void CFastDBDaoTransactionImpl::update(CMDBDaoImpl* pDao)
{
	pDao->updateMDB();
}

void CFastDBDaoTransactionImpl::insert(CMDBDaoImpl* pDao)
{
	pDao->insertMDB();
}

void CFastDBDaoTransactionImpl::remove(CMDBDaoImpl* pDao)
{
	pDao->removeMDB();
}

/*

		Class	CFastDBDaoDatabaseImpl
*/
CFastDBDaoDatabaseImpl::CFastDBDaoDatabaseImpl()
{
	m_nSessionID = -1;
    m_bIsCloseDB = false;
}

CMDBDaoTransactionImpl* CFastDBDaoDatabaseImpl::createTransaction(TRANSACTION_TYPE TransType)
{
	return new CFastDBDaoTransactionImpl(*this);
}

#define MB 1024*1024

bool CFastDBDaoDatabaseImpl::init(const char* DBName,int64 InitSize,int64 MaxSize,int64 IndexSize,int64 IncreSize)
{
	cleanup( DBName );
	m_nSessionID = cli_create(DBName,NULL,0,cli_open_default,(size_t)(InitSize*MB),(size_t)(IncreSize*MB),(size_t)(IndexSize*MB),(size_t)(MaxSize*MB));
	if ( m_nSessionID < 0 )
		ThrowException<CCException>("create memdb error[%d]",m_nSessionID);
	m_bIsCloseDB = true;
	return true;
}

bool CFastDBDaoDatabaseImpl::open( const char* DBName,int64 InitSize/*=1024*/,int64 MaxSize/*=2048*/,int64 IndexSize/*=1024*/,int64 IncreSize/*=20*/ )
{
    m_nSessionID = cli_create(DBName,NULL,0,cli_open_default,InitSize*MB,IncreSize*MB,IndexSize*MB,MaxSize*MB);
    if ( m_nSessionID < 0 )
        ThrowException<CCException>("open memdb error[%d]",m_nSessionID);
	m_bIsCloseDB = false;
    return true;
}

CFastDBDaoDatabaseImpl::~CFastDBDaoDatabaseImpl()
{
	if ( m_nSessionID >= 0 && m_bIsCloseDB )
		cli_close(m_nSessionID);
}

void CFastDBDaoDatabaseImpl::cleanup( const char* DBName )
{
    removeSharedMemory(DBName, ".dm"); 
    removeSharedMemory(DBName, ".fdb");
    removeSemaphore(DBName, ".in");    
    removeSemaphore(DBName, ".ws");    
    removeSemaphore(DBName, ".rs");    
    removeSemaphore(DBName, ".us");    
    removeSemaphore(DBName, ".bce");
    removeSemaphore(DBName, ".dce");
    removeSemaphore(DBName, ".cs");
    removeSemaphore(DBName, ".mcs");    
}

int CFastDBDaoDatabaseImpl::hashFunction( char const* s )
{
    int ch, h = 0;    
    while ((ch = *s++) != '\0') { 
        h ^= ch;
    }
    h &= 0xFF;
    if (h == 0) { 
        h = 1;
    }
    return h;
}

int CFastDBDaoDatabaseImpl::getKey( char const* databaseName, char const* suffix )
{
#ifdef WIN32
    return 0;
#else
    static char const* keyFileDir = "/tmp/";

    char buf[1024];
    *buf = '\0';
    if (strchr(databaseName, '/') == NULL) { 
        strcpy(buf, keyFileDir);
    }
    strcat(buf, databaseName);
    strcat(buf, suffix);
#if defined(USE_STD_FTOK) 
    return ftok(buf, hashFunction(buf));
#else
    struct stat st;
    if (::stat(buf, &st) < 0) { 
        return (key_t)-1;
    }
    return (key_t)(((st.st_dev & 0x7f) << 24) ^ (st.st_ino & 0x7fffffff));
#endif
#endif
}

void CFastDBDaoDatabaseImpl::removeSemaphore( char const* databaseName, char const* suffix )
{
    int key = getKey(databaseName, suffix);
#ifdef WIN32

#else
    if (key >= 0) { 
        int id = semget(key, 0, 0);
        if (id >= 0) { 
            int rc = semctl(id, 0, IPC_RMID, NULL);
            if (rc < 0) { 
                fprintf(stderr, "Failed to remove semaphore %s%s errno:%d\n", databaseName, suffix, errno);
            }
        } else { 
            fprintf(stderr, "Failed to get key for semaphore %s%s errno:%d\n", databaseName, suffix, errno);
        }
    }
#endif
}

void CFastDBDaoDatabaseImpl::removeSharedMemory( char const* databaseName, char const* suffix )
{
    int key = getKey(databaseName, suffix);
#ifdef WIN32

#else
    if (key >= 0) 
    { 
        size_t size = DOALIGN(sizeof(dbMonitor), 4096);
        int id = shmget(key, size, ACCESS_PERMISSION_MASK);
        if (id >= 0) 
        {
            char* paddr = NULL;
            paddr = (char*)shmat(id, 0, 0);
            shmdt((char*)paddr);
            int rc = shmctl(id, IPC_RMID, NULL);
            if (rc < 0) 
            { 
                fprintf(stderr, "Failed to remove schared section %s%s errno:%d\n", databaseName, suffix, errno);
            }
        } 
        else 
        { 
            fprintf(stderr, "Failed to get key for sharedmemory %s%s errno:%d\n", databaseName, suffix, errno);
        }
    }
#endif
}

/*

		Class	CCompiledQueryManager

*/
CCompiledQueryManager::CCompiledQueryManager()
{
}

bool CCompiledQueryManager::getPreCompiledData(const string& SQL,const string& TableName,CFastDBDaoQueryImpl*& pQuery)
{
	string key = TableName;
	key += SQL;
	map<string,compliedquery>::iterator i = m_CompiledQuerys.find(key);
	if ( i == m_CompiledQuerys.end() )
		return false;
	(*i).second.usedtimes ++;
	pQuery = (*i).second.query;
	return true;
}

bool CCompiledQueryManager::setPreCompiledData(const string& SQL,const string& TableName,CFastDBDaoQueryImpl* pQuery)
{
	compliedquery query;
	query.usedtimes = 1;
	query.query = pQuery;
	string key = TableName;
	key += SQL;
	map<string,compliedquery>::iterator i = m_CompiledQuerys.find(key);
	if ( i != m_CompiledQuerys.end() )
	{
		delete (*i).second.query;
		(*i).second.query = pQuery;
		return false;
	}
	else
		m_CompiledQuerys.insert(map<string,compliedquery>::value_type(key,query));
	return true;
}

/*

		Class	CFastDBDaoImpl

*/
CFastDBDaoImpl::CFastDBDaoImpl()
{
	m_pQueryImpl = NULL;
}

CFastDBDaoImpl::~CFastDBDaoImpl()
{
	clear();
}

void CFastDBDaoImpl::clear()
{
}

bool CFastDBDaoImpl::prepareMDB(const char* sql)
{
	if ( CCompiledQueryManager::getInstance()->getPreCompiledData(sql,m_Name,this->m_pQueryImpl) )
		return true;
	CDaoSQLParser parser;
	parser.prepare(sql);
	m_pQueryImpl = new CFastDBDaoQueryImpl();
	m_pQueryImpl->compile(parser.m_Statement,sql);
	CCompiledQueryManager::getInstance()->setPreCompiledData(sql,m_Name,this->m_pQueryImpl);
	return true;
}

int CFastDBDaoImpl::findMDB()
{
	if ( m_pQueryImpl == NULL )
		ThrowException<CDaoException>("not prepared");
	return select(m_pQueryImpl->getQuery());
}

void CFastDBDaoImpl::setQString(int Index,const string& Value)
{
	assert(m_pQueryImpl!=NULL);
	m_pQueryImpl->setString(Index,Value);
}

void CFastDBDaoImpl::setQInt64(int Index,int64 Value)
{
	assert(m_pQueryImpl!=NULL);
	m_pQueryImpl->setInt64(Index,Value);
}

void CFastDBDaoImpl::setQInt(int Index,int32 Value)
{
	assert(m_pQueryImpl!=NULL);
	m_pQueryImpl->setInt(Index,Value);
}

void CFastDBDaoImpl::setQInt16(int Index,int16 Value)
{
	assert(m_pQueryImpl!=NULL);
	m_pQueryImpl->setInt16(Index,Value);
}


void CFastDBDaoImpl::setQUInt8(int Index,uint8 Value)
{
	assert(m_pQueryImpl!=NULL);
	m_pQueryImpl->setUInt8(Index,Value);
}

void CFastDBDaoImpl::setQUInt16(int Index,uint16 Value)
{
	assert(m_pQueryImpl!=NULL);
	m_pQueryImpl->setUInt16(Index,Value);
}


void CFastDBDaoImpl::setQUInt32(int Index,uint32 Value)
{
	assert(m_pQueryImpl!=NULL);
	m_pQueryImpl->setUInt32(Index,Value);
}

void CFastDBDaoImpl::setQUInt64(int Index,uint64 Value)
{
	assert(m_pQueryImpl!=NULL);
	m_pQueryImpl->setUInt64(Index,Value);
}

void CFastDBDaoImpl::setQBytes(int Index,const void* pValue,int nLength)
{
	assert(m_pQueryImpl!=NULL);
	m_pQueryImpl->setBytes(Index,pValue,nLength);
}

void CFastDBDaoImpl::setQDateTime(int Index,const CDateTime& Value)
{
	assert(m_pQueryImpl!=NULL);
	m_pQueryImpl->setDateTime(Index,Value);

}
