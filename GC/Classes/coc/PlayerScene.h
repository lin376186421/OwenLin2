#ifndef __COC_PLAYERSCENE_H__
#define __COC_PLAYERSCENE_H__

#include "BaseDef.h"
#include "SceneBgr.h"
#include "AStar.h"
#include "Path.h"
#include "SelectBuildingListener.h"
#include "GoldEmitter.h"
#include "OilEmitter.h"
#include "BuildingDestroyEmitter.h"
#include "Projectile2.h"
#include "GameLayer.h"
#include "WorldSceneNode.h"
#include "Building_Wall.h"

class GameWorld;

USING_NS_CC_ZEPHYR;
NS_COC_BEGIN

class PlayerScene : public CCObject, public CCBAnimationManagerDelegate
{
public:
	
	typedef std::list<Building*>	_BuildingList;
	typedef std::list<Person*>		_PersonList;

	typedef std::vector<CCSprite*>	_SpriteList;

	struct _BombInfo{
		CCAction*	pAct;
		CCSprite*	pBomb;
		bool		isBoom;
	};

	struct _BoomInfo{
		CCAction*	pAct;
		CCSprite*	pBoom;
	};

	struct _TombInfo{
		CCSprite*	pTomb;

		int			lx;
		int			ly;
	};

	struct _TmpAniObj{
		CCNode*		pNode;
		CCAction*	pAct;
	};

	struct _FindPosNode{
		int		lx;			//! 坐标
		int		ly;
		float	dis;		//! 距离
		int		snum;		//! 已有人数
	};
    
    struct _PathCacheNode {
        Path path;
        Person* pPerson;
        BaseObj* pDestTarget;
        Building* pWall;
    };
    
public:
	static PlayerScene& getSingleton();
private:
	PlayerScene();
	~PlayerScene();
public:
	void init(int w, int h);
    
    void initScene();

	void release();
    
    void onIdle(int ot);
    
    void onNormalProc(int ot);
    
    void onPersonProc(int ot);
    
    void onBuildingProc(int ot);
    
    void onFightProc(int ot);
    
    void onReplayProc(int ot);

	CCPoint countScenePos(int lx, int ly);
    
    CCPoint countTilePos(int sx, int sy);

	bool isInTile(int ox, int oy);

	//void setSel(int x, int y);

	void addBuilding(int buildingid, int lx, int ly);

	void setScale(float scale, CCPoint ptCenter);

	float getScale() const { return m_scale; }

	void setPosition(int x, int y);

    bool canBuilding(int buildingid, int lx, int ly, int lw);
    
    bool canBuildingEx(int buildingid, int lx, int ly, int lw);
    
    bool canBuildingLine(int buildingid, int lx, int ly, int lw);
        
	Building* addBuildingEx(int buildingid, UserBuildingInfo* pUBI);

	Building* addBuildingEx(int buildingid, int lx, int ly, UserBuildingInfo* pUBI);

	CCSprite* getTileColor(bool canBuild, int lw);

	Building_Wall* isWall(int lx, int ly);

    bool collectRes(int x, int y, int lx, int ly);
    
	void onTouchBegan(int sx, int sy);
	void onTouchMoved(int ox, int oy);
	void onTouchEnded(int ox, int oy);
    
	Person* addPerson(int personid, int lx, int ly, int act, int camp);

	Person* addTowerPerson(int personid, int lx, int ly);

    void sortAllChildren();
    
	void rebuildMapArea(bool toAttack);

	void setAreaSpr(int x, int y, const char* name, bool toAttack);

	void setBuildUIPos(int x, int y, bool bNoOK);

	void hideBuildUI();

	void cancelCurBuilding();

	void buildCurBuilding();

	void showArrow(int lx, int ly, int lw);

	void hideArrow();

	void setBuildUI(CCNode* ui);

	void onRefurbishBuilding();

	void addSelectBuildingListener(SelectBuildingListener* pListener);

	void removeSelectBuildingListener(SelectBuildingListener* pListener);

	void onBuildingSelect(bool isSelected, Building* pBuilding);

	void shootGold(Building& building, int nums);
	void shootOil(Building& building, int nums);

	Building* onUpdateBuilding(UserBuildingInfo* pUBI);

	void onRemoveBuilding(UserBuildingInfo* pUBI);

	void clearScene();

	void deleteBuilding(Building* pBuilding);

	void onInitOther();

	void onUpdateBarrack(BarrackInfo& barrack);

	void onInitTroophousing(TroophousingInfo& troophousing, bool isOther, bool isOnlyHero = false);
    
    void onSetLogicMap(int lx, int ly, int lw, int buildingid, int value, bool isUpdOccData = true);
	void onRefurbishLogicMap();
    
	Building* findTroophousing(int troophousingid);

	void countCurPosTroophousing(CCPoint& pt, int curindex);

	Person* dropPerson2Attack(int personid, int lx, int ly);

	//! 判断一个Person是否还存在
	bool hasPerson(Person* pPerson);
	//! 添加防守方的英雄
	void addDefenceHero();
	//! 修正落地的坐标（尝试分散下落的士兵）
	void fixDropPosition(int& lx, int& ly);
        
    void revengeAttack(Person* pPerson);
    
    // mode 0 - 地面, 1 - 天空, 2 - 天空&地面
	Person* findNearPerson(int lx, int ly, float min, float max, int mode, int camp);
    
    // type == 0墙以外的建筑，最近
	// type == 1攻击建筑
	// type == 2炸弹人的找墙
	// type == 3资源
	// type == 5普通士兵的找墙策略
	// type == 6远程的策略
	Building* findNearAttackBuilding(Person* pPerson, int lx, int ly, int type);
    
    BaseObj* findNextAttackBuilding(Person* pPerson, bool isFlying, bool canJump = false);
    BaseObj* findNextAttackPerson(Person* pPerson, bool isFlying, bool canJump = false);
    
	Person* findNearSoldierWithAddHP(Person* pPerson);  // 查找最近的加血士兵
    
    Building_Wall* findAttackWall(Person* pPerson, BaseObj* pDestObj, int findnums = 0);
    
    bool checkAttackWall(Person* pPerson, Building_Wall* pWall, BaseObj* pDestObj);
    
    CCPoint findAttackPos(Person* pPerson, BaseObj* pDestObj, CCPoint destPt);
            
    void fixAttackPos(Person* pPerson);
    
    BaseObj* checkAttackObject(Person* pPerson, BaseObj *pDestObj);
    
    //! 判断目标是否可以被攻击
	bool canAttackTarget(Person* pPerson, BaseObj* pDestObj);
    
    bool searchCachePath(Person* pPerson, BaseObj* &pDestTarget);

	void killPerson(Person* pPerson);

	void addTomb(int lx, int ly);

	void addBuildingBoom(int sx, int sy);

	void addCannonSmoke(int sx, int sy);

	void addGetResAni(int sx, int sy, int nums,const ccColor3B &color);

	void onGoHome();

	int countBuildingNums();

	int countSoldierNums(int camp);
    
	void onReplay();

	void onStopReplay();

	void onFixPos(float& x, float& y);

	void onStopAttack();

	void onTroophousingMov(Building& building);

	void addTile(CCSprite* pSpr);

	void removeTile(CCSprite* pSpr);

	void addShadow(CCSprite* pSpr);

	void addFlyingShadow(CCSprite* pSpr);

	void removeShadow(CCSprite* pSpr, bool isFlying);

	void addParticle(CCSprite* pSpr);

	void removeParticle(CCSprite* pSpr);

	void initOtherPerson();

	void randXY(int& x, int& y);
    
    void randRangeXY(int& x, int &y, int range);

	Projectile2* addProjectile2(const char* strFrame, int bx, int by, BaseObj* pObj, float bendVal, const char* strBomb, bool isDrop, bool earthquake, bool bRotate, float speed, const char* strEndFrame, float endTime,
         bool noTrack, bool isAOE, float damage, int camp, float rAOE, bool flyfont);

	void addBoomEx(const char* strBomb, int dir, int sx, int sy, bool isFlip = false);

	void addBoomEx2(const char* strBomb, int dir, int sx, int sy, int zorder);

	void setUpDownAction();

	void onUpdateHero();
        
    Building* findBuilding(int buildingid);

	void centerOfBuilding(Building& building);
    
    void centerOfPoint(CCPoint point);

	void setCenterPos(float x, float y);

	void centerOfTownhall();

	void unselectCurBuilding();

    void onDestroyBuilding(Building& building);
    
    // mode 0 - 地面, 1 - 天空, 2 - 天空&地面
	void procAOE(int x, int y, int damage, float range, int camp, int mode, bool isWall, int damageEx, bool flyfont);
	void procStab(int x, int y, float range, int camp, float rx, float ry);

	void chg2Win(int camp = CAMP_ATTACK);

	// 这里的坐标是逻辑坐标，上角是原点，返回上下左右4方向
	int countDirWithWall(int ox, int oy);
	// countDirWithWall的反函数
	void countOffPosWithWall(int dir, int& ox, int& oy);
	// 判断下一个墙出现的方向
	int countNextDirWithWall(int clx, int cly);
	//// 刷新墙，主要计算连接点
	//void refurbishAllWallForAttack();
	//! 士兵升级
	void onSoldierUpgrade(int soldierid);
	// 快速升级士兵
	void onFastUpgradeSoldier(int ot);

	bool hasAttackBuilding();

	bool hasResourceBuilding();

	void addNote(const char* str);

	void onDropSoldier2Attack(int sx, int sy);

	void procAOE_Medical(int x, int y, int damage, float range, int camp, bool isWall, int damageEx);

	void refurbishWall();

	void clearWallLine();

	// 处理 push
	void onProcPushMsg();

	void setReplaySpeed(int speed);

	void chgScaleMode(bool isScaleMode);

	void selectBuilding(Building& building);

    bool canSelectWallLine();
    
    bool canBuildWallLine();
    
	//! 选择一排墙
	bool selectWallLine();

	//! 显示箭头
	void showWallLineArrow(int bx, int by, int ex, int ey);
	//! 清空墙选择
	void clearSelectWallLine();
	//! 墙升级，更新选择列表
	void chgSelectWallLine(Building* ob, Building* nb);
    //! 一排墙回到原来的位置
	void resetSelectWallLine();
    
	//! 判断一个墙是否在选择的墙中
	bool isSelectWallLine(Building* building);
	//！判断是否点中了一排墙中的一个
	bool isInSelectWallLine(int lx, int ly);
	//! 移动一排墙
	void moveSelectWallLine(CCPoint& pt);
    //! 旋转一排墙
	void rotationSelectWallLine();
	//! 放下一排墙
	void downSelectWallLine();
	
	//! 初始化一排红色地格
	void initRedTileLine(int num);
	//! 取一排红色地格中的一个
	CCSprite* getRedTileLine(int index);

	//! 资源建筑升级，更新资源建筑列表
	void chgResBuilding(Building* ob, Building* nb);

    void buildTrashBuilding(); //生成杂物
    
	CCPoint findCanBuildPos(int buildingid, int lx, int ly, int lw);

	void setBuildingTouchMode(Building& building, bool isTouch);

	int getAreaIndex(int x, int y);

	void onWarEnd();

	void clearTouchMoving();

	void addTouchMoving(int ox, int oy, float offTime);

	void clearNormalSoldier();

	void clearHero();
    
    bool isValidPos(int lx, int ly);

	bool isValidBuildingPos(int buildingid, int lx, int ly, int lw);
    
    bool isValidBuildingPosEx(int buildingid, int lx, int ly, int lw);

	//! 英雄释放技能
	void useHeroSkill(int id);

	bool isHeroDie();

	void initHero(HeroInfo2 hero);
	//int onInitHero(int heroid, Person* pPerson);
    
    void actionEndCallback(std::string actionName);
    
    void completedAnimationSequenceNamed(const char *name);
    
	Building* findBuildingWithIndex(int index);

	Person* findPersonWithIndex(int camp, int index);

	Person* addDefenceSoldier(Building* pBuilding, int soldierid, int skillid);
    
    void addTestLog(const char* s);
    
    void onHeroSelect();
    
    void setIsEditMode(bool isEditMode);
    void setIsEraseMode(bool isEraseMode);
    void moveBuilding2EditList(Building& building);
    
    int getUpgradeWallsMoney();

public:
	WorldSceneNode*	m_pRoot;

	SceneBgr		m_bgr;

	CCImage			m_tilemask;

	_BuildingList	m_lstBuilding;
    
    _BuildingList   m_lstResBuilding;
    
    _BuildingList   m_lstWallBuilding;

	float			m_scale;

	int				m_centerX;
	int				m_centerY;

	int				m_screenW;
	int				m_screenH;

	Building*		m_pCurBuilding;
    
	_SpriteList		m_lstRedTile;

	_SpriteList		m_lstRedTileLine;		//! 用于一排墙的红色地格

	CCPoint			m_ptRootBegin;

	_PersonList		m_lstPerson[3];			//! 攻守分离

	AStar			m_astar;
    
    vector<_PathCacheNode> m_vecPathCache;

	int				m_curSelPersonID;
	bool			m_bSelClanSoldier;

	CCSprite*		m_mapTile[LOGIC_W][LOGIC_W];

	CCSprite*		m_mapArea[LOGIC_W][LOGIC_W];

	CCNode*			m_pBuildUI;

	CCControlButton* m_pBuildOK;
	CCControlButton* m_pBuildCancel;

	CCSprite*		m_pArrow[4];

	int				m_walldirx;
	int				m_walldiry;

	bool			m_isInit;

	std::vector<SelectBuildingListener*>	m_lstSelectBuildingListener;

	GoldEmitter					m_goldEmitter;
	OilEmitter					m_oilEmitter;
	BuildingDestroyEmitter		m_buildingDestroyEmitter;

	bool			m_isProcListener;

	CCPoint			m_ptTouchBegin;

	std::list<_TombInfo>		m_lstTomb;
    
	bool			m_isReplayMode;
	int				m_curCtrlIndex;
	int				m_speedReplay;
    int             m_beginReplayTime;
    
    time_t			m_replayTime;
    int             m_replayReadyTime;

	MapData			m_mapDataOcc;
    
    MapData         m_mapData;
    
    MapData         m_mapAttackPos;
    
    bool            m_isAlreadyInit;
    
    GameWorld*      m_pGameWorld;

	CCSpriteBatchNode*		m_pTileBatchNode;
	CCSpriteBatchNode*		m_pParticleBatchNode;

	std::list<Projectile2*>	m_lstProjectile2;

	Person*					m_pHero;
    Person*                 m_pOtherHero;
    
	CCAction*				m_pEarthquakeAct;

	CCPoint					m_ptTouchOff;
	bool					m_isTouched;
	int						m_timeTouched;

	int						m_curFrameCanSearchNums;

	bool					m_isHoldDropSoldier;	// 是否进入持续放兵状态
    bool                    m_isCanDropSoldier;
    bool                    m_isCanHoldDropSoldier;

	bool					m_isScaleMode;			// 是否缩放状态
	bool					m_isTouchBkMove;		// 是否拖动屏幕状态

	CCAction*				m_pTouchMoving;			// 拖的滑动动画
	float					m_timeTouch;

	std::list<ParticleSystem*>		m_lstParticle;

    Building*               m_pTownHall;
    
	Building*				m_pBuildingHeroAltar;
    Building*               m_pBuildingClan;
    Building*               m_pBuildingClanAllies;
    
	bool					m_bDefenceHeroFight;		//! 防守英雄是否出战
    
    int                     m_trashBuildingNums;   //杂物
    
	std::vector<Building_Wall*>	m_lstSelectWall;
	std::vector<Building_Wall*>	m_lstUpgradeWall;
    
    int                     m_backHomeView;
    
	bool                    m_isEditMode;
    bool                    m_isEraseMode;
};

NS_COC_END

#endif  // __COC_PLAYERSCENE_H__
