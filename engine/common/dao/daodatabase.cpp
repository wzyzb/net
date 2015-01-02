#include "daodatabase.h"

/*

		Class	CDaoDatabase

*/
CDaoDatabase::~CDaoDatabase()
{
}

bool CDaoDatabase::initialize()
{
	if ( !this->m_LocalTransaction.initialize() )
		return false;
	if ( ! this->m_LocalDBEnv.initialize() )
		return false;
	return true;
}

CDaoTransactionImpl* CDaoDatabase::getLocalTransaction()
{
	return (CDaoTransactionImpl*)m_LocalTransaction.getValue();
}

CDaoTransactionImpl* CDaoDatabase::setLocalTransaction(CDaoTransactionImpl *pTransaction)
{
	CDaoTransactionImpl* pold = getLocalTransaction();
	if ( m_LocalTransaction.setValue(pTransaction) )
		return pold;
	return NULL;
}

void CDaoDatabase::setLocalDBEnv(DBENV EnvType)
{
	m_LocalDBEnv.setValue((void*)EnvType);
}

DBENV CDaoDatabase::getLocalDBEnv()
{
	return (DBENV)(long)(m_LocalDBEnv.getValue());
}
