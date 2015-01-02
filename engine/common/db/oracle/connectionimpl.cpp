#include <util/stringfun.h>
#include <db/oracle/oraclepublic.h>
#include <db/oracle/refconnection.h>
#include <db/oracle/resultsetimpl.h>
#include <db/oracle/statementimpl.h>
#include <db/oracle/callablestatementimpl.h>
#include <db/oracle/connectionimpl.h>
#include <oci.h>

using namespace avocado_oracle;

CConnectionImpl::CConnectionImpl(string UsrName,string UsrPwd,string DataBase)	\
:m_pEnv(NULL),m_pConn(NULL),	\
m_UsrName(UsrName),m_UsrPwd(UsrPwd),m_Database(DataBase),	\
m_AutoCommit(true),m_AutoCommitTmp(true),m_bConnected(false),m_BeginCount(0)
//,m_pRefConn(NULL)
{	
	this->connect();
	//this->m_pRefConn = new CRefConnection(this->m_pEnv,this->m_pConn);	 		
}

CConnectionImpl::~CConnectionImpl()
{
	/*
	if (this->m_pRefConn != NULL)
	{
		this->m_pRefConn->decCount();
	}
	*/
	this->close();
}

CAutoPtr<CConnection> CConnectionImpl::clone()
{
	if ((this->m_UsrName=="")||(this->m_UsrPwd=="")||(this->m_Database==""))
	{
		throw CSQLException(ERROR_DATABASE_INVALIDPROTOCOL, "Connection param error while calling CConnectionImpl::clone");  
	}

	return new CConnectionImpl(m_UsrName,m_UsrPwd,m_Database);
}

CAutoPtr <CStatement> CConnectionImpl::createStatement()
{
	checkConnect("createStatement");
	CStatementImpl* stmti = new CStatementImpl();

	try
	{	
		stmti->m_pStmt = this->m_pConn->createStatement();	
					
	}
	catch (oracle::occi::SQLException& e)
	{
		if (stmti != NULL)
		{
			delete stmti;
		}
		throw CSQLException(e.getErrorCode(), e.what());\
	}
    
	stmti->m_pConn = this;	
	CRefStatement* prefstmt = new CRefStatement(this->m_pConn,stmti->m_pStmt/*,this->m_pRefConn*/);
	stmti->m_pRefStmt = prefstmt;
	CStatement* stmt = stmti;
	CAutoPtr<CStatement> aptr(stmt);
	return aptr;
}


CAutoPtr<CStatement> CConnectionImpl::prepareStatement(const char* pSQL)
{
	checkConnect("prepareStatement");
	CStatementImpl* stmti = new CStatementImpl();
	string stmp = pSQL;
	stmti->m_Sql = StrReplace(stmp,"?",":p");
	try
	{
		stmti->m_pStmt = this->m_pConn->createStatement(stmti->m_Sql);	
	}
	catch (oracle::occi::SQLException& e)
	{
		if (stmti != NULL)
		{
			delete stmti;
		}
		throw CSQLException(e.getErrorCode(), e.what());
	}
	stmti->m_pConn = this;
	CRefStatement* prefstmt = new CRefStatement(this->m_pConn,stmti->m_pStmt/*,this->m_pRefConn*/);
	stmti->m_pRefStmt = prefstmt;
	//prefstmt->incCount();
	CAutoPtr<CStatement> aptr(stmti);
	return aptr;
}

bool CConnectionImpl::testStatement()
{
	oracle::occi::Statement*  stmt=NULL;
	ResultSet* rset=NULL;
	try
	{
		if (this->m_pConn != NULL)
		{			
			OCCI_STD_NAMESPACE::string tmp = "select 1 from dual";
			stmt = this->m_pConn->createStatement(tmp);	
			if (stmt != NULL)
			{
				ResultSet *rset = stmt->executeQuery();
				stmt->closeResultSet (rset);
				this->m_pConn->terminateStatement(stmt);
			}
			else
			{
				return false;
			}			
			
			/*
			OCIError* ociError;
			ub4 serverStatus = -1;
			OCIHandleAlloc( this->m_pEnv->getOCIEnvironment(), (void**)&ociError, OCI_HTYPE_ERROR, 0, NULL );
			OCIAttrGet( m_pConn->getOCIServer(), OCI_HTYPE_SERVER, &serverStatus, 0, OCI_ATTR_SERVER_STATUS, ociError );
			OCIHandleFree( (dvoid*)ociError, OCI_HTYPE_ERROR );
			if ( serverStatus == OCI_SERVER_NOT_CONNECTED )
			{
				//printf("connection is down\n");
				return false;
			}	
			else if (serverStatus == OCI_SERVER_NORMAL)
			{	//printf("connection is up\n");
				return true;
			}
			*/
		}
		else
		{
			return false;
		}
		
	}
	catch (oracle::occi::SQLException& e)
	{
		if (this->m_pConn != NULL)
		{
			if (stmt != NULL)
			{
				if (rset != NULL)
				{
					stmt->closeResultSet (rset);
				}
				this->m_pConn->terminateStatement(stmt);
			}
		}
		
		printf("CConnectionImpl::isClosed errorcode=%d,errmsg=%s",e.getErrorCode(),e.getMessage().c_str());
		this->m_bConnected = false;			
		
		return false;
	}
	
	return true;
}

CAutoPtr< CCallableStatement > CConnectionImpl::prepareCall(const char* pSQL)
{	
	checkConnect("prepareCall");
	CCallableStatementImpl* cstmti = new CCallableStatementImpl();
	string stmp = pSQL;
	cstmti->m_Sql = StrReplaceAddIndex(stmp,"?",":p");	
	try
	{	
		cstmti->m_pStmt = this->m_pConn->createStatement(cstmti->m_Sql);
	}
	catch (oracle::occi::SQLException& e)
	{
		if (cstmti != NULL)
		{
			delete cstmti;
		}
		throw CSQLException(e.getErrorCode(), e.what());\
	}	
	cstmti->m_pConn = this;
	CRefStatement* prefstmt = new CRefStatement(this->m_pConn,cstmti->m_pStmt/*,this->m_pRefConn*/);
	cstmti->m_pRefStmt = prefstmt;
	//prefstmt->incCount();
	CAutoPtr<CCallableStatement> aptr(cstmti);
	return aptr;	
}

void CConnectionImpl::setAutoCommit(bool AutoCommit)
{
	m_AutoCommit = AutoCommit;
}


bool CConnectionImpl::getAutoCommit()
{
	return m_AutoCommit;
}


bool CConnectionImpl::begin()
{
	this->m_BeginCount++;
	this->m_AutoCommitTmp = this->m_AutoCommit;
	this->m_AutoCommit = false;
	
	return true;
}

bool CConnectionImpl::commit()
{
	this->m_BeginCount--;

	if (this->m_BeginCount<=0)
	{
		if (this->m_AutoCommit != this->m_AutoCommitTmp)
		{
			this->m_AutoCommit = this->m_AutoCommitTmp;
		}

		checkConnect("commit");
		try
		{			
			m_pConn->commit();		
			
			return true;
		}
		catch (oracle::occi::SQLException &e)
		{
			throw CSQLException(e.getErrorCode(), e.what());  
		}
	}
	return false;
}

bool CConnectionImpl::rollback()
{
	this->m_BeginCount--;

	if (this->m_BeginCount<=0)
	{
		if (this->m_AutoCommit != this->m_AutoCommitTmp)
		{
			this->m_AutoCommit = this->m_AutoCommitTmp;
		}	
	
		try
		{
			checkConnect("rollback");
				
			m_pConn->rollback();
			
			return true;
		}
		catch (oracle::occi::SQLException &e)
		{
	#ifdef WIN32
			e;
	#endif
			return false;  
		}
		catch (CSQLException& sqle)
		{
	#ifdef WIN32		
			sqle;
	#endif
			return false;
		}
	}
	return false;
}


bool CConnectionImpl::connect()
{	
	try
	{
		this->m_pEnv = Environment::createEnvironment(Environment::OBJECT);  
		this->m_pConn = this->m_pEnv->createConnection (this->m_UsrName, this->m_UsrPwd, this->m_Database);
		this->m_bConnected = true;
	}
	catch (oracle::occi::SQLException& e)
	{
		this->close();

		throw CSQLException(e.getErrorCode(), e.what());		
	}
	return true;
}

void CConnectionImpl::close()
{
	BEGIN_DBTRY;
	if (this->m_pEnv != NULL)
	{
		
		if (this->m_pConn != NULL)
		{
			this->m_pEnv->terminateConnection(this->m_pConn);
			this->m_pConn = NULL;
		}
	
		Environment::terminateEnvironment(this->m_pEnv);		
		this->m_pEnv = NULL;
	}	
	END_DBTRY;
	this->m_bConnected = false;
}

bool CConnectionImpl::checkConnect(std::string MethodName)
{
	if (this->m_pConn == NULL)
	{
		throw CSQLException(ERROR_DATABASE_CLOSED, "connection ptr is NULL,while calling "+ MethodName);
	}
	//else if (this->isClosed())
	{
		//throw CSQLException(ERROR_DATABASE_CLOSED, "connection is closed,while calling "+ MethodName +", please call connect first!");
	}
	return true;
}

bool CConnectionImpl::isClosed()
{
	if ((this->m_pConn == NULL)||(!m_bConnected))
		return true;

	try
	{
		return (!this->testStatement());		
	}
	catch (CSQLException& e)
	{
#ifdef WIN32
		e;
#endif
		return true;
	}

	return false;
}

void CConnectionImpl::setTransactionIsolation(int nLevel)
{
	switch(nLevel)
	{
	case TRANSACTION_NONE:
		//nLevel
		break;
	case TRANSACTION_READ_COMMITTED:
		//nLevel
		break;

	case TRANSACTION_READ_UNCOMMITTED:
		//nLevel
		break;
	case TRANSACTION_REPEATABLE_READ:
		//nLevel
		break;
	case TRANSACTION_SERIALIZABLE:
		//nLevel
		break;
	default:
		break;
	}
}

bool CConnectionImpl::terminateStatement(oracle::occi::Statement*  m_pStmt)
{
	if (this->m_pConn ==NULL)
	{
		return false;
	}
	try
	{
		this->m_pConn->terminateStatement(m_pStmt);
	}
	catch (oracle::occi::SQLException& e)
	{
#ifdef WIN32		
		e;
#endif
		return false;
	}
	return true;
}

oracle::occi::Environment* CConnectionImpl::getEnvironment()
{
	return m_pEnv;
}

//
oracle::occi::Connection* CConnectionImpl::getConnection()
{
	return m_pConn;
}

void CConnectionImpl::keepActive()
{
	checkConnect("keepActive");
	
	this->isClosed();	
}
