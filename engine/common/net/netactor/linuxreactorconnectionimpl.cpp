#include <public.h>

#ifndef WINDOWS
#include <net/netactor/unix/linuxreactorconnectionimpl.h>

CLinuxReactorConnectionImpl::CLinuxReactorConnectionImpl(CLinuxReactorImpl* pReactor, CEpollBindData* pBindData, 
                                                         IConnectionPtr pConnection) : m_pReactor(pReactor),
                                                                                       m_pBindData(pBindData),
                                                                                       m_pConnection(pConnection)
{
	m_pBindData->incRef();
}

CLinuxReactorConnectionImpl::~CLinuxReactorConnectionImpl()
{
	m_pBindData->decRef();
}

IConnectionPtr CLinuxReactorConnectionImpl::getRealConnection()
{
    return m_pConnection;
}

int32 CLinuxReactorConnectionImpl::setMode(ECOMMMODE Mode)
{
    return m_pConnection->setMode(Mode);
}

IInputStreamPtr CLinuxReactorConnectionImpl::getInputStream()
{
    return m_pConnection->getInputStream();
}

IOutputStreamPtr CLinuxReactorConnectionImpl::getOutputStream()
{
    return this;
}

void CLinuxReactorConnectionImpl::close()
{
    m_pConnection->close();
}

int CLinuxReactorConnectionImpl::readBytes(char *pBuf, uint32& Buflen)
{
    return m_pConnection->getInputStream()->readBytes(pBuf, Buflen);
}

int CLinuxReactorConnectionImpl::popBytes(uint32 BytesToPop)
{
    return m_pConnection->getInputStream()->popBytes(BytesToPop);
}

uint32 CLinuxReactorConnectionImpl::getDataSize() const
{
	//return ((const IInputStreamPtr)(m_pConnection->getInputStream()))->getDataSize();
	return 0;
}

const char* CLinuxReactorConnectionImpl::c_str()
{
    return m_pConnection->getInputStream()->c_str();
}

int CLinuxReactorConnectionImpl::writeBytes( const char* pBuf, uint32& Buflen )
{
    return m_pReactor->asynWrite(m_pConnection, m_pBindData.get(), pBuf, Buflen);
}

void CLinuxReactorConnectionImpl::incRef()
{
     CRefShare::incRef();
}

bool CLinuxReactorConnectionImpl::decRef()
{
     return CRefShare::decRef();
}

SOCKET CLinuxReactorConnectionImpl::getHandle() const
{
    IConnectionPtr p = m_pConnection;
    return p->getHandle();
}


IAddressPtr CLinuxReactorConnectionImpl::getHostAddress()
{
    return m_pConnection->getHostAddress();
}
    
IAddressPtr CLinuxReactorConnectionImpl::getRemoteAddress()
{
    return m_pConnection->getRemoteAddress();
}

int32 CLinuxReactorConnectionImpl::setBufSize(int32 nBufSize)
{
    m_pConnection->setBufSize(nBufSize);
}

CEpollBindData* CLinuxReactorConnectionImpl::getKey()
{
	return m_pBindData.get();
}


#endif

