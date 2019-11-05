//
//  AppConfig.h
//  RobinHood
//
//  Created by qsun on 13-1-4.
//
//

#ifndef __Global__AppConfig__
#define __Global__AppConfig__

#include "AppDebug.h"
#include "CDevice.h"
#include "NGNKit.h"

enum PayType {
    PAY_TYPE_GOOGLEPLAY = 0,
    PAY_TYPE_PAYPAL = 1,
    PAY_TYPE_AMAZON = 2,
    
    PAY_TYPE_IOS = 3
};

//App Platform
#define	IPhone  1
#define	IPad  2
#define	Android 3

#define APP_GAME_NAME               "ww2"                                          //Game Name

#define PLATFORM_TARGET Android

#if (PLATFORM_TARGET == IPad)       //iPad
#define APP_ID                      "909539860"                                     //App Id
#define APP_STORE_URL               "https://itunes.apple.com/us/app/a-battle-of-glory-hd/id909539860?l=zh&ls=1&mt=8"
#elif (PLATFORM_TARGET == IPhone)   //iPhone
#define APP_ID                      "923930032"                                     //App Id
#define APP_STORE_URL               "https://itunes.apple.com/us/app/amazing-clan-war/id686389428?ls=1&mt=8"
#elif (PLATFORM_TARGET == Android)  //Android
#define APP_ID                      "com.mgstudio.bg"                               //App Id
#define APP_STORE_URL               "market://details?id=com.mgstudio.bg"
#endif

#define DEVICE_ID               CDevice::sharedDevice()->getIDFA().c_str() //"100000000048" 

#if (SERVICE_DEBUG == 0)
#define SERVER_VERSION              "v8"
#define SOCKET_SERVER_URL           "ws://boc.ngnsoft.com:8080"
#elif(SERVICE_DEBUG == 1)
#define SERVER_VERSION              "dev"
#define SOCKET_SERVER_URL           "192.168.0.138:9090"
#elif(SERVICE_DEBUG == 2)
#define SERVER_VERSION              "dv"
#define SOCKET_SERVER_URL           "ws://boc.ngnsoft.com:8080"
#endif

#define APP_PORT                    8080

#define SERVER_URL                  "http://lsenw2.ngnsoft.com/w2serv/lsen/lsen.php"
#define PAYPAL_URL                  "http://ww2.ngnsoft.com/w2serv/%s/paypal/product.php?uid=%d&pid=%d" //paypal
#define WEB_URL                     "http://www.ngnsoft.com/List.html" //webview

// mark - * Ads Config
#if (PLATFORM_TARGET == IPad)       //iPad

#define ADWHIRL_APP_KEY             "413b3cbae08e447dad3727ecbae080f8"                 //adWhirl App Key
#define FLURRY_API_KEY              "T6ZTTCBJ8PHCKK4YYQCR"                             //Flurry Api key
#define FLURRY_AD_SPACE             "VIDEO_ONLY_SPACE"                                 //Flurry Ad Space
#define ADMOB_APP_ID                "a151ff4747997a9"                                  //Admob App Id
#define INMOBI_APP_ID               "9e6647b048d34b24b787fa44d23322ea"
#define TAPJOY_APP_ID               "e44a8158-0a48-4194-a1c9-ed74f6210616"             //Tapjoy App Id
#define TAPJOY_SECRET_KEY           "fVZvw324iFh3wF5m6bCQ"                             //Tapjoy Secret Key
#define AARKI_APP_ID                "43D044698A85C618AA"
#define AARKI_SECRET_KEY            "QgdfU4RkyPtbZ8UDBUdPmglGX0gn"
#define CHARTBOOST_APP_ID           "53ed914d89b0bb29e73b6319"
#define CHARTBOOST_APP_SIGNATURE    "5a5f39e541dc284f433b3a46247cd6dc9690a36b"
#define MDOTM_APP_KEY               "0af880692a048091b6613c2253f7cd86"                 //MdotM App Id
#define MOBILE_APP_ID               "18326"
#define MOBILE_APP_KEY              "cc35d557ac8d081e33dacb528d588fd1"
#define SSA_APP_KEY                 "311b0715"
#define NATIVE_X_KEY                "18920"
#define NATIVE_X_NOT_GEMS           "NotEnoughGems"
#define NATIVE_X_GET_GEMS           "FreeGemsStore"
#define NATIVE_X_GAMELAUNCH         "Game Launch"
#define GETUI_APP_ID                "fwvm7PPEkY5Pug7wGVJSi2"
#define GETUI_APP_KEY               "4pkKo2rMZl50UhckH523T7"
#define GETUI_APP_SECRET            "wXYlP2hAXb78vyiyuj3wh8"

#elif (PLATFORM_TARGET == IPhone)   //iPhone
#define ADWHIRL_APP_KEY             "413b3cbae08e447dad3727ecbae080f8"                 //adWhirl App Key
#define FLURRY_API_KEY              "R5QJVVXCBZ3KBJCYR78Z"                             //Flurry Api key
#define FLURRY_AD_SPACE             "VIDEO_ONLY_SPACE"                             //Flurry Ad Space
#define ADMOB_APP_ID                "a15200a167d7e35"                                  //Admob App Id
#define INMOBI_APP_ID               "9e6647b048d34b24b787fa44d23322ea"
#define TAPJOY_APP_ID               "90e07df1-77db-4dda-a62c-c1cbe339f56e"             //Tapjoy App Id
#define TAPJOY_SECRET_KEY           "nwK22YtbsoyUzKmhUEiI"                             //Tapjoy Secret Key
#define AARKI_APP_ID                "8E17CFCE741D9661AA"
#define AARKI_SECRET_KEY            "2YJqPcOJzSjY2ct9BuDENxEfn4H3"
#define CHARTBOOST_APP_ID           "542924e189b0bb1f7c446f3c"
#define CHARTBOOST_APP_SIGNATURE    "e5a34fff6c9ce966dcbd2e318fd6b61c23e5c6d1"
#define MDOTM_APP_KEY               "9ed4e981d41a4a0bc1fb58570d36818a"                 //MdotM App Id
#define MOBILE_APP_ID               "18326"
#define MOBILE_APP_KEY              "cc35d557ac8d081e33dacb528d588fd1"
#define SSA_APP_KEY                 "31a2175d"
#define NATIVE_X_KEY                "21798"
#define NATIVE_X_NOT_GEMS           "NotEnoughGems"
#define NATIVE_X_GET_GEMS           "FreeGemsStore"
#define NATIVE_X_GAMELAUNCH         "Game Launch"
#define GETUI_APP_ID                "iMahVVxurw6BNr7XSn9EF2"
#define GETUI_APP_KEY               "yIPfqwq6OMAPp6dkqgLpG5"
#define GETUI_APP_SECRET            "G0aBqAD6t79JfzTB6Z5lo5"

#elif (PLATFORM_TARGET == Android)  //CC_PLATFORM_ANDROID
//Android
#define ADWHIRL_APP_KEY             "413b3cbae08e447dad3727ecbae080f8"                 //adWhirl App Key
#define FLURRY_API_KEY              "F7VNVPQCP3JD58BFYVRP"                             //Flurry Api key
#define FLURRY_AD_SPACE             "VIDEO_ONLY_SPACE"                                 //Flurry Ad Space
#define ADMOB_APP_ID                "a151ff4747997a9"                                  //Admob App Id
#define INMOBI_APP_ID               "9e6647b048d34b24b787fa44d23322ea"
#define TAPJOY_APP_ID               "e7515d8b-4df2-4a39-a9c0-355f3b5e28c1"             //Tapjoy App Id
#define TAPJOY_SECRET_KEY           "xidyAtZXOYC0An0cdDhZ"                             //Tapjoy Secret Key
#define AARKI_APP_ID                "9AA79A62C053CCF7AA"
#define AARKI_SECRET_KEY            "IgKDwduFTpKLvBowk5XdEUzxA2do"
#define CHARTBOOST_APP_ID           "53cc98341873da2c896c03db"
#define CHARTBOOST_APP_SIGNATURE    "d00ea13d56e7bc098aa9b099c53ac6494db935e4"
#define MDOTM_APP_KEY               ""                 //MdotM App Id
#define MOBILE_APP_ID               "18326"
#define MOBILE_APP_KEY              "cc35d557ac8d081e33dacb528d588fd1"
#define SSA_APP_KEY                 "300be3a1"
#define NATIVE_X_KEY                "21060"
#define NATIVE_X_NOT_GEMS           "NotEnoughGems"
#define NATIVE_X_GET_GEMS           "FreeGemsStore"
#define NATIVE_X_GAMELAUNCH         "Game Launch"
#define GETUI_APP_ID                "iMahVVxurw6BNr7XSn9EF2"
#define GETUI_APP_KEY               "yIPfqwq6OMAPp6dkqgLpG5"
#define GETUI_APP_SECRET            "G0aBqAD6t79JfzTB6Z5lo5"

#endif

// In Purchase Key
#if (PLATFORM_TARGET == IPad)          //iPad
#define PURCHASE_KEY_1 "gems_01_tire2_ABattleofGloryHD0815"
#define PURCHASE_KEY_2 "gems_02_tire5_ABattleofGloryHD0815"
#define PURCHASE_KEY_3 "gems_03_tire10_ABattleofGloryHD0815"
#define PURCHASE_KEY_4 "gems_04_tire20_ABattleofGloryHD0815"
#define PURCHASE_KEY_5 "gems_05_tire50_ABattleofGloryHD0815"
#define PURCHASE_KEY_6 "gems_06_tire60_ABattleofGloryHD0815"
#elif (PLATFORM_TARGET == IPhone)      //iPhone
#define PURCHASE_KEY_1 "gems_01_tire2_actionworldwar0929"
#define PURCHASE_KEY_2 "gems_02_tire5_actionworldwar0929"
#define PURCHASE_KEY_3 "gems_03_tire10_actionworldwar0929"
#define PURCHASE_KEY_4 "gems_04_tire20_actionworldwar0929"
#define PURCHASE_KEY_5 "gems_05_tire50_actionworldwar0929"
#define PURCHASE_KEY_6 "gems_06_tire60_actionworldwar0929"
#elif (PLATFORM_TARGET == Android)     //Android
#define PURCHASE_KEY_1 "gems_01_tire2_battleglory0721"
#define PURCHASE_KEY_2 "gems_02_tire5_battleglory0721"
#define PURCHASE_KEY_3 "gems_03_tire10_battleglory0721"
#define PURCHASE_KEY_4 "gems_04_tire20_battleglory0721"
#define PURCHASE_KEY_5 "gems_05_tire50_battleglory0721"
#define PURCHASE_KEY_6 "gems_06_tire60_battleglory0721"

#endif

#define GAME_CENTER_TOP_ACHIEVEMENT "top_achievements_ABattleofGloryHD0815"

#define PURCHASE_PRODUCTID_1	1
#define PURCHASE_PRODUCTID_2	2
#define PURCHASE_PRODUCTID_3	3
#define PURCHASE_PRODUCTID_4	4
#define PURCHASE_PRODUCTID_5	5
#define PURCHASE_PRODUCTID_6	6
#define PURCHASE_PRICE_1 "$1.99"
#define PURCHASE_PRICE_2 "$3.99"
#define PURCHASE_PRICE_3 "$7.99"
#define PURCHASE_PRICE_4 "$19.99"
#define PURCHASE_PRICE_5 "$39.99"
#define PURCHASE_PRICE_6 "$99.99"
#define PURCHASE_GEM_1	200
#define PURCHASE_GEM_2	480
#define PURCHASE_GEM_3	1000
#define PURCHASE_GEM_4	2600
#define PURCHASE_GEM_5	6500
#define PURCHASE_GEM_6	14000

#endif /* defined(__Global__AppConfig__) */
