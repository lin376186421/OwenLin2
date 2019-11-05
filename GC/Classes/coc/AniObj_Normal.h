/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYRP_ANIOBJ_NORMAL_H__
#define __ZEPHYRP_ANIOBJ_NORMAL_H__

#include "../zephyr/ZephyrBaseDef.h"
#include "AniObj.h"

NS_COC_BEGIN

class AniObj_Normal : public AniObj
{
public:
	AniObj_Normal();
	virtual ~AniObj_Normal();
public:
	static AniObj_Normal* create(const char* str, int frames, float frameTime, float scale, bool isLoop, CCActionManager* amgr);
public:
	bool init(const char* str, int frames, float frameTime, float scale, bool isLoop, CCActionManager* amgr);

	virtual void start();

	virtual void onIdle(float ot);

	virtual bool isFinish();

	virtual void releaseImp();

	virtual void setPosition(const CCPoint& pt);

	virtual void setParent(CCNode* pNode, int zorder);

	virtual void setScale(float scale);
	virtual void setScaleX(float scale);
	virtual void setScaleY(float scale);

	virtual void setAnchorPoint(const CCPoint& anchor);
private:
	CCSprite*			m_pSpr;

	CCAnimate*			m_pAnimate;
	CCAction*			m_pAction;
};

NS_COC_END

#endif  // __ZEPHYRP_ANIOBJ_NORMAL_H__