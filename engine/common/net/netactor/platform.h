#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "netactordriver.h"
/*
  使用工厂模式屏蔽平台的差异性和各种netactor
*/
class CNetActorDriverFactory
{
public :
	enum 	NETACTOR_TYPE
	{
		NETACTOR_REACTOR,
		NETACTOR_PROACTOR
	};
	static INetActorDriver* createNetActorDriver(NETACTOR_TYPE Type);
};

#endif
