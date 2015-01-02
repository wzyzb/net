#ifndef _NORMALBATTLECONTROL_H_
#define _NORMALBATTLECONTROL_H_

#include "NetEngine.h"
#include "Singleton.h"
#include "ObjKeyGenerator.h"
#include <map>

#include "Player.h"
#include "BattleInfo.h"
#include "IBattleRoomControl.h"

using namespace JyCommon;
using namespace NetWork;

class CNormalBattleControl:public IBattleRoomControl
{
private:
    AccountID       m_playerID;
    uint32          m_battleMapID;                              //当前战场的ID
    uint32          m_nBattleType;                              //类型
    //CBattleInfo     m_BattleInfo;

    bool            m_isCompleted;

    //std::vector<CPlayerPtr>       m_allyAccountIDVec;           //盟友

    typedef std::pair<uint32,uint32>   MONSTERTYPEPAIR;
    std::vector<MONSTERTYPEPAIR>  m_monsterTypeVec;                //记录副本怪物生死

public:
    CNormalBattleControl(){}
    virtual ~CNormalBattleControl(){}
public:
    uint32          Init(uint32 battleMapID,uint32 nBattleType,AccountID playerID);//,CBattleInfo battleInfo
    uint32          UnInit();

    void            Update();
    uint32          Operate(uint32 unOperType,AccountID playerID,void* nData);

    uint32          EnterBattleRoom(AccountID playerID);        //
    uint32          LeaveBattleRoom(AccountID playerID);

    bool            IsCompleted() ;                         //判断是否完成,
    bool            CanDestroy() ;                          //是否可以销毁
public:
    uint32          OnTimeOut();
    uint32          OnComplete();
};

typedef Smart_Ptr<CNormalBattleControl> CNormalBattleControlPtr;

#endif