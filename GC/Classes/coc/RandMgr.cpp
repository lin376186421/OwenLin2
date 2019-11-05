#include "RandMgr.h"

NS_COC_BEGIN

RandMgr& RandMgr::getSingleton()
{
	static RandMgr s_mgr;

	return s_mgr;
}


RandMgr::RandMgr()
{
}

RandMgr::~RandMgr()
{
}

void RandMgr::init(const Json::Value& val)
{
	m_lst.clear();
	m_cur = getAsInt(val["randcur"]);

	int max = getAsInt(val["max"]);

	for(int i = 0; i < max; ++i)
	{
		char str[128];
		sprintf(str, "n%d", i);
		int num = getAsInt(val[str]);
		m_lst.push_back(num);
	}
}

int RandMgr::rand(int max)
{
	++m_cur;

	if(m_cur < 0)
		m_cur = 0;
	else if(m_cur >= m_lst.size())
		m_cur = 0;

	return m_lst[m_cur] % max;
}

int RandMgr::randWar(int max)
{
	m_warBegin += m_warOff;
	return m_warBegin % max;
}

void RandMgr::initRandWar()
{
	m_warBegin = COC_ATTACK_VER;
	m_warOff = COC_STATIC_RANDKEY;
}

NS_COC_END
