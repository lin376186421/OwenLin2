//
//  Building_Clan.h
//  GameClient
//
//  Created by qsun on 13-9-16.
//
//

#ifndef __COC__BUILDING_CLAN__H__
#define __COC__BUILDING_CLAN__H__

#include "BaseDef.h"
#include "Building.h"

NS_COC_BEGIN

class Building_Clan : public Building, public ClanInfo
{
public:
    
public:
	Building_Clan(BuildingInfo info);
	virtual ~Building_Clan();
    
    virtual void init(int buildingid, int lx, int ly);
    
	virtual void setLogicPos(int lx, int ly);
        
	virtual void releaseFromScene();
    
	virtual void reorder(int zorder);
    
	virtual void chgSelMode(bool isSel);
    
	virtual void onProc(int ot);
    
	virtual void onBuildingEnd();
    
	virtual void setBeAttackMode();
    
    virtual void onBuilding(bool isBuilding);

	void updateClan(int cid, int flag, const char* name);

	void setClanCD(int fulltime, int lasttime);
public:
    CCSprite* m_pBulding;
	CCSprite* m_pShadow;

	CCSprite* m_pLightArea;

	CCLabelTTF*	m_pClanName;
    CCSprite* m_pClanIcon;

	CCAction* m_pSelAction;
    
    int m_isDefenseActive;
    int m_outSoldierTime;
};

NS_COC_END

#endif /* defined(__COC__BUILDING_CLAN__H__) */
