/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYRP_ANIOBJ_FLYINGFONT2_H__
#define __ZEPHYRP_ANIOBJ_FLYINGFONT2_H__

#include "../zephyr/ZephyrBaseDef.h"
#include "AniObj.h"

NS_COC_BEGIN

class AniObj_FlyingFont2 : public AniObj
{
public:
	AniObj_FlyingFont2();
	virtual ~AniObj_FlyingFont2();
public:
	static AniObj_FlyingFont2* create(int time, int yoff, CCActionManager* amgr);
public:
	bool init(int time, int yoff, CCActionManager* amgr);

	virtual void start();

	virtual void onIdle(float ot);

	virtual bool isFinish();

	virtual void releaseImp();

	virtual void setPosition(const CCPoint& pt);

	virtual void setParent(CCNode* pNode, int zorder);
public:
	void setString(const char* str);

	float getWidth();

	void setFontSize(int fontSize) {
		if(m_pFont != NULL)
			m_pFont->setScale(fontSize / 100.0f);

		m_size = fontSize;
	}

	void setFontColor(ccColor3B c) {
		if(m_pFont != NULL)
			m_pFont->setColor(c);
	}
public:
	CCLabelBMFont*		m_pFont;

	CCAction*			m_pAction;

	int					m_ctime;
	int					m_yoff;
	int					m_width;
	int					m_size;
};

NS_COC_END

#endif  // __ZEPHYRP_ANIOBJ_FLYINGFONT2_H__