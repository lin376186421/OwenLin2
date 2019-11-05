//
//  ClanMgr.h
//  GameClient
//
//  Created by chowray on 13-11-30.
//
//

#ifndef __GameClient__ClanMgr__
#define __GameClient__ClanMgr__

#include <algorithm>
#include "BaseDef.h"
#include "Mod_Clan.h"
#include "Building_Clan.h"

NS_COC_BEGIN

static string national[] = {"", "Saudi Arabia", "UAE", "Ukraine", "USA", "United Kingdom", "Indonesia", "Thailand", "France", "Malaysia", "Spain", "Canada", "Italy", "Greece", "Netherlands", "Turkey", "Mexico",
    "Vietnam", "Argentina", "Belgium", "Australia", "Sweden", "Korea", "Japan", "India", "China", "Russia",
    "Germany", "Poland", "Brazil", "Philippines" };

class ClanMgr
{
public:
	
    ClanMgr();
    ~ClanMgr();
public:
	static ClanMgr& getSingleton();
    
    int createClan(int& need, const char* clanname, const char* symbol, const char* description, int clantype, int mintrophies, int flag, int national);
    
    void updateClan(int cid, int flag = 0, const char* name = "");
    
    void leftClan();
    
    ClanMember* getClanMember(int uid);
    
	void onIdle(int ot);
	
	bool onProc(const Json::Value& val);
    
	void askJoin(int cid); //请求加入联盟

    void acceptJoin(int chatid, int destuid);  //接受请求

    void rejectJoin(int chatid, int destuid);  //拒绝请求
    
    void kickOut(int uid, string uname);  //踢人
    
    void leaveClan();   //离开联盟
    
    bool chgMemberJob(int uid, int promote, int clanjob);    //修改成员等级
    
    void levelupClanCastle();
    
    bool sendHelp(const char* chat, int gem);
    
    void giveSoldier(int uid, string uname, int sid);
    
    void getSoldier(int uid, string uname, int sid);
    
    void refreshClanBuildingCD();
    
    void onRefreshCastleSoldier(Json::Value val);
    
    void onMemberStage(int uid, int state);
    
    void onRequestHelp();
    
    void resetClanFightSoldier();
        
public:
    
    int m_askCD;//请求送兵CD
    int m_ticker;
        
    ClanInfo m_myclan;
    ClanInfo m_otherclan;
    
    int m_clanjob;
    
    int m_castleLevel;
    
    std::vector<ClanInfo> m_pClanRankingLst;
    std::vector<ClanInfo> m_pClanSearchLst;
        
    int m_myMaxSpace;
    int m_myCurSpace;
    vector<int> m_myListSoldier;
    
    int m_otherMaxSpace;
    int m_otherCurSpace;
    vector<int> m_otherListSoldier;
    
    vector<int> m_fightAttackSoldier;
    vector<int> m_fightDefendSoldier;
    
    //Mod_Clan modClan;
};

NS_COC_END

#endif /* defined(__GameClient__ClanMgr__) */
