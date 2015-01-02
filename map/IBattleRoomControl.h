#ifndef _IBATTLEROOMCONTROL_H_
#define _IBATTLEROOMCONTROL_H_

#include "NetEngine.h"
#include "Singleton.h"
#include "ObjKeyGenerator.h"
#include <map>

#include "Player.h"
#include "BattleInfo.h"


using namespace JyCommon;
using namespace NetWork;



class IBattleRoomControl:public Ref_Object
{
public:
    IBattleRoomControl(){};
    virtual ~IBattleRoomControl(){};

    virtual uint32          Init(uint32 battleMapID,uint32 nBattleType,AccountID playerID)=0;//, CBattleInfo battleInfo
    virtual uint32          UnInit()=0;

    virtual void            Update()=0;
    virtual uint32          Operate(uint32 unOperType,AccountID playerID,void* nData)=0;

    virtual uint32          EnterBattleRoom(AccountID playerID)=0;        //
    virtual uint32          LeaveBattleRoom(AccountID playerID)=0;        //离开战场

public:
    virtual bool            IsCompleted() = 0;                         //判断是否完成,
    virtual bool            CanDestroy() = 0;                          //是否可以销毁
public:
    virtual uint32          OnTimeOut()=0;
    virtual uint32          OnComplete()=0;
public:
    static IBattleRoomControl*   createBattleControl(uint32 uiBattleID);
};

typedef     Smart_Ptr<IBattleRoomControl>  IBattleRoomControlPtr;


#endif