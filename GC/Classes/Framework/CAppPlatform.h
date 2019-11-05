//
//  CCAppPlatform.h
//  RobinHood
//
//  Created by qsun on 13-1-28.
//
//

#ifndef __CCGame__CAppPlatform__
#define __CCGame__CAppPlatform__

#include <iostream>
#include "AppConfig.h"
#include "cocos2d.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif
#include "CUserData.h"
//#include "MessageUI.h"

#include "../coc/LogicInfo.h"
#include "../coc/HttpClient.h"
#include "../form/SupportUI.h"
#include "../coc/FacebookMgr.h"



using namespace std;
using namespace cocos2d;
USING_NS_COC;





class CAppPlatform {
    
public:
    
    static CAppPlatform* sharedAppPlatform();
    
    void initPlatform();

    void displayFlurryAds(string pid);
    void logFlurryEvent(const char* eventKey, CCDictionary* pDict);
    void logMobileAppEvent(const char* purchaseID, float price);
    
    void requestTapjoyConnect();
    void showTapJoyOfferswall();
    void getTapJopPoint();
    void showDirectPlayVideoAd();
    void showDirectPlayVideoAdError();
    
    void getScreenShot();
    
    void getAarkiPoint();
    void showAarkiOfferAds();
    void showFlurryVideoAds();
    void showTapjoyVideoAds();
    void showChartBoostAds();
    void showSSAOfferAds();
    void getSSAPoint();
    void showNativeXOfferAds(const char* placementStr);
    void getNativeXPoint();
    void showAnnouncement();
    void hideAnnouncement();
    void webViewDidFinishLoad(int state);
    
    void getChartBoostClickGems();
    void getMoregame();
    
    void setInputMode(int mode);
    int getInputMode(int mode);
    
    void bindDeviceUserId();
    void verifyLocalPlayer(string playerId, string userName)
    {        
        CCLOG("%s", playerId.c_str());
		LogicInfo::getSingleton().onGetDeviceUserName(playerId.c_str(), userName.c_str());
    }
    
    string getBundleId();
    
    string getAppName();
    string getBundleVersion();
        
    void purchasePayment(string productId, int payType);
    void consumePurchase(string productId);
    void purchasesResume();

    double getElapsedRealtime();
    bool connectedToNet();
    
    void fireNotification(int delay, string message);
    void cancelAllNotifications();
    void updateNewVersion(string message, string newUrl);
    
    void rateApp();
    void rateAppOption(bool rate)
    {
        if (rate)
        {
            CCUserDefault::sharedUserDefault()->setBoolForKey(RATE_FREE_GEMS, rate);
            CCUserDefault::sharedUserDefault()->flush();
        }
    };
    
    void registerServerPush(string tokenId)
    {
        CCLOG("server push token %s", tokenId.c_str());
		m_pushTokenID = tokenId;

		if(LogicInfo::getSingleton().m_myInfo.uid > 0)
			HttpClient::getSingleton().updPushDeviceID(m_pushTokenID.c_str());
    }
    
    void reportScore(int score, string category);
    void reportAchievementIdentifier(string identifier, float percent);
    void showGameCenterLeaderboard(string category);
    
    void openUrl(const char *url);
    void openGooglePlusLink();
    
    void exit();
    
    ~CAppPlatform();
public:
	std::string		m_pushTokenID;
};

#endif /* defined(__CCGame__CAppPlatform__) */
