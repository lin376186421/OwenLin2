//
//  Building_Air.h
//  GameClient
//
//  Created by qsun on 14-5-4.
//
//

#ifndef __GameClient__Building_Air__
#define __GameClient__Building_Air__

#include "BaseDef.h"
#include "Building_Attack.h"

NS_COC_BEGIN

class Building_AirDefense : public Building_Attack
{
public:
	Building_AirDefense(BuildingInfo info);
	virtual ~Building_AirDefense();
public:
	virtual void init(int buildingid, int lx, int ly);
    
	virtual void setLogicPos(int lx, int ly);
        
	virtual void releaseFromScene();
    
	virtual void reorder(int zorder);
    
	virtual void chgSelMode(bool isSel);
    
	virtual void onBuildingEnd();
    
	virtual void setBeAttackMode();
    
	bool canAttack(Person* pPerson);
    
	virtual void onStartAttack();
};

NS_COC_END

#endif /* defined(__GameClient__Building_Air__) */
