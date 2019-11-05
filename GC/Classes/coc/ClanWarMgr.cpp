//
//  ClanWarMgr.cpp
//  GameClient
//
//  Created by qsun on 14-9-4.
//
//

#include "ClanWarMgr.h"
#include "LogicInfo.h"
#include "zlib.h"
#include "ClanMgr.h"
#include "../base32.h"
#include "../form/MessageUI.h"
#include "../Utils/Base64.h"
#include "PlayerScene.h"
#include "GameWorldScene.h"

NS_COC_BEGIN

const int CLANWAR_NPC_CLAN_ID = 5688;

ClanWarMgr::ClanWarMgr()
: m_pSelectCityInfo(NULL)
, m_warId(0)
, m_selectDefendIndex(0)
, m_selectAttackClanId(0)
, m_warDefendCityId(0)
, m_warLogTipStarNums(0)
{

}

ClanWarMgr::~ClanWarMgr()
{
}

ClanWarMgr& ClanWarMgr::getSingleton()
{
	static ClanWarMgr s_mgr;
	return s_mgr;
}

int ClanWarMgr::getWarState()
{
//    return CLANWAR_STATE_BATTLE_START;
    
    if (m_warState == CLANWAR_STATE_CLOSE || m_warState == CLANWAR_STATE_END) {
        return m_warState;
    }
    
    int time = getCurrentTime();
    
    if (time >= m_warEndTime) {
        m_warState = CLANWAR_STATE_BATTLE_SCORE;
    }
    else if (time >= m_warStartTime) {
        m_warState = CLANWAR_STATE_BATTLE_START;
    }
    else if (time >= m_warPrepareTime) {
        m_warState = CLANWAR_STATE_BATTLE_PREPARE;
    }
    else if (time >= m_warSignupTime) {
        m_warState = CLANWAR_STATE_SIGNUP;
    }
    
    return m_warState;
}

bool ClanWarMgr::isWarStateOpen()
{
    int warState = getWarState();
    
    if (warState >= CLANWAR_STATE_SIGNUP && warState < CLANWAR_STATE_END) {
        return true;
    }
    
    return false;
}

bool ClanWarMgr::isWarStateBattle()
{
    int warState = getWarState();
    
    if (warState >= CLANWAR_STATE_BATTLE_PREPARE && warState < CLANWAR_STATE_BATTLE_SCORE) {
        return true;
    }
    
    return false;
}

bool ClanWarMgr::onProc(const Json::Value &val)
{
    std::string cid = val["cid"].asString();
    
	if (cid == "clanwar_info")
    {
        m_warId = getAsInt(val["war_id"]);
        m_warState = getAsInt(val["state"]);
        
        m_warMinWinStar = getAsInt(val["min_star_win"]);
        m_warClanMinMember = getAsInt(val["min_member_singup"]);
        
        m_warSignupTime = getAsInt(val["signup_start_time"]);
        m_warPrepareTime = getAsInt(val["prepare_start_time"]);
        m_warStartTime = getAsInt(val["battle_start_time"]);
        m_warEndTime = getAsInt(val["war_end_time"]);
        m_warNextTime = getAsInt(val["next_war_time"]);
        
        m_warSignupCityId = getAsInt(val["signup_cityid"]);
        
        if (m_warState == 0) {
            m_warState = CLANWAR_STATE_CLOSE;
        }
        else if (m_warState == 2) {
            m_warState = CLANWAR_STATE_BATTLE_SCORE;
        }
        else if (m_warState == 3) {
            m_warState = CLANWAR_STATE_END;
        }
        
        LogicInfo::getSingleton().onNotify(NOTIFY_CLANWAR_INFO);
    }
    else if (cid == "clanwar_city")
    {
        m_warCityList.clear();
        
        Json::Value cityList = val["city"];
        int size = cityList.size();
        
        for (int i = 0; i < size; i ++) {
            
            Json::Value cityVal = cityList[i];
            
            ClanCityInfo cityInfo;
            cityInfo.countryid = getAsInt(cityVal["countryid"]);
            cityInfo.cityid = getAsInt(cityVal["cityid"]);
            cityInfo.cityname = cityVal["cityname"].asString();
            cityInfo.citytype = getAsInt(cityVal["citytype"]);
            cityInfo.productgems = getAsInt(cityVal["productgem"]);
            cityInfo.wingems = getAsInt(cityVal["wingem"]);
            cityInfo.posx = getAsInt(cityVal["posx"]);
            cityInfo.posy = getAsInt(cityVal["posy"]);
            cityInfo.cursignnums = getAsInt(cityVal["cursignnums"]);
            cityInfo.maxsignnums = getAsInt(cityVal["maxsignnums"]);
            cityInfo.dayrewardid = getAsInt(cityVal["dayrawardid"]);
            cityInfo.clanInfo.encodeInfo(cityVal["claninfo"]);
            
            if (cityInfo.clanInfo.id == CLANWAR_NPC_CLAN_ID) {
                cityInfo.clanInfo.national = cityInfo.countryid;
            }
            
            if (cityInfo.clanInfo.id == ClanMgr::getSingleton().m_myclan.id && cityInfo.citytype == CLANWAR_CITY) {
                m_warDefendCityId = cityInfo.cityid;
            }
                        
            m_warCityList.push_back(cityInfo);
        }
        
        LogicInfo::getSingleton().onNotify(NOTIFY_CLANWAR_CITYINFO);
    }
    else if (cid == "clanwar_signsuccess") {
        
        string msg = val["msg"].asString();
        MessageUI::show(MSG_NORMAL, NULL, msg.c_str());
        
        int cityid = getAsInt(val["cityid"]);
        m_warSignupCityId = cityid;
    }
    else if (cid == "clanwar_signerror") {
        
        string msg = val["msg"].asString();
        MessageUI::show(MSG_NORMAL, NULL, msg.c_str());
    }
    else if (cid == "clanwar_attackclans")
    {
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
     
        LogicInfo::getSingleton().onNotify(NOTIFY_CLANWAR_ATTACKCLANS);
    }
    else if (cid == "clanwar_defensemembers") {
        
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
            
            m_warDefenseUserList.push_back(info);
        }
        
        m_warCityMinWinStar = ClanWarMgr::getSingleton().m_warMinWinStar;
        int baseStar = ClanWarMgr::getSingleton().m_warClanMinMember * 3;
        int totalStar = ClanWarMgr::getSingleton().m_warDefenseUserList.size() * 3;
        
        if (totalStar < baseStar) {
            m_warCityMinWinStar = ClanWarMgr::getSingleton().m_warMinWinStar - (baseStar - totalStar);
        }
        if (m_warCityMinWinStar < 0) {
            m_warCityMinWinStar = 1;
        }
        
        LogicInfo::getSingleton().onNotify(NOTIFY_CLANWAR_DEFENSEINFO);
    }
    else if (cid == "clanwar_attackinfo") {
        
        m_warAttackInfo.canAttackNums = getAsInt(val["attacknum_left"]);
        m_warAttackInfo.maxAttackNums = getAsInt(val["maxattacknums"]);
        m_warAttackInfo.curAttackNums = getAsInt(val["curattacknums"]);
        m_warAttackInfo.winStar = getAsInt(val["winstar"]);
        
        LogicInfo::getSingleton().onNotify(NOTIFY_CLANWAR_BATTLEINFO);
    }
    else if (cid == "clanwar_battlelog") {
        
        m_warBattleLog.clear();
        
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
                
                m_warBattleLog.push_back(msg);
            }
        }
        LogicInfo::getSingleton().onNotify(NOTIFY_CLANWAR_BATTLELOG);
    }
    else if (cid == "clanwar_score") {
        
        m_warWin = getAsInt(val["iswin"]);
        
        m_warCityId = getAsInt(val["cityid"]);
        m_warCityName = val["cityname"].asString();
        m_warRanking = getAsInt(val["rank"]);
        m_warRewardGems = getAsInt(val["gems"]);
        m_warAttacker = getAsInt(val["attacker"]);
        m_warRewardId = getAsInt(val["rewardid"]);
        
        PlayerScene::getSingleton().m_pGameWorld->showClanWarScoreUI();
    }
    else if (cid == "clanwar_isattacking")
    {
        bool isAttacking = getAsInt(val["isattacking"]);
        int uid = getAsInt(val["targetuid"]);
        
        string msg = val["msg"].asString();
        if (isAttacking) {
            
            PlayerScene::getSingleton().m_pGameWorld->hideChgScene();
            MessageUI::show(MSG_NORMAL, NULL, msg.c_str());
        }
        else {
            LogicInfo::getSingleton().onNotify(NOTIFY_CLANWAR_ATTACKUSER, &uid);
        }
    }

    return false;
}

ClanWarAttackClanInfo* ClanWarMgr::getAttackClanInfo(int clanid)
{
    for (int i = 0; i < m_warAttackClanList.size(); i ++) {
        if (m_warAttackClanList[i].id == clanid) {
            return &m_warAttackClanList[i];
        }
    }
    return NULL;
}

NS_COC_END
