/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYRP_ANIOBJ_FLASH_H__
#define __ZEPHYRP_ANIOBJ_FLASH_H__

#include "BaseDef.h"
#include "AniObj.h"

NS_COC_BEGIN

class AniObj_Flash : public AniObj
{
public:
	AniObj_Flash();
	virtual ~AniObj_Flash();
public:
	static AniObj_Flash* create(const char* str, int frames, float frameTime, float scale, bool isLoop, float flashtime, CCActionManager* amgr);
public:
	bool init(const char* str, int frames, float frameTime, float scale, bool isLoop, float flashtime, CCActionManager* amgr);

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
	CCAction*			m_pAction;

	float				m_fFlashTime;
};

NS_COC_END

#endif  // __ZEPHYRP_ANIOBJ_FLASH_H__