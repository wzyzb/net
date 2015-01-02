#ifndef _MAPDATA_H_
#define _MAPDATA_H_


#include "Commdefine.h"
#include "protocol_common.h"
using namespace Protocol;

typedef std::pair<uint64,uint32> ITEMNUMPAIR;
typedef std::pair<uint32,uint32> MONSTERIDNUMPAIR;

class CMapData : public Ref_Object
{
public:
	CMapData(){}
	virtual ~CMapData(){}

    bool        IsBattle()
    {
        if (m_mapStyle == MapType_Normal)
        {
            return false;
        }
        return true;
    }
public:
	uint32                      m_mapID; //地图ID
    uint16                      m_mapMode;//地图模式
    uint32                      m_mapModeParam;//模式参数
	uint32                      m_mapStyle;//地图类型
	uint32                      m_mapLvl;//地图等级
    std::vector<ITEMNUMPAIR>    m_consumeItemVec;
	std::vector<ITEMNUMPAIR>    m_resRewardVec;//资源奖励
    uint64                      m_dropItemID;
	uint32                      m_mapLimit;//进入次数限制
	POSITION                    m_mapBornPos;//出生点
    uint8                       m_revivalStyle;//复活类型
    int16                       m_revivalTimes;//复活次数
    POSITION                    m_revivalPosition;//复活点
    std::vector<MONSTERIDNUMPAIR> m_monsterLimit;//怪物同时存在的数量上限
};

typedef Smart_Ptr<CMapData> CMapDataPtr;
#endif
