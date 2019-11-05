//
//  Building_ClanAllies.h
//  GameClient
//
//  Created by qsun on 14-4-22.
//
//

#ifndef __GameClient__Building_ClanAllies__
#define __GameClient__Building_ClanAllies__

#include "BaseDef.h"
#include "Building.h"

NS_COC_BEGIN

class Building_ClanAllies : public Building, public ClanInfo
{
public:
    
public:
	Building_ClanAllies(BuildingInfo info);
	virtual ~Building_ClanAllies();
    
    virtual void init(int buildingid, int lx, int ly);
    
	virtual void setLogicPos(int lx, int ly);
        
	virtual void releaseFromScene();
    
	virtual void reorder(int zorder);
    
	virtual void chgSelMode(bool isSel);
    
	virtual void onProc(int ot);
    
	virtual void onBuildingEnd();
    
	virtual void setBeAttackMode();
    
    virtual void onBuilding(bool isBuilding);
    
public:
    CCSprite*		m_pBulding;
    
	CCSprite*		m_pFlag;
    
    vector<int> m_lstSoldier;
    int m_outSoldierTime;
};

NS_COC_END

#endif /* defined(__GameClient__Building_ClanAllies__) */
