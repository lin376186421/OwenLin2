//
//  ClanWarMgr.h
//  GameClient
//
//  Created by qsun on 14-9-4.
//
//

#ifndef __GameClient__ClanWarMgr__
#define __GameClient__ClanWarMgr__

#include "BaseDef.h"

NS_COC_BEGIN

enum ClanWarState{
    CLANWAR_STATE_CLOSE,
    CLANWAR_STATE_SIGNUP,
    CLANWAR_STATE_BATTLE_PREPARE,
    CLANWAR_STATE_BATTLE_START,
    CLANWAR_STATE_BATTLE_SCORE,
    CLANWAR_STATE_END
};

class ClanWarMgr {
    
protected:
	ClanWarMgr();
	~ClanWarMgr();
    
public:
	static ClanWarMgr& getSingleton();
    
public:

    ClanCityInfo* getClanCityInfo(int stageid);
    
    virtual bool onProc(const Json::Value& val);
    
    int getWarState();
    
    bool isWarStateOpen();
    
    bool isWarStateBattle();
    
    ClanWarAttackClanInfo* getAttackClanInfo(int clanid);
    
public:
    
    int m_warId;
    int m_warState;
    
    int m_warMinWinStar;
    int m_warClanMinMember;
    
    int m_warSignupTime;
    int m_warPrepareTime;
    int m_warStartTime;
    int m_warEndTime;
    int m_warNextTime;
    
    int m_warSignupCityId;
    int m_warDefendCityId;
    
    int m_warCityMinWinStar;
    
    int m_warLogTipStarNums;
    
    vector<ClanCityInfo> m_warCityList;
    vector<ClanWarAttackClanInfo> m_warAttackClanList;
    
    vector<ClanWarUserInfo> m_warDefenseUserList;
    vector<FightMsg> m_warBattleLog;
    
    ClanWarAttackInfo m_warAttackInfo;
    ClanWarUserInfo m_warUserInfo;
    
    ClanCityInfo* m_pSelectCityInfo;
    int m_selectAttackClanId;
    int m_selectDefendIndex;
    
    bool    m_warWin;
    int     m_warCityId;
    string  m_warCityName;
    int     m_warRewardGems;
    int     m_warRanking;
    bool    m_warAttacker;
    int     m_warRewardId;
};

NS_COC_END

#endif /* defined(__GameClient__ClanWarMgr__) */
