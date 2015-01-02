#include <public.h>
#include <framework/idipprotocol.h>
#include <framework/debuginfo.h>

#define PREFIX_RESPONSE 0X00000001

#define IS_RESPONSE(ServantName)      ( 0 == ( ServantName & PREFIX_RESPONSE ) )
#define GET_SERVANT_NAME(ServantName) ( IS_RESPONSE(ServantName) ? ServantName -1  : ServantName )

/*

		Class	CIDIPProtocol

*/
CIDIPProtocol::CIDIPProtocol()
{
}

CIDIPProtocol::~CIDIPProtocol()
{
}


PROTOCOL_RETCODE CIDIPProtocol::next(IInputStream& Input,uint32& nSerialNo,uint32& nServantName,IOutputStream& Body)
{
	int32 datalen = Input.getDataSize();
	const char* pbegin = Input.c_str();
	if ( datalen < sizeof(IDIP_HEADER) )
		return PRC_NOMOREPACK;
	IDIP_HEADER header;
	memcpy(&header,pbegin,sizeof(IDIP_HEADER));

	header.uiPacketLen = ntohl(header.uiPacketLen);
	header.uiCmdid = ntohl(header.uiCmdid);
	header.uiSeqid = ntohl(header.uiSeqid);
	header.uiSendTime = ntohl(header.uiSendTime);
	header.uiVersion = ntohl(header.uiVersion);
	header.iResult = ntohl(header.iResult);

	//判断剩余的数据是否足够
	//packetlen包含包头部分
	if ( datalen < header.uiPacketLen )
	{
		return PRC_NOMOREPACK;
	}
	nSerialNo = header.uiSeqid;
	nServantName = header.uiCmdid;

	//把包头也写进去，方便pack的时候，获取一些必要信息，如version，鉴权信息等
	uint32 writesize = header.uiPacketLen;
	Body.writeBytes(pbegin,writesize);

	Input.popBytes(header.uiPacketLen);
	return PRC_SUCCESS;
}

PROTOCOL_RETCODE CIDIPProtocol::pack(uint32 nSerialNo, uint32 nServantName, const CBytesBuffer &Body, IOutputStream& Output)
{
	int retcode = 0;
	CBytesBufferPtr pbuff = CBytesBufferPtr::createInstance();
	const char* pbegin = ((CBytesBuffer&)Body).c_str();
	uint32 totalsize = Body.getDataSize();
	IDIP_HEADER header;
	memcpy(&header,pbegin,sizeof(IDIP_HEADER));

	header.uiPacketLen = htonl(totalsize);
	header.uiCmdid = htonl(nServantName);
	header.uiSeqid = htonl(nSerialNo);
	CDateTime current;
	header.uiSendTime = htonl(current.getYear()*10000+current.getMonth()*100+current.getDay());
	header.uiVersion = htonl(header.uiVersion);
	header.iResult = htonl(header.iResult);
	uint32 sendsize = sizeof(IDIP_HEADER);
	Output.writeBytes((const char*)&header,sendsize);

	sendsize = totalsize - sendsize;
	Output.writeBytes(pbegin+sizeof(IDIP_HEADER),sendsize);

	return PRC_SUCCESS;
}

void CIDIPProtocol::incRef()
{
	CRefShare::incRef();
}

bool CIDIPProtocol::decRef()
{
	return CRefShare::decRef();
}

uint32 CIDIPProtocol::getResponseName(uint32 nServantName)
{
	return ( nServantName + PREFIX_RESPONSE ) ;
}

bool CIDIPProtocol::isResponse(uint32 nServantName)
{
	return ( 0 != IS_RESPONSE(nServantName) );
}

bool CIDIPProtocol::isHeartBeat(uint32 nServantName)
{
	return false;
}

uint32 CIDIPProtocol::getServantName(uint32 nServantName)
{
	return GET_SERVANT_NAME(nServantName);
}

PROTOCOL_RETCODE CIDIPProtocol::packHeartBeat(IOutputStream &Output)
{
	//不用发送心跳包
	return PRC_SUCCESS;
}

IProtocol* CIDIPProtocol::newInstance()
{
	return new CIDIPProtocol();
}
