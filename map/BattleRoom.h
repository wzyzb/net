#ifndef _BATTLEROM_H_
#define _BATTLEROM_H_

#include "NetEngine.h"
#include "MapData.h"
#include "Player.h"
#include "BattleInfo.h"
#include "IBattleRoomControl.h"

using namespace JyCommon;


class CBattleRoom:public Ref_Object
{
public:
    CBattleRoom();
    ~CBattleRoom();

    void            SetRoomKey( uint32 uiKey ){ m_uiBattleRoomKey = uiKey;}
    uint32          GetRoomKey(){ return m_uiBattleRoomKey;}
    
    void            SetPlayerID(AccountID playerID){m_playerAccountID=playerID;}
    AccountID       GetPlayerID(){return m_playerAccountID;}
public:
    uint32          Init(uint32 battleMapID,uint32 nBattleType,AccountID playerID,uint32 nBattleTime/*,CBattleInfo battleInfo*/);
    uint32          UnInit();

    void            Update();
    uint32          Operate(uint32 unOperType,AccountID playerID,void* nData);

    uint32          EnterBattleRoom(AccountID playerID);        //
    uint32          LeaveBattleRoom(AccountID playerID);        //离开战场

    bool            CheckIsTimeOut();                           //判断是否超时
    bool            CanDestroy();                               //是否可以销毁
public:
    void            SetCanDestroy(bool flag){ m_bCanDestroy = flag;}
private:
    uint32          m_uiBattleRoomKey;                          //战场房间KEY
    uint32          m_uiCreateTime;                             //创建房间的时间
    uint32          m_uiBattleTime;                             //战场持续时间
    uint32          m_battleMapID;                              //当前战场的ID
    uint32          m_nBattleType;                              //类型

    uint16          m_iStatus;//玩家进入战场前的状态
    //CBattleInfo     m_BattleInfo;                               //战场信息

    AccountID       m_playerAccountID;                          //玩家id
    uint32          m_playerOldMapID;                           //玩家之前的mapID
    //std::vector<CPlayerPtr>       m_allyAccountIDVec;           //盟友
    
    IBattleRoomControl*         m_battleControl;              //
    //==================
    bool            m_bCanDestroy;                              //是否可以销毁
    bool            m_isComplete;                               //是否已经完成
private:
    void            SetPlayerOldStatus();                          //设置为玩家进入战场前的状态 和 场景
};

typedef Smart_Ptr<CBattleRoom> CBattleRoomPtr;

#endif