#include <dao/mysql/mysqldaoimpl.h>

using namespace dao_mysql;

/*

		Class	CMysqlDaoImpl

*/
CMysqlDaoImpl::CMysqlDaoImpl() : m_QueryStmt(NULL) , m_Resultset(NULL),m_QueryCountStmt(NULL)
{
	m_TableName = NULL;
}

CMysqlDaoImpl::~CMysqlDaoImpl()
{
}

bool CMysqlDaoImpl::prepareDB(const char* Sql,CPoolConnectionPtr& Conn)
{
	char sql[1024],sql_count[1024];
	if ( Sql[0] != 0 )
	{
		snprintf(sql,sizeof(sql)-1,"select * from %s where %s",m_TableName,Sql);
		snprintf(sql_count,sizeof(sql_count)-1,"select count(*) from %s where %s",m_TableName,Sql);
	}
	else
	{
		snprintf(sql,sizeof(sql)-1,"select * from %s",m_TableName);
		snprintf(sql_count,sizeof(sql_count)-1,"select count(*) from %s",m_TableName);
	}
	m_QueryCountStmt = Conn->prepareStatement(sql_count);
	m_QueryStmt = Conn->prepareStatement(sql);
	m_Resultset = NULL;
	m_Conn = Conn;
	return true;
}

void CMysqlDaoImpl::setQInt(int Index,int32 nValue)
{
	m_QueryStmt->setInt(Index,nValue);
	m_QueryCountStmt->setInt(Index,nValue);
}

void CMysqlDaoImpl::setQInt64(int Index,int64 nValue)
{
	m_QueryStmt->setInt64(Index,nValue);
	m_QueryCountStmt->setInt64(Index,nValue);
}

void CMysqlDaoImpl::setQInt16(int Index,int16 nValue)
{
	m_QueryStmt->setInt(Index,nValue);
	m_QueryCountStmt->setInt(Index,nValue);
}

void CMysqlDaoImpl::setQString(int Index,const string& Value)
{
	m_QueryStmt->setString(Index,Value);
	m_QueryCountStmt->setString(Index,Value);
}

void CMysqlDaoImpl::setQDateTime(int Index,const CDateTime& Value)
{
	m_QueryStmt->setTimestamp(Index,Value);
	m_QueryCountStmt->setTimestamp(Index,Value);
}

void CMysqlDaoImpl::setQBytes(int Index,const void* pValue,int nLength)
{
	m_QueryStmt->setBytes(Index,(const unsigned char*)pValue,nLength);
	m_QueryCountStmt->setBytes(Index,(const unsigned char*)pValue,nLength);
}

void CMysqlDaoImpl::setQUInt8(int Index,uint8 Value)
{
	m_QueryStmt->setUInt(Index,Value);
	m_QueryCountStmt->setUInt(Index,Value);
}

void CMysqlDaoImpl::setQUInt16(int Index,uint16 Value)
{
	m_QueryStmt->setUInt(Index,Value);
	m_QueryCountStmt->setUInt(Index,Value);
}

void CMysqlDaoImpl::setQUInt32(int Index,uint32 Value)
{
	m_QueryStmt->setUInt(Index,Value);
	m_QueryCountStmt->setUInt(Index,Value);
}

void CMysqlDaoImpl::setQUInt64(int Index,uint64 Value)
{
	m_QueryStmt->setUInt64(Index,Value);
	m_QueryCountStmt->setUInt64(Index,Value);
}

int CMysqlDaoImpl::findDB()
{
	int count = 0;
	m_Resultset = m_QueryCountStmt->executeQuery();
	if ( m_Resultset->next() )
	{
		//count = atoi(m_Resultset->getString(1).c_str());
		count = (int)m_Resultset->getInt64(1);
	}
	m_Resultset->close();
	m_QueryCountStmt->close();
	//
	m_Resultset = m_QueryStmt->executeQuery();
	return count;
}

bool CMysqlDaoImpl::nextDB()
{
	if ( this->m_Resultset->next() )
	{
		this->readDB(*m_Resultset.get());
		return true;
	}
	return false;
}

bool CMysqlDaoImpl::compare(const CDBDaoImpl* pOther) const
{
	const CMysqlDaoImpl* pdao = dynamic_cast<const CMysqlDaoImpl*>(pOther);
	if ( pdao == NULL )
		return false;

	if ( this->m_TableName < pdao->m_TableName )
		return true;
	else if( this->m_TableName > pdao->m_TableName )
		return false;
	else
		return this->check(pdao);
}

/*

		Class	CMysqlDBDaoTransactionImpl

*/
CMysqlDBDaoTransactionImpl::CMysqlDBDaoTransactionImpl(CConnectionPool& ConnPool) : m_pConnPool(&ConnPool)
{
}

CMysqlDBDaoTransactionImpl::~CMysqlDBDaoTransactionImpl()
{
}

bool CMysqlDBDaoTransactionImpl::begin()
{
	m_pUpdateConn = m_pConnPool->getConnection(1000);
	return ( m_pUpdateConn->begin() );
}

bool CMysqlDBDaoTransactionImpl::commit()
{
	return ( m_pUpdateConn->commit() );
}

bool CMysqlDBDaoTransactionImpl::rollback()
{
	return ( m_pUpdateConn->rollback() );
}

void CMysqlDBDaoTransactionImpl::update(CDBDaoImpl* pDao)
{
	pDao->updateDB(*this->m_pUpdateConn.get());
}

void CMysqlDBDaoTransactionImpl::insert(CDBDaoImpl* pDao)
{
	pDao->insertDB(*this->m_pUpdateConn.get());
}

void CMysqlDBDaoTransactionImpl::remove(CDBDaoImpl* pDao)
{
	pDao->removeDB(*this->m_pUpdateConn.get());
}

void CMysqlDBDaoTransactionImpl::prepare(const char* Sql,CDBDaoImpl* pDao)
{
	CPoolConnectionPtr pconn = m_pConnPool->getConnection(1000);
	pDao->prepareDB(Sql,pconn);
}
