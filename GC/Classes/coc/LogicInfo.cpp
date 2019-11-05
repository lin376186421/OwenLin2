#include "LogicInfo.h"
//#include "curl.h"
#include "HttpClient.h"
#include "BuildingMgr.h"
#include "PlayerScene.h"
#include "PersonMgr.h"
#include "BuildingMgr.h"
#include "TownhallLevelInfo.h"
#include "SimpleAudioEngine.h"
#include "../base32.h"
#include "SoundMgr.h"
#include "../form/MessageUI.h"
#include "ExperienceLevels.h"
#include "AchievementInfo.h"
#include "math.h"
#include "GuideMgr.h"
#include "HeroMgr.h"
#include "../HelloWorldScene.h"
#include "../GameWorldScene.h"
#include "../Framework/CAppPlatform.h"
#include "../Framework/InPurchase.h"
#include "../Framework/CLocalManager.h"
#include "../Utils/Base64.h"
#include "RandMgr.h"
#include "SkillMgr.h"
#include "GuideMgr.h"
#include "ClanMgr.h"
#include "ClanWarMgr.h"
#include "GroupWarMgr.h"
#include "CSVMgr.h"
#include "TownhallLevelInfo.h"
#include "ConfigValMgr.h"
#include "zlib.h"
#include "EnvMapMgr.h"
#include "SocketClient.h"
#include "LeagueMgr.h"
#include "MilitaryRankMgr.h"
#include "../form/MainUI.h"

NS_COC_BEGIN

LogicInfo::LogicInfo()
: m_isPlayerSceneInitOK(false)
, m_isPause(false)
, m_isGoHome(false)
, m_isReplayMode(false)
, m_maxSoldier(0)
, m_curSoldier(0)
, m_curState(LOGICSTATE_WAIT)
, m_bChgScene2Home(false)
, m_guideBuildingID(0)
, m_myLabLevel(0)
, m_fightBeginTimeMS(0)
, m_fightMode(FIGHT_MODE_WAIT)
, m_maxUpdateNums(-1)
, m_curInPurchaseProductID(-1)
, m_myMsgNums(0)
, m_pauseBeginTime(0)
, m_pFunc_ClearTouch(NULL)
, m_pTarget_ClearTouch(NULL)
, m_beginTime(0)
, m_bShowRaidNotify(true)
//, m_isWarEnd(false)
, m_isVerOK(false)
, m_offServiceTime(0)
, m_zyval(0)
, m_pauseTime(0)
, m_isLogin(false)
, m_willPause(false)
, m_isProcKeepLife(false)
, m_isCheckUserBind(false)
, m_revengeBattleId(-1)
, m_signinDays(-1)
, m_totalAttackNums(0)
, m_isErrorMsg(false)
, m_isExitMsg(false)
, m_bEnvMapInit(false)
, m_isAttackLogLoad(false)
, m_iAdsOpen(0)
, m_isReviewing(0)
, m_isChartboostAdOn(0)
, m_isAarkiOn(0)
, m_isFlurryAdOn(0)
, m_isTapjoyAdOn(0)
, m_isSupersonicAdsOn(0)
, m_isNativeXAdOn(0)
, m_chartboostClickGem(0)
, m_isChartboostFreeGemReady(false)
, m_clanSoldierUsed(0)
, m_hasChartboostFreeGem(false)
, m_isShowFreeGemInBonus(1)
, m_isFreeGemOn(true)
, m_lastBeAttackTime(0)
, m_waitBeAttackTime(0)
, m_payType(PAY_TYPE_GOOGLEPLAY)
, m_announcementType(ANNOUNCEMENT_TYPE_NO)
, m_addGemType(ADD_GEM_NORMAL)
, m_defaultAdsType(6)
, m_defaultFullAdsType(0)
, m_bindType(BIND_SYSTEM_TYPE)
, m_webUrlTip(false)
, m_pendingNativeXGem(0)
, m_isSigninDays(0)
{
	m_myInfo.sessionid = 0;
    
	memset(&m_soldierupgrade, 0, sizeof(m_soldierupgrade));
	memset(&m_achievement, 0, sizeof(m_achievement));
    
    m_defaultUserName = "";
    
	initInPurchase();
    
	TownhallLevelInfo::getSingleton();
	ExperienceLevels::getSingleton();
	AchievementInfo::getSingleton();
    MilitaryRankMgr::getSingleton();
    FacebookMgr::getSingleton();
    
	m_isProcListener = false;
    
	HttpClient::getSingleton().addListener(this);
    
	m_localUID = CCUserDefault::sharedUserDefault()->getIntegerForKey("uid");
	m_localUName = UrlDecode(CCUserDefault::sharedUserDefault()->getStringForKey("uname").c_str());
    
	m_loadingProgress = 0.0f;
    
//    m_webURL = WEB_URL;
    
    m_debugLogTime = time(NULL);
}

LogicInfo::~LogicInfo()
{
}

LogicInfo& LogicInfo::getSingleton()
{
	static LogicInfo s_info;
    
	return s_info;
}

UserBuildingInfo* LogicInfo::findBuilding(int x, int y)
{
	for(std::list<UserBuildingInfo*>::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if((*it)->x == x && (*it)->y == y)
			return *it;
	}
    
	return NULL;
}

bool LogicInfo::isTownHallComplete()
{
    for(std::list<UserBuildingInfo*>::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if((*it)->buildingid / 100 == 201)
        {
            if((*it)->state == 2)
            {
                return true;
            }
        }
	}
    return false;
}

void LogicInfo::checkConfig(const string key, int &config, const Json::Value cfgVal)
{
    string adKey(key+"_"+APP_ID+"_"+CAppPlatform::sharedAppPlatform()->getBundleVersion());
    if(cfgVal[adKey].type() != Json::nullValue)
    {
        int newConfig = getAsInt(cfgVal[adKey]);
        config = newConfig;
    }
    else
    {
        string adKey(key+"_"+APP_ID);
        if(cfgVal[adKey].type() != Json::nullValue)
        {
            int newConfig = getAsInt(cfgVal[adKey]);
            config = newConfig;
        }
        else
        {
            string adKey(key);
            if(cfgVal[adKey].type() != Json::nullValue)
            {
                int newConfig = getAsInt(cfgVal[adKey]);
                config = newConfig;
            }
        }
    }
}

bool LogicInfo::onProc(const Json::Value& val)
{
    std::string cid = val["cid"].asString();
    
	if(val["cid"].asString() == "initok")
	{
        bool initScene = true;
        if(HTTPCLIENT_STATE_PROCLAST == HttpClient::getSingleton().m_curClientState || !PlayerScene::getSingleton().m_isAlreadyInit) {
			initScene = false;
        }
        
		m_isInitOK = true;
        
		m_localUID = m_myInfo.uid;
		m_localUName = m_myInfo.uname;
        
		CCUserDefault::sharedUserDefault()->setIntegerForKey("uid", m_localUID);
		CCUserDefault::sharedUserDefault()->setStringForKey("uname", UrlEncode(m_localUName.c_str()));
        
		char userid[128];
		sprintf(userid, "%s[%d]", m_localUName.c_str(), m_localUID);
        CCUserDefault::sharedUserDefault()->flush();//这里一定要提交写入哦，否则不会记录到xml中，下次启动游戏你就获取不到value了。
        
		onRefurbishAllBarrack();
		onRefurbishAllTroophousing();
		refurbishBaseInfo();
        
		refurbishSoldierNums();
		onRefurbishTownhall();
        
        if(initScene) {
			PlayerScene::getSingleton().initScene();
            HttpClient::getSingleton().loginok();
        }
        
		m_isGoHome = false;
        
//        HttpClient::getSingleton().requestSysNotify();
        HttpClient::getSingleton().requestAchievement();
        HttpClient::getSingleton().requestDefenseLog();
        HttpClient::getSingleton().requestMailInfo();
        HttpClient::getSingleton().requestLeagueInfo();
        HttpClient::getSingleton().requestClanWarInfo();
        
        m_attackType = ATTACK_TYPE_NULL;
		m_isProcKeepLife = true;
        
		SocketClient::getSingleton().onNeedLogin();
                
        InPurchase::sharedInPurchase()->resumePurchases(this, purchase_callback(LogicInfo::onInPurchaseComplete));
	}
	else if(val["cid"].asString() == "userinfo")
	{
		m_curBuildingIndex = 0;
		m_curPersonAtk = 0;
		m_curPersonDef = 0;
        
		m_myBaseLevel = 0;
        
		m_curState = LOGICSTATE_NORMAL;
        
		m_myInfo.uid = getAsInt(val["uid"]);
		m_myInfo.sessionid = getAsInt(val["sessionid"]);
        
        int64_t serverTime = getAsInt(val["curtime"]);
        setCurrentTime(serverTime*1000);
        
		CCLOG("sessionid = %d", m_myInfo.sessionid);
        
        m_totalAttackNums = getAsInt(val["atknums"]);
        
		m_myInfo.curlevel = getAsInt(val["curlevel"]);
		m_myInfo.exp = getAsInt(val["exp"]);
		m_myInfo.cupex = getAsInt(val["cup"]);
        
		m_myInfo.goldex = getAsInt(val["gold"]);
		m_myInfo.oilex = getAsInt(val["oil"]);
        m_myInfo.maxgoldex = getAsInt(val["maxgold"]);
        m_myInfo.maxoilex = getAsInt(val["maxoil"]);
        
		m_myInfo.gemex = getAsInt(val["gem"]);
        
        m_myInfo.military = getAsInt(val["mrank"]);
        
        m_myInfo.medal = getAsInt(val["medal"]);
        
		m_myInfo.vip = getAsInt(val["vip"]);
        
		m_myMsgNums = getAsInt(val["msgnums"]);
        
		m_zyval = getAsInt(val["zyvalue"]);
        
		m_lastLoginTime = getAsInt(val["lastlogintime_ms"]);
        
        m_trashLastGrowTime = getAsInt(val["plantgrowthtime"]);
        
        m_lastBeAttackTime = getAsInt(val["lastbeattacktime"]);
        
		m_myInfo.uname = UrlDecode(val["uname"].asString().c_str());
		m_myInfo.handle = val["handle"].asString();
        
		m_myInfo.buildbuildingnumsex = 0;
		m_myInfo.totalworkernumsex = 0;
		m_myInfo.maxBarrackLevel = 0;
        
		m_myInfo.protectTime = getAsInt(val["prottime"]);
		
		m_myInfo.lastProtectTime = m_myInfo.protectTime - getCurrentTime();// time(NULL);
		if(m_myInfo.lastProtectTime <= 0)
			m_myInfo.lastProtectTime = 0;
        
		m_myInfo.mask[0] = getAsInt(val["mask1"]);
		m_myInfo.mask[1] = getAsInt(val["mask2"]);
        
		m_serverTime[1] = m_serverTime[0] = getAsInt(val["curtime"]);
		m_curTime[2] = m_curTime[1] = m_curTime[0] = getCurrentTime();// time(NULL);
		m_offServiceTime = m_serverTime[0] - m_curTime[0];
        
        time_t regtime = getAsInt(val["regtime"]);
        
        if(CCUserDefault::sharedUserDefault()->getBoolForKey("FB_LOGIN_TIP") == false)
        {
            if(getCurrentTime()-regtime >= (3*24*60*60))
            {
                FacebookMgr::getSingleton().m_isTipLogin = true;
            }
        }
        string fbid = val["fbid"].asString();
        if(fbid.length() <= 1)
        {
        	FacebookMgr::getSingleton().m_isGetFreeGemLogin = true;
        }
        else
        {
        	FacebookMgr::getSingleton().m_isGetFreeGemLogin = false;
        }
		clearMyScene();
        
		m_mapBarrack.clear();
		m_mapTroophousing.clear();
        m_soldierupgrade.reset();
        
		m_isInitOK = false;
        
        LogicInfo::getSingleton().addTestLog("login end");
	}
	else if(val["cid"].asString() == "building")
	{
		if(m_isInitOK)
			return true;
        
		UserBuildingInfo* pbi = new UserBuildingInfo;
        pbi->id = getAsInt(val["id"]);
		pbi->buildingid = getAsInt(val["bid"]);
        pbi->level = getAsInt(val["level"]);
		pbi->state = getAsInt(val["state"]);
        pbi->hp = getAsInt(val["hp"]);
		pbi->lasttime = getAsInt(val["lasttime"]);
		pbi->curtime = getAsInt(val["curtime"]);
		pbi->x = getAsInt(val["x"]) - 1;
		pbi->y = getAsInt(val["y"]) - 1;
        
		pbi->currestime = getAsInt(val["currestime"]);
		pbi->timeres = getAsInt(val["timeres"]);
        
		pbi->store[0] = getAsInt(val["storegold"]);
		pbi->store[1] = getAsInt(val["storeoil"]);
        
		int bid = pbi->buildingid / 100;
		if(bid == BUILDING_TOWN_HALL)
		{
			m_myBaseLevel = pbi->buildingid % 100;
            if (pbi->state == BUILDING_STATE_UPGRADING) { //升级中临时减1
                m_myBaseLevel -= 1;
            }
		}
        
		m_lstBuilding.push_back(pbi);
        
		onAddBuilding(*pbi);
	}
    else if(val["cid"].asString() == "otherbuilding")
	{
		UserBuildingInfo* pbi = new UserBuildingInfo;
        pbi->id = getAsInt(val["id"]);
		pbi->buildingid = getAsInt(val["bid"]);
        pbi->level = getAsInt(val["level"]);
		pbi->state = getAsInt(val["state"]);
        pbi->hp = getAsInt(val["hp"]);
		pbi->lasttime = 0;
		pbi->curtime = 0;
		pbi->x = getAsInt(val["x"]) - 1;
		pbi->y = getAsInt(val["y"]) - 1;
        
		pbi->currestime = getAsInt(val["currestime"]);
		pbi->timeres = getAsInt(val["timeres"]);
        
		pbi->store[0] = getAsInt(val["storegold"]);
		pbi->store[1] = getAsInt(val["storeoil"]);
        
		int bid = pbi->buildingid / 100;
		if(bid == BUILDING_TOWN_HALL)
		{
			m_otherBaseLevel = pbi->buildingid % 100;
		}
        
		m_otherBuilding.push_back(pbi);
		onAddOtherBuilding(*pbi);
                
		PlayerScene::getSingleton().onUpdateBuilding(pbi);
	}
	else if(val["cid"].asString() == "otheruser")
	{
		m_curBuildingIndex = 0;
		m_curPersonAtk = 0;
		m_curPersonDef = 0;
        
		m_otherBaseLevel = 0;
        
		m_otherInfo.uid = getAsInt(val["uid"]);
        
		m_otherInfo.curlevel = getAsInt(val["curlevel"]);
		m_otherInfo.exp = getAsInt(val["exp"]);
		m_otherInfo.cupex = getAsInt(val["cup"]);
        
		m_otherInfo.goldex = getAsInt(val["gold"]);
		m_otherInfo.oilex = getAsInt(val["oil"]);
		m_otherInfo.gemex = getAsInt(val["gem"]);
        
		m_otherInfo.maxgoldex = 0;
		m_otherInfo.maxoilex = 0;
		m_otherInfo.maxBarrackLevel = 0;
        
		m_otherInfo.uname = UrlDecode(val["uname"].asString().c_str());
		m_otherInfo.handle = val["handle"].asString();
        
        m_otherInfo.military = getAsInt(val["mrank"]);
        m_otherInfo.medal = getAsInt(val["medal"]);
        
		clearOtherScene();
        m_infoFightScore.reset();
        
        LogicInfo::getSingleton().addTestLog("find user ok");
	}
	else if(val["cid"].asString() == "otheruserok")
	{
        if (!m_bChgScene2Home) {
            m_curState = LOGICSTATE_ATTACK;
            
//            m_otherInfo.goldex = m_otherInfo.maxgoldex;
//            m_otherInfo.oilex = m_otherInfo.maxoilex;
            
            PlayerScene::getSingleton().onInitOther();
            
            onNotify(NOTIFY_NEWFIND);
        }
	}
    else if(val["cid"].asString() == "leagueuserinfook")
	{
        if (!m_bChgScene2Home) {
            m_curState = LOGICSTATE_ATTACK;
            
            m_otherInfo.goldex = 0;
            m_otherInfo.oilex = 0;
            
            PlayerScene::getSingleton().onInitOther();
            
            onNotify(NOTIFY_NEWFIND);
        }
	}
    else if (val["cid"].asString() == "clanwaruserok")
    {
        if (!m_bChgScene2Home) {
            m_curState = LOGICSTATE_ATTACK;
            
            PlayerScene::getSingleton().onInitOther();
            
            onNotify(NOTIFY_NEWFIND);
        }
    }
    else if (val["cid"].asString() == "groupwaruserok")
    {
        if (!m_bChgScene2Home) {
            m_curState = LOGICSTATE_ATTACK;
            
            PlayerScene::getSingleton().onInitOther();
            
            onNotify(NOTIFY_NEWFIND);
        }
    }
	else if(val["cid"].asString() == "visitotherok")
	{
        if (!m_bChgScene2Home) {
            m_curState = LOGICSTATE_VIEW;
            
            m_otherInfo.goldex = m_otherInfo.maxgoldex;
            m_otherInfo.oilex = m_otherInfo.maxoilex;
            
            PlayerScene::getSingleton().onInitOther();
            
            onNotify(NOTIFY_NEWFIND);
        }
	}
	else if(val["cid"].asString() == "barrackinfo")
	{
		if(HttpClient::getSingleton().m_curClientState == HTTPCLIENT_STATE_PROCLAST)
			return true;
        
		int barrackid = getAsInt(val["barrackid"]);
        
		MapBarrackInfo::iterator it = m_mapBarrack.find(barrackid);
		if(it != m_mapBarrack.end())
		{
			it->second.curtime = getAsInt(val["oldtime"]);//getAsInt(val["curtime"]);// time(NULL);
			it->second.lasttime = getAsInt(val["lasttime"]);
            it->second.isPause = false;
            
			it->second.lstSoldier.clear();
            
			int nums = getAsInt(val["nums"]);
			for(int i = 0; i < nums; ++i)
			{
				BarrackSoldierInfo bsi;
                
				char name[128];
                
				sprintf(name, "pid%d", i);
				bsi.pid = getAsInt(val[name]);
                
				sprintf(name, "num%d", i);
				bsi.numsex = getAsInt(val[name]);
                
				CCLOG("barrack %d %d %d", barrackid, bsi.pid, (int)bsi.numsex);
                
				if(bsi.numsex <= 0)
					continue;
                
				const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(bsi.pid);
				if(pInfo == NULL)
					return true;
                
				bsi.needtimeex = pInfo->trainingTime;
                
				it->second.lstSoldier.push_back(bsi);
			}
            
			PlayerScene::getSingleton().onUpdateBarrack(it->second);
		}
	}
	else if(val["cid"].asString() == "troophousinginfo")
	{
		if(HttpClient::getSingleton().m_curClientState == HTTPCLIENT_STATE_PROCLAST)
			return true;
        
		int troophousingid = getAsInt(val["troophousingid"]);
        
		MapTroophousingInfo::iterator it = m_mapTroophousing.find(troophousingid);
		if(it != m_mapTroophousing.end())
		{
			it->second.lstSoldier.clear();
            
			int curnums = 0;
			int nums = getAsInt(val["nums"]);
			for(int i = 0; i < nums; ++i)
			{
				TroophousingSoldierInfo bsi;
                
				char name[128];
                
				sprintf(name, "pid%d", i);
				bsi.pid = getAsInt(val[name]);
                
				sprintf(name, "num%d", i);
				bsi.numsex = getAsInt(val[name]);
                
				CCLOG("troophousingid %d %d %d", troophousingid, bsi.pid, (int)bsi.numsex);
                
				if(bsi.numsex <= 0)
					continue;
                
				curnums += bsi.numsex;
                
				const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(bsi.pid);
				if(pInfo == NULL)
					return true;
                
				bsi.spaceex = pInfo->housingSpaceex;
                
				it->second.lstSoldier.push_back(bsi);
			}
		}
	}
	else if(val["cid"].asString() == "othertroophousinginfo")
	{
		if(m_isInitOK)
			return true;
        
		int troophousingid = getAsInt(val["troophousingid"]);
        
		MapTroophousingInfo::iterator it = m_mapOtherTroophousing.find(troophousingid);
		if(it != m_mapOtherTroophousing.end())
		{            
			it->second.lstSoldier.clear();
            
			int curnums = 0;
			int nums = getAsInt(val["nums"]);
			for(int i = 0; i < nums; ++i)
			{
				TroophousingSoldierInfo bsi;
                
				char name[128];
                
				sprintf(name, "pid%d", i);
				bsi.pid = getAsInt(val[name]);
                
				sprintf(name, "num%d", i);
				bsi.numsex = getAsInt(val[name]);
                
				if(bsi.numsex <= 0)
					continue;
                
				curnums += bsi.numsex;
                
				const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(bsi.pid);
				if(pInfo == NULL)
					return true;
                
				bsi.spaceex = pInfo->housingSpaceex;
                
				it->second.lstSoldier.push_back(bsi);
			}
		}
	}
	else if(val["cid"].asString() == "defenselog")
	{
        m_vecDefenseLog.clear();
        
        Json::Value logList = val["lst"];
        
        int size = logList.size();
        
        for (int i = 0; i < size; i ++) {
        
            Json::Value logInfo = logList[i];
            
            FightMsg msg;
            
            memset(&msg.fsi, 0, sizeof(msg.fsi));
            
            msg.msgid = getAsInt(logInfo["logid"]);
            if(msg.msgid > 0)
            {
                msg.star = getAsInt(logInfo["star"]);
                msg.attackCup = getAsInt(logInfo["atkcup"]);
                msg.defendCup = getAsInt(logInfo["defcup"]);
                msg.time = getAsInt(logInfo["battletime"]);
                msg.isAlreadyAttack = getAsInt(logInfo["isattack"]);
                msg.clientver = getAsInt(logInfo["clientver"]);
                
                unsigned char* pDest = (unsigned char*)malloc(sizeof(msg.fsi) * 2);
                unsigned char* pDest1 = (unsigned char*)malloc(sizeof(msg.fsi) * 2);
                
                if (msg.clientver < COC_ATTACK_VER_FIX) {
                    std::string str = logInfo["result"].asString();
                    
                    FightScoreInfo fsi;
                    
                    unsigned long destLen = DecodeLengthBase32(str.length());
                    DecodeBase32(str.c_str(), sizeof(fsi) * 2, pDest);
                    
                    unsigned long destLen1 = sizeof(fsi) * 2;
                    uncompress(pDest1, &destLen1, pDest, destLen);
                    
                    memcpy(&fsi, pDest1, sizeof(fsi));
                    
                    msg.fsi = fsi;
                }
                else if (msg.clientver < COC_ATTACK_VER_FIX2) {
                    std::string str = logInfo["result2"].asString();
                    
                    FightScoreInfoEx fsi;
                    
                    unsigned long destLen = DecodeLengthBase32(str.length());
                    DecodeBase32(str.c_str(), sizeof(fsi) * 2, pDest);
                    
                    unsigned long destLen1 = sizeof(fsi) * 2;
                    uncompress(pDest1, &destLen1, pDest, destLen);
                    memcpy(&fsi, pDest1, sizeof(fsi));
                    
                    msg.fsi = fsi;
                }
                else if (msg.clientver < COC_ATTACK_VER_FIX3) {
                    std::string str = logInfo["result2"].asString();
                    
                    FightScoreInfoEx2 fsi;
                    
                    unsigned long destLen = DecodeLengthBase32(str.length());
                    DecodeBase32(str.c_str(), sizeof(fsi) * 2, pDest);
                    
                    unsigned long destLen1 = sizeof(fsi) * 2;
                    uncompress(pDest1, &destLen1, pDest, destLen);
                    memcpy(&fsi, pDest1, sizeof(fsi));
                    
                    msg.fsi = fsi;
                }
                else if (msg.clientver < COC_ATTACK_VER_FIX4) {
                    
                    std::string str = logInfo["result"].asString();
                    
                    unsigned long destLen = DecodeLengthBase32(str.length());
                    DecodeBase32(str.c_str(), sizeof(msg.fsi) * 2, pDest);
                    
                    unsigned long destLen1 = sizeof(msg.fsi) * 2;
                    uncompress(pDest1, &destLen1, pDest, destLen);
                    memcpy(&msg.fsi, pDest1, sizeof(msg.fsi));
                }
                else if (msg.clientver < COC_ATTACK_VER_FIX5) {
                    
                    std::string str = logInfo["result"].asString();
                    
                    unsigned long destLen = 0;
                    Base64::Decode((unsigned char*)str.c_str(), strlen(str.c_str()), pDest, destLen);
                    
                    unsigned long destLen1 = sizeof(msg.fsi) * 2;
                    uncompress(pDest1, &destLen1, (unsigned char*)pDest, destLen);
                    memcpy(&msg.fsi, pDest1, sizeof(msg.fsi));
                }
                else {
                    std::string str = logInfo["result2"].asString();
                    
                    unsigned long destLen = 0;
                    Base64::Decode((unsigned char*)str.c_str(), strlen(str.c_str()), pDest, destLen);
                    
                    unsigned long destLen1 = sizeof(msg.fsi) * 2;
                    uncompress(pDest1, &destLen1, (unsigned char*)pDest, destLen);
                    memcpy(&msg.fsi, pDest1, sizeof(msg.fsi));
                }
                
                free(pDest);
                free(pDest1);
                
                if(msg.fsi.user[0].uid == 0 || msg.fsi.user[1].uid == 0)
                    continue;
                
                m_vecDefenseLog.push_back(msg);
                m_curAttackMsgID = msg.msgid;
            }
        }
        
        if (PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pMainUI) {
            PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->countMailNums(-1);
        }
        
		// 战斗日志收完了
        onNotify(NOTIFY_MAIL_BATTLELOG);
	}
    else if(val["cid"].asString() == "attacklog")
    {
        m_vecAttackLog.clear();
        
        Json::Value logList = val["lst"];
        
        int size = logList.size();
        
        for (int i = 0; i < size; i ++) {
            
            Json::Value logInfo = logList[i];
            
            FightMsg msg;
            
            memset(&msg.fsi, 0, sizeof(msg.fsi));
            
            msg.msgid = getAsInt(logInfo["logid"]);
            if(msg.msgid > 0)
            {
                msg.star = getAsInt(logInfo["star"]);
                msg.attackCup = getAsInt(logInfo["atkcup"]);
                msg.defendCup = getAsInt(logInfo["defcup"]);
                msg.time = getAsInt(logInfo["battletime"]);
                msg.isAlreadyAttack = getAsInt(logInfo["isattack"]);
                msg.clientver = getAsInt(logInfo["clientver"]);
                
                unsigned char* pDest = (unsigned char*)malloc(sizeof(msg.fsi) * 2);
                unsigned char* pDest1 = (unsigned char*)malloc(sizeof(msg.fsi) * 2);
                
                if (msg.clientver < COC_ATTACK_VER_FIX) {
                    std::string str = logInfo["result"].asString();
                    
                    FightScoreInfo fsi;
                    
                    unsigned long destLen = DecodeLengthBase32(str.length());
                    DecodeBase32(str.c_str(), sizeof(fsi) * 2, pDest);
                    
                    unsigned long destLen1 = sizeof(fsi) * 2;
                    uncompress(pDest1, &destLen1, pDest, destLen);
                    memcpy(&fsi, pDest1, sizeof(fsi));
                    
                    msg.fsi = fsi;
                }
                else if (msg.clientver < COC_ATTACK_VER_FIX2) {
                    std::string str = logInfo["result2"].asString();
                    
                    FightScoreInfoEx fsi;
                    
                    unsigned long destLen = DecodeLengthBase32(str.length());
                    DecodeBase32(str.c_str(), sizeof(fsi) * 2, pDest);
                    
                    unsigned long destLen1 = sizeof(fsi) * 2;
                    uncompress(pDest1, &destLen1, pDest, destLen);
                    memcpy(&fsi, pDest1, sizeof(fsi));
                    
                    msg.fsi = fsi;
                }
                else if (msg.clientver < COC_ATTACK_VER_FIX3) {
                    std::string str = logInfo["result2"].asString();
                    
                    FightScoreInfoEx2 fsi;
                    
                    unsigned long destLen = DecodeLengthBase32(str.length());
                    DecodeBase32(str.c_str(), sizeof(fsi) * 2, pDest);
                    
                    unsigned long destLen1 = sizeof(fsi) * 2;
                    uncompress(pDest1, &destLen1, pDest, destLen);
                    memcpy(&fsi, pDest1, sizeof(fsi));
                    
                    msg.fsi = fsi;
                }
                else if (msg.clientver < COC_ATTACK_VER_FIX4) {
                    
                    std::string str = logInfo["result"].asString();
                    
                    unsigned long destLen = DecodeLengthBase32(str.length());
                    DecodeBase32(str.c_str(), sizeof(msg.fsi) * 2, pDest);
                    
                    unsigned long destLen1 = sizeof(msg.fsi) * 2;
                    uncompress(pDest1, &destLen1, pDest, destLen);
                    memcpy(&msg.fsi, pDest1, sizeof(msg.fsi));
                }
                else if (msg.clientver < COC_ATTACK_VER_FIX5) {
                    
                    std::string str = logInfo["result"].asString();
                    
                    unsigned long destLen = 0;
                    Base64::Decode((unsigned char*)str.c_str(), strlen(str.c_str()), pDest, destLen);
                    
                    unsigned long destLen1 = sizeof(msg.fsi) * 2;
                    uncompress(pDest1, &destLen1, (unsigned char*)pDest, destLen);
                    memcpy(&msg.fsi, pDest1, sizeof(msg.fsi));
                }
                else {
                    std::string str = logInfo["result2"].asString();
                    
                    unsigned long destLen = 0;
                    Base64::Decode((unsigned char*)str.c_str(), strlen(str.c_str()), pDest, destLen);
                    
                    unsigned long destLen1 = sizeof(msg.fsi) * 2;
                    uncompress(pDest1, &destLen1, (unsigned char*)pDest, destLen);
                    memcpy(&msg.fsi, pDest1, sizeof(msg.fsi));
                }
                
                free(pDest);
                free(pDest1);
                
                if(msg.fsi.user[0].uid == 0 || msg.fsi.user[1].uid == 0)
                    continue;
                
                if (m_isAttackLogLoad) {
                    if (m_vecAttackLog.size() > 0 && msg.time > m_vecAttackLog[0].time) { //new attack log
                        m_vecAttackLog.insert(m_vecAttackLog.begin(), msg);
                    }else{
                        m_vecAttackLog.push_back(msg);
                    }
                }
                
                m_curAttackMsgID = msg.msgid;
            }
        }

        onNotify(NOTIFY_MAIL_ATTACKLOG);
    }
	else if(val["cid"].asString() == "replay")
	{
		m_curBuildingIndex = 0;
		m_curPersonAtk = 0;
		m_curPersonDef = 0;
        
		m_curState = LOGICSTATE_REPLAY;
		//FightMsg msg;
        
		m_logFight.clear();
        
        string map = val["map"].asString();
        
        unsigned long dataLen = 0;
        unsigned long destLen = 0;
        
        unsigned char* pData = (unsigned char*)malloc(strlen(map.c_str())*2);
        Base64::Decode((unsigned char*)map.c_str(), strlen(map.c_str()), pData, dataLen);
        
        int buffsize = 4096;
        destLen = buffsize;
        unsigned char* mapData = (unsigned char*)malloc(buffsize);
        
        while (uncompress(mapData, &destLen, pData, dataLen) != Z_OK) {
            
            free(mapData);
            
            buffsize *= 2;
            
            mapData = (unsigned char*)malloc(buffsize);
            destLen = buffsize;
        }
        
        m_logFight.setMapData(mapData, destLen);
        
        free(pData);
        free(mapData);
        
		string detail = val["detail"].asString();
        
        dataLen = 0;
        destLen = 0;
        
        pData = (unsigned char*)malloc(strlen(detail.c_str())*2);
        Base64::Decode((unsigned char*)detail.c_str(), strlen(detail.c_str()), pData, dataLen);
        
		buffsize = 4096;
        destLen = buffsize;
		unsigned char* detailData = (unsigned char*)malloc(buffsize);
        
		while(uncompress(detailData, &destLen, pData, dataLen) != Z_OK)
		{
			free(detailData);
            
			buffsize *= 2;
            
			detailData = (unsigned char*)malloc(buffsize);
			destLen = buffsize;
		}
        
        m_logFight.setFightData(detailData, destLen);
        
        free(pData);
		free(detailData);
        
		onBeginReplay();
	}
	else if(val["cid"].asString() == "achievement")
	{
		m_achievement.goldstorage = getAsInt(val["goldstorage"]);
		m_achievement.pvestar = getAsInt(val["pvestar"]);
		m_achievement.townhall = getAsInt(val["townhall"]);
		m_achievement.otherbuilding = getAsInt(val["otherbuilding"]);
		m_achievement.barrack = getAsInt(val["barrack"]);
		m_achievement.wargold = getAsInt(val["wargold"]);
		m_achievement.warelixir = getAsInt(val["warelixir"]);
		m_achievement.cup = getAsInt(val["cup"]);
		m_achievement.alliancecastle = getAsInt(val["alliancecastle"]);
		m_achievement.warwall = getAsInt(val["warwall"]);
		m_achievement.wartownhall = getAsInt(val["wartownhall"]);
		m_achievement.warworker = getAsInt(val["warworker"]);
		m_achievement.warwin = getAsInt(val["warwin"]);
		m_achievement.defwin = getAsInt(val["defwin"]);
		m_achievement.givesoldier = getAsInt(val["givesoldier"]);
		m_achievement.warmortar = getAsInt(val["warmortar"]);
		m_achievement.wardarkelixir = getAsInt(val["wardarkelixir"]);
        
		m_achievement.goldstorage_star = getAsInt(val["goldstorage_star"]);
		m_achievement.pvestar_star = getAsInt(val["pvestar_star"]);
		m_achievement.townhall_star = getAsInt(val["townhall_star"]);
		m_achievement.otherbuilding_star = getAsInt(val["otherbuilding_star"]);
		m_achievement.barrack_star = getAsInt(val["barrack_star"]);
		//m_achievement.player_star = getAsInt(val["barrack_star"]);
		m_achievement.wargold_star = getAsInt(val["wargold_star"]);
		m_achievement.warelixir_star = getAsInt(val["warelixir_star"]);
		m_achievement.cup_star = getAsInt(val["cup_star"]);
		m_achievement.alliancecastle_star = getAsInt(val["alliancecastle_star"]);
		m_achievement.warwall_star = getAsInt(val["warwall_star"]);
		m_achievement.wartownhall_star = getAsInt(val["wartownhall_star"]);
		m_achievement.warworker_star = getAsInt(val["warworker_star"]);
		m_achievement.warwin_star = getAsInt(val["warwin_star"]);
		m_achievement.defwin_star = getAsInt(val["defwin_star"]);
		m_achievement.givesoldier_star = getAsInt(val["givesoldier_star"]);
		m_achievement.warmortar_star = getAsInt(val["warmortar_star"]);
		m_achievement.wardarkelixir_star = getAsInt(val["wardarkelixir_star"]);
        
        m_vecAchievement.clear();
        for(int i = 0; i < 16; ++i)
        {
            if (i != 8 && i != 14) { //unopen
                m_vecAchievement.push_back(i);
            }
        }
        
		onAchievementChg();
	}
	else if("rankuser" == val["cid"].asString())
	{
        m_lstCupRank.clear();
        
        Json::Value rankList = val["lst"];
        int size = rankList.size();
        
        for (int i = 0; i < size; i ++) {
            
            Json::Value rankUser = rankList[i];
            
            RankUserInfo user;
            
            user.uid = getAsInt(rankUser["uid"]);
            user.name = rankUser["uname"].asString();
            user.curlevel = getAsInt(rankUser["curlevel"]);
            user.curcup = getAsInt(rankUser["cup"]);
            user.currank = getAsInt(rankUser["currank"]);
            user.lastrank = getAsInt(rankUser["lastrank"]);
            
            user.clanid = getAsInt(rankUser["clanid"]);
            user.clanflag = getAsInt(rankUser["clanflag"]);
            user.clanname = rankUser["clanname"].asString();
            user.clancup = getAsInt(rankUser["clancup"]);
            user.clantype = getAsInt(rankUser["clantype"]);
            user.clanneedcup = getAsInt(rankUser["mintrophies"]);
            user.military = getAsInt(rankUser["mrank"]);
            
            m_lstCupRank.push_back(user);
        }
        
        onNotify(NOTIFY_RANKEND);
	}
    else if("supersonicads" == val["cid"].asString())
	{
        CCLOG("gems supersonicads");
		Json::Value ssaVal = val["ssads"];
        if(ssaVal.type() == Json::objectValue)
        {
            vector<string> member = ssaVal.getMemberNames();
            int reward = 0;
            for(int i = 0; i < member.size(); i++)
            {
                Json::Value v = ssaVal[member[i]];
                int r = getAsInt(v["r"]);
                reward += r;
            }
            if(reward > 0)
            {
                LogicInfo::getSingleton().m_addGemType = ADD_GEM_SSA;
                requestAdsGem(reward);
            }
        }
	}
	else if("soldierupgrade" == val["cid"].asString())
	{
		m_soldierupgrade.reset();
        
		int nums = getAsInt(val["nums"]);
		m_soldierupgrade.nums = 0;//getAsInt(val["nums"]);
		m_soldierupgrade.curtime = getCurrentTime();// time(NULL);
		m_soldierupgrade.offtime = getAsInt(val["offtime"]);
        
		for(int i = 0; i < nums; ++i)
		{
			char str[128];
            
			sprintf(str, "soldierid%d", i);
			int sid = getAsInt(val[str]);
            
			sprintf(str, "lasttime%d", i);
			int lt = getAsInt(val[str]);
            
			addSoldierUpgradeInfo(sid, lt);
		}
        
		onMyRefurbishSoldierLevel();
	}
	else if("info" == val["cid"].asString())
	{
		std::string info = UrlDecode(val["info"].asString().c_str());
        
		CCLOG("recv info: %s", info.c_str());
        
		PlayerScene::getSingleton().addNote(info.c_str());
	}
	else if("err" == val["cid"].asString())
	{
        string title = UrlDecode(val["title"].asString().c_str());
		string message = UrlDecode(val["info"].asString().c_str());
        
        int msgid = getAsInt(val["msgid"]);
        
        if (msgid == MSG_DATE_ERROR || msgid == MSG_DATA_NOTSYNC) {
            
            MessageUI::show(MSG_ERROR, NULL, ViewString::getValue("data_sync_error").c_str());
        }
        else if (msgid == MSG_PVE_NOT_LIFE) {
            MessageUI::show(msgid, NULL, ViewString::getValue("pve_not_life").c_str());
        }
        else if (msgid == MSG_NOGEM) {
            MessageUI::show(msgid, NULL, NULL);
        }
        else if (msgid == MSG_ERROR) {
            MessageUI::show(msgid, NULL, message.c_str());
        }
        else {
            MessageUI::show(msgid, title.c_str(), message.c_str());
        }
        
        CCLOG("recv err: %s", message.c_str());
	}
	else if("exit" == val["cid"].asString())
	{
		CCLOG("recv exit.");
	}
	else if("ver" == val["cid"].asString())
	{        
		int verok = getAsInt(val["verok"]);
        CCLOG("verok --> %d", verok);
		if(verok == 0)
		{
			std::string url = val["url"].asString();
            std::string message = val["msg"].asString();
            
			CAppPlatform::sharedAppPlatform()->updateNewVersion(message, url);
		}
		else
		{
			CSVMgr::getSingleton().saveAll();
            
			m_isVerOK = true;
		}
        m_isFreeGemOn = getAsInt(val["isopen_freegem"]);
        
	}
    else if(val["cid"].asString() == "time")
	{
        int64_t curtime = getCurrentTime();
        int64_t serverTime = getAsInt(val["curtime"]);
        
        if (abs((int)(curtime - serverTime)) > 60) { //data sync error
            MessageUI::show(MSG_ERROR, NULL, ViewString::getValue("data_sync_error").c_str());
        }
        
        m_serverTime[1] = serverTime;
        m_curTime[2] = m_curTime[1] = curtime;
        m_offServiceTime = m_serverTime[1] - m_curTime[1];
	}
	else if("usergem" == val["cid"].asString())
	{
        //		int gem = getAsInt(val["gem"]);
        //		m_myInfo.gemex = gem;
        
		onNotify(NOTIFY_BASEINFO);
	}
	else if("errmsg" == val["cid"].asString())
	{
		string info = UrlDecode(val["info"].asString().c_str());
        
        if (APP_DEBUG) {
            MessageUI::show(MSG_NORMAL, NULL, info.c_str());
        }
	}
	else if("iapinfo" == val["cid"].asString())
	{
		m_lstIAPIndex.clear();
        
        char strkey[128];
		int len = m_lstInPurchaseProductInfo.size();
		for(int i = 1; i <= len; ++i)
		{
            std::map<int, InPurchaseProductInfo>::iterator it = m_lstInPurchaseProductInfo.find(i);
            if(it != m_lstInPurchaseProductInfo.end())
            {
                sprintf(strkey, "gem%d", i);
                it->second.gem = getAsInt(val[strkey]);
                sprintf(strkey, "pricef%d", i);
                it->second.price = val[strkey].asDouble();
                sprintf(strkey, "price%d", i);
                it->second.strPrice = val[strkey].asString();
                
                m_lstIAPIndex.push_back(i);
            }
		}
        
        initShopData();
	}
	else if("heroinfo2" == val["cid"].asString())
	{
        CCLOG("heroinfo2");
        
		HeroMgr::getSingleton().m_myHero.curtime = getCurrentTime();// time(NULL);
        
		HeroMgr::getSingleton().m_myHero.uid = m_myInfo.uid;//getAsInt(val["uid"]);
        HeroMgr::getSingleton().m_myHero.heroid = getAsInt(val["heroid"]);;
        HeroMgr::getSingleton().m_myHero.hp = getAsInt(val["hp"]);
        HeroMgr::getSingleton().m_myHero.curexp = getAsInt(val["curexp"]);
        HeroMgr::getSingleton().m_myHero.equ0 = getAsInt(val["equ0"]);
        HeroMgr::getSingleton().m_myHero.equ1 = getAsInt(val["equ1"]);
        HeroMgr::getSingleton().m_myHero.equ2 = getAsInt(val["equ2"]);
        HeroMgr::getSingleton().m_myHero.store = getAsInt(val["store"]);
        HeroMgr::getSingleton().m_myHero.skill0 = getAsInt(val["skill0"]);
        HeroMgr::getSingleton().m_myHero.skill1 = getAsInt(val["skill1"]);
        HeroMgr::getSingleton().m_myHero.skill2 = getAsInt(val["skill2"]);
        HeroMgr::getSingleton().m_myHero.skill3 = getAsInt(val["skill3"]);
        HeroMgr::getSingleton().m_myHero.curhptime = getAsInt(val["curhptime"]);
        HeroMgr::getSingleton().m_myHero.luck = getAsInt(val["luck"]);
        HeroMgr::getSingleton().m_isFreeTrain = getAsInt(val["free_train"]);
        int time = getAsInt(val["lastuptime"]);
        
        const SoldierInfo* pHeroInfo = PersonMgr::getSingleton().getSoldierInfo(HeroMgr::getSingleton().m_myHero.heroid + 1);
        SoldierInfo pInfo ;
        if(pHeroInfo != NULL)
        {
            HeroMgr::getSingleton().m_heroFullTime = pHeroInfo->upgradeTimeHour*60*60;
            HeroMgr::getSingleton().m_heroLastTime = time;
            HeroMgr::getSingleton().m_heroCurTime = getCurrentTime();
            
            if(time > 0)
            {
                HeroMgr::getSingleton().m_myHero.heroid = HeroMgr::getSingleton().m_myHero.heroid + 1;
                HeroMgr::getSingleton().m_heroState = HERO_STATE_UPGRADING;
                pInfo = *PersonMgr::getSingleton().getSoldierInfo(HeroMgr::getSingleton().m_myHero.heroid-1);
                
            }
            else {
                pInfo = *PersonMgr::getSingleton().getSoldierInfo(HeroMgr::getSingleton().m_myHero.heroid);
                HeroMgr::getSingleton().m_heroState = HERO_STATE_NORMAL;
            }
        }
        else
        {
            pInfo = *PersonMgr::getSingleton().getSoldierInfo(HeroMgr::getSingleton().m_myHero.heroid);
        }
        
            
        char skillkey[128];
        char expkey[128];
        for (int i = 0; i < 12; i++) {
            
            sprintf(skillkey, "bskill%d", i);
            HeroMgr::getSingleton().m_myHero.bskill[i] = getAsInt(val[skillkey]);
            
            sprintf(expkey, "bskillexp%d", i);
            HeroMgr::getSingleton().m_myHero.bskillexp[i] = getAsInt(val[expkey]);
        }
        
		HeroMgr::getSingleton().m_lbnums = getAsInt(val["lbnums"]);
		HeroMgr::getSingleton().m_cgnums = getAsInt(val["cgnums"]);
        
        HeroMgr::getSingleton().m_myHero.maxhp = pInfo.hpex;
        HeroMgr::getSingleton().m_myHero.maxexp = pInfo.needexp;
        
        onNotify(NOTIFY_HERO);
    }
	else if("otherheroinfo" == val["cid"].asString())
	{
		HeroMgr::getSingleton().m_otherHero.curtime = getCurrentTime();// time(NULL);
        
        HeroMgr::getSingleton().m_otherHero.uid = getAsInt(val["uid"]);
        HeroMgr::getSingleton().m_otherHero.heroid = getAsInt(val["heroid"]);
        HeroMgr::getSingleton().m_otherHero.hp = getAsInt(val["hp"]);
        HeroMgr::getSingleton().m_otherHero.curexp = getAsInt(val["curexp"]);
        HeroMgr::getSingleton().m_otherHero.equ0 = getAsInt(val["equ0"]);
        HeroMgr::getSingleton().m_otherHero.equ1 = getAsInt(val["equ1"]);
        HeroMgr::getSingleton().m_otherHero.equ2 = getAsInt(val["equ2"]);
        HeroMgr::getSingleton().m_otherHero.store = getAsInt(val["store"]);
        HeroMgr::getSingleton().m_otherHero.skill0 = getAsInt(val["skill0"]);
        HeroMgr::getSingleton().m_otherHero.skill1 = getAsInt(val["skill1"]);
        HeroMgr::getSingleton().m_otherHero.skill2 = getAsInt(val["skill2"]);
        HeroMgr::getSingleton().m_otherHero.skill3 = getAsInt(val["skill3"]);
        HeroMgr::getSingleton().m_otherHero.curhptime = getAsInt(val["curhptime"]);
        HeroMgr::getSingleton().m_otherHero.luck = getAsInt(val["luck"]);
        HeroMgr::getSingleton().m_otherHero.maxhp = getAsInt(val["maxhp"]);
        HeroMgr::getSingleton().m_otherHero.maxexp = getAsInt(val["maxexp"]);
        
        int time = getAsInt(val["lastuptime"]);
        if(time > 0) {
            HeroMgr::getSingleton().m_otherHeroState = HERO_STATE_UPGRADING;
        }
        else {
            HeroMgr::getSingleton().m_otherHeroState = HERO_STATE_NORMAL;
        }
        
        char skillkey[128];
        char expkey[128];
        for (int i = 0; i < 12; i++) {
            
            sprintf(skillkey, "bskill%d", i);
            HeroMgr::getSingleton().m_otherHero.bskill[i] = getAsInt(val[skillkey]);
            
            sprintf(expkey, "bskillexp%d", i);
            HeroMgr::getSingleton().m_otherHero.bskillexp[i] = getAsInt(val[expkey]);
        }
	}
	else if("userrand" == val["cid"].asString())
	{
		RandMgr::getSingleton().init(val);
	}
	else if("heroskill3" == val["cid"].asString())
	{
        HeroMgr::getSingleton().m_myHero.skill3 = getAsInt(val["skill3"]);
        
        onNotify(NOTIFY_RANDSKILL3);
    }
	else if("csvinfo" == val["cid"].asString())
	{
        LogicInfo::getSingleton().addTestLog("csv ver end");
        
		int ver = getAsInt(val["ver"]);
		std::string name = val["name"].asString();
		std::string info = val["info"].asString();
        
//        string type = name.substr(name.find('.'));
        
		if(name == "person")
		{
			CSVMgr::getSingleton().m_csvVer[0] = ver;
			CSVMgr::getSingleton().save("player.cfg", info.c_str());
			PersonMgr::getSingleton().loadData(info.c_str(), info.length(), true);
		}
		else if(name == "building")
		{
			CSVMgr::getSingleton().m_csvVer[1] = ver;
			CSVMgr::getSingleton().save("build.cfg", info.c_str());
            
			BuildingMgr::getSingleton().loadData(info.c_str(), info.length(), true);
		}
		else if(name == "skill")
		{
			CSVMgr::getSingleton().m_csvVer[2] = ver;
			CSVMgr::getSingleton().save("heroskill.cfg", info.c_str());
			SkillMgr::getSingleton().loadData(info.c_str(), info.length(), true);
		}
		else if(name == "townhall")
		{
			CSVMgr::getSingleton().m_csvVer[3] = ver;
			CSVMgr::getSingleton().save("townhall.cfg", info.c_str());
			TownhallLevelInfo::getSingleton().loadData(info.c_str(), info.length(), true);
		}
		else if(name == "configval")
		{
			CSVMgr::getSingleton().m_csvVer[4] = ver;
			CSVMgr::getSingleton().save("configval.cfg", info.c_str());
			ConfigValMgr::getSingleton().reloadFromMem(info.c_str(), info.length(), true);
		}
		else if(name == "pve")
		{
			CSVMgr::getSingleton().m_csvVer[5] = ver;
			CSVMgr::getSingleton().save("pve.cfg", info.c_str());
			EnvMapMgr::getSingleton().loadData(info.c_str(), info.length(), true);
		}
//        else if (type == "ccbi")
//        {
//            CLocalManager::sharedLocalManager()->saveData(name.c_str(), info.c_str(), info.length());
//        }
	}
	else if("gamecenterinfo" == val["cid"].asString())
	{
		int uid = getAsInt(val["uid"]);
		if(uid != 0 && uid != m_myInfo.uid)
		{
			m_unameGameCenterBind = val["uname"].asString();
			m_townlevelGameCenterBind = getAsInt(val["curbaselevel"]);
			m_userlevelGameCenterBind = getAsInt(val["curlevel"]);
            
			m_uidGameCenterBind = uid;
            
            onNotify(NOTIFY_USERLOGIN_INFO);
		}
		else if(uid == 0)
		{
			m_uidGameCenterBind = 0;
            if (GuideMgr::getSingleton().isGuideEnd()) {
                onNotify(NOTIFY_NEW_USERLOGIN);
            }
		}
	}
	else if("attackresult" == val["cid"].asString())
	{
		m_fightWinCups[2] = getAsInt(val["wincup"]);
		m_fightWinCups[1] = getAsInt(val["wincup2"]);
		m_fightWinCups[0] = getAsInt(val["wincup1"]);
		m_fightLoseCups = -getAsInt(val["losecup"]);
	}
    else if(val["cid"].asString() == "revengeuser")
    {
        int msgId = getAsInt(val["msgid"]);
        int uid = getAsInt(val["uid"]);
        int userstate = getAsInt(val["userstate"]);
        
        if ((userstate & USER_STATE_ONLINE) != 0) {
            onNotify(NOTIFY_USER_ONLINE);
        }
        else if ((userstate & USER_STATE_SHIELD) != 0) {
            onNotify(NOTIFY_USER_SHIELD);
        }
        else if((userstate & NOTIFY_USER_ATTACK) != 0)
        {
            onNotify(NOTIFY_USER_ATTACK);
        }
        else{
            
            if (m_revengeBattleId == -1) {
                m_revengeBattleId = msgId;
                m_revengeUserId = uid;
                
                onNotify(NOTIFY_REVENGE_USER);
            }
        }
    }
	else if("findusererr" == val["cid"].asString())
	{
		// find err
		std::string info = val["info"].asString();
        m_revengeBattleId = -1;
	}
    else if ("maillist" == val["cid"].asString())
    {
        m_vecMail.clear();
        
        Json::Value mailList = val["lst"];
        
        int size = mailList.size();
        for (int i = 0; i < size; i ++) {
            
            Json::Value mailInfo = mailList[i];
            
            MailInfo info;
            info.mid = getAsInt(mailInfo["id"]);
            info.uid1 = getAsInt(mailInfo["uid1"]);
            info.uid2 = getAsInt(mailInfo["uid2"]);
            info.uname2 = mailInfo["uname2"].asString();
            info.state = getAsInt(mailInfo["state"]);
            info.title = mailInfo["title"].asString();
            info.message = mailInfo["content"].asString();
            info.gold = getAsInt(mailInfo["gold"]);
            info.elixir = getAsInt(mailInfo["oil"]);
            info.gem = getAsInt(mailInfo["gem"]);
            info.time = getAsInt(mailInfo["curtime"]);
            
            m_vecMail.push_back(info);
        }
        
        if (PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pMainUI) {
            PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->countMailNums(-1);
        }
        onNotify(NOTIFY_MAIL_INBOX);
    }
    else if ("loginwait" == val["cid"].asString())
    {
        int userstate = getAsInt(val["userstate"]);
        
        int time = getAsInt(val["waitbeattacktime"]);
        
        if ((userstate & USER_STATE_BEATTACK) != 0 && m_waitBeAttackTime <= 0) {
            
            HttpClient::getSingleton().m_curClientState = HTTPCLIENT_STATE_WAITLOGIN;
            
            MessageUI::show(MSG_VILLAGE_BEATTACK, ViewString::getValue("warning_title").c_str(), NULL, time, -1);
            
            m_waitBeAttackTime = time;
        }
        
        if (time <= 0) {
            m_waitBeAttackTime = 0;
            
            if ((userstate & USER_STATE_BEATTACK) != 0) {
                HttpClient::getSingleton().login();
            }
        }
        
        CCLOG("loginwait");
    }
    else if ("getmailaward" == val["cid"].asString())
    {
        
    }
    else if ("signininfo" == val["cid"].asString())
    {
        int lastdays = getAsInt(val["lastday"]);
        int days = getAsInt(val["days"]);
        
        if (lastdays == 0) {
            m_signinDays = -1;
        }
        else{
            m_signinDays = days;
        }        
    }
    else if ("spinaward" == val["cid"].asString())
    {
        onNotify(NOTIFY_SPIN_AWARD, &val);
    }
    else if ("adflag" == val["cid"].asString())
    {
        m_iAdsOpen = getAsInt(val["adflag"]);
    }
    else if ("cfg" == val["cid"].asString())
    {
        Json::Value cfgVal = val["cfg"];
        checkConfig("isChartboostAdOn", m_isChartboostAdOn, cfgVal);
        checkConfig("isFlurryAdOn", m_isFlurryAdOn, cfgVal);
        checkConfig("isTapjoyAdOn", m_isTapjoyAdOn, cfgVal);
        checkConfig("isAarkiOn", m_isAarkiOn, cfgVal);
        checkConfig("isSupersonicAdsOn", m_isSupersonicAdsOn, cfgVal);
        checkConfig("chartboostClickGems", m_chartboostClickGem, cfgVal);
        checkConfig("isNativeXAdOn", m_isNativeXAdOn, cfgVal);
        checkConfig("AdsType", m_defaultAdsType, cfgVal);
        checkConfig("FullAdsType", m_defaultFullAdsType, cfgVal);
        checkConfig("SigninDays", m_isSigninDays, cfgVal);
        
        checkConfig("FBShare", FacebookMgr::getSingleton().m_shareState, cfgVal);
        checkConfig("FBInvite", FacebookMgr::getSingleton().m_inviteState, cfgVal);
        checkConfig("FBInviteType", FacebookMgr::getSingleton().m_inviteType, cfgVal);
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//        if(cfgVal["facebook_android"].type() != Json::nullValue)
//        {
//            FacebookMgr::getSingleton().m_state = getAsInt(cfgVal["facebook_android"]);
//        }
        
        checkConfig("facebook_android", FacebookMgr::getSingleton().m_state, cfgVal);
#endif
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//         if(cfgVal["facebook_ios"].type() != Json::nullValue)
//         {
//             FacebookMgr::getSingleton().m_state = getAsInt(cfgVal["facebook_ios"]);
//         }
        checkConfig("facebook_ios", FacebookMgr::getSingleton().m_state, cfgVal);
#endif
        
        string adKey(APP_ID);
        adKey += ("_"+CAppPlatform::sharedAppPlatform()->getBundleVersion());
        Json::Value v = cfgVal[adKey];
        if(v.type() != Json::nullValue)
            m_isShowFreeGemInBonus = getAsInt(cfgVal[adKey]);
        
        string isReviewKey("review"APP_ID);
        isReviewKey += ("_"+CAppPlatform::sharedAppPlatform()->getBundleVersion());
        v = cfgVal[isReviewKey];
        if(v.type() != Json::nullValue)
            m_isReviewing = getAsInt(cfgVal[isReviewKey]);
        
        string paytype(APP_ID"_paytype");
        v = cfgVal[paytype];
        if(v.type() != Json::nullValue)
            m_payType = getAsInt(cfgVal[paytype]);
        
        string message;
        string key = "FBShareMessage";
        string shareMessgaeKey(key+"_"+APP_ID+"_"+CAppPlatform::sharedAppPlatform()->getBundleVersion());
        if(cfgVal[shareMessgaeKey].type() != Json::nullValue)
        {
            message = cfgVal[shareMessgaeKey].asString();
        }
        else
        {
            string shareMessgaeKey(key+"_"+APP_ID);
            if(cfgVal[shareMessgaeKey].type() != Json::nullValue)
            {
                message = cfgVal[shareMessgaeKey].asString();
            }
            else
            {
                string shareMessgaeKey(key);
                if(cfgVal[shareMessgaeKey].type() != Json::nullValue)
                {
                    message = cfgVal[shareMessgaeKey].asString();
                }
            }
        }
        if(message.length() > 0)
            FacebookMgr::getSingleton().m_shareMessaage = message;
        
        if(APP_DEBUG)
        {
            FacebookMgr::getSingleton().m_state = 1;
            FacebookMgr::getSingleton().m_shareState = 1;
            FacebookMgr::getSingleton().m_inviteState = 1;
            m_defaultAdsType = 6;
            m_defaultFullAdsType = 1;
            m_isReviewing = 1;
            m_isSigninDays = 1;
        }
        
        if(FacebookMgr::getSingleton().m_state == 0)
        {
            LogicInfo::getSingleton().m_bindType = BIND_SYSTEM_TYPE;
        }
    }
    else if ("notice" == val["cid"].asString())
    {
        if(val["notice_status"] != Json::nullValue)
            m_announcementType = getAsInt(val["notice_status"]);
        
        if(val["notice_url"] != Json::nullValue)
            m_webURL = val["notice_url"].asCString();
        
        string oldWebURL = CCUserDefault::sharedUserDefault()->getStringForKey("WEB_URL");
        if(oldWebURL != m_webURL && m_webURL.length() > 0)
        {
            CCUserDefault::sharedUserDefault()->setStringForKey("WEB_URL", m_webURL);
            m_webUrlTip = true;
        }
        
        if(m_announcementType == ANNOUNCEMENT_TYPE_FORCE)
        {
            AnnouncementUI *pAnnouncementUI = AnnouncementUI::create();
            CCDirector::sharedDirector()->getRunningScene()->addChild(pAnnouncementUI, 10);
        }
    }
    else if ("freegem" == val["cid"].asString())
    {
        m_isChartboostFreeGemReady = getAsInt(val["rst"]);
    }
    else if ("fbshare" == val["cid"].asString())
    {
        int rst = getAsInt(val["rst"]);
        if(rst > 0)
        {
            LogicInfo::getSingleton().m_addGemType = ADD_GEM_FB_SHARE;
            m_asyncViewList.insert(pair<int, int>(ASYNC_VIEW_ADD_GEM, 5));
        }
    }
    else if ("fbfreegem" == val["cid"].asString())
    {
        int addGem = getAsInt(val["rst"]);
        if(addGem > 0)
        {
            m_addGemType = ADD_GEM_FB_LOGIN;
            requestAdsGem(addGem);
        }
    }
    else if ("adgemrst" == val["cid"].asString())
    {
        string rst = val["rst"].asCString();
        int type = getAsInt(val["type"]);
        int gem = getAsInt(val["gem"]);
        if(rst == "success")
        {
            m_addGemType = type;
            m_asyncViewList.insert(pair<int, int>(ASYNC_VIEW_ADD_GEM, gem));
            if(m_addGemType == ADD_GEM_NATIVE_X)
            {
                m_pendingNativeXGem = 0;
            }
        }
        else if(rst == "invalid")
        {
            
        }
    }
    else if ("fbinvite" == val["cid"].asString())
    {
        Json::Value inviteVal = val["fbinvite"];
        CCLOG("%s",val.toStyledString().c_str());
        int gem = 0;
        int count = 0;
        for(int i = 0; i < inviteVal.size(); i++)
        {
//            string info = val[i]["info"].asString();
            int rst = getAsInt(inviteVal[i]["rst"]);
            if(rst == 1)
            {
                gem += 5;
                count += 1;
            }
        };
        if(gem > 0)
        {
            FacebookMgr::getSingleton().m_inviteCount = count;
            m_addGemType = ADD_GEM_FB_INVITE;
            m_asyncViewList.insert(pair<int, int>(ASYNC_VIEW_ADD_GEM, gem));
        }
    }
    else if ("fbids" == val["cid"].asString())
    {
        CCLOG("%s",val.toStyledString().c_str());
        Json::Value fnameVal = val["fbids"];
        for(int i = 0; i < fnameVal.size(); i++)
        {
            string fname = fnameVal[i].asCString();
            FacebookMgr::getSingleton().deleteFriendByName(fname);
        };
//        string idStr = val["fbids"].asCString();
//        string pattern = "㊣";
//        idStr += pattern;
//        std::string::size_type pos;
//        int size=idStr.size();
//        for(int i=0; i<size; i++)
//        {
//            pos=idStr.find(pattern,i);
//            if(pos<size)
//            {
//                std::string fname=idStr.substr(i,pos-i);
//                FacebookMgr::getSingleton().deleteFriendByName(fname);
//                i=pos+pattern.size()-1;
//            }
//        }

        onNotify(NOTIFY_FB_FRIEND_CHECK_OK);
        ;
    }
    else if ("corinfo" == val["cid"].asString())
    {
        m_iapGiftInfo.corflag = getAsInt(val["corflag"]);
        
        if(m_iapGiftInfo.corflag == 0)
        {
            m_iapGiftInfo.last_time =getAsInt(val["lasttime"]);
            Json::Value proVal = val["corbuyid"];
            
            for(int i = 0; i < proVal.size(); i++)
            {
                int bid = getAsInt(proVal[i]["bid"]);
                int totalGem = getAsInt(proVal[i]["totalgem"]);
                int highlight = getAsInt(proVal[i]["highlight"]);
                int totalDays = getAsInt(proVal[i]["totaldays"]);
                IapHighlightInfo info = {totalGem,highlight,totalDays};
                m_iapGiftInfo.corIapMap.insert(pair<int, IapHighlightInfo>(bid, info));
            }
            
        }
        else if (m_iapGiftInfo.corflag == 1)
        {
            //活动屏蔽
        }
        else if (m_iapGiftInfo.corflag == 2)
        {
            m_iapGiftInfo.totalDays = getAsInt(val["totaldays"]);
            m_iapGiftInfo.gotDays = getAsInt(val["gotdays"]);
            m_iapGiftInfo.buyId = getAsInt(val["buyid"]);
            m_iapGiftInfo.gotflag = getAsInt(val["gotflag"]);
            m_iapGiftInfo.corclaim = getAsInt(val["corclaim"]);
            m_iapGiftInfo.stopDays = m_iapGiftInfo.totalDays - m_iapGiftInfo.gotDays;
        }
        
    }
    else if ("sysnotifyinfo" == val["cid"].asString())
    {
        string info = val["info"].asString();
        PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI->addNote(info.c_str());
    }
    else if ("userpveinfo" == val["cid"].asString())
    {
        m_bEnvMapInit = true;
        
        time_t restime = getAsInt(val["lasttime_res"]);
        time_t lifetime = getAsInt(val["lasttime_life"]);
        int lifetime_off = getAsInt(val["lifetime_static"]);
        int curlife = getAsInt(val["curlife"]);
        int maxlife = getAsInt(val["maxlife"]);
        
        LogicInfo::getSingleton().m_pveInfo.restime = restime;
        LogicInfo::getSingleton().m_pveInfo.lifetime = lifetime;
        LogicInfo::getSingleton().m_pveInfo.curlife = curlife;
        LogicInfo::getSingleton().m_pveInfo.maxlife = maxlife;
        LogicInfo::getSingleton().m_pveInfo.lifetime_off = lifetime_off;
        
        LogicInfo::getSingleton().m_pveInfo.last_restime = getCurrentTime();
        LogicInfo::getSingleton().m_pveInfo.last_lifetime = getCurrentTime();
        
        Json::Value stageInfo = val["stageinfo"];
        
        for (int i = 0; i < stageInfo.size(); i++) {
            
            int stageid = getAsInt(stageInfo[i]["stageid"]);
            
            EnvStageInfo* info = EnvMapMgr::getSingleton().getEnvStageInfo(stageid);
            info->star = getAsInt(stageInfo[i]["star"]);
            info->gold = getAsInt(stageInfo[i]["gold"]);
            info->oil = getAsInt(stageInfo[i]["oil"]);
        }
        
        onNotify(NOTIFY_PVE_MAP);
    }
    else if ("userpvelifeinfo" == val["cid"].asString())
    {
        int curlife = getAsInt(val["curlife"]);
        //        int lasttime_life = getAsInt(val["lasttime_life"]);
        
        LogicInfo::getSingleton().m_pveInfo.curlife = curlife;
        
        onNotify(NOTIFY_PVE_ADD_LIFE);
    }
    else if ("purchaseresult" == val["cid"].asString())
    {
        int productid = getAsInt(val["productid"]);
        
        std::map<int, InPurchaseProductInfo>::iterator it = m_lstInPurchaseProductInfo.find(productid);
        
        string result = val["result"].asString();
        int status = getAsInt(val["rstcode"]);
        
        if (it != m_lstInPurchaseProductInfo.end()) {
            
            CAppPlatform::sharedAppPlatform()->consumePurchase(it->second.strPlatformProductID);
            onInPurchaseVerifyComplete(productid, status);
        }
    }
    else if("league_all_info" == val["cid"].asString())
    {
        const Json::Value leagueInfo = val["league_info"];
        LeagueMgr::getSingleton().m_leagueid = getAsInt(leagueInfo["league_id"]);
        LeagueMgr::getSingleton().m_state = getAsInt(leagueInfo["league_status"]);
        LeagueMgr::getSingleton().m_startTime = getAsInt(leagueInfo["start_time"]);
        LeagueMgr::getSingleton().m_endTime = getAsInt(leagueInfo["stop_time"]);
        LeagueMgr::getSingleton().m_nextTime = getAsInt(leagueInfo["next_time"]);
        const Json::Value levelinfo = val["league_level_info"];
        LeagueMgr::getSingleton().setLevelCupInfo(levelinfo);
        CCLOG("league_all_info %s",val.toStyledString().c_str());
        
        onNotify(NOTIFY_BASEINFO);
    }
    else if("league_my_group" == val["cid"].asString())
    {
        //CCLOG("league_my_group %s",val.toStyledString().c_str());
        LeagueMgr::getSingleton().m_myGroup.state = getAsInt(val["user_league_flag"]);
        LeagueMgr::getSingleton().m_myGroup.groupnums = getAsInt(val["group_nums"]);
        const Json::Value group_user = val["group_user"];
        LeagueMgr::getSingleton().setPlayerInfo(group_user);
        const Json::Value group_info = val["group_info"];
        LeagueMgr::getSingleton().setCurBattleAwardInfo(group_info["battle_award"]);
        LeagueMgr::getSingleton().setRankAwardInfo(group_info["rank_award"]);
        LeagueMgr::getSingleton().m_myGroup.groupid = getAsInt(group_info["group_id"]);
        LeagueMgr::getSingleton().m_myGroup.levelid = getAsInt(group_info["level"]);
        LeagueMgr::getSingleton().m_myGroup.name    = group_info["level_name"].asString();
    }
    else if("league_my_award" ==  val["cid"].asString())
    {
        CCLOG("league_my_award ====== %s",val.toStyledString().c_str());
        LeagueMgr::getSingleton().setMyAward(val);
    }
    else if ("league_init_ok" ==  val["cid"].asString())
    {
        if(PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pMainUI)
        {
            PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->showLeagueStartUI();
            PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->showGetLeagueAwardUI();
        }
        CCLOG("NOTIFY_LG_LEVEL_INFO_OK");
        onNotify(NOTIFY_LG_LEVEL_INFO_OK);
    }
	else if("battlelogok" == val["cid"].asString())
	{
		int clansoldier = getAsInt(val["clansoldier"]);
        
		if(clansoldier > 0)
		{
			SocketClient::getSingleton().m_modClan.resetCastleInfo();
		}
	}
    else if ("getgem" == val["cid"].asString())
    {
        int mask = getAsInt(val["mask"]);
        int gem = getAsInt(val["gem"]);
        string title = val["title"].asString();
        string msg = val["msg"].asString();
        
        MessageUI::show(MSG_GET_GEM, title.c_str(), msg.c_str(), gem, mask);
    }
    else if ("editok" == val["cid"].asString())
    {
        PlayerScene::getSingleton().m_pGameWorld->hideEditUI();
        
        PlayerScene::getSingleton().clearScene();
        PlayerScene::getSingleton().initScene();
        
    }
	if (ClanMgr::getSingleton().onProc(val)) {
		return true;
    }
    if (ClanWarMgr::getSingleton().onProc(val)) {
        return true;
    }
    if (GroupWarMgr::getSingleton().onProc(val)) {
        return true;
    }
    
	return true;
}

void LogicInfo::onAddBarrack(UserBuildingInfo buildingInfo, const BuildingInfo* pInfo)
{
	MapBarrackInfo::iterator it = m_mapBarrack.find(buildingInfo.barrackid);
	if(it == m_mapBarrack.end())
	{
		PairBarrackInfo pbi;
        
		pbi.first = buildingInfo.barrackid;
		pbi.second.barrackid = buildingInfo.barrackid;
		pbi.second.lasttime = 0;
		pbi.second.curtime = 0;
		pbi.second.ischg = false;
        pbi.second.isPause = false;
        pbi.second.isUpgrade = false;
        pbi.second.maxnumsex = pInfo->housingspace;
        
        if (buildingInfo.state == BUILDING_STATE_UPGRADING) {
            pbi.second.isUpgrade = true;
        }
        
		m_mapBarrack.insert(pbi);
	}
    else
    {
		it->second.curtime = getCurrentTime();
		it->second.ischg = false;
        it->second.isPause = false;
        it->second.isUpgrade = false;
        it->second.maxnumsex = pInfo->housingspace;
        
        if (buildingInfo.state == BUILDING_STATE_UPGRADING) {
            it->second.isUpgrade = true;
        }
        
        HttpClient::getSingleton().updateBarrackInfo(it->second);
    }
}

void LogicInfo::onAddTroophousing(int troophousingid, int maxnums)
{
	MapTroophousingInfo::iterator it = m_mapTroophousing.find(troophousingid);
	if(it == m_mapTroophousing.end())
	{
		PairTroophousingInfo pbi;
        
		pbi.first = troophousingid;
		pbi.second.troophousingid = troophousingid;
		pbi.second.maxnumsex = maxnums;
		pbi.second.ischg = false;
        
		m_mapTroophousing.insert(pbi);
	}
}

void LogicInfo::onAddOtherTroophousing(int troophousingid, int maxnums)
{
	MapTroophousingInfo::iterator it = m_mapOtherTroophousing.find(troophousingid);
	if(it == m_mapOtherTroophousing.end())
	{
		PairTroophousingInfo pbi;
        
		pbi.first = troophousingid;
		pbi.second.troophousingid = troophousingid;
		pbi.second.maxnumsex = maxnums;
		pbi.second.ischg = false;
        
		m_mapOtherTroophousing.insert(pbi);
	}
}

bool LogicInfo::addSoldier(int soldierid, Building* pFrom)
{
	if(m_curState != LOGICSTATE_NORMAL && m_curState != LOGICSTATE_WAIT)
		return false;
    
	const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(soldierid);
	if(pInfo == NULL)
		return false;
    
    int totalspace = 0;
    int maxspace = 0;
    
    for(MapTroophousingInfo::iterator it = m_mapTroophousing.begin(); it != m_mapTroophousing.end(); ++it)
	{
		int curspace = 0;
		for(std::list<TroophousingSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
		{
			curspace += it1->spaceex * it1->numsex;
		}
        
        totalspace += curspace;
        maxspace += it->second.maxnumsex;
    }
    
    int freespace = 0;
    
	for(MapTroophousingInfo::iterator it = m_mapTroophousing.begin(); it != m_mapTroophousing.end(); ++it)
	{
		int curspace = 0;
		for(std::list<TroophousingSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
		{
			curspace += it1->spaceex * it1->numsex;
		}
        
        freespace += it->second.maxnumsex - curspace;
        
        bool hasSoldier = false;
        
        if((totalspace + pInfo->housingSpaceex <= maxspace) && pInfo->housingSpaceex <= freespace)
		{
			curspace += pInfo->housingSpaceex;
            freespace -= pInfo->housingSpaceex;
            
            std::list<TroophousingSoldierInfo>::iterator it1;
			for( it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
			{
				if(it1->pid == soldierid)
				{
					it->second.ischg = true;
					it1->numsex += 1;
                    
                    hasSoldier = true;
					break;
				}
			}
            
            if (!hasSoldier) {
                TroophousingSoldierInfo tsi;
                
                tsi.pid = soldierid;
                tsi.numsex = 1;
                tsi.spaceex = pInfo->housingSpaceex;
                
                it->second.lstSoldier.push_back(tsi);
                it->second.ischg = true;
            }
            
			if(pFrom != NULL)
			{
				Person* pPerson = PlayerScene::getSingleton().addPerson(soldierid, pFrom->getLogicX(), pFrom->getLogicY(), PERSON_ACT_WALK, CAMP_DEFENCE);
				Building* pDest = PlayerScene::getSingleton().findTroophousing(it->second.troophousingid);
				if(pDest != NULL)
				{
					pPerson->m_personType = PERSON_TYPE_SOLDIER;
                    
					CCPoint pt;
					PlayerScene::getSingleton().countCurPosTroophousing(pt, curspace);
                    
                    int ex = pDest->getLogicX() - pt.x;
                    int ey = pDest->getLogicY() - pt.y;
                    PlayerScene::getSingleton().m_astar.procDestPt(pPerson->getLogicX(), pPerson->getLogicY(), ex, ey);
                    
                    int rx = (rand() % 9) - 4;
                    pPerson->setLogicPos(pPerson->getLogicX() - rx, pPerson->getLogicY());
                    
					pPerson->setPersonPath(ex, ey);
					pPerson->m_ptSleep.x = ex;
					pPerson->m_ptSleep.y = ey;
                    
					pPerson->m_pTroophousing = pDest->m_pBuildingInfo;
					pPerson->m_tlx = pt.x;
					pPerson->m_tly = pt.y;
				}
                
                if (PlayerScene::getSingleton().m_isEditMode) {
                    pPerson->setVisible(false);
                }
			}
            
			return true;
		}
	}
    
	return false;
}

bool sortBarrackByLastTime(BarrackInfo barrack1, BarrackInfo barrack2)
{
    if (barrack1.lasttime < barrack2.lasttime) {
        return true;
    }
    return false;
}

void LogicInfo::onRefurbishAllBarrack()
{
    int ct = getCurrentTime();
    
    vector<BarrackInfo> barrackList;
    for(MapBarrackInfo::iterator it = m_mapBarrack.begin(); it != m_mapBarrack.end(); ++it)
    {
        if (it->second.curtime > 0) {
            it->second.offtime = ct - it->second.curtime;
            barrackList.push_back(it->second);
        }
    }
    
    bool ret = true;
    while (ret) {
        
        sort(barrackList.begin(), barrackList.end(), sortBarrackByLastTime);
        
        ret = false;
        for (int i = 0; i < barrackList.size(); i ++) {
            map<int, BarrackInfo>::iterator it = m_mapBarrack.find(barrackList[i].barrackid);
            
            if (it->second.offtime > 0) {
                bool result = onRefurbishBarrack(it->first, it->second.offtime, NULL);
                
                if (result) {
                    ret = true;
                }
                it->second.offtime -= barrackList[i].lasttime;
                it->second.curtime = ct;
                barrackList[i].lasttime = it->second.lasttime;
            }
        }
    }
    
    for(MapBarrackInfo::iterator it = m_mapBarrack.begin(); it != m_mapBarrack.end(); ++it)
	{
        if (it->second.curtime > 0) {
            if (it->second.ischg) {
                HttpClient::getSingleton().updateBarrackInfo(it->second);
            }
            it->second.curtime = ct;
        }
	}
}

void LogicInfo::onRefurbishAllTroophousing()
{
	for(MapTroophousingInfo::iterator it = m_mapTroophousing.begin(); it != m_mapTroophousing.end(); ++it)
	{
		for(std::list<TroophousingSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); )
		{
			if(!canRecruitSoldier(it1->pid, false))
			{
				it1 = it->second.lstSoldier.erase(it1);
                
				it->second.ischg = true;
			}
			else
				++it1;
		}
        
        if (it->second.ischg) {
            HttpClient::getSingleton().updateTroophousingInfo(it->second);
        }
	}
}

void LogicInfo::refurbishSoldierNums()
{
	m_maxSoldier = 0;
	m_curSoldier = 0;
    
	for(MapTroophousingInfo::iterator it = m_mapTroophousing.begin(); it != m_mapTroophousing.end(); ++it)
	{
		m_maxSoldier += it->second.maxnumsex;
        
		for(std::list<TroophousingSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
		{
			const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(it1->pid);
			if(pInfo != NULL)
				m_curSoldier += it1->numsex * pInfo->housingSpaceex;
		}
	}
    
	for(MapBarrackInfo::iterator it = m_mapBarrack.begin(); it != m_mapBarrack.end(); ++it)
	{
		for(std::list<BarrackSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
		{
			const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(it1->pid);
			if(pInfo != NULL)
				m_curSoldier += it1->numsex * pInfo->housingSpaceex;
		}
	}
}

bool LogicInfo::onRefurbishBarrackMs(int barrackid, int ot, Building* pBuilding)
{
	if(m_curState != LOGICSTATE_NORMAL)
		return false;
    
    int curspace = 0;
    int maxspace = 0;
    
    LogicInfo::getSingleton().countTroopSpace(m_mapTroophousing, curspace, maxspace);
    
    int space = 0;
    MapBarrackInfo::iterator it = m_mapBarrack.find(barrackid);
    
    if (it->second.lstSoldier.size() > 0) {
        list<BarrackSoldierInfo>::iterator it1 = it->second.lstSoldier.begin();
        const SoldierInfo* info = PersonMgr::getSingleton().getSoldierInfo(it1->pid);
        space = info->housingSpaceex;
    }
    if (curspace + space <= maxspace)
    {
        it->second.isPause = false;
    }
    
	if(it->second.isPause)
	{
        if (pBuilding) {
            if (it->second.lstSoldier.size() > 0) {
                if (pBuilding->m_strIconEx != "ui_img_exc") {
                    pBuilding->chgIconEx("ui_img_exc");
                }
            }else{
                if(pBuilding->m_pIconEx != NULL){
                    pBuilding->m_pIconEx->setVisible(false);
                }
            }
            pBuilding->setIcon(NULL);
            pBuilding->setTime(it->second.lasttime, 0);
        }
        
        if(it != m_mapBarrack.end())
        {
            it->second.ischg = true;
            it->second.curtime = getCurrentTime();
        }
        
		return false;
	}
    
	if(it != m_mapBarrack.end())
	{
		if(it->second.lstSoldier.empty())
		{
			if(it->second.ischg)
			{
				onBarrackFinish();
				//it->second.ischg = false;
			}
            
            if (pBuilding) {
                pBuilding->setTime(0, 0);
            }
            
			return false;
		}
        
        time_t curtime = getCurrentTime();
		int offtime = curtime - it->second.curtime;
        
		if(offtime <= 0)
			return false;
        
		onRefurbishBarrack(barrackid, offtime, pBuilding);
        it->second.curtime = curtime;
	}
    
	return true;
}

bool LogicInfo::onRefurbishBarrack(int barrackid, int offtime, Building* pBuilding)
{
	MapBarrackInfo::iterator it = m_mapBarrack.find(barrackid);
	if(it != m_mapBarrack.end())
	{
        if (it->second.isPause) {
            return false;
        }
        if (it->second.isUpgrade) {
            return false;
        }
        
		if(it->second.lstSoldier.empty())
		{
			if(pBuilding != NULL)
				pBuilding->setTime(0, 0);
            
			if(it->second.ischg)
			{
				onBarrackFinish();
			}
            
            return false;
		}
        else {
            
            list<BarrackSoldierInfo>::iterator it1 = it->second.lstSoldier.begin();
            
            if(offtime >= it->second.lasttime)
            {
                it->second.ischg = true;
                
                if(!addSoldier(it1->pid, pBuilding))
                {
                    it->second.isPause = true;
                    it->second.lasttime = 0;
                    
                    if (it->second.ischg)
                        HttpClient::getSingleton().updateBarrackInfo(it->second);
                }
                else
                {
                    it->second.lasttime = it1->needtimeex;
                    it1->numsex -= 1;
                    
                    if (pBuilding != NULL) {
                        SoundMgr::getSingleton().playEffect("training_finished.m4a", 200);
                    }
                }
            }
            else
            {
                it->second.ischg = true;
                it->second.lasttime -= offtime;
            }
            
            if (it1->numsex <= 0) {
                it1 = it->second.lstSoldier.erase(it1);
                
                if(it1 != it->second.lstSoldier.end())
                    it->second.lasttime = it1->needtimeex;
            }
            
			if(pBuilding != NULL && !it->second.lstSoldier.empty())
                pBuilding->setTime(it->second.lstSoldier.front().needtimeex, it->second.lasttime);
            
            return true;
		}
	}
    
	return false;
}

int LogicInfo::countMilitaryRank()
{
    MilitaryRankMgr::getSingleton().countMilitaryRank();
    return m_myInfo.military;
}

int LogicInfo::countTroopSpace()
{
    int curspace, maxspace;
    
    LogicInfo::getSingleton().countTroopSpace(LogicInfo::getSingleton().m_mapTroophousing, curspace, maxspace);
    
    return curspace;
}

void LogicInfo::countTroopSpace(MapTroophousingInfo troophousingInfo, int &curspace, int &maxspace)
{
    curspace = 0;
    maxspace = 0;
    
    for(MapTroophousingInfo::iterator it = troophousingInfo.begin(); it != troophousingInfo.end(); ++it)
	{
		int space = 0;
		for(std::list<TroophousingSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
		{
			space += it1->spaceex * it1->numsex;
		}
        
        curspace += space;
        maxspace += it->second.maxnumsex;
    }
}

int LogicInfo::addSoldier2Barrack(int barrackid, int pid)
{
	MapBarrackInfo::iterator it = m_mapBarrack.find(barrackid);
	if(it != m_mapBarrack.end())
	{
		const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(pid);
		if(pInfo == NULL)
			return MSG_ERROR;
        
		if(pInfo->trainingCost > getMoney(pInfo->trainingRes))
			return MSG_NOELIXIR;
        
		int curnums = 0;
		for(std::list<BarrackSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
		{
			const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(it1->pid);
			if(pInfo != NULL)
				curnums += it1->numsex * pInfo->housingSpaceex;
		}
        
		if(curnums >= it->second.maxnumsex)
			return MSG_ERROR;
        
		chgMoney(pInfo->trainingRes, -pInfo->trainingCost);
        
		m_barrackMoney[pInfo->trainingRes] += -pInfo->trainingCost;
        
		it->second.ischg = true;
        
		if(it->second.lstSoldier.empty())
		{
			it->second.lasttime = pInfo->trainingTime;
			it->second.curtime = getCurrentTime();// time(NULL);
		}
        
		for(std::list<BarrackSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
		{
			if(it1->pid == pid)
			{
				it1->numsex += 1;
                
				refurbishSoldierNums();
                
				return MSG_ERROR;
			}
		}
        
		BarrackSoldierInfo bsi;
        
		bsi.pid = pid;
		bsi.numsex = 1;
        
		bsi.needtimeex = pInfo->trainingTime;
        
		it->second.lstSoldier.push_back(bsi);
        
		refurbishSoldierNums();
	}
    
    return 1;
}

void LogicInfo::clearMyScene()
{
	PlayerScene::getSingleton().clearScene();
    
	for(std::list<UserBuildingInfo*>::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		delete *it;
	}
    
	m_lstBuilding.clear();
}

void LogicInfo::clearOtherScene()
{
    ClanMgr::getSingleton().m_otherclan.id = 0;
    
	PlayerScene::getSingleton().clearScene();
    
	for(std::list<UserBuildingInfo*>::iterator it = m_otherBuilding.begin(); it != m_otherBuilding.end(); ++it)
	{
		delete *it;
	}
    
	m_otherBuilding.clear();
	m_mapOtherTroophousing.clear();
}

void LogicInfo::delSoldier2Barrack(int barrackid, int pid)
{
	MapBarrackInfo::iterator it = m_mapBarrack.find(barrackid);
	if(it != m_mapBarrack.end())
	{
		const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(pid);
		if(pInfo == NULL)
			return ;
        
		chgMoney(pInfo->trainingRes, pInfo->trainingCost);
        
		m_barrackMoney[pInfo->trainingRes] += pInfo->trainingCost;
        
		//if(pInfo->trainingRes == MONEY_GOLD)
		//	HttpClient::getSingleton().addMoney(0, pInfo->trainingCost, 0);
		//else if(pInfo->trainingRes == MONEY_ELIXIR)
		//	HttpClient::getSingleton().addMoney(0, 0, pInfo->trainingCost);
		//else if(pInfo->trainingRes == MONEY_GEM)
		//	HttpClient::getSingleton().addMoney(pInfo->trainingCost, 0, 0);
        
		it->second.ischg = true;
        
		int i = 0;
		for(std::list<BarrackSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
		{
			if(it1->pid == pid)
			{
				if(it1->numsex == 1)
				{
					it->second.lstSoldier.erase(it1);
					if(i == 0)
					{
						if(it->second.lstSoldier.empty())
						{
							it->second.lasttime = 0;
							it->second.curtime = getCurrentTime();// time(NULL);
                            it->second.isPause = false;
						}
						else
						{
							it1 = it->second.lstSoldier.begin();
                            
							it->second.lasttime = it1->needtimeex;
							it->second.curtime = getCurrentTime();// time(NULL);
						}
					}
				}
				else
					it1->numsex -= 1;
                
				refurbishSoldierNums();
				return ;
			}
			++i;
		}
	}
}

void LogicInfo::onCloseBarrack()
{
    for(MapBarrackInfo::iterator it = m_mapBarrack.begin(); it != m_mapBarrack.end(); ++it)
	{
        if(it != m_mapBarrack.end())
        {
            if(it->second.ischg)
            {
                HttpClient::getSingleton().updateBarrackInfo(it->second);
            }
        }
    }
    
	if(m_barrackMoney[0] < 0)
		HttpClient::getSingleton().subMoney(m_barrackMoney[0], 0, 0);
    
	if(m_barrackMoney[1] != 0)
	{
		if(m_barrackMoney[1] < 0)
			HttpClient::getSingleton().subMoney(0, -m_barrackMoney[1], 0);
		else
			HttpClient::getSingleton().addMoney(0, m_barrackMoney[1], 0);
	}
    
	if(m_barrackMoney[2] != 0)
	{
		if(m_barrackMoney[2] < 0)
			HttpClient::getSingleton().subMoney(0, 0, -m_barrackMoney[2]);
		else
			HttpClient::getSingleton().addMoney(0, 0, m_barrackMoney[2]);
	}
}

void LogicInfo::onOpenBarrack()
{
	m_barrackMoney[0] = m_barrackMoney[1] = m_barrackMoney[2] = 0;
}

void LogicInfo::onInitTroophousingSoldier()
{
    if (PlayerScene::getSingleton().m_isEditMode) {
        return;
    }
    
	PlayerScene::getSingleton().clearNormalSoldier();

    if (m_curState == LOGICSTATE_VIEW)
    {
        for(MapTroophousingInfo::iterator it = m_mapOtherTroophousing.begin(); it != m_mapOtherTroophousing.end(); ++it)
        {
            PlayerScene::getSingleton().onInitTroophousing(it->second, false);
        }
    }
    else
    {
        for(MapTroophousingInfo::iterator it = m_mapTroophousing.begin(); it != m_mapTroophousing.end(); ++it)
        {
            PlayerScene::getSingleton().onInitTroophousing(it->second, false);
        }
    }
	
}

void LogicInfo::onInitOtherTroophousingSoldier()
{
	for(MapTroophousingInfo::iterator it = m_mapOtherTroophousing.begin(); it != m_mapOtherTroophousing.end(); ++it)
	{
		PlayerScene::getSingleton().onInitTroophousing(it->second, true);
	}
}

void LogicInfo::onFastBuild()
{
    if(HeroMgr::getSingleton().m_isSelect)
    {
        MessageUI::show(MSG_BUILDING_FAST, "", "");
        return;
    }
    if (PlayerScene::getSingleton().m_pCurBuilding) {
        
        if (GuideMgr::getSingleton().isGuiding()) {
            Building* pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
            LogicInfo::getSingleton().fastBuilding(*pBuilding);
            GuideMgr::getSingleton().next();
        }
        else
        {
            MessageUI::show(MSG_BUILDING_FAST, "", "");
        }
    }
}

void LogicInfo::onRemoveOther(Building& building)
{
    const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(building.m_buildingid);
    if (building.m_buildingid / 10000 == 6) {
        if(pInfo->removecost > LogicInfo::getSingleton().getMoney(MONEY_GEM))
        {
            MessageUI::show(MSG_NOGEM, NULL, NULL);
            return;
        }
        
        if (pInfo->removecost > 0) {
            
            MessageUI::show(MSG_REMOVE_ORNAMENT,
                            CCStringFormat(ViewString::getSingleton().getValue("remove_ornament_title").c_str(), pInfo->name.c_str()).c_str(),
                            CCStringFormat(ViewString::getSingleton().getValue("remove_ornament_message").c_str(),
                                           pInfo->name.c_str(), (int)pInfo->removecost).c_str(), pInfo->removecost,
                            GETGEM_REMOVE_BUILDING);
        }
        else {
            int need = 0;
            int ret = LogicInfo::getSingleton().removeOrnament(*PlayerScene::getSingleton().m_pCurBuilding, need);
            if(ret <= 0)
            {
                // msg
                MessageUI::show(ret, "", "", need, GETGEM_REMOVE_BUILDING);
            }
        }
    }
    else {
        if(pInfo->removecost > LogicInfo::getSingleton().getMoney(pInfo->buildres))
        {
            char message[512];
            int money = pInfo->removecost - LogicInfo::getSingleton().getMoney(pInfo->buildres);
            
            LogicInfo::getSingleton().m_pGetGemNums = money;
            
            if(pInfo->buildres == MONEY_GOLD)
            {
                sprintf(message, ViewString::getSingleton().getValue("msg_nogold_message").c_str(), money);
                MessageUI::show(MSG_NOGOLD, ViewString::getSingleton().getValue("msg_nogold_title").c_str(), message, money, GETGEM_REMOVE_BUILDING);
            }
            else if(pInfo->buildres == MONEY_ELIXIR)
            {
                sprintf(message, ViewString::getSingleton().getValue("msg_nooil_message").c_str(), money);
                MessageUI::show(MSG_NOELIXIR, ViewString::getSingleton().getValue("msg_nooil_title").c_str(), message, money, GETGEM_REMOVE_BUILDING);
            }
            return;
        }
        int need = 0;
        int ret = LogicInfo::getSingleton().removeBuilding(building, need);
        if(ret <= 0)
        {
            MessageUI::show(ret, "", "", need, GETGEM_REMOVE_BUILDING);
        }
    }
}

void LogicInfo::harvest(Building& building)
{
	if(building.hasNewRes())
	{
		if(building.m_info.producesres == MONEY_GOLD || building.m_info.producesres == MONEY_ELIXIR)
		{
			int64_t res = building.m_curResTime * building.m_info.producesperhour / (60 * 60);
			if(res > building.m_info.maxstored[building.m_info.producesres - 1])
				res = building.m_info.maxstored[building.m_info.producesres - 1];
            
			int fullnum;
            
            if(building.m_info.producesres == MONEY_GOLD)
            {
                if (m_myInfo.goldex >= m_myInfo.maxgoldex)
				{
                    PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI->addNote(ViewString::getSingleton().getValue("gold_storages_full_tip").c_str());
					fullnum = 0;
                }
				else
				{
					fullnum = m_myInfo.maxgoldex - m_myInfo.goldex;
				}
            }
            else if (building.m_info.producesres == MONEY_ELIXIR)
            {
                if (m_myInfo.oilex >= m_myInfo.maxoilex)
				{
                    PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI->addNote(ViewString::getSingleton().getValue("elixir_storages_full_tip").c_str());
					fullnum = 0;
                }
				else
				{
					fullnum = m_myInfo.maxoilex - m_myInfo.oilex;
				}
            }
            
			int asec;
            
			if(res > fullnum)
			{
				asec = (fullnum * (60 * 60)) / building.m_info.producesperhour;
                
				if((fullnum * (60 * 60)) % building.m_info.producesperhour != 0)
				{
					asec += 1;
				}
                
				res = asec * building.m_info.producesperhour / (60 * 60);
			}
			else
			{
				asec = building.m_curResTime;
			}
            
			if(res != 0)
			{
				chgMoney(building.m_info.producesres, res);
                
				HttpClient::getSingleton().harvest(building.m_pBuildingInfo->id, building.getLogicX(), building.getLogicY(), asec);
                
				building.m_pBuildingInfo->curtime += asec;
				building.m_pBuildingInfo->currestime -= asec;
				building.m_curResTime -= asec;
                
                if (building.m_pBuildingInfo->currestime < 0) {
                    building.m_pBuildingInfo->currestime = 0;
                }
			}
		}
        
		if(&building == PlayerScene::getSingleton().m_pCurBuilding)
			PlayerScene::getSingleton().onBuildingSelect(true, &building);
	}
}

int LogicInfo::upLevel(Building& building, int& need)
{
	if(!building.canBuild())
		return MSG_POSERR;
    
	need = 0;
    
	if(building.m_pBuildingInfo != NULL)
	{
		if(!hasFreeWorker())
		{
			return MSG_NOWORKER;
		}
        
		const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(building.m_buildingid + 1);
		if(pInfo == NULL)
			return MSG_ERROR;
        
		if(pInfo->buildcost > getMoney(pInfo->buildres))
		{
			need = pInfo->buildcost - getMoney(pInfo->buildres);
            
			return MSG_NOGEM - pInfo->buildres;
		}
        
		chgMoney(pInfo->buildres, -pInfo->buildcost);
        
		HttpClient::getSingleton().levelupBuilding(building.m_pBuildingInfo->id, building.m_pBuildingInfo->buildingid, building.m_pBuildingInfo->x, building.m_pBuildingInfo->y);
        
		building.m_pBuildingInfo->buildingid = pInfo->buildingid;
		building.m_pBuildingInfo->lasttime = ((pInfo->buildtime[0] * 24 + pInfo->buildtime[1]) * 60 + pInfo->buildtime[2]) * 60;
		building.m_pBuildingInfo->curtime = getCurrentTime();// time(NULL);
        
		if(building.m_pBuildingInfo->lasttime == 0)
		{
			building.m_pBuildingInfo->state = 2;
            
			int bid = pInfo->buildingid / 100;
			if(!(bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_GOLD_MINE))
			{
				building.m_pBuildingInfo->maxstore[0] = pInfo->maxstored[0];
				building.m_pBuildingInfo->maxstore[1] = pInfo->maxstored[1];
			}
		}
		else
			building.m_pBuildingInfo->state = 1;
        
		if(pInfo->producesres > 0)
			building.m_pBuildingInfo->maxres = pInfo->maxstored[pInfo->producesres - 1];
		else
			building.m_pBuildingInfo->maxres = 0;
        
        if (building.m_pBuildingInfo->barrackid >= 0) {
            onAddBarrack(*building.m_pBuildingInfo, pInfo);
        }
        
        //		building.m_pBuildingInfo->currestime = 0;
		building.m_pBuildingInfo->timeres = pInfo->producesperhour;
        
		UserBuildingInfo* pUBI = building.m_pBuildingInfo;
        
		Building* pBuilding = PlayerScene::getSingleton().onUpdateBuilding(pUBI);
        
        if (pBuilding) {
                    
            PlayerScene::getSingleton().onBuildingSelect(true, pBuilding);
        }
                
		refurbishBaseInfo();
        
		return 1;
	}
    
	return MSG_NOWORKER;
}

int LogicInfo::upgradeWalls(vector<Building_Wall*> lstUpgradeWall, int& need)
{
    for (vector<Building_Wall*>::iterator it = lstUpgradeWall.begin(); it != lstUpgradeWall.end(); ++it) {
        
        Building_Wall *pWall = *it;
        
        if(pWall->m_pBuildingInfo == NULL)
            return MSG_NOWORKER;
        
		const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(pWall->m_buildingid + 1);
		if(pInfo == NULL)
			return MSG_ERROR;
    }
    
	need = 0;
	
    if(!hasFreeWorker())
    {
        return MSG_NOWORKER;
    }
    
    int upgradeMoney = PlayerScene::getSingleton().getUpgradeWallsMoney();
    if(upgradeMoney > getMoney(MONEY_GOLD))
    {
        need = upgradeMoney - getMoney(MONEY_GOLD);
        
        return MSG_NOGOLD;
    }
    
    chgMoney(MONEY_GOLD, -upgradeMoney);
    
    HttpClient::getSingleton().upgradeWalls(lstUpgradeWall, 0, upgradeMoney, 0);
    
    Building* pCurBuilding = PlayerScene::getSingleton().m_pCurBuilding;
    int lx = pCurBuilding->getLogicX();
    int ly = pCurBuilding->getLogicY();
    
    for (vector<Building_Wall*>::iterator it = lstUpgradeWall.begin(); it != lstUpgradeWall.end(); ++it) {
        Building_Wall *pWall = *it;
		const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(pWall->m_buildingid + 1);
        
        pWall->m_pBuildingInfo->buildingid = pInfo->buildingid;
        pWall->m_pBuildingInfo->state = 2;
        
        UserBuildingInfo* pUBI = pWall->m_pBuildingInfo;
        Building* pBuilding = PlayerScene::getSingleton().onUpdateBuilding(pUBI);
        
        if (pBuilding->getLogicX() == lx && pBuilding->getLogicY() == ly) {
            pCurBuilding = pBuilding;
        }
    }
    
    PlayerScene::getSingleton().downSelectWallLine();
    
    PlayerScene::getSingleton().m_pCurBuilding = pCurBuilding;
    PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->onSelectRow(NULL, NULL);
    
    refurbishBaseInfo();
    
    return 1;
}

int LogicInfo::upLevelEx(Building& building, int& need)
{
    if(!building.canBuild())
        return MSG_POSERR;
    
    need = 0;
    
    if(building.m_pBuildingInfo != NULL)
    {
        if(!hasFreeWorker())
        {
            return MSG_NOWORKER;
        }
        
        const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(building.m_buildingid + 1);
        if(pInfo == NULL)
            return MSG_ERROR;
        
        if(pInfo->buildcost > getMoney(pInfo->buildres))
        {
            need = pInfo->buildcost - getMoney(pInfo->buildres);
            
            return MSG_NOGEM - pInfo->buildres;
        }
        
        chgMoney(pInfo->buildres, -pInfo->buildcost);
        
        HttpClient::getSingleton().levelupBuilding(building.m_pBuildingInfo->id, building.m_pBuildingInfo->buildingid, building.m_pBuildingInfo->x, building.m_pBuildingInfo->y);
        
        building.m_pBuildingInfo->buildingid = building.m_buildingid; //pInfo->buildingid;
        building.m_pBuildingInfo->lasttime = ((pInfo->buildtime[0] * 24 + pInfo->buildtime[1]) * 60 + pInfo->buildtime[2]) * 60;
        building.m_pBuildingInfo->curtime = getCurrentTime();// time(NULL);
        
        if(building.m_pBuildingInfo->lasttime == 0)
        {
            building.m_pBuildingInfo->state = 2;
            
            int bid = pInfo->buildingid / 100;
            if(!(bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_GOLD_MINE))
            {
                building.m_pBuildingInfo->maxstore[0] = pInfo->maxstored[0];
                building.m_pBuildingInfo->maxstore[1] = pInfo->maxstored[1];
            }
        }
        else
            building.m_pBuildingInfo->state = 1;
        
        if(pInfo->producesres > 0)
            building.m_pBuildingInfo->maxres = pInfo->maxstored[pInfo->producesres - 1];
        else
            building.m_pBuildingInfo->maxres = 0;
        
        if (building.m_pBuildingInfo->barrackid >= 0) {
            onAddBarrack(*building.m_pBuildingInfo, pInfo);
        }
        
        //		building.m_pBuildingInfo->currestime = 0;
        building.m_pBuildingInfo->timeres = pInfo->producesperhour;
        
//        UserBuildingInfo* pUBI = building.m_pBuildingInfo;
//        
//        Building* pBuilding = PlayerScene::getSingleton().onUpdateBuilding(pUBI);
//        if (pBuilding) {
//            PlayerScene::getSingleton().onBuildingSelect(true, &building);
//        }
        
        PlayerScene::getSingleton().onBuildingSelect(true, &building);
        
        refurbishBaseInfo();
        
        return 1;
    }
    
    return MSG_NOWORKER;
}

void LogicInfo::chgGold(int off, int from)
{
	if(off == 0)
		return;
    
	m_myInfo.goldex += off;
    
	if(m_myInfo.goldex >= m_myInfo.maxgoldex)
		m_myInfo.goldex = m_myInfo.maxgoldex;
	else if(m_myInfo.goldex < 0)
		m_myInfo.goldex = 0;
    
    if (isNormalMode()) {
     	LogicInfo::getSingleton().fixStorageReserve(true);
    }
    
    onNotify(NOTIFY_BASEINFO);
}

void LogicInfo::chgOil(int off, int from)
{
	if(off == 0)
		return;
    
	m_myInfo.oilex += off;
    
	if(m_myInfo.oilex >= m_myInfo.maxoilex)
		m_myInfo.oilex = m_myInfo.maxoilex;
	else if(m_myInfo.oilex < 0)
		m_myInfo.oilex = 0;
    
	LogicInfo::getSingleton().fixStorageReserve(true);
    
    onNotify(NOTIFY_BASEINFO);
}

void LogicInfo::chgGem(int off, int from)
{
	if(off < 0 && m_myInfo.gemex < abs(off))
		return;
    
    if (off > 0) {
        MessageUI::show(MSG_ADD_GEM, ViewString::getValue("get_gem_title").c_str(), ViewString::getValue("get_gem_message").c_str(), off, -1);
    }
    else {
        m_myInfo.gemex += off;
        
        onNotify(NOTIFY_BASEINFO);
    }
}

void LogicInfo::addGem(int off)
{
    if(off < 0 && m_myInfo.gemex < abs(off))
		return;
    
	m_myInfo.gemex += off;
    
    onNotify(NOTIFY_BASEINFO);
    
    SoundMgr::getSingleton().playEffect("get_gems.mp3", 0);
}

void LogicInfo::chgMoney(int moneytype, int off, int from)
{
	if(moneytype == MONEY_GOLD)
		chgGold(off, from);
	else if(moneytype == MONEY_ELIXIR)
		chgOil(off, from);
	else if(moneytype == MONEY_GEM)
		chgGem(off, from);
}

int LogicInfo::getMoney(int moneytype)
{
	if(moneytype == MONEY_GOLD)
		return m_myInfo.goldex;
	else if(moneytype == MONEY_ELIXIR)
		return m_myInfo.oilex;
	else if(moneytype == MONEY_GEM)
		return m_myInfo.gemex;
    
	return 0;
}

void LogicInfo::onNotify(int notifyid, const void* pVal)
{
	m_isProcListener = true;
	bool noproc = false;
    
	for(ListNotifyListener::iterator it = m_lstNotifyListener.begin(); it != m_lstNotifyListener.end(); )
	{
		if(!noproc && !it->isRelease)
		{
			bool ret = it->pListener->onNotify(notifyid, pVal);
			if(!ret)
				noproc = true;
		}
        
		if(it->isRelease)
			it = m_lstNotifyListener.erase(it);
		else
			++it;
	}
    
    if(notifyid == NOTIFY_NETWORK_FAIL) {
        MessageUI::show(MSG_NETWORK_FAIL, "Disconnected", "Unable to connect to the network. Try again in a place with a better signal.", -1);
    }
    else if (notifyid == NOTIFY_NETWORK_ERROR) {
        MessageUI::show(MSG_NETWORK_FAIL, "Disconnected", (const char*)pVal, -1);
    }
    else if (NOTIFY_NEW_USERLOGIN == notifyid)
    {
        string title = "";
        string message = "";
        
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        title = CCStringFormat(ViewString::getValue("new_user_login_title"), "Game Center");
        message = CCStringFormat(ViewString::getValue("new_user_login_message"), "Game Center");
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        title = CCStringFormat(ViewString::getValue("new_user_login_title"), "Google Mail");
        message = CCStringFormat(ViewString::getValue("new_user_login_message"), "Google Mail");
#endif
        
        if(m_bindType == BIND_FACEBOOK_TYPE)
        {
            title = CCStringFormat(ViewString::getValue("user_login_change_title"), "Facebook");
            message = CCStringFormat(ViewString::getValue("new_user_login_message"), "Facebook");
        }
        
        MessageUI::show(MSG_USERID_LOGIN, title.c_str(), message.c_str());
    }
    else if (NOTIFY_USERLOGIN_INFO == notifyid)
    {
        string title = "";
        string message = "";
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        title = CCStringFormat(ViewString::getValue("user_login_change_title"), "Game Center");
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        title = CCStringFormat(ViewString::getValue("user_login_change_title"), "Google Mail");
#endif
        if(m_bindType == BIND_FACEBOOK_TYPE)
        {
            title = CCStringFormat(ViewString::getValue("user_login_change_title"), "Facebook");
        }

        message = CCStringFormat(ViewString::getValue("user_login_change_message"), LogicInfo::getSingleton().m_unameGameCenterBind.c_str(), LogicInfo::getSingleton().m_townlevelGameCenterBind);
        MessageUI::show(MSG_USERID_CHANGED, title.c_str(), message.c_str());
    }
    
	m_isProcListener = false;
}

void LogicInfo::addListener(NotifyListener* pListener)
{
	for(ListNotifyListener::iterator it = m_lstNotifyListener.begin(); it != m_lstNotifyListener.end(); )
	{
		if(it->pListener == pListener)
		{
			it = m_lstNotifyListener.erase(it);
		}
		else
			++it;
	}
    
	_ListenerNode node;
    
	node.isRelease = false;
	node.pListener = pListener;
    
	m_lstNotifyListener.push_back(node);
}

void LogicInfo::removeListener(NotifyListener* pListener)
{
	if(m_isProcListener)
	{
		for(ListNotifyListener::iterator it = m_lstNotifyListener.begin(); it != m_lstNotifyListener.end(); ++it)
		{
			if(it->pListener == pListener)
			{
				it->isRelease = true;
			}
		}
	}
	else
	{
		for(ListNotifyListener::iterator it = m_lstNotifyListener.begin(); it != m_lstNotifyListener.end(); )
		{
			if(it->pListener == pListener)
			{
				it = m_lstNotifyListener.erase(it);
			}
			else
				++it;
		}
	}
}

bool LogicInfo::hasFreeWorker() const
{
	return m_myInfo.buildbuildingnumsex < m_myInfo.totalworkernumsex;
}

int LogicInfo::buildBuilding(Building& building, int& need)
{
	need = 0;
	if(!building.canBuild())
		return MSG_ERROR;
    
    if ((building.m_buildingid / 10000) == 9) {
        
        building.setTime(0, building.m_pBuildingInfo->lasttime);
        
        HttpClient::getSingleton().build(building.m_pBuildingInfo->id, building.m_buildingid, building.getLogicX(), building.getLogicY(), building.m_pBuildingInfo->state);
    }
    else {
        
        if(!hasFreeWorker())
            return MSG_NOWORKER;
        
        int bid = building.m_buildingid / 100;
        int cost = building.m_info.buildcost;
        if(bid == BUILDING_BUILDER_HOME)
            cost = countCurWorkerBuildingPrice();
        
        if(cost > getMoney(building.m_info.buildres))
        {
            need = cost - getMoney(building.m_info.buildres);
            
            return  MSG_NOGEM - building.m_info.buildres;
        }
        
        //addExp(building.m_info.exp);
        
        chgMoney(building.m_info.buildres, -cost);
        
        if(building.m_info.buildtime[0] == 0 && building.m_info.buildtime[1] == 0 && building.m_info.buildtime[2] == 0)
        {
            building.m_pBuildingInfo->state = BUILDING_STATE_NORMAL;
            
            const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(building.m_buildingid);
            if(pInfo == NULL)
                return MSG_ERROR;
            
            int bid = pInfo->buildingid / 100;
            if(!(bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_GOLD_MINE))
            {
                building.m_pBuildingInfo->maxstore[0] = pInfo->maxstored[0];
                building.m_pBuildingInfo->maxstore[1] = pInfo->maxstored[1];
            }
            
            onBuildingComplete(building);
            //m_myInfo.buildbuildingnums++;
        }
        else
        {
            building.m_pBuildingInfo->state = BUILDING_STATE_BUILD;
            
            int fulltime = ((building.m_info.buildtime[0] * 24 + building.m_info.buildtime[1]) * 60 + building.m_info.buildtime[2]) * 60;
            
            building.setTime(fulltime, building.m_pBuildingInfo->lasttime);
        }
        
        HttpClient::getSingleton().build(building.m_pBuildingInfo->id, building.m_buildingid, building.getLogicX(), building.getLogicY(), building.m_pBuildingInfo->state);
                
        refurbishBaseInfo();
        
        PlayerScene::getSingleton().buildCurBuilding();
                
        if(building.m_buildingid / 100 != BUILDING_WALL)
            PlayerScene::getSingleton().selectBuilding(building);
        
        SoundMgr::getSingleton().playEffect("building_uplevel.mp3");
    }
    
	return 1;
}

Building* LogicInfo::addBuilding(int buildingid, int lx, int ly)
{
	const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(buildingid);
	if(pInfo != NULL)
	{
		UserBuildingInfo* pbi = new UserBuildingInfo;
        
		pbi->buildingid = buildingid;
		pbi->state = BUILDING_STATE_BUILD;
		pbi->lasttime = 0;
		pbi->curtime = 0;
		pbi->x = lx;
		pbi->y = ly;
        pbi->hp = pInfo->hp;
        
		if(pInfo->producesres > 0)
			pbi->maxres = pInfo->maxstored[pInfo->producesres - 1];
		else
			pbi->maxres = 0;
        
		pbi->currestime = 0;
		pbi->timeres = pInfo->producesperhour;
        
		pbi->store[0] = 0;
		pbi->store[1] = 0;
		pbi->maxstore[0] = 0;
		pbi->maxstore[1] = 0;
        
		m_lstBuilding.push_back(pbi);
        
		Building* pBuilding = NULL;
		if(lx < 0 || ly < 0)
        {
            pbi->state = BUILDING_STATE_NONE;
			pBuilding = PlayerScene::getSingleton().addBuildingEx(buildingid, pbi);
        }
		else
			pBuilding = PlayerScene::getSingleton().addBuildingEx(buildingid, lx, ly, pbi);
        
        onAddBuilding(*pbi);
        
        return pBuilding;
	}
    
	onRefurbishTownhall();
    
    return NULL;
}

void LogicInfo::finishBuilding(Building& building)
{
	if(building.m_pBuildingInfo == NULL)
		return;
    
	const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(building.m_buildingid);
	if(pInfo == NULL)
		return;
    
    int bid = pInfo->buildingid / 100;
    
	building.m_pBuildingInfo->state = 2;
	building.m_pBuildingInfo->lasttime = 0;
	//building.m_pBuildingInfo->currestime = 0;
    
	if(building.m_curResTime < 0)
		building.m_curResTime = 0;
    
	if(!(bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_GOLD_MINE))
	{
		building.m_pBuildingInfo->maxstore[0] = pInfo->maxstored[0];
		building.m_pBuildingInfo->maxstore[1] = pInfo->maxstored[1];
	}
    
	building.setTime(0, building.m_pBuildingInfo->lasttime);
    
	building.onRefurbish();
    
    building.m_pBuildingInfo->level += 1;
    
	HttpClient::getSingleton().updateBuilding(building.m_pBuildingInfo->id, building.m_buildingid, building.getLogicX(), building.getLogicY(), building.m_pBuildingInfo->level, 0);
    
	refurbishBaseInfo();
    
	onBuildingComplete(building);
    
	if(PlayerScene::getSingleton().m_pCurBuilding == &building)
		PlayerScene::getSingleton().onBuildingSelect(true, &building);
    
    SoundMgr::getSingleton().playEffect("building_uplevel.mp3");
    
	if(building.m_buildingid == m_guideBuildingID)
		onNotify(NOTIFY_GUIDE_REFRESH);
    
    if (PlayerScene::getSingleton().m_pGameWorld->m_pMainUI) {
        if (GuideMgr::getSingleton().m_pActiveBtn != PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->m_pBtnFast) {
            GuideMgr::getSingleton().next(); //skip fast build
        }
    }
    
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next(); //end build
    }
    
	if(building.m_buildingid >= 90000)
		onRemoveOtherOK(building.m_pBuildingInfo->x, building.m_pBuildingInfo->y);
    
    if(bid == BUILDING_TOWN_HALL)
    {
        FacebookMgr::getSingleton().m_shareFreeGem = true;
        onNotify(NOTIFY_FB_SHARE_FINISH);
    }
}

void LogicInfo::finishBuildingEx(Building& building)
{
    if(building.m_pBuildingInfo == NULL)
        return;
    
    const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(building.m_buildingid);
    if(pInfo == NULL)
        return;
    
    int bid = pInfo->buildingid / 100;
    
    building.m_pBuildingInfo->buildingid += 1;
    building.m_pBuildingInfo->state = 2;
    building.m_pBuildingInfo->lasttime = 0;
    //building.m_pBuildingInfo->currestime = 0;
    
    if(building.m_curResTime < 0)
        building.m_curResTime = 0;
    
    if(!(bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_GOLD_MINE))
    {
        building.m_pBuildingInfo->maxstore[0] = pInfo->maxstored[0];
        building.m_pBuildingInfo->maxstore[1] = pInfo->maxstored[1];
    }
    
    building.setTime(0, building.m_pBuildingInfo->lasttime);
    
    building.onRefurbish();
    
    building.m_pBuildingInfo->level += 1;
    
    HttpClient::getSingleton().updateBuilding(building.m_pBuildingInfo->id, building.m_buildingid, building.getLogicX(), building.getLogicY(), building.m_pBuildingInfo->level, 0);
    
    refurbishBaseInfo();
    
    onBuildingComplete(building);
    
    if(PlayerScene::getSingleton().m_pCurBuilding == &building)
        PlayerScene::getSingleton().onBuildingSelect(true, &building);
    
    SoundMgr::getSingleton().playEffect("building_uplevel.mp3");
    
    if(building.m_buildingid == m_guideBuildingID)
        onNotify(NOTIFY_GUIDE_REFRESH);
    
    if (PlayerScene::getSingleton().m_pGameWorld->m_pMainUI) {
        if (GuideMgr::getSingleton().m_pActiveBtn != PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->m_pBtnFast) {
            GuideMgr::getSingleton().next(); //skip fast build
        }
    }
    
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next(); //end build
    }
    
    if(building.m_buildingid >= 90000)
        onRemoveOtherOK(building.m_pBuildingInfo->x, building.m_pBuildingInfo->y);
    
    if(bid == BUILDING_TOWN_HALL)
    {
        FacebookMgr::getSingleton().m_shareFreeGem = true;
        onNotify(NOTIFY_BASEINFO);
    }
    
    PlayerScene::getSingleton().onUpdateBuilding(building.m_pBuildingInfo);

    refurbishBaseInfo();
}

void LogicInfo::cancelBuild(Building& building)
{
	if(building.m_pBuildingInfo != NULL)
	{
		if(building.m_pBuildingInfo->state == 2)
			return ;
        
		bool isRelease = false;
        
		const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(building.m_buildingid - 1);
		if(pInfo == NULL)
		{
			//return ;
			pInfo = BuildingMgr::getSingleton().getBuildingInfo(building.m_buildingid);
			if(pInfo == NULL)
				return ;
            
			isRelease = true;
		}
        
		chgMoney(building.m_info.buildres, building.m_info.buildcost / 2);
        
		if(isRelease)
		{
			HttpClient::getSingleton().removeBuilding(building.m_pBuildingInfo->id, building.m_pBuildingInfo->buildingid, building.m_pBuildingInfo->x, building.m_pBuildingInfo->y);
            
			for(std::list<UserBuildingInfo*>::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
			{
				if(*it == building.m_pBuildingInfo)
				{
					m_lstBuilding.erase(it);
                    
					break;
				}
			}
            
			PlayerScene::getSingleton().onRemoveBuilding(building.m_pBuildingInfo);
		}
		else
		{
			HttpClient::getSingleton().cancelLevelupBuilding(building.m_pBuildingInfo->buildingid, building.m_pBuildingInfo->x, building.m_pBuildingInfo->y);
            
			building.m_pBuildingInfo->buildingid = pInfo->buildingid;
			building.m_pBuildingInfo->lasttime = 0;
			building.m_pBuildingInfo->curtime = getCurrentTime();// time(NULL);
            
			if(building.m_pBuildingInfo->lasttime == 0)
				building.m_pBuildingInfo->state = 2;
			else
				building.m_pBuildingInfo->state = 1;
            
			if(pInfo->producesres > 0)
				building.m_pBuildingInfo->maxres = pInfo->maxstored[pInfo->producesres - 1];
			else
				building.m_pBuildingInfo->maxres = 0;
            
            if (building.m_pBuildingInfo->barrackid >= 0) {
                onAddBarrack(*building.m_pBuildingInfo, pInfo);
            }
            
			//building.m_pBuildingInfo->currestime = 0;
			building.m_pBuildingInfo->timeres = pInfo->producesperhour;
            
            Building* pBuilding = PlayerScene::getSingleton().onUpdateBuilding(building.m_pBuildingInfo);
            
            if (pBuilding) {
                PlayerScene::getSingleton().onBuildingSelect(true, pBuilding);
            }
		}
        
		refurbishBaseInfo();
        
		onRefurbishTownhall();
	}
}

bool LogicInfo::fastBuilding(Building& building)
{
	if(building.m_pBuildingInfo != NULL)
	{
		const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(building.m_buildingid);
		if(pInfo == NULL)
			return false;
        
		int gem = building.m_lasttime / 240;
		if(gem < 1)
			gem = 1;
        
		int lasttime = building.m_lasttime;
        
		if(m_myInfo.gemex < gem)
        {
            // msg
            MessageUI::show(MSG_NOGEM, "", "", gem - m_myInfo.gemex, -1);
            
            return false;
        }
		chgGem(-gem);
        
		//HttpClient::getSingleton().subMoney(gem, 0, 0);
        
		building.m_pBuildingInfo->state = 2;
		building.m_pBuildingInfo->lasttime = 0;
		//building.m_pBuildingInfo->currestime = 0;
		//building.m_curResTime = 0;
		if(building.m_curResTime < 0)
			building.m_curResTime = 0;
        
		int bid = pInfo->buildingid / 100;
		if(!(bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_GOLD_MINE))
		{
			building.m_pBuildingInfo->maxstore[0] = pInfo->maxstored[0];
			building.m_pBuildingInfo->maxstore[1] = pInfo->maxstored[1];
		}
        
		building.setTime(0, building.m_pBuildingInfo->lasttime);
        
		building.onRefurbish();
        
        building.m_pBuildingInfo->level += 1;
        
		HttpClient::getSingleton().fastBuilding(building.m_pBuildingInfo->id, building.m_buildingid, building.getLogicX(), building.getLogicY(), building.m_pBuildingInfo->level, lasttime);
        
		onBuildingComplete(building);
        
		refurbishBaseInfo();
		//chgMoney(building.m_info.buildres, building.m_info.buildcost);
        
        if(PlayerScene::getSingleton().m_pCurBuilding == &building)
            PlayerScene::getSingleton().onBuildingSelect(true, &building);
        
		if(building.m_buildingid == m_guideBuildingID)
			onNotify(NOTIFY_GUIDE_REFRESH);
                
		if(building.m_buildingid >= 90000)
			onRemoveOtherOK(building.m_pBuildingInfo->x, building.m_pBuildingInfo->y);
		else
			SoundMgr::getSingleton().playEffect("building_uplevel.mp3");
        
        
        if(bid == BUILDING_TOWN_HALL)
        {
            FacebookMgr::getSingleton().m_shareFreeGem = true;
            onNotify(NOTIFY_FB_SHARE_FINISH);
        }
        
        return true;
	}
    return false;
}

void LogicInfo::onAddBuilding(UserBuildingInfo& ubi)
{
    const BuildingInfo* pPreInfo = BuildingMgr::getSingleton().getBuildingInfo(ubi.buildingid-1);
	const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(ubi.buildingid);
	if(pInfo == NULL)
		return;
    
	if(pInfo->producesres > 0)
		ubi.maxres = pInfo->maxstored[pInfo->producesres - 1];
	else
		ubi.maxres = 0;
    
	ubi.timeres = pInfo->producesperhour;
    ubi.maxhp = pInfo->hp;
    
    if (m_lastBeAttackTime <= m_lastLoginTime) {
        ubi.hp = ubi.maxhp;
    }
    
    ubi.maxstore[0] = pInfo->maxstored[0];
    ubi.maxstore[1] = pInfo->maxstored[1];
    
	if(ubi.buildingid >= 30200 && ubi.buildingid <= 30299)
	{
		int level = ubi.buildingid % 100;
		if(level > m_myInfo.maxBarrackLevel)
			m_myInfo.maxBarrackLevel = level;
        
		ubi.barrackid = m_mapBarrack.size();
	}
	else
		ubi.barrackid = -1;
    
	if(ubi.buildingid >= 30100 && ubi.buildingid <= 30199)
		ubi.troophousingid = m_mapTroophousing.size();
	else
		ubi.troophousingid = -1;
    
	if(ubi.buildingid >= 20200 && ubi.buildingid <= 20299 && ubi.state == 2)
	{
		m_myInfo.totalworkernumsex += 1;
	}
    
    int bid = ubi.buildingid / 100;
    
	if(bid == BUILDING_GOLD_MINE || bid == BUILDING_ELIXIR_COLLECTOR)
	{
		if(ubi.currestime < 0)
			ubi.currestime = 0;
	}
    
    if(ubi.barrackid >= 0)
    {
        if (ubi.state != -1 && ubi.state != BUILDING_STATE_BUILD) {
            onAddBarrack(ubi, pInfo);
        }
    }
    else if(ubi.troophousingid >= 0)
    {
        if (ubi.state == 1 && pPreInfo != NULL) {
            onAddTroophousing(ubi.troophousingid, pPreInfo->housingspace);
        }
        else if(ubi.state == 2){
            onAddTroophousing(ubi.troophousingid, pInfo->housingspace);
        }
    }
    
	onNotify(NOTIFY_BASEINFO);
}

void LogicInfo::onAddOtherBuilding(UserBuildingInfo& ubi)
{
	const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(ubi.buildingid);
	if(pInfo == NULL)
		return;
    
	if(pInfo->producesres > 0)
		ubi.maxres = pInfo->maxstored[pInfo->producesres - 1];
	else
		ubi.maxres = 0;
    
	ubi.timeres = pInfo->producesperhour;
    
    ubi.hp = pInfo->hp;
    ubi.maxhp = pInfo->hp;
    
    ubi.store[0] = 0;
    ubi.laststore[0] = 0;
    ubi.store[1] = 0;
    ubi.laststore[1] = 0;
    
    ubi.maxstore[0] = pInfo->maxstored[0];
    ubi.maxstore[1] = pInfo->maxstored[1];
    
	int bid = pInfo->buildingid / 100;
	if(bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_GOLD_MINE)
	{
        int res1 = ubi.store[0];
        int res2 = ubi.store[1];
        
        ubi.store[0] = res1;
        ubi.laststore[0] = ubi.store[0];
        ubi.store[1] = res2;
        ubi.laststore[1] = ubi.store[1];
        
        m_otherInfo.goldex += ubi.store[0];
        m_otherInfo.oilex += ubi.store[1];
	}
    
	if(bid == BUILDING_BARRACKS)
	{
		int level = ubi.buildingid % 100;
		if(level > m_otherInfo.maxBarrackLevel)
			m_otherInfo.maxBarrackLevel = level;
	}
    
	if(ubi.buildingid >= 30100 && ubi.buildingid <= 30199)
		ubi.troophousingid = m_mapOtherTroophousing.size();
	else
		ubi.troophousingid = -1;
    
	if(ubi.troophousingid >= 0)
		onAddOtherTroophousing(ubi.troophousingid, pInfo->housingspace);
}

bool LogicInfo::getGoldEx(int gem)
{
	int gold = countGem2Other(gem);
	//int gem = gold / 100;
	if(m_myInfo.gemex >= gem)
	{
		m_myInfo.gemex -= gem;
        
		chgGold(gold);
		//m_myInfo.gold += gold;
        
		if(m_myInfo.goldex >= m_myInfo.maxgoldex)
			m_myInfo.goldex = m_myInfo.maxgoldex;
		else if(m_myInfo.goldex < 0)
			m_myInfo.goldex = 0;
        
		onNotify(NOTIFY_BASEINFO);
        
		HttpClient::getSingleton().getGold(gem);
        
        return true;
	}
    else
    {
        MessageUI::show(MSG_NOGEM, "", "", gem-m_myInfo.gemex, -1);
    }
    
    return false;
}

bool LogicInfo::getOilEx(int gem)
{
	int oil = countGem2Other(gem);
	//int gem = oil / 100;
	if(m_myInfo.gemex >= gem)
	{
		m_myInfo.gemex -= gem;
		chgOil(oil);
		//m_myInfo.oil += oil;
        
		if(m_myInfo.oilex >= m_myInfo.maxoilex)
			m_myInfo.oilex = m_myInfo.maxoilex;
		else if(m_myInfo.oilex < 0)
			m_myInfo.oilex = 0;
        
		onNotify(NOTIFY_BASEINFO);
        
		HttpClient::getSingleton().getOil(gem);
        
        return true;
	}
    else
    {
        MessageUI::show(MSG_NOGEM, "", "", gem-m_myInfo.gemex, -1);
    }
    
    return false;
}

void LogicInfo::refurbishBaseInfo()
{
	m_myInfo.buildbuildingnumsex = 0;
	m_myInfo.maxgoldex = 0;
	m_myInfo.maxoilex = 0;
    
	for(std::list<UserBuildingInfo*>::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if((*it)->state == 0 || (*it)->state == 1)
			m_myInfo.buildbuildingnumsex += 1;
        
		int bid = (*it)->buildingid / 100;
		if(bid == BUILDING_TOWN_HALL || bid == BUILDING_ELIXIR_STORAGE || bid == BUILDING_GOLD_STORAGE)
		{
			if((*it)->state == 2)
			{
				const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo((*it)->buildingid);
				if(pInfo != NULL)
				{
					m_myInfo.maxgoldex += (*it)->maxstore[0];
					m_myInfo.maxoilex += (*it)->maxstore[1];
				}
			}
			else
			{
				const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo((*it)->buildingid - 1);
				if(pInfo != NULL)
				{
					m_myInfo.maxgoldex += (*it)->maxstore[0];
					m_myInfo.maxoilex += (*it)->maxstore[1];
				}
			}
		}
	}
    
    onRefurbishTownhall();
    
	onNotify(NOTIFY_BASEINFO);
}

BarrackInfo* LogicInfo::getBarrackInfo(int barrackid)
{
	MapBarrackInfo::iterator it = m_mapBarrack.find(barrackid);
	if(it != m_mapBarrack.end())
	{
		return &(it->second);
	}
    
	return NULL;
}

void LogicInfo::refurbishFightSoldierInfo()
{
	m_lstUsedFightSoldier.clear();
	m_lstFightSoldier.clear();
    ClanMgr::getSingleton().resetClanFightSoldier();
    
	m_lastFightSoldierNums = 0;
    
	for(MapTroophousingInfo::iterator it = m_mapTroophousing.begin(); it != m_mapTroophousing.end(); ++it)
	{
		for(std::list<TroophousingSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
		{
			if(it1->numsex > 0)
			{
				addFightSoldierInfo(it1->pid, it1->numsex);
                
				m_lastFightSoldierNums += it1->numsex;
			}
		}
	}
    
    m_lstFightSoldier.sort();
    
    if ((m_lastFightSoldierNums > 0 || HeroMgr::getSingleton().isFullHP()) && HeroMgr::getSingleton().m_heroState != HERO_STATE_UPGRADING) {
        addFightSoldierInfo(HeroMgr::getSingleton().m_myHero.heroid, 1);
        
        if (HeroMgr::getSingleton().isFullHP()) {
            ++m_lastFightSoldierNums;
        }
    }
}

void LogicInfo::refurbishOtherFightSoldierInfo()
{
	m_lstOtherFightSoldier.clear();
    
	for(MapTroophousingInfo::iterator it = m_mapOtherTroophousing.begin(); it != m_mapOtherTroophousing.end(); ++it)
	{
		for(std::list<TroophousingSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
		{
			if(it1->numsex > 0)
			{
				addOtherFightSoldierInfo(it1->pid, it1->numsex);
			}
		}
	}
    
	m_lstOtherFightSoldier.sort();
}

void LogicInfo::addOtherFightSoldierInfo(int pid, int nums)
{
	if(!canRecruitSoldier(pid, true))
		return ;
    
	for(std::list<FightSoldierInfoEx>::iterator it = m_lstOtherFightSoldier.begin(); it != m_lstOtherFightSoldier.end(); ++it)
	{
		if(it->pid == pid)
		{
			it->numsex += nums;
            
			return ;
		}
	}
    
	FightSoldierInfoEx fsi;
    
	fsi.pid = pid;
	fsi.numsex = nums;
    
	m_lstOtherFightSoldier.push_back(fsi);
}

void LogicInfo::addFightSoldierInfo(int pid, int nums)
{
	if(!canRecruitSoldier(pid, false))
		return;
    
	for(std::list<FightSoldierInfoEx>::iterator it = m_lstFightSoldier.begin(); it != m_lstFightSoldier.end(); ++it)
	{
		if(it->pid == pid)
		{
			it->numsex += nums;
            
			return;
		}
	}
    
	FightSoldierInfoEx fsi;
    
	fsi.pid = pid;
	fsi.numsex = nums;
    
	m_lstFightSoldier.push_back(fsi);
}

void LogicInfo::onBarrackFinish()
{
    for(MapBarrackInfo::iterator it = m_mapBarrack.begin(); it != m_mapBarrack.end(); ++it)
	{
        if (it->second.ischg) {
            HttpClient::getSingleton().updateBarrackInfo(it->second);
        }
	}
    
	for(MapTroophousingInfo::iterator it = m_mapTroophousing.begin(); it != m_mapTroophousing.end(); ++it)
	{
		if(it->second.ischg)
        {
			HttpClient::getSingleton().updateTroophousingInfo(it->second);
        }
	}
}

void LogicInfo::setSoldier2Fight(int pid)
{
	PlayerScene::getSingleton().m_bSelClanSoldier = false;
	PlayerScene::getSingleton().m_curSelPersonID = pid;
}

void LogicInfo::setClanSoldier2Fight()
{
    PlayerScene::getSingleton().m_curSelPersonID = -1;
	PlayerScene::getSingleton().m_bSelClanSoldier = true;
}

void LogicInfo::dropSoldier2Fight(int soldierid, int lx, int ly, int buildingindex)
{
	if(m_isFinishAttack)
		return;
    
	m_isAttackDropPerson = true;
        
    int sid = -1;
    
    if(buildingindex >= 0)
    {
        sid = soldierid;
    }
    else
    {
        for(std::list<FightSoldierInfoEx>::iterator it = m_lstFightSoldier.begin(); it != m_lstFightSoldier.end(); ++it)
        {
            if(it->pid == soldierid && it->numsex > 0)
            {
                sid = soldierid;
                it->numsex -= 1;
                
                delSoldierFromTroophousing(soldierid);
                
                addUsedFightSoldier(sid);
                
                --m_lastFightSoldierNums;
                break;
            }
        }
    }
    
    if(sid > 0)
    {
        if(isHero(sid))
            HeroMgr::getSingleton().m_isHeroInWar = true;
        
        if (buildingindex == -1) {
            onAddCtrl_DropSoldier(sid, lx, ly);
        }
        else {
            onAddCtrl_ClanSoldierAttack(sid, lx, ly);
        }
        
        Person* pPerson = PlayerScene::getSingleton().dropPerson2Attack(soldierid, lx, ly);
        
        if(pPerson != NULL)
        {
            BaseObj* pDestTarget = pPerson->m_pDestTarget;
            
            if(pPerson->m_curCamp == CAMP_ATTACK)
            {
                if(pDestTarget == NULL)
                    onAddCtrl_ChgAtkDest(pPerson->m_indexPerson, -1, 0);
                else
                {
                    if(pDestTarget->getObjType() == OBJTYPE_PERSON)
                        onAddCtrl_ChgAtkDest(pPerson->m_indexPerson, OBJTYPE_PERSON, ((Person*)pDestTarget)->m_indexPerson);
                    else
                        onAddCtrl_ChgAtkDest(pPerson->m_indexPerson, OBJTYPE_BUILDING, ((Building*)pDestTarget)->m_indexBuilding);
                }
            }
            else
            {
                if(pDestTarget == NULL)
                    onAddCtrl_ChgDefDest(pPerson->m_indexPerson, -1, 0);
                else
                {
                    if(pDestTarget->getObjType() == OBJTYPE_PERSON)
                        onAddCtrl_ChgDefDest(pPerson->m_indexPerson, OBJTYPE_PERSON, ((Person*)pDestTarget)->m_indexPerson);
                    else
                        onAddCtrl_ChgDefDest(pPerson->m_indexPerson, OBJTYPE_BUILDING, ((Building*)pDestTarget)->m_indexBuilding);
                }
            }
        }
                
        onNotify(NOTIFY_DROPSOLDIER);
    }
}

void LogicInfo::endBattle()
{
    if (m_isAttackDropPerson)
    {
        onRefurbishAllTroophousing();
        
        m_infoFightScore.numsfsi = m_lstUsedFightSoldier.size();
        int i = 0;
        for(std::list<FightSoldierInfoEx>::iterator it = m_lstUsedFightSoldier.begin(); it != m_lstUsedFightSoldier.end(); ++it)
        {
            m_infoFightScore.fsi[i] = *it;
            ++i;
        }
        
        if (GuideMgr::getSingleton().isGuiding()) {
            
            //教程获取资源满
            
            HttpClient::getSingleton().addMoney(0, LogicInfo::getSingleton().m_otherInfo.maxgoldex, LogicInfo::getSingleton().m_otherInfo.maxoilex);
        }
        else if(m_attackType == ATTACK_TYPE_LEAGUE)
        {
            if (m_curWarStar > 0)
            {
                LeagueMgr::ResInfo info = LeagueMgr::getSingleton().getCurBattleAwardInfo(m_curWarStar);
                int gold, oil = 0;
                for (int i = 0; i < info.types; i++)
                {
                    int type = info.resType[i];
                    int num = info.num[i];
                    if (type == MONEY_GOLD) {
                        gold = num;
                    }
                    else if (type == MONEY_ELIXIR) {
                        oil = num;
                    }
                }
                
                LeagueMgr::getSingleton().m_myGroup.myInfo.star += m_curWarStar;
                LeagueMgr::getSingleton().m_myGroup.myInfo.winNums++;
                HttpClient::getSingleton().addMoney(0, abs(gold), abs(oil));
            }
        }
        else
        {
            HttpClient::getSingleton().addMoney(0, abs(m_infoFightScore.goldex), abs(m_infoFightScore.oilex));
        }
        
        if (m_attackType == ATTACK_TYPE_PVP) {
            
            HttpClient::getSingleton().addBattleLog(m_otherInfo.uid, m_curWarStar, m_myBaseLevel - m_otherBaseLevel, m_infoFightScore, m_logFight, m_warParam, LogicInfo::getSingleton().m_fightUseSkillGems);
        }
        else if (m_attackType == ATTACK_TYPE_PVE)
        {
            HttpClient::getSingleton().addEnvBattleLog(m_pveInfo.curstage, m_curWarStar, m_infoFightScore);
            
            //add pve video
            if (PVE_VIDEO_DEBUG) {
                HttpClient::getSingleton().addAttackEnvVideo(m_pveInfo.curstage, m_otherInfo.uid, m_curWarStar, m_myBaseLevel - m_otherBaseLevel, m_infoFightScore, m_logFight, m_warParam, LogicInfo::getSingleton().m_fightUseSkillGems);
            }
        }
        else if (m_attackType == ATTACK_TYPE_LEAGUE)
        {
            HttpClient::getSingleton().addLeagueBattleLog(m_otherInfo.uid, LeagueMgr::getSingleton().m_leagueid,LeagueMgr::getSingleton().m_myGroup.groupid,m_curWarStar, m_myBaseLevel - m_otherBaseLevel, m_infoFightScore, m_logFight, m_warParam, LogicInfo::getSingleton().m_fightUseSkillGems);
        }
        else if (m_attackType == ATTACK_TYPE_CLANWAR)
        {
            HttpClient::getSingleton().addClanWarBattleLog(m_otherInfo.uid ,m_curWarStar, m_myBaseLevel - m_otherBaseLevel, m_infoFightScore, m_logFight, m_warParam, LogicInfo::getSingleton().m_fightUseSkillGems);
        }
        else if (m_attackType == ATTACK_TYPE_GROUPWAR)
        {
            HttpClient::getSingleton().addGroupWarBattleLog(m_otherInfo.uid ,m_curWarStar, m_myBaseLevel - m_otherBaseLevel, m_infoFightScore, m_logFight, m_warParam, LogicInfo::getSingleton().m_fightUseSkillGems);
        }
    }
    else {
        
        if (m_attackType == ATTACK_TYPE_PVP) {
            HttpClient::getSingleton().endBattle(m_otherInfo.uid);
        }
        else if (m_attackType == ATTACK_TYPE_CLANWAR) {
            HttpClient::getSingleton().endClanWarBattle(m_otherInfo.uid);
        }
    }
}

void LogicInfo::goHome()
{
	if(m_isGoHome)
		return;
    
	m_isGoHome = true;
    m_bChgScene2Home = true;
    
    m_revengeBattleId = -1;
    
	clearMyScene();
    
	m_mapBarrack.clear();
	m_mapTroophousing.clear();
    
    ClanMgr::getSingleton().m_myListSoldier.clear();
    
	chg2Wait();
    
	m_isInitOK = false;
	PlayerScene::getSingleton().clearScene();
	HttpClient::getSingleton().login();
    
    m_clanSoldierUsed = 0;
}

void LogicInfo::findAttackUser(int attackType)
{
    if (ATTACK_PLAYER_BY_UID)
    {
        findAttackUserEx(2577761);
        return;
    }
    
    m_clanSoldierUsed = 0;
    
    m_attackType = (AttackType)attackType;
    
    int attackCost = getFindGold();
    
    if(m_attackType == ATTACK_TYPE_LEAGUE)
    {
        attackCost = LeagueMgr::getSingleton().getSearchCost();
    }
    
    if(m_myInfo.goldex < attackCost)
        return;
    
    CSVMgr::getSingleton().checkData();
    
	chg2Wait();
    
    chgMoney(MONEY_GOLD, 0 - attackCost);
    
    HttpClient::getSingleton().subMoney(0, attackCost, 0);
    
    clearMyScene();
	clearOtherScene();

    if (attackType == ATTACK_TYPE_PVP) {
        HttpClient::getSingleton().findUser();
    }
    else if (attackType == ATTACK_TYPE_LEAGUE) {
        HttpClient::getSingleton().findLeagueAttactUser();
    }
}

void LogicInfo::revengeAttackUserEx(int uid, int msgid)
{
    m_clanSoldierUsed = 0;
    
    m_attackType = ATTACK_TYPE_PVP;
    
    CSVMgr::getSingleton().checkData();
    
	chg2Wait();
    
    clearMyScene();
	clearOtherScene();
    
	HttpClient::getSingleton().revengeUserEx(uid, msgid);
    
	for(std::vector<FightMsg>::iterator it = m_vecDefenseLog.begin(); it != m_vecDefenseLog.end(); ++it)
	{
		if(it->msgid == msgid)
			it->isAlreadyAttack = 1;
	}
}

void LogicInfo::findAttackUserEx(int uid, int attackType)
{
    m_clanSoldierUsed = 0;
    
    m_attackType = (AttackType)attackType;
    
    CSVMgr::getSingleton().checkData();
    
	chg2Wait();
    
    clearMyScene();
	clearOtherScene();
	
    if (attackType == ATTACK_TYPE_PVP) {
        HttpClient::getSingleton().attackUserEx(uid);
    }
    else if (attackType == ATTACK_TYPE_CLANWAR) {
        HttpClient::getSingleton().attackClanWar(uid);
    }
    else if (attackType == ATTACK_TYPE_GROUPWAR) {
        HttpClient::getSingleton().attackGroupWar(uid);
    }
}

void LogicInfo::attackEnv(int stageid)
{
    m_attackType = ATTACK_TYPE_PVE;
    
    m_clanSoldierUsed = 0;
    
    clearMyScene();
    
	clearOtherScene();
    
    HttpClient::getSingleton().requestAttackEnv(stageid);
}

//! 攻击是否结束，自己的兵over了，别人的建筑全部over了
//! 倒计时ui显示的时候自己计时比较好，所以这里不考虑时间结束
bool LogicInfo::isFinishAttack()
{
    bool hasClanSoldier = false;
    
    Building_ClanAllies* pClanAllies = (Building_ClanAllies*)PlayerScene::getSingleton().m_pBuildingClanAllies;
    if (ClanMgr::getSingleton().m_fightAttackSoldier.size() > 0 && (pClanAllies == NULL || pClanAllies->m_lstSoldier.size() > 0)) {
        hasClanSoldier = true;
    }
    
	if(PlayerScene::getSingleton().countBuildingNums() <= 0)
	{
		return true;
	}
	else if(m_lastFightSoldierNums <= 0 && PlayerScene::getSingleton().countSoldierNums(CAMP_ATTACK) <= 0 &&
            !hasClanSoldier)
	{
		return true;
	}
    
	return false;
}

//! 攻击的倒计时结束
void LogicInfo::attackTimeOver()
{
}

void LogicInfo::delSoldierFromTroophousing(int soldierid)
{
	for(MapTroophousingInfo::iterator it = m_mapTroophousing.begin(); it != m_mapTroophousing.end(); ++it)
	{
		for(std::list<TroophousingSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
		{
			if(it1->pid == soldierid)
			{
				it->second.ischg = true;
                
				if(it1->numsex > 1)
					it1->numsex -= 1;
				else
					it->second.lstSoldier.erase(it1);
                
				return;
			}
		}
	}
}

void LogicInfo::updateUserName(const char* name)
{
	HttpClient::getSingleton().upddateUserName(name);
    
	m_myInfo.uname = name;
    
	m_localUID = m_myInfo.uid;
	m_localUName = m_myInfo.uname;
    
	CCUserDefault::sharedUserDefault()->setIntegerForKey("uid", m_localUID);
	CCUserDefault::sharedUserDefault()->setStringForKey("uname", UrlEncode(m_localUName.c_str()));
    
	CCUserDefault::sharedUserDefault()->flush();//这里一定要提交写入哦，否则不会记录到xml中，下	次启动游戏你就获取不到value了。
    
	onNotify(NOTIFY_BASEINFO);
}

void LogicInfo::beginFight()
{
    if (m_fightMode == FIGHT_MODE_BEGIN) {
        return;
    }
    
    m_fightMode = FIGHT_MODE_BEGIN;
    
	RandMgr::getSingleton().initRandWar();
    
	m_curWarStar = 0;
    m_progHalfWar = false;
    
	m_isFinishAttack = false;
    
	m_fightBeginTimeMS = 0;
    
    m_infoFightScore.reset();
	memset(&m_warParam, 0, sizeof(m_warParam));
    
	HeroMgr::getSingleton().onBeginAttack();
    
	m_infoFightScore.numsBuilding = PlayerScene::getSingleton().countBuildingNums();
    
	m_infoFightScore.user[0].uid = m_myInfo.uid;
	strcpy(m_infoFightScore.user[0].name, m_myInfo.uname.c_str());
	m_infoFightScore.user[0].gold = m_myInfo.goldex;
	m_infoFightScore.user[0].oil = m_myInfo.oilex;
	m_infoFightScore.user[0].curlevel = m_myInfo.curlevel;
	m_infoFightScore.user[0].exp = m_myInfo.exp;
	m_infoFightScore.user[0].cup = m_myInfo.cupex;
    m_infoFightScore.user[0].clanid = ClanMgr::getSingleton().m_myclan.id;
    strcpy(m_infoFightScore.user[0].clanname, ClanMgr::getSingleton().m_myclan.name.c_str());
    m_infoFightScore.user[0].clanflag = ClanMgr::getSingleton().m_myclan.flag;
    m_infoFightScore.user[0].clanjointype = ClanMgr::getSingleton().m_myclan.jointype;
    m_infoFightScore.user[0].clanneedcup = ClanMgr::getSingleton().m_myclan.needcupnums;
    m_infoFightScore.user[0].clancup = ClanMgr::getSingleton().m_myclan.cupnums;
    m_infoFightScore.user[0].military = m_myInfo.military;
    
	m_infoFightScore.user[1].uid = m_otherInfo.uid;
	strcpy(m_infoFightScore.user[1].name, m_otherInfo.uname.c_str());
	m_infoFightScore.user[1].gold = m_otherInfo.goldex;
	m_infoFightScore.user[1].oil = m_otherInfo.oilex;
	m_infoFightScore.user[1].curlevel = m_otherInfo.curlevel;
	m_infoFightScore.user[1].exp = m_otherInfo.exp;
	m_infoFightScore.user[1].cup = m_otherInfo.cupex;
    m_infoFightScore.user[1].clanid = ClanMgr::getSingleton().m_otherclan.id;
    strcpy(m_infoFightScore.user[1].clanname, ClanMgr::getSingleton().m_otherclan.name.c_str());
    m_infoFightScore.user[1].clanflag = ClanMgr::getSingleton().m_otherclan.flag;
    m_infoFightScore.user[1].clanjointype = ClanMgr::getSingleton().m_otherclan.jointype;
    m_infoFightScore.user[1].clanneedcup = ClanMgr::getSingleton().m_otherclan.needcupnums;
    m_infoFightScore.user[1].clancup = ClanMgr::getSingleton().m_otherclan.cupnums;
    m_infoFightScore.user[1].military = m_otherInfo.military;
    
	m_logFight.clear();
    
    int index = 0;
	for(std::list<UserBuildingInfo*>::iterator it = m_otherBuilding.begin(); it != m_otherBuilding.end(); ++it)
	{
		int bid = (*it)->buildingid / 100;
		if (bid == BUILDING_HERO_ALTAR) //标志英雄是否防御
        {
            m_logFight.expand(index);
            m_logFight.node[index].type = CTRL_TYPE_HEROINFO;
            m_logFight.node[index].cid = HeroMgr::getSingleton().m_otherHero.heroid;
            m_logFight.node[index].state = HeroMgr::getSingleton().m_otherHero.hp;
            ++index;
            
            if (PlayerScene::getSingleton().m_pOtherHero) {
                
                Person* pHero = PlayerScene::getSingleton().m_pOtherHero;
                
                m_logFight.expand(index);
                m_logFight.node[index].type = CTRL_TYPE_HEROPOS;
                m_logFight.node[index].ctrl = pHero->m_walkStep;
                m_logFight.node[index].x = pHero->getLogicX();
                m_logFight.node[index].y = pHero->getLogicY();
                m_logFight.node[index].state = pHero->m_sleepTime;
                ++index;
            }
        }
	}
    
	m_logFight.ctrlIndex = index;
    m_logFight.numsBuilding = m_otherBuilding.size();
}

void LogicInfo::onDestroyBuilding(Building& building)
{
    if (m_isFinishAttack) {
        return;
    }
    
	LogicInfo::getSingleton().onAddCtrl_DestroyBuidling(building.m_indexBuilding);
    
    int bid = building.m_info.buildingid/100;
    
    if (bid / 100 == 5) { //陷阱
        return;
    }
    
	for(int i = 0; i < 2; ++i)
	{
		int res = building.m_pBuildingInfo->laststore[i];
		if(res <= 0)
			continue;
        
		if(i == 0)
		{
			if(m_otherInfo.goldex < res)
				res = m_otherInfo.goldex;
            
            building.m_pBuildingInfo->laststore[i] -= res;
            
			m_otherInfo.goldex -= res;
			m_infoFightScore.goldex += res;
		}
		else
		{
			if(m_otherInfo.oilex < res)
				res = m_otherInfo.oilex;
            
            building.m_pBuildingInfo->laststore[i] -= res;
            
			m_otherInfo.oilex -= res;
			m_infoFightScore.oilex += res;
		}
	}
    
	if(building.m_type == BUILDING_TYPE_WALL)
	{
		++m_warParam.numsWall;
	}
	else
	{
		++m_infoFightScore.numsDestroyBuilding;
                
		m_infoFightScore.progress = ((float)m_infoFightScore.numsDestroyBuilding) / m_infoFightScore.numsBuilding;
        
		int bid = building.m_buildingid / 100;
        if(bid == BUILDING_TOWN_HALL)
        {
			++m_warParam.numsTownhall;
            
			++m_curWarStar;
            
            onNotify(NOTIFY_DESTROY_HOME);
        }
        else if(bid == BUILDING_BUILDER_HOME)
        {
			++m_warParam.numsWorker;
        }
        else if(bid == BUILDING_BIG_MORTAR)
        {
			++m_warParam.numsMortar;
        }
        
        if(m_infoFightScore.progress >= 1.0f){
            m_curWarStar = 3;
        }
        else if(m_infoFightScore.progress >= 0.5f && !m_progHalfWar){
            m_curWarStar++;
            m_progHalfWar = true;
        }
	}
    
	onNotify(NOTIFY_OTHERBASEINFO);
    
    SoundMgr::getSingleton().playEffect("building_destroy.mp3");
	PlayerScene::getSingleton().setUpDownAction();
}

void LogicInfo::onMySoldierDead(Person& soldier)
{
}

void LogicInfo::onDropOutRes(Building& building, float per)
{
    if(m_isFinishAttack)
        return;
    
	int store = building.m_pBuildingInfo->store[0] > 0 ? building.m_pBuildingInfo->store[0] : building.m_pBuildingInfo->store[1];
	if(store != 0)
	{
		for(int i = 0; i < 2; ++i)
		{
            int bid = building.m_pBuildingInfo->buildingid/100;
			int res = building.m_pBuildingInfo->store[i];
			if(res == 0 || building.m_pBuildingInfo->laststore[i] <= 0)
				continue;
#if(CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_WP8)            
            int cr = res * per;
#else
			int cr = roundf(res * per);
#endif
            if (cr > building.m_pBuildingInfo->laststore[i]) {
                cr = building.m_pBuildingInfo->laststore[i];
            }
            
			if(i == 0)
			{
				if(m_otherInfo.goldex < cr)
					cr = m_otherInfo.goldex;
                
                building.m_pBuildingInfo->laststore[i] -= cr;
                
				m_otherInfo.goldex -= cr;
				m_infoFightScore.goldex += cr;
                
                if (bid == BUILDING_TOWN_HALL || bid == BUILDING_GOLD_STORAGE) {
                    m_infoFightScore.storegoldex += cr;
                }
                
				PlayerScene::getSingleton().shootGold(building, 1);
                SoundMgr::getSingleton().playEffect("att_gold.mp3");
			}
			else
			{
				if(m_otherInfo.oilex < cr)
					cr = m_otherInfo.oilex;
                
                building.m_pBuildingInfo->laststore[i] -= cr;
                
				m_otherInfo.oilex -= cr;
				m_infoFightScore.oilex += cr;
                
                if (bid == BUILDING_TOWN_HALL || bid == BUILDING_ELIXIR_STORAGE) {
                    m_infoFightScore.storeoilex += cr;
                }
                
				PlayerScene::getSingleton().shootOil(building, 1);
                SoundMgr::getSingleton().playEffect("att_oil.mp3");
			}
		}
	}
    
	onNotify(NOTIFY_OTHERBASEINFO);
}

void LogicInfo::onAddCtrl_DropSoldier(int personid, int x, int y)
{
	if(!isRealAttackMode())
		return;
    
	FightCtrlNode info;
    
    info.type = CTRL_TYPE_DROP_SOLDIER;
    info.cid = personid;
	info.ctrl = m_fightBeginTimeMS;
	info.x = x;
	info.y = y;
    
	m_logFight.expand(m_logFight.ctrlIndex + m_logFight.numsCtrl);
	m_logFight.node[m_logFight.ctrlIndex + m_logFight.numsCtrl] = info;
    
	++m_logFight.numsCtrl;
}

void LogicInfo::onAddCtrl_KillAtkSoldier(int personindex)
{
	if(!isRealAttackMode())
		return;
    
	FightCtrlNode info;
    
    info.type = CTRL_TYPE_KILL_ATTACK_SOLDIER;
    info.cid = personindex;
	info.ctrl = m_fightBeginTimeMS;
	info.x = 0;
	info.y = 0;
    
	m_logFight.expand(m_logFight.ctrlIndex + m_logFight.numsCtrl);
	m_logFight.node[m_logFight.ctrlIndex + m_logFight.numsCtrl] = info;
    
	++m_logFight.numsCtrl;
}

void LogicInfo::onAddCtrl_KillDefSoldier(int personindex)
{
	if(!isRealAttackMode())
		return;
    
	FightCtrlNode info;
    
    info.type = CTRL_TYPE_KILL_DEFEND_SOLDIER;
    info.cid = personindex;
	info.ctrl = m_fightBeginTimeMS;
	info.x = 0;
	info.y = 0;
    
	m_logFight.expand(m_logFight.ctrlIndex + m_logFight.numsCtrl);
	m_logFight.node[m_logFight.ctrlIndex + m_logFight.numsCtrl] = info;
    
	++m_logFight.numsCtrl;
}

void LogicInfo::onAddCtrl_DestroyBuidling(int buildingindex)
{
	if(!isRealAttackMode())
		return;
    
	FightCtrlNode info;
    
    info.type = CTRL_TYPE_DESTROY_BUILDING;
    info.cid = buildingindex;
	info.ctrl = m_fightBeginTimeMS;
	info.x = 0;
	info.y = 0;
    
	m_logFight.expand(m_logFight.ctrlIndex + m_logFight.numsCtrl);
	m_logFight.node[m_logFight.ctrlIndex + m_logFight.numsCtrl] = info;
    
	++m_logFight.numsCtrl;
}

void LogicInfo::onAddCtrl_UseSkill(int personindex, int skillid)
{
	if(!isRealAttackMode())
		return;
    
	FightCtrlNode info;
    
    info.type = CTRL_TYPE_USESKILL;
    info.cid = personindex;
	info.ctrl = m_fightBeginTimeMS;
    info.state = skillid;
	info.x = 0;
	info.y = 0;
    
	m_logFight.expand(m_logFight.ctrlIndex + m_logFight.numsCtrl);
	m_logFight.node[m_logFight.ctrlIndex + m_logFight.numsCtrl] = info;
    
	++m_logFight.numsCtrl;
}

void LogicInfo::onAddCtrl_ChgAtkDest(int personindex, int destype, int destindex)
{
	if(!isRealAttackMode())
		return;
    
    //! 调整策略，整合相邻且目标相同的一组消息
	if(m_logFight.ctrlIndex + m_logFight.numsCtrl > 0)
	{
		int index = m_logFight.ctrlIndex + m_logFight.numsCtrl - 1;
        
		if(m_logFight.node[index].type == CTRL_TYPE_CHANGE_ATTACK_TARGET)
		{
            int soldierid = m_logFight.node[index].cid;
			if(m_logFight.node[index].x == destype && m_logFight.node[index].y == destindex)
			{
				int bpindex = soldierid / 1000;
				int epindex = soldierid % 1000;
                
				if(personindex == epindex + 1)
				{
					m_logFight.node[index].cid = bpindex * 1000 + personindex;
					return;
				}
				else if(personindex == bpindex - 1)
				{
					m_logFight.node[index].cid = personindex * 1000 + epindex;
					return;
				}
			}
		}
	}
    
	FightCtrlNode info;
    
    info.type = CTRL_TYPE_CHANGE_ATTACK_TARGET;
	info.ctrl = m_fightBeginTimeMS;
	info.cid = personindex * 1000 + personindex;
	info.x = destype;
	info.y = destindex;
    
	m_logFight.expand(m_logFight.ctrlIndex + m_logFight.numsCtrl);
	m_logFight.node[m_logFight.ctrlIndex + m_logFight.numsCtrl] = info;
    
	++m_logFight.numsCtrl;
}

void LogicInfo::onAddCtrl_ChgDefDest(int personindex, int destype, int destindex)
{
	if(!isRealAttackMode())
		return;
    
    //! 调整策略，整合相邻且目标相同的一组消息
	if(m_logFight.ctrlIndex + m_logFight.numsCtrl > 0)
	{
		int index = m_logFight.ctrlIndex + m_logFight.numsCtrl - 1;
        
		if(m_logFight.node[index].type == CTRL_TYPE_CHANGE_DEFEND_TARGET)
		{
            int soldierid = m_logFight.node[index].cid;
			if(m_logFight.node[index].x == destype && m_logFight.node[index].y == destindex)
			{
				int bpindex = soldierid / 1000;
				int epindex = soldierid % 1000;
                
				if(personindex == epindex + 1)
				{
					m_logFight.node[index].cid = bpindex * 1000 + personindex;
					return;
				}
				else if(personindex == bpindex - 1)
				{
					m_logFight.node[index].cid = personindex * 1000 + epindex;
					return;
				}
			}
		}
	}
    
	FightCtrlNode info;
    
    info.type = CTRL_TYPE_CHANGE_DEFEND_TARGET;
	info.ctrl = m_fightBeginTimeMS;
	info.cid = personindex * 1000 + personindex;
	info.x = destype;
	info.y = destindex;
    
	m_logFight.expand(m_logFight.ctrlIndex + m_logFight.numsCtrl);
	m_logFight.node[m_logFight.ctrlIndex + m_logFight.numsCtrl] = info;
    
	++m_logFight.numsCtrl;
}

void LogicInfo::onAddCtrl_UseClanSoldier(int x, int y)
{
    if(!isRealAttackMode())
		return;
    
    FightCtrlNode info;
    
    info.type = CTRL_TYPE_USE_CLAN_SOLDIER;
	info.ctrl = m_fightBeginTimeMS;
	info.cid = ClanMgr::getSingleton().m_myclan.id;
    info.state = ClanMgr::getSingleton().m_myclan.flag;
	info.x = x;
	info.y = y;
    
	m_logFight.expand(m_logFight.ctrlIndex + m_logFight.numsCtrl);
	m_logFight.node[m_logFight.ctrlIndex + m_logFight.numsCtrl] = info;
    
	++m_logFight.numsCtrl;
}

void LogicInfo::onAddCtrl_ClanSoldierAttack(int personid, int x, int y)
{
    if(!isRealAttackMode())
		return;
    
	FightCtrlNode info;
    
    info.type = CTRL_TYPE_CLAN_SOLDIER_ATTACK;
    info.ctrl = m_fightBeginTimeMS;
    info.cid = personid;
	info.x = x;
	info.y = y;
    
	m_logFight.expand(m_logFight.ctrlIndex + m_logFight.numsCtrl);
	m_logFight.node[m_logFight.ctrlIndex + m_logFight.numsCtrl] = info;
    
	++m_logFight.numsCtrl;
}

void LogicInfo::onAddCtrl_ClanSoldierDefend(int buildingindex, int personid, int skillid)
{
	if(!isRealAttackMode())
		return;
    
	FightCtrlNode info;
    
    info.type = CTRL_TYPE_CLAN_SOLDIER_DEFEND;
	info.ctrl = m_fightBeginTimeMS;
	info.cid = buildingindex;
	info.x = personid;
	info.y = skillid;
    
	m_logFight.expand(m_logFight.ctrlIndex + m_logFight.numsCtrl);
	m_logFight.node[m_logFight.ctrlIndex + m_logFight.numsCtrl] = info;
    
	++m_logFight.numsCtrl;
}

void LogicInfo::onAddCtrl_EndFight()
{
    if(!isRealAttackMode())
		return;
    
	FightCtrlNode info;
    
    info.type = CTRL_TYPE_FIGHT_END;
	info.ctrl = m_fightBeginTimeMS;
    
	m_logFight.expand(m_logFight.ctrlIndex + m_logFight.numsCtrl);
	m_logFight.node[m_logFight.ctrlIndex + m_logFight.numsCtrl] = info;
    
	++m_logFight.numsCtrl;
}

int LogicInfo::fastBarrack(BarrackInfo* pBarrackInfo, Building* pBuilding)
{
    if(pBarrackInfo->barrackid >= 0)
	{
        int ms = 0;
        for(std::list<BarrackSoldierInfo>::iterator it = pBarrackInfo->lstSoldier.begin(); it != pBarrackInfo->lstSoldier.end(); ++it)
        {
            if(it == pBarrackInfo->lstSoldier.begin())
                ms += it->needtimeex * 1000 * (it->numsex - 1);
            else
                ms += it->needtimeex * 1000 * it->numsex;
        }
        
        ms += pBarrackInfo->lasttime * 1000;
        
        int gem = ms / 1000 / 240;
        if(gem < 1)
            gem = 1;
        
        if(m_myInfo.gemex < gem)
            return gem - m_myInfo.gemex;
        
        chgGem(-gem);
        
        onNotify(NOTIFY_BASEINFO);
        
        char str[128];
        sprintf(str, "%d", gem);
        HttpClient::getSingleton().subMoney(gem, 0, 0);
        
        while (pBarrackInfo->lstSoldier.size() > 0) {
            bool ret = onRefurbishBarrack(pBarrackInfo->barrackid, pBarrackInfo->lasttime, pBuilding);
            
            if (!ret) {
                break;
            }
        }
        
        pBarrackInfo->lasttime = 0;
        pBarrackInfo->curtime = 0;
        
        onRefurbishAllTroophousing();
        
        return 0;
	}
    
	return -1;
}

void LogicInfo::onBeginReplay()
{
	m_otherBuilding.clear();

	RandMgr::getSingleton().initRandWar();
    
	m_isFinishAttack = false;
	m_isReplayMode = true;
    
    m_curWarStar = 0;
    m_curBuildingIndex = 0;
    m_curPersonAtk = 0;
    m_curPersonDef = 0;
    
	clearMyScene();
	clearOtherScene();
    
	m_mapBarrack.clear();
	m_mapTroophousing.clear();
    m_lstUsedFightSoldier.clear();
    
    m_infoFightScore.progress = 0;
    m_infoFightScore.goldex = 0;
    m_infoFightScore.oilex = 0;
    m_infoFightScore.storegoldex = 0;
    m_infoFightScore.storeoilex = 0;
    
    m_otherInfo.goldex = m_infoFightScore.user[1].gold;
    m_otherInfo.oilex = m_infoFightScore.user[1].oil;
    m_otherInfo.cupex = m_infoFightScore.user[1].cup;
    
	m_otherInfo.maxgoldex = 0;
	m_otherInfo.maxoilex = 0;
    
    ClanMgr::getSingleton().m_otherclan.id = m_infoFightScore.user[1].clanid;
    ClanMgr::getSingleton().m_otherclan.flag = m_infoFightScore.user[1].clanflag;
    ClanMgr::getSingleton().m_otherclan.name = m_infoFightScore.user[1].clanname;
    
    for (int i = 0; i < m_logFight.numsBuilding; ++i) {
        UserBuildingInfo* pbi = new UserBuildingInfo;
        pbi->id = m_logFight.building[i].id;
        pbi->buildingid = m_logFight.building[i].bid;
        pbi->x = m_logFight.building[i].x;
        pbi->y = m_logFight.building[i].y;
        pbi->state = m_logFight.building[i].state;
        pbi->hp = m_logFight.building[i].hp;
        pbi->maxhp = m_logFight.building[i].maxhp;
        pbi->lasttime = 0;
        pbi->curtime = 0;
        pbi->maxres = 0;
        pbi->currestime = 0;
        pbi->timeres = 0;
        
        pbi->store[0] = m_logFight.building[i].store[0];
        pbi->laststore[0] = pbi->store[0];
        pbi->store[1] = m_logFight.building[i].store[1];
        pbi->laststore[1] = pbi->store[1];
        
        const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(pbi->buildingid);
        if(pInfo != NULL)
        {
            pbi->maxstore[0] = pInfo->maxstored[0];
            pbi->maxstore[1] = pInfo->maxstored[1];
        }
        
        m_otherBuilding.push_back(pbi);
        onAddOtherBuilding(*pbi);
    }
    
    for(int i = 0; i < m_logFight.ctrlIndex; ++i)
    {
        if (m_logFight.node[i].type == CTRL_TYPE_HEROINFO) {
            
            HeroMgr::getSingleton().m_otherHero.heroid = m_logFight.node[i].cid;
            HeroMgr::getSingleton().m_otherHero.hp = m_logFight.node[i].state;
        }
        else if (m_logFight.node[i].type == CTRL_TYPE_HEROPOS) {
            
            HeroMgr::getSingleton().m_otherHeroCtrl = m_logFight.node[i];
        }
    }
    
	PlayerScene::getSingleton().onInitOther();
	PlayerScene::getSingleton().onReplay();
    
	m_infoFightScore.numsBuilding = PlayerScene::getSingleton().countBuildingNums();
    m_infoFightScore.numsDestroyBuilding = 0;
}

void LogicInfo::stopReplay()
{
	m_isReplayMode = false;
    
	PlayerScene::getSingleton().onStopReplay();
    
	clearMyScene();
    
	m_mapBarrack.clear();
	m_mapTroophousing.clear();
    
	m_isInitOK = false;
	PlayerScene::getSingleton().clearScene();
}

void LogicInfo::onStopAttack(bool isPause)
{
	if(m_isFinishAttack)
		return;
    
    HeroMgr::getSingleton().m_curAttackExp = HeroMgr::getSingleton().countWarExp(LogicInfo::getSingleton().m_curWarStar, LogicInfo::getSingleton().m_myBaseLevel, LogicInfo::getSingleton().m_otherBaseLevel);
    
	if(m_isAttackDropPerson) {
		m_winCups = countWarCup(m_curWarStar, m_otherBaseLevel - m_myBaseLevel, m_otherInfo.cupex - m_myInfo.cupex);
    }
	else {
        m_winCups = 0;
    }
		  
	//m_isFighting = false;
	m_isFinishAttack = true;
    
	onAddCtrl_EndFight();
    
    endBattle();
    
	m_fightBeginTimeMS = 0;
    
	if(!isPause)
		PlayerScene::getSingleton().onStopAttack();
}

void LogicInfo::addUsedFightSoldier(int personid)
{
    for(std::list<FightSoldierInfoEx>::iterator it = m_lstUsedFightSoldier.begin(); it != m_lstUsedFightSoldier.end(); ++it)
    {
        if(it->pid == personid)
        {
            it->numsex += 1;
            return;
        }
    }
    
	FightSoldierInfoEx fsi;
    
	fsi.pid = personid;
	fsi.numsex = 1;
    
	m_lstUsedFightSoldier.push_back(fsi);
    
	//CCLOG("addUsedFightSoldier %d", m_lstUsedFightSoldier.size());
}

//void LogicInfo::loadTownhall()
//{
//	CCSVFile csv;
//	csv.Open(CCFileUtils::fullPathFromRelativePath("csv/townhall.csv"));
//	for(int i = 1; i < csv.GetLineCount(); ++i)
//	{
//		std::pair<int, Townhall_BuildingNums> p;
//
//		p.first = atoi(csv.GetCell(i, 0));
//
//		p.second.level = p.first;
//
//		p.second.attackCost = atoi(csv.GetCell(i, 1));
//		p.second.troopHousing = atoi(csv.GetCell(i, 2));
//		p.second.elixirStorage = atoi(csv.GetCell(i, 3));
//		p.second.goldStorage = atoi(csv.GetCell(i, 4));
//		p.second.elixirPump = atoi(csv.GetCell(i, 5));
//		p.second.goldMine = atoi(csv.GetCell(i, 6));
//		p.second.barrack = atoi(csv.GetCell(i, 7));
//		p.second.cannon = atoi(csv.GetCell(i, 8));
//		p.second.wall = atoi(csv.GetCell(i, 9));
//		p.second.archerTower = atoi(csv.GetCell(i, 10));
//		p.second.wizardTower = atoi(csv.GetCell(i, 11));
//		p.second.airDefense = atoi(csv.GetCell(i, 12));
//		p.second.mortar = atoi(csv.GetCell(i, 13));
//
//		p.second.workerBuilding = atoi(csv.GetCell(i, 18));
//		p.second.laboratory = atoi(csv.GetCell(i, 19));
//
//		m_mapTownhall.insert(p);
//	}
//}

void LogicInfo::onRefurbishTownhall()
{
	m_myLabLevel = 0;
	memset(&m_curTownhall, 0, sizeof(m_curTownhall));
    
	for(std::list<UserBuildingInfo*>::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
        if((*it)->state == -1)
            continue;
        
		int bid = (*it)->buildingid / 100;
		if(bid == BUILDING_ARMY_CAMP)
			m_curTownhall.troopHousing++;
		else if(bid == BUILDING_ELIXIR_COLLECTOR)
			m_curTownhall.elixirPump++;
		else if(bid == BUILDING_ELIXIR_STORAGE)
			m_curTownhall.elixirStorage++;
		else if(bid == BUILDING_GOLD_MINE)
			m_curTownhall.goldMine++;
		else if(bid == BUILDING_GOLD_STORAGE)
			m_curTownhall.goldStorage++;
		else if(bid == BUILDING_BARRACKS)
			m_curTownhall.barrack++;
		else if(bid == BUILDING_LABORATORY)
		{
			m_curTownhall.laboratory++;
            
			m_myLabLevel = (*it)->buildingid % 100;
		}
		else if(bid == BUILDING_CANNON)
			m_curTownhall.cannon++;
		else if(bid == BUILDING_ARCHER_TOWER)
			m_curTownhall.archerTower++;
		else if(bid == BUILDING_WALL)
			m_curTownhall.wall++;
		else if(bid == BUILDING_WIZARD_TOWER)
			m_curTownhall.wizardTower++;
		else if(bid == BUILDING_AIR_DEFENSE)
			m_curTownhall.airDefense++;
		else if(bid == BUILDING_BIG_MORTAR)
			m_curTownhall.mortar++;
		else if(bid == BUILDING_BUILDER_HOME)
			m_curTownhall.workerBuilding++;
		else if(bid == BUILDING_BOMB)
			m_curTownhall.bomb++;
		else if(bid == BUILDING_SPIKE_TRAP)
			m_curTownhall.spring++;
		else if(bid == BUILDING_AIR_BOMB)
			m_curTownhall.flybomb++;
		else if(bid == BUILDING_BIG_BOMB)
			m_curTownhall.bigbomb++;
        else if(bid == BUILDING_HERO_ALTAR)
            m_curTownhall.heroAltar++;
		else if(bid == BUILDING_MAIL_BOX)
			m_curTownhall.obuilding[0]++;
		else if(bid == BUILDING_STELE)
			m_curTownhall.obuilding[1]++;
		else if(bid == BUILDING_WEATHER_VANE)
			m_curTownhall.obuilding[2]++;
		else if(bid == BUILDING_ROAD_SIGN)
			m_curTownhall.obuilding[3]++;
		else if(bid == BUILDING_BULLETIN_BOARD)
			m_curTownhall.obuilding[4]++;
		else if(bid == BUILDING_FLOWER_BED1)
			m_curTownhall.obuilding[5]++;
		else if(bid == BUILDING_FLOWER_BED2)
			m_curTownhall.obuilding[6]++;
        else if(bid >= 608 && bid <= 647)
            m_curTownhall.letter[bid - 608]++;
        
		if(bid == BUILDING_TOWN_HALL)
		{
			if((*it)->state == 2)
				m_curTownhall.level = (*it)->buildingid % 100;
			else
				m_curTownhall.level = (*it)->buildingid % 100 - 1;
		}
	}
}

void LogicInfo::onChangeScene()
{
    for(MapBarrackInfo::iterator it = m_mapBarrack.begin(); it != m_mapBarrack.end(); ++it)
	{
        if(it != m_mapBarrack.end()) {
            if (it->second.ischg) {
                HttpClient::getSingleton().updateBarrackInfo(it->second);
            }
        }
    }
    
    for(MapTroophousingInfo::iterator it = m_mapTroophousing.begin(); it != m_mapTroophousing.end(); ++it)
    {
        if (it != m_mapTroophousing.end()) {
            if (it->second.ischg) {
                HttpClient::getSingleton().updateTroophousingInfo(it->second);
            }
        }
    }
    
    m_fightMode = FIGHT_MODE_WAIT;
    m_fightBeginTimeMS = 0;
    m_isFinishAttack = false;
    m_isAttackDropPerson = false;
    
    if (!m_isGoHome) {
        m_bChgScene2Home = false;
    }
}

void LogicInfo::onPause()
{
	m_willPause = true;
    
    if (PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pBarrackUI) {
        LogicInfo::getSingleton().onCloseBarrack();
    }
    
	onRefurbishAllBarrack();
	onRefurbishAllTroophousing();
    
	time_t ct = getCurrentTime();// time(NULL);
	if(m_beginTime > 0)
	{
		int offtime = ct - m_beginTime;
		if(offtime > 0)
		{
			HttpClient::getSingleton().updUserGameTime(offtime);
		}
	}
    
	if(m_pTarget_ClearTouch != NULL && m_pFunc_ClearTouch != NULL)
	{
		(m_pTarget_ClearTouch->*m_pFunc_ClearTouch)();
	}
    
	m_isPause = true;
    
	m_pauseBeginTime = getCurrentTime();// time(NULL);
    
    PlayerScene::getSingleton().onProcPushMsg();
    
	m_willPause = false;
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    Sleep(1000);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	
#else
    usleep(1000);
#endif // _MSC_VER
}

void LogicInfo::onResume()
{
    if(HelloWorld::show())
    {
        return;
    }
    
	m_beginTime = getCurrentTime();// time(NULL);
    
    if(m_pauseBeginTime > 0) {
        m_pauseTime = m_beginTime - m_pauseBeginTime;
    }
    
    if (m_hasChartboostFreeGem)
    {
        m_hasChartboostFreeGem = false;
        LogicInfo::getSingleton().m_addGemType = ADD_GEM_CHARTBOOST;
        m_asyncViewList.insert(pair<int, int>(ASYNC_VIEW_ADD_GEM, m_chartboostClickGem));
    }
    
    HttpClient::getSingleton().sendRequest(HttpClient::getSingleton().m_pCurRequest);
    
    HttpClient::getSingleton().keeplive(false);
    
    m_pauseBeginTime = 0;
    m_isPause = false;
}

void LogicInfo::onExit()
{
    onPause();
    
    if (isRealAttackMode()) {
        onStopAttack(true);
    }
    
    int i = 0;
    while (i < 2) { //wait
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		Sleep(500);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) 

#else
        usleep(500);
#endif // _MSC_VER
        
        i++;
    }
}

void LogicInfo::resetAll()
{
	m_isVerOK = false;
	m_isProcKeepLife = false;
    m_bShowRaidNotify = true;

    HttpClient::getSingleton().clearRequestList();
    
	m_willPause = false;
	m_isLogin = false;
    //m_isPause = true;
    
    m_curState = LOGICSTATE_WAIT;
    
    m_isErrorMsg = false;
    m_isExitMsg = false;
    
    PlayerScene::getSingleton().m_pBuildOK = NULL;
    PlayerScene::getSingleton().m_pBuildCancel = NULL;
    PlayerScene::getSingleton().m_pBuildUI = NULL;
    PlayerScene::getSingleton().m_isEditMode = false;
    
    m_lstNotifyListener.clear();
    PlayerScene::getSingleton().m_lstSelectBuildingListener.clear();
    HttpClient::getSingleton().m_lstListener.clear();
    
	clearMyScene();
	clearOtherScene();
	m_isInitOK = false;
	PlayerScene::getSingleton().clearScene();
    
    GuideMgr::getSingleton().reset();
    
    HttpClient::getSingleton().addListener(this);
}

//! 该接口仅用于除草
int LogicInfo::removeBuilding(Building& building, int& need)
{
	need = 0;
    
	if(building.m_pBuildingInfo != NULL)
	{
		if(!hasFreeWorker())
			return MSG_NOWORKER;
        
		if(building.m_buildingid / 10000 != 9)
			return MSG_ERROR;
        
		const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(building.m_buildingid);
		if(pInfo == NULL)
			return MSG_ERROR;
        
		if(pInfo->removecost > getMoney(pInfo->buildres))
		{
			need = pInfo->removecost - getMoney(building.m_info.buildres);
            
			return  MSG_NOGEM - building.m_info.buildres;
		}
        
		chgMoney(pInfo->buildres, -pInfo->removecost);
        
		HttpClient::getSingleton().removeBuilding(building.m_pBuildingInfo->id, building.m_buildingid, building.getLogicX(), building.getLogicY());
        
		building.m_pBuildingInfo->buildingid = pInfo->buildingid;
		building.m_pBuildingInfo->lasttime = ((pInfo->buildtime[0] * 24 + pInfo->buildtime[1]) * 60 + pInfo->buildtime[2]) * 60;
		building.m_pBuildingInfo->curtime = getCurrentTime();// time(NULL);
        
		if(building.m_pBuildingInfo->lasttime == 0)
		{
			building.m_pBuildingInfo->state = 2;
            
			int bid = pInfo->buildingid / 100;
			if(!(bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_GOLD_MINE))
			{
				building.m_pBuildingInfo->maxstore[0] = pInfo->maxstored[0];
				building.m_pBuildingInfo->maxstore[1] = pInfo->maxstored[1];
			}
            
			if(pInfo->buildingid >= 90000)
				onRemoveOtherOK(building.m_pBuildingInfo->x, building.m_pBuildingInfo->y);
		}
		else
			building.m_pBuildingInfo->state = 1;
        
		if(pInfo->producesres > 0)
			building.m_pBuildingInfo->maxres = pInfo->maxstored[pInfo->producesres - 1];
		else
			building.m_pBuildingInfo->maxres = 0;
        
		building.m_pBuildingInfo->currestime = 0;
		building.m_pBuildingInfo->timeres = pInfo->producesperhour;
        
		UserBuildingInfo* pUBI = building.m_pBuildingInfo;
        
		Building* pBuilding = PlayerScene::getSingleton().onUpdateBuilding(pUBI);
        
        if (pBuilding) {
            PlayerScene::getSingleton().onBuildingSelect(true, pBuilding);
        }
        
        if (PlayerScene::getSingleton().m_isEditMode) {
            PlayerScene::getSingleton().m_pGameWorld->m_pMainEditUI->onRemoveOther(pUBI);
        }
        
		refurbishBaseInfo();
	}
    
	return 1;
}

int LogicInfo::removeOrnament(Building& building, int& need)
{
	need = 0;
    
	if(building.m_pBuildingInfo != NULL)
	{
		if(building.m_buildingid / 10000 != 6)
			return MSG_ERROR;
        
		const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(building.m_buildingid);
		if(pInfo == NULL)
			return MSG_ERROR;
        
        //
		if(pInfo->removecost > getMoney(MONEY_GEM))
		{
			need = pInfo->removecost - getMoney(MONEY_GEM);
            
			return  MSG_NOGEM - MONEY_GEM;
		}
        
		chgMoney(MONEY_GEM, -pInfo->removecost);
        
		HttpClient::getSingleton().removeBuilding(building.m_pBuildingInfo->id, building.m_buildingid, building.getLogicX(), building.getLogicY());
        
		building.m_pBuildingInfo->buildingid = pInfo->buildingid;
		building.m_pBuildingInfo->lasttime = ((pInfo->buildtime[0] * 24 + pInfo->buildtime[1]) * 60 + pInfo->buildtime[2]) * 60;
		building.m_pBuildingInfo->curtime = getCurrentTime();// time(NULL);
        
		if(building.m_pBuildingInfo->lasttime == 0)
		{
			building.m_pBuildingInfo->state = 2;
            
			int bid = pInfo->buildingid / 100;
			if(!(bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_GOLD_MINE))
			{
				building.m_pBuildingInfo->maxstore[0] = pInfo->maxstored[0];
				building.m_pBuildingInfo->maxstore[1] = pInfo->maxstored[1];
			}
            
			if(pInfo->buildingid / 10000 == 6)
                onRemoveOrnamentOK(building.m_pBuildingInfo->x, building.m_pBuildingInfo->y);
		}
		else
			building.m_pBuildingInfo->state = 1;
        
		//building.m_pBuildingInfo->x = getAsInt(val["x"]);
		//building.m_pBuildingInfo->y = getAsInt(val["y"]);
        
		if(pInfo->producesres > 0)
			building.m_pBuildingInfo->maxres = pInfo->maxstored[pInfo->producesres - 1];
		else
			building.m_pBuildingInfo->maxres = 0;
        
		building.m_pBuildingInfo->currestime = 0;
		building.m_pBuildingInfo->timeres = pInfo->producesperhour;
        
		UserBuildingInfo* pUBI = building.m_pBuildingInfo;
        
		Building* pBuilding = PlayerScene::getSingleton().onUpdateBuilding(pUBI);
        
        if (pBuilding) {
            PlayerScene::getSingleton().onBuildingSelect(true, pBuilding);
        }
        
		refurbishBaseInfo();
	}
    
	return 1;
    
}

void LogicInfo::cancelFirstBuinding(Building& building)
{
	if(building.m_pBuildingInfo != NULL)
	{
		const BuildingInfo* pInfo;
        
		pInfo = BuildingMgr::getSingleton().getBuildingInfo(building.m_buildingid);
		if(pInfo == NULL)
			return;
        
		PlayerScene::getSingleton().cancelCurBuilding();
        
		for(std::list<UserBuildingInfo*>::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
		{
			if(*it == building.m_pBuildingInfo)
			{
				m_lstBuilding.erase(it);
                
				break;
			}
		}
        
		onRefurbishTownhall();
	}
}

//! 计算快速建造最少的宝石消耗
Building* LogicInfo::countFastBuildingGem(int& need)
{
	int lasttime = -1;
	Building* pBuilding = NULL;
    
	for(PlayerScene::_BuildingList::iterator it = PlayerScene::getSingleton().m_lstBuilding.begin(); it != PlayerScene::getSingleton().m_lstBuilding.end(); ++it)
	{
		if((*it)->m_pBuildingInfo->state != 2 && (*it)->m_pBuildingInfo->state != -1)
		{
            if(lasttime == -1)
            {
                pBuilding = (*it);
                lasttime = (*it)->m_lasttime;
            }
            else if(lasttime > (*it)->m_lasttime)
            {
                pBuilding = (*it);
                lasttime = (*it)->m_lasttime;
            }
		}
	}
    
	if(lasttime <= 0)
		return NULL;
    
	int gem = lasttime / (60 * 4);
	if(gem <= 0)
		gem = 1;
    
	need = gem;
    
	return pBuilding;
}

//! 计算升级英雄需要的宝石消耗
int LogicInfo::countBuildingHeroGem()
{
    if(PlayerScene::getSingleton().m_pHero == NULL)
        return 0;
    
    int gem = 0;
    int lasttime = HeroMgr::getSingleton().m_heroLastTime;
    if(lasttime <= 0)
        gem = 0;
    
    gem = lasttime / (60 * 4);
    if(gem <= 0)
        gem = 1;
    
    return gem;
        
}

//! 计算当前建筑需要的宝石消耗
int LogicInfo::countCurBuildingGem()
{
    Building* pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
    int lasttime;
    
    if(pBuilding != NULL)
    {
        lasttime = pBuilding->m_lasttime;
        
        if(lasttime <= 0)
            return 0;
        
        int gem = lasttime / (60 * 4);
        if(gem <= 0)
            gem = 1;
        
        return gem;
    }
    
    return 0;
}

//! 计算宝石到金币或者油
int LogicInfo::countGem2Other(int gem)
{
    int price = 200;
    
    float v =  price * pow((float)gem, 4 / 3.0f);
    
    v = floor(v);
    
    return (int)v;
}

//! 计算金币或者油到宝石
int LogicInfo::countOther2Gem(int other)
{
    int price = 200;
    
    float v = pow((float)((other + price-1) / price), 3 / 4.0f);
    
    v = ceil(v);
    
    return (int)v;
}

void LogicInfo::addExp(int exp)
{
	int leveloff = 0;
	m_myInfo.exp += exp;
    
	do{
		int cexp = ExperienceLevels::getSingleton().getCurExp(m_myInfo.curlevel);
		if(m_myInfo.exp >= cexp)
		{
			SoundMgr::getSingleton().playEffect("levelup.mp3");
            
			++m_myInfo.curlevel;
			m_myInfo.exp -= cexp;
			++leveloff;
            
            onNotify(NOTIFY_UPLEVEL);
		}
		else
			break;
        
	}while(true);
    
    onNotify(NOTIFY_BASEINFO);
	SoundMgr::getSingleton().playEffect("getexp.mp3");
}

void LogicInfo::onNotifyLoading(float progress)
{
	m_loadingProgress = progress;
    
	onNotify(NOTIFY_LOADINGPROGRESS);
}

void LogicInfo::requestRank()
{    
	HttpClient::getSingleton().requestRank();
}

void LogicInfo::setGuide(int val1)
{
    LogicInfo::getSingleton().chgGuideVal(val1);
}

void LogicInfo::viewUserEx(int uid, int attackType)
{
    m_isInitOK = false;
    m_clanSoldierUsed = 0;
    
    m_attackType = (AttackType)attackType;
    
	chg2Wait();
    
    onRefurbishAllBarrack();
	onRefurbishAllTroophousing();
    
    clearMyScene();
	clearOtherScene();
    
	HttpClient::getSingleton().viewUserEx(uid);
}

int LogicInfo::getHeroLastTime()
{
	return HeroMgr::getSingleton().getHeroLastTime();
}

bool LogicInfo::isHero(int soldierid) const
{
	return (soldierid/100) >= PERSON_HERO;
}

void LogicInfo::countLastTimeString(std::string& str, int lasttime)
{
	char strtime[128];
    
	int s = lasttime % 60;
	int m = lasttime % (60 * 60) / 60;
	int h = lasttime % (60 * 60 * 24) / (60 * 60);
    int d = lasttime / (60 * 60 * 24);
    
    if(d > 0)
    {
        sprintf(strtime, "%dd %dh %dm", d, h, m);
    }
	else if(h > 0)
		sprintf(strtime, "%dh %dm %ds", h, m, s);
	else if(m > 0)
		sprintf(strtime, "%dm %ds", m, s);
	else
		sprintf(strtime, "%ds", s);
    
	str = strtime;
}

void LogicInfo::initShopData()
{
    for(int i = 0; i < 6; ++i)
    {
        m_shoplst[i].clear();
    }
    //std::vector<int> lst1;
    for(int i = 0; i < getIAPNums(); ++i)
    {
        m_shoplst[0].push_back(0);
    }
    
    m_shoplst[0].push_back(0);
    m_shoplst[0].push_back(0);
    m_shoplst[0].push_back(0);
    m_shoplst[0].push_back(0);
    m_shoplst[0].push_back(0);
    m_shoplst[0].push_back(0);
    //m_shoplst.push_back(lst1);
    
    //std::vector<int> lst2;
    m_shoplst[1].push_back(BUILDING_ELIXIR_COLLECTOR);
    m_shoplst[1].push_back(BUILDING_ELIXIR_STORAGE);
    m_shoplst[1].push_back(BUILDING_GOLD_MINE);
    m_shoplst[1].push_back(BUILDING_GOLD_STORAGE);
    m_shoplst[1].push_back(BUILDING_BUILDER_HOME);
    
    m_shoplst[2].push_back(BUILDING_CANNON);
    m_shoplst[2].push_back(BUILDING_ARCHER_TOWER);
    m_shoplst[2].push_back(BUILDING_WALL);
    m_shoplst[2].push_back(BUILDING_BIG_MORTAR);
    m_shoplst[2].push_back(BUILDING_AIR_DEFENSE);
    m_shoplst[2].push_back(BUILDING_WIZARD_TOWER);
    m_shoplst[2].push_back(BUILDING_BOMB);
    m_shoplst[2].push_back(BUILDING_SPIKE_TRAP);
    m_shoplst[2].push_back(BUILDING_AIR_BOMB);
    m_shoplst[2].push_back(BUILDING_BIG_BOMB);
    
    m_shoplst[3].push_back(BUILDING_ARMY_CAMP);
    m_shoplst[3].push_back(BUILDING_BARRACKS);
    m_shoplst[3].push_back(BUILDING_LABORATORY);
    m_shoplst[3].push_back(BUILDING_HERO_ALTAR);
    
    m_shoplst[4].push_back(BUILDING_MAIL_BOX);
    m_shoplst[4].push_back(BUILDING_STELE);
    m_shoplst[4].push_back(BUILDING_WEATHER_VANE);
    m_shoplst[4].push_back(BUILDING_ROAD_SIGN);
    m_shoplst[4].push_back(BUILDING_BULLETIN_BOARD);
    m_shoplst[4].push_back(BUILDING_FLOWER_BED1);
    m_shoplst[4].push_back(BUILDING_FLOWER_BED2);
    
    for(int i = 0; i < BUILDING_LETTER_NUM; ++i)
    {
        m_shoplst[4].push_back(BUILDING_LETTER + i);
    }
    
    m_shoplst[5].push_back(-1);
    m_shoplst[5].push_back(-1);
    m_shoplst[5].push_back(-1);
}

int LogicInfo::showNums(int shopid)
{
    std::vector<int> lst = LogicInfo::getSingleton().m_shoplst[shopid - 1];
    
    int nums = 0;
    
    for(unsigned int i = 0; i < lst.size(); ++i)
    {
        nums += TownhallLevelInfo::getSingleton().getBuildingMaxNums(lst[i]) - TownhallLevelInfo::getSingleton().getBuildingCurNums(lst[i]);
    }
    
    return nums;
}

int LogicInfo::addUpgradeSoldier(int soldierid, int& need)
{
	const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(soldierid);
	if(pInfo == NULL)
		return MSG_ERROR;
    
    if(m_myInfo.oilex < pInfo->upgradeCost)
	{
		need = pInfo->upgradeCost - m_myInfo.oilex;
        
        return MSG_NOELIXIR;
	}
    
	chgOil(-pInfo->upgradeCost);
	HttpClient::getSingleton().subMoney(0, 0, pInfo->upgradeCost);
    
	addSoldierUpgradeInfo(soldierid, pInfo->upgradeTimeHour * 60 * 60);
    
	m_soldierupgrade.ischg = true;
    m_soldierupgrade.offtimems = 0;
    
    HttpClient::getSingleton().updateUpgradeSoldier(m_soldierupgrade);
	refurbishBaseInfo();
    
    return 1;
}

//void LogicInfo::refurbishSoldierUpgrade()
//{
//	//HttpClient::getSingleton().refurbishSoldierUpgrade();
//}

//! 等级差是敌人-自己，基地等级
//! 奖杯差也是敌人-自己
int LogicInfo::countWarCup(int star, int offlevel, int offcup)
{
	if(star <= 0)
		return m_fightLoseCups;
	else if(star == 1)
		return m_fightWinCups[0];
	else if(star == 2)
		return m_fightWinCups[1];
	else
		return m_fightWinCups[2];
}

//! 计算当前工人房价格
int LogicInfo::countCurWorkerBuildingPrice()
{
	const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(20201);
	if(pInfo == NULL)
		return 0;
    
    if(m_myInfo.totalworkernumsex == 5)
        return 0;
    
	return pInfo->buildcost * pow(2.0f, m_myInfo.totalworkernumsex - 1);
}

//! 判断当前是否可以继续建造墙
bool LogicInfo::canBuildingWall()
{
	int nums = m_curTownhall.wall;
    
	const Townhall_BuildingNums* pInfo = TownhallLevelInfo::getSingleton().getInfo(m_curTownhall.level);
	if(pInfo == NULL)
		return false;
    
	return nums < pInfo->wall;
}

//! 建筑完成后调用
void LogicInfo::onBuildingComplete(Building& building)
{
    int buildingid = building.m_pBuildingInfo->buildingid;
	int bid = buildingid / 100;
    
	const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(building.m_pBuildingInfo->buildingid);
	if(pInfo == NULL)
		return;
    
	addExp(pInfo->exp);
    
	if(bid == BUILDING_ARMY_CAMP)
	{
		for(MapTroophousingInfo::iterator it = m_mapTroophousing.begin(); it != m_mapTroophousing.end(); ++it)
		{
			if(building.m_pBuildingInfo->troophousingid == it->second.troophousingid)
			{
				it->second.maxnumsex = pInfo->housingspace;
				break;
			}
		}
        
		refurbishSoldierNums();
	}
	else if(bid == BUILDING_BARRACKS)
	{
		int level = building.m_pBuildingInfo->buildingid % 100;
		if(level > m_myInfo.maxBarrackLevel)
			m_myInfo.maxBarrackLevel = level;
        
		for(MapBarrackInfo::iterator it = m_mapBarrack.begin(); it != m_mapBarrack.end(); ++it)
		{
			if(building.m_pBuildingInfo->barrackid == it->second.barrackid)
			{
				it->second.maxnumsex = pInfo->housingspace;
				break;
			}
		}
        
		refurbishSoldierNums();
	}
    
    if (bid == BUILDING_TOWN_HALL) {
        ++ m_myBaseLevel;
    }
    
    if(building.m_pBuildingInfo->buildingid >= 20200 && building.m_pBuildingInfo->buildingid <= 20299)
	{
		m_myInfo.totalworkernumsex += 1;
	}
    
    if (bid == BUILDING_HERO_ALTAR) {
        
        PlayerScene::getSingleton().m_pBuildingHeroAltar = &building;
        HeroMgr::getSingleton().createHero(30101);
    }
    
	if(bid == BUILDING_CLAN_CASTLE)
	{
        PlayerScene::getSingleton().m_pBuildingClan = &building;
        ClanMgr::getSingleton().m_myMaxSpace = building.m_info.housingspace;
        ClanMgr::getSingleton().m_castleLevel = buildingid % 100;
        
        ClanMgr::getSingleton().levelupClanCastle();
	}
    
    if (bid == BUILDING_TOWN_HALL)
    {
        LogicInfo::getSingleton().m_bShowShopTips1 = true;
        LogicInfo::getSingleton().m_bShowShopTips2 = true;
        LogicInfo::getSingleton().m_bShowShopTips3 = true;
        
        CCUserDefault::sharedUserDefault()->setBoolForKey("shopTips1", true);
        CCUserDefault::sharedUserDefault()->setBoolForKey("shopTips2", true);
        CCUserDefault::sharedUserDefault()->setBoolForKey("shopTips3", true);
        CCUserDefault::sharedUserDefault()->setIntegerForKey("LastTownHallLevel", m_myBaseLevel);
        CCUserDefault::sharedUserDefault()->flush();
        onNotify(NOTIFY_BASEINFO);
    }
    
    if(building.m_pBuildingInfo->barrackid >= 0)
    {
        onAddBarrack(*building.m_pBuildingInfo, pInfo);
    }
    else if(building.m_pBuildingInfo->troophousingid >= 0)
    {
        onAddTroophousing(building.m_pBuildingInfo->troophousingid, pInfo->housingspace);
    }
    
	onAchievement_BuildingComplete(building.m_pBuildingInfo->buildingid);
    
	onNotify(NOTIFY_BASEINFO);
    
    countMilitaryRank();
    
    SoundMgr::getSingleton().playEffect("building_finished.mp3");
    
	if(bid == BUILDING_ELIXIR_STORAGE || bid == BUILDING_GOLD_STORAGE)
		fixStorageReserve(true);
    
}

// 士兵升级成功
void LogicInfo::onSoldierUpgrade(int soldierid)
{
    
	PlayerScene::getSingleton().onSoldierUpgrade(soldierid);
    
	for(MapTroophousingInfo::iterator it = m_mapTroophousing.begin(); it != m_mapTroophousing.end(); ++it)
	{
		for(std::list<TroophousingSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
		{
			if((it1->pid / 100) == (soldierid / 100) && (it1->pid % 100) != (soldierid % 100))
			{
				it1->pid = soldierid;
                
				it->second.ischg = true;
                m_soldierupgrade.ischg = true;
			}
		}
	}
    
	for(MapBarrackInfo::iterator it = m_mapBarrack.begin(); it != m_mapBarrack.end(); ++it)
	{
		for(std::list<BarrackSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
		{
			if((it1->pid / 100) == (soldierid / 100) && (it1->pid % 100) != (soldierid % 100))
			{
				it1->pid = soldierid;
                
				it->second.ischg = true;
                m_soldierupgrade.ischg = true;
			}
		}
	}
}

// 刷新实验室
void LogicInfo::onRefurbishLabMS(int ot, Building& building)
{
	if(!isNormalMode())
		return;
    
	if(m_soldierupgrade.nums <= 0)
	{
		building.setTime(0, 0);
        
		return;
	}
    
	m_soldierupgrade.offtimems += ot;
    
	if(m_soldierupgrade.offtimems > 1000)
	{
		int ct = m_soldierupgrade.offtimems / 1000;
		m_soldierupgrade.offtimems %= 1000;
        
		for(int i = 0; i < m_soldierupgrade.nums; ++i)
		{
			if(m_soldierupgrade.lasttime[i] > ct)
			{
				m_soldierupgrade.lasttime[i] -= ct;
                
				ct = 0;
                
				const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(m_soldierupgrade.soldierid[i]);
				if(pInfo != NULL)
				{
					building.setTime(pInfo->upgradeTimeHour * 60 * 60, m_soldierupgrade.lasttime[i]);
				}
                
				break;
			}
			else if(m_soldierupgrade.lasttime[i] > 0)
			{
				ct = m_soldierupgrade.lasttime[i];
                
				m_soldierupgrade.lasttime[i] = 0;
                
                m_soldierupgrade.ischg = true;
                
                onNotify(NOTIFY_BASEINFO);
                
				building.setTime(0, 0);
                
                onMyRefurbishSoldierLevel();
			}
		}
	}
}

//! 增加一个士兵升级
void LogicInfo::addSoldierUpgradeInfo(int soldierid, int lasttime)
{
	int dsid = soldierid / 100;
	int dlevel = soldierid % 100;
	for(int i = 0; i < m_soldierupgrade.nums; ++i)
	{
		int sid = m_soldierupgrade.soldierid[i] / 100;
		if(sid == dsid)
		{
			int level = m_soldierupgrade.soldierid[i] % 100;
			if(dlevel == level + 1)
			{
				if(m_soldierupgrade.lasttime[i] <= 0)
				{
					m_soldierupgrade.soldierid[i] = soldierid;
					m_soldierupgrade.lasttime[i] = lasttime;
				}
			}
            
			return;
		}
	}
    
	m_soldierupgrade.soldierid[m_soldierupgrade.nums] = soldierid;
	m_soldierupgrade.lasttime[m_soldierupgrade.nums] = lasttime;
    
	++m_soldierupgrade.nums;
}

//! 获得当前升级的士兵
int LogicInfo::getCurUpgradeSoldier()
{
	for(int i = 0; i < m_soldierupgrade.nums; ++i)
	{
		if(m_soldierupgrade.lasttime[i] > 0)
			return m_soldierupgrade.soldierid[i];
	}
    
	return -1;
}

// 加速升级
int LogicInfo::fastSoldierUpgrade()
{
	int totaltime = 0;
    
	for(int i = 0; i < m_soldierupgrade.nums; ++i)
	{
		if(m_soldierupgrade.lasttime[i] > 0)
			totaltime += m_soldierupgrade.lasttime[i];
	}
    
	int gem = totaltime / 240;
	if(gem < 1)
		gem = 1;
    
	if(m_myInfo.gemex < gem)
		return gem - m_myInfo.gemex;
    
	chgGem(-gem);
    
	HttpClient::getSingleton().subMoney(gem, 0, 0);
    
	PlayerScene::getSingleton().onFastUpgradeSoldier(totaltime * 1000);
    
    onNotify(NOTIFY_BASEINFO);
    
	return 0;
}

// 判断当前正在升级的士兵数量
int LogicInfo::countSoldierUpgradeNums()
{
	int nums = 0;
    
	for(int i = 0; i < m_soldierupgrade.nums; ++i)
	{
		if(m_soldierupgrade.lasttime[i] > 0)
			++nums;
	}
    
	return nums;
}

// 刷新自己的士兵等级
void LogicInfo::onMyRefurbishSoldierLevel()
{
	for(int i = 0; i < m_soldierupgrade.nums; ++i)
	{
		if(m_soldierupgrade.lasttime[i] <= 0)
		{
			chgMySoldierLevel(m_soldierupgrade.soldierid[i]);
            
			onSoldierUpgrade(m_soldierupgrade.soldierid[i]);
		}
		else
		{
			if(m_soldierupgrade.soldierid[i] % 10 == 1)
				chgMySoldierLevel(m_soldierupgrade.soldierid[i]);
			else
				chgMySoldierLevel(m_soldierupgrade.soldierid[i] - 1);
		}
	}
    
	if(m_soldierupgrade.ischg)
    {
        countMilitaryRank();
		HttpClient::getSingleton().updateUpgradeSoldier(m_soldierupgrade);
        
        onRefurbishAllTroophousing();
    }	
}

// 改变自己的士兵等级
void LogicInfo::chgMySoldierLevel(int soldierid)
{
	for(int i = 0; i < 10; ++i)
	{
		if(m_myInfo.soldierlevel[i] / 100 == soldierid / 100)
		{
			m_myInfo.soldierlevel[i] = soldierid;
            
			return;
		}
	}
    
	for(int i = 0; i < 10; ++i)
	{
		if(m_myInfo.soldierlevel[i] == 0)
		{
			m_myInfo.soldierlevel[i] = soldierid;
            
			return;
		}
	}
}
//获得建筑的某个等级以上的总数量
int LogicInfo::getBuildNumsByBid(int bid)
{
    int num = 0;
    for(PlayerScene::_BuildingList::iterator it = PlayerScene::getSingleton().m_lstBuilding.begin(); it != PlayerScene::getSingleton().m_lstBuilding.end(); ++it)
    {
        Building *pBuilding = *it;
        if(pBuilding->m_pBuildingInfo->buildingid/100 == bid/100 && pBuilding->m_pBuildingInfo->buildingid >= bid && pBuilding->m_pBuildingInfo->state == 2)
        {
            num++;
        }
    }
    
    return num;
}
//获得某个建筑的最大等级
int LogicInfo::getBuildMaxLevel(int bid)
{
    int maxlevel = 0;
    for(PlayerScene::_BuildingList::iterator it = PlayerScene::getSingleton().m_lstBuilding.begin(); it != PlayerScene::getSingleton().m_lstBuilding.end(); ++it)
    {
        Building *pBuilding = *it;
        if(pBuilding->m_pBuildingInfo->buildingid/100 == bid && pBuilding->m_pBuildingInfo->state == 2)
        {
            int level = pBuilding->m_pBuildingInfo->buildingid%100;
            if(level > maxlevel)
                maxlevel = level;
        }
    }
    
    return maxlevel;
}
// 获得自己某个士兵的当前等级
int LogicInfo::getMySoldierLevel(int soldierid)
{
	for(int i = 0; i < 10; ++i)
	{
		if(m_myInfo.soldierlevel[i] / 100 == soldierid / 100)
		{
			return m_myInfo.soldierlevel[i];
		}
	}
    
	return soldierid / 100 * 100 + 1;
}

int LogicInfo::getSoldierMaxLevel(int index)
{
    int max = 1;
    
    while (PersonMgr::getSingleton().getSoldierInfo(10000 + (index + 1) * 100 + max) != NULL)
    {
        max ++;
    }
    
    max --;
    
    return max;
}

//// 关闭实验室界面
//void LogicInfo::onLabClose()
//{
//	if(m_soldierupgrade.ischg)
//		HttpClient::getSingleton().updateUpgradeSoldier(m_soldierupgrade);
//}

//! 战斗结束，统计成就
void LogicInfo::onAchievement_WarEnd(bool isWin, int wall, int townhall, int worker, int mortar, int gold, int oil, int cup)
{
	if(isWin)
		m_achievement.warwin++;
    
	m_achievement.warwall += wall;
	m_achievement.wartownhall += townhall;
	m_achievement.warworker += worker;
	m_achievement.warmortar += mortar;
	m_achievement.wargold += gold;
	m_achievement.warelixir += oil;
    
	if(m_achievement.cup < cup)
		m_achievement.cup = cup;
    
	onAchievementChg();
}

//! 移除杂物成功，统计成就
void LogicInfo::onAchievement_RemoveOtherOK()
{
	m_achievement.otherbuilding++;
    
	onAchievementChg();
}

//! 建造成功，统计成就
void LogicInfo::onAchievement_BuildingComplete(int buildingid)
{
	int bid = buildingid / 100;
	int level = buildingid % 100;
	if(bid == BUILDING_GOLD_STORAGE)
	{
		if(m_achievement.goldstorage < level)
		{
			m_achievement.goldstorage = level;
            
			onAchievementChg();
		}
	}
	else if(bid == BUILDING_TOWN_HALL)
	{
		if(m_achievement.townhall < level)
		{
			m_achievement.townhall = level;
            
			onAchievementChg();
		}
	}
	else if(bid == BUILDING_BARRACKS)
	{
		if(m_achievement.barrack < level)
		{
			m_achievement.barrack = level;
            
			onAchievementChg();
		}
	}
	else if(bid == 305)
	{
		if(m_achievement.alliancecastle < level)
		{
			m_achievement.alliancecastle = level;
            
			onAchievementChg();
		}
	}
}

//! 移除杂物成功
void LogicInfo::onRemoveOtherOK(int lx, int ly)
{
	onAchievement_RemoveOtherOK();
    
	for(std::list<UserBuildingInfo*>::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if((*it)->x == lx && (*it)->y == ly)
		{
			PlayerScene::getSingleton().onRemoveBuilding(*it);
            
            if (PlayerScene::getSingleton().m_pGameWorld->m_pMainEditUI) {
                PlayerScene::getSingleton().m_pGameWorld->m_pMainEditUI->onRemoveOtherOK(*it);
            }
            
			it = m_lstBuilding.erase(it);
            
			break;
		}
	}
}

//! 移除装饰物成功
void LogicInfo::onRemoveOrnamentOK(int lx, int ly)
{
	for(std::list<UserBuildingInfo*>::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if((*it)->x == lx && (*it)->y == ly)
		{
			PlayerScene::getSingleton().onRemoveBuilding(*it);
            
			it = m_lstBuilding.erase(it);
            
			break;
		}
	}
}

// 成就从0开始排
const char* LogicInfo::getAchievementString(int iAchievement)
{
	switch(iAchievement)
	{
        case 0:
            return "goldstorage";
        case 1:
            return "pvestar";
        case 2:
            return "townhall";
        case 3:
            return "otherbuilding";
        case 4:
            return "barrack";
        case 5:
            return "wargold";
        case 6:
            return "warelixir";
        case 7:
            return "cup";
        case 8:
            return "alliancecastle";
        case 9:
            return "warwall";
        case 10:
            return "wartownhall";
        case 11:
            return "warworker";
        case 12:
            return "warwin";
        case 13:
            return "defwin";
        case 14:
            return "givesoldier";
        case 15:
            return "warmortar";
        case 16:
            return "wardarkelixir";
	}
    
	return NULL;
}

void LogicInfo::requireAchievementAward(int iAchievement)
{
	const char* str = getAchievementString(iAchievement);
	if(str == NULL)
		return;
    
	int val = getAchievementVal(iAchievement);
	int star = getAchievementStar(iAchievement);
	const AchievementNode* pNode = getCurAchievementNode(iAchievement);
	if(pNode == NULL)
		return;
    
	if(val >= pNode->val)
	{
		HttpClient::getSingleton().requireAchievementAward(str);
		UpgradeAchievementStar(iAchievement);
		addExp(pNode->exp);
		chgGem(pNode->gem);
        
		onNotify(NOTIFY_BASEINFO);
		onNotify(NOTIFY_ACHIEVEMENT_OK);
	}
}

void LogicInfo::onAchievementChg()
{
	onNotify(NOTIFY_ACHIEVEMENT);
    
	if(countAchievementNums())
		onNotify(NOTIFY_ACHIEVEMENT_OK);
}

int LogicInfo::countAchievementNums()
{
	int nums = 0;
    
	for(int i = 0; i < 16; ++i)
	{
        if (i != 8 && i != 14) { //unopen
            if(canRequireAchievementAward(i))
                ++nums;
        }
        
	}
    
	return nums;
}

int LogicInfo::getAchievementVal(int iAchievement)
{
	switch(iAchievement)
	{
        case 0:
            return m_achievement.goldstorage;
        case 1:
            return m_achievement.pvestar;
        case 2:
            return m_achievement.townhall;
        case 3:
            return m_achievement.otherbuilding;
        case 4:
            return m_achievement.barrack;
        case 5:
            return m_achievement.wargold;
        case 6:
            return m_achievement.warelixir;
        case 7:
            return m_achievement.cup;
        case 8:
            return m_achievement.alliancecastle;
        case 9:
            return m_achievement.warwall;
        case 10:
            return m_achievement.wartownhall;
        case 11:
            return m_achievement.warworker;
        case 12:
            return m_achievement.warwin;
        case 13:
            return m_achievement.defwin;
        case 14:
            return m_achievement.givesoldier;
        case 15:
            return m_achievement.warmortar;
        case 16:
            return m_achievement.wardarkelixir;
	}
    
	return -1;
}

void LogicInfo::UpgradeAchievementStar(int iAchievement)
{
	switch(iAchievement)
	{
        case 0:
            ++m_achievement.goldstorage_star;
            break;
        case 1:
            ++m_achievement.pvestar_star;
            break;
        case 2:
            ++m_achievement.townhall_star;
            break;
        case 3:
            ++m_achievement.otherbuilding_star;
            break;
        case 4:
            ++m_achievement.barrack_star;
            break;
        case 5:
            ++m_achievement.wargold_star;
            break;
        case 6:
            ++m_achievement.warelixir_star;
            break;
        case 7:
            ++m_achievement.cup_star;
            break;
        case 8:
            ++m_achievement.alliancecastle_star;
            break;
        case 9:
            ++m_achievement.warwall_star;
            break;
        case 10:
            ++m_achievement.wartownhall_star;
            break;
        case 11:
            ++m_achievement.warworker_star;
            break;
        case 12:
            ++m_achievement.warwin_star;
            break;
        case 13:
            ++m_achievement.defwin_star;
            break;
        case 14:
            ++m_achievement.givesoldier_star;
            break;
        case 15:
            ++m_achievement.warmortar_star;
            break;
        case 16:
            ++m_achievement.wardarkelixir_star;
            break;
	}
}

int LogicInfo::getAchievementStar(int iAchievement)
{
	switch(iAchievement)
	{
        case 0:
            return m_achievement.goldstorage_star;
        case 1:
            return m_achievement.pvestar_star;
        case 2:
            return m_achievement.townhall_star;
        case 3:
            return m_achievement.otherbuilding_star;
        case 4:
            return m_achievement.barrack_star;
        case 5:
            return m_achievement.wargold_star;
        case 6:
            return m_achievement.warelixir_star;
        case 7:
            return m_achievement.cup_star;
        case 8:
            return m_achievement.alliancecastle_star;
        case 9:
            return m_achievement.warwall_star;
        case 10:
            return m_achievement.wartownhall_star;
        case 11:
            return m_achievement.warworker_star;
        case 12:
            return m_achievement.warwin_star;
        case 13:
            return m_achievement.defwin_star;
        case 14:
            return m_achievement.givesoldier_star;
        case 15:
            return m_achievement.warmortar_star;
        case 16:
            return m_achievement.wardarkelixir_star;
	}
    
	return -1;
}

bool LogicInfo::canRequireAchievementAward(int iAchievement)
{
    const AchievementNode* pNode = getCurAchievementNode(iAchievement);
	if(pNode == NULL)
		return false;
    
    int val = pNode->val;
    switch(iAchievement)
	{
        case 0:
            return m_achievement.goldstorage >= val;
        case 1:
            return m_achievement.pvestar >= val;
        case 2:
            return m_achievement.townhall >= val;
        case 3:
            return m_achievement.otherbuilding >= val;
        case 4:
            return m_achievement.barrack >= val;
        case 5:
            return m_achievement.wargold >= val;
        case 6:
            return m_achievement.warelixir >= val;
        case 7:
            return m_achievement.cup >= val;
        case 8:
            return m_achievement.alliancecastle >= val;
        case 9:
            return m_achievement.warwall >= val;
        case 10:
            return m_achievement.wartownhall >= val;
        case 11:
            return m_achievement.warworker >= val;
        case 12:
            return m_achievement.warwin >= val;
        case 13:
            return m_achievement.defwin >= val;
        case 14:
            return m_achievement.givesoldier >= val;
        case 15:
            return m_achievement.warmortar >= val;
        case 16:
            return m_achievement.wardarkelixir >= val;
	}
    
    return false;
}

float LogicInfo::getAchievementPro(int iAchievement)
{
    //    float pro = 0.0f;
    
    const AchievementNode* pNode = getCurAchievementNode(iAchievement);
	if(pNode == NULL)
		return 0.0f;
    
    int val = pNode->val;
    switch(iAchievement)
	{
        case 0:
            return m_achievement.goldstorage / (float)val;
        case 1:
            return m_achievement.pvestar / (float)val;
        case 2:
            return m_achievement.townhall / (float)val;
        case 3:
            return m_achievement.otherbuilding / (float)val;
        case 4:
            return m_achievement.barrack / (float)val;
        case 5:
            return m_achievement.wargold / (float)val;
        case 6:
            return m_achievement.warelixir / (float)val;
        case 7:
            return m_achievement.cup / (float)val;
        case 8:
            return m_achievement.alliancecastle / (float)val;
        case 9:
            return m_achievement.warwall / (float)val;
        case 10:
            return m_achievement.wartownhall / (float)val;
        case 11:
            return m_achievement.warworker / (float)val;
        case 12:
            return m_achievement.warwin / (float)val;
        case 13:
            return m_achievement.defwin / (float)val;
        case 14:
            return m_achievement.givesoldier / (float)val;
        case 15:
            return m_achievement.warmortar / (float)val;
        case 16:
            return m_achievement.wardarkelixir / (float)val;
	}
    
    return 0.0f;
}

const AchievementNode* LogicInfo::getCurAchievementNode(int iAchievement)
{
	int star = getAchievementStar(iAchievement);
	if(star < 0)
		return NULL;
    
	return AchievementInfo::getSingleton().getNode((iAchievement + 1) * 100 + star);
}

int LogicInfo::getFindGold()
{
//    int gold;
//    int level = m_myBaseLevel;
//    if(level == 1)
//        gold = 10;
//    else if(level == 2)
//        gold = 50;
//    else if(level == 3)
//        gold = 75;
//    else if(level == 4)
//        gold = 110;
//    else if(level == 5)
//        gold = 170;
//    else if(level == 6)
//        gold = 250;
//    else if(level == 7)
//        gold = 380;
//    else if(level == 8)
//        gold = 580;
//    else if(level == 9)
//        gold = 750;
//    else if(level == 10)
//        gold = 1000;
//    else if(level == 11)
//        gold = 1330;
//    else if(level == 12)
//        gold = 1730;
//    else if(level == 13)
//        gold = 2160;
//    else if(level == 14)
//        gold = 2790;
    
    int gold = TownhallLevelInfo::getSingleton().getAttackCost(m_myBaseLevel);
    
    return gold;
}


bool LogicInfo::isGoldFull(int curGold)
{
	return m_myInfo.goldex >= m_myInfo.maxgoldex;
}

bool LogicInfo::isOilFull(int curOil)
{
	return m_myInfo.oilex >= m_myInfo.maxoilex;
}

bool LogicInfo::isEmptyBarrack(int barrackid)
{
	MapBarrackInfo::iterator it = m_mapBarrack.find(barrackid);
	if(it != m_mapBarrack.end())
	{
		return it->second.lstSoldier.empty();
	}
    
	return false;
}

bool LogicInfo::isFullTroophousing()
{
    int maxnums = 0;
    int curspace = 0;
    MapTroophousingInfo::iterator it;
    for (it = m_mapTroophousing.begin(); it != m_mapTroophousing.end(); it++)
    {
        for(std::list<TroophousingSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
		{
			curspace += it1->spaceex * it1->numsex;
		}
        
        maxnums += it->second.maxnumsex;
    }
    
    if (curspace >= maxnums) {
        return true;
    }else{
        return false;
    }
}

int LogicInfo::getSoldierlevel(int index)
{
    for(int i = 0; i < 10; ++i)
    {
        if((m_myInfo.soldierlevel[i] / 100) % 100 == index + 1)
        {
            return m_myInfo.soldierlevel[i] % 100;
        }
    }
    
    return 1;
}

// 初始化内购数据
void LogicInfo::initInPurchase()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
	addInPurchaseProductInfo(PURCHASE_PRODUCTID_1, -1, PURCHASE_KEY_1, PURCHASE_PRICE_1);
	addInPurchaseProductInfo(PURCHASE_PRODUCTID_2, -1, PURCHASE_KEY_2, PURCHASE_PRICE_2);
	addInPurchaseProductInfo(PURCHASE_PRODUCTID_3, -1, PURCHASE_KEY_3, PURCHASE_PRICE_3);
	addInPurchaseProductInfo(PURCHASE_PRODUCTID_4, -1, PURCHASE_KEY_4, PURCHASE_PRICE_4);
	addInPurchaseProductInfo(PURCHASE_PRODUCTID_5, -1, PURCHASE_KEY_5, PURCHASE_PRICE_5);
	addInPurchaseProductInfo(PURCHASE_PRODUCTID_6, -1, PURCHASE_KEY_6, PURCHASE_PRICE_6);
#else
	addInPurchaseProductInfo(PURCHASE_PRODUCTID_1, -1, "1", PURCHASE_PRICE_1);
	addInPurchaseProductInfo(PURCHASE_PRODUCTID_2, -1, "2", PURCHASE_PRICE_2);
	addInPurchaseProductInfo(PURCHASE_PRODUCTID_3, -1, "3", PURCHASE_PRICE_3);
	addInPurchaseProductInfo(PURCHASE_PRODUCTID_4, -1, "4", PURCHASE_PRICE_4);
	addInPurchaseProductInfo(PURCHASE_PRODUCTID_5, -1, "5", PURCHASE_PRICE_5);
	addInPurchaseProductInfo(PURCHASE_PRODUCTID_6, -1, "6", PURCHASE_PRICE_6);
#endif
}

//! 添加商品
void LogicInfo::addInPurchaseProductInfo(int productID, int gem, const char* strPlatformProductID, const char* strPrice)
{
	std::pair<int, InPurchaseProductInfo> p;
    
	p.first = productID;
    
	p.second.productID = productID;
	p.second.gem = gem;
	p.second.strPlatformProductID = strPlatformProductID;
	p.second.strPrice = strPrice;
    
	m_lstInPurchaseProductInfo.insert(p);
}

//! 商品
void LogicInfo::updInPurchaseProductInfo(int productID, int gem, const char* strPrice)
{
	std::map<int, InPurchaseProductInfo>::iterator it = m_lstInPurchaseProductInfo.find(productID);
	if(it != m_lstInPurchaseProductInfo.end())
	{
		it->second.gem = gem;
		it->second.strPrice = strPrice;
	}
}

// 开始内购，如果发现前面一个内购未结束，则返回失败
bool LogicInfo::InPurchase(int productID)
{
	if(m_curInPurchaseProductID > 0)
		return false;
    
	std::map<int, InPurchaseProductInfo>::iterator it = m_lstInPurchaseProductInfo.find(productID);
	if(it == m_lstInPurchaseProductInfo.end())
		return false;
    
	m_curInPurchaseProductID = productID;
    
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    if (m_payType == PAY_TYPE_PAYPAL) {
        
        string paypal = CCStringFormat(PAYPAL_URL, SERVER_VERSION, m_myInfo.uid, m_curInPurchaseProductID);
        
        InPurchase::sharedInPurchase()->makePayment(paypal, m_payType, this, purchase_callback(LogicInfo::onInPurchaseComplete));
        
        m_curInPurchaseProductID = -1;
    }
    else {
        InPurchase::sharedInPurchase()->makePayment(it->second.strPlatformProductID, m_payType, this, purchase_callback(LogicInfo::onInPurchaseComplete));
    }
    
#else
    InPurchase::sharedInPurchase()->makePayment(it->second.strPlatformProductID, this, purchase_callback(LogicInfo::onInPurchaseComplete));
#endif
    
	return true;
}

// 内购完成
void LogicInfo::onInPurchaseCompleteImp(std::string strPlatformProductID, bool isSuccess, std::string result)
{
	std::map<int, InPurchaseProductInfo>::iterator it = m_lstInPurchaseProductInfo.find(m_curInPurchaseProductID);
	if(it != m_lstInPurchaseProductInfo.end())
	{
		if(!isSuccess || result.empty())
		{
            m_asyncViewList.insert(pair<int, int>(ASYNC_VIEW_PAY_ERROR, -1));
            m_curInPurchaseProductID = -1;
            
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
            HttpClient::getSingleton().pay3(m_curInPurchaseProductID, false, result.c_str());
            
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
            if(m_payType == PAY_TYPE_AMAZON)
            {
            	HttpClient::getSingleton().pay4(m_curInPurchaseProductID, false, result.c_str());
            }
            else
            {
            	HttpClient::getSingleton().pay2(m_curInPurchaseProductID, false, result.c_str());
            }
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
            HttpClient::getSingleton().pay2(m_curInPurchaseProductID, false, result.c_str());
#endif
		}
		else
		{
			Json::Reader r;
			Json::Value val;
            
			if(r.parse(result, val))
			{
				std::string orderid;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
				orderid = val["receipt-data"].asString();
				HttpClient::getSingleton().pay3(m_curInPurchaseProductID, true, orderid.c_str());
                
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
				if(m_payType == PAY_TYPE_AMAZON)
				{
				    HttpClient::getSingleton().pay4(m_curInPurchaseProductID, true, result.c_str());
				}
				else
				{
				  	HttpClient::getSingleton().pay2(m_curInPurchaseProductID, true, result.c_str());
				}
				            
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
				orderid = "win32";
				orderid += result;
                
				HttpClient::getSingleton().pay2(m_curInPurchaseProductID, true, "{\"receipt\":{\"purchaseInfo\":\"{\\\"orderId\\\":\\\"12999763169054705758.1369897403007238\\\",\\\"packageName\\\":\\\"com.koolgig.gob\\\",\\\"productId\\\":\\\"gems_01_tire2_gameofbattles0122\\\",\\\"purchaseTime\\\":1392613797142,\\\"purchaseState\\\":0,\\\"developerPayload\\\":\\\"cc5d752cf52a775b4f7492ddf649de66\\\",\\\"purchaseToken\\\":\\\"vmwokcaoikgzunrunutysyxy.AO-J1OzSRvi1Ka-V--Lv0ZEqDzge6PFZkI8IsqFsHDO0-JfIXhC6500ZcY2cEgnkY43_K7-tjHk_oX_bbvX8GKpQKx1CVXSaUo3-ZMD98U5h1ZuvTDoUZE8TMtrTaUQTEtns38DIQpbHUmRfsKUTqv3x3ZBN1BtumA\\\"}\",\"signature\":\"E6gIKXbw+pwIRj\/lbMNvFQhgNAQMU5O5pTKuX4vQfEvE9XLrFuDXwm8lmszqg3ObmlwDasibWk8HDS4Mfa5HhQzb9k1an2QhjqICFHR6pPsRZ4dgelAlgZ6D7SLIAHThXwN6Jjvyf6eB1\/KBUSUmwMG7sW31FU+7wqKz+lBykZOjBdF\/hYmouFF72jOI96j6kkZ1j1bltcHMA+NM90ja3Y982j27YKQVlz0IGnkpxRgzEMAH+rZGDukAlgi0ZoN7oAIqOquMw6w4H9unfXPpsEQr4B+E64HKiPKvMfp9ddFuVLM3iHbLX4VVJp6ylJIIFuCHsEKFPtV79EEdAXlZOw==\"}}");
#endif
                
                m_asyncViewList.insert(pair<int, int>(ASYNC_VIEW_PAY_OK, -1));
            }
            else{
                m_asyncViewList.insert(pair<int, int>(ASYNC_VIEW_PAY_ERROR, -1));
            }
        }
	}
}

// 内购完成
void LogicInfo::onInPurchaseComplete(std::string strPlatformProductID, bool isSuccess, std::string result)
{
    if(InPurchase::sharedInPurchase()->purchaseType == PURCHASE_RESUME)
    {
        if (isSuccess && !strPlatformProductID.empty()) {
            
            std::map<int, InPurchaseProductInfo>::iterator it = m_lstInPurchaseProductInfo.begin();
            for (; it != m_lstInPurchaseProductInfo.end(); it++) {
                
                if (strPlatformProductID == it->second.strPlatformProductID ) {
                    m_curInPurchaseProductID = it->second.productID;
                    break;
                }
            }
        }
    }
    
	LogicInfo::getSingleton().onInPurchaseCompleteImp(strPlatformProductID, isSuccess, result);
}

void LogicInfo::onInPurchaseVerifyComplete(int productId, int status)
{
    if (status == 1) {
        std::map<int, InPurchaseProductInfo>::iterator it = m_lstInPurchaseProductInfo.find(productId);
        
        if(m_iapGiftInfo.iapDoneForGift(m_curInPurchaseProductID))
        {
            if(PlayerScene::getSingleton().m_pGameWorld->m_pGiftUI != NULL)
            {
                PlayerScene::getSingleton().m_pGameWorld->m_pGiftUI->refreshInfo();
            }
            else
            {
                PlayerScene::getSingleton().m_pGameWorld->showGiftUI();
            }
        }
        
        m_myInfo.mask[0] |= USER_MASK1_ALREADYPAY;
        
        onNotify(NOTIFY_PAY_OK, &it->second.gem);
        CAppPlatform::sharedAppPlatform()->logMobileAppEvent(it->second.strPlatformProductID.c_str(), it->second.price);
        
        SoundMgr::getSingleton().playEffect("purchasesucceeded.m4a", 0);
    }
    else {
        onNotify(NOTIFY_PAY_FAILED);
        
        SoundMgr::getSingleton().playEffect("purchasefailed.m4a", 0);
    }
    
    m_curInPurchaseProductID = -1;
}

int LogicInfo::protectTimeGem(int day)
{
    int gem = 0;
    if (day == 1) {
        gem = 100;
    }else if(day == 2){
        gem = 150;
    }else if (day == 7){
        gem = 250;
    }

    return gem;
}

bool LogicInfo::payProtectTime(int day)
{
    int gem = protectTimeGem(day);
    
	if(getMoney(MONEY_GEM) < gem)
    {
		return false;
    }
	HttpClient::getSingleton().payProtectTime(day);
    
    chgGem(-gem);
	m_myInfo.lastProtectTime += day * 24 * 60 * 60;
    
	onNotify(NOTIFY_BASEINFO);
    
    return true;
}

//! 请求Google+宝石
void LogicInfo::requestGooglePlusGem()
{
    HttpClient::getSingleton().requestGetGem(6);
    
    chgGem(5);
    
    m_myInfo.mask[0] |= USER_MASK1_GOOGLE_PLUS;
}

//! 请求评价宝石
void LogicInfo::requestGradeGem()
{
	HttpClient::getSingleton().requestGradeGem();
    
	chgGem(10);
    
	m_myInfo.mask[0] |= USER_MASK1_GRADEGEM;
}

//! 请求首充宝石
void LogicInfo::requestFirstPayGem()
{
	HttpClient::getSingleton().requestFirstPayGem();
    
	chgGem(200);
    
	m_myInfo.mask[0] |= USER_MASK1_ALREADYPAYRETURN;
}

void LogicInfo::requestAdsGem(int value)
{
    HttpClient::getSingleton().requestChgGemOnly(value);
//    m_asyncViewList.insert(pair<int, int>(ASYNC_VIEW_ADD_GEM, value));
}

void LogicInfo::requestChartboostFlag()
{
    HttpClient::getSingleton().requestAdFreeGemFlag("chartboost");
}

void LogicInfo::requestChartboostClickGems()
{
    HttpClient::getSingleton().requestChartboostClickGems();
}

bool LogicInfo::canUpgradeLaboratry(Building& building)
{
	return getCurUpgradeSoldier() == -1;
}

void LogicInfo::chg2Replay(FightMsg msg, int attackType)
{
    m_infoFightScore.user[0] = msg.fsi.user[0];
    m_infoFightScore.user[1] = msg.fsi.user[1];
    
    m_logFightMsg = msg;
    
	chg2Wait();
    
    if (attackType == ATTACK_TYPE_PVP) {
        HttpClient::getSingleton().requestBattleLogDetail(msg.msgid);
    }
    else if (attackType == ATTACK_TYPE_CLANWAR) {
        HttpClient::getSingleton().requestClanWarBattleLog(msg.msgid);
    }
}

void LogicInfo::setClearTouchFunc(CCObject* pTarget_ClearTouch, SEL_CallFunc pFuncClearTouch)
{
	m_pFunc_ClearTouch = pFuncClearTouch;
	m_pTarget_ClearTouch = pTarget_ClearTouch;
}

int LogicInfo::countSoldierNums()
{
	int nums = 0;
	for(MapTroophousingInfo::iterator it = m_mapTroophousing.begin(); it != m_mapTroophousing.end(); ++it)
	{
		for(std::list<TroophousingSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
		{
			nums += it1->spaceex * it1->numsex;
		}
	}
    
	return nums;
}

void LogicInfo::onProcAsyncView()
{
    map<int, int>::const_iterator it = m_asyncViewList.begin();
    
    for (; it != m_asyncViewList.end(); it++) {
        int asyncView = it->first;
        if (asyncView == ASYNC_VIEW_PAY_OK) {
            MessageUI::show(MSG_PURCHASE_WAIT, ViewString::getValue("order_confirm_title").c_str(), ViewString::getValue("order_confirm_message").c_str());
        }
        else if (asyncView == ASYNC_VIEW_PAY_ERROR) {
            MessageUI::show(NOTIFY_PAY_ERROR, "", ViewString::getSingleton().getValue("buygem_fail").c_str(), -1);
        }
        else if (asyncView == ASYNC_VIEW_ADD_GEM) {
        	if(m_addGemType == ADD_GEM_FB_LOGIN)
        		FacebookMgr::getSingleton().m_isGetFreeGemLogin = false;
            int gem = it->second;
            chgGem(gem);
        }
        else if (asyncView == ASYNC_FB_LOGIN)
        {
            onNotify(NOTIFY_FB_LOGIN_OK);
        }
        else if (asyncView == ASYNC_FB_GET_PIC)
        {
            LogicInfo::getSingleton().onNotify(NOTIFY_FB_GET_PIC_OK);
        }
        else if (asyncView == ASYNC_FB_INVITE)
        {
            LogicInfo::getSingleton().onNotify(NOTIFY_FB_INVITE_OK);
        }
        else if (asyncView == ASYNC_FB_POST)
        {
            PlayerScene::getSingleton().addNote("Posting to Facebook is successful!");
        }
    }
    m_addGemType = ADD_GEM_NORMAL;
	m_asyncViewList.clear();
}

void LogicInfo::checkUserCenterBind()
{
    if (!m_isCheckUserBind && m_uidGameCenter.length() > 0) {
        
        HttpClient::getSingleton().checkUserCenterBind(m_uidGameCenter.c_str());
        m_isCheckUserBind = true;
    }
    
}

void LogicInfo::rebindGameCenter(bool isChg)
{
	if(isChg)
	{
		if(m_uidGameCenterBind == 0)
		{
			HttpClient::getSingleton().rebindGameCenter(m_uidGameCenter.c_str(), m_myInfo.uid);
		}
		else
		{            
			HttpClient::getSingleton().rebindGameCenter(m_uidGameCenter.c_str(), m_uidGameCenterBind);
			HelloWorld::showEx();
		}
	}
	else
	{
		HttpClient::getSingleton().rebindGameCenter(m_uidGameCenter.c_str(), m_myInfo.uid);
	}
}

void LogicInfo::onGetDeviceUserName(const char* username, const char* nickname)
{
	m_defaultUserName = nickname;
    m_uidGameCenter = username;
    
    if (PlayerScene::getSingleton().m_pGameWorld) {
        if(m_isLogin)
        {
            m_isCheckUserBind = false;
            checkUserCenterBind();
        }
    }
}

// 兵营是否能募兵
bool LogicInfo::canRecruitSoldier(int soldierid, bool isOther)
{
	const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(soldierid);
	if(pInfo == NULL)
		return false;
    
	if(isOther)
	{
		if(pInfo->barrackLevelex > m_otherInfo.maxBarrackLevel)
			return false;
	}
	else
	{
		if(pInfo->barrackLevelex > m_myInfo.maxBarrackLevel)
			return false;
	}
    
	return true;
}

const InPurchaseProductInfo* LogicInfo::getIAPInfo(int index)
{
	if(index >= 0 && index < m_lstIAPIndex.size())
	{
		int productID = m_lstIAPIndex[index];
		std::map<int, InPurchaseProductInfo>::iterator it = m_lstInPurchaseProductInfo.find(productID);
		if(it != m_lstInPurchaseProductInfo.end())
			return &(it->second);
	}
    
	return NULL;
}

const InPurchaseProductInfo* LogicInfo::getIAPInfoFromProductID(int productID)
{
    std::map<int, InPurchaseProductInfo>::iterator it = m_lstInPurchaseProductInfo.find(productID);
    if(it != m_lstInPurchaseProductInfo.end())
        return &(it->second);
    
	return NULL;
}

void LogicInfo::chgGuideVal(int val)
{
	if(val <= m_zyval)
		return;
    
	m_zyval = val;
    
	HttpClient::getSingleton().updGuideVal(m_zyval);
}

void LogicInfo::selectHero(int heroid)
{
	HeroMgr::getSingleton().m_myHero.heroid = heroid;
    
	HttpClient::getSingleton().selectHero(heroid);
}

bool LogicInfo::isNeedDestroyBuidling(int buildingid)
{
	int bid = buildingid / 100;
    
	if(bid == BUILDING_WALL)
		return false;
    
    if(buildingid == 30501)
		return false;
    
	if(buildingid >= 50000)
		return false;
    
	return true;
}

bool LogicInfo::isCanDestroyBuidling(int buildingid)
{
	if(buildingid == 30501)
		return false;
    
	if(buildingid >= 50000)
		return false;
    
	return true;
}

//! 修正仓库的储量
void LogicInfo::fixStorageReserve(bool self)
{
	int store[2];
    int res[2] = {0, 0};
    
	std::list<Building*> lstbuilding;
	UserBuildingInfo* townhall = NULL;
    
	if(self)
	{
		store[0] = m_myInfo.goldex;
		store[1] = m_myInfo.oilex;
	}
	else
	{
		store[0] = m_otherInfo.goldex;
		store[1] = m_otherInfo.oilex;
	}
    
    lstbuilding = PlayerScene::getSingleton().m_lstBuilding;
    
    std::list<Building*> srbuilding[2];
	for(PlayerScene::_BuildingList::iterator it = lstbuilding.begin(); it != lstbuilding.end(); ++it)
	{
        (*it)->m_pBuildingInfo->store[0] = 0;
        (*it)->m_pBuildingInfo->store[1] = 0;
        (*it)->m_pBuildingInfo->laststore[0] = 0;
        (*it)->m_pBuildingInfo->laststore[1] = 0;
        
        int bid = (*it)->m_pBuildingInfo->buildingid / 100;
		if(bid == BUILDING_TOWN_HALL)
		{
			townhall = (*it)->m_pBuildingInfo;
		}
        else if(bid == BUILDING_GOLD_STORAGE)
        {
            srbuilding[0].push_back(*it);
        }
        else if(bid == BUILDING_ELIXIR_STORAGE)
        {
            srbuilding[1].push_back(*it);
        }
        else if (bid == BUILDING_GOLD_MINE)
        {
            int gold = (*it)->countRes();
            
            if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_PVE ||
                LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_CLANWAR ||
                LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_GROUPWAR) {
                gold = 0;
            }
            
            res[0] += gold;
            (*it)->m_pBuildingInfo->store[0] = gold;
            (*it)->m_pBuildingInfo->laststore[0] = gold;
        }
        else if (bid == BUILDING_ELIXIR_COLLECTOR)
        {
            int oil = (*it)->countRes();
            
            if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_PVE ||
                LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_CLANWAR ||
                LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_GROUPWAR) {
                oil = 0;
            }
            
            res[1] += oil;
            (*it)->m_pBuildingInfo->store[1] = oil;
            (*it)->m_pBuildingInfo->laststore[1] = oil;
        }
	}
    
	for(int i = 0; i < 2; ++i)
	{
		if(townhall != NULL)
		{
			if(store[i] <= townhall->maxstore[i])
			{
				townhall->store[i] = store[i];
                townhall->laststore[i] = townhall->store[i];
			}
			else
			{
				townhall->store[i] = townhall->maxstore[i];
                townhall->laststore[i] = townhall->store[i];
			}

			store[i] -= townhall->store[i];
		}
        
        int nums = srbuilding[i].size();
        
        while (store[i] > 0 && nums > 0) {
            
            int pnum = store[i] / nums;
            int anum = store[i] % nums;
            
            for(std::list<Building*>::iterator it = srbuilding[i].begin(); it != srbuilding[i].end(); ++it)
            {
                if ((*it)->m_pBuildingInfo->store[i] >= (*it)->m_pBuildingInfo->maxstore[i]) {
                    continue;
                }
                
                int _store = pnum + anum;
                if ((*it)->m_pBuildingInfo->maxstore[i] >= _store) {
                    
                    _store = pnum + anum;
                    
                    anum --;
                    if (anum < 0) {
                        anum = 0;
                    }
                }
                else {
                    
                    _store = (*it)->m_pBuildingInfo->maxstore[i] - (*it)->m_pBuildingInfo->store[i];
                    nums --;
                }
                
                (*it)->m_pBuildingInfo->store[i] += _store;
                (*it)->m_pBuildingInfo->laststore[i] = (*it)->m_pBuildingInfo->store[i];
                
                store[i] -= _store;
                
                Building_Storage* building = (Building_Storage*)(*it);
                building->onChgStoreRes();
            }
        }
	}
    
    m_otherInfo.goldex += res[0];
    m_otherInfo.oilex += res[1];
}

//! 防守士兵死亡
void LogicInfo::deadDefenceSoldier(int pid)
{
    ClanMgr::getSingleton().m_fightDefendSoldier.push_back(pid);
}

void LogicInfo::chg2Wait()
{
    m_curState = LOGICSTATE_WAIT;
    
    if (m_isGoHome) {
        return;
    }
    
    onRefurbishAllBarrack();
    onRefurbishAllTroophousing();
}

#ifdef COC_MODE_EDIT

void LogicInfo::load()
{
	FILE* fp = fopen("output.json", "r");
	if(fp != NULL)
	{
		fseek(fp, -1, SEEK_END);
		int len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
        
		char* str = new char[len + 1];
        
		fread(str, len, 1, fp);
		str[len] = '\0';
        
		Json::Reader r;
		Json::Value val;
        
		if(r.parse(str, val))
		{
			if(val["cid"].asString() == "cmbmsg")
			{
				int nums = val["nums"].asInt();
				for(int i = 0; i < nums; ++i)
				{
					char strname[32];
					sprintf(strname, "c%d", i);
                    
					onProc(val[strname]);
				}
			}
			else
				onProc(val);
		}
        
		fclose(fp);
	}
}

void LogicInfo::save()
{
	FILE* fp = fopen("output.json", "w");
	if(fp != NULL)
	{
		Json::FastWriter writer;
		Json::Value root;
        
		root["cid"] = Json::Value("cmbmsg");
		root["nums"] = Json::Value(m_lstBuilding.size());
        
		int i = 0;
		for(std::list<UserBuildingInfo*>::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
		{
			const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo((*it)->buildingid);
			if(pInfo == NULL)
				continue;
            
			Json::Value b;
            
			b["cid"] = Json::Value("building");
			b["bid"] = Json::Value((*it)->buildingid);
			b["x"] = Json::Value((*it)->x);
			b["y"] = Json::Value((*it)->y);
			b["state"] = Json::Value(2);
            
			b["lasttime"] = Json::Value(0);
			b["curtime"] = Json::Value(0);
            
			b["maxres"] = pInfo->maxstored[pInfo->producesres - 1];
			b["currestime"] = Json::Value(0);
			b["timeres"] = pInfo->producesperhour;
            
			b["storegold"] = pInfo->maxstored[0];
			b["storeoil"] = pInfo->maxstored[1];
			b["maxstoregold"] = pInfo->maxstored[0];
			b["maxstoreoil"] = pInfo->maxstored[1];
            
			char str[128];
			sprintf(str, "c%d", i);
			++i;
            
			root[str] = b;
		}
		//root =
        
		fprintf(fp, writer.write(root).c_str());
        
		fclose(fp);
	}
}

void LogicInfo::addBuildingEx(int bid, int x, int y)
{
	UserBuildingInfo* pbi = new UserBuildingInfo;
    
	pbi->buildingid = getAsInt(bid);
	pbi->state = getAsInt(2);
	pbi->lasttime = getAsInt(0);
	pbi->curtime = getAsInt(0);
	pbi->x = getAsInt(x);
	pbi->y = getAsInt(y);
    
	pbi->maxres = getAsInt(0);
	pbi->currestime = getAsInt(0);
	pbi->timeres = getAsInt(0);
    
	m_lstBuilding.push_back(pbi);
}

#endif // COC_MODE_EDIT

NS_COC_END
