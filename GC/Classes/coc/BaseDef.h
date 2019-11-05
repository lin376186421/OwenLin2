#ifndef __COC_BASEDEF_H__
#define __COC_BASEDEF_H__

#include <list>
#include <math.h>
#include "cocos2d.h"
#include "ViewString.h"
#include "../Utils/jsoncpp/json.h"
#include "../zephyr/Zephyr.h"
#include "../Global/NGNKit.h"
#include "../zephyr/EncryptInt.h"

USING_NS_CC;
USING_NS_CC_ZEPHYR;
//USING_NS_CC_EXT;

//#define COC_NOBUILDING_ATTACK
#define COC_HTTP_ZLIB

//#define COC_SHOWTILE

#define CUR_SERVER_KEY		"asfupir8gudfvjhwyueor29457053hdhjizfksbv"

#define COC_ATTACK_VER	1512010

#define COC_STATIC_RANDKEY	21

#define FACEBOOK_ON 1
#define LEAGUE_ON 1
#define COC_CLAN 1

#define MAX_COC_NETWORK_FAIL_NUMS			3
#define MAX_COC_NETWORK_TIMEOUT				60

// mask1
#define USER_MASK1_GRADEGEM				1 << 0 // 领宝石
#define USER_MASK1_ALREADYPAY			1 << 1 // 是否已充值
#define USER_MASK1_ALREADYPAYRETURN		1 << 2 // 是否已领取首充
#define USER_MASK1_GOOGLE_PLUS          1 << 5 // google+

#define USER_MASK2_LEAGUE_GUIDE         1 << 0 // 联赛引导

#define NS_COC_BEGIN	namespace coc{
#define NS_COC_END	}
#define USING_NS_COC                     using namespace coc

#define _random (rand()%RAND_MAX)

NS_COC_BEGIN

class BaseObj;
class PlayerScene;
class SceneBgr;
class Building;
class Wall;
class Person;
//class ImgMgr;
class Particle;
class ResConfig;

int getAsInt(const Json::Value& val);

const float SCENE_NORMAL_SCALE = 0.5;

const float SCALE_PERSONSPEED	=	1.0f;
const int	CASTELSOLDIER_MAXNUM	=	25;
const int	SOLDIERTYPE_MAXNUM	=	10;

const int	HERO_HP_SECOND		=	5;

const int	MAXFRAME_SEARCHNUMS	=	5;

const int   PLAY_TIMES_OUT = 5 * 60;

const int   FIGHT_READY_TIME = 30;

const int   FIGHT_TIME = 3 * 60;

const int   WALL_SEARCH_GROUP_NUMS = 3;

const int   TRASH_GROW_TIME = 12 * 60 * 60;

const int   TRASH_MAX_NUMS = 30;

static bool BUILDING_UPLEVEL_ID_FIX = false; //fix building uplevel id

// dir
const int	DIR_LEFT		=	0;
const int	DIR_LEFT_UP		=	1;
const int	DIR_UP			=	2;
const int	DIR_UP_RIGHT	=	3;
const int	DIR_RIGHT		=	4;
const int	DIR_RIGHT_DOWN	=	5;
const int	DIR_DOWN		=	6;
const int	DIR_DOWN_LEFT	=	7;

// dir extensions
const int	DIREX_LEFT			=	0;
const int	DIREX_LEFT_UP		=	1;
const int	DIREX_UP_LEFT		=	2;
const int	DIREX_UP			=	3;
const int	DIREX_UP_RIGHT		=	4;
const int	DIREX_RIGHT_UP		=	5;
const int	DIREX_RIGHT			=	6;
const int	DIREX_RIGHT_DOWN	=	7;
const int	DIREX_DOWN_RIGHT	=	8;
const int	DIREX_DOWN			=	9;
const int	DIREX_DOWN_LEFT		=	10;
const int	DIREX_LEFT_DOWN		=	11;

//shop type
const int   SHOP_TYPE_TREASURE      =   1;
const int   SHOP_TYPE_RESOURCES     =   2;
const int   SHOP_TYPE_DEFENSES      =   3;
const int   SHOP_TYPE_ARMY          =   4;
const int   SHOP_TYPE_DECORATIONS   =   5;
const int   SHOP_TYPE_SHIELD        =   6;
const int   SHOP_TYPE_FREEGEMS      =   7;


// building type
const int	BUILDING_TYPE_NORMAL        =	0;		// normal
const int	BUILDING_TYPE_COLLECT       =	1;		// collect
const int	BUILDING_TYPE_STORAGE	    =	2;		// storage
const int	BUILDING_TYPE_BARRACK       =	3;		// barrack
const int	BUILDING_TYPE_CLAN          =	4;		// clan
const int	BUILDING_TYPE_HERO_ALTAR    =	5;
const int   BUILDING_TYPE_DECORATION    =   9;
const int	BUILDING_TYPE_ATTACK	    =	10;		// attack
const int	BUILDING_TYPE_CANNON        =	11;		// cannon
const int   BUILDING_TYPE_TOWER         =   12;     // tower
const int	BUILDING_TYPE_MORTAR	    =	13;		// mortar
const int   BUILDING_TYPE_AIR_DEFENSE   =   14;     // air defense
const int	BUILDING_TYPE_WALL		    =	18;		// wall
const int	BUILDING_TYPE_TRAP		    =	19;		// trap
const int   BUILDING_TYPE_CLAN_ALLIES   =   20;

// building id
const int   BUILDING_CANNON         =   101;
const int   BUILDING_ARCHER_TOWER   =   102;
const int   BUILDING_WALL           =   103;
const int   BUILDING_BIG_MORTAR     =   104;
const int   BUILDING_AIR_DEFENSE    =   106;
const int   BUILDING_WIZARD_TOWER   =   108;
const int   BUILDING_TOWN_HALL      =   201;
const int   BUILDING_BUILDER_HOME   =   202;
const int   BUILDING_ELIXIR_COLLECTOR = 203;
const int   BUILDING_ELIXIR_STORAGE =   204;
const int   BUILDING_GOLD_MINE      =   205;
const int   BUILDING_GOLD_STORAGE   =   206;
const int   BUILDING_ARMY_CAMP      =   301;
const int   BUILDING_BARRACKS       =   302;
const int   BUILDING_LABORATORY     =   303;
const int   BUILDING_CLAN_ALLIES    =   304;
const int   BUILDING_CLAN_CASTLE    =   305;
const int   BUILDING_HERO_ALTAR     =   306;
const int   BUILDING_BOMB           =   501;
const int   BUILDING_SPIKE_TRAP     =   502;
const int   BUILDING_AIR_BOMB       =   503;
const int   BUILDING_BIG_BOMB       =   504;
//widget
const int   BUILDING_MAIL_BOX       =   601;
const int   BUILDING_STELE          =   602;
const int   BUILDING_WEATHER_VANE   =   603;
const int   BUILDING_ROAD_SIGN      =   604;
const int   BUILDING_BULLETIN_BOARD =   605;
const int   BUILDING_FLOWER_BED1    =   606;
const int   BUILDING_FLOWER_BED2    =   607;
const int   BUILDING_LETTER         =   608;
//trash
const int   BUILDING_TRASH_FLOWER   =   901;
const int   BUILDING_TRASH_TRUNK    =   902;
//
const int   BUILDING_LETTER_NUM     =   40; //letter num

//person id
const int   PERSON_BARBARIAN        =   101;
const int   PERSON_ARCHER           =   102;
const int   PERSON_GOBLIN           =   103;
const int   PERSON_GIANT            =   104;
const int   PERSON_WALL_BREAKER     =   105;
const int   PERSON_BALLOON          =   106;
const int   PERSON_WIZARD           =   107;
const int   PERSON_HEALER           =   108;
const int   PERSON_DRAGON           =   109;
const int   PERSON_PEKKA            =   110;

const int   PERSON_WORKMAN          =   201;
const int   PERSON_WORKWOMAN        =   202;

const int   PERSON_HERO             =   301;

const int   CLAN_SOLDIER_ID         =   90101;

// building ctrl
const int	BUILDING_CTRL_NORMAL	=	0;		// normal
const int	BUILDING_CTRL_BUILD     =	1;		// build
const int	BUILDING_CTRL_MOVE		=	2;		// move
const int	BUILDING_CTRL_SELECTED	=	3;		// select

// fight replay type
const int   FIGHT_REPLAY_ATTACK     =   0;
const int   FIGHT_REPLAY_DEFENSE    =   1;

const int   TARGET_WALL_COMBO       =   3;

// 货币种类
const int	MAX_MONEY_NUMS			=	3;

const int	MONEY_GEM				=	0;		// 宝石
const int	MONEY_GOLD				=	1;		// 金币
const int	MONEY_ELIXIR			=	2;		// 油

const int   MONEY_FROM_NORMAL       =   0;
const int   MONEY_FROM_AWARDS       =   1;
const int   MONEY_FROM_BUY          =   2;

const int   LEAGUE_MEDAL            =  11; //league奖牌

//const int   LEAFUE_MEDAL_DIAMOND    =  11; //钻石奖牌
//const int   LEAFUE_MEDAL_CRYSTAL    =  12; //水晶奖牌
//const int   LEAFUE_MEDAL_GOLD       =  13; //黄金奖牌
//const int   LEAFUE_MEDAL_SILVER     =  14; //白银奖牌
//const int   LEAFUE_MEDAL_BRONZE     =  15; //青铜奖牌

//Attack Target
const int   ATTACK_TARGET_NORMAL    =   0;
const int   ATTACK_TARGET_DEFENSE   =   1;
const int   ATTACK_TARGET_WALL      =   2;
const int   ATTACK_TARGET_RESOURCES =   3;

// Notify
const int   NOTIFY_SHOW_GAME        =   0;
const int	NOTIFY_BASEINFO			=	1;		// 基本信息
const int	NOTIFY_DROPSOLDIER		=	2;		// 放兵
const int	NOTIFY_FIGHT_END        =	3;		// 结束战斗
const int	NOTIFY_FIGHTSCORE		=	4;		// 对战结果
const int	NOTIFY_OTHERBASEINFO	=	5;		// 其他信息
const int	NOTIFY_NEWFIND          =	6;
const int	NOTIFY_DESTROY_HOME     =	7;
const int	NOTIFY_LOADINGPROGRESS  =	8;
const int   NOTIFY_GUIDE_REFRESH    =   9;
const int   NOTIFY_UPLEVEL          =   10;		// 玩家升级
const int   NOTIFY_RANKEND          =   11;		// 排行榜数据刷新
const int   NOTIFY_PURCHASE_WAIT    =   12;
const int   NOTIFY_PAY_OK	        =   13;		// 内购成功
const int   NOTIFY_PAY_FAILED       =   14;		// 内购失败
const int   NOTIFY_PAY_ERROR        =   15;
const int   NOTIFY_MAIL_BATTLELOG   =   16;		//
const int   NOTIFY_MAIL_ATTACKLOG   =   17;		//
const int   NOTIFY_MAIL_INBOX       =   18;
const int   NOTIFY_ACHIEVEMENT      =   19;		//
const int   NOTIFY_ACHIEVEMENT_OK   =   20;		// 有成就可以领取
const int   NOTIFY_REPLAY_OK		=   21;		// 回放完成
const int   NOTIFY_HERO             =   22;		// 刷新英雄
//const int   NOTIFY_RANDSKILL      =   23;
//const int   NOTIFY_RANDSKILLx10   =   23;
const int   NOTIFY_NOHERO           =   24;
const int   NOTIFY_RANDSKILL3       =   25;
const int   NOTIFY_NETWORK_FAIL		=   26;     // 网络连接失败
const int   NOTIFY_NETWORK_ERROR    =   27;
const int   NOTIFY_USERLOGIN_INFO	=   28;     // bind info
const int   NOTIFY_NEW_USERLOGIN	=   29;     // new bind
const int   NOTIFY_CLAN_CREATE      =   30;     //帮会创建成功
const int   NOTIFY_CLAN_REFRESH     =   31;     //自己的帮会
const int   NOTIFY_CLAN_RANKING     =   32;     //排行榜
const int   NOTIFY_CLAN_SEARCH      =   33;
const int   NOTIFY_CLAN_CHANGE      =   34;
const int   NOTIFY_USER_ONLINE      =   35;
const int   NOTIFY_USER_SHIELD      =   36;
const int   NOTIFY_REVENGE_USER     =   37;
const int   NOTIFY_SIGNIN_INFO      =   38;
const int   NOTIFY_SPIN_AWARD       =   39;
const int   NOTIFY_USER_ATTACK      =   40;
//const int   NOTIFY_CLAN_REFRESH     =   40;
//const int   NOTIFY_CLAN_HELPSOLDIER =   41;		//送兵返回
const int   NOTIFY_MESSAGE_CLOSE    =   43;
const int	NOTIFY_MYCLANCHG		=	44;		//自己的帮派变化 0表示退出 其他值表示加入的帮派的id
const int	NOTIFY_MYCLANJOBCHG		=	45;		//自己的帮派职务变化
const int   NOTIFY_PVE_MAP          =   46;
const int   NOTIFY_PVE_ADD_LIFE     =   47;
const int   NOTIFY_ALLIANCE_EXITOK  =   48;     //退出工会成功
const int   NOTIFY_HERO_FULL_HP     =   49;     //英雄血满
const int   NOTIFY_HEROHPCHG	    =   50;		// 英雄hp变化

const int   NOTIFY_LG_LEVEL_INFO_OK	=   51;		//联赛信息初始化

const int   NOTIFY_FB_LOGIN_OK      =   52;		//fb登录成功
const int   NOTIFY_FB_LOGIN_OUT_OK  =   53;		//fb退出
const int   NOTIFY_FB_FRIENDS_OK	=   54;		//fb获得好友信息
const int   NOTIFY_FB_POST_OK       =   55;
const int   NOTIFY_FB_GET_PIC_OK    =   56;
const int   NOTIFY_FB_INVITE_OK     =   57;
const int   NOTIFY_FB_FRIEND_CHECK_OK =   58;

const int   NOTIFY_CLANWAR_SIGNSUCCESS  = 59;
const int   NOTIFY_CLANWAR_ATTACKCLANS  = 60;
const int   NOTIFY_CLANWAR_DEFENSEINFO  = 61;
const int   NOTIFY_CLANWAR_BATTLEINFO   = 62;
const int   NOTIFY_CLANWAR_BATTLELOG    = 63;
const int   NOTIFY_CLANWAR_REWARD       = 64;
const int   NOTIFY_CLANWAR_ATTACKUSER   = 65;
const int   NOTIFY_CLANWAR_INFO         = 66;
const int   NOTIFY_CLANWAR_CITYINFO     = 67;
const int   NOTIFY_GROUPWAR_INFO        = 70;
const int   NOTIFY_GROUPWAR_DEFENSEINFO = 71;
const int   NOTIFY_GROUPWAR_BATTLEINFO  = 72;
const int   NOTIFY_GROUPWAR_ATTACKCLANS = 73;
const int   NOTIFY_GROUPWAR_ATTACKUSER  = 74;
const int   NOTIFY_GROUPWAR_BATTLELOG   = 75;
const int   NOTIFY_FB_SHARE_FINISH      = 76;
const int   NOTIFY_ADS_LOADINF_FAILD    = 77;

//
const int   MSG_EXIT                =   100;
const int   MSG_BUILDING_STOP       =   102;
const int   MSG_BUILDING_FAST       =   103;
const int   MSG_SHARE_REPLAY        =   104;
const int   MSG_END_BATTLE          =   105;
const int   MSG_BUY_GOLD            =   106;
const int   MSG_BUY_OIL             =   107;
const int   MSG_BUY_SHIELD          =   108;
const int   MSG_BUY_OK              =   110;
const int   MSG_HAVE_PROTECT        =   111;
const int   MSG_NO_SOLDIER          =   112;
const int   MSG_RATE                =   113;     //评分
const int   MSG_RATE_GEM            =   114;     //评分的宝石
const int   MSG_REPLAY              =   115;
const int   MSG_FILL_HERO_HP        =   116;
const int   MSG_CHANGE_GLYPH_SKILL  =   117;
const int   MSG_USERID_LOGIN        =   118;
const int   MSG_USERID_CHANGED      =   119;
const int   MSG_USERID_CONFIRM      =   120;
const int   MSG_TIMES_OUT           =   122;
const int   MSG_VILLAGE_BEATTACK    =   123;
const int   MSG_CLAN_LEAVE          =   124;
const int   MSG_CLAN_JOIN           =   125;
const int   MSG_EXIT_CLAN_FAILED    =   126;
const int   MSG_PVE_NOT_LIFE        =   127;  //MSG_PVE_NOT_LIFE server return 127
const int   MSG_RESTORE_PVE_LIFE    =   128;
const int   MSG_REQUEST_TROOPS      =   129;
const int   MSG_REQUEST_TROOPS_AGAIN=   130;
const int   MSG_PURCHASE_WAIT       =   131;
const int   MSG_ADD_GEM             =   132;
const int   MSG_GET_GEM             =   133;
const int   MSG_CLAN_NOT_NATIONAL   =   134;
const int   MSG_FB_SHARE_TIP        =   135;
const int   MSG_REMOVE_ORNAMENT     =   136;
const int   MSG_EDIT_SAVE           =   137;
const int   MSG_EDIT_EXIT           =   138;
const int   MSG_UPGRADE_WALLS       =   139;

// Message
const int	MSG_NORMAL			=	0;
const int	MSG_ERROR			=	-1;
const int   MSG_NETWORK_FAIL    =   -2;
const int   MSG_DATE_ERROR      =   -3;   //MSG_DATE_ERROR server return -3
const int   MSG_DATA_NOTSYNC    =   -4;   //MSG_DATA_NOTSYNC server return -4
const int	MSG_POSERR			=	-5;
const int   MSG_NOWORKER        =   -10;
const int   MSG_NOGEM           =   -11;
const int   MSG_NOGOLD          =   -12;
const int   MSG_NOELIXIR        =   -13;

enum GetGemType {
    GETGEM_FINDMATCH_PVP = 0,
    GETGEM_FINDMATCH_LEAGUE = 1,
    GETGEM_CREATE_CLAN = 2,
    GETGEM_BUILD_BUILDING = 3,
    GETGEM_REMOVE_BUILDING = 4,
    GETGEM_UPLEVEL_BUILDING = 5,
    GETGEM_UPLEVEL_HERO = 6,
    GETGEM_UPLEVEL_WALLS = 7,
    GETGEM_UPLEVEL_SOLDIER = 8
};

enum AsyncViewType {
    ASYNC_VIEW_PAY_OK = 0,
    ASYNC_VIEW_PAY_ERROR = 1,
    ASYNC_VIEW_ADD_GEM = 2,
    ASYNC_FB_LOGIN = 3,
    ASYNC_FB_GET_PIC = 4,
    ASYNC_FB_INVITE = 5,
    ASYNC_FB_POST = 6,
};

// user state
const int USER_STATE_ONLINE         =   1;
const int USER_STATE_SHIELD         =   1 << 1;
const int USER_STATE_BEATTACK       =   1 << 2;

// Logic State
const int	LOGICSTATE_WAIT			=	0;
const int	LOGICSTATE_NORMAL		=	1;
const int	LOGICSTATE_ATTACK		=	2;
const int	LOGICSTATE_REPLAY		=	3;
const int	LOGICSTATE_VIEW			=	4;
const int	LOGICSTATE_LOADING		=	5;

// person frame action
const int	PERSON_ACTION_STAND		=	0;
const int	PERSON_ACTION_WALK		=	1;
const int	PERSON_ACTION_ATTACK	=	2;
const int	PERSON_ACTION_DEAD		=	3;
const int	PERSON_ACTION_WIN		=	4;

// person act
const int	PERSON_ACT_NORMAL		=	0;
const int	PERSON_ACT_WALK			=	1;
const int	PERSON_ACT_ATTACK		=	2;
const int	PERSON_ACT_DROPDOWN		=	3;
const int	PERSON_ACT_JUMP			=	4;
const int	PERSON_ACT_USESKILL		=	5;
const int   PERSON_ACT_HIDE         =   6;
const int   PERSON_ACT_DEAD         =   7;

// person type
const int	PERSON_TYPE_FREE		=	0;
const int	PERSON_TYPE_TOWER		=	1;
const int	PERSON_TYPE_SOLDIER		=	2;

// person state
const int	PERSON_STATE_ATTACK             =	0;
const int	PERSON_STATE_WAITATTACK_TARGET	=	1;
const int	PERSON_STATE_WAIT_TARGET		=	2;
const int	PERSON_STATE_WAITFREE_TARGET	=	3;
const int	PERSON_STATE_WIN				=	4;
const int	PERSON_STATE_WAITNEXTATTACK     =	5;		// 2次攻击之间
const int   PERSON_STATE_DEAD               =   6;

// person status
const int	PERSON_STATUS_DEFENSE_ACTIVE    =	1;
const int   PERSON_STATUS_MOVE_OUT          =   1 << 1;
const int   PERSON_STATUS_UPGRADE           =   1 << 2;
const int	PERSON_STATUS_RELEASE           =	1 << 30;

// obj type
const int	OBJTYPE_BUILDING			=	1;
const int	OBJTYPE_PERSON				=	2;

// flying height
const int	FLYING_HEIGHT				=	120;

// 攻方 和 守方
const int	CAMP_ATTACK					=	0;	// 攻方
const int	CAMP_DEFENCE				=	1;	// 守方
const int   CAMP_WORKER                 =   2;  // worker

const float PERSON_RESEARCHTIME[2]		=	{ 6.0f, 3.0f };

// HttpClient状态
const int	HTTPCLIENT_STATE_WAIT			=	0;	// 未初始化完成
const int	HTTPCLIENT_STATE_PROCLAST		=	1;	// 加载完成，有消息缓存需要处理
const int	HTTPCLIENT_STATE_INIT			=	2;	// 初始化完成
const int	HTTPCLIENT_STATE_WAITLOGIN		=	3;	// 等待新的登录完成
const int	HTTPCLIENT_STATE_WAITLOGINEX	=	4;	// 等待新的登录完成，可以保存了

enum AttackType {
    ATTACK_TYPE_NULL = -1,
    ATTACK_TYPE_PVE = 0,
    ATTACK_TYPE_PVP = 1,
    ATTACK_TYPE_LEAGUE = 2,
    ATTACK_TYPE_CLANWAR = 3,
    ATTACK_TYPE_GROUPWAR = 4
};

enum FightMode {
    FIGHT_MODE_WAIT = 0,
    FIGHT_MODE_BEGIN = 1,
    FIGHT_MODE_END = 2
};

enum BackHomeScene {
    BACK_HOME_TOWN_HALL = 0,
    BACK_HOME_PVE_MAP = 1,
    BACK_HOME_CLANWAR_BATTLE = 2,
    BACK_HOME_GROUPWAR_BATTLE = 3
};

struct BuildingInfo {
	int				buildingid;

	std::string		name;
	std::string		res;

	int				type;
	int				width;

	int				offHeight;

	EncryptInt		buildtime[3];
	EncryptInt		buildres;
	EncryptInt		buildcost;
	EncryptInt		removecost;

	EncryptInt		townhalllevel;

	EncryptInt		maxstored[2];
    
	EncryptInt		producesres;
	EncryptInt		producesperhour;

	int				hp;

	int				regentime;

	int				attackrange;
	int				attackspeed;
	int				damage;
	int				airtargets;
	int				groundtargets;
	int				minattackrange;
	int				unitproduction;
    
    int             exp;

	int				towerSoldierID;
	int				towerSoldierNums;

	int				towerSoldierX[3];
	int				towerSoldierY[3];
    
    int             housingspace;

	int				frameNums;
	bool			isAniLoop;

	int				connonFrameNums;
	int				connonMainFrame;

	int				attack_exp;
};

struct SoldierInfo {
	int				soldierid;

	std::string		name;
	std::string		resname;

	int				frames[5];		// 0 - stand, 1 - walk, 2 - attack, 3 - dead, 4 - win
	int				framesTime[5];
	int				dirmode;

	int				timeShoot;			// 射击时间

	EncryptInt		housingSpaceex;
	EncryptInt		barrackLevelex;
	EncryptInt		laboratoryLevelex;
	
    EncryptInt		speedex;
    EncryptInt      hpex;

	EncryptInt		trainingTime;
	EncryptInt		trainingRes;
	EncryptInt		trainingCost;

	EncryptInt		upgradeTimeHour;
	EncryptInt		upgradeRes;
	EncryptInt		upgradeCost;

	EncryptInt		attackRangeex;
	EncryptInt		attackSpeedex;
	EncryptInt      damageex;
	int				preferedTargetDamageMod; //攻击力加成(针对偏好目标)
	int				damageRadius; //伤害范围
	int				preferedTargetBuilding;
	int				isFlying;
	int				airTargets;
	int				groundTargets;
    int             attackTarget;
	int				needexp;

	int				needtownhall;
	int				baseexp;
	int				recoverhp;
};

struct UserInfo {
	std::string		uname;
	std::string		handle;

	int				uid;
	int				sessionid;

	int				curlevel;
	int				exp;
    int             military;
    int             medal;

    int             baselevel;
    
	//int				gold;
	//int				oil;
	//int				gem;
	EncryptInt		goldex;
	EncryptInt		oilex;
	EncryptInt		gemex;

	//int				cup;
	EncryptInt		cupex;

	//int				maxgold;
	//int				maxoil;

	EncryptInt		maxgoldex;
	EncryptInt		maxoilex;

	int				protectTime;
	int				lastProtectTime;

	//int				buildbuildingnums;
	//int				totalworkernums;
	int				maxBarrackLevel;

	EncryptInt		buildbuildingnumsex;
	EncryptInt		totalworkernumsex;

	int				vip;
    
	int				soldierlevel[10];

	int				mask[2];
};

enum BuildingState {
    BUILDING_STATE_NONE = -1,
    BUILDING_STATE_BUILD = 0,
    BUILDING_STATE_UPGRADING = 1,
    BUILDING_STATE_NORMAL = 2
};

enum EditBuildStatus {
    EDITBUILD_STATUS_INVALID = 0,
    EDITBUILD_STATUS_SUCCESS = 1,
};

struct UserBuildingInfo {
    int             id;
	int				buildingid;

	int				x;
	int				y;

    int             level;
	int             state;  //0-建设中，1-升级中，2-正常，-1未建造
	int				curtime;
	int				lasttime;

    int             hp;
    int             maxhp;
    
	int				maxres;
	int				currestime;
	int				timeres;

	int				barrackid;
	int				troophousingid;

	EncryptInt		maxstore[2];
	EncryptInt		store[2];
    EncryptInt      laststore[2];
};

struct BarrackSoldierInfo {
	int				pid;
	EncryptInt		numsex;
	EncryptInt		needtimeex;
};

struct BarrackInfo {
	int				barrackid;

	int				curtime;
	int				lasttime;
    
    int             offtime;

	EncryptInt		maxnumsex;

	std::list<BarrackSoldierInfo>	lstSoldier;

	bool			ischg;
    
    bool            isUpgrade;
    bool            isPause;
};

struct TroophousingSoldierInfo {
	int				pid;
	EncryptInt		numsex;
	EncryptInt		spaceex;
};

struct TroophousingInfo {
	int				troophousingid;

	EncryptInt		maxnumsex;

	std::list<TroophousingSoldierInfo>	lstSoldier;

	bool			ischg;
};

struct FightSoldierInfo {
	int				pid;
	
	int             nums;
//
//	bool operator < (const FightSoldierInfo& info) const {
//
//		if(pid > 30000 && info.pid < 30000)
//			return true;
//		else if(pid < 30000 && info.pid > 30000)
//			return false;
//
//		return pid < info.pid;
//	}
};

struct FightSoldierInfoEx {
	int				pid;
	EncryptInt		numsex;

	void operator = (FightSoldierInfoEx fsi) {
		pid = fsi.pid;
		numsex = fsi.numsex;
	}
    
    bool operator < (const FightSoldierInfoEx& info) const {
        
		if(pid > 30000 && info.pid < 30000)
			return true;
		else if(pid < 30000 && info.pid > 30000)
			return false;
        
		return pid < info.pid;
	}
};

struct FightUserInfo {
	int				uid;
	char			name[32];
	int				gold;
	int				oil;
	int				curlevel;
	int				exp;
	int				cup;
};

struct FightUserInfoEx
{
    int				uid;
	char			name[32];
	int				gold;
	int				oil;
	int				curlevel;
	int				exp;
	int				cup;
    
    int             clanid;
    int             clanflag;
    char            clanname[32];
    
    int             military;
    
    int             clanjointype;
    int             clancup;
    int             clanneedcup;
    
    char            exbyte[52]; //加52个字节,方便后面扩展
    
    void operator = (FightUserInfo user) //temp
    {
        uid	= user.uid;
        strcpy(name, user.name);
        gold = user.gold;
        oil = user.oil;
        curlevel = user.curlevel;
        exp = user.exp;
        cup = user.cup;
        
        clanid = 0;
        clanflag = 0;
        clanname[0] = '\0';
        clanjointype = 0;
        clancup = 0;
        clanneedcup = 0;
        military = 0;
    }
};

struct FightUserInfoEx2 { //140829 处理clan 追加信息
    int				uid;
	char			name[32];
	int				gold;
	int				oil;
	int				curlevel;
	int				exp;
	int				cup;
    
    int             military;
    
    char            exbyte[32]; //加32个字节,方便后面扩展
    
    int             clanid;
    int             clanflag;
    char            clanname[32];
    int             clanjointype;
    int             clancup;
    int             clanneedcup;
    
    char            exbyte2[32]; //加32个字节,方便后面扩展
    
    void operator = (FightUserInfo& user) //temp
    {
        uid	= user.uid;
        strcpy(name, user.name);
        gold = user.gold;
        oil = user.oil;
        curlevel = user.curlevel;
        exp = user.exp;
        cup = user.cup;
        military = 0;

        clanid = 0;
        clanflag = 0;
        clanname[0] = '\0';
        clanjointype = 0;
        clancup = 0;
        clanneedcup = 0;
    }
    
    void operator = (FightUserInfoEx& user) //temp
    {
        uid	= user.uid;
        strcpy(name, user.name);
        gold = user.gold;
        oil = user.oil;
        curlevel = user.curlevel;
        exp = user.exp;
        cup = user.cup;
        
        clanid = user.clanid;
        clanflag = user.clanflag;
        strcpy(clanname, user.clanname);
        military = user.military;
        clanjointype = user.clanjointype;
        clancup = user.clancup;
        clanneedcup = user.clanneedcup;
    }
};

struct FightScoreInfo {
	float			progress;					

	int				numsDestroyBuilding;		
	int				numsBuilding;				
	int				gold;						
	int				oil;
    int             storegold;
    int             storeoil;

	FightUserInfo	user[2];

	int					numsfsi;
	FightSoldierInfo	fsi[16];
};

struct FightScoreInfoEx {
    float			progress;
    
	int				numsDestroyBuilding;
	int				numsBuilding;
	int				gold;
	int				oil;
    int             storegold;
    int             storeoil;
    
    char            exbyte[64]; //加64空字节,方便后面扩展
    
    FightUserInfoEx user[2];
    int				numsfsi;
	FightSoldierInfoEx	fsi[32];
    
    void operator = (FightScoreInfo& _fsi) //temp
    {
        progress = _fsi.progress;
        
        numsDestroyBuilding = _fsi.numsDestroyBuilding;
        numsBuilding = _fsi.numsBuilding;
        gold = _fsi.gold;
        oil = _fsi.oil;
        storegold = _fsi.storegold;
        storeoil = _fsi.storeoil;
        
        user[0] = _fsi.user[0];
        user[1] = _fsi.user[1];
        
        numsfsi = _fsi.numsfsi;
        
        for (int i = 0; i < 16; i++) {
            fsi[i].pid = _fsi.fsi[i].pid;
            fsi[i].numsex = _fsi.fsi[i].nums;
        }
    }
};

struct FightScoreInfoEx2 {
    float			progress;
    
	int				numsDestroyBuilding;
	int				numsBuilding;
	int				gold;
	int				oil;
    int             storegold;
    int             storeoil;
    
    char            exbyte[64]; //加64空字节,方便后面扩展
    
    FightUserInfoEx2 user[2];
    int				 numsfsi;
	FightSoldierInfoEx	fsi[32];
    
    void operator = (FightScoreInfo& _fsi) //temp
    {
        progress = _fsi.progress;
        
        numsDestroyBuilding = _fsi.numsDestroyBuilding;
        numsBuilding = _fsi.numsBuilding;
        gold = _fsi.gold;
        oil = _fsi.oil;
        storegold = _fsi.storegold;
        storeoil = _fsi.storeoil;
        
        user[0] = _fsi.user[0];
        user[1] = _fsi.user[1];
        
        numsfsi = _fsi.numsfsi;
        
        for (int i = 0; i < 16; i++) {
            fsi[i].pid = _fsi.fsi[i].pid;
            fsi[i].numsex = _fsi.fsi[i].nums;
        }
    }
    
    void operator = (FightScoreInfoEx& _fsi) //temp
    {
        progress = _fsi.progress;
        
        numsDestroyBuilding = _fsi.numsDestroyBuilding;
        numsBuilding = _fsi.numsBuilding;
        gold = _fsi.gold;
        oil = _fsi.oil;
        storegold = _fsi.storegold;
        storeoil = _fsi.storeoil;
        
        user[0] = _fsi.user[0];
        user[1] = _fsi.user[1];
        
        numsfsi = _fsi.numsfsi;
        
        for (int i = 0; i < 16; i++) {
            fsi[i].pid = _fsi.fsi[i].pid;
            fsi[i].numsex = _fsi.fsi[i].numsex;
        }
    }
    
    void operator = (FightScoreInfoEx2& _fsi) //temp
    {
        progress = _fsi.progress;
        
        numsDestroyBuilding = _fsi.numsDestroyBuilding;
        numsBuilding = _fsi.numsBuilding;
        gold = _fsi.gold;
        oil = _fsi.oil;
        storegold = _fsi.storegold;
        storeoil = _fsi.storeoil;
        
        user[0] = _fsi.user[0];
        user[1] = _fsi.user[1];
        
        numsfsi = _fsi.numsfsi;
        
        for (int i = 0; i < 16; i++) {
            fsi[i].pid = _fsi.fsi[i].pid;
            fsi[i].numsex = _fsi.fsi[i].numsex;
        }
    }
};

struct FightScoreInfoEx3 {
    float			progress;
    
	int				numsDestroyBuilding;
	int				numsBuilding;
	EncryptInt		goldex;
	EncryptInt		oilex;
	EncryptInt		storegoldex;
	EncryptInt		storeoilex;
    
    char            exbyte[64]; //加64空字节,方便后面扩展
    
    FightUserInfoEx2 user[2];
    int				 numsfsi;
	FightSoldierInfoEx	fsi[32];
    
    void reset()
    {
        memset(this, 0, sizeof(FightScoreInfoEx3));
        
        goldex = 0;
        oilex = 0;
        storegoldex = 0;
        storeoilex = 0;
    }

    void operator = (FightScoreInfo& _fsi) //temp
    {
        progress = _fsi.progress;
        
        numsDestroyBuilding = _fsi.numsDestroyBuilding;
        numsBuilding = _fsi.numsBuilding;
        goldex = _fsi.gold;
        oilex = _fsi.oil;
        storegoldex = _fsi.storegold;
        storeoilex = _fsi.storeoil;
        
        user[0] = _fsi.user[0];
        user[1] = _fsi.user[1];
        
        numsfsi = _fsi.numsfsi;
        
        for (int i = 0; i < 16; i++) {
            fsi[i].pid = _fsi.fsi[i].pid;
            fsi[i].numsex = _fsi.fsi[i].nums;
        }
    }
    
    void operator = (FightScoreInfoEx& _fsi) //temp
    {
        progress = _fsi.progress;
        
        numsDestroyBuilding = _fsi.numsDestroyBuilding;
        numsBuilding = _fsi.numsBuilding;
        goldex = _fsi.gold;
        oilex = _fsi.oil;
        storegoldex = _fsi.storegold;
        storeoilex = _fsi.storeoil;
        
        user[0] = _fsi.user[0];
        user[1] = _fsi.user[1];
        
        numsfsi = _fsi.numsfsi;
        
        for (int i = 0; i < 16; i++) {
            fsi[i].pid = _fsi.fsi[i].pid;
            fsi[i].numsex = _fsi.fsi[i].numsex;
        }
    }
    
    void operator = (FightScoreInfoEx2& _fsi) //temp
    {
        progress = _fsi.progress;
        
        numsDestroyBuilding = _fsi.numsDestroyBuilding;
        numsBuilding = _fsi.numsBuilding;
        goldex = _fsi.gold;
        oilex = _fsi.oil;
        storegoldex = _fsi.storegold;
        storeoilex = _fsi.storeoil;
        
        user[0] = _fsi.user[0];
        user[1] = _fsi.user[1];
        
        numsfsi = _fsi.numsfsi;
        
        for (int i = 0; i < 16; i++) {
            fsi[i].pid = _fsi.fsi[i].pid;
            fsi[i].numsex = _fsi.fsi[i].numsex;
        }
    }
    
    void operator = (const FightScoreInfoEx3& _fsi) //temp
    {
        progress = _fsi.progress;
        
        numsDestroyBuilding = _fsi.numsDestroyBuilding;
        numsBuilding = _fsi.numsBuilding;
        goldex = _fsi.goldex;
        oilex = _fsi.oilex;
        storegoldex = _fsi.storegoldex;
        storeoilex = _fsi.storeoilex;
        
        user[0] = _fsi.user[0];
        user[1] = _fsi.user[1];
        
        numsfsi = _fsi.numsfsi;
        
        for (int i = 0; i < 16; i++) {
            fsi[i].pid = _fsi.fsi[i].pid;
            fsi[i].numsex = _fsi.fsi[i].numsex;
        }
    }
    
    void toFightScoreInfoEx2(FightScoreInfoEx2& _fsi)
    {
        _fsi.progress = progress;
        
        _fsi.numsDestroyBuilding = numsDestroyBuilding;
        _fsi.numsBuilding = numsBuilding;
        _fsi.gold = goldex;
        _fsi.oil = oilex;
        _fsi.storegold = storegoldex;
        _fsi.storeoil = storeoilex;
        
        _fsi.user[0] = user[0];
        _fsi.user[1] = user[1];
        
        _fsi.numsfsi = numsfsi;
        
        for (int i = 0; i < 16; i++) {
            _fsi.fsi[i].pid = fsi[i].pid;
            _fsi.fsi[i].numsex = fsi[i].numsex;
        }
    }
};

#define COC_ATTACK_VER_FIX 1407070 //处理追加字节
#define COC_ATTACK_VER_FIX2 1408290 //处理clan 追加信息
#define COC_ATTACK_VER_FIX3 1410130 //处理用户掠夺资源内存修改
#define COC_ATTACK_VER_FIX4 1411240 //处理对战地图信息及资源收集建筑掠夺&Base32 to Base64
#define COC_ATTACK_VER_FIX5 1412180 //处理result 和 result 2 错乱问题

struct FightMsg {  //COC_ATTACK_VER	1407070
	int	msgid;
    
    FightScoreInfoEx3 fsi;
    
	int	star;
    int attackCup;
    int defendCup;
    
	time_t time;
    
	int	isAlreadyAttack;
	int	clientver;
    
	bool canReplay() const {
		return clientver == COC_ATTACK_VER;
	}

	void operator = (const FightMsg msg) {
		msgid = msg.msgid;

		fsi = msg.fsi;

		star = msg.star;
		attackCup = msg.attackCup;
		defendCup = msg.defendCup;

		time = msg.time;

		isAlreadyAttack = msg.isAlreadyAttack;
		clientver = msg.clientver;
	}
};

struct WarParam {
	int				numsWall;
	int				numsTownhall;
	int				numsWorker;
	int				numsMortar;
};

enum FightCtrlType {
    CTRL_TYPE_BUILDING = 1,  //对方建筑
    CTRL_TYPE_DROP_SOLDIER = 2, //放兵
    CTRL_TYPE_KILL_ATTACK_SOLDIER = 3, //攻击士兵死亡
    CTRL_TYPE_KILL_DEFEND_SOLDIER = 4, //防守士兵死亡
    CTRL_TYPE_DESTROY_BUILDING = 5, //建筑被摧毁
    CTRL_TYPE_RESOURCES = 6, //资源容量
    CTRL_TYPE_HEROINFO = 7, //英雄防守信息
    CTRL_TYPE_HEROPOS = 8,  //英雄位置信息
    CTRL_TYPE_USESKILL = 9, //使用技能
    CTRL_TYPE_CHANGE_ATTACK_TARGET = 10, //改变攻击目标
    CTRL_TYPE_CHANGE_DEFEND_TARGET = 11, //改变防守目标
    CTRL_TYPE_USE_CLAN_SOLDIER = 12,
    CTRL_TYPE_CLAN_SOLDIER_ATTACK = 13,
    CTRL_TYPE_CLAN_SOLDIER_DEFEND = 14, //建筑出兵
    
    
    CTRL_TYPE_FIGHT_END = 99
};

struct FightBuilding { //FightBuilding 必须和服务端解析结构一致
    int id;
    int bid;
    int state;
    int x;
    int y;
    int hp;
    int maxhp;
    int store[2];
};

struct FightCtrlNode {
    unsigned char   type; //byte
    int             cid;
	int				ctrl;
	int				x;
	int				y;
    int             state;
};

struct FightLog {
	int					numsBuilding;
	int					numsCtrl;
    int                 ctrlIndex;
	int					nodenum;
	FightCtrlNode*      node;
    FightBuilding*      building;
    
	FightLog()
	{
        numsCtrl = 0;
		nodenum = 1024;
		node = new FightCtrlNode[nodenum];
        building = NULL;
		clear();
	}

	~FightLog() 
	{
        delete [] building;
		delete [] node;
	}

	void clear() { memset(node, 0, sizeof(FightCtrlNode) * nodenum); }

	int size() { return sizeof(int) * 4 + sizeof(FightCtrlNode) * (ctrlIndex + numsCtrl); }

	void expand(int index)
	{
		if(index < nodenum)
			return ;

		nodenum += 1024;

		FightCtrlNode* pn = new FightCtrlNode[nodenum];
		memset(pn, 0, sizeof(FightCtrlNode) * nodenum);
		memcpy(pn, node, sizeof(FightCtrlNode) * (nodenum - 1024));
		delete [] node;
		node = pn;
	}

	//! 合并成一片内存，用完之后需要delete
	unsigned char* get()
	{
		unsigned char* ng = new unsigned char[size()];

		memcpy(ng, this, sizeof(int) * 4);
		memcpy(ng + sizeof(int) * 4, node, sizeof(FightCtrlNode) * (ctrlIndex + numsCtrl));

		return ng;
	}

//	//! 设置数据
//	bool set(unsigned char* data, int len) //temp
//	{
//		if((len - sizeof(int) * 3) % sizeof(FightCtrlNode) != 0)
//			return false;
//
//		int onum = nodenum;
//
//		memcpy(this, data, sizeof(int) * 3);
//
//		nodenum = onum;
//        ctrlIndex = numsBuilding;
//
//		if(nodenum < numsBuilding + numsCtrl)
//		{
//			delete [] node;
//
//			while(nodenum < numsBuilding + numsCtrl)
//			{
//				nodenum += 1024;
//			}
//
//			node = new FightCtrlNode[nodenum];
//			memset(node, 0, sizeof(FightCtrlNode) * nodenum);
//		}
//
//		memcpy(node, data + sizeof(int) * 3, sizeof(FightCtrlNode) * (numsBuilding + numsCtrl));
//		return true;
//	}
    
    bool setMapData(unsigned char* mapdata, int len) {
        
        if(len % sizeof(FightBuilding) != 0)
            return false;
        
        numsBuilding = len / sizeof(FightBuilding);
        
        delete [] building;
        building = new FightBuilding[numsBuilding];
        
        memcpy(building, mapdata, sizeof(FightBuilding)*numsBuilding);
        return true;
    }
    
    bool setFightData(unsigned char* data, int len)
    {
        if((len - sizeof(int) * 4) % sizeof(FightCtrlNode) != 0)
            return false;
        
        int onum = nodenum;
        
        memcpy(this, data, sizeof(int) * 4);
        
        nodenum = onum;
        
        if(nodenum < ctrlIndex + numsCtrl)
        {
            delete [] node;
            
            while(nodenum < ctrlIndex + numsCtrl)
            {
                nodenum += 1024;
            }
            
            node = new FightCtrlNode[nodenum];
            memset(node, 0, sizeof(FightCtrlNode) * nodenum);
        }
        
        memcpy(node, data + sizeof(int) * 4, sizeof(FightCtrlNode) * (ctrlIndex + numsCtrl));
        return true;
    }
};

struct Townhall_BuildingNums {
	int				level;

	int				attackCost;

	int				troopHousing;
	int				elixirStorage;
	int				goldStorage;
	int				elixirPump;
	int				goldMine;
	int				barrack;
	int				cannon;
	int				wall;
	int				archerTower;
	int				wizardTower;
	int				airDefense;
	int				mortar;

	int				workerBuilding;
	int				laboratory;

    int             heroAltar;
    
	int				bomb;
	int				flybomb;
	int				spring;
	int				bigbomb;
    
    int             obuilding[7];
    
    int             letter[40];
};

struct Achievement {
	int				goldstorage;
	int				pvestar;
	int				townhall;
	int				otherbuilding;
	int				barrack;
	int				wargold;
	int				warelixir;
	int				cup;
	int				alliancecastle;
	int				warwall;
	int				wartownhall;
	int				warworker;
	int				warwin;
	int				defwin;
	int				givesoldier;
	int				warmortar;
	int				wardarkelixir;

	int				goldstorage_star;
	int				pvestar_star;
	int				townhall_star;
	int				otherbuilding_star;
	int				barrack_star;
	int				wargold_star;
	int				warelixir_star;
	int				cup_star;
	int				alliancecastle_star;
	int				warwall_star;
	int				wartownhall_star;
	int				warworker_star;
	int				warwin_star;
	int				defwin_star;
	int				givesoldier_star;
	int				warmortar_star;
	int				wardarkelixir_star;
};

struct AchievementNode {
	int				id;
	int				level;
	std::string		name;
	std::string		info;
	int				exp;
	int				gem;
	int				val;
};

struct RankUserInfo {
	int				uid;
	std::string		name;
	int				curlevel;
	int				curcup;
    int             lastcup;
	int				currank;
	int				lastrank;
    
    int             military;
    
    int             clanid;
    int             clanflag;
    string          clanname;
    int             clantype;
    int             clancup;
    int             clanneedcup;
};

struct MailInfo {
    int mid;
    int uid1;
    int uid2;
    std::string uname2;
    std::string title;
    std::string message;
    int state;
    int gold;
    int elixir;
    int gem;
    time_t time;
};

struct InPurchaseProductInfo {
	int				productID;
	int				gem;
    float           price;
	std::string		strPlatformProductID;
	std::string		strPrice;
};

struct HeroInfo2 {
    int             uid;
    int             heroid;
    int             hp;
    int             curexp;
    int             equ0;
    int             equ1;
    int             equ2;
    int             store;
    int             skill0;
    int             skill1;
    int             skill2;
    int             skill3;
	int             curhptime;
    int             curtime;
    int             luck;
    int             maxhp;
    int             maxexp;
    int             bskill[12];
    int             bskillexp[12];
};

struct SkillInfo {
    int             skillid;
    std::string     name;
    std::string     info;
    int             needlevel;      //! 解锁需要英雄等级

	int				per;
	int				per_begin;

	std::string		att_name;
	int				att_layer;
	int				att_type;			//! 0播放一次 1循环播放 2三方向
	std::string		eff_name;
	int				eff_layer;
	int				eff_type;

	int				cd;

	int				bvalue1;			//! 属性初始值
	int				bvalue2;
	int				btime1;				//! Buff时间
	int				delay;


	int				maxlevel;			//! 最大等级

	int				trainExp;
	int				maxExp;
	int				trainGem;
};

const float	TRAPBOMB_TIME	=	0.5f;	//! 陷阱出现到爆炸的时间
const float	PERSONFLIP_TIME	=	1.0f;	//! 角色被弹走的时间

//! Buff相关，Buff明确相应功能，由技能进行减增
const int	BUFF_DAMAGE		=	0;		//! 增加（减少）伤害值
const int	BUFF_DAMAGE_P	=	1;		//! 增加（减少）伤害万分比
const int	BUFF_SPEED		=	2;		//! 增加（减少）速度值,正值更快
const int	BUFF_SPEED_P	=	3;		//! 增加（减少）速度万分比,正值更快
const int	BUFF_ATTSPEED	=	4;		//! 增加（减少）攻击速度值，正值更快
const int	BUFF_ATTSPEED_P	=	5;		//! 增加（减少）攻击速度万分比，正值更快
const int	BUFF_DEFENCE	=	6;		//! 增加（减少）防御值
const int	BUFF_DEFENCE_P	=	7;		//! 增加（减少）防御万分比
const int	BUFF_AGAINST	=	8;		//! 抵御多少点伤害
const int	BUFF_JUMP		=	9;		//! 具有跳墙能力
const int	BUFF_LIFE		=	10;		//! 每秒增加（减少）血量
const int	BUFF_COMA		=	11;		//! 昏迷
const int	BUFF_ATTACKTYPE	=	12;		//! 攻击方式 >0表示有Buff 方式为value-1
const int	BUFF_RANGE		=	13;		//! 增加（减少）射程
const int	BUFF_MAXLIFE	=	14;		//! 增加（减少）生命上限
const int	BUFF_MAXLIFE_P	=	15;		//! 增加（减少）生命万分比
const int	BUFF_HIDDEN		=	16;		//! 隐身
const int	BUFF_SUPER		=	17;		//! 几率无敌
const int	BUFF_CRIT		=	18;		//! 几率暴击

const int	BUFF_MAXNUM		=	19;		//! Buff种类

struct BuffNode {
	int			value;		//! 数值
	int			time;		//! 持续时间（毫秒）

	BuffNode() : value(0), time(0) {}
};

struct TalentNode {
	int			value;		//! 数值
	int			prob;		//! 几率

	TalentNode() : value(0), prob(0) {}
};

struct HeroInfo {
	int			heroid;			// heroid
	int			curhp;			// 当前HP，不可超过maxhp
	int			maxhp;
	int			lasttime;		// 最后一次更新hp时间，暂定每分钟回复10点，应该改成填表的

	int			curhpoff;		// 这次战斗以前，该值为0，结束后，这个值需要同步服务器
};

struct SoldierUpgradeInfo {
    
	int			offtime;			// 服务器的时间差，
	int			curtime;			// 同步时的时间，currime - offtime 是初始时间
	int			nums;

	int			offtimems;	// 本地本地偏移时间，ms单位

    int         soldierid[10];
	int			lasttime[10];		// 这个时间是客户端时间
    
	bool		ischg;

	SoldierUpgradeInfo() : nums(0) {}
    
    void reset() { memset(this, 0, sizeof(SoldierUpgradeInfo)); }
};

struct ClanMember {
    int             uid;			//成员id
    std::string     name;			//成员姓名
    int             clanid;			//联盟id
    int             clanjob;		//联盟职位 0 普通 1 长老 2 会长
    int             sendsoldiers;   //这周送兵量
    int             getsoldiers;    //这周得到兵量
    int             cupoff;         //这周杯数变化
    std::string     jointime;       //加入联盟时间
    int             castlelevel;    //城堡等级
    int             level;
    int             curcup;
    
    int             currank;
    int             lastrank;
    
    int             military;
    
    int             troopsdonated;
    int             troopsreceived;
    
    bool            isonline;
    bool            warinvalid;
};

struct ClanInfo {
    int             id;				//联盟ID
    std::string     name;			//联盟名字
    int             national;       //国籍
    std::string     description;	//描述
    int             jointype;		// 0、任何人都可加入 1、需要邀请加入 2、不准加入
    int             needcupnums;    //加入条件，需要的杯数
    int             leaderid;		//盟主ID
    int             cupnums;		//联盟总杯数
    int             level;			//联盟等级
    int             membercurnums;	//联盟现有成员数
    int             membermaxnums;	//联盟最大成员数
    int             flag;           //旗子
    
    vector<ClanMember> memberList;
    
    ClanInfo()
    : id (0)
    {
    }
    
    ClanInfo(const ClanInfo& right)
    {
        id = right.id;
        name = right.name;
        national = right.national;
        description = right.description;
        jointype = right.jointype;
        needcupnums = right.needcupnums;
        leaderid = right.leaderid;
        cupnums = right.cupnums;
        level = right.level;
        membercurnums = right.membercurnums;
        membermaxnums = right.membermaxnums;
        flag = right.flag;
        
        memberList = right.memberList;
    }
    
    void encodeInfo(Json::Value val)
    {
        id = getAsInt(val["clanid"]);
        national = getAsInt(val["national"]);
        cupnums = getAsInt(val["cup"]);
        membercurnums = getAsInt(val["curnums"]);
        membermaxnums = getAsInt(val["maxnums"]);
        jointype = getAsInt(val["clantype"]);
        flag = getAsInt(val["clanflag"]);
        needcupnums = getAsInt(val["mintrophies"]);
        leaderid = getAsInt(val["clanleader"]);
        name = val["clanname"].asString();
        description = val["description"].asString();
    }

};

struct ClanBaseInfo {
    int clanid;
    int national;
    string clanname;
    int clanflag;
    int jointype;
    int clancup;
    int clanneedcup;
    
    void encodeInfo(Json::Value val)
    {
        clanid = getAsInt(val["clanid"]);
        national = getAsInt(val["national"]);
        clanname = val["clanname"].asString();
        clanflag = getAsInt(val["clanflag"]);
        clancup = getAsInt(val["clancup"]);
        jointype = getAsInt(val["clantype"]);
        clanneedcup = getAsInt(val["mintrophies"]);
    }
    
    void operator = (const ClanInfo& right)
    {
        clanid = right.id;
        clanname = right.name;
        national = right.national;
        jointype = right.jointype;
        clanneedcup = right.needcupnums;
        clancup = right.cupnums;
        clanflag = right.flag;
    }
};

enum ClanMemberJob {
    CLAN_JOB_MEMBER = 0,
    CLAN_JOB_ELDER = 1,
    CLAN_JOB_COLEADER = 2,
    CLAN_JOB_LEADER = 3
};

enum ChatType { //聊天类型
    CHAT_TYPE_NORMAL = 0,
    CHAT_TYPE_JOIN_CLAN = 1,
    CHAT_TYPE_HELP = 2,
    CHAT_TYPE_VIDEO = 3,
    CHAT_TYPE_INVATE = 4
};

struct ChatData {
    int cid;  //chat id
	int type; //聊天类型 0普通聊天 1请求加入工会 2请求援助 3视频分享
    int uid;
    string uname;
    int military;
    int memberType;
    string message;
    time_t time;
    
    ClanBaseInfo clanInfo;
    
    int status;  //0-未处理 1-已处理
    string statusMessage;
    
    Json::Value battleLog;
        
    bool isShow;
};

struct ClanSoldierInfo { //请求援兵
    int uid;  //赠送者id
    int sid;  //兵种id
};

struct ClanTroopsData {
    int cid; //chat id
    int uid;
    int maxspace;
    int curspace;
    int donateNums; //自己捐兵个数
    
    std::vector<ClanSoldierInfo> lstSoldier;
};

enum WarCityType {
    CLANWAR_CITY = 1,
    GROUPWAR_CITY = 2
};

struct ClanCityInfo {
    
    int countryid;
    
    int cityid;
    string cityname;
    
    int citytype; //1-Clan City 2-NPC City
    
    int citysite;
    int posx;
    int posy;
    
    int cursignnums;
    int maxsignnums;
    
    int dayrewardid;
    int productgems;
    int wingems;
    
    ClanInfo clanInfo;
};

struct ClanWarAttackClanInfo : ClanInfo {
    
    int rankingid;
    
    int winstar;
    int laststartime;
};

struct ClanWarUserInfo : UserInfo {
    int destroyStar;
    
    int levelupStar;
    int defenseLevel;
    float defenseRatio;
};

struct ClanWarAttackInfo {
        
    int canAttackNums;
    
    int maxAttackNums;
    int curAttackNums;
    
    int winStar;
};

struct GroupWarFightLog {
    
    ClanMember user;
    
    int winstar;
    int battlenums;

    int laststartime;
};

struct PVEInfo { // PVE info
    int curstage;
    int curlife;
    int maxlife;
    int lifetime_off;
    time_t restime;
    time_t last_restime;
    time_t lifetime;
    time_t last_lifetime;
};

struct EnvStageInfo {
    int stage;
    int star;
    int gold;
    int oil;
    
    int basegold;
    int baseoil;
    
    string name;
};

struct IapHighlightInfo {
    int totalGem;
    int Highlight;//1表示该内购项需要更突出显示 0无
    int totalDays;
};

struct IapGiftInfo {
    int corflag;                    // 0 可以领取礼包  1 不可领取 2 已领取,处于每天签收礼包状态
    int gotflag;                    //corflag = 2 时  标记每天是否签收过礼包
    time_t last_time;
    int corclaim;                   //给的奖励
    std::map <int,IapHighlightInfo> corIapMap;        //大礼包产品的list
    int totalDays;                   //活动总天数
    int gotDays;                    //领取时的天数
    int stopDays;                   //剩余天数
    int buyId;                      //参加礼包的productID
    IapGiftInfo() : corflag(1), totalDays(7) {};
    
    void timeEnd(){
        corflag = 1;
        corIapMap.clear();
    }
    
    const IapHighlightInfo* getIapGift(int productID)
    {
        std::map <int,IapHighlightInfo>::const_iterator it = corIapMap.find(productID);
        if(it != corIapMap.end())
        {
            return &(it->second);
        }
        return NULL;
    }
    
    bool iapDoneForGift(int productID){

        const IapHighlightInfo *info = getIapGift(productID);
        if(corflag == 0 && info != NULL)
        {
            buyId = productID;
            corflag = 2;
            gotDays = 1;
            stopDays = 6;
            totalDays = info->totalDays;
            corclaim = info->totalGem/totalDays;
            
            return true;
        }
        return false;
    }
    
//    void setCorIap(const std::string &iapStr,const string &pattern) //按pattern分割字符
//    {
//        std::string::size_type pos;
//        int size=iapStr.size();
//        corIap.clear();
//        for(int i=0; i<size; i++)
//        {
//            pos=iapStr.find(pattern,i);
//            if(pos<size)
//            {
//                std::string s=iapStr.substr(i,pos-i);
//                corIap.push_back(atoi(s.c_str()));
//                i=pos+pattern.size()-1;
//            }
//        }
//    }
};

struct WallLine {
	int			bx, by;
	int			ex, ey;

	int getLength() {
		if(bx == ex)
			return (ey - by) / 2;
		else
			return (ex - bx) / 2;
	}

	bool isIn(int lx, int ly) {
		if(bx == ex)
		{
			if(lx == bx)
			{
				if(ly >= by && ly < ey)
					return true;
			}

			return false;
		}
		else
		{
			if(ly == by)
			{
				if(lx >= bx && lx < ex)
					return true;
			}

			return false;
		}
	}

	bool isNear(const WallLine& wl) {
		if(bx == ex)
		{
			if(wl.bx == wl.ex)
			{
				if(bx == wl.bx + 1 || bx == wl.bx - 1)
					return true;
			}

			return false;
		}
		else
		{
			if(wl.by == wl.ey)
			{
				if(by == wl.by + 1 || by == wl.by - 1)
					return true;
			}

			return false;
		}
	}

	bool isCross(const WallLine& wl) {
		if(bx == ex)
		{
			if(wl.by == wl.ey)
			{
				if(bx == wl.bx || by == wl.by)
					return true;
			}

			return false;
		}
		else
		{
			if(wl.bx == wl.ex)
			{
				if(bx == wl.bx || by == wl.by)
					return true;
			}

			return false;
		}
	}
};


enum FBSharePlaceType {
    FBShare_Base = 0,
    FBShare_Fight = 1,
    FBShare_Military = 2,
    FBShare_League = 3,
    FBShare_Clanwar = 4,
};

enum AddGemType {
    ADD_GEM_NORMAL = 0,
    ADD_GEM_FLURRLY = 1,
    ADD_GEM_TAPJOY = 2,
    ADD_GEM_CHARTBOOST = 3,
    ADD_GEM_AARKI = 4,
    ADD_GEM_SSA = 5,
    ADD_GEM_NATIVE_X = 6,
    ADD_GEM_FB_LOGIN = 7,
    ADD_GEM_FB_INVITE = 8,
    ADD_GEM_FB_SHARE = 9,
};
enum AnnouncementType {
    ANNOUNCEMENT_TYPE_NO = -1,//无公告
    ANNOUNCEMENT_TYPE_CHOOSE = 0,//非强制性公告
    ANNOUNCEMENT_TYPE_FORCE = 1,//强制性公告
};

enum BindType {
    BIND_SYSTEM_TYPE = 0,//系统账号绑定
    BIND_FACEBOOK_TYPE = 1,//facebook
};


// z position
const int	SCENE_Z_TILE				=	1;		// tile
const int	SCENE_Z_TILEAREA			=	2;		// tile area
const int	SCENE_Z_SHADOW				=	3;		//
const int	SCENE_Z_ARROW				=	4;		// arrow
const int	SCENE_Z_HUAN				=	5;		// attack zone
const int	SCENE_Z_DESTROY				=	6;		//
const int	SCENE_Z_BUILDING_BEGIN		=	10;
const int	SCENE_Z_BUILDING_JUMP		=	4;		
const int	SCENE_Z_TILECOLOR			=	5000;	// tilecolor
const int	SCENE_Z_FLYINGSHADOW		=	5100;	// flying shadow
const int	SCENE_Z_CURBUILDING			=	5100;	// cur building
const int	SCENE_Z_PARTICLE			=	5201;	// particle
const int	SCENE_Z_FLYING				=	5300;	// flying
const int	SCENE_Z_SCREENMASK			=	10200;	// screen mask
const int	SCENE_Z_BUILDINGUI			=	10201;	// ui

const int	SCENE_Z_SKILLEFF_GROUND		=	4;
const int	SCENE_Z_SKILLEFF_BUILDING	=	10;
const int	SCENE_Z_SKILLEFF_TOP		=	10200;

std::string UrlEncode(const char* szToEncode);
std::string UrlDecode(const char* str);

float countRadian(float bx, float by, float ex, float ey);

const char* getServiceKey();

bool isUserMask(int mask, int m);

NS_COC_END

#endif // __COC_BASEDEF_H__
