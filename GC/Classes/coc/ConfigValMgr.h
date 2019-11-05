#ifndef __COC_CONFIGVALMGR_H__
#define __COC_CONFIGVALMGR_H__

#include "BaseDef.h"

NS_COC_BEGIN

enum
{
    BONUS_ID_RATE_TYPE = 0, //评论
    BONUS_ID_FIRST_PURCHASE_TYPE = 1,//首次充值
    BONUS_ID_ADS_TYPE = 2,//广告
    BONUS_ID_GIFT_TYPE = 3,//礼包
    BONUS_FB_LOGIN = 4,//facebook登录提示
    BONUS_FB_INVITE = 5,//facebook邀请提示
    BOUNS_GP_COMMUNITY_TYPE = 6,//google+社区
};

struct SigninAwardInfo{
	std::string		type;
	int				val;
};

struct SpinItemInfo {
    std::string type;
    float val;
};

struct BonusItemInfo {
    int bonusId;
    std::string type;
    int val;
    int nums;
};

class ConfigValMgr
{
private:
    ConfigValMgr();
    ~ConfigValMgr();
public:
	static ConfigValMgr& getSingleton();
public:
	const char* getConfigVal_String(const char* str);

	int getIntValForKey(const char* str);
	float getFloatValForKey(const char* str);
    std::string getStringValForKey(const char* str);
    int getArrayNumsForKey(const char* str);
    vector<BonusItemInfo> filterBonusInfo(const char* str);
    
    Json::Value getObjectForKey(const char* str);
public:
    
    void load(const char* name);
    
	void reloadFromMem(const char* date, unsigned long size, bool isNeedDecode);
    
    const SigninAwardInfo getSigninAward(int id);
    const SpinItemInfo getSpinItemInfo(int id);
    const BonusItemInfo getBonusItemInfo(int id);
    
    
    
public:
	Json::Value		m_jsonData;
    char*			m_pBuff;
    vector<BonusItemInfo> m_bonusList;
    
    bool m_isRemote;
};

NS_COC_END

#endif // __COC_CONFIGVALMGR_H__
