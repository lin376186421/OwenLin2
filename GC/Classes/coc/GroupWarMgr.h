//
//  GroupWarMgr.h
//  GameClient
//
//  Created by qsun on 14-10-20.
//
//

#ifndef __GameClient__GroupWarMgr__
#define __GameClient__GroupWarMgr__

#include "BaseDef.h"

NS_COC_BEGIN

enum GroupWarState{
    GROUPWAR_STATE_CLOSE,
    GROUPWAR_STATE_SIGNUP,
    GROUPWAR_STATE_BATTLE_PREPARE,
    GROUPWAR_STATE_BATTLE_START,
    GROUPWAR_STATE_BATTLE_SCORE,
    GROUPWAR_STATE_END
};

struct GroupWarStarScore {
    int starLevel;
    int starNums;
    int rewardGems;
};

class GroupWarMgr {
    
protected:
	GroupWarMgr();
	~GroupWarMgr();
    
public:
	static GroupWarMgr& getSingleton();
    
    virtual bool onProc(const Json::Value& val);
    
    int getWarState();
    
    bool isWarStateOpen();
    
    bool isWarStateBattle();
    
    ClanWarAttackClanInfo* getAttackClanInfo(int clanid);
    
public:
    
    int m_warId;
    int m_warState;
    
    int m_warSignupTime;
    int m_warPrepareTime;
    int m_warStartTime;
    int m_warEndTime;
    int m_warNextTime;
    
    int m_warLogTipStarNums;
    
    vector<GroupWarFightLog> m_warBattleLog;
    vector<ClanWarAttackClanInfo> m_warAttackClanList;
    vector<ClanWarUserInfo> m_warDefenseUserList;
    
    ClanWarAttackInfo m_warAttackInfo;
    ClanCityInfo* m_pSelectCityInfo;
    
    ClanWarUserInfo m_warUserInfo;
    
    int m_selectAttackClanId;
    int m_selectDefendIndex;
    
    bool    m_warWin;
    int     m_warCityId;
    string  m_warCityName;
    int     m_warRanking;
    int     m_warStar;
    int     m_warRankingGems;
    int     m_warStarGems;
    bool    m_warAttacker;
    int     m_warRewardId;
    int     m_warStarRewardId;
    
    GroupWarStarScore m_warStarScore[3];
};

NS_COC_END

#endif /* defined(__GameClient__GroupWarMgr__) */
