#include "BuildingInfoMgr.h"

NS_COC_BEGIN

BuildingInfoMgr::BuildingInfoMgr()
{
	load("building_info.cfg");
}

BuildingInfoMgr::~BuildingInfoMgr()
{
	release();
}

BuildingInfoMgr& BuildingInfoMgr::getSingleton()
{
	static BuildingInfoMgr s_mgr;

	return s_mgr;
}

void BuildingInfoMgr::onLoadEnd()
{
	for(int i = 1; i < getHeight(); ++i)
	{
		_PairBuildingInfo p;

		p.first = atoi(get("buildingid", i));
		p.second = get("info", i);

		m_mapBuildingInfo.insert(p);
	}
}

void BuildingInfoMgr::release()
{
	m_mapBuildingInfo.clear();

	BaseCSV::release();
}

const char* BuildingInfoMgr::getBuildingInfo(int buildingid) const
{
	_MapBuildingInfo::const_iterator it = m_mapBuildingInfo.find(buildingid);
	if(it != m_mapBuildingInfo.end())
		return it->second.c_str();

	return NULL;
}

NS_COC_END