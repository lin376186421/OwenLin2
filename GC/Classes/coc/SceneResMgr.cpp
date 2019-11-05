#include "SceneResMgr.h"
#include "ImgMgr.h"

NS_COC_BEGIN

SceneResMgr::SceneResMgr()
{
}

SceneResMgr::~SceneResMgr()
{
}

SceneResMgr& SceneResMgr::getSingleton()
{
	static SceneResMgr s_mgr;

	return s_mgr;
}

void SceneResMgr::loadRes(const char* resname)
{
	_Map::iterator it = m_map.find(resname);
	if(it != m_map.end())
	{
		++(it->second.nums);
	}
	else
	{
		_Pair p;
		p.first = resname;
		p.second.resname = resname;
		p.second.nums = 1;

		m_map.insert(p);

		ImgMgr::getSingleton().addSpriteFrames(resname);
	}
}

void SceneResMgr::freeRes(const char* resname)
{
	return ;

	_Map::iterator it = m_map.find(resname);
	if(it != m_map.end())
	{
		--(it->second.nums);

		if(it->second.nums == 0)
		{
			ImgMgr::getSingleton().removeRes(resname);

			m_map.erase(it);
		}
	}
}

void SceneResMgr::clearRes()
{
	//return ;

	for(_Map::iterator it = m_map.begin(); it != m_map.end(); ++it)
	{
		ImgMgr::getSingleton().removeRes(it->first.c_str());
	}

	m_map.clear();
}

NS_COC_END
