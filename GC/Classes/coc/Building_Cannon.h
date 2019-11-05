#ifndef __COC_BUILDING_CANNON_H__
#define __COC_BUILDING_CANNON_H__

#include "BaseDef.h"
#include "Building_Attack.h"

NS_COC_BEGIN

class Building_Cannon : public Building_Attack
{
public:
	Building_Cannon(BuildingInfo info);
	virtual ~Building_Cannon();
public:
	virtual void init(int buildingid, int lx, int ly);

	virtual void setLogicPos(int lx, int ly);
    
    virtual int getBuildingWidth();
    
    virtual int getBuildingHeight();
    
	virtual void releaseFromScene();

	virtual void onProc(int ot);

	virtual void reorder(int zorder);

	void chgDirEx(int direx);

	int countDirExWithLogicPos(int bx, int by, int ex, int ey);

	int countDirExWithScreenPos(int bx, int by, int ex, int ey);

	void shoot();

	virtual void setBeAttackMode();

	virtual void chgSelMode(bool isSel);

	virtual void onBuildingEnd();

	virtual void onStartAttack();

	int countDirExIndex(int direx);
public:
	CCSpriteFrame*	m_pFirstFrame[7];
	CCAnimate*		m_pAnimate[7];
	CCSprite*		m_pCannon;

	int				m_iDirEx;

	int				m_lastTurnTime;

	CCAction*		m_pCannonSelAction;
    CCAction*		m_pShootAction;
};

NS_COC_END

#endif  // __COC_BUILDING_CANNON_H__