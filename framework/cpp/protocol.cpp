#include <framework/protocol.h>

/*

	Class	CProtocolFactory

*/
map<string,IProtocolPtr> CProtocolFactory::g_Protocols;

void CProtocolFactory::regProtocol(const char *pProtocolName, IProtocolPtr pProtocol)
{
	if ( pProtocolName == NULL )
		ThrowException<CCException>("CProtocolFactory::regProtocol|ProtocolName is NULL");
	map<string,IProtocolPtr>::iterator i = g_Protocols.find(pProtocolName);
	if ( i != g_Protocols.end() )
		ThrowException<CCException>("Protocol[%s] already exists",pProtocolName);
	g_Protocols[pProtocolName] = pProtocol;
}

IProtocolPtr CProtocolFactory::createProtocol(const char* pProtocolName)
{
	if ( pProtocolName == NULL )
		ThrowException<CCException>("CProtocolFactory::createProtocol|ProtocolName is NULL");
	map<string,IProtocolPtr>::iterator i = g_Protocols.find(pProtocolName);
	if ( i == g_Protocols.end() )
		ThrowException<CCException>("Protocol[%s] not exists",pProtocolName);
	return i->second->newInstance();
}

