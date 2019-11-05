#include "AchievementInfo.h"

NS_COC_BEGIN

AchievementInfo::AchievementInfo()
{
	load("achievement.cfg");
}

AchievementInfo::~AchievementInfo()
{
	release();
}

AchievementInfo& AchievementInfo::getSingleton()
{
	static AchievementInfo s_mgr;

	return s_mgr;
}

void AchievementInfo::onLoadEnd()
{
	for(int i = 1; i < getHeight(); ++i)
	{
		_Pair p;

		p.first = atoi(get("ID", i));

		p.second.id = p.first;
		p.second.level = atoi(get("LEVEL", i));
		p.second.name = get("NAME", i);
		p.second.info = get("INFO", i);
        
        string::size_type pos = 0;
		do{
			pos = p.second.info.find('`');
			if(pos != string::npos)
				p.second.info.replace(pos, 1, ",");
		}while(pos != string::npos);
        
        
		p.second.exp = atoi(get("EXP", i));
		p.second.gem = atoi(get("GEM", i));
		p.second.val = atoi(get("VAL", i));

		m_mapInfo.insert(p);
	}
}

void AchievementInfo::release()
{
	m_mapInfo.clear();

	BaseCSV::release();
}

const AchievementNode* AchievementInfo::getNode(int id) const
{
	_Map::const_iterator it = m_mapInfo.find(id);
	if(it != m_mapInfo.end())
		return &(it->second);

	return NULL;
}

NS_COC_END