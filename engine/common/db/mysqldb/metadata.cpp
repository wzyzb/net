#include "../../public.h"
#include "mysqlpublic.h"
#include "metadata.h"

using namespace mysql;

/*

		Class	CMetaData

*/
/*
CMetaData::CMetaData()
{
	memset(&m_Param,0,sizeof(m_Param));
	//m_Param.buffer = NULL;
	//m_Param.length = NULL;
	//m_Param.is_null = NULL;
}

CMetaData::~CMetaData()
{
	clear();
}

MYSQL_BIND* CMetaData::MYSQL_BIND1()
{
	return &m_Param;
}

void CMetaData::clear()
{
	if ( NULL != m_Param.buffer )
	{
		free(m_Param.buffer);
		m_Param.buffer = NULL;
	}
	if ( NULL != m_Param.length )
	{
		delete m_Param.length;
		m_Param.length = NULL;
	}
	if ( NULL != m_Param.is_null )
	{
		delete m_Param.is_null;
		m_Param.is_null = NULL;
	}
}
*/
