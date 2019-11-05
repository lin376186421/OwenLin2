#ifndef __COC_BUILDING_STORAGE_H__
#define __COC_BUILDING_STORAGE_H__

#include "BaseDef.h"
#include "Building.h"

NS_COC_BEGIN

class Building_Storage : public Building
{
public:
	Building_Storage(BuildingInfo info);
	virtual ~Building_Storage();
public:
	virtual void init(int buildingid, int lx, int ly);

	virtual void setLogicPos(int lx, int ly);

	virtual void releaseFromScene();

	virtual void reorder(int zorder);

	virtual void chgSelMode(bool isSel);

	virtual void onBuildingEnd();

	virtual void setBeAttackMode();

	void onChgStoreRes();

	virtual void onRefurbish();
public:
	CCSprite*		m_pBulding;
	CCSprite*		m_pShadow;

	CCSprite*		m_pLevel;
	CCSpriteFrame*	m_pLevelFrame[4];
	int				m_iLevel;

	CCAction*		m_pSelAction;
	CCAction*		m_pSelAction_Level;
};

NS_COC_END

#endif  // __COC_BUILDING_STORAGE_H__