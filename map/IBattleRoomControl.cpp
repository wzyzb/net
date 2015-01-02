#include "IBattleRoomControl.h"
#include "NormalBattleControl.h"

IBattleRoomControl*   IBattleRoomControl::createBattleControl(uint32 uiBattleID)
{
    switch(uiBattleID)
    {
    case 0:
    default:
        return new CNormalBattleControl();
    }
}


