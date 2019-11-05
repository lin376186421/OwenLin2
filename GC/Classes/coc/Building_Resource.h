#ifndef __COC_BUILDING_RESOURCE_H__
#define __COC_BUILDING_RESOURCE_H__

#include "BaseDef.h"
#include "Building.h"

NS_COC_BEGIN

class Building_Resource : public Building
{
public:
	Building_Resource(BuildingInfo info);
	virtual ~Building_Resource();
public:
	virtual void init(int buildingid, int lx, int ly);

	virtual void setLogicPos(int lx, int ly);

	virtual void releaseFromScene();

	virtual void reorder(int zorder);

	virtual void chgSelMode(bool isSel);

	virtual void onBuildingEnd();

	virtual void setBeAttackMode();

	virtual void onProc(int ot);

	// 建筑建造状态
	virtual void onBuilding(bool isBuilding);
public:
	CCSprite*		m_pBulding;
	CCSprite*		m_pLevel[2];

	CCSprite*		m_pShadow;

	CCSprite*		m_pSprAni;

	CCAction*		m_pSelAction[4];

	int				m_lastPersonAniTime;
	CCAnimate*		m_pPersonAni;
	CCAction*		m_pCurPersonAction;
};

NS_COC_END

#endif  // __COC_BUILDING_RESOURCE_H__