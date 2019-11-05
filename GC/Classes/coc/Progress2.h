#ifndef __COC_PROGRESS2_H__
#define __COC_PROGRESS2_H__

#include "BaseDef.h"
#include <map>

#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
USING_NS_CC_EXT;

NS_COC_BEGIN

class Progress2 : public CCNode
{
public:
	Progress2();
	virtual ~Progress2();
public:
	static Progress2* create(const char* strImage, const char* strBack, CCRect& rectInsets);
public:
	void setProgress(float fProgress);

	bool init(const char* strImage, const char* strBack, CCRect& rectInsets);

	void chgImg(const char* strImage);

	virtual void setOpacity(GLubyte opacity);
public:
	int				m_maxWidth;
	int				m_minWidth;

	float			m_fProgress;

	CCSprite*		m_pBack;
	CCScale9Sprite*	m_pImage;
};

NS_COC_END

#endif  // __COC_PROGRESS2_H__