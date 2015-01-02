#include "../public.h"
#include "refShare.h"
#include "automicOper.h"


/*

		Class	CRefShare

*/
bool CRefShare::decRef()
{
	return decRefEx(NULL); 
}

CRefShare& CRefShare::operator=(const CRefShare& Right)
{
	return *this;
}


#if defined(HAS_MULTITHREAD)
CRefShare::CRefShare()
{
	atomic_set(&m_nRefCount,0);
}

CRefShare::CRefShare(const CRefShare& Right)
{
	atomic_set(&m_nRefCount,0);
}

CRefShare::~CRefShare()
{
}

void CRefShare::incRef()
{
	atomic_inc(&m_nRefCount);
}

bool CRefShare::decRefEx(FreeRefFunc pFreeFunc)
{
	if ( atomic_dec_and_test(&m_nRefCount) )
	{
		if ( pFreeFunc )
			(*pFreeFunc)(this);
		else
			delete this;
		return true;
	}
	return false;
}
#else

CRefShare::CRefShare()
{
	m_nRefCount = 0;
}

CRefShare::~CRefShare()
{
}

CRefShare::CRefShare(const CRefShare& Right)
{
	m_nRefCount = 0;
}

void CRefShare::incRef()
{
	m_nRefCount ++;
}

bool CRefShare::decRefEx(FreeRefFunc pFreeFunc)
{
	if ( --m_nRefCount ==  0 )
	{
		if ( pFreeFunc )
			(*pFreeFunc)(this);
		else
			delete this;
		return true;
	}
	return false;
}
#endif