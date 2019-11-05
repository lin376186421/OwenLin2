#ifndef __COC_BUILDINGMGR_H__
#define __COC_BUILDINGMGR_H__

#include "BaseDef.h"
#include "BaseCSV.h"
#include "Building.h"
#include "Building_Resource.h"
#include "Building_Barrack.h"
#include "Building_Cannon.h"
#include "Building_Storage.h"
#include "Building_Wall.h"
#include "Building_Normal.h"
#include "Building_Attack.h"
#include "Building_Tower.h"
#include "Building_Trap.h"
#include "Building_Mortar.h"
#include "Building_AirDefense.h"
#include "Building_Clan.h"
#include "Building_ClanAllies.h"
#include "Building_HeroAltar.h"

NS_COC_BEGIN

class BuildingMgr : public BaseCSV 
{
public:
	typedef std::pair<int, BuildingInfo>		_PairBuildingInfo;
	typedef std::map<int, BuildingInfo>			_MapBuildingInfo;

	typedef std::vector<Building_Wall*>			_WallList;
protected:
	BuildingMgr();
	~BuildingMgr();
public:
	static BuildingMgr& getSingleton();

	Building* newBuilding(int buildingid);

	void release();

	const BuildingInfo* getBuildingInfo(int buildingid) const;
    const BuildingInfo* getBuildingMaxInfo(int buildingid) const;
    
public:
	virtual void onLoadEnd();

	virtual void onRelease();
public:
	_MapBuildingInfo		m_mapBuildingInfo;
};

NS_COC_END

#endif  // __COC_BUILDINGMGR_H__