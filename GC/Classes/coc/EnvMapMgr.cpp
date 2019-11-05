//
//  EnvMapMgr.cpp
//  GameClient
//
//  Created by qsun on 14-2-20.
//
//

#include "EnvMapMgr.h"

NS_COC_BEGIN

EnvMapMgr::EnvMapMgr()
{
	m_isRemote = true;
    
	load("pve.cfg");
}

EnvMapMgr::~EnvMapMgr()
{
}

EnvMapMgr& EnvMapMgr::getSingleton()
{
	static EnvMapMgr s_mgr;
	return s_mgr;
}

void EnvMapMgr::onRelease()
{
	m_mapEvnStages.clear();
}

void EnvMapMgr::onLoadEnd()
{
    CCDictionary* pNames = ViewString::getValueDict("env_stage_names");
    char str[128];

	for(int i = 1; i < getHeight(); ++i)
	{
		_PairEnvStageInfo p;
        
		p.first = atoi(get("stageid", i));
        
		p.second.stage = p.first;
        
        p.second.star = 0;
		p.second.gold = atoi(get("gold", i));
		p.second.oil = atoi(get("oil", i));
        
        p.second.basegold = atoi(get("gold", i));
		p.second.baseoil = atoi(get("oil", i));
        
        sprintf(str, "stage_%d", p.first);
        p.second.name = pNames->valueForKey(str)->m_sString;
        
        m_stageidVec.push_back(p.first);
        
		m_mapEvnStages.insert(p);
	}
}

EnvStageInfo* EnvMapMgr::getEnvStageInfo(int stageid)
{
	_MapEnvStageInfo::iterator it = m_mapEvnStages.find(stageid);
	if(it != m_mapEvnStages.end())
		return &(it->second);
    
	return NULL;
}

int EnvMapMgr::getEnvStageId(int index)
{
    return m_stageidVec[index];
}

int EnvMapMgr::getEnvStageNums()
{
    return m_mapEvnStages.size();
}

NS_COC_END
