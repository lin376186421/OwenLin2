#ifndef __COC_BUILDING_ATTACK_H__
#define __COC_BUILDING_ATTACK_H__

#include "BaseDef.h"
#include "Building.h"

NS_COC_BEGIN

class Building_Attack : public Building
{
public:
	Building_Attack(BuildingInfo info);
	virtual ~Building_Attack();
public:
	virtual void init(int buildingid, int lx, int ly);

	virtual void setLogicPos(int lx, int ly);
    
    virtual void setVisible(bool isVisible);
    
    virtual int getBuildingWidth();
    
    virtual int getBuildingHeight();
    
	virtual void releaseFromScene();

	virtual void reorder(int zorder);

	virtual void chgSelMode(bool isSel);

	virtual void onProc(int ot);

	virtual void onBuildingEnd();

	virtual void setBeAttackMode();

	bool canAttack(Person* pPerson);

	virtual void onStartAttack();
public:
	CCSprite*		m_pBulding;
	CCSprite*		m_pShadow;

	CCSprite*		m_pLightArea[2];

	CCAction*		m_pSelAction;
    
	int				m_lastAttackTimeMs;

	Person*			m_pDestPerson;
};

NS_COC_END

#endif  // __COC_BUILDING_ATTACK_H__