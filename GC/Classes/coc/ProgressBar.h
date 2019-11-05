#ifndef __COC_PROGRESSBAR_H__
#define __COC_PROGRESSBAR_H__

#include "BaseDef.h"
#include "SceneBgr.h"

NS_COC_BEGIN

class ProgressBar
{
public:
	ProgressBar();
	virtual ~ProgressBar();
public:
	void init(const char* bar, const char* bk);

	void release();
public:
	CCSprite*		m_pSprBk;
	CCSprite*		m_pSpr;
};

NS_COC_END

#endif  // __COC_PROGRESSBAR_H__