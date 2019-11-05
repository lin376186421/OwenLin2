/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYR_PROGRESS_H__
#define __ZEPHYR_PROGRESS_H__

#include "ZephyrBaseDef.h"

NS_CC_ZEPHYR_BEGIN

class Progress : public CCNode
{
public:
	Progress();
	virtual ~Progress();
public:
	static Progress* create(CCSpriteFrame* pImg, CCSpriteFrame* pBack, CCRect& rectInsets);
public:
	void setProgress(float fProgress);

	bool init(CCSpriteFrame* pImg, CCSpriteFrame* pBack, CCRect& rectInsets);
public:
	int				m_maxWidth;
	int				m_minWidth;

	float			m_fProgress;

	CCSprite*		m_pBack;
	CCScale9Sprite*	m_pImage;
};

NS_CC_ZEPHYR_END

#endif  // __ZEPHYR_PROGRESS_H__