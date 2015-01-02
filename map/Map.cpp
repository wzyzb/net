#include "Map.h"
#include "PlayerManager.h"
#include <math.h>
CMap::CMap()
{
    m_isInit = false;
}

CMap::~CMap()
{

}

bool CMap::Init(const CMapDataPtr& mapData)
{
    m_mapData = mapData;
    m_mapID = mapData->m_mapID;
    m_isInit = true;
    //m_playersMap.clear();
    m_playerAccountIDsVec.clear();
    return 0;
}

bool CMap::UnInit()
{
    //m_playersMap.clear();
    m_playerAccountIDsVec.clear();
    return 0;
}

uint32  CMap::AddPlayer(AccountID id)
{
    CPlayerPtr player = CPlayerManager::Instance()->GetPlayerByAccount(id);;
    if (player != NULL)
    {
        if (m_playerAccountIDsVec.find(id) != m_playerAccountIDsVec.end())
        {
            LOG_INFO("player AccountID=%d already in map",id);
            return 1;
        }
        m_playerAccountIDsVec.insert(id);


        POSITION  pos = player->getPlayerPos();
        list<CPlayerPtr> aroundPlayers;
        aroundPlayers.clear();
        GetCirclePlayer(pos, aroundPlayers);


        AOIBasic  aoiBasic;
        AOIBasicList aoiBasicList_other;
        AOIInfo  aoiInfo_other;
        AOIBasicList aoiBasicList_self;
        AOIInfo  aoiInfo_self;
        player->writeAddAoiMsg(aoiBasic);
        aoiBasicList_other.push_back(aoiBasic);
        aoiInfo_other.aoiInfoList.assign(aoiBasicList_other.begin(),aoiBasicList_other.end());
        //向周围的玩家广播增加avatar消息
        list<CPlayerPtr>::iterator it = aroundPlayers.begin();
        for (;it!=aroundPlayers.end();it++)
        {
            AOIBasic temp;
            CPlayerPtr  other = (*it);
            if (other != NULL)
            {
                other->writeAddAoiMsg(temp);

                other->SendToClient(GAME_SEND_CLIENT_AOI_BROADCAST,aoiInfo_other);
                aoiBasicList_self.push_back(temp);
            }
        }

        //向自己广播周围玩家的信息
        if (aoiBasicList_self.size() != 0)
        {
            aoiInfo_self.aoiInfoList.assign(aoiBasicList_self.begin(),aoiBasicList_self.end());
            player->SendToClient(GAME_SEND_CLIENT_AOI_BROADCAST,aoiInfo_self);
        }
        return 0;
    }
    return 1;    
}

uint32  CMap::RemovePlayer(AccountID id)
{
    CPlayerPtr player = CPlayerManager::Instance()->GetPlayerByAccount(id);;
    if (player!=NULL)
    {
        std::set<AccountID>::iterator it_idVec = m_playerAccountIDsVec.find(id);
        if ( it_idVec != m_playerAccountIDsVec.end())
        {
            LOG_INFO("player AccountID=%d already in map",id);
            return 1;
        }
        m_playerAccountIDsVec.erase(it_idVec);
        AOIBasic  aoiBasic;
        player->writeRemoveMsg(aoiBasic);
        //向其他玩家广播删除消息
        POSITION  pos = player->getPlayerPos();
        list<CPlayerPtr> aroundPlayers;
        aroundPlayers.clear();
        GetCirclePlayer(pos, aroundPlayers);
        AOIBasicList aoiBasicList_other;
        AOIInfo  aoiInfo_other;
        aoiBasicList_other.push_back(aoiBasic);
        aoiInfo_other.aoiInfoList.assign(aoiBasicList_other.begin(),aoiBasicList_other.end());

        //向其他玩家广播删除玩家消息
        list<CPlayerPtr>::iterator iter = aroundPlayers.begin();
        for (;iter!=aroundPlayers.end();iter++)
        {
            (*iter)->SendToClient(GAME_SEND_CLIENT_AOI_BROADCAST,aoiInfo_other);
        }
    }
    return 0;
}
/*
uint32  CMap::AddPlayer(CPlayerPtr player)
{
    if (player != NULL)
    {
        AccountID  playerID = player->GetAccountID();
        if (m_playersMap.find(playerID) != m_playersMap.end())
        {
           LOG_INFO("player AccountID=%d already in map",playerID);
           return 1;
        }
        m_playersMap.insert(std::make_pair(playerID,player));
        

        POSITION  pos = player->getPlayerPos();
        list<CPlayerPtr> aroundPlayers;
        aroundPlayers.clear();
        GetCirclePlayer(pos,C_BROADCAST_RADIUS,aroundPlayers);

        
        AOIBasic  aoiBasic;
        AOIBasicList aoiBasicList_other;
        AOIInfo  aoiInfo_other;
        AOIBasicList aoiBasicList_self;
        AOIInfo  aoiInfo_self;
        player->writeAddAoiMsg(aoiBasic);
        aoiBasicList_other.push_back(aoiBasic);
        aoiInfo_other.aoiInfoList.assign(aoiBasicList_other.begin(),aoiBasicList_other.end());
        //向周围的玩家广播增加avatar消息
        list<CPlayerPtr>::iterator it = aroundPlayers.begin();
        for (;it!=aroundPlayers.end();it++)
        {
            AOIBasic temp;
            CPlayerPtr  other = (*it);
            if (other != NULL)
            {
                other->writeAddAoiMsg(temp);
                 
                other->SendToClient(C_TEMP_ID,aoiInfo_other,sizeof(aoiInfo_other));
                aoiBasicList_self.push_back(temp);
            }
        }
        
        //向自己广播周围玩家的信息
        if (aoiBasicList_self.size() != 0)
        {
            aoiInfo_self.aoiInfoList.assign(aoiBasicList_self.begin(),aoiBasicList_self.end());
            player->SendToClient(C_TEMP_ID,aoiInfo_self,sizeof(aoiInfo_self));
        }
    }
    return 0;    
}

uint32  CMap::RemovePlayer(CPlayerPtr player)
{
    if (player!=NULL)
    {
        AccountID playerID = player->GetAccountID();
        map<AccountID,CPlayerPtr>::iterator it = m_playersMap.find(playerID);
        if ( it == m_playersMap.end())
        {
            return 1;
        }
        m_playersMap.erase(it);//删除
        AOIBasic  aoiBasic;
        player->writeRemoveMsg(aoiBasic);
        //向其他玩家广播删除消息
        POSITION  pos = player->getPlayerPos();
        list<CPlayerPtr> aroundPlayers;
        aroundPlayers.clear();
        GetCirclePlayer(pos,C_BROADCAST_RADIUS,aroundPlayers);
        AOIBasicList aoiBasicList_other;
        AOIInfo  aoiInfo_other;
        aoiBasicList_other.push_back(aoiBasic);
        aoiInfo_other.aoiInfoList.assign(aoiBasicList_other.begin(),aoiBasicList_other.end());
        
        //向其他玩家广播删除玩家消息
        list<CPlayerPtr>::iterator iter = aroundPlayers.begin();
        for (;iter!=aroundPlayers.end();iter++)
        {
            (*iter)->SendToClient(C_TEMP_ID,aoiInfo_other,sizeof(aoiInfo_other));
        }
    }
    return 0;
}
*/
uint32  CMap::GetCirclePlayer(POSITION center, list<CPlayerPtr>& players)
{
    std::set<AccountID>::iterator iter = m_playerAccountIDsVec.begin();
    for (; iter != m_playerAccountIDsVec.end(); ++iter)
    {
        CPlayerPtr player = CPlayerManager::Instance()->GetPlayerByAccount(*iter);
        if (IsInAoiScope(center, player->GetPos()));
        {
            players.push_back(player);
        }
    }
    return 0;
}

bool CMap::IsInAoiScope(POSITION center, POSITION pos)
{
    float X = pos.fPositionX - center.fPositionX;
    float Z = pos.fPositionZ - center.fPositionZ;
    double distance = sqrt(X*X + Z*Z);

    //判断在范围内， 排除自己
    if (distance >= 1.0e-6 && distance <= C_BROADCAST_RADIUS)
    {
        return true;
    }
    else
    {
        return false;
    }

   
}

uint32  CMap::OnMove(AccountID acID,POSITION lastPos)
{
    CPlayerPtr player = CPlayerManager::Instance()->GetPlayerByAccount(acID);
    if (player!=NULL)
    {
        AccountID playerAcID = player->GetAccountID();
        if (m_playerAccountIDsVec.find(playerAcID) == m_playerAccountIDsVec.end())
        {
            LOG_INFO("player AccountID=%d not in map",playerAcID);
            return 1;
        }

        POSITION  curPos = player->getPlayerPos();

        list<CPlayerPtr> oldAroundPlayers;
        list<CPlayerPtr> curAroundPlayers;
        oldAroundPlayers.clear();
        curAroundPlayers.clear();


        GetCirclePlayer(lastPos, oldAroundPlayers);
        GetCirclePlayer(curPos , curAroundPlayers);


        set<AccountID> removeSet;
        set<AccountID> addSet;
        set<AccountID> moveSet;

        list<CPlayerPtr>::iterator it=oldAroundPlayers.begin();
        for (;it!=oldAroundPlayers.end();it++)
        {
            bool bIsExist = false;
            list<CPlayerPtr>::iterator iter = curAroundPlayers.begin();
            for (;iter!=curAroundPlayers.end();iter++)
            {
                if ((*iter)->GetAccountID() == (*it)->GetAccountID())
                {
                    bIsExist = true;
                    break;
                }
            }
            if (bIsExist)//存在
            {
                moveSet.insert((*it)->GetAccountID());
            }
            else
                removeSet.insert((*it)->GetAccountID());

        }
        //增加的
        list<CPlayerPtr>::iterator iter = curAroundPlayers.begin();
        for (;iter!=curAroundPlayers.end();iter++)
        {
            if (  moveSet.find( (*iter)->GetAccountID() ) == moveSet.end()  ) //不再移动中
            {
                addSet.insert((*iter)->GetAccountID());
            }
        }
        AOIBasic aoiBasicAdd;
        AOIBasic aoiBasicRemove;
        AOIBasic aoiBasicMove;
        AOIBasicList aoiBasicList;
        AOIInfo  aoiInfo;

        player->writeAddAoiMsg(aoiBasicAdd);
        player->writeMoveMsg(aoiBasicMove,lastPos);
        player->writeRemoveMsg(aoiBasicRemove);

        //广播增加avatar消息
        set<AccountID>::iterator iter_add = addSet.begin();
        for (;iter_add!=addSet.end();iter_add++)
        {
            CPlayerPtr player = CPlayerManager::Instance()->GetPlayerByAccount(*iter_add);
            //map<AccountID,CPlayerPtr>::iterator it = m_playersMap.find(*iter_add);
            if (player!=NULL)
            {
                AOIInfo  aoiInfo;
                aoiInfo.aoiInfoList.clear();
                aoiInfo.aoiInfoList.push_back(aoiBasicAdd);
                player->SendToClient(GAME_SEND_CLIENT_AOI_BROADCAST,aoiInfo);
            }
        }
        //广播删除消息
        set<AccountID>::iterator iter_remove = removeSet.begin();
        for (;iter_remove!=removeSet.end();iter_remove++)
        {
            CPlayerPtr player = CPlayerManager::Instance()->GetPlayerByAccount(*iter_remove);
            if (player!= NULL)
            {
                AOIInfo  aoiInfo;
                aoiInfo.aoiInfoList.clear();
                aoiInfo.aoiInfoList.push_back(aoiBasicRemove);
                player->SendToClient(GAME_SEND_CLIENT_AOI_BROADCAST,aoiInfo);
            }
        }
        //广播移动消息
        set<AccountID>::iterator iter_move = moveSet.begin();
        for (;iter_move!=moveSet.end();iter_move++)
        {
            CPlayerPtr player = CPlayerManager::Instance()->GetPlayerByAccount(*iter_move);
            if (player!= NULL)
            {
               AOIInfo  aoiInfo;
               aoiInfo.aoiInfoList.clear();
               aoiInfo.aoiInfoList.push_back(aoiBasicMove);
               player->SendToClient(GAME_SEND_CLIENT_AOI_BROADCAST,aoiInfo);
            }
        }
        return 0;
    }
    else
    {
        LOG_ERROR(" CMap::OnMove player AccountID=%d not online ",acID);
        return 1;
    }
}

/*uint32  CMap::OnMove(CPlayerPtr player,POSITION lastPos)
{
    if (player == NULL)
    {
        LOG_INFO("CMap::onMove :  player == NULL");
        return 1;
    }
   
    if (m_playersMap.find(player->GetAccountID()) == m_playersMap.end() )
    {
        LOG_INFO("CMap::onMove,Account %d not in map!!",player->GetAccountID());
        return 1;
    }
   
    POSITION  curPos = player->getPlayerPos();

    list<CPlayerPtr> oldAroundPlayers;
    list<CPlayerPtr> curAroundPlayers;
    oldAroundPlayers.clear();
    curAroundPlayers.clear();


    GetCirclePlayer(lastPos,C_BROADCAST_RADIUS,oldAroundPlayers);
    GetCirclePlayer(curPos,C_BROADCAST_RADIUS,curAroundPlayers);


    set<AccountID> removeSet;
    set<AccountID> addSet;
    set<AccountID> moveSet;

    list<CPlayerPtr>::iterator it=oldAroundPlayers.begin();
    for (;it!=oldAroundPlayers.end();it++)
    {
        bool bIsExist = false;
        list<CPlayerPtr>::iterator iter = curAroundPlayers.begin();
        for (;iter!=curAroundPlayers.end();iter++)
        {
            if ((*iter)->GetAccountID() == (*it)->GetAccountID())
            {
                bIsExist = true;
                break;
            }
        }
        if (bIsExist)//存在
        {
            moveSet.insert((*it)->GetAccountID());
        }
        else
            removeSet.insert((*it)->GetAccountID());

    }
    //增加的
    list<CPlayerPtr>::iterator iter = curAroundPlayers.begin();
    for (;iter!=curAroundPlayers.end();iter++)
    {
        if (  moveSet.find( (*iter)->GetAccountID() ) == moveSet.end()  ) //不再移动中
        {
            addSet.insert((*iter)->GetAccountID());
        }
    }
    AOIBasic aoiBasicAdd;
    AOIBasic aoiBasicRemove;
    AOIBasic aoiBasicMove;
    AOIBasicList aoiBasicList;
    AOIInfo  aoiInfo;
    
    player->writeAddAoiMsg(aoiBasicAdd);
    player->writeMoveMsg(aoiBasicMove,lastPos);
    player->writeRemoveMsg(aoiBasicRemove);

    //广播增加avatar消息
    set<AccountID>::iterator iter_add = addSet.begin();
    for (;iter_add!=addSet.end();iter_add++)
    {
        map<AccountID,CPlayerPtr>::iterator it = m_playersMap.find(*iter_add);
        if (it!=m_playersMap.end())
        {
            CPlayerPtr p = (it->second);
            if (p!= NULL)
            {
                AOIInfo  aoiInfo;
                aoiInfo.aoiInfoList.clear();
                aoiInfo.aoiInfoList.push_back(aoiBasicAdd);
                p->SendToClient(C_TEMP_ID,aoiInfo,sizeof(aoiInfo));
            }
        }
    }
    //广播删除消息
    set<AccountID>::iterator iter_remove = removeSet.begin();
    for (;iter_remove!=removeSet.end();iter_remove++)
    {
        map<AccountID,CPlayerPtr>::iterator it = m_playersMap.find(*iter_remove);
        if (it!=m_playersMap.end())
        {
            CPlayerPtr p = it->second;
            if (p!= NULL)
            {
                AOIInfo  aoiInfo;
                aoiInfo.aoiInfoList.clear();
                aoiInfo.aoiInfoList.push_back(aoiBasicRemove);
                p->SendToClient(C_TEMP_ID,aoiInfo,sizeof(aoiInfo));
            }
        }
    }
    //广播移动消息
    set<AccountID>::iterator iter_move = moveSet.begin();
    for (;iter_move!=moveSet.end();iter_move++)
    {
        map<AccountID,CPlayerPtr>::iterator it = m_playersMap.find(*iter_move);
        if (it!=m_playersMap.end())
        {
            CPlayerPtr p = it->second;
            if (p!= NULL)
            {
                AOIInfo  aoiInfo;
                aoiInfo.aoiInfoList.clear();
                aoiInfo.aoiInfoList.push_back(aoiBasicMove);
                p->SendToClient(C_TEMP_ID,aoiInfo,sizeof(aoiInfo));
            }
        }
    }
    return 0;
}*/