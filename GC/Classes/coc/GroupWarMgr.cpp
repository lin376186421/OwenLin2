//
//  GroupWarMgr.cpp
//  GameClient
//
//  Created by qsun on 14-10-20.
//
//

#include "GroupWarMgr.h"
#include "LogicInfo.h"
#include "zlib.h"
#include "ClanMgr.h"
#include "../base32.h"
#include "../form/MessageUI.h"
#include "PlayerScene.h"
#include "GameWorldScene.h"

NS_COC_BEGIN

GroupWarMgr::GroupWarMgr()
: m_pSelectCityInfo(NULL)
, m_warId(0)
, m_selectDefendIndex(0)
, m_selectAttackClanId(0)
, m_warLogTipStarNums(0)
{
    
}

GroupWarMgr::~GroupWarMgr()
{
}

GroupWarMgr& GroupWarMgr::getSingleton()
{
	static GroupWarMgr s_mgr;
	return s_mgr;
}

int GroupWarMgr::getWarState()
{
    //    return GROUPWAR_STATE_BATTLE_START;
    
    if (m_warState == GROUPWAR_STATE_CLOSE || m_warState == GROUPWAR_STATE_END) {
        return m_warState;
    }
    
    int time = getCurrentTime();
    
    if (time >= m_warEndTime) {
        m_warState = GROUPWAR_STATE_BATTLE_SCORE;
    }
    else if (time >= m_warStartTime) {
        m_warState = GROUPWAR_STATE_BATTLE_START;
    }
    else if (time >= m_warPrepareTime) {
        m_warState = GROUPWAR_STATE_BATTLE_PREPARE;
    }
    else if (time >= m_warSignupTime) {
        m_warState = GROUPWAR_STATE_SIGNUP;
    }
    
    return m_warState;
}

bool GroupWarMgr::isWarStateOpen()
{
    int warState = getWarState();
    
    if (warState >= GROUPWAR_STATE_SIGNUP && warState < GROUPWAR_STATE_END) {
        return true;
    }
    
    return false;
}

bool GroupWarMgr::isWarStateBattle()
{
    int warState = getWarState();
    
    if (warState >= GROUPWAR_STATE_BATTLE_PREPARE && warState < GROUPWAR_STATE_BATTLE_SCORE) {
        return true;
    }
    
    return false;
}

bool GroupWarMgr::onProc(const Json::Value &val)
{
    std::string cid = val["cid"].asString();
    
	if (cid == "groupwar_info")
    {
        m_warId = getAsInt(val["war_id"]);
        
        m_warState = getAsInt(val["state"]);
        
        m_warSignupTime = getAsInt(val["signup_start_time"]);
        m_warPrepareTime = getAsInt(val["prepare_start_time"]);
        m_warStartTime = getAsInt(val["battle_start_time"]);
        m_warEndTime = getAsInt(val["war_end_time"]);
        m_warNextTime = getAsInt(val["next_war_time"]);
        
        if (m_warState == 0) {
            m_warState = GROUPWAR_STATE_CLOSE;
        }
        else if (m_warState == 2) {
            m_warState = GROUPWAR_STATE_BATTLE_SCORE;
        }
        else if (m_warState == 3) {
            m_warState = GROUPWAR_STATE_END;
        }
        
        LogicInfo::getSingleton().onNotify(NOTIFY_GROUPWAR_INFO);
    }
    else if (cid == "groupwar_score") {
        
        m_warWin = getAsInt(val["iswin"]);
        
        m_warCityId = getAsInt(val["cityid"]);
        m_warCityName = val["cityname"].asString();
        m_warRanking = getAsInt(val["rank"]);
        m_warStar = getAsInt(val["star"]);
        m_warRankingGems = getAsInt(val["gems"]);
        m_warStarGems = getAsInt(val["stargems"]);
        m_warAttacker = getAsInt(val["attacker"]);
        m_warRewardId = getAsInt(val["rewardid"]);
        m_warStarRewardId = getAsInt(val["rewardid2"]);

        PlayerScene::getSingleton().m_pGameWorld->showGroupWarScoreUI();
    }
    else if (cid == "groupwar_defensemembers") {
        
        m_warDefenseUserList.clear();
        Json::Value members = val["members"];
        
        for (int i = 0; i < members.size(); i ++) {
            
            Json::Value userVal = members[i];
            
            ClanWarUserInfo info;
            info.uid = getAsInt(userVal["uid"]);
            info.uname = userVal["uname"].asString();
            info.cupex = getAsInt(userVal["cup"]);
            info.military = getAsInt(userVal["medal"]);
            info.curlevel = getAsInt(userVal["curlevel"]);
            info.baselevel = getAsInt(userVal["curbaselevel"]);
            info.goldex = getAsInt(userVal["gold"]);
            info.oilex = getAsInt(userVal["oil"]);
            info.destroyStar = getAsInt(userVal["destroystar"]);
            info.levelupStar = getAsInt(userVal["levelupstar"]);
            info.defenseLevel = getAsInt(userVal["difficulty_level"]);
            info.defenseRatio = userVal["difficulty_ratio"].asDouble();
            
            m_warDefenseUserList.push_back(info);
        }
        
        LogicInfo::getSingleton().onNotify(NOTIFY_GROUPWAR_DEFENSEINFO);
    }
    else if (cid == "groupwar_attackclans")
    {
        int attackclannums = getAsInt(val["attackclannums"]);
        
        m_warAttackClanList.clear();
        Json::Value clanList = val["clans"];
        
        int size = clanList.size();
        
        for (int i = 0; i < size; i++) {
            Json::Value clanVal = clanList[i];
            
            ClanWarAttackClanInfo info;
            info.rankingid = getAsInt(clanVal["rank"]);
            info.winstar = getAsInt(clanVal["winstar"]);
            info.laststartime = getAsInt(clanVal["laststartime"]);
            info.encodeInfo(clanVal);
            
            m_warAttackClanList.push_back(info);
        }
        
        LogicInfo::getSingleton().onNotify(NOTIFY_GROUPWAR_ATTACKCLANS, &attackclannums);
    }
    else if (cid == "groupwar_memberrank")
    {
        m_warBattleLog.clear();
        Json::Value members = val["members"];
        
        for (int i = 0; i < members.size(); i ++) {
            
            Json::Value userVal = members[i];
            
            GroupWarFightLog info;
            info.user.uid = getAsInt(userVal["uid"]);
            info.user.name = userVal["uname"].asString();
            info.user.military = getAsInt(userVal["military"]);
            info.winstar = getAsInt(userVal["win_stars"]);
            info.battlenums = getAsInt(userVal["battle_nums"]);
            info.laststartime = getAsInt(userVal["last_battle_time"]);
            
            m_warBattleLog.push_back(info);
        }
        
        LogicInfo::getSingleton().onNotify(NOTIFY_GROUPWAR_BATTLELOG);
    }
    else if (cid == "groupwar_attackinfo")
    {
        m_warAttackInfo.winStar = getAsInt(val["winstar"]);
        
        
        LogicInfo::getSingleton().onNotify(NOTIFY_GROUPWAR_BATTLEINFO);
    }
    else if (cid == "groupwar_starscore") {
        
        Json::Value starVal = val["warstarscore"];
        
        for (int i = 0; i < 3; i ++) {
            m_warStarScore[i].starLevel = getAsInt(starVal[i]["starlevel"]);
            m_warStarScore[i].starNums = getAsInt(starVal[i]["starnums"]);
            m_warStarScore[i].rewardGems = getAsInt(starVal[i]["rewardgems"]);
        }
    }
    
    return false;
}

ClanWarAttackClanInfo* GroupWarMgr::getAttackClanInfo(int clanid)
{
    for (int i = 0; i < m_warAttackClanList.size(); i ++) {
        if (m_warAttackClanList[i].id == clanid) {
            return &m_warAttackClanList[i];
        }
    }
    return NULL;
}

NS_COC_END