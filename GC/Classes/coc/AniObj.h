/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYRP_ANIOBJ_H__
#define __ZEPHYRP_ANIOBJ_H__

#include "BaseDef.h"

NS_COC_BEGIN

class AniObj : public CCObject
{
public:
	AniObj();
	virtual ~AniObj();
public:
	static AniObj* create();
public:
	bool init();

	virtual void start();

	virtual void onIdle(float ot);

	virtual bool isFinish() { return false; }

	virtual void releaseImp() {}

	virtual void setPosition(const CCPoint& pt) {}

	virtual void setParent(CCNode* pNode, int zorder) { m_pParent = pNode; }

	CCNode* getParent() { return m_pParent; }

	virtual void setScale(float scale) {}

	virtual void setAnchorPoint(const CCPoint& anchor) {}

	bool isLoopEnd();
public:
	bool	m_bAutoRelease;
	bool	m_bLoop;
	float	m_timeBegin;
	float	m_timeMax;				// 动画播放这么长时间就自动停止，只有loop时有效,-1表示无限循环

	std::string		m_nameAni;

	CCNode*			m_pParent;
};

NS_COC_END

#endif  // __ZEPHYRP_ANIOBJ_H__