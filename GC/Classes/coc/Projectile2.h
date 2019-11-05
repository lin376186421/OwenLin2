#ifndef __COC_PROJECTILE2_H__
#define __COC_PROJECTILE2_H__

#include "BaseDef.h"
#include <map>

using namespace cocos2d;

NS_COC_BEGIN

class Projectile2 : public CCSprite
{
	typedef std::pair<char, std::string>	_Pair;
	typedef std::map<char, std::string>		_Map;
public:
	Projectile2();
	virtual ~Projectile2();
public:
	CREATE_FUNC(Projectile2);
public:
	bool init();

	//CCAction* beginAct(int cx, int cy, int dx, int dy, float param, const char* spr);

	//! speed是1秒移动多少像素
	void shoot2Obj(const char* name, int bx, int by, BaseObj* pObj, float speed, float param, const char* strBoom, bool isDrop,
                   bool earthquake, bool bRotate, const char* strEndFrame, float endTime, 
                   bool isTrack, bool isAOE, float damage, int camp, float rAOE, bool flyfont);
    void shoot2Point(const char* name, int bx, int by, CCPoint point, CCPoint centerPoint, float speed, float param, const char*
                     strBoom, bool isDrop, bool earthquake, bool bRotate, const char* strEndFrame, float endTime,
                     bool isTrack, bool isAOE, float damage, int camp, float rAOE, bool flyfont);

	//void onAnim();

	//virtual void draw();
	void onIdle(float dt);

	void procDir(float cx, float cy, float dx, float dy);

	bool isFinished() const { return m_isRelease || (m_isShootDest && m_curTime >= m_totalTime + m_lastEndFrame); }

	bool isEndState() const { return !m_isRelease && m_curTime >= m_totalTime; }

	CCPoint countPos();
public:
	//int			m_iFrame;
	//CCPoint		m_ptLast;

	BaseObj*			m_pDest;

	//float				m_lastTime;

	float				m_speed;

	CCPoint				m_ptSrc;
	CCPoint				m_ptDest;

	//ccBezierConfig		m_sConfig;

	float				m_totalTime;
	float				m_curTime;

	float				m_bendVal;			// 抛物线高度权值，越大越高

	float				m_chgDirTime;

	std::string			m_strBoom;			// 爆炸炸弹特效
    
    bool                m_bDrop;            //是否空中投放炮弹

	bool				m_bEarthquake;		// 爆炸是否产生地震

	//CCAnimate*			m_pAnimate;
	//CCAction*			m_pAction;

	std::string			m_strEndFrame;
	float				m_lastEndFrame;
	bool				m_isChg2EndFrame;
	float				m_lastDegree;

	bool				m_isRelease;

	bool				m_isTrack;			// 是否追踪目标
	bool				m_isAOE;			// 范围攻击
	float				m_damage;			// 攻击力

	int					m_camp;				// 被攻击方的阵营
	float				m_rAOE;				// AOE的半径

	bool				m_bFlyFont;			//! 是否飘字
    
    CCPoint             m_centerPoint;
    
private:
    bool                m_isShootDest;  // 是否已射击完毕
};

NS_COC_END

#endif  // __COC_PROJECTILE2_H__