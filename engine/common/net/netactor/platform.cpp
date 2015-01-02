#include "../../public.h"
#include "platform.h"
#include "netactor.h"

#ifdef WINDOWS
#include "winreactorimpl.h"
#else
#include "linuxnetactordriver.h"
#include "linuxreactorimpl.h"
#endif

INetActorDriver* CNetActorDriverFactory::createNetActorDriver(CNetActorDriverFactory::NETACTOR_TYPE Type)
{
	switch ( Type )
	{
	case NETACTOR_REACTOR :
		{
#ifdef WINDOWS
			return new CWinReactorDriver();
#else
			return new CLinuxNetActorDriver();
#endif
		}
		break;
	}
	return NULL;
}
