#ifndef __COC_TOWNHALLLEVELINFO_H__
#define __COC_TOWNHALLLEVELINFO_H__

#include "BaseDef.h"
#include "BaseCSV.h"

NS_COC_BEGIN

class TownhallLevelInfo : public BaseCSV 
{
public:
	typedef std::pair<int, Townhall_BuildingNums>			_PairTownhallInfo;
	typedef std::map<int, Townhall_BuildingNums>			_MapTownhallInfo;
protected:
	TownhallLevelInfo();
	~TownhallLevelInfo();
public:
	static TownhallLevelInfo& getSingleton();

	const Townhall_BuildingNums* getInfo(int level) const;

    vector<int> getLevelBuildings();
    
    int getBuildingNums(const Townhall_BuildingNums* buildingNums, int buildid);
    
    int getBuildingCurNums(int buildid);
    int getBuildingMaxNums(int buildid);
    
    int getAttackCost(int level);
    
	void release();
public:
	virtual void onLoadEnd();

	virtual void onRelease();
public:
	_MapTownhallInfo		m_mapInfo;
};

NS_COC_END

#endif  // __COC_TOWNHALLLEVELINFO_H__