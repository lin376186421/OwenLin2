//
//  Building_HeroAltar.h
//  GameClient
//
//  Created by qsun on 14-3-7.
//
//

#ifndef __GameClient__Building_HeroAltar__
#define __GameClient__Building_HeroAltar__

#include "BaseDef.h"
#include "Building.h"

NS_COC_BEGIN

class Building_HeroAltar : public Building
{
public:
	Building_HeroAltar(BuildingInfo info);
	virtual ~Building_HeroAltar();
    
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
	CCSprite*		m_pShadow;
    
	CCSprite*		m_pFlag;
	CCSprite*		m_pLightArea;
    
	CCAction*		m_pSelAction;

};

NS_COC_END

#endif /* defined(__GameClient__Building_HeroAltar__) */
