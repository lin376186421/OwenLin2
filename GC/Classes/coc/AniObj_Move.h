/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYRP_ANIOBJ_MOVE_H__
#define __ZEPHYRP_ANIOBJ_MOVE_H__

#include "../zephyr/ZephyrBaseDef.h"
#include "AniObj.h"

NS_COC_BEGIN

class AniObj_Move : public AniObj
{
public:
	AniObj_Move();
	virtual ~AniObj_Move();
public:
	static AniObj_Move* create(const char* str, int frames, float frameTime, int bx, int by, int ex, int ey, float time, bool sinein, CCActionManager* amgr);
public:
	bool init(const char* str, int frames, float frameTime, int bx, int by, int ex, int ey, float time, bool sinein, CCActionManager* amgr);

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

	CCAnimate*			m_pAnimate;			//! 动画

	CCAction*			m_pAction1;			//! 循环动画
	CCAction*			m_pAction2;			//! 移动动画

	int					m_iBX;
	int					m_iBY;
	int					m_iEX;
	int					m_iEY;
	float				m_fTime;
	bool				m_bSineIn;
};

NS_COC_END

#endif  // __ZEPHYRP_ANIOBJ_MOVE_H__