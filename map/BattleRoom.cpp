#include "BattleRoom.h"
#include "JyTime_Value.h"
#include "PlayerManager.h"
#include "protocol_client_game.h"

CBattleRoom::CBattleRoom():m_uiBattleRoomKey(0)
{
    //m_allyAccountIDVec.clear();
    m_bCanDestroy = false;
    m_battleControl = NULL;
    m_isComplete = false;
    m_playerAccountID = 0;
}

CBattleRoom::~CBattleRoom()
{
    //m_allyAccountIDVec.clear();
    if(m_battleControl != NULL)
    {
        m_battleControl->UnInit();
        delete m_battleControl;
    }
    m_battleControl = NULL;
    m_isComplete = false;
}

void CBattleRoom::Update()
{
    //do
    if (m_battleControl!=NULL)
    {
        if (this->CheckIsTimeOut())
        {
            m_battleControl->OnTimeOut();
            m_bCanDestroy = true;
        }
        else
        {
            if (m_isComplete == false)
            {
                if (m_battleControl->IsCompleted())
                {
                    m_battleControl->OnComplete();
                    m_isComplete = true;
                }
                else
                {
                    m_battleControl->Update();
                }
            }
            else
            {
                //   
            }
        }
    }
}

uint32  CBattleRoom::Init(uint32 nBattleMapID,uint32 nBattleType,AccountID playerID,uint32 nBattleTime/*,CBattleInfo battleInfo*/)
{
    m_battleMapID = nBattleMapID;
    m_nBattleType = nBattleType;
    //m_BattleInfo = battleInfo;
    m_bCanDestroy = false;
    m_isComplete = false;
    m_uiBattleTime = nBattleTime;
    m_uiCreateTime = JyTime_Value::gettimeofday().msec();
    m_battleControl = IBattleRoomControl::createBattleControl(nBattleMapID); 
    if (m_battleControl == NULL)
    {
        LOG_ERROR("CBattleRoom::Init: IBattleRoomControl::createBattleControl %d error m_battleControl=NULL ",nBattleMapID);
        return EnumBattleResult_ServerInnerError;
    }
    m_battleControl->Init(nBattleMapID,nBattleType,playerID);
    m_playerAccountID = playerID;//创建者iD
    return 0;
}

uint32   CBattleRoom::UnInit()
{
    //m_allyAccountIDVec.clear();
    if (m_playerAccountID != 0)
    {
        CPlayerPtr player =  CPlayerManager::Instance()->GetPlayerByAccount(m_playerAccountID);
        if (player != NULL)
        {
            //发送消息通知客户端弹出玩家
            G2CNotifyBattleTimeOut  msg;
            msg.uiBattleID = m_battleMapID;
            msg.uiBattleInstanceID = m_uiBattleRoomKey;
            msg.uiMapID = m_playerOldMapID;
            player->SendToClient(GAME_NOTIFY_BATTLETIMEOUT,msg);


            SetPlayerOldStatus();
        }
    }
    if(m_battleControl != NULL)
    {
        m_battleControl->UnInit();
        delete m_battleControl;
    }
    m_battleControl = NULL;
    return 0;
}

uint32    CBattleRoom::Operate(uint32 unOperType,AccountID playerID,void* nData)
{
    uint32  nRst = 0;
   
    if (m_battleControl!=NULL)
    {
         nRst = m_battleControl->Operate(unOperType,playerID,nData);
    }
    return nRst;
}

bool     CBattleRoom::CheckIsTimeOut()//判断是否超时
{
    if ( m_uiCreateTime + m_uiBattleTime * 1000 <=  JyTime_Value::gettimeofday().msec() )
    {
        return true;
    }
    return false;
}

 bool   CBattleRoom::CanDestroy()
 {
    if (m_battleControl!=NULL)
    {
        return m_bCanDestroy || m_battleControl->CanDestroy();
    }
    return true;
 }

uint32     CBattleRoom::EnterBattleRoom(AccountID playerID)//allyID 盟友的ID
{
    if (m_battleControl != NULL)
    {
        //m_allyAccountIDVec.push_back(playerID);
        if (playerID != m_playerAccountID)
        {
             LOG_ERROR("CBattleRoom::EnterBattleRoom : 进入不是自己的房间");
        }
        CPlayerPtr player = CPlayerManager::Instance()->GetPlayerByAccount(playerID);
        m_iStatus = player->GetPlayerStatus();
        m_battleControl->EnterBattleRoom(playerID);
    }
    else
    {
        LOG_ERROR("CBattleRoom::EnterBattleRoom : m_battleControl = NULL");
    }
    return 0;
}

uint32     CBattleRoom::LeaveBattleRoom(AccountID playerID)//离开战场
{
    if (m_battleControl != NULL)
    {
        m_battleControl->LeaveBattleRoom(playerID);
    }
    else
    {
        LOG_ERROR("CBattleRoom::LeaveBattleRoom : unknow error");
    } 
    
    SetPlayerOldMap();
    return 0;
}

void   CBattleRoom::SetPlayerOldStatus()
{
    CPlayerPtr player = CPlayerManager::Instance()->GetPlayerByAccount(m_playerAccountID);
    if (player != NULL)
    {
        player->SetPlayerMapID(m_playerOldMapID);
        player->SetPlayerStatus(m_iStatus);
    }
    m_playerAccountID = 0;
}