#ifndef __COC_PERSON_H__
#define __COC_PERSON_H__

#include "BaseDef.h"
#include "SceneBgr.h"
#include "Path.h"
#include "Progress2.h"
#include "BaseObj.h"
#include "CUIExt.h"

NS_COC_BEGIN

class Person : public BaseObj
{
public:
	Person(SoldierInfo info, int camp);
	virtual ~Person();
public:
	virtual void init(int lx, int ly, int act, int skill0);

	virtual void setLogicPos(int lx, int ly);
    
    virtual void reorderMe();
    
    virtual void reorder(int zorder);
    
    const SoldierInfo& getSoldierInfo() const { return m_info; }
	virtual int getMaxHP() { return getSoldierInfo().hpex; }

    virtual bool isFullHp() { return m_hp >= getMaxHP(); }
    
	virtual bool isEmptyHP() { return BaseObj::isEmptyHP(); }

	CCPoint countSceneTilePt(int lx, int ly);
    
    CCPoint countLogicTilePt(int sx, int sy);

	virtual void onProc(int ot);

	void nextPath();

	void addPath(int lx, int ly);

	void chgDir(int dir);

	int countDirWithLogicPos(int bx, int by, int ex, int ey);

	int countDirWithScreenPos(int bx, int by, int ex, int ey);

	void releaseFromScene();

	virtual void chgHP(int off, bool flyfont = false, bool isReal = false);

	bool canAttack();

	bool canAttackBuildingEx(CCPoint& pt, Building& building);

	void onAttackBegin();

	void onTroophousingMov(Building& building);

	void clearPath();

	int countDirIndex(int dir);

    CCPoint getPosition(){ return m_sprPerson->getPosition(); }
	void chgScenePos(float x, float y);
    
    virtual int getWidth();
    virtual int getHeight();

	virtual CCPoint getAttackPos() const;

	void onProcChat(float fOffTime);

	void onProc_DropDown(int ot);

	void onProc_Walk(int ot);

	void onProc_Attack(int ot);

	void onProc_Jump(int ot);

	void onPreAttack_Boom();  // 每次攻击，自爆攻击

	void onPreAttack();  	  // 每次攻击，普通攻击

    void onRevengeAttack();   //反击

	void onProc_Attack_OnTower(int ot);

	void onProc_Normal(int ot);

    void researchAttack();
    
	bool nextAttack();
    
	int getSearchLength();

	void beginAttackAni();

	void chg2Win();

	void chg2WaitNextAttack();

	void beginShootAni();

	virtual void setVisible(bool bVisible);

	void upgrade(int soldierid);

	bool canMedical();

	void onProc_Medical();

	void onPreMedical();

	bool isFlying();

	bool isCanJumpWall();

	void jumpTo(int ex, int ey);

	void flip(float rx, float ry);

	void onMoveEnd();

	//! 释放技能
	void useSkill(int id);

	void onProc_Skill(int ot);

	bool isShootAttack() const;

	void move2Obj(BaseObj& obj);
    
    void move2LogicPos(int lx, int ly);

	//! 是否是英雄
	bool isHero() { return m_personid > 30000; }

	//! 虚函数，载入资源用
	virtual void onLoadRes();
	//! 虚函数，释放资源用
	virtual void onFreeRes();

    bool findNextAddHpSoldier();
    
	//! 攻击周围的敌人
	bool findNextAttackEnemy();
    
    bool findNextAttackBuilding();

	//! 更新寻路占地
	void updateDestPosition(int lx, int ly);
	//! 清除寻路占地
	void clearDestPosition();

	//! 更换目标（录像用）
	void chgDest_Person(Person* person);
	void chgDest_Building(Building* building);

    void chgDestTarget(BaseObj* pDestObj);
    
    void addPersonPath(int lx, int ly, int dx, int dy);
    
    void setPersonPath(int dx, int dy);
    void setPathFlying(int dx, int dy);
        
    bool canStopAttack();
    
    bool canFindAttackStop();
    
    void findDestAttackStop();
    
    void updateDestAttackPos();
    
    void chgState(int act);
    
    void addStatus(int status);

    void removeStatus(int status);
    
    bool isStatus(int status);
    
    int getWalkStepTime();
    
public:
	SoldierInfo     m_info;

	int				m_personid;

	//CCAnimation*	m_pAni[6];
	CCAnimate*		m_pAnimate[5][5];
    
	CCSprite*		m_sprPerson;
	CCNode*			m_nodPersonBask;

	Path			m_path;
	PathNode		m_curPathNode;
	CCPoint			m_curPtOff;
	CCPoint			m_curPtEnd;
    
    float           m_pathTime;
	float			m_lastPathTime;
    
    int             m_walkStep;
    int             m_nextWalkStep;
    
    int             m_walkTime;

	int				m_curDir;
    int             m_curAct;
    int             m_curDirIndex;

	CCAction*		m_pCurAction;

	CCSpriteFrame*	m_pFirstFrame[5][5];

	bool			m_bFlip;

	int				m_iAct;
	int				m_lastDropTime;

	CProgress*			m_pProHP;

	int					m_sleepTime;
	CCPoint				m_ptSleep;

	bool				m_isRelease;

	int				m_speed;
	int				m_attackSpeed;
    
    int             m_defendRange;

	UserBuildingInfo* m_pTroophousing;
	int				m_tlx;
	int				m_tly;

	CCPoint			m_ptTower;

	CCPoint			m_ptOff;
    
    CCPoint         m_ptDestAttack;
    
    int             m_findAttackPosNums;

	int				m_personType;
	int				m_personState;
    int             m_status;

	BaseObj*		m_pDestTarget;
    BaseObj*        m_pWallTarget;

	CCSprite*		m_pShadow;

	CCSprite*		m_pIcon;
	CCLabelBMFont*	m_pMFTime;
	CProgress*		m_pProTime;
    
	int				m_beginAttackTime;		// 从开始攻击到现在的时间
	int				m_damageOnTower;		// 塔上角色的攻击力
	bool			m_isShootAttack;		// 是否是射击攻击类型
    
	bool			m_bAlreadyShoot;		// 已经发射

	float			m_lastResearch;			// 这个时间减到0，重新刷新目标

	int				m_curCamp;				// 阵营

	int				m_lastJumpTime;			// 剩余跳跃时间
	CCPoint			m_ptJumpEnd;

	int				m_iBeforeState;			// 之前的状态（释放技能之后还原）

	bool			m_isShadowFlying;

	int				m_indexPerson;
    
    int             m_attackTarget;
    
    float           m_scale;
};

NS_COC_END

#endif  // __COC_PERSON_H__