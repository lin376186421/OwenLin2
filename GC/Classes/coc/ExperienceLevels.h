#ifndef __COC_EXPERIENCELEVELS_H__
#define __COC_EXPERIENCELEVELS_H__

#include "BaseDef.h"
#include "BaseCSV.h"

NS_COC_BEGIN

class ExperienceLevels : protected BaseCSV 
{
public:
	typedef std::pair<int, int>			_Pair;
	typedef std::map<int, int>			_Map;
protected:
	ExperienceLevels();
	~ExperienceLevels();
public:
	static ExperienceLevels& getSingleton();

	int getCurExp(int level) const;

	void release();
public:
	virtual void onLoadEnd();
public:
	_Map		m_mapInfo;
};

NS_COC_END

#endif  // __COC_EXPERIENCELEVELS_H__