#ifndef _DBLOADDBPLAYERDATA_H_
#define _DBLOADDBPLAYERDATA_H_

#include <public.h>
#include <framework/serial.h>
#include <framework/request.h>
#include <framework/response.h>
#include <framework/commonptl.h>


#include <./dbtable.h>

class CDBLoadDBPlayerDataRequest : public IRequest
{
public:
	uint32 m_nPlayerID;

	virtual bool serial(char *pOut, int &Length){return true;}
	virtual bool serial(IOutputStream& Output)
	{
		writeuint32(Output, m_nPlayerID);
		return true;
	}
	virtual bool unserial(const char *pIn, int Length){return true;}
	virtual bool unserial(IInputStream& Input)
	{
		readuint32(Input, m_nPlayerID);
		return true;
	}
	CDBLoadDBPlayerDataRequest()
	{
		m_nPlayerID = 0;
	}
	CDBLoadDBPlayerDataRequest(const CDBLoadDBPlayerDataRequest &t)
	{
		m_nPlayerID = t.m_nPlayerID;
	}
	CDBLoadDBPlayerDataRequest& operator = (const CDBLoadDBPlayerDataRequest& t)
	{
		m_nPlayerID = t.m_nPlayerID;
		return *this;
	}
	virtual ~CDBLoadDBPlayerDataRequest()
	{
	}
};

typedef CRefObject<CDBLoadDBPlayerDataRequest> CDBLoadDBPlayerDataRequestPtr;


class CDBLoadDBPlayerDataResponse : public IResponse
{
public:
	uint32 m_nRetCode;

	virtual bool serial(char *pOut, int &Length){return true;}
	virtual bool serial(IOutputStream& Output)
	{
		writeuint32(Output, m_nRetCode);
		return true;
	}
	virtual bool unserial(const char *pIn, int Length){return true;}
	virtual bool unserial(IInputStream& Input)
	{
		readuint32(Input, m_nRetCode);
		return true;
	}
	CDBLoadDBPlayerDataResponse()
	{
		m_nRetCode = 0;
	}
	CDBLoadDBPlayerDataResponse(const CDBLoadDBPlayerDataResponse &t)
	{
		m_nRetCode = t.m_nRetCode;
	}
	CDBLoadDBPlayerDataResponse& operator = (const CDBLoadDBPlayerDataResponse& t)
	{
		m_nRetCode = t.m_nRetCode;
		return *this;
	}
	virtual ~CDBLoadDBPlayerDataResponse()
	{
	}
};

typedef CRefObject<CDBLoadDBPlayerDataResponse> CDBLoadDBPlayerDataResponsePtr;

#endif

