#include <public.h>
#include <framework/zwprotocol.h>
#include <framework/debuginfo.h>

#define MAGIC_NUM 0xC0074346    // Magic Number, 'F' 'C' 0x07 0xC0
#define DAS_PROTOCOL_VERSION 0x00   // 协议的版本号

#ifdef WINDOWS
#pragma pack(push,1)
#else
#pragma pack(push,1)
#endif
typedef struct _t_header
{
	uint32 magic;
	uint8  version;
	uint32 serialno;
	uint32 servantname;
	uint32 checksum;
	uint16 flag;
	uint8  sizeflag;
	_t_header()
	{
		memset(this,0,sizeof(struct _t_header));
		magic = MAGIC_NUM;
		version = DAS_PROTOCOL_VERSION;
	}
}HEADER,*LPHEADER;
typedef struct _t_longheader
{
	uint32 magic;
	uint8  version;
	uint32 serialno;
	uint32 servantname;
	uint32 checksum;
	uint16 flag;
	uint8  sizeflag;
	uint16 datasize;
	_t_longheader()
	{
		memset(this,0,sizeof(struct _t_longheader));
		magic = MAGIC_NUM;
		version = DAS_PROTOCOL_VERSION;
	}
}LONGHEADER,*PLONGHEADER;

#ifdef WINDOWS
#pragma pack(pop)
#else
#pragma pack(pop)
#endif

#define PREFIX_REQUEST  0x00000000
#define PREFIX_RESPONSE 0X80000000

#define IS_RESPONSE(ServantName)      ( ServantName & PREFIX_RESPONSE ) 
#define GET_SERVANT_NAME(ServantName) ( ServantName & 0x7FFFFFFF )

// 获取 Servant Name 的前缀
#define GET_SERVANT_PREFIX(ServantName) ( ( ( ServantName ) >> 16 ) & 0xFFFF )

#define SYSTEM_PREFIX 0x70000000
#define IS_SYSTEM_PROTO(type) ( type & SYSTEM_PREFIX ) == SYSTEM_PREFIX ? true : false
#define GET_SERVANT_NAME(ServantName) ( ServantName & 0x7FFFFFFF )



enum SYSTEM_PROTO
{
    SYSTEM_HEARTBEAT = 0x70000001,
    SYSTEM_END = 0x7FFFFFFF
};

/*

		Class	CZWProtocol

*/
CZWProtocol::CZWProtocol()
{
}

CZWProtocol::~CZWProtocol()
{
}

int CZWProtocol::readInt(const char* pBuffer,int nReadSize)
{
	switch ( nReadSize )
	{
		case 1 :
			return *(uint8*)pBuffer;
		case 2 :
			return *(uint16*)pBuffer;
		case 4 :
			return *(uint32*)pBuffer;
	}
	return -1;
}

PROTOCOL_RETCODE CZWProtocol::next(IInputStream& Input,uint32& nSerialNo,uint32& nServantName,IOutputStream& Body)
{
	int32 datalen = Input.getDataSize();
	uint32 totalsize = 0;
	const char* pbegin = Input.c_str();
	//判断是否所有包到达
	bool finished = false;
	//多包判断
	while ( !finished )
	{
		if ( datalen < sizeof(HEADER) )
			return PRC_NOMOREPACK;
		int index = 0;
		HEADER head;
		memcpy(&head,pbegin+index,sizeof(HEADER));
		index += sizeof(HEADER);
		//判断MAGIC_NUM是否正确
		if ( head.magic != MAGIC_NUM )
		{
			log_error("CZWProtocol|next|MAGICNUM ERROR -- receive [%08X] but the correct is [%08X]",head.magic,MAGIC_NUM);
			return PRC_ERROR;
		}

		//必须两个字节
		uint16 datasize = 0;

		//需要多读取两个字节
		if( head.sizeflag == 0xFF )
		{
			//判断长度是否足够
			if ( datalen < sizeof(HEADER) + sizeof(datasize) )
				return PRC_NOMOREPACK;
			else
			{
				datasize = readInt(pbegin+index, sizeof(datasize));
				index += sizeof(datasize);
			}
		}
		else
		{
			datasize = head.sizeflag;
		}

		//判断剩余的数据是否足够
		if ( datalen < index + datasize )
		{
			return PRC_NOMOREPACK;
		}
		nSerialNo = head.serialno;
		nServantName = head.servantname;
		//
		uint32 writesize = datasize;
		Body.writeBytes(pbegin+index,writesize);

		totalsize += index+datasize;
		pbegin += index + datasize;
		datalen -= (index+datasize);

		//没有后续包
		if ( head.flag == 0 || datasize < 0xFFFE )
			finished = true;
	}
	Input.popBytes(totalsize);
	return PRC_SUCCESS;
}

PROTOCOL_RETCODE CZWProtocol::pack(uint32 nSerialNo, uint32 nServantName, const char* pBody,uint32 nBodyLen, IOutputStream& Output)
{
	int retcode = 0;
	CBytesBufferPtr pbuff = CBytesBufferPtr::createInstance();
	const char* pbegin = pBody;
	uint32 totalsize = nBodyLen;
	if ( pbegin == NULL )
		totalsize = 0;
	bool finished = false;
	while ( !finished )
	{
		pbuff->clear();
		//判断是否需要分包
		uint32 bodysize = totalsize;
		if ( bodysize <= 0xFFFE )
			finished = true;
		else
			bodysize = 0xFFFE;

		//写入head
		uint32 writelen = 0;
		if ( bodysize >= 0xFF )
		{
			LONGHEADER head;
			head.serialno = nSerialNo;
			head.servantname = nServantName;
			head.sizeflag = 0xFF;
			head.datasize = bodysize;
			if ( finished )
				head.flag = 0;
			else
				head.flag = 1;
			writelen = sizeof(LONGHEADER);
			pbuff->writeBytes((const char*)&head,writelen);
		}
		else
		{
			HEADER head;
			head.serialno = nSerialNo;
			head.servantname = nServantName;
			head.sizeflag = bodysize;
			if ( finished )
				head.flag = 0;
			else
				head.flag = 1;
			writelen = sizeof(HEADER);
			pbuff->writeBytes((const char*)&head,writelen);
		}
		//写入body
		writelen += bodysize;
		if ( pbegin != NULL )
			pbuff->writeBytes(pbegin,bodysize);
		if ( 0 != ( retcode = Output.writeBytes(pbuff->c_str(),writelen) ) )
		{
			//发送信息错误
			log_warning("CZWProtocol|pack|outputstream writebytes error[%d]",retcode);
			return PRC_ERROR;
		}
		DEBUG_BLOG(pbuff->c_str(),writelen,"pack");
		if ( pbegin != NULL )
			pbegin += bodysize;
		totalsize -= bodysize;
	}
	return PRC_SUCCESS;
}

PROTOCOL_RETCODE CZWProtocol::pack(uint32 nSerialNo, uint32 nServantName, const CBytesBuffer &Body, IOutputStream& Output)
{
	const char* pbegin = ((CBytesBuffer&)Body).c_str();
	uint32 totalsize = Body.getDataSize();
	return pack(nSerialNo,nServantName,pbegin,totalsize,Output);
}

void CZWProtocol::incRef()
{
	CRefShare::incRef();
}

bool CZWProtocol::decRef()
{
	return CRefShare::decRef();
}

uint32 CZWProtocol::getResponseName(uint32 nServantName)
{
	return ( nServantName | PREFIX_RESPONSE ) ;
}

bool CZWProtocol::isResponse(uint32 nServantName)
{
	return ( 0 != IS_RESPONSE(nServantName) );
}

bool CZWProtocol::isHeartBeat(uint32 nServantName)
{
	return SYSTEM_HEARTBEAT == nServantName;
}

uint32 CZWProtocol::getServantName(uint32 nServantName)
{
	return GET_SERVANT_NAME(nServantName);
}

IProtocol* CZWProtocol::newInstance()
{
	return new CZWProtocol();
}

PROTOCOL_RETCODE CZWProtocol::packHeartBeat(IOutputStream& Output)
{
	return this->pack(0,SYSTEM_HEARTBEAT,NULL,0,Output);
}
