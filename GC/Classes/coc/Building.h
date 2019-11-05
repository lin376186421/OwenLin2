#ifndef __COC_BUILDING_H__
#define __COC_BUILDING_H__

#include "BaseDef.h"
#include "SceneBgr.h"
#include "Progress2.h"
#include "ParticleSystem.h"
#include "BaseObj.h"
#include "AniObj.h"
#include "CUIExt.h"

NS_COC_BEGIN

class Building : public BaseObj
{
public:
	Building(BuildingInfo info);
	virtual ~Building();
public:
	virtual void init(int buildingid, int lx, int ly);

	virtual void setLogicPos(int lx, int ly);
    
    virtual void setVisible(bool isVisible);

	CCPoint countSceneTilePt(int lx, int ly);
    
    CCPoint getScenePt();
    void setScenePt(CCPoint pt);

	bool operator < (const Building& right) const {
		return getLogicX() + getLogicY() < right.getLogicX() + right.getLogicY();
	}

	bool isIn(int lx, int ly) const;
    bool isInIcon(int px, int py);

	bool isOverlap(int lx, int ly, int lw) const;

	void buildTest();

	virtual void releaseFromScene();

	//virtual void resort();

	virtual void reorder(int zorder);

	virtual bool buildOK();

	virtual void chgSelMode(bool isSel);

	int countDistance(int lx, int ly);

	void chgState(int state);

	void setUpDownAction(CCSprite* pSpr, bool isCenter);

	void showTime();

	virtual int getWidth();

	virtual int getHeight();
    
    virtual int getBuildingWidth();
    
    virtual int getBuildingHeight();

	virtual void onProc(int ot);

	virtual void onUpdateTime();

	void setTime(int fulltime, int lasttime);

	void showResTitle();

	void hideResTitle();

	bool hasNewRes();

	bool canMove();

	void showFence(bool bVisible);

	//bool isEmptyHP() const { return m_hp <= 0; }

	virtual void chgHP(int off, bool flyfont = false, bool isReal = false);

	BarrackInfo* getBarrackInfo();

	void showDestroy();

	void showPreBuilding();

	virtual void onBuildingEnd();
    
    virtual void onRefurbish();

	void setBeAttackAni(CCSprite* spr, bool isMain, bool isCenter = true);

	virtual void setBeAttackMode() = 0;

	int countRes();
	int countRes_Sub();

	bool canBuild();

	void shootGold(int gold);

	void shootOil(int oil);

	void setIcon(const char* str);

	int countAttackMode();

	void setAttackTargetAni();

	void reorderMe();

	bool isFenceVisible() const;
	// 建筑建造状态
	virtual void onBuilding(bool isBuilding) {}

	void onProcIconEx();

	void chgIconEx(const char* strIcon);

	virtual int getMaxHP() { return m_info.hp; }

	bool isAttackBuilding();
    
	bool isResourceBuilding();
    
public:
	int					m_type;

	CCSprite*			m_pTile;

	CCSprite*			m_pTileColor;

	bool				m_isTouch;
	int					m_xBeginTouch;
	int					m_yBeginTouch;
	bool				m_isTouchMoved;

	int					m_buildingid;

	BuildingInfo		m_info;
    UserBuildingInfo*	m_pBuildingInfo;

	int					m_state;

	int					m_lbx;
	int					m_lby;
    
    int                 m_buildingWidth;
    int                 m_buildingHeight;
    
    CCLabelBMFont*      m_pLabelLevel;

	CCLabelBMFont*		m_pMFTime;
	CProgress*			m_pProTime;

	int					m_fulltime;
	int					m_lasttime;
	int					m_lasttimems;

	CCSprite*			m_pFence;

	int64_t             m_curResTime;
	int64_t				m_curResTimems;
    int                 m_collectResTimes;

	CProgress*          m_pProHP;

	CCSprite*			m_pDestroy;
	CCSprite*			m_pPreBuilding;
	bool				m_isPreBuilding;

	CCAction*			m_pActBeAttack;

	ParticleSystem*		m_psGold;
	ParticleSystem*		m_psOil;

	CCSprite*			m_pIcon;

	AniObj*				m_pAttackTarget;
    
    CCParticleSystem*   par_select;

	bool				m_isRemove;

	bool				m_isResFull;		// 资源是否已经满了

	CCSprite*			m_pIconEx;			// 资源、满了、感叹号等
	std::string			m_strIconEx;		// iconex的名字

	int					m_indexBuilding;
    
    CCPoint             m_scenePt;
        
    EditBuildStatus     m_editStatus;
};

NS_COC_END

#endif  // __COC_BUILDING_H__