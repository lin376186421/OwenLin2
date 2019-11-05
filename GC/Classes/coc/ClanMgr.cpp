//
//  ClanMgr.cpp
//  GameClient
//
//  Created by chowray on 13-11-30.
//
//

#include "ClanMgr.h"
#include "PlayerScene.h"
#include "LogicInfo.h"
#include "HttpClient.h"
#include "GameWorldScene.h"
#include "Building.h"
#include "Building_Clan.h"
#include "PersonMgr.h"
#include "SocketClient.h"
#include "ChatMgr.h"
#include "ViewString.h"

NS_COC_BEGIN

int MEMBER_STATE_JOIN = 1;
int MEMBER_STATE_LEFT = 2;
int MEMBER_STATE_TO_NORMAL = 3;
int MEMBER_STATE_TO_ELDER = 4;
int MEMBER_STATE_TO_COLEADER = 5;
int MEMBER_STATE_TO_LEADER = 6;

ClanMgr::ClanMgr()
: m_myMaxSpace(0)
, m_myCurSpace(0)
, m_otherMaxSpace(0)
, m_otherCurSpace(0)
, m_clanjob(-1)
{

}

ClanMgr::~ClanMgr()
{
}

ClanMgr& ClanMgr::getSingleton()
{
	static ClanMgr s_mgr;
    
	return s_mgr;
}

//新建联盟
int ClanMgr::createClan(int& need, const char* clanname, const char* symbol, const char* description, int clantype, int mintrophies, int flag, int national)
{
	need = 40000;
    
	if(LogicInfo::getSingleton().getMoney(MONEY_GOLD) < need)
    {
		need -= LogicInfo::getSingleton().getMoney(MONEY_GOLD);
        
		return MSG_NOGOLD;
    }
    
	LogicInfo::getSingleton().chgMoney(MONEY_GOLD, -need);
    
    HttpClient::getSingleton().createClan(clanname, symbol, description, clantype, mintrophies, flag, national);
    
	return 1;
}

//! 更新帮派信息
void ClanMgr::updateClan(int cid, int flag, const char* name)
{
	if(!LogicInfo::getSingleton().isNormalMode())
		return;
    
    m_myclan.id = cid;
    m_myclan.flag = flag;
    m_myclan.name = name;
    
    if (PlayerScene::getSingleton().m_pBuildingClan) {
        ((Building_Clan*)PlayerScene::getSingleton().m_pBuildingClan)->updateClan(cid, flag, name);
    }
}

void ClanMgr::leftClan()
{
    m_clanjob = -1;
    updateClan(0);
    
    ChatMgr::getSingleton().onClearClanChat();
    
    if (PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pChatUI) {
        
        PlayerScene::getSingleton().m_pGameWorld->m_pChatUI->refreshPage();
    }
    
    m_myListSoldier.clear();
}

bool ClanMgr::chgMemberJob(int uid, int promote, int clanjob)//修改成员等级
{
    ClanMember* pMember = getClanMember(uid);
    if (promote > 0) {
        clanjob ++;
    }
    else if (promote < 0) {
        clanjob --;
    }
    
    SocketClient::getSingleton().m_modClan.chgUserRight(uid, pMember->name, clanjob);
    
    LogicInfo::getSingleton().onNotify(NOTIFY_CLAN_REFRESH, &m_myclan);
    
    return true;
}

void ClanMgr::askJoin(int cid)//请求加入联盟
{
    SocketClient::getSingleton().m_modClan.joinClan(cid);
}

void ClanMgr::acceptJoin(int chatid, int destuid)//接受请求
{
	SocketClient::getSingleton().m_modClan.acceptJoin(chatid, destuid);
}

void ClanMgr::rejectJoin(int chatid, int destuid)//拒绝请求
{
    SocketClient::getSingleton().m_modClan.rejectJoin(chatid, destuid);
}

void ClanMgr::leaveClan() //离开联盟
{
    SocketClient::getSingleton().m_modClan.leftClan(m_myclan.id);
}

ClanMember* ClanMgr::getClanMember(int uid)
{
    ClanMember* pMember = NULL;
    for(std::vector<ClanMember>::iterator it = m_myclan.memberList.begin(); it != m_myclan.memberList.end(); ++it)
    {
        if(it->uid == uid)
        {
            pMember = &(*it);
            break;
        }
    }
    return pMember;
}

void ClanMgr::kickOut(int uid, string uname)
{
    SocketClient::getSingleton().m_modClan.kickUser(uid, uname);    
}

//! 时间
void ClanMgr::onIdle(int ot)
{
	if(!PlayerScene::getSingleton().m_isAlreadyInit)
		return;

    if(m_askCD > 0)
    {
        m_ticker += ot;
        
        if (m_ticker >= 1000) {
         
            m_ticker -= 1000;
            
            --m_askCD;
            refreshClanBuildingCD();
        }
    }
}

//! 获取消息
bool ClanMgr::onProc(const Json::Value& val)
{
    if(val["cid"].asString() == "myclaninfo")
	{
        m_myclan.id = getAsInt(val["clanid"]);
		m_myclan.name = val["clanname"].asString();
        m_myclan.flag = getAsInt(val["clanflag"]);
        m_myclan.national = getAsInt(val["national"]);
        m_myclan.jointype = getAsInt(val["clantype"]);
        m_myclan.description = val["description"].asString();
        m_myclan.needcupnums = getAsInt(val["mintrophies"]);
        m_myclan.leaderid = getAsInt(val["clanleader"]);
        m_myclan.cupnums = getAsInt(val["cup"]);
        m_myclan.membercurnums = getAsInt(val["curnums"]);
        m_myclan.membermaxnums = getAsInt(val["maxnums"]);
        
        int status = getAsInt(val["status"]);
        
        if (status == 1) {  //create clan success
            LogicInfo::getSingleton().onNotify(NOTIFY_CLAN_CREATE);
			SocketClient::getSingleton().m_modClan.createClan(m_myclan.id, m_myclan.name.c_str());
        }
        else if (status == 2)  //join clan success
        {
            if (PlayerScene::getSingleton().m_pGameWorld) {
                PlayerScene::getSingleton().m_pGameWorld->hideClanUI();
                
                if (PlayerScene::getSingleton().m_pGameWorld->m_pChatUI) {
                    PlayerScene::getSingleton().m_pGameWorld->m_pChatUI->scrollToTop();
                }
            }
        }
        else if (status == 3) //clan edit success
        {
            LogicInfo::getSingleton().onNotify(NOTIFY_CLAN_CHANGE);
        }
        
        m_clanjob = getAsInt(val["rightcode"]);
        
        m_askCD = getAsInt(val["rscd"]);
		refreshClanBuildingCD();
        
        updateClan(m_myclan.id, m_myclan.flag, m_myclan.name.c_str());
        
        LogicInfo::getSingleton().onNotify(NOTIFY_CLAN_REFRESH, &m_myclan);
	}
    else if(val["cid"].asString() == "otherclaninfo")
    {
        int clanid = getAsInt(val["clanid"]);
        
        ClanInfo* clanInfo = NULL;
        if (clanid == m_myclan.id) {
            clanInfo = &m_myclan;
        }
        else {
            clanInfo = &m_otherclan;
        }
        
        clanInfo->id = getAsInt(val["clanid"]);
        clanInfo->name = val["clanname"].asString();
        clanInfo->flag = getAsInt(val["clanflag"]);
        clanInfo->national = getAsInt(val["national"]);
        clanInfo->leaderid = getAsInt(val["clanleader"]);
        clanInfo->jointype = getAsInt(val["clantype"]);
        clanInfo->cupnums = getAsInt(val["cup"]);
        clanInfo->membercurnums = getAsInt(val["curnums"]);
        clanInfo->membermaxnums = getAsInt(val["maxnums"]);
        clanInfo->needcupnums = getAsInt(val["mintrophies"]);
        clanInfo->description = val["description"].asString();
        
        LogicInfo::getSingleton().onNotify(NOTIFY_CLAN_REFRESH, clanInfo);
    }
    else if (val["cid"].asString() == "otherbaseclaninfo")
    {
        m_otherclan.id = getAsInt(val["clanid"]);
        m_otherclan.flag = getAsInt(val["clanflag"]);
        m_otherclan.national = getAsInt(val["national"]);
        m_otherclan.name = val["clanname"].asString();
        m_otherclan.jointype = getAsInt(val["clantype"]);
        m_otherclan.leaderid = getAsInt(val["clanleader"]);
        m_otherclan.cupnums = getAsInt(val["cup"]);
        m_otherclan.membercurnums = getAsInt(val["curnums"]);
        m_otherclan.membermaxnums = getAsInt(val["maxnums"]);
        m_otherclan.needcupnums = getAsInt(val["mintrophies"]);
        m_otherclan.description = val["description"].asString();
    }
	else if(val["cid"].asString() == "mycastleinfo")
	{
        onRefreshCastleSoldier(val);
        
		return true;
	}
	else if(val["cid"].asString() == "othercastleinfo")
	{
        m_otherListSoldier.clear();
        
        m_otherMaxSpace = getAsInt(val["maxspace"]);
        int curspace = 0;
        
		Json::Value troops = val["info"];
        
        int nums = troops.size();
        for (int i = 0; i < nums; i ++) {
            
            int sid = getAsInt(troops[i]["soldier"]);
            
            if (sid > 0) {
                m_otherListSoldier.push_back(sid);
                
                SoldierInfo soldierInfo = *PersonMgr::getSingleton().getSoldierInfo(sid);
                curspace += soldierInfo.housingSpaceex;
            }
        }
        
        m_otherCurSpace = curspace;

		return true;
	}
	else if(val["cid"].asString() == "clanuserlist")
    {
        int clanid = getAsInt(val["clanid"]);
        
        ClanInfo* clanInfo = NULL;
        if (clanid == m_myclan.id) {
            clanInfo = &m_myclan;
        }
        else {
            clanInfo = &m_otherclan;
        }
        
        clanInfo->memberList.clear();
        
        Json::Value memberList = val["lst"];
        
        int size = memberList.size();
        for (int i = 0; i < size; i ++) {
            
            Json::Value memberInfo = memberList[i];
            
            ClanMember member;
            
            member.uid = getAsInt(memberInfo["uid"]);
            member.clanid = getAsInt(memberInfo["clanid"]);
            member.clanjob = getAsInt(memberInfo["rightcode"]);
            member.name = memberInfo["uname"].asString();
            member.curcup = getAsInt(memberInfo["cup"]);
            member.level = getAsInt(memberInfo["curlevel"]);
            member.sendsoldiers = getAsInt(memberInfo["sendsoldiers"]);
            member.getsoldiers = getAsInt(memberInfo["getsoldiers"]);
            member.jointime = getAsInt(memberInfo["jointime"]);
            member.currank = getAsInt(memberInfo["rank"]);
            member.lastrank = getAsInt(memberInfo["lastrank"]);
            member.castlelevel = getAsInt(memberInfo["castlelevel"]);
            member.military = getAsInt(memberInfo["mrank"]);
            member.troopsdonated = getAsInt("sendsoldiers");
            member.troopsreceived = getAsInt("getsoldiers");
            member.isonline = getAsInt(memberInfo["isonline"]);
            member.warinvalid = getAsInt(memberInfo["newfromWinclan"]);
            
            if (member.uid == LogicInfo::getSingleton().m_myInfo.uid) {
                m_clanjob = member.clanjob;
            }
            
            clanInfo->memberList.push_back(member);
        }
        
        LogicInfo::getSingleton().onNotify(NOTIFY_CLAN_REFRESH, clanInfo);
    }
    else if (val["cid"].asString() == "clansearchlist")
    {
        m_pClanSearchLst.clear();
        
        Json::Value clanlist = val["lst"];
        
        int size = clanlist.size();
        for (int i = 0; i < size; i ++) {
            
            Json::Value clanInfo = clanlist[i];
            
            ClanInfo clan;
            
            clan.id = getAsInt(clanInfo["clanid"]);
            clan.name = clanInfo["clanname"].asString();
            clan.flag = getAsInt(clanInfo["clanflag"]);
            clan.national = getAsInt(clanInfo["national"]);
            clan.leaderid = getAsInt(clanInfo["clanleader"]);
            clan.jointype = getAsInt(clanInfo["clantype"]);
            clan.cupnums = getAsInt(clanInfo["cup"]);
            clan.membercurnums = getAsInt(clanInfo["curnums"]);
            clan.membermaxnums = getAsInt(clanInfo["maxnums"]);
            clan.needcupnums = getAsInt(clanInfo["mintrophies"]);
            clan.description = clanInfo["description"].asString();
            
            m_pClanSearchLst.push_back(clan);
        }
        
        LogicInfo::getSingleton().onNotify(NOTIFY_CLAN_SEARCH);
    }
    else if(val["cid"].asString() == "rankclan")
    {
        m_pClanRankingLst.clear();
        
        Json::Value ranklist = val["lst"];
        
        int size = ranklist.size();
        for (int i = 0; i < size; i ++) {
            
            Json::Value clanInfo = ranklist[i];
            
            ClanInfo clan;
            
            clan.id = getAsInt(clanInfo["clanid"]);
            clan.leaderid = getAsInt(clanInfo["clanleader"]);
            clan.name = clanInfo["clanname"].asString();
            clan.flag = getAsInt(clanInfo["clanflag"]);
            clan.national = getAsInt(clanInfo["national"]);
            clan.cupnums = getAsInt(clanInfo["cup"]);
            clan.membercurnums = getAsInt(clanInfo["curnums"]);
            clan.membermaxnums = getAsInt(clanInfo["maxnums"]);
            clan.jointype = getAsInt(clanInfo["clantype"]);
            clan.needcupnums = getAsInt(clanInfo["mintrophies"]);
            clan.description = clanInfo["description"].asString();
            
            m_pClanRankingLst.push_back(clan);
        }
        
        LogicInfo::getSingleton().onNotify(NOTIFY_CLAN_RANKING);
    }

	return false;
}

void ClanMgr::levelupClanCastle()
{    
    SocketClient::getSingleton().m_modClan.levelupCastle(m_castleLevel);
}

//! 请求送兵
bool ClanMgr::sendHelp(const char* chat, int gem)
{
	refreshClanBuildingCD();
    
    ChatData chatData;
    chatData.type = CHAT_TYPE_HELP;
    chatData.uid = LogicInfo::getSingleton().m_myInfo.uid;
    chatData.uname = LogicInfo::getSingleton().m_myInfo.uname;
    chatData.message = chat;
        
    ChatMgr::getSingleton().requestSoldier(chatData, gem);
    
    return true;
}

void ClanMgr::giveSoldier(int uid, string uname, int soldierId)
{
    Person* pPerson = NULL;
    for(PlayerScene::_PersonList::iterator it = PlayerScene::getSingleton().m_lstPerson[CAMP_DEFENCE].begin(); it != PlayerScene::getSingleton().m_lstPerson[CAMP_DEFENCE].end(); ++it)
    {
        if ((*it)->m_personid == soldierId) {
            
            if (!(*it)->isStatus(PERSON_STATUS_MOVE_OUT)) {
                
                pPerson = (*it);
                pPerson->setPersonPath(LOGIC_W-2, LOGIC_W-2);
                pPerson->addStatus(PERSON_STATUS_MOVE_OUT);
                
                break;
            }
        }
    }
    
    if (pPerson) {
        
        MapTroophousingInfo::iterator it = LogicInfo::getSingleton().m_mapTroophousing.find(pPerson->m_pTroophousing->troophousingid);
        
        for(std::list<TroophousingSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1)
        {
            if (it1->pid == soldierId) {
                
                if (it1->numsex > 1) {
                    it1->numsex -= 1;
                }
                else{
                    it->second.lstSoldier.erase(it1);
                }
                
                it->second.ischg = true;
                break;
            }
        }
    }

    LogicInfo::getSingleton().onRefurbishAllTroophousing();
    
    map<int, ClanTroopsData>::iterator it = ChatMgr::getSingleton().m_mapClanTroopsData.find(uid);
    if (it != ChatMgr::getSingleton().m_mapClanTroopsData.end()) {
        
        if (PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pChatUI) {
            PlayerScene::getSingleton().m_pGameWorld->m_pChatUI->m_pDonateUI->refresh(it->second);
        }
    }
}

void ClanMgr::getSoldier(int uid, string uname, int sid)
{
//    m_myListSoldier.push_back(sid);
    
    SoldierInfo soldierInfo = *PersonMgr::getSingleton().getSoldierInfo(sid);
//    m_myCurSpace += soldierInfo.housingSpaceex;
    
    int lv = sid % 100;
    
    char tip[128];
    sprintf(tip, ViewString::getValue("get_soldier_tip").c_str(), lv, soldierInfo.name.c_str(), uname.c_str());
    
    PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI->addNote(tip);
}

//! 刷新送兵CD时间
void ClanMgr::refreshClanBuildingCD()
{
	Building_Clan* pBuildingClan = (Building_Clan*)PlayerScene::getSingleton().m_pBuildingClan;
    
	if(pBuildingClan == NULL)
		return;
    
    if (m_myclan.id == 0) {

        pBuildingClan->setClanCD(0, 0);
        return;
    }
    
	if(m_askCD == 0 || !LogicInfo::getSingleton().isNormalMode())
	{
		pBuildingClan->setClanCD(0, 0);
	}
	else
	{
		pBuildingClan->setClanCD(20 * 60, m_askCD);
	}
}

void ClanMgr::onRefreshCastleSoldier(Json::Value val)
{
    m_myListSoldier.clear();
    
    m_myMaxSpace = getAsInt(val["maxspace"]);
    int curspace = 0;
    
    Json::Value troops = val["info"];
    
    int nums = troops.size();
    for (int i = 0; i < nums; i ++) {
        
        int sid = getAsInt(troops[i]["soldier"]);
        if (sid > 0) {
            m_myListSoldier.push_back(sid);
            
            SoldierInfo soldierInfo = *PersonMgr::getSingleton().getSoldierInfo(sid);
            curspace += soldierInfo.housingSpaceex;
        }
    }
    
    m_myCurSpace = curspace;
}

void ClanMgr::onMemberStage(int uid, int state)
{
    ClanMember* pMember = getClanMember(uid);
    
    if (pMember == NULL) {
        return;
    }
    
    if (state == MEMBER_STATE_JOIN) {
        // member join
        
    }
    else if (state == MEMBER_STATE_LEFT) {
        // member left
        for(std::vector<ClanMember>::iterator it = m_myclan.memberList.begin(); it != m_myclan.memberList.end(); ++it)
        {
            if(it->uid == uid)
            {
                m_myclan.memberList.erase(it);
                break;
            }
        }
        
        m_myclan.membercurnums = m_myclan.memberList.size();
    }
    else if (state == MEMBER_STATE_TO_NORMAL) {
        pMember->clanjob = CLAN_JOB_MEMBER;
    }
    else if (state == MEMBER_STATE_TO_ELDER) {
        pMember->clanjob = CLAN_JOB_ELDER;
    }
    else if (state == MEMBER_STATE_TO_COLEADER) {
        pMember->clanjob = CLAN_JOB_COLEADER;
    }
    else if (state == MEMBER_STATE_TO_LEADER) {
        pMember->clanjob = CLAN_JOB_LEADER;
    }
    
    if (uid == LogicInfo::getSingleton().m_myInfo.uid) {
        m_clanjob = pMember->clanjob;
    }
    
    LogicInfo::getSingleton().onNotify(NOTIFY_CLAN_REFRESH, &m_myclan);
}

void ClanMgr::onRequestHelp()
{
    m_askCD = 20 * 60;
    m_ticker = 0;
    
    refreshClanBuildingCD();
    
    if (PlayerScene::getSingleton().m_pCurBuilding == PlayerScene::getSingleton().m_pBuildingClan) {
        PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->onSelect(true, PlayerScene::getSingleton().m_pCurBuilding);
    }
}

void ClanMgr::resetClanFightSoldier()
{
    m_fightAttackSoldier.clear();
    m_fightDefendSoldier.clear();
    
    m_fightAttackSoldier.clear();
    for (vector<int>::iterator it = m_myListSoldier.begin(); it != m_myListSoldier.end(); it ++) {
        m_fightAttackSoldier.push_back(*it);
    }
}

NS_COC_END