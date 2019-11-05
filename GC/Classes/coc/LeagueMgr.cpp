//
//  LeagueMgr.cpp
//  GameClient
//
//  Created by Lee  on 14-4-2.
//
//

#include "LeagueMgr.h"
#include "BaseDef.h"
#include "LogicInfo.h"

USING_NS_COC;

LeagueMgr::~LeagueMgr()
{
}

LeagueMgr::LeagueMgr():m_endTime(0),
m_startTime(0),
m_state(0)
{
    m_myGroup.myAward.hasRank = false;
    m_myGroup.myAward.hasAward = false;
}

LeagueMgr& LeagueMgr::getSingleton()
{
	static LeagueMgr s_mgr;
	return s_mgr;
}

LeagueMgr::ResInfo LeagueMgr::getResInfo(Json::Value val)
{
        ResInfo info;
        info.types = 0;
        for (int j = 0; j<val.size(); j++)
        {
            Json::Value value = val[j];
            info.resType[j] = getAsInt(value["type"]);
            info.num[j] = getAsInt(value["num"]);
            info.types++;
        }

    return info;
}


void LeagueMgr::setLevelCupInfo(const Json::Value val)
{
    m_levelInfoLst.clear();
    char str[64];
    for (int i = 0; i<val.size(); i++)
    {
        sprintf(str, "%d",i+1);
        LeagueMgr::LevelInfo levelInfo;
        Json::Value levelValue = val[str];
        levelInfo.maxcup     = getAsInt(levelValue["cup_high"]);
        levelInfo.mincup     = getAsInt(levelValue["cup_low"]);
        levelInfo.levelid    = getAsInt(levelValue["level"]);
        levelInfo.name       = levelValue["level_name"].asString();
        levelInfo.searchCost = getAsInt(levelValue["search_cost"]);

        if (levelValue.isMember("battle_award"))
        {
            Json::Value basValue = levelValue["battle_award"];
            for (int j = 0; j<basValue.size(); j++)
            {
                sprintf(str, "%d",j+1);
                Json::Value resValue = basValue[str];
                ResInfo resInfo = getResInfo(resValue);
                levelInfo.basLst.push_back(resInfo);
            }
        }
         m_levelInfoLst.push_back(levelInfo);
    }
}

void LeagueMgr::setCurBattleAwardInfo(const Json::Value val)
{
    m_myGroup.btAwardLst.clear();
    char str[64];
    for (int i = 0; i<val.size(); i++)
    {
        sprintf(str, "%d",i+1);
        Json::Value resValue = val[str];
        ResInfo info = getResInfo(resValue);;
        m_myGroup.btAwardLst.push_back(info);
    }
}

LeagueMgr::ResInfo LeagueMgr::getCurBattleAwardInfo(int star)
{
    return  m_myGroup.btAwardLst[star-1];
}

void LeagueMgr::setRankAwardInfo(const Json::Value val)
{
    m_myGroup.rankAwardLst.clear();
    char str[64];
    for (int i = 0; i<val.size(); i++)
    {
        sprintf(str, "%d",i+1);
        Json::Value resValue = val[str];
        ResInfo resInfo = getResInfo(resValue);
        m_myGroup.rankAwardLst.push_back(resInfo);
    }
}

LeagueMgr::ResInfo LeagueMgr::getRankAwardInfo(int rank)
{
    return  m_myGroup.rankAwardLst[rank-1];
}

void LeagueMgr::setPlayerInfo(const Json::Value val)
{
    m_myGroup.playerLst.clear();
    char str[64];
    for (int i = 0; i<val.size(); i++)
    {
        sprintf(str, "%d",i+1);
        Json::Value value = val[str];
        
        PlayerInfo info;
        info.clanid     = getAsInt(value["clanid"]);
        info.clanFlag   = getAsInt(value["clanflag"]);
        info.clanName   = value["clanname"].asString();
        info.uname      = value["uname"].asString();
        info.star       = getAsInt(value["star"]);
        info.lv         = getAsInt(value["lv"]);
        info.cup         = getAsInt(value["cup"]);
        info.winNums    = getAsInt(value["win"]);
        info.loseNums   = getAsInt(value["lose"]);
        info.uid        = getAsInt(value["uid"]);
        info.mrank      = getAsInt(value["mrank"]);
        info.rank       = getAsInt(value["rank"]);
        info.medal      = getAsInt(value["medal"]);
        m_myGroup.playerLst.push_back(info);
        
        if (info.uid == LogicInfo::getSingleton().m_myInfo.uid)
        {
            m_myGroup.myInfo = info;
        }
    }
}

void LeagueMgr::setMyAward(const Json::Value val)
{
    m_myGroup.myAward.hasRank = false;
    m_myGroup.myAward.hasAward = false;

    if (val.isMember("rank"))
    {
        m_myGroup.myAward.hasRank = true;
        m_myGroup.myAward.rank = getAsInt(val["rank"]);
        m_myGroup.myAward.levelid = getAsInt(val["league_level"]);
        m_myGroup.myAward.awardid = getAsInt(val["award_id"]);
        m_myGroup.myAward.leagueid = getAsInt(val["league_id"]);
        if (val["accessory"] != Json::nullValue)
        {
            m_myGroup.myAward.hasAward = true;
            Json::Value resValue = val["accessory"];
            ResInfo resInfo = getResInfo(resValue);
            m_myGroup.myAward.resInfo = resInfo;
        }
        else
        {
            m_myGroup.myAward.hasAward = false;
        }
    }
    else
    {
        m_myGroup.myAward.hasRank = false;
    }
}

int LeagueMgr::getSearchCost()
{
    if (m_myGroup.levelid > m_levelInfoLst.size())
    {
        return 100;
    }
    return m_levelInfoLst[m_myGroup.levelid-1].searchCost;
}

int LeagueMgr::getLevel(int cup)
{
    for (int i = m_levelInfoLst.size()-1; i>=0; i--)
    {
        LevelInfo info = m_levelInfoLst.at(i);
        if (cup>=info.mincup)
        {
            return (i+1);
        }
    }
    return 1;
}


bool LeagueMgr::canAttack()
{
    return (m_myGroup.myInfo.winNums + m_myGroup.myInfo.loseNums < m_myGroup.playerLst.size()-1 && isBattleState());
}


bool LeagueMgr::isInLeague()
{
    return (m_myGroup.state == 1 && m_state > 0);
}

bool LeagueMgr::isReadyState()
{
    return ( getCurrentTime() < m_startTime );
}

bool LeagueMgr::isBattleState()
{
    return ( getCurrentTime() >= m_startTime && getCurrentTime() <= m_endTime);
}

bool LeagueMgr::isDoneGroupState()
{
    return ( getCurrentTime() < m_startTime && m_state == LB_STATUS_ON_GROUPED);
}

bool LeagueMgr::isClearingState()
{
    return (getCurrentTime() > m_endTime);
}