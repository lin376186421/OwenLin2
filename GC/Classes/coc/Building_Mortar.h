#ifndef __COC_BUILDING_MORTAR_H__
#define __COC_BUILDING_MORTAR_H__

#include "BaseDef.h"
#include "Building.h"
#include "Building_Attack.h"

NS_COC_BEGIN

class Building_Mortar : public Building_Attack
{
public:
	Building_Mortar(BuildingInfo info);
	virtual ~Building_Mortar();
public:
	virtual void init(int buildingid, int lx, int ly);

	virtual void setLogicPos(int lx, int ly);

	virtual void releaseFromScene();

	virtual void reorder(int zorder);

	virtual void chgSelMode(bool isSel);

	virtual void onBuildingEnd();

	virtual void setBeAttackMode();

	virtual void onStartAttack();
public:

//	CCSpriteFrame*	m_pFirstFrame;
//	CCAnimate*		m_pAnimate;
};

NS_COC_END

#endif  // __COC_BUILDING_MORTAR_H__