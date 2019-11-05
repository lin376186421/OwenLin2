#ifndef __COC_BUILDING_NORMAL_H__
#define __COC_BUILDING_NORMAL_H__

#include "BaseDef.h"
#include "Building.h"

NS_COC_BEGIN

class Building_Normal : public Building
{
public:
	Building_Normal(BuildingInfo info);
	virtual ~Building_Normal();
public:
	virtual void init(int buildingid, int lx, int ly);

	virtual void setLogicPos(int lx, int ly);

	virtual void releaseFromScene();

	virtual void reorder(int zorder);

	virtual void chgSelMode(bool isSel);

	virtual int getHeight();
	//virtual bool buildOK();

	virtual void onBuildingEnd();

	virtual void setBeAttackMode();
    
    // 建筑建造状态
	virtual void onBuilding(bool isBuilding);
    
    virtual void onProc(int ot);
    
public:
	CCSprite*		m_pBulding;
	CCSprite*		m_pShadow;

	CCAction*		m_pSelAction;
    
    CCSprite*       m_pSprAni;
    CCAction*       m_pSprAction;
};

NS_COC_END

#endif  // __COC_BUILDING_NORMAL_H__