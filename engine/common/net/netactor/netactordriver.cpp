
#include "netactordriver.h"
#include "../../util/drivermanager.h"


INetActorPtr INetActorDriver::getNetActor()
{
	return static_cast<INetActorDriver*>( CDriverManager::getDriver("NetActorDriver") )->getNetActorInstance();
}

