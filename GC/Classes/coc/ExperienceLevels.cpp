#include "ExperienceLevels.h"

NS_COC_BEGIN

ExperienceLevels::ExperienceLevels()
{
	load("experience_levels.cfg");
}

ExperienceLevels::~ExperienceLevels()
{
	release();
}

ExperienceLevels& ExperienceLevels::getSingleton()
{
	static ExperienceLevels s_mgr;

	return s_mgr;
}

void ExperienceLevels::onLoadEnd()
{
	for(int i = 1; i < getHeight(); ++i)
	{
		_Pair p;

		p.first = atoi(get("Name", i));

		p.second = atoi(get("ExpPoints", i));

		m_mapInfo.insert(p);
	}
}

void ExperienceLevels::release()
{
	m_mapInfo.clear();

	BaseCSV::release();
}

int ExperienceLevels::getCurExp(int level) const
{
	_Map::const_iterator it = m_mapInfo.find(level);
	if(it != m_mapInfo.end())
		return it->second;

	return 0;
}

NS_COC_END
