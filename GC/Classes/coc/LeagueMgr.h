//
//  PvpMgr.h
//  GameClient
//
//  Created by Lee  on 14-4-2.
//
//

#ifndef __GameClient__PvpMgr__
#define __GameClient__PvpMgr__

#include <iostream>
#include <list>
#include "BaseDef.h"

USING_NS_COC;


//联赛状态
enum LB_STATUS {
    LB_STATUS_CLOSE = 0,//联赛未开始
    LB_STATUS_ON_UNGROUP = 1,//未分组
    LB_STATUS_ON_DOGROUP = 2,//正在分组
    LB_STATUS_ON_GROUPED = 3,//分组完成
    LB_STATUS_BS = 4,//开始结算
    LB_STATUS_ES = 5//结算完成
};

//用户在联赛中得状态 >0 表示在正在参加联赛
enum LB_USER_STATUS {
    LB_USER_STATUS_NOT_NEW = -1,//联赛开始后才注册的
    LB_USER_STATUS_NOT_CUP = -2,//参加联赛的杯数不够
    LB_USER_STATUS_NOT_ACT = -3,//非活跃用户
    LB_USER_STATUS_NOT_VER = -4,//版本不支持
};

class LeagueMgr
{
public:
	
    LeagueMgr();
    ~LeagueMgr();
    
    struct ResInfo
    {
        int resType[MAX_MONEY_NUMS];
        int num[MAX_MONEY_NUMS];
        int types;
    };
    
    struct PlayerInfo
    {
        int clanid;
        int clanFlag;
        std::string clanName;
        std::string uname;
        int star;
        int lv;
        int cup;
        int winNums;
        int loseNums;
        int uid;
        int mrank;  //军衔等级
        int rank;   //联赛排名
        int medal;  //奖章
    };
    
    struct LevelInfo
    {
        int mincup;                     //联赛最低cup要求
        int maxcup;                     //联赛最高cup要求
        int levelid;                    //联赛id
        int searchCost;                 //联赛搜索花费的金币
        std::string name;               //联赛的名字
        std::vector<ResInfo> basLst;    //下一级别战斗奖励，服务器给的时候才取
    };
    
    struct AwardInfo
    {
        ResInfo resInfo;
        int rank;
        bool hasAward;
        bool hasRank;
        int awardid;    //联赛总期数
        int levelid;
        int leagueid;
    };
    
    struct MyGroup
    {
        //        int  rank;       //联赛中排名
        //        int  star;       //联赛中获取的星数
        //        int  win;        //联赛中获胜次数

        int  state;      //用户在联赛中状态 0 不在  1 在
        int  levelid;    //所在联赛等级
        int  groupid;    //分在联赛中第几组
        int  groupnums;    //分在联赛中第几组

        std::string name;  //联赛名字
        std::vector<ResInfo>    rankAwardLst;   //当前联赛排名奖励
        std::vector<ResInfo>    btAwardLst;     //玩家所属联赛获得123星可获得的奖励,只有金币和油
        std::vector<PlayerInfo> playerLst;      //当前联赛中的对手信息(包括自己的信息都放在里面)
        PlayerInfo              myInfo;         //自己的信息保存一份方便使用
        AwardInfo               myAward;        //奖励信息
    };
    
public:
    
	static LeagueMgr& getSingleton();
    
    int m_state;                //0 联赛未开始 1 联赛中
    int m_leagueid;             //联赛期数
    std::string m_des;          //联赛描述
    int     m_startTime;        //联赛开始时间
    int     m_endTime;          //联赛结束时间
    int     m_nextTime;         //下期联赛开始时间
    
    std::vector<LevelInfo> m_levelInfoLst;  //所有联赛的基本信息(所需奖杯，联赛名字...)
    MyGroup m_myGroup;          //自己在联赛中所分到的组中信息

    void setLevelCupInfo(const Json::Value val);
    void setCurBattleAwardInfo(const Json::Value val);
    ResInfo getCurBattleAwardInfo(int star);
    void setRankAwardInfo(const Json::Value val);
    ResInfo getRankAwardInfo(int rank);
    ResInfo getResInfo(Json::Value val);
    void setPlayerInfo(const Json::Value val);
    void setMyAward(const Json::Value val);
    int getSearchCost();
    int getLevel(int cup);
    
    bool canAttack();
    bool isInLeague();
    bool isReadyState();
    bool isBattleState();
    bool isClearingState();
    bool isDoneGroupState();
};

#endif /* defined(__GameClient__PvpMgr__) */
