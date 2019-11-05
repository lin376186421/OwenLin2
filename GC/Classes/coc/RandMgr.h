#ifndef __COC_RANDMGR_H__
#define __COC_RANDMGR_H__

#include "BaseDef.h"
#include <map>

using namespace cocos2d;

NS_COC_BEGIN

class RandMgr
{
public:
	static RandMgr& getSingleton();
private:
	RandMgr();
	virtual ~RandMgr();
public:
	void init(const Json::Value& val);

	int rand(int max);

	int randWar(int max);

	void initRandWar();
public:
	std::vector<int>		m_lst;
	int						m_cur;

	int						m_warBegin;
	int						m_warOff;
};

NS_COC_END

#endif  // __COC_RANDMGR_H__