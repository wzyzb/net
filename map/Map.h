#ifndef _MAP_H_
#define _MAP_H_

#include "MapData.h"
#include "Player.h"
using namespace std;
using namespace Protocol;

#define C_BROADCAST_RADIUS  10.0

class CMap:public Ref_Object
{
public:
	CMap();
	virtual ~CMap();

    bool Init(const CMapDataPtr& mapData);
	
    bool UnInit();

    /*uint32  AddPlayer(CPlayerPtr player);
    uint32  RemovePlayer(CPlayerPtr player);*/

    uint32  AddPlayer(AccountID id);
    uint32  RemovePlayer(AccountID id);

    uint32  GetCirclePlayer(POSITION center, list<CPlayerPtr>& players);//需要广播的范围

    //uint32  OnMove(CPlayerPtr player,POSITION lastPos);
    uint32  OnMove(AccountID acID,POSITION lastPos);

    bool IsInAoiScope(POSITION center, POSITION pos);

private:
	CMapDataPtr  m_mapData;
	//map<AccountID,CPlayerPtr>  m_playersMap;
    std::set<AccountID>     m_playerAccountIDsVec;
    bool      m_isInit;
    uint32    m_mapID;
    
};

typedef Smart_Ptr<CMap> CMapPtr;
#endif