#include <public.h>
#include <framework/txlogprotocol.h>
#include <framework/debuginfo.h>

/*

		Class	CTXLogProtocol

*/
CTXLogProtocol::CTXLogProtocol()
{
}

CTXLogProtocol::~CTXLogProtocol()
{
}


PROTOCOL_RETCODE CTXLogProtocol::next(IInputStream& Input,uint32& nSerialNo,uint32& nServantName,IOutputStream& Body)
{
	return PRC_NOMOREPACK;
}

PROTOCOL_RETCODE CTXLogProtocol::pack(uint32 nSerialNo, uint32 nServantName, const CBytesBuffer &Body, IOutputStream& Output)
{
	const char* pbegin = ((CBytesBuffer&)Body).c_str();
	uint32 totalsize = Body.getDataSize();
	Output.writeBytes(pbegin,totalsize);

	return PRC_SUCCESS;
}

void CTXLogProtocol::incRef()
{
	CRefShare::incRef();
}

bool CTXLogProtocol::decRef()
{
	return CRefShare::decRef();
}

uint32 CTXLogProtocol::getResponseName(uint32 nServantName)
{
	return nServantName;
}

bool CTXLogProtocol::isResponse(uint32 nServantName)
{
	return false;
}

bool CTXLogProtocol::isHeartBeat(uint32 nServantName)
{
	return false;
}

uint32 CTXLogProtocol::getServantName(uint32 nServantName)
{
	return nServantName;
}

PROTOCOL_RETCODE CTXLogProtocol::packHeartBeat(IOutputStream &Output)
{
	//不用发送心跳包
	return PRC_ERROR;
}

IProtocol* CTXLogProtocol::newInstance()
{
	return new CTXLogProtocol();
}
