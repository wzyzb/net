#ifndef _BATTLEINFO_H_
#define _BATTLEINFO_H_


class CBattleInfo:public Ref_Object
{
public:
    CBattleInfo()
    {

    }
    virtual ~CBattleInfo(){}

public:
    uint32  m_nBattleTime;
    uint32  m_nBattleID;
};

#endif