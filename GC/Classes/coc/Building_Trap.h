#ifndef __COC_BUILDING_TRAP_H__
#define __COC_BUILDING_TRAP_H__

#include "BaseDef.h"
#include "Building_Attack.h"

NS_COC_BEGIN

class Building_Trap : public Building_Attack
{
public:
	Building_Trap(BuildingInfo info);
	virtual ~Building_Trap();
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

	virtual void onProc(int ot);
public:
	bool			m_bDestroy;
	int				m_iAttackTime;
};

NS_COC_END

#endif  // __COC_BUILDING_TRAP_H__