#ifndef _BATTLEMANAGER_H_
#define _BATTLEMANAGER_H_
#include "NetEngine.h"
#include "Singleton.h"
#include "ObjKeyGenerator.h"
#include <map>

#include "BattleInfo.h"
#include "BattleRoom.h"
#include "Player.h"

using namespace JyCommon;
using namespace NetWork;

typedef map< uint32, CBattleRoomPtr> BATTLEMAP;


class CBattleInstanceManager:public Ref_Object
{
private:
    map<uint32,CBattleRoomPtr>           m_BattleMap;//key-battle map
    map<AccountID,uint32>                m_AccountKeyMap;//玩家id 副本实例key 的map表

    uint32                               m_BattleMapID;//战场id
    uint32                               m_BattleLastTime;//战场最大时间 单位秒
    
    //CBattleInfo                          m_BattleInfo;
    CObjKeyGenerator    m_BattleKeyGenerator;       //room分配器
public:
    CBattleInstanceManager();
    virtual ~CBattleInstanceManager();

    inline uint32       GetNewKey(){ m_BattleKeyGenerator.GetKey();}//获得一个新的战场房间Key
    inline void         RestoreKey( uint32 uiKey ){ m_BattleKeyGenerator.RestoreKey(uiKey);}//归还一个战场房间Key
public:
    uint32              Init(uint32 battleMapID,uint32 uLastTime/*,CBattleInfo battleInfo*/);
    uint32              UnInit();

    void                Update();
public:
    uint32              ProcessBattleReq(AccountID playerID,uint32  nReqType,uint32 nBattleID,uint32 nBattleType,uint32 nInstanceID,void* pData,uint32& uKey );
    CBattleRoomPtr      FindPlayerBattleRoom(AccountID playerID);
    uint32              FindPlayerBattleUkey(AccountID playerID);

    uint32              LeaveBattleRoom(uint32 battleMapID,AccountID playerID,uint32 nKey);//离开战场

    uint32              GetBattleInstanceCount(){return m_BattleMap.size();}
private:
    bool                IsKeyValid(uint32 uKey){ return uKey >=1 && uKey <= 100000;}
    CBattleRoomPtr      CreateBattleInstance(uint32 nBattleID);

    uint32              Open(uint32 battleMapID,uint32 nBattleType,AccountID playerID,uint32& uKey); //开启

    uint32              Operate(uint32 unOperType,AccountID playerID,void* nData,uint32 uKey);//uKey 房间号

    uint32              EnterBattleRoom(uint32 battleMapID,AccountID playerID,uint32 nKey);//
};

typedef     Smart_Ptr<CBattleInstanceManager>  CBattleInstanceManagerPtr;




class CBattleManager:public Singleton<CBattleManager>
{
private:
    std::map<uint32,CBattleInstanceManagerPtr>          m_BattleInstanceMgrMap;//
    //CSVLoader                                           m_Loader;
    //std::map<uint32, CBattleInfo>                       m_BattleInfoMap;//副本ID数组
public:
    CBattleManager();
    ~CBattleManager();

public:
    uint32              Init();
    uint32              UnInit();

    CBattleInstanceManagerPtr           getBattleInstanceManager(uint32 unBattleMapID);
public:
    void                Update();
private:                                       
    //uint32  LoadFile(const char* szPath);
};


#endif