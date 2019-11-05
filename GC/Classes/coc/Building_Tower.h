#ifndef __COC_BUILDING_TOWER_H__
#define __COC_BUILDING_TOWER_H__

#include "BaseDef.h"
#include "Building_Attack.h"

NS_COC_BEGIN

class Building_Tower : public Building_Attack
{
public:
	Building_Tower(BuildingInfo info);
	virtual ~Building_Tower();
public:
	virtual void init(int buildingid, int lx, int ly);

	virtual void setLogicPos(int lx, int ly);
    
    virtual int getBuildingWidth();
    
    virtual int getBuildingHeight();
    
	virtual void releaseFromScene();

	virtual void reorder(int zorder);

	virtual void chgSelMode(bool isSel);

	virtual void onProc(int ot);

	virtual void onBuildingEnd();

	virtual void setBeAttackMode();
    
    virtual void onStartAttack();

	// 建筑建造状态
	virtual void onBuilding(bool isBuilding);
    
public:
	std::vector<Person*>	m_lstSoldier;
    
    int				m_iDirEx;
};

NS_COC_END

#endif  // __COC_BUILDING_TOWER_H__