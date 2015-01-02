#include "MapManager.h"
#include "MapDataLoader.h"


uint32  CMapManager::init()
{
    //m_mapDatas.clear();
    m_playerMap.clear();
    m_maps.clear();

    //读取地图配置文件,不需处理有个专门读取地图的类CMapDataLoader
    const map<uint32,CMapDataPtr> mapData = CMapDataLoader::Instance()->getMaps();
    map<uint32, CMapDataPtr>::const_iterator iter = mapData.begin();
    for (; iter != mapData.end(); ++iter)
    {
        onlineMap(iter->first);
    }
    return 0;
}

uint32  CMapManager::uninit()
{
    //m_mapDatas.clear();
    m_playerMap.clear();
    m_maps.clear();
    return 0;
}

uint32  CMapManager::onlineMap(uint32 mapID)
{
    //地图上线  创建地图对象
    CMapPtr pMap(new CMap());
    CMapDataPtr mapData = *(CMapDataLoader::Instance()->GetMapData(mapID));
    pMap->Init(mapData);
    m_maps.insert(std::make_pair(mapID,pMap));
    return 0;
}

uint32  CMapManager::offlineMap(uint32 mapID)
{
    //地图下线  销毁地图对象
    map<uint32,CMapPtr>::iterator it = m_maps.find(mapID); 
    if ( it == m_maps.end())
    {
        LOG_INFO("CMapManager::offlineMap : map %d not online; ",mapID);
        return 1;
    }

    it->second->UnInit();
    m_maps.erase(it);
    return 0;
}

uint32   CMapManager::playerOnline(CPlayerPtr player,uint32 mapID)
{
    if (player==NULL)
    {
        LOG_INFO("ERROR: CMapManager::playerOnline player=NULL");
        return 1;
    }
    //玩家上线 玩家在地图上线
    map<uint32,CMapPtr>::iterator it = m_maps.find(mapID);
    if ( it == m_maps.end() )
    {
        LOG_INFO("CMapManager::playerOnline : map %d is not online",mapID);
        return 1;
    }

    it->second->AddPlayer(player->GetAccountID());
    m_playerMap[player->GetAccountID()]=mapID;
    return 0;
}

uint32  CMapManager::playerOffline(CPlayerPtr player)
{
    return playerLeaveMap(player);
}


uint32  CMapManager::playerLeaveMap(CPlayerPtr player)
{
    //玩家不再主城了 从地图下线
    if (player==NULL)
    {
        LOG_INFO("ERROR: CMapManager::playerLeaveMap player=NULL");
        return 1;
    }
    //玩家从地图下线
    uint32 mapID = player->GetPlayerMapID();
    map<uint32,CMapPtr>::iterator it = m_maps.find(mapID);
    if ( it == m_maps.end() )
    {
        LOG_INFO("CMapManager::playerLeaveMap : map %d is not online",mapID);
        return 1;
    }
    it->second->RemovePlayer(player->GetAccountID());
    map<AccountID,uint32>::iterator iter = m_playerMap.find(player->GetAccountID());
    if (iter != m_playerMap.end())
    {
        m_playerMap.erase(iter);
    }
    return 0;
}

bool  CMapManager::isMapOnline(uint32 mapID)
{
    map<uint32,CMapPtr>::iterator it = m_maps.find(mapID);
    if ( it == m_maps.end() )
    {
        return false;
    }
    return true;
}