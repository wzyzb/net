#include "public.h"
#include "dao.h"
//#include <io/bytesbuffer.h>
#include "daostruct.h"
#include "daoimpl.h"
#include "daodatabase.h"

#ifdef WINDOWS
#pragma warning(disable:4297)
#else
#include <assert.h>
#endif

#ifndef max
#define max(a,b) ((a>b)?a:b)
#endif

#ifdef __cplusplus
extern "C" {
#endif
void throwException(const char* fmt,...)
{

	char buffer[200];
	va_list list;
	va_start(list,fmt);
#ifdef WIN32
	int retval = _vsnprintf(buffer,sizeof(buffer)-1,fmt,list);
	if ( retval > 0 )
	{
		buffer[retval] = 0;
	}
	else if ( retval == -1 )
	{
		buffer[sizeof(buffer)-1] = 0;
	}
#elif defined GCC
	//linux下自动增加0在最后面
	int retval = vsnprintf(buffer,sizeof(buffer),fmt, list);
#else
	int retval = vsnprintf(buffer,sizeof(buffer)-1,fmt,list);
	if ( retval > 0 )
	{
		buffer[retval] = 0;
	}
	else if ( retval == -1 )
	{
		buffer[sizeof(buffer)-1] = 0;
	}
#endif

	buffer[sizeof(buffer)-1] = 0 ;
	va_end(list);
	throw CDaoException(buffer);
}
#ifdef __cplusplus
}
#endif

/*

		Class	CDao

*/
CDao::CDao()
{
	m_pTranImpl = CDaoDatabase::getInstanceImpl().getLocalTransaction();
	m_nRowCount = 0;
}

CDao::~CDao()
{
}

bool CDao::prepare(const char *sql)
{
	if ( m_pTranImpl == NULL )
		return false;
	m_nRowCount = 0;
	return ( NULL != ( m_pQuery = m_pTranImpl->prepare(sql,this)) );
}

bool CDao::find()
{
	if ( m_pTranImpl == NULL )
		return false;
	this->m_nRowCount = m_pTranImpl->find(this);
	if ( m_nRowCount == 0 )
		return false;
	return true;
}

bool CDao::next()
{
	if ( m_pTranImpl == NULL )
		return false;
	return m_pTranImpl->next(this);
}

bool CDao::insert()
{
	if ( m_pTranImpl == NULL )
		return false;
	return m_pTranImpl->insert(this);
}

bool CDao::remove()
{
	if ( m_pTranImpl == NULL )
		return false;
	return m_pTranImpl->remove(this);
}

bool CDao::update()
{
	if ( m_pTranImpl == NULL )
		return false;
	return m_pTranImpl->update(this);
}

void CDao::setInt(int Index,int32 nValue)
{
	if ( m_pQuery == NULL )
		ThrowException<CDaoException>("not prepare or prepare failed");
	m_pQuery->setQInt(Index,nValue);
}

void CDao::setInt64(int Index,int64 nValue)
{
	if ( m_pQuery == NULL )
		ThrowException<CDaoException>("not prepare or prepare failed");
	m_pQuery->setQInt64(Index,nValue);
}

void CDao::setInt16(int Index,int16 nValue)
{
	if ( m_pQuery == NULL )
		ThrowException<CDaoException>("not prepare or prepare failed");
	m_pQuery->setQInt16(Index,nValue);
}

void CDao::setString(int Index,const string& Value)
{
	if ( m_pQuery == NULL )
		ThrowException<CDaoException>("not prepare or prepare failed");
	m_pQuery->setQString(Index,Value);
}

void CDao::setDateTime(int Index,const CDateTime& Value)
{
	if ( m_pQuery == NULL )
		ThrowException<CDaoException>("not prepare or prepare failed");
	m_pQuery->setQDateTime(Index,Value);
}

void CDao::setBytes(int Index,const void* pValue,int nLength)
{
	if ( m_pQuery == NULL )
		ThrowException<CDaoException>("not prepare or prepare failed");
	m_pQuery->setQBytes(Index,(const unsigned char*)pValue,nLength);
}

void CDao::setUInt8(int Index,uint8 Value)
{
	if ( m_pQuery == NULL )
		ThrowException<CDaoException>("not prepare or prepare failed");
	m_pQuery->setQUInt8(Index,Value);
}

void CDao::setUInt16(int Index,uint16 Value)
{
	if ( m_pQuery == NULL )
		ThrowException<CDaoException>("not prepare or prepare failed");
	m_pQuery->setQUInt16(Index,Value);
}

void CDao::setUInt32(int Index,uint32 Value)
{
	if ( m_pQuery == NULL )
		ThrowException<CDaoException>("not prepare or prepare failed");
	m_pQuery->setQUInt32(Index,Value);
}

void CDao::setUInt64(int Index,uint64 Value)
{
	if ( m_pQuery == NULL )
		ThrowException<CDaoException>("not prepare or prepare failed");
	m_pQuery->setQUInt64(Index,Value);
}

int CDao::getRowCount() const
{
	return this->m_nRowCount;
}


/*

		class	CDaoException

*/
CDaoException::CDaoException(const string& Message ) : CCException(Message)
{

}

CDaoException::~CDaoException() throw()
{

}

/*

		Class	CDaoTransaction

*/
CDaoTransaction::CDaoTransaction(TRANSACTION_TYPE TransType)
{
	//创建新的事务实例
	m_pTranImpl = CDaoDatabase::getInstanceImpl().createTransaction(TransType);
	//保存上一级事务
	m_pOldTranImpl = CDaoDatabase::getInstanceImpl().setLocalTransaction(m_pTranImpl);
	m_bHasBegin = false;
}

CDaoTransaction::~CDaoTransaction() 
{
	try { rollback(); } catch (exception& e) { }
	CDaoDatabase::getInstanceImpl().setLocalTransaction(m_pOldTranImpl);
	delete m_pTranImpl;
}

bool CDaoTransaction::begin()
{
	if ( m_bHasBegin )
		ThrowException<CDaoException>("transactoin has already begin");
	m_bHasBegin = true;
	return m_pTranImpl->begin();
}

bool CDaoTransaction::rollback()
{
	//rollback会经常调用，不需要抛异常
	if ( ! m_bHasBegin )
		return false;
	m_bHasBegin = false;
	return m_pTranImpl->rollback();
}

bool CDaoTransaction::commit()
{
	if ( ! m_bHasBegin )
		ThrowException<CDaoException>("transactoin has not begin");
	m_bHasBegin = false;
	return m_pTranImpl->commit();
}

/*

		Class	CDaoTransactionImpl

*/
CDaoTransactionImpl::CDaoTransactionImpl(TRANSACTION_TYPE TransType) : m_TransType(TransType)
{
}

CDaoTransactionImpl::~CDaoTransactionImpl()
{
}

