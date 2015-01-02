#ifndef _MAPMANAGER_H_
#define _MAPMANAGER_H_

#include "Singleton.h"
#include "MapData.h"
#include "Player.h"
#include "Map.h"
using namespace std;

class CMapManager:public Singleton<CMapManager>
{
protected:
	CMapManager(){}
	virtual ~CMapManager(){}

public:
	uint32  init();

	uint32  uninit();

    uint32  onlineMap(uint32 mapID);
    uint32  offlineMap(uint32 mapID);

    uint32  playerOnline(CPlayerPtr player,uint32 mapID);
    uint32  playerOffline(CPlayerPtr player);

public:
    uint32  playerLeaveMap(CPlayerPtr player);

private:
    uint32  loadMapDataFile();
    bool    isMapOnline(uint32 mapID);
private:
	//map<uint32,CMapDataPtr>      m_mapDatas;
	map<AccountID,uint32>	  m_playerMap;//玩家id,地图id map
    map<uint32,CMapPtr>         m_maps;//上线的地图对象 
    
};




#endif