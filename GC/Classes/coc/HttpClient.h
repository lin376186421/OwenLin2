#ifndef __COC_HTTPCLIENT_H__
#define __COC_HTTPCLIENT_H__

#include <algorithm>
#include "BaseDef.h"
#include "Building_Wall.h"
#include "NetListener.h"
#include "../zephyr/Lock.h"
#include <list>
#include <queue>

#include "cocos-ext.h"

class HelloWorld;

NS_COC_BEGIN

class HttpClient : public CCObject
{
public:
	struct _RequestParamNode{
		std::string		key;
		std::string		val;

		bool operator < (const _RequestParamNode& right) {
			return key < right.key;
		}

		bool operator == (const char* right) const {
			return key == right;
		}
	};

	struct _finder_key{
		_finder_key(const char* key) : m_key(key) {}

		bool operator() (_RequestParamNode& node) { return (m_key == node.key); }

		std::string m_key;
	};

	struct _RequestParam{
		std::list<_RequestParamNode>		lst;
        int bytes;
        
        _RequestParam() : bytes(0)
        {
            
        }

		void addParam(const char* key, const char* val) {
			_RequestParamNode n;

			n.key = key;
			n.val = val;
            
            bytes += sizeof(char) * strlen(val);

			lst.push_back(n);
		}

		void addParam(const char* key, int val) {
			_RequestParamNode n;

			n.key = key;
			
			char str[128];
			sprintf(str, "%d", val);

			n.val = str;
            
            bytes += sizeof(int);

			lst.push_back(n);
		}

		int getParamAsInt(const char* key) {
			std::list<_RequestParamNode>::iterator it = std::find_if(lst.begin(), lst.end(), _finder_key(key));
            
			if(it != lst.end())
			{
				return atoi(it->val.c_str());
			}

			return 0;
		}
	};

	struct _RequestNode{
        
		CCHttpRequest*	pRequest;
        
		std::string		strRequest;

		std::string		strParam;

		_RequestParam	param;

		std::string		fullurl;
		std::string		fullparam;

		bool			bCanFail;			// 是否允许失败

		int				failNums;			// 失败次数

		bool			bPost;				// 是否要post
		bool			bSave;				// 是否要保存
		bool			bNeedCommonParam;	// 是否需要基本参数
        bool            bAsync;             // 是否异步
        
        int             requestCount;
	};
public:
	static HttpClient& getSingleton();
protected:
	HttpClient();
	~HttpClient();
public:
	void onHttpRequestCompleted(cocos2d::CCNode *sender, CCHttpResponse *response);
    void onHttpRequestAdsCompleted(cocos2d::CCNode *sender,CCHttpResponse *response);
	//virtual int process(void* data, size_t size, size_t nmemb);

	void addListener(NetListener* pListener);

	void removeListener(NetListener* pListener);

	void procCtrl(Json::Value& val);

	void onIdle();

	void onHelloWorldRelease();
    
public:
	void doHttpRequestEx(const char* url, _RequestParam& param, bool bCanFail, bool isSave, bool isNeedCommonParam, bool bAsync = false, bool isupload = false);

	void doHttpRequestPostEx(const char* url, _RequestParam& param, bool bCanFail, bool isSave, bool isNeedCommonParam, bool bAsync = false, bool isupload = false);

	void makeCommonParam2(std::string& sparam, _RequestParam& param, bool isNeedBaseParam, bool bAsync, bool isupload);

	void pushRequestEx(CCHttpRequest* pRequest, const char* strURL, _RequestParam& param, bool bCanFail, bool isPost, bool isSave, bool isNeedCommonParam, const char* fullurl, const char* fullparam, bool bAsync = false);

	bool isNeedSaveParam(std::string& key);
    
    void sendRequest(CCHttpRequest* pRequest);
        
	void onRequestCompleted(CCHttpResponse* pResponse);
    
    void nextRequest(CCHttpRequest* pRequest);
    
	void makeCommonPostParam(std::string& param);
    
	void makeCommonGetURL(std::string& fullurl, const char* url, const char* param);
    
	void makeCommonPostParamEx(std::string& param);
    
	void makeCommonGetURLEx(std::string& fullurl, const char* url, const char* param);
    
public:
    
    void procMsgNums(int msgNums);
    
    void lsenRequest();
    
	void reg();
    
	void login();
    
    void loginok();
    
	void build(int id, int buildingid, int lx, int ly, int state);
    
	void move(int id, int bx, int by, int ex, int ey);
    
	void keeplive(bool bCanFail);
    
	void updateBuilding(int id, int building, int x, int y, int level, int lasttime);
    
	void harvest(int id, int x, int y, int restime);
    
	void levelupBuilding(int id, int buildingid, int x, int y);
    
	void fastBuilding(int id, int buildingid, int x, int y, int level, int lasttime);
    
	void removeBuilding(int id, int buildingid, int x, int y);
    
	void findUser();
    
	void updateBarrackInfo(BarrackInfo& info);
    
	void updateTroophousingInfo(TroophousingInfo& info);
    
	void subMoney(int m0, int m1, int m2);
    
	void getGold(int gold);
    
	void getOil(int oil);
    
	void addBattleLog(int destuid, int star, int offlevel, FightScoreInfoEx3& fsi, FightLog& nlog, WarParam& param, int usegems = 0);
    
	void addLeagueBattleLog(int destuid, int leagueid, int groupid,int star, int offlevel, FightScoreInfoEx3& fsi, FightLog& nlog, WarParam& warparam, int usegems = 0);
    
    void addEnvBattleLog(int stage, int star, FightScoreInfoEx3& fsi);
    
    void addClanWarBattleLog(int destuid, int star, int offlevel, FightScoreInfoEx3& fsi, FightLog& nlog, WarParam& warparam, int usegems = 0);
    
    void addGroupWarBattleLog(int destuid, int star, int offlevel, FightScoreInfoEx3& fsi, FightLog& nlog, WarParam& warparam, int usegems = 0);
    
    void endBattle(int destuid);
    
    void endClanWarBattle(int destuid);
    
    void addClanUseSoldier(_RequestParam &param);
    
    void addBattleMap(_RequestParam &param);
    
	void requestDefenseLog();
    
    void requestAttackLog(int logid);
    
    void requestBattleLogDetail(int logid);
    
	void upddateUserName(const char* name);
    
	void addMoney(int m0, int m1, int m2);
    
	void cancelLevelupBuilding(int buildingid, int x, int y);
    
	void requestAchievement();
    
	void chgExp(int leveloff, int exp);
    
	void requestRank();
    
    void requestRevenge(int uid, int msgid);
    
    void attackUserEx(int uid);
    
	void revengeUserEx(int uid, int msgid);
    
    void viewUserEx(int uid);
    
	void refurbishHero();
    
	void updateHeroHP(int off);
    
	void fastHeroHP(int offtime);
    
    void selectHero(int uid);
    
    void selectSkill(int skillid, int pos);
    
    void unselectSkill(int pos);
    
	void updateUpgradeSoldier(SoldierUpgradeInfo& sui);
    
	void requireAchievementAward(const char* str);
    
	void updUserKeys(const char* key0, const char* key1, const char* key2);
    
	void pay(int productid, const char* orderid);
    
	void pay2(int productid, bool result, const char* receiptdata);
    
	void pay3(int productid, bool result, const char* receiptdata);

	void pay4(int productid, bool result, const char* receiptdata);
    
	void loginWithDeviceID();
    
    void updateUserMask2(int mask);
    
	void checkVer();
    
	void payProtectTime(int day);
    
	void updUserGameTime(int timeoff);
    
	void updUserInfo();
        
	void requestGradeGem();
    
	void requestFirstPayGem();
    
	void updPushDeviceID(const char* deviceid);
    
	void updRegistrationID(const char* deviceid);
    
	void updDeviceToken(const char* deviceid);
    
	void loginWithUserPlatform(int pid, const char* puid, const char* puname);
    
	void updGuideVal(int val);
        
	void chgSkill3(int skillid);
    
	void checkVer3();
    
    void checkCSV(string key[6]);
    
	void checkUserCenterBind(const char* uidUserCenter);
    
	void rebindGameCenter(const char* uidUserCenter, int uid);
    
	void upgradeSkill(int skillindex, int type);
    
	void chgHeroInBuilding(int bx, int by);
    
    void getUserState(int uid);
    
	void requestMailInfo();
    
	void getMailAward(int mailid);
    
    void requestChgGemOnly(int value);
    
	void save();

	bool load();

	void onProcLoad();
    
	void createClan(const char* clanname, const char* symbol, const char* description, int clantype, int mintrophies, int flag, int national);
    
	void editClan(ClanInfo clanInfo);
    
    void requestClanData(int aid);
    
    void searchClan(const char* word);
    
    void getClanRanking();
    
	void outputLog(const char* str);

	void addBug(const char* str, const char* email);
    
    void signin();
    
    void requestAdFreeGemFlag(const char *str);
    
    void requestFBInviteFriends(const vector<string> friendIds);
    
    void requestFBAvailableFriends();
    void requestFBShareLog();
    
    void requestChartboostClickGems();
    
    void requestSysNotify();
    
    void requestEnvMap();
    
    void requestAttackEnv(int stage);
    
    void addAttackEnvVideo(int stage, int destuid, int star, int offlevel, FightScoreInfoEx3& fsi, FightLog& nlog, WarParam& warparam, int usegems);
    
    void requestAttackEnvVideo(int stage);
    
    void addPVELife();
    
    void addPVELife(int life, int gems);
    
    void getIapClaim(int bid);
    
    void requestSupersonicadsData();
    
    void requestLeagueInfo();
    
    void findLeagueAttactUser();
    
    void getLeagueAward(int leagueid);
    
	void clearRequestList();
    
    void updateMilitaryRank();
    
    void requestSupersonicadsGems(const char* data);
    
    void requestPaypalPurchase();
    
    void requestGetGem(int mask);
    
    void requestMsgNums();
    
    void requestClanWarInfo();
    
    void clanWarSignup(int cityid);
    
    void getClanWarSignList(int cityid);
    
    void getClanWarBattleInfo(int cityid, int atkclanid);
    
    void attackClanWar(int uid);
    
    void getClanWarBattleLog(int cityid, int clanid);
    
    void requestClanWarBattleLog(int logid);
    
    void getClanWarRewardGems(int rewardid);
    
    void checkClanWarAttack(int uid);
    
    void attackGroupWar(int uid);
    
    void getGroupWarBattleInfo(int cityid, int atkclanid);
    
    void getGroupWarBattleLog(int cityid, int clanid);
    
    void getGroupWarRewardGems(int rewardid, int starrewardid);
    
    void saveEdit(list<UserBuildingInfo*> lstBuilding);
    
    void upgradeWalls(vector<Building_Wall*> lstUpgradeWall, int m0, int m1, int m2);
    
    void upLevelHero();
    
    void finishUpLevelHero(int gem);
    
    void cancelUpLevelHero();
    
public:
	std::list<NetListener*>		m_lstListener;
    
	std::queue<std::string>		m_msgCache;

	bool						m_isProcListener;

	std::string					m_urlroot;
    
	std::list<_RequestNode*>    m_queueRequest;
	LockImp						m_lockRequest;
	CCHttpRequest*				m_pCurRequest;

	LockImp						m_lockRecv;

	HelloWorld*					m_pHelloWorld;

	int							m_curClientState;

	int							m_numsRequestFail;
	time_t						m_totalRequestTime;
};

NS_COC_END

#endif  // __COC_HTTPCLIENT_H__
