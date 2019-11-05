//
//  CCAppPlatform.mm
//  RobinHood
//
//  Created by qsun on 13-1-28.
//
//

#import "CAppPlatform.h"
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <GameKit/GameKit.h>
#import <GameKit/GKLocalPlayer.h>
#import "AppController.h"
#import "RootViewController.h"
#import "AppPlatform_ios.h"
#import "Appirater.h"
#import "CUserData.h"
#import "EAGLView.h"
#import "InPurchaseIOS.h"

//#import "Flurry.h"
//#import "FlurryAds.h"
//#import "CFlurryAdDelegate.h"
#import <Tapjoy/Tapjoy.h>
#import <Chartboost/Chartboost.h>
#import "AarkiContact.h"
#import "IMAdRequest.h"
#import "IMAdView.h"
#import "MdotMAdView.h"
#import "MdotMRequestParameters.h"
#import "MdotMAdSizes.h"
#import "ChartboostDelegate.h"
#import <FacebookSDK/FacebookSDK.h>
#import <MobileAppTracker/MobileAppTracker.h>
#import <AdSupport/ASIdentifierManager.h>
#import "AarkiContact.h"
#import "AarkiUserBalance.h"
#import "Aarki.h"
#import "CWebVIewDelegate.h"
#import "NativeXSDK.h"
#import "CNativeXAdDelegate.h"

#import "SupersonicAdsPublisher.h"

#include "PlayerScene.h"
#include "../../../form/Shop.h"
#include "../../../GameWorldScene.h"

#include "CFacebookService.h"


static CAppPlatform* s_pSharedAppPlatform = NULL;
CAppPlatform* CAppPlatform::sharedAppPlatform()
{
    if (s_pSharedAppPlatform == NULL) {
        s_pSharedAppPlatform = new CAppPlatform();
        
    }
    return s_pSharedAppPlatform;
}

CNativeXAdDelegate *nativeXDelegate = NULL;
UIWebView *web = NULL;
CWebVIewDelegate *webDelegate = NULL;
SupersonicAdsPublisher *ssaPublisher;
Aarki *aarkiLoader;

//Chartboost *cb;
//CFlurryAdDelegate *flurryAdDelegate = NULL;
ChartboostDelegate *chartboostDelegate = NULL;


void CAppPlatform::initPlatform()
{
    // NOTE: This is the only step required if you're an advertiser.
	// NOTE: This must be replaced by your App ID. It is retrieved from the Tapjoy website, in your account.
    
    nativeXDelegate = [[CNativeXAdDelegate alloc] init];
    [[NativeXSDK sharedInstance] setDelegate:(id)nativeXDelegate];
    NSString *nativeXKey = [NSString stringWithUTF8String:NATIVE_X_KEY];
    if(APP_DEBUG)
        nativeXKey = @"18920";
    [[NativeXSDK sharedInstance] createSessionWithAppId:nativeXKey];
    
    webDelegate = [[CWebVIewDelegate alloc] init];
    
    ssaPublisher = [SupersonicAdsPublisher sharedSupersonicAds];
    
    [ssaPublisher initBrandConnectWithApplicationKey:[NSString stringWithUTF8String:SSA_APP_KEY] userId:[NSString stringWithUTF8String:DEVICE_ID] delegate:[AppPlatform_ios sharedAppPlatform] additionalParameters:nil];

    aarkiLoader = [[Aarki alloc] init];
    aarkiLoader.delegate = [AppPlatform_ios sharedAppPlatform];
    
    [[NSNotificationCenter defaultCenter] addObserver:[AppPlatform_ios sharedAppPlatform]
                                             selector:@selector(tapjoyConnectSuccess:)
                                                 name:TJC_CONNECT_SUCCESS
                                               object:nil];    
    [[NSNotificationCenter defaultCenter] addObserver:[AppPlatform_ios sharedAppPlatform]
                                             selector:@selector(tapjoyViewClosed:)
                                                 name:TJC_VIEW_CLOSED_NOTIFICATION
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:[AppPlatform_ios sharedAppPlatform]
											 selector:@selector(showDirectPlayVideoAd:)
												 name:TJC_FULL_SCREEN_AD_RESPONSE_NOTIFICATION
											   object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:[AppPlatform_ios sharedAppPlatform]
											 selector:@selector(showDirectPlayVideoAdError:)
												 name:TJC_FULL_SCREEN_AD_RESPONSE_NOTIFICATION_ERROR
											   object:nil];
    
//    flurryAdDelegate = [[CFlurryAdDelegate alloc] init];
//    UIViewController *viewController = (UIViewController*)[AppController sharedAppController].viewController;
//    
//    [Flurry startSession:[NSString stringWithUTF8String:FLURRY_API_KEY]];
//    
//    [FlurryAds initialize:viewController];
//    
//    // Register yourself as a delegate for ad callbacks
//    [FlurryAds setAdDelegate:flurryAdDelegate];
//    [FlurryAds fetchAdForSpace:[NSString stringWithUTF8String:FLURRY_AD_SPACE] frame:[AppController sharedAppController].viewController.view.frame size:FULLSCREEN];
    
    //init Chartboost
//    cb = [Chartboost sharedChartboost];
//    chartboostDelegate = [[ChartboostDelegate alloc] init];
//    cb.appId = [NSString stringWithUTF8String:CHARTBOOST_APP_ID];
//    cb.appSignature = [NSString stringWithUTF8String:CHARTBOOST_APP_SIGNATURE];
//    
//    [cb startSession];
//    [cb cacheInterstitial];
//    [cb setDelegate:chartboostDelegate];
    
    chartboostDelegate = [[ChartboostDelegate alloc] init];
    [Chartboost startWithAppId:[NSString stringWithUTF8String:CHARTBOOST_APP_ID] appSignature:[NSString stringWithUTF8String:CHARTBOOST_APP_SIGNATURE] delegate:chartboostDelegate];
    [Chartboost cacheInterstitial:CBLocationHomeScreen];
//    [Chartboost cacheRewardedVideo:CBLocationMainMenu];
//    [Chartboost cacheMoreApps:CBLocationHomeScreen];
    
    //init Aarki
//    [AarkiContact registerApp:[NSString stringWithUTF8String:AARKI_APP_ID] withClientSecurityKey:[NSString stringWithUTF8String:AARKI_SECRET_KEY]];
    
    //init InMobi
    IMAdView *imAdView = [[[IMAdView alloc] initWithFrame:CGRectMake(0, 480 - 50, 320, 50)
                                                  imAppId:[NSString stringWithUTF8String:INMOBI_APP_ID]
                                                 imAdSize:IM_UNIT_320x50] autorelease];
    IMAdRequest *request = [IMAdRequest request];
    [imAdView loadIMAdRequest:request];
    imAdView.refreshInterval = 60;
    
    //MdotM
    MdotMAdView *adview = [[[MdotMAdView alloc] initWithFrame:CGRectMake (0,0, 320, 50)] autorelease];
    MdotMRequestParameters *requestParameters;
    requestParameters = [[[MdotMRequestParameters alloc] init] autorelease];
    requestParameters.appKey =[NSString stringWithUTF8String:MDOTM_APP_KEY];
    requestParameters.test = @"0";
    
    [adview loadBannerAd:requestParameters withSize:BANNER_320_50];
    
    [MobileAppTracker initializeWithMATAdvertiserId:[NSString stringWithUTF8String:MOBILE_APP_ID]
                                   MATConversionKey:[NSString stringWithUTF8String:MOBILE_APP_KEY]];
    
    [MobileAppTracker setAppleAdvertisingIdentifier:[[ASIdentifierManager sharedManager] advertisingIdentifier]
                         advertisingTrackingEnabled:[[ASIdentifierManager sharedManager] isAdvertisingTrackingEnabled]];
    
    NSString *siteId = [NSString stringWithUTF8String:getBundleId().c_str()];
    [MobileAppTracker setSiteId:siteId];
    
    // For existing users prior to MAT SDK implementation, call setExistingUser:YES before measureSession.
    // Otherwise, existing users will be counted as new installs the first time they run your app.
    BOOL isExistingUser = YES;
    if (isExistingUser) {
        [MobileAppTracker setExistingUser:YES];
    }
    
}

void CAppPlatform::showNativeXOfferAds(const char* placementStr)
{
    
    if(strcmp(placementStr, NATIVE_X_GAMELAUNCH) != 0)
    {
        if(PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pMainUI)
        {
            PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->showAdsLoadingUI();
        }
    }
//    [[NativeXSDK sharedInstance] fetchAdWithCustomPlacement:[NSString stringWithUTF8String:placementStr] delegate:nativeXDelegate];
    if([[NativeXSDK sharedInstance] isAdReadyWithCustomPlacement:[NSString stringWithUTF8String:placementStr]])
    {
        [[NativeXSDK sharedInstance] showReadyAdWithCustomPlacement:[NSString stringWithUTF8String:placementStr]];
    }
    else
    {
        [[NativeXSDK sharedInstance] fetchAdWithCustomPlacement:[NSString stringWithUTF8String:placementStr] delegate:nativeXDelegate];
    }
}

void CAppPlatform::getNativeXPoint()
{
    [[NativeXSDK sharedInstance] redeemCurrency];
}
 
void CAppPlatform::getScreenShot()
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    CCRenderTexture *screen = CCRenderTexture::create(size.width, size.height);
    CCScene *scene = CCDirector::sharedDirector()->getRunningScene();
    screen->begin();
    scene->visit();//将当前的整个scene绘出来
    screen->end();
    screen->saveToFile(ScreenShotFile, kCCImageFormatPNG);
}

void CAppPlatform::showAnnouncement()
{
    if(web == NULL)
    {
        AnnouncementUI* pAnnouncementUI = PlayerScene::getSingleton().m_pGameWorld->m_pAnnouncementUI;
        
        CCNode *webNode = pAnnouncementUI->m_pWebNode;
        UIViewController *viewController = (UIViewController*)[AppController sharedAppController].viewController;
        CGSize viewSize = viewController.view.bounds.size;
        CCDirector *dir = CCDirector::sharedDirector();
        CCSize dirSize = dir->getWinSize();
        float sca = MAX(viewSize.width/dirSize.width, viewSize.height/dirSize.height);
        
        CCSize size = webNode->getContentSize()*sca;
        float xPos = webNode->getPositionX()*sca;
        float yPos = (dir->getWinSize().height - webNode->getPositionY())*sca;
        [[NSURLCache sharedURLCache] removeAllCachedResponses];
        web = [[[UIWebView alloc]initWithFrame:CGRectMake(0, 0, size.width, size.height)] autorelease];
        NSURL *url=[NSURL URLWithString:[NSString stringWithUTF8String:LogicInfo::getSingleton().m_webURL.c_str()]];
        [web setBackgroundColor:[UIColor clearColor]];
        if([[UIDevice currentDevice].systemVersion floatValue]>=7.0)
        {
            [web setTintColor:[UIColor clearColor]];
        }
        [web setCenter:CGPointMake(xPos,yPos)];
        [web setDelegate:webDelegate];
        web.opaque = NO;
        
        NSURLRequest *request=[[[NSURLRequest alloc] initWithURL:url] autorelease];
        [web loadRequest:request];
        [web setUserInteractionEnabled:YES];
        
        [viewController.view addSubview:web];
    }
}

//state 1 加载成功 0失败
void CAppPlatform::webViewDidFinishLoad(int state)
{
    
    if(PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pAnnouncementUI == NULL)
        return;
    
    AnnouncementUI* pAnnouncementUI = PlayerScene::getSingleton().m_pGameWorld->m_pAnnouncementUI;
    
    if(pAnnouncementUI == NULL)
        return;
    if(state == 1)
    {
        CCNode *pLoading = pAnnouncementUI->m_pLoading;
        if(pLoading && pLoading->isVisible())
        {
            pLoading->setVisible(false);
        }
    }
    else if(state == 0)
    {
        //失败
        pAnnouncementUI->menuBackCallback(NULL, CCControlStateNormal);
        LogicInfo::getSingleton().onNotify(NOTIFY_NETWORK_FAIL);
    }
}

void CAppPlatform::hideAnnouncement()
{
    if(web)
    {
        web.delegate = nil;
        [web removeFromSuperview];
         web = nil;
    }
}

void CAppPlatform::getSSAPoint()
{
    HttpClient::getSingleton().requestSupersonicadsData();
}

void CAppPlatform::showSSAOfferAds()
{
    [ssaPublisher showOfferWallWithApplicationKey:[NSString stringWithUTF8String:SSA_APP_KEY] userId:[NSString stringWithUTF8String:DEVICE_ID] delegate:[AppPlatform_ios sharedAppPlatform] shouldGetLocation:YES extraParameters:nil parentViewController:[AppController sharedAppController].viewController];
}

void CAppPlatform::displayFlurryAds(string pid)
{
//    NSMutableDictionary *userCookies = [NSMutableDictionary dictionary];
//    [userCookies setObject:[NSString stringWithUTF8String:APP_GAME_NAME] forKey:@"game"];
//    [userCookies setObject:[NSString stringWithUTF8String:APP_ID] forKey:@"appId"];
//    [userCookies setObject:[NSString stringWithUTF8String:pid.c_str()] forKey:@"pid"];
//    [userCookies setObject:[NSString stringWithUTF8String:DEVICE_ID] forKey:@"user"];
//    
//    [FlurryAds setUserCookies:userCookies];
//    
//    UIViewController *viewController = (UIViewController*)[AppController sharedAppController].viewController;
//    
//    // Fetch and display banner ads
//    [FlurryAds fetchAdForSpace:@"BANNER_MAIN_VC" frame:viewController.view.frame size:FULLSCREEN];
//    [FlurryAds displayAdForSpace:@"BANNER_MAIN_VC" onView:viewController.view];
}

void CAppPlatform::logFlurryEvent(const char *eventKey, cocos2d::CCDictionary *pDict)
{
//    if (pDict == NULL || pDict->allKeys() == NULL) {
//        return;
//    }
//    
//    NSMutableDictionary *pParams = [NSMutableDictionary dictionaryWithCapacity:pDict->allKeys()->count()];
//    
//    for (int i = 0; i < pDict->allKeys()->count(); i++) {
//        
//        cocos2d::CCObject* obj = pDict->objectForKey(((cocos2d::CCString *)pDict->allKeys()->objectAtIndex(i))->getCString());
//        NSString* pObject = @"";
//        if (isKindOfClass(obj, cocos2d::CCString))
//        {
//            const char* cstring = ((cocos2d::CCString*)obj)->getCString();
//            pObject = [[[NSString alloc] initWithBytes:cstring length:strlen(cstring) encoding:NSUTF8StringEncoding] autorelease];
//        }
//        else if (isKindOfClass(obj, cocos2d::CCInteger))
//        {
//            pObject = [NSString stringWithFormat:@"%d", ((cocos2d::CCInteger*)obj)->getValue()];
//        }
//        
//        NSString* pKey = [NSString stringWithUTF8String:((CCString*)pDict->allKeys()->objectAtIndex(i))->getCString()];
//        
//        [pParams setValue:pObject forKey:pKey];
//    }
//    
//    [Flurry logEvent:[NSString stringWithUTF8String:eventKey] withParameters:pParams];
}

void CAppPlatform::logMobileAppEvent(const char* purchaseID, float price)
{
    [MobileAppTracker measureAction:@"purchase"
                         eventItems:[NSArray arrayWithObject:[MATEventItem eventItemWithName:[NSString stringWithUTF8String:purchaseID] unitPrice:price quantity:1]]
                        referenceId:[NSString stringWithUTF8String:MOBILE_APP_ID]
                      revenueAmount:price*0.7f
                       currencyCode:nil];
}

void CAppPlatform::requestTapjoyConnect()
{
    [Tapjoy requestTapjoyConnect:[NSString stringWithUTF8String:TAPJOY_APP_ID]
					   secretKey:[NSString stringWithUTF8String:TAPJOY_SECRET_KEY]
						 options:@{ TJC_OPTION_ENABLE_LOGGING : @(NO) }
     // If you are not using Tapjoy Managed currency, you would set your own user ID here.
     //TJC_OPTON_USER_ID : @"A_UNIQUE_USER_ID"
     ];
}

void CAppPlatform::showAarkiOfferAds()
{
     [aarkiLoader showAds:[NSString stringWithUTF8String:AARKI_APP_ID] withParent:[AppController sharedAppController].viewController options:nil];
}

void CAppPlatform::getAarkiPoint()
{
    [AarkiUserBalance check:[NSString stringWithUTF8String:AARKI_APP_ID] completion:^(UserBalanceStatus status, NSNumber *balance) {
        if (status == UserBalanceStatusOK) {
            int amount = [balance intValue];
            if(amount > 0)
            {
                [AarkiUserBalance add:[NSString stringWithUTF8String:AARKI_APP_ID] amount:-amount completion:^(UserBalanceStatus status, NSNumber *balance) {
                if (status == UserBalanceStatusOK) {
                    LogicInfo::getSingleton().m_addGemType = ADD_GEM_AARKI;
                    LogicInfo::getSingleton().requestAdsGem(amount);
                }
            
                }];
            }
            
        }
    }];
}

void CAppPlatform::showTapJoyOfferswall()
{
    [Tapjoy showOffersWithViewController:[AppController sharedAppController].viewController];
}

void CAppPlatform::showTapjoyVideoAds()
{
    [Tapjoy getFullScreenAd];
}

void CAppPlatform::showFlurryVideoAds()
{
//    int random = arc4random() % 100;
//    if(random < 50)
//    {
//        showTapjoyVideoAds();
//        return;
//    }
//    if ([FlurryAds adReadyForSpace:[NSString stringWithUTF8String:FLURRY_AD_SPACE]])
//    {
//        [FlurryAds displayAdForSpace:[NSString stringWithUTF8String:FLURRY_AD_SPACE] onView:[AppController sharedAppController].viewController.view];
//    } else {
//        // Fetch an ad
//        [FlurryAds fetchAdForSpace:[NSString stringWithUTF8String:FLURRY_AD_SPACE] frame:[AppController sharedAppController].viewController.view.frame size:FULLSCREEN];
//    }
}

void CAppPlatform::showChartBoostAds()
{
    [Chartboost showInterstitial:CBLocationHomeScreen];
}

void CAppPlatform::getChartBoostClickGems()
{
    if (LogicInfo::getSingleton().m_isChartboostFreeGemReady && PlayerScene::getSingleton().m_pGameWorld->m_pShop)
    {
        if(PlayerScene::getSingleton().m_pGameWorld->m_pShop->m_shopId == SHOP_TYPE_FREEGEMS)
        {
            LogicInfo::getSingleton().requestChartboostClickGems();
            LogicInfo::getSingleton().m_isChartboostFreeGemReady  = false;
            LogicInfo::getSingleton().m_hasChartboostFreeGem      = true;
            PlayerScene::getSingleton().m_pGameWorld->m_pShop->refreshShopItem(3);
        }
    }
}

void CAppPlatform::getTapJopPoint()
{
    [Tapjoy getTapPointsWithCompletion:^(NSDictionary *parameters, NSError *error)
     {
         if (!error)
         {
             CCLOG("amount %d",[parameters[@"amount"] intValue]);
             if ([parameters[@"amount"] intValue]>0)
             {
                 
                 int value = [parameters[@"amount"] intValue];
                 [Tapjoy spendTapPoints:value completion:^(NSDictionary *parameters, NSError *error)
                  {
                      if (!error)
                      {
                          LogicInfo::getSingleton().m_addGemType = ADD_GEM_TAPJOY;
                          LogicInfo::getSingleton().requestAdsGem(value);
                      }
                  }];
             }
         }
     }];
}

void CAppPlatform::showDirectPlayVideoAd()
{
    NSLog(@"showDirectPlayVideoAd");
    [Tapjoy showFullScreenAd];
    
}

void CAppPlatform::showDirectPlayVideoAdError()
{
//    NSLog(@"showDirectPlayVideoAdError");
//    //tapjoy获取视频广告失败 调用flurry的广告
//    if ([FlurryAds adReadyForSpace:[NSString stringWithUTF8String:FLURRY_AD_SPACE]])
//    {
//        [FlurryAds displayAdForSpace:[NSString stringWithUTF8String:FLURRY_AD_SPACE] onView:[AppController sharedAppController].viewController.view];
//    } else {
//        // Fetch an ad
//        [FlurryAds fetchAdForSpace:[NSString stringWithUTF8String:FLURRY_AD_SPACE] frame:[AppController sharedAppController].viewController.view.frame size:FULLSCREEN];
//    }
    
}

void CAppPlatform::setInputMode(int mode)
{
    EAGLView* view = [EAGLView sharedEGLView];
    [view setKeyboardType:(UIKeyboardType)mode];
}

int CAppPlatform::getInputMode(int mode)
{
    int inputMode = mode;
    //    switch (mode) {
    //        case kKeyboardTypeDefault:
    //            break;
    //        default:
    //            break;
    //    }
    return inputMode;
}

void CAppPlatform::purchasePayment(string productId, int payType)
{
    [[InPurchaseIOS getInstance] makePayment:[NSString stringWithUTF8String:productId.c_str()]];
}

void CAppPlatform::consumePurchase(string productId)
{
    [InPurchaseIOS getInstance];
}

void CAppPlatform::purchasesResume()
{
    //purchases resume
    [[InPurchaseIOS getInstance] resumePurchase];
}

bool gameCenterAvailable()
{
    // Check for presence of GKLocalPlayer API.
    Class gcClass = (NSClassFromString(@"GKLocalPlayer"));
    // The device must be running running iOS 4.1 or later.
    NSString *reqSysVer = @"4.1";
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
    BOOL osVersionSupported = ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending);
    
    return (gcClass && osVersionSupported);
}

void CAppPlatform::bindDeviceUserId()
{
    if(CFacebookService::sharedFaceBook()->fbBindActivity) {
        return;
    }
    if (gameCenterAvailable()) {
        
        if ([GKLocalPlayer localPlayer].authenticated == NO)
        {
            [[GKLocalPlayer localPlayer] authenticateWithCompletionHandler:^(NSError *error) {
                if (error == nil){
                    NSLog(@"成功");
                    NSLog(@"1--alias--.%@",[GKLocalPlayer localPlayer].alias);
                    NSLog(@"2--authenticated--.%d",[GKLocalPlayer localPlayer].authenticated);
                    NSLog(@"3--isFriend--.%d",[GKLocalPlayer localPlayer].isFriend);
                    NSLog(@"4--playerID--.%@",[GKLocalPlayer localPlayer].playerID);
                    NSLog(@"5--underage--.%d",[GKLocalPlayer localPlayer].underage);
                    
                }else{
                    NSLog(@"失败 %@",error);
                }
                
                if([GKLocalPlayer localPlayer].authenticated){
                    string playerId = [[GKLocalPlayer localPlayer].playerID UTF8String];
                    string userName = [[GKLocalPlayer localPlayer].alias UTF8String];
                    LogicInfo::getSingleton().m_bindType = BIND_SYSTEM_TYPE;
                    verifyLocalPlayer(playerId, userName);
                }
            }];
        }
        else
        {
            if([GKLocalPlayer localPlayer].authenticated){
                string playerId = [[GKLocalPlayer localPlayer].playerID UTF8String];
                string userName = [[GKLocalPlayer localPlayer].alias UTF8String];
                LogicInfo::getSingleton().m_bindType = BIND_SYSTEM_TYPE;
                verifyLocalPlayer(playerId, userName);
            }
        }
	}else {
		NSLog(@"Game Center Available");
	}
}

string CAppPlatform::getBundleId()
{
    NSDictionary *info = [[NSBundle mainBundle] infoDictionary];    //获取info－plist
    NSString *bundleId = [info objectForKey:@"CFBundleIdentifier"];    //获取Bundle identifier
    
    return [bundleId UTF8String];
}

string CAppPlatform::getBundleVersion()
{
    NSDictionary *info = [[NSBundle mainBundle] infoDictionary];    //获取info－plist
    NSString *bundleVersion = [info objectForKey:@"CFBundleVersion"];    //获取Bundle identifier
    return [bundleVersion UTF8String];
}

string CAppPlatform::getAppName()
{
    NSDictionary *info = [[NSBundle mainBundle] infoDictionary];    //获取info－plist
    NSString *bundleName = [info objectForKey:@"CFBundleName"];    //获取Bundle identifier
    return [bundleName UTF8String];
}

void CAppPlatform::fireNotification(int delay, string message)
{
    Class cls = NSClassFromString(@"UILocalNotification");
    if (cls == nil) {
        return;
    }
    
    //save last time
    NSTimeInterval curTime = [[NSDate date] timeIntervalSince1970];
    [[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithDouble:curTime] forKey:@"lastTime"];
    NSTimeInterval sysupTime = [[NSProcessInfo processInfo] systemUptime];
    [[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithDouble:sysupTime] forKey:@"preSysupTime"];
    
    UILocalNotification *localNotif = [[UILocalNotification alloc] init];
    localNotif.repeatInterval = 0;
    localNotif.fireDate = [NSDate dateWithTimeIntervalSinceNow:delay];
    localNotif.alertBody = [NSString stringWithUTF8String:message.c_str()];
    
    localNotif.timeZone = [NSTimeZone defaultTimeZone];
    localNotif.soundName = UILocalNotificationDefaultSoundName;
    localNotif.applicationIconBadgeNumber = 1;
    [[UIApplication sharedApplication] scheduleLocalNotification:localNotif];
    [localNotif release];
}

void CAppPlatform::cancelAllNotifications()
{
    [[UIApplication sharedApplication] cancelAllLocalNotifications];
    [[UIApplication sharedApplication] setApplicationIconBadgeNumber:0];
}

void CAppPlatform::updateNewVersion(string message, string newUrl)
{
    NSString *title = @"Update is available!";
    
    if (message.length() <= 0) {
        message = "Good news! A new version of the game is available.";
    }
    
    if (newUrl.length() > 0)
    {
        [AppPlatform_ios sharedAppPlatform].appNewVersionURL = [NSString stringWithUTF8String:newUrl.c_str()];
    }
    else
    {
        [AppPlatform_ios sharedAppPlatform].appNewVersionURL = [NSString stringWithUTF8String:APP_STORE_URL];
    }
    
    [AppPlatform_ios sharedAppPlatform].appNewVersionAlert = [[UIAlertView alloc] initWithTitle:title message:[NSString stringWithUTF8String:message.c_str()] delegate:[AppPlatform_ios sharedAppPlatform] cancelButtonTitle:@"Update" otherButtonTitles:nil];
    
    [[AppPlatform_ios sharedAppPlatform].appNewVersionAlert show];
    [[AppPlatform_ios sharedAppPlatform].appNewVersionAlert release];
}

void CAppPlatform::rateApp()
{
    int rateCount = CCUserDefault::sharedUserDefault()->getIntegerForKey(RATER_ALERT_COUNT);
    
    if (rateCount < RATER_ALERT_MAX_COUNT) {
        [Appirater appEnteredForeground:TRUE];
        
        CCUserDefault::sharedUserDefault()->setIntegerForKey(RATER_ALERT_COUNT, rateCount+1);
        CCUserDefault::sharedUserDefault()->flush();
    }
}
void CAppPlatform::getMoregame()
{
    if (PLATFORM_TARGET == IPad)
    {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://itunes.apple.com/us/app/aaa-casino-slots-hd/id713448649?ls=1&mt=8"]];
    }
    else if(PLATFORM_TARGET == IPhone)
    {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://itunes.apple.com/us/app/aaa-casino-slots/id713448635?ls=1&mt=8"]];
    }
}
void CAppPlatform::reportScore(int score, string category)
{
	NSLog(@"category %@", [NSString stringWithUTF8String:category.c_str()]);
    GKScore *scoreReporter = [[[GKScore alloc] initWithCategory:[NSString stringWithUTF8String:category.c_str()]] autorelease];
    scoreReporter.value = score;
    [scoreReporter reportScoreWithCompletionHandler:^(NSError *error) {
		if (error != nil){
            NSLog(@"error %@",error);
        }
    }];
}

void CAppPlatform::reportAchievementIdentifier(string identifier, float percent)
{
	if(percent == 0){
		return;
	}
    GKAchievement *achievement = [[[GKAchievement alloc] initWithIdentifier:[NSString stringWithUTF8String:identifier.c_str()]] autorelease];
    if (achievement){
		achievement.percentComplete = percent;
		[achievement reportAchievementWithCompletionHandler:^(NSError *error){
            if (error != nil) {
                NSLog(@"Error %@",error);
            }else {
                //NSLog(@"reported achievement %f",percent);
            }
        }];
    }
}

void CAppPlatform::showGameCenterLeaderboard(string category)
{
    GKLeaderboardViewController *leaderboardController = [[GKLeaderboardViewController alloc] init];
    if (leaderboardController != nil){
        leaderboardController.leaderboardDelegate = [AppController sharedAppController].viewController;
        leaderboardController.category = [NSString stringWithUTF8String:category.c_str()];
        
        [[AppController sharedAppController].viewController presentModalViewController: leaderboardController animated: YES];
        
        [leaderboardController release];
    }
}

void CAppPlatform::openUrl(const char *url)
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]]];
}

void CAppPlatform::openGooglePlusLink()
{
    CCUserDefault::sharedUserDefault()->setBoolForKey(GP_FREE_GEMS, true);
    CCUserDefault::sharedUserDefault()->flush();
    
    openUrl("https://plus.google.com/communities/102982482049792636737");
}

void CAppPlatform::exit()
{
    
}

CAppPlatform::~CAppPlatform()
{
//    [flurryAdDelegate release];
    [chartboostDelegate release];
}