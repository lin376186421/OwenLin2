/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYRP_ANIOBJMGR_H__
#define __ZEPHYRP_ANIOBJMGR_H__

#include "BaseDef.h"
#include "BaseCSV.h"
#include "AniObj.h"

NS_COC_BEGIN

//! 变速的CCActionManager
class ActionManager_SpeedUp : public CCActionManager
{
public:
	ActionManager_SpeedUp();

	//! 设置变速速率
	void setSpeedUp(float speed) { m_fSpeedUp = speed; }

protected:
	virtual void update(float dt);

public:
	float		m_fSpeedUp;
};

struct AniObjInfo{
    int		type;
    
    string	aniname;
    int		iParam[7];
};

class AniObjMgr : public BaseCSV
{
public:

	typedef std::pair<std::string, AniObjInfo>		_Pair;
	typedef std::map<std::string, AniObjInfo>		_Map;
public:
	static AniObjMgr& getSingleton();

	ActionManager_SpeedUp* getActionMgr_Speed() { return m_amgrSpeed; }
protected:
	AniObjMgr();
	virtual ~AniObjMgr();
public:
	AniObj* newAniObj(CCNode* pParent, const char* name, int dir, bool isAutoRelease, bool isLoop, float maxTime, int zorder, bool inscene, int* oparam);

	void onIdle(float ot);

	void cleanup();

	AniObj* findAniObj(CCNode* pParent, const char* str);

    AniObjInfo getAniObjInfo(const char* name);
    
    void releaseAllAni();
    
	void releaseAllAni(CCNode* pParent);
public:
	virtual void onLoadEnd();
public:
	_Map					m_map;

	std::list<AniObj*>		m_lst;

	ActionManager_SpeedUp*	m_amgrSpeed;
};

NS_COC_END

#endif  // __ZEPHYRP_ANIOBJMGR_H__