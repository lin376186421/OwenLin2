#ifndef __COC_LOGICINFO_H__
#define __COC_LOGICINFO_H__

#include "BaseDef.h"
#include "NetListener.h"
#include "NotifyListener.h"
#include "Building_Wall.h"
#include <list>
#include <vector>

using namespace cocos2d;

NS_COC_BEGIN

typedef std::map<int, BarrackInfo>	MapBarrackInfo;
typedef std::pair<int, BarrackInfo>	PairBarrackInfo;

typedef std::map<int, TroophousingInfo>		MapTroophousingInfo;
typedef std::pair<int, TroophousingInfo>	PairTroophousingInfo;

class LogicInfo : public NetListener, public CCObject
{
public:
	
	struct _ListenerNode{
		NotifyListener*		pListener;
		bool				isRelease;
	};

	typedef std::list<_ListenerNode>	ListNotifyListener;
public:
	static LogicInfo& getSingleton();
    
protected:
	LogicInfo();
	~LogicInfo();
public:

	virtual bool onProc(const Json::Value& val);

	void addListener(NotifyListener* pListener);

	void removeListener(NotifyListener* pListener);

    void onNotify(int notifyid, const void* pVal = NULL);
public:
	UserBuildingInfo* findBuilding(int x, int y);
    
	void clearMyScene();

	void clearOtherScene();

	void onRefurbishAllBarrack();
    
	void onRefurbishAllTroophousing();

	bool onRefurbishBarrack(int barrackid, int offtime, Building* pBuilding);

	bool onRefurbishBarrackMs(int barrackid, int ot, Building* pBuilding);
    
    int countTroopSpace();
    
    void countTroopSpace(MapTroophousingInfo troophousingInfo, int &curspace, int &maxspace);

	bool addSoldier(int soldierid, Building* pFrom);

	int addSoldier2Barrack(int barrackid, int pid);

	void delSoldier2Barrack(int barrackid, int pid);

	void onAddBarrack(UserBuildingInfo buildingInfo, const BuildingInfo* pInfo);

	void onCloseBarrack();

	void onOpenBarrack();

	void onAddTroophousing(int troophousingid, int maxnums);

	void onAddOtherTroophousing(int troophousingid, int maxnums);

	void onInitTroophousingSoldier();

	void onInitOtherTroophousingSoldier();
    
    void onFastBuild();
    
    void onRemoveOther(Building& building);

	void harvest(Building& building);

	int upLevel(Building& building, int& need);
    
    int upLevelEx(Building& building, int& need);
    
    int upgradeWalls(vector<Building_Wall*> lstUpgradeWall, int& need);

	void chgGold(int off, int from = MONEY_FROM_NORMAL);

	void chgOil(int off, int from = MONEY_FROM_NORMAL);

	void chgGem(int off, int from = MONEY_FROM_NORMAL);

    void addGem(int off);
    
	void chgMoney(int moneytype, int off, int from = MONEY_FROM_NORMAL);

	int getMoney(int moneytype);

	bool hasFreeWorker() const;

	int buildBuilding(Building& building, int& need);

	Building* addBuilding(int buildingid, int lx, int ly);

    int countBuildingHeroGem();
    
	void finishBuilding(Building& building);
    
    void finishBuildingEx(Building& building);

	void cancelBuild(Building& building);

	bool fastBuilding(Building& building);

	void onAddBuilding(UserBuildingInfo& ubi);

	void onAddOtherBuilding(UserBuildingInfo& ubi);

	bool getGoldEx(int gem);

	bool getOilEx(int gem);

	void refurbishBaseInfo();

	BarrackInfo* getBarrackInfo(int barrackid);

	void refurbishFightSoldierInfo();

	void refurbishOtherFightSoldierInfo();

	void addFightSoldierInfo(int pid, int nums);
    
	void addOtherFightSoldierInfo(int pid, int nums);

	void onBarrackFinish();

	void setSoldier2Fight(int pid);
	void setClanSoldier2Fight();

	void dropSoldier2Fight(int soldierid, int lx, int ly, int buildingindex = -1);

    void endBattle();
    
	void goHome();

	void findAttackUser(int attackType = ATTACK_TYPE_PVP);

	void findAttackUserEx(int uid, int attackType = ATTACK_TYPE_PVP);
    
	void revengeAttackUserEx(int uid, int msgid);
    
    void attackEnv(int stageid);

	bool isFinishAttack();
	
	void attackTimeOver();

	void delSoldierFromTroophousing(int soldierid);

	void updateUserName(const char* name);

	void beginFight();

	void onDestroyBuilding(Building& building);

	void onMySoldierDead(Person& soldier);

	void onDropOutRes(Building& building, float per);

	void onAddCtrl_DropSoldier(int personid, int x, int y);
	void onAddCtrl_KillAtkSoldier(int personindex);
	void onAddCtrl_KillDefSoldier(int personindex);
	void onAddCtrl_DestroyBuidling(int buildingindex);
	void onAddCtrl_UseSkill(int personindex, int skillid);
	//! 录像增加攻击目标
	void onAddCtrl_ChgAtkDest(int personindex, int destype, int destindex);
	void onAddCtrl_ChgDefDest(int personindex, int destype, int destindex);
        
    void onAddCtrl_UseClanSoldier(int x, int y);
    void onAddCtrl_ClanSoldierAttack(int personid, int x, int y);
    void onAddCtrl_ClanSoldierDefend(int buildingindex, int personid, int skillid);
    void onAddCtrl_EndFight();

	int fastBarrack(BarrackInfo* pBarrackInfo, Building* pBuilding);

	void onBeginReplay();
    
	void stopReplay();

	void onStopAttack(bool isPause);

	void addUsedFightSoldier(int personid);

	void loadTownhall();

	void onRefurbishTownhall();

	void resetAll();

	void cancelFirstBuinding(Building& building);
    
	//! 该接口仅用于除草
	int removeBuilding(Building& building, int& need);
	//! 该接口仅用于移除装饰物
	int removeOrnament(Building& building, int& need);

	//! 计算快速建造最少的宝石消耗
	Building* countFastBuildingGem(int& need);
    //! 计算当前建筑需要的宝石消耗
    int countCurBuildingGem();
	//! 计算宝石到金币或者油
	int countGem2Other(int gem);
	//! 计算金币或者油到宝石
	int countOther2Gem(int other);

	void refurbishSoldierNums();

	void addExp(int exp);

//	int countCup(int star, int leveloff);

	void onNotifyLoading(float progress);

	void requestRank();
    
    void setGuide(int val1);
    
    void viewUserEx(int uid, int attackType = ATTACK_TYPE_NULL);
    
	bool isAttackMode() const { return LOGICSTATE_ATTACK == m_curState || LOGICSTATE_REPLAY == m_curState; }
    
	bool isRealAttackMode() const { return LOGICSTATE_ATTACK == m_curState; }
    
    bool isReplayMode() const { return LOGICSTATE_REPLAY == m_curState; }
    
    bool isVisitMode() const { return LOGICSTATE_VIEW == m_curState; }
    
	void chg2Wait();

	bool isNormalMode() const { 
		return LOGICSTATE_NORMAL == m_curState;
	}

	//void chgName(const char* str);

	bool isHero(int soldierid) const;

	int getHeroLastTime();

	void countLastTimeString(std::string& str, int lasttime);
    
    void initShopData();
        
    int showNums(int shopid);

	int addUpgradeSoldier(int soldierid, int& need);

	//! 等级差是敌人-自己，基地等级
	//! 奖杯差也是敌人-自己
	int countWarCup(int star, int offlevel, int offcup);

	//! 计算当前工人房价格
	int countCurWorkerBuildingPrice();

	//! 判断当前是否可以继续建造墙
	bool canBuildingWall();
	//! 建筑完成后调用
	void onBuildingComplete(Building& building);
	// 刷新实验室
	void onRefurbishLabMS(int ot, Building& building);
	//! 增加一个士兵升级
	void addSoldierUpgradeInfo(int soldierid, int lasttime);
	//! 获得当前升级的士兵
	int getCurUpgradeSoldier();
	// 士兵升级成功
	void onSoldierUpgrade(int soldierid);
	// 加速升级
	int fastSoldierUpgrade();
	// 判断当前正在升级的士兵数量
	int countSoldierUpgradeNums();
	// 刷新自己的士兵等级
	void onMyRefurbishSoldierLevel();
	// 改变自己的士兵等级
	void chgMySoldierLevel(int soldierid);
	// 获得自己某个士兵的当前等级
	int getMySoldierLevel(int soldierid);
    // 获得某个士兵最大的等级
    int getSoldierMaxLevel(int index);
	//// 关闭实验室界面
	//void onLabClose();

	//! 战斗结束，统计成就
	void onAchievement_WarEnd(bool isWin, int wall, int townhall, int worket, int mortar, int gold, int oil, int cup);
	//! 移除杂物成功，统计成就
	void onAchievement_RemoveOtherOK();
	//! 建造成功，统计成就
	void onAchievement_BuildingComplete(int buildingid);
	//! 移除杂物成功
	void onRemoveOtherOK(int lx, int ly);
	//! 移除装饰物成功
	void onRemoveOrnamentOK(int lx, int ly);

    const char* getAchievementString(int iAchievement);
    
	void requireAchievementAward(int iAchievement);

	int getAchievementVal(int iAchievement);

	int getAchievementStar(int iAchievement);

	void UpgradeAchievementStar(int iAchievement);
    
    bool canRequireAchievementAward(int iAchievement);
    
    float getAchievementPro(int iAchievement);

	const AchievementNode* getCurAchievementNode(int iAchievement);
    
//    std::string getMacAddress();
//    void getMacAddress(char* macAddressString);
    
    void onChangeScene();
    
	void onPause();  //程序进入后台回调
    
	void onResume(); //程序回到前台回调
    
    void onExit();   //程序终止回调
    
	bool isGoldFull(int curGold);

	bool isOilFull(int curOil);

	bool isEmptyBarrack(int barrackid);

	bool isFullTroophousing();
    
    int getFindGold();
    
    bool isTownHallComplete();
    
    int getSoldierlevel(int i);
    int getBuildMaxLevel(int bid);
    int getBuildNumsByBid(int bid);

	// 初始化内购数据
	void initInPurchase();
	//! 添加商品
	void addInPurchaseProductInfo(int productID, int gem, const char* strPlatformProductID, const char* strPrice);
	// 开始内购，如果发现前面一个内购未结束，则返回失败
	bool InPurchase(int productID);
	// 内购完成
	void onInPurchaseComplete(std::string strPlatformProductID, bool isSuccess, std::string result);
	// 内购完成
	void onInPurchaseCompleteImp(std::string strPlatformProductID, bool isSuccess, std::string result);
    
    void onInPurchaseVerifyComplete(int procuctId, int status);

    int protectTimeGem(int day);
    
	bool payProtectTime(int day);

	bool canUpgradeLaboratry(Building& building);

	void chg2Replay(FightMsg msg, int attackType);

	void onAchievementChg();

	int countAchievementNums();

	void setClearTouchFunc(CCObject* pTarget_ClearTouch, SEL_CallFunc pFuncClearTouch);

	int countSoldierNums();
    
    int countMilitaryRank();

	void onProcAsyncView();
    

    void checkConfig(const string key, int &config, const Json::Value cfgVal);
    
    //! 请求Google+宝石
    void requestGooglePlusGem();
	//! 请求评价宝石
	void requestGradeGem();
	//! 请求首充宝石
	void requestFirstPayGem();
    //!请求广告宝石
    void requestAdsGem(int value);
    void requestChartboostFlag();
    void requestChartboostClickGems();
	void onGetDeviceUserName(const char* username, const char* nickname);
    
	//// 复仇
	//void revenge(int msgid);
	// 兵营是否能募兵
	bool canRecruitSoldier(int soldierid, bool isOther);
	//! 添加商品
	void updInPurchaseProductInfo(int productID, int gem, const char* strPrice);

	int getIAPNums() { return m_lstIAPIndex.size(); }

	const InPurchaseProductInfo* getIAPInfo(int index);
    
    const InPurchaseProductInfo* getIAPInfoFromProductID(int index);

	void chgGuideVal(int val);
	//void chg2Visit(int uid);

	void selectHero(int heroid);
    
	void checkUserCenterBind();

	void rebindGameCenter(bool isChg);

	bool isNeedDestroyBuidling(int buildingid);

	bool isCanDestroyBuidling(int buildingid);

	//! 修正仓库的储量
	void fixStorageReserve(bool self);
    
	//! 防守士兵死亡
	void deadDefenceSoldier(int pid);
    
    void addTestLog(const char *s)
    {
        if (NETWORK_TIME_DEBUG) {
            
            string text = CCStringFormat("%s -> %d \n", s, time(NULL) - m_debugLogTime);
            m_strDebugLog = m_strDebugLog + text;
        }
    }
    
#ifdef COC_MODE_EDIT
	void load();

	void save();

	void addBuildingEx(int bid, int x, int y);
#endif // COC_MODE_EDIT
public:
	UserInfo						m_myInfo;
    
	std::list<UserBuildingInfo*>	m_lstBuilding;

	int								m_localUID;
	std::string						m_localUName;
    int                             m_localGuide;
	//CCUserDefault					m_localUserInfo;

	time_t							m_serverTime[2];
	time_t							m_curTime[3];
	int								m_offServiceTime;

	UserInfo						m_otherInfo;
	std::list<UserBuildingInfo*>	m_otherBuilding;

	MapBarrackInfo					m_mapBarrack;
	MapTroophousingInfo				m_mapTroophousing;
	MapTroophousingInfo				m_mapOtherTroophousing;
    
	ListNotifyListener				m_lstNotifyListener;

	bool							m_isProcListener;
	bool							m_isInitOK;
	bool							m_isPlayerSceneInitOK;
    bool                            m_isGameShow;

    int                             m_revengeBattleId;
    int                             m_revengeUserId;
    
    int                             m_fightReplayType;
    
	std::list<FightSoldierInfoEx>	m_lstFightSoldier;
	int								m_lastFightSoldierNums;
    
	std::list<FightSoldierInfoEx>	m_lstOtherFightSoldier;

    FightMsg                        m_logFightMsg;
	FightScoreInfoEx3				m_infoFightScore;
	FightLog						m_logFight;
	long long						m_fightBeginTimeMS;
	int								m_curWarStar;
    bool                            m_progHalfWar;
    
    int                             m_fightMode;

	std::vector<FightMsg>			m_vecDefenseLog;
    std::vector<FightMsg>           m_vecAttackLog;
    std::vector<MailInfo>           m_vecMail;
    
    std::vector<int>                m_vecAchievement;
    
    
	int								m_curAttackMsgID;
    bool                            m_isAttackLogLoad;

	std::list<FightSoldierInfoEx>	m_lstUsedFightSoldier;
    
    int                             m_clanSoldierUsed;

	//std::map<int, Townhall_BuildingNums>	m_mapTownhall;
	Townhall_BuildingNums					m_curTownhall;
    
	int								m_barrackMoney[3];

	bool							m_isFinishAttack;

    bool                            m_isPause;
	bool							m_isGoHome;

	int                             m_pMsg;
    int                             m_pGetGemType;      //0:升级 1:新建 2:新查找
    int                             m_pGetGemNums;
    bool                            m_isErrorMsg;
    bool                            m_isExitMsg;

	bool							m_isReplayMode;

	Achievement						m_achievement;

	int								m_maxSoldier;
	int								m_curSoldier;

	int								m_curState;

	float							m_loadingProgress;
    
    int                             m_showinfo_soldierid;

	std::vector<RankUserInfo>		m_lstCupRank;

	bool							m_bChgScene2Home;

	//HeroInfo						m_myHeroInfo;

	int								m_guideBuildingID;
    
    std::vector<int>				m_shoplst[6];

	SoldierUpgradeInfo				m_soldierupgrade;

	int								m_myLabLevel;

	int								m_myBaseLevel;
	int								m_otherBaseLevel;
    
    int                             m_totalAttackNums;

	WarParam						m_warParam;

	std::string						m_key0;
	std::string						m_key1;
	std::string						m_key2;
    
    std::string                     m_webURL;

	std::string						m_strLoadingInfo;
    std::string                     m_strDebugLog;
    time_t                          m_debugLogTime;
	int								m_maxUpdateNums;			// 最大的需要启动时上传的消息数量
    
    bool                            m_bShowShopTips1;
    bool                            m_bShowShopTips2;
    bool                            m_bShowShopTips3;
    bool                            m_bClickedMoreTips;

	std::map<int, InPurchaseProductInfo>	m_lstInPurchaseProductInfo;
	int										m_curInPurchaseProductID;		// 如果>0，表示当前正在处理的productid，小于0才表示可以内购

    int                             m_fightUseSkillGems;
    
	int								m_myMsgNums;				// 自己的消息数量

	time_t							m_pauseBeginTime;			// 暂停时间

	SEL_CallFunc					m_pFunc_ClearTouch;
	CCObject*						m_pTarget_ClearTouch;

	//bool							m_isWarEnd;					// 战斗结束

    map<int, int>                   m_asyncViewList;            //处理平台异步刷新UI
    
    int                             m_freeBonusGems;            //第一次购买获得奖励宝石

	bool							m_isAttackDropPerson;		// 是否已经放兵了，战斗

	time_t							m_beginTime;				// 进入游戏时的时间

	time_t                          m_lastLoginTime;			// 上一次登陆的时间
    
    time_t                          m_trashLastGrowTime;        //上次生长杂物时间

	std::string						m_defaultUserName;			// 默认名字
    
    bool                            m_bShowRaidNotify;

	bool							m_isVerOK;					// 版本正确

	std::vector<int>				m_lstIAPIndex;				// 有效的内购索引队列
    int                             m_shopid;

	int								m_zyval;					// 指引的值

	int								m_pauseTime;

	std::string						m_uidGameCenter;
	bool							m_isLogin;
	int								m_uidGameCenterBind;
    
	std::string						m_unameGameCenterBind;		// gamecenter绑定的帐号
	int								m_townlevelGameCenterBind;	// gamecenter绑定的帐号的基地等级
	int								m_userlevelGameCenterBind;	// gamecenter绑定的帐号的玩家等级

	bool							m_willPause;

	bool							m_isProcKeepLife;

	bool							m_isCheckUserBind;

	int								m_fightWinCups[3];
	int								m_fightLoseCups;
    
    int                             m_winCups;

	int								m_curBuildingIndex;
	int								m_curPersonAtk;
	int								m_curPersonDef;
    
    int                             m_signinDays;
    
    AttackType                      m_attackType;
    
    PVEInfo                         m_pveInfo;
    
    bool                            m_bEnvMapInit;
    
    bool                            m_isFreeGemOn;
    
    IapGiftInfo                     m_iapGiftInfo;
    
    int                             m_iAdsOpen;
    
    int                             m_isReviewing;
    
    int                             m_isChartboostAdOn;
    
    int                             m_isAarkiOn;
    
    int                             m_isFlurryAdOn;
    
    int                             m_isTapjoyAdOn;
    
    int                             m_isSupersonicAdsOn;
    
    int                             m_isNativeXAdOn;
    
    int                             m_pendingNativeXGem;
    
    int                             m_chartboostClickGem;
    bool                            m_isChartboostFreeGemReady;
    bool                            m_hasChartboostFreeGem;
    int                             m_isShowFreeGemInBonus;
    
    int                             m_lastBeAttackTime;
    int                             m_waitBeAttackTime;
    int                             m_payType;
    
    int                             m_announcementType;
    
    int                             m_addGemType;
    
    int                             m_defaultAdsType;
    
    int                             m_defaultFullAdsType;
    
    int                             m_bindType;
    
    
    bool                            m_webUrlTip;
    
    int                             m_isSigninDays;
};

NS_COC_END

#endif  // __COC_HTTPCLIENT_H__
