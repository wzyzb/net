#include <framework/session.h>
#include <util/refshare.h>

typedef struct _t_sessionvalue
{
	enum
	{
		VALUE_INT32,
		VALUE_INT64,
		VALUE_STRING,
		VALUE_PTR
	}type;
	union
	{
		int32 nNum;
		int64 nBigNum;
		char* pValue;
		IRefShare* pPtr;
	}value;
	_t_sessionvalue()
	{
		value.pValue = NULL;
	}

	_t_sessionvalue(const struct _t_sessionvalue& Other)
	{
		*this = Other;
	}

	_t_sessionvalue& operator= (const struct _t_sessionvalue& Other)
	{
		memcpy(this,&Other,sizeof(struct _t_sessionvalue));
		if ( type == VALUE_STRING )
			((struct _t_sessionvalue&)Other).value.pValue = NULL;
		else if ( type == VALUE_PTR )
			((struct _t_sessionvalue&)Other).value.pPtr = NULL;
		return *this;
	}
	~_t_sessionvalue()
	{
		if ( type == VALUE_STRING && value.pValue != NULL )
			free(value.pValue);
		else if ( type == VALUE_PTR && value.pPtr != NULL )
			value.pPtr->decRef();
	}
}SESSION_VALUE,*PSESSION_VALUE;

CSession::CSession()
{
    reset();
}

CSession::~CSession()
{

}

void CSession::reset()
{
	m_nSessionID = 0;
	m_Values.clear();
}

void CSession::setInt(int Name,int32 Value)
{
	SESSION_VALUE value;
	value.type = SESSION_VALUE::VALUE_INT32;
	value.value.nNum = Value;
	m_Values[Name] = value;
}

void CSession::setInt64(int Name,int64 Value)
{
	SESSION_VALUE value;
	value.type = SESSION_VALUE::VALUE_INT64;
	value.value.nBigNum = Value;
	m_Values[Name] = value;
}

void CSession::setString(int Name,const char* Value)
{
	SESSION_VALUE value;
	value.type = SESSION_VALUE::VALUE_STRING;
	int strlen = length(Value)+1;
	value.value.pValue = (char*)malloc(strlen);
	memcpy(value.value.pValue,Value,strlen);
	m_Values[Name] = value;
}

void CSession::setPtr(int Name,IRefShare *Value)
{
	SESSION_VALUE value;
	value.type = SESSION_VALUE::VALUE_PTR;
	value.value.pPtr = Value;
	Value->incRef();
	m_Values[Name] = value;
}

bool CSession::isExist(int Name)
{
	map<int,SESSION_VALUE>::iterator i = m_Values.find(Name);
	if ( i != m_Values.end() )
		return true;
	return false;
}

int32 CSession::getInt(int Name, bool bThrowable, int32 nDefault)
{
	map<int,SESSION_VALUE>::iterator i = m_Values.find(Name);
	if ( i == m_Values.end() )
	{
		if ( bThrowable )
			ThrowException<CCException>("session get error -- key[%d] not exists",Name);
		return nDefault;
	}
	return (*i).second.value.nNum;
}

int64 CSession::getInt64(int Name, bool bThrowable, int64 nDefault)
{
	map<int,SESSION_VALUE>::iterator i = m_Values.find(Name);
	if ( i == m_Values.end() )
	{
		if ( bThrowable )
			ThrowException<CCException>("session get error -- key[%d] not exists",Name);
		return nDefault;
	}
	return (*i).second.value.nBigNum;
}

const char* CSession::getString(int Name, bool bThrowable, const char *pDefault)
{
	map<int,SESSION_VALUE>::iterator i = m_Values.find(Name);
	if ( i == m_Values.end() )
	{
		if ( bThrowable )
			ThrowException<CCException>("session get error -- key[%d] not exists",Name);
		return pDefault;
	}
	return (*i).second.value.pValue;
}

IRefShare* CSession::getPtr(int Name,bool bThrowable,IRefShare* pDefault)
{
	map<int,SESSION_VALUE>::iterator i = m_Values.find(Name);
	if ( i == m_Values.end() )
	{
		if ( bThrowable )
			ThrowException<CCException>("session get error -- key[%d] not exists",Name);
		return pDefault;
	}
	return (*i).second.value.pPtr;
}

bool CSession::remove(int Name)
{
	map<int,SESSION_VALUE>::iterator i = m_Values.find(Name);
	if ( i == m_Values.end() )
		return false;
	m_Values.erase(Name);
	return true;
}

void CSession::setSessionID(uint32 nID)
{
    m_nSessionID = nID;
}

uint32 CSession::getSessionID()
{
    return m_nSessionID;
}

