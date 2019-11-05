#include <algorithm>
#include "ConfigValMgr.h"
#include "PlayerScene.h"
#include "LogicInfo.h"
#include "BaseCSV.h"
#include "../Utils/DataDecrypt.h"
#include "../coc/HttpClient.h"
#include "md5.h"
#include "FacebookMgr.h"

NS_COC_BEGIN

ConfigValMgr::ConfigValMgr()
{
	m_isRemote = true;

    load("configval.cfg");
}

ConfigValMgr::~ConfigValMgr()
{
}

ConfigValMgr& ConfigValMgr::getSingleton()
{
	static ConfigValMgr s_mgr;
    
	return s_mgr;
}

void ConfigValMgr::load(const char* name)
{
	std::string fullPath = name;//CCFileUtils::sharedFileUtils()->fullPathFromRelativeFile(name);
	unsigned long size;
	char* pBuff = NULL;
	bool isNeedDecode = false;
    
	if(m_isRemote)
	{
		fullPath = CCFileUtils::sharedFileUtils()->getWritablePath() + name;
		FILE* fp = fopen(fullPath.c_str(), "rb");
		if(fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			size = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			pBuff = new char[size];
			fread(pBuff, size, 1, fp);
			fclose(fp);
		}
	}
	else
	{
		pBuff = (char*)CCFileUtils::sharedFileUtils()->getFileData(fullPath.c_str(), "rb", &size);
	}
    
	if(pBuff != NULL)
	{
        if (DATA_ENCRYPT && string(name).find(".cfg") != -1) {
            isNeedDecode = true;
		}
        
		reloadFromMem(pBuff, size, isNeedDecode);
		delete[] pBuff;
	}
}

void ConfigValMgr::reloadFromMem(const char* pBuff1, unsigned long size, bool isNeedDecode)
{
	const char* pBuff = pBuff1;
    
	if(isNeedDecode)
	{
		unsigned long outSize = 0;
		pBuff = (char*)decryptData(pBuff1, size, &outSize);
		size = outSize;
	}
	
	{
		m_pBuff = (char*)malloc(size + 1);
		memcpy(m_pBuff, pBuff, size);
	       
        Json::Reader reader;
        reader.parse(string(m_pBuff), m_jsonData);
        
		if(m_pBuff != NULL)
		{
			free(m_pBuff);
			m_pBuff = NULL;
		}
	}
    
	if(isNeedDecode)
		delete[] pBuff;
}

int ConfigValMgr::getIntValForKey(const char* str)
{
    int val = m_jsonData[str].asInt();
	return val;
}

int ConfigValMgr::getArrayNumsForKey(const char *str)
{
    int nums = m_jsonData[str].size();
    return nums;
}

//过滤需要屏蔽的bonus
vector<BonusItemInfo> ConfigValMgr::filterBonusInfo(const char *str)
{
    Json::Value val;
    val = m_jsonData[str];
    m_bonusList.clear();
    for(int i = 0; i<val.size(); i++)
    {
        BonusItemInfo info;
        info.bonusId = val[i]["bonusId"].asInt();
        info.type = val[i]["type"].asString();
        info.val = val[i]["val"].asInt();
        info.nums = val[i]["nums"].asInt();
        if(info.bonusId == BONUS_ID_RATE_TYPE)
        {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
            m_bonusList.push_back(info);//warning...
#endif
        }
        else if (info.bonusId == BONUS_ID_FIRST_PURCHASE_TYPE)
        {
        }
        else if (info.bonusId == BONUS_ID_ADS_TYPE)
        {
            if(LogicInfo::getSingleton().m_isShowFreeGemInBonus)
            {
                if(LogicInfo::getSingleton().m_isChartboostAdOn | LogicInfo::getSingleton().m_isFlurryAdOn | LogicInfo::getSingleton().m_isTapjoyAdOn | LogicInfo::getSingleton().m_isAarkiOn | LogicInfo::getSingleton().m_isSupersonicAdsOn | LogicInfo::getSingleton().m_isNativeXAdOn)
                    m_bonusList.push_back(info);
            }
        }
        else if (info.bonusId == BONUS_ID_GIFT_TYPE)
        {
            if(LogicInfo::getSingleton().m_iapGiftInfo.corflag == 0)
            {
                m_bonusList.push_back(info);
            }
        }
        else if (info.bonusId == BONUS_FB_INVITE)
        {
            if(FacebookMgr::getSingleton().isLogin() == true && FacebookMgr::getSingleton().isInvite())
            {
                m_bonusList.push_back(info);
            }
        }
        else if (info.bonusId == BONUS_FB_LOGIN)
        {
            if(FacebookMgr::getSingleton().isLogin() == false && FacebookMgr::getSingleton().isOpenState() && CCUserDefault::sharedUserDefault()->getBoolForKey("FB_LOGIN") == false)
            {
                m_bonusList.push_back(info);
            }
        }
        else if(info.bonusId == BOUNS_GP_COMMUNITY_TYPE)
        {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
            m_bonusList.push_back(info);//warning...
#endif
        }
    }
    
    return m_bonusList;
}


float ConfigValMgr::getFloatValForKey(const char* str)
{
    float val = m_jsonData[str].asDouble();
    return val;
}

std::string ConfigValMgr::getStringValForKey(const char* str)
{
    return m_jsonData[str].asString();
}

Json::Value ConfigValMgr::getObjectForKey(const char *str)
{
    Json::Value val = m_jsonData[str];
    return val;
}

const SigninAwardInfo ConfigValMgr::getSigninAward(int id)
{
    Json::Value val = ConfigValMgr::getSingleton().getObjectForKey("signin");
    
	SigninAwardInfo info;
    info.type = val[id]["type"].asString();
    info.val = getAsInt(val[id]["val"]);
    
	return info;
}

const SpinItemInfo ConfigValMgr::getSpinItemInfo(int id)
{
    Json::Value val = ConfigValMgr::getSingleton().getObjectForKey("spin");

    SpinItemInfo info;
    info.type = val[id]["type"].asString();
    info.val = val[id]["val"].asDouble();
    
	return info;
}

const BonusItemInfo ConfigValMgr::getBonusItemInfo(int id)
{
    BonusItemInfo info;
    for (vector<BonusItemInfo>::iterator it = m_bonusList.begin(); it != m_bonusList.end(); it ++)
    {
        info= *it;
        if(info.bonusId == id)
            return info;
    }
    
    return info;
}

NS_COC_END
