#ifndef __COC_BUILDING_BARRACK_H__
#define __COC_BUILDING_BARRACK_H__

#include "BaseDef.h"
#include "Building.h"

NS_COC_BEGIN

class Building_Barrack : public Building
{
public:
	Building_Barrack(BuildingInfo info);
	virtual ~Building_Barrack();
public:
	virtual void init(int buildingid, int lx, int ly);

	virtual void setLogicPos(int lx, int ly);

	virtual void releaseFromScene();

	virtual void reorder(int zorder);

	virtual void chgSelMode(bool isSel);

	virtual void onBuildingEnd();

	virtual void setBeAttackMode();

	virtual void onUpdateTime();
public:
	CCSprite*		m_pBulding;
	CCSprite*		m_pShadow;
	//CCSprite*		m_pLighting;

	CCAction*		m_pSelAction;

	int				m_iCurSoldierID;
};

NS_COC_END

#endif  // __COC_BUILDING_BARRACK_H__