//
//  MilitaryRankMgr.cpp
//  GameClient
//
//  Created by owenlin on 14-5-19.
//
//

#include "MilitaryRankMgr.h"
#include <algorithm>
#include "MilitaryRankMgr.h"
#include "PlayerScene.h"
#include "LogicInfo.h"
#include "BaseCSV.h"
#include "../Utils/DataDecrypt.h"
#include "../coc/HttpClient.h"
#include "../GameWorldScene.h"
#include "md5.h"
#include "AppDebug.h"

NS_COC_BEGIN

MilitaryRankMgr::MilitaryRankMgr()
{
	m_isRemote = false;
    if(LEAGUE_ON)
        load("military.cfg");
    
}

MilitaryRankMgr::~MilitaryRankMgr()
{
    
}

MilitaryRankMgr& MilitaryRankMgr::getSingleton()
{
	static MilitaryRankMgr s_mgr;
    
	return s_mgr;
}

void MilitaryRankMgr::onRelease()
{
    m_militaryInfoList.clear();
}

void MilitaryRankMgr::onLoadEnd()
{
    m_militaryInfoList.clear();
    for(int i = 1; i < getHeight(); i++)
    {
        MilitaryInfo mInfo;
        mInfo.rank = atoi(get("rank", i));
        mInfo.name = get("name", i);
        mInfo.troops = atoi(get("troops", i));
        
        mInfo.oId = split(get("sid", i));
        mInfo.oTarget = split(get("slevel", i));
        
        mInfo.cup = atoi(get("cup", i));
        mInfo.medal = atoi(get("medal", i));
        
        m_militaryInfoList.push_back(mInfo);
    }
    
}

vector<int> MilitaryRankMgr::split(const char *str)
{
    std::string::size_type pos;
    std::vector<int> result;
    std::string mStr(str);
    std::string pattern = "/";
    mStr += pattern;
    int size = mStr.size();
    for(int i = 0; i<size; i++)
    {
        pos = mStr.find(pattern,i);
        if(pos<size)
        {
            std::string s = mStr.substr(i,pos-i);
            if(s.length() > 0)
                result.push_back(atoi(s.c_str()));
            i = pos+pattern.size()-1;
        }
    }
    return result;
}

MilitaryInfo MilitaryRankMgr::getMilitaryInfo(int rank)
{
    MilitaryInfo info = m_militaryInfoList[rank-1];
    return info;
}

bool MilitaryRankMgr::checkRank(int rank)
{
    MilitaryInfo info = getMilitaryInfo(rank);
    
    if(LogicInfo::getSingleton().m_myInfo.cupex < info.cup)
        return false;
    
    if(LogicInfo::getSingleton().m_myInfo.medal < info.medal)
        return false;
    
    for(int i = 0; i < info.oId.size(); i++)
    {
        int oid = info.oId[i];
        int cur;
        if(oid > 10000)//配置表中 兵种id只取前3位 建筑id五位
        {
            if(oid/100 == BUILDING_WALL)
            {
                cur = LogicInfo::getSingleton().getBuildNumsByBid(oid);
            }
            else
            {
                cur = LogicInfo::getSingleton().getBuildMaxLevel(oid/100);
            }
        }
        else
        {
            cur = LogicInfo::getSingleton().getSoldierlevel(oid%100-1);
        }
        
        if(cur < info.oTarget[i])//兵种没达到要求的条件
            return false;
    }
    
    int curspace, maxspace;
    LogicInfo::getSingleton().countTroopSpace(LogicInfo::getSingleton().m_mapTroophousing, curspace, maxspace);
    if(maxspace < info.troops)
        return false;
    
    return true;
}

string MilitaryRankMgr::getMilitaryName(int rank)
{
    if(!LEAGUE_ON)
        return "";
    if(rank == 0)
        return "Private";
    MilitaryInfo info = getMilitaryInfo(rank);
    return info.name;
}

void MilitaryRankMgr::countMilitaryRank()
{
    if(!LEAGUE_ON)
        return;

    if(PlayerScene::getSingleton().m_pGameWorld == false)
        return;
    
    if(GuideMgr::getSingleton().isGuideEnd() == false && GuideMgr::getSingleton().m_isInitOk == true && LogicInfo::getSingleton().m_myInfo.military == 0)
    {
        LogicInfo::getSingleton().m_myInfo.military = 0;
        LogicInfo::getSingleton().onNotify(NOTIFY_BASEINFO);
        return;
    }
    
    //第一次游戏 过完新手后  默认军衔等级为1
//    if(LogicInfo::getSingleton().m_myInfo.military == 0 && GuideMgr::getSingleton().isGuideEnd() == true)
//    {
//        LogicInfo::getSingleton().m_myInfo.military = 1;
//        HttpClient::getSingleton().updateMilitaryRank();
//        LogicInfo::getSingleton().onNotify(NOTIFY_BASEINFO);
//        PlayerScene::getSingleton().m_pGameWorld->showMilitaryUpgradeUI();
//        return;
//    }
    
    for(int level = m_militaryInfoList.size(); level > 1; level--)
    {
        if(checkRank(level))
        {
            //等级上升 不能下降
            if(LogicInfo::getSingleton().m_myInfo.military != level && LogicInfo::getSingleton().m_myInfo.military < level)
            {
                LogicInfo::getSingleton().m_myInfo.military = level;
                HttpClient::getSingleton().updateMilitaryRank();
                LogicInfo::getSingleton().onNotify(NOTIFY_BASEINFO);
                if(PlayerScene::getSingleton().m_pGameWorld)
                    PlayerScene::getSingleton().m_pGameWorld->showMilitaryUpgradeUI();
            }
            
            return;
        }
    }
    
    int rank = 1;
    if(LogicInfo::getSingleton().m_myInfo.military < rank && GuideMgr::getSingleton().isGuideEnd())
    {
        LogicInfo::getSingleton().m_myInfo.military = rank;
        HttpClient::getSingleton().updateMilitaryRank();
        LogicInfo::getSingleton().onNotify(NOTIFY_BASEINFO);
        if(PlayerScene::getSingleton().m_pGameWorld)
            PlayerScene::getSingleton().m_pGameWorld->showMilitaryUpgradeUI();
    }
    
}


NS_COC_END