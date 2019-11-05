/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYRP_ANIOBJ_WAITFADEOUT_H__
#define __ZEPHYRP_ANIOBJ_WAITFADEOUT_H__

#include "BaseDef.h"
#include "AniObj.h"

NS_COC_BEGIN

class AniObj_WaitFadeOut : public AniObj
{
public:
	AniObj_WaitFadeOut();
	virtual ~AniObj_WaitFadeOut();
public:
	static AniObj_WaitFadeOut* create(const char* str, int time1, int time2, int time3, CCActionManager* amgr);
public:
	bool init(const char* str, int time1, int time2, int time3, CCActionManager* amgr);

	virtual void start();

	virtual void onIdle(float ot);

	virtual bool isFinish();

	virtual void releaseImp();

	virtual void setPosition(const CCPoint& pt);

	virtual void setParent(CCNode* pNode, int zorder);

	virtual void setScale(float scale);
private:
	CCSprite*			m_pSpr;

	int					m_time[3];

	int					m_curTime;

	//CCAction*			m_pAction;
};

NS_COC_END

#endif  // __ZEPHYRP_ANIOBJ_WAITFADEOUT_H__