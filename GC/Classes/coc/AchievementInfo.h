#ifndef __COC_ACHIEVEMENTINFO_H__
#define __COC_ACHIEVEMENTINFO_H__

#include "BaseDef.h"
#include "BaseCSV.h"

NS_COC_BEGIN

class AchievementInfo : protected BaseCSV 
{
public:
	typedef std::pair<int, AchievementNode>			_Pair;
	typedef std::map<int, AchievementNode>			_Map;
protected:
	AchievementInfo();
	~AchievementInfo();
public:
	static AchievementInfo& getSingleton();

	const AchievementNode* getNode(int id) const;

	void release();
public:
	virtual void onLoadEnd();
public:
	_Map		m_mapInfo;
};

NS_COC_END

#endif  // __COC_ACHIEVEMENTINFO_H__