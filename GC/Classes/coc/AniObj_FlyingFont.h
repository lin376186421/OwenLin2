/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYRP_ANIOBJ_FLYINGFONT_H__
#define __ZEPHYRP_ANIOBJ_FLYINGFONT_H__

#include "../zephyr/ZephyrBaseDef.h"
#include "AniObj.h"

NS_COC_BEGIN

class AniObj_FlyingFont : public AniObj
{
public:
	AniObj_FlyingFont();
	virtual ~AniObj_FlyingFont();
public:
	static AniObj_FlyingFont* create(const char* str, int time, int yoff, CCActionManager* amgr);
public:
	bool init(const char* str, int time, int yoff, CCActionManager* amgr);

	virtual void start();

	virtual void onIdle(float ot);

	virtual bool isFinish();

	virtual void releaseImp();

	virtual void setPosition(const CCPoint& pt);

	virtual void setParent(CCNode* pNode, int zorder);
public:
	void setString(const char* str);
    
    void setColor(const ccColor3B& color);

	float getWidth();
public:
	CCLabelBMFont*		m_pFont;

	CCAction*			m_pAction;

	int					m_ctime;
	int					m_yoff;
};

NS_COC_END

#endif  // __ZEPHYRP_ANIOBJ_FLYINGFONT_H__