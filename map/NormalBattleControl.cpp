#include "NormalBattleControl.h"



//=======

uint32     CNormalBattleControl::Init(uint32 battleMapID,uint32 nBattleType,AccountID playerID)//,CBattleInfo battleInfo
{
    m_playerID = playerID;
    // m_BattleInfo = battleInfo;
    m_battleMapID = battleMapID;
    m_monsterTypeVec.clear();
    m_nBattleType = nBattleType;
    m_isCompleted = false;
    return 0;
}

uint32     CNormalBattleControl::UnInit()
{
    //战场时间到了 如果玩家在战场 需要重新设置玩家的地图
    m_monsterTypeVec.clear();
    return 0;
}

void       CNormalBattleControl::Update()
{
    //do

}

uint32     CNormalBattleControl::Operate(uint32 unOperType,AccountID playerID,void* nData)
{
    switch(unOperType)
    {
    case EnumBattleOper_KillMonster:
        {
            //更新怪物状态
            uint32  nMonsterID = (uint32)nData;

        }
        break;
    case EnumBattleOper_CompleteReport:
        {
            //check 
            m_isCompleted = true;
        }
        break;
    default:
        break;
    }
    return 0;
}

uint32     CNormalBattleControl::EnterBattleRoom(AccountID playerID)        //
{
    //do
    return 0;
}

uint32     CNormalBattleControl::LeaveBattleRoom(AccountID playerID)
{
    //do
    return 0;
}

bool       CNormalBattleControl::IsCompleted()                         //判断是否完成
{
    return m_isCompleted;
}

bool       CNormalBattleControl::CanDestroy()                          //是否可以销毁
{
    return false;
}

uint32     CNormalBattleControl::OnTimeOut()
{
    //战场时间到了处理
    return 0;
}


uint32     CNormalBattleControl::OnComplete()
{
    //战场完成处理
    return 0;
}