/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYR_WORLDSCENENODE_H__
#define __ZEPHYR_WORLDSCENENODE_H__

#include "BaseDef.h"

NS_COC_BEGIN

class WorldSceneNode : public CCNode
{
public:
	struct FontState{
		std::string         str;
		int                 offx;
		int                 offy;
	};

	typedef std::pair<int, FontState>	Pair;
	typedef std::map<int, FontState>	Map;
public:
	WorldSceneNode();
	virtual ~WorldSceneNode();
public:
	static WorldSceneNode* create(float w, float h);
public:
	bool init(float w, float h);

	// 这个坐标是绝对坐标，正常情况下应该是负数，因为是摄像机不动，移动场景的
	void chgPos(float x, float y);
	// 缩放，以屏幕中心为原点
    void chgScale(float fScale);
    // 缩放，传入偏移点ptOff是屏幕偏移点，ptCenter是该点对应的场景坐标点
	void chgScaleEx(float fScale, CCPoint& ptCenter, CCPoint& ptOff);
	// 内部函数
	void fixPos(float& x, float& y);
	// 内部函数
	void fixScale(float& fScale);
	// 改变中心坐标，这个坐标是场景中心坐标，一般来说，不是负数
	void chgCenterPos(float x, float y);

	bool onProcScaleAni(float ot);
	// chgCenterPos相对应的函数
	CCPoint getCenterPos();
    // 设置中心点，ptOff是屏幕相对坐标，如果是sw/2、sh/2就是屏幕正中心，那样和chgCenterPos一致
    void chgCenterPosEx(float x, float y, const CCPoint& ptOff);
    // chgCenterPosEx相对应的函数
	CCPoint getCenterPosEx(const CCPoint& ptOff);
public:
	CCSize					m_size;

	float					m_maxScale;
	float					m_maxScaleEx;

	float					m_timeTotalScaleAni;
    
    CCPoint                 m_ptScreenCenter;
    
    bool                    m_bProcScaleAni;

	//SEL_CallFunc			m_pFunc_SetPosition;
	//CCObject*				m_pTarget_SetPosition;
};

NS_COC_END

#endif  // __ZEPHYR_WORLDSCENENODE_H__
