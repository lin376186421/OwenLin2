#ifndef __COC_BUILDINGINFOMGR_H__
#define __COC_BUILDINGINFOMGR_H__

#include "BaseDef.h"
#include "BaseCSV.h"

NS_COC_BEGIN

class BuildingInfoMgr : protected BaseCSV 
{
public:
	typedef std::pair<int, std::string>		_PairBuildingInfo;
	typedef std::map<int, std::string>		_MapBuildingInfo;
protected:
	BuildingInfoMgr();
	~BuildingInfoMgr();
public:
	static BuildingInfoMgr& getSingleton();

	void release();

	const char* getBuildingInfo(int buildingid) const;
public:
	virtual void onLoadEnd();
public:
	_MapBuildingInfo		m_mapBuildingInfo;
};

NS_COC_END

#endif  // __COC_BUILDINGINFOMGR_H__