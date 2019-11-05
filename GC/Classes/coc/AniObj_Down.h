/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYRP_ANIOBJ_DOWN_H__
#define __ZEPHYRP_ANIOBJ_DOWN_H__

#include "../zephyr/ZephyrBaseDef.h"
#include "AniObj.h"

NS_COC_BEGIN

class AniObj_Down : public AniObj
{
public:
	AniObj_Down();
	virtual ~AniObj_Down();
public:
	static AniObj_Down* create(const char* str, int frames, float frameTime, int height, float time, int rotate, CCActionManager* amgr);
public:
	bool init(const char* str, int frames, float frameTime, int height, float time, int rotate, CCActionManager* amgr);

	virtual void start();

	virtual void onIdle(float ot);

	virtual bool isFinish();

	virtual void releaseImp();

	virtual void setPosition(const CCPoint& pt);

	virtual void setParent(CCNode* pNode, int zorder);

	virtual void setScale(float scale);

	virtual void setAnchorPoint(const CCPoint& anchor);
private:
	CCSprite*			m_pSpr;

	CCAnimate*			m_pAnimate;

	CCAction*			m_pAction1;
	CCAction*			m_pAction2;

	int					m_iRotate;
	int					m_iHeight;
	float				m_fAllTime;

	bool				m_bFinish;
};

NS_COC_END

#endif  // __ZEPHYRP_ANIOBJ_DOWN_H__