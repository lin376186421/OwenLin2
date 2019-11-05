#include "HelloWorldScene.h"
#include "GameWorldScene.h"
#include "coc/ImgMgr.h"
#include "coc/HttpClient.h"
#include "coc/LogicInfo.h"
#include "form/LogoUI.h"
#include "form/LoadingUI.h"
#include "coc/ViewString.h"
#include "coc/AniObjMgr.h"
#include "coc/PlayerScene.h"
#include "coc/SoundMgr.h"
#include <signal.h>
#include "CAppPlatform.h"
#include "coc/CSVMgr.h"
#include "coc/ResConfig.h"
#include "coc/SocketClient.h"
#include "coc/HeroMgr.h"
#include "InPurchase.h"

using namespace cocos2d;

HelloWorld::HelloWorld()
: m_pLogoUI(NULL)
, m_pLoadingUI(NULL)
, m_loadingProgress(0)
{
}

HelloWorld::~HelloWorld()
{
    LogicInfo::getSingleton().removeListener(this);
}

CCScene* HelloWorld::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::create();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        HelloWorld *layer = HelloWorld::create();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    bool bRet = false;
    do 
    {
        //////////////////////////////////////////////////////////////////////////
        // super init first
        //////////////////////////////////////////////////////////////////////////

        CC_BREAK_IF(! CCLayer::init());
        
        HttpClient::getSingleton().addListener(this);
        LogicInfo::getSingleton().addListener(this);
        
        LogicInfo::getSingleton().m_loadingProgress = 0;
        
        LogicInfo::getSingleton().m_strDebugLog = "";
        LogicInfo::getSingleton().m_strLoadingInfo = "Loading...";
        HttpClient::getSingleton().lsenRequest();
        
        SocketClient::getSingleton().init();
		CSVMgr::getSingleton().init();
        
        ImgMgr::getSingleton().load("imgmgr.csv");
		AniObjMgr::getSingleton().load("aniobj.csv");
        ImgMgr::getSingleton().addSpriteFrames("game_ui");
        
		ImgMgr::getSingleton().addAsyncImg("building001");
		ImgMgr::getSingleton().addAsyncImg("building002");
		ImgMgr::getSingleton().addAsyncImg("building003");
		ImgMgr::getSingleton().addAsyncImg("building004");

		ImgMgr::getSingleton().addAsyncImg("scenemov001");

        ImgMgr::getSingleton().addAsyncImg("scenemov002");

		ImgMgr::getSingleton().addAsyncImg("barrack");
        
        ImgMgr::getSingleton().addAsyncImg("clan_icon");
        
        SoundMgr::getSingleton().init();
        HeroMgr::getSingleton().init();
        
        PlayerScene::getSingleton().m_pGameWorld = NULL;
        
		schedule(schedule_selector(HelloWorld::onIdle));

        bRet = true;
    } while (0);
    
    return bRet;
}

void HelloWorld::onEnter()
{
    CCLayer::onEnter();
    
    showLogoUI();
}

void HelloWorld::onExit()
{
    this->removeAllChildrenWithCleanup(true);
    CCLayer::onExit();
}

void HelloWorld::onLogoEnd()
{
    this->m_pDelegate = &(HttpClient::getSingleton());
    this->m_funcRelease = callfunc_selector(HttpClient::onHelloWorldRelease);
    this->retain();
    
    HttpClient::getSingleton().m_pHelloWorld = this;
    
    ImgMgr::getSingleton().startAsyncLoad(this, callfunc_selector(HelloWorld::onAsyncLoad));
    
    hideLogoUI();
    showLoadingUI();
}

void HelloWorld::OnLogoStart(float dt)
{
    unschedule(schedule_selector(HelloWorld::OnLogoStart));
}

bool HelloWorld::onNotify(int notifyid, const void* pVal)
{
    return true;
}

//! 如果返回false，表示拦截消息
bool HelloWorld::onProc(const Json::Value& val)
{
    if (val["cid"].asString() == "lsen")
    {
        addLoadingProgress(0.1f);
        HttpClient::getSingleton().checkVer3();
        
        CCLOG("check ver3");
    }
	else if(val["cid"].asString() == "initok")
	{
        if (NETWORK_TIME_DEBUG) {
            CCDirector::sharedDirector()->drawScene();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
			Sleep(1000);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)

#else
			usleep(1000);
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
        }
        
		if(HTTPCLIENT_STATE_PROCLAST == HttpClient::getSingleton().m_curClientState)
		{
			HttpClient::getSingleton().onProcLoad();
            
			return true;
		}
		else if(HTTPCLIENT_STATE_WAITLOGIN == HttpClient::getSingleton().m_curClientState)
			HttpClient::getSingleton().m_curClientState = HTTPCLIENT_STATE_INIT;
        
		HttpClient::getSingleton().removeListener(this);
        
        LogicInfo::getSingleton().m_strDebugLog = "";
        
		GameWorld::show();
	}

	return true;
}

void HelloWorld::chg2Game()
{

}

bool HelloWorld::show()
{
    bool reload = true;
    
	if(LogicInfo::getSingleton().m_curInPurchaseProductID != -1)
		reload = false;
    
	if(LogicInfo::getSingleton().m_isPause)
	{
		if(LOGICSTATE_WAIT != LogicInfo::getSingleton().m_curState)
		{
			if (LOGICSTATE_ATTACK == LogicInfo::getSingleton().m_curState) {
				if(getCurrentTime() - LogicInfo::getSingleton().m_pauseBeginTime < 30)
				{
					reload = false;
				}
				else if (LogicInfo::getSingleton().m_attackType != ATTACK_TYPE_NULL)
				{
					LogicInfo::getSingleton().onStopAttack(true);
				}
			}
			else
			{
				if(getCurrentTime() - LogicInfo::getSingleton().m_pauseBeginTime < 60)
				{
                    LogicInfo::getSingleton().onInitTroophousingSoldier();
					reload = false;
				}
			}
		}
	}
	else {
		reload = false;
	}
    
    if (reload) {
        showEx();
    }
    else {
        if (LogicInfo::getSingleton().m_payType == PAY_TYPE_PAYPAL) {
            HttpClient::getSingleton().requestPaypalPurchase();
        }
    }
        
    return reload;
}

void HelloWorld::showEx()
{
    if (PlayerScene::getSingleton().m_pGameWorld) {
        PlayerScene::getSingleton().m_pGameWorld->hideMainUI();
    }
    
    LogicInfo::getSingleton().m_myInfo.sessionid = 0;
    
	LogicInfo::getSingleton().m_pauseBeginTime = 0;
        
	LogicInfo::getSingleton().resetAll();
    
    PlayerScene::getSingleton().clearScene();
    
	CCScene *scene = HelloWorld::scene();
    
	CCScene* pScene = CCDirector::sharedDirector()->getRunningScene();
	if(!pScene)
		CCDirector::sharedDirector()->runWithScene(scene);
	else
	{
		CCDirector::sharedDirector()->replaceScene(scene);
	}
    
//    if (LogicInfo::getSingleton().m_isReviewing || LogicInfo::getSingleton().m_iAdsOpen)
//    {
//        if(LogicInfo::getSingleton().m_defaultFullAdsType == 1)
//        {
//            CAppPlatform::sharedAppPlatform()->showNativeXOfferAds(NATIVE_X_GAMELAUNCH);
//        }
//        else if(LogicInfo::getSingleton().m_defaultFullAdsType == 0)
//        {
//            CAppPlatform::sharedAppPlatform()->showChartBoostAds();
//        }
//    }
}

void HelloWorld::pause()
{
	LogicInfo::getSingleton().onPause();
}

void HelloWorld::showLogoUI()
{
	ImgMgr::getSingleton().addSpriteFrames("game_title");

    m_pLogoUI = LogoUI::create();
    addChild(m_pLogoUI, 1);
}

void HelloWorld::hideLogoUI()
{
    removeChild(m_pLogoUI, true);
    m_pLogoUI = NULL;
}

void HelloWorld::showLoadingUI()
{
    ImgMgr::getSingleton().addSpriteFrames("loading");
    
    m_pLoadingUI = LoadingUI::create();
    
    addChild(m_pLoadingUI, 1);
}

void HelloWorld::hideLoadingUI()
{
    ImgMgr::getSingleton().removeRes("loading");
    
    removeChild(m_pLoadingUI, true);

	m_pLoadingUI = NULL;
}

void HelloWorld::onIdle(float dt)
{
    if (m_pLogoUI) {
        if (m_pLogoUI->m_isActionEnd)
        {
            onLogoEnd();
        }
        return;
    }
    
	HttpClient::getSingleton().onIdle();

	ImgMgr::getSingleton().onAsyncLoad(dt);
    
	if(m_pLoadingUI != NULL)
	{
		if(m_strLoadingInfo != LogicInfo::getSingleton().m_strLoadingInfo)
		{
			m_strLoadingInfo = LogicInfo::getSingleton().m_strLoadingInfo;
			m_pLoadingUI->refreshLoadingState(m_strLoadingInfo.c_str());
		}
        
        if (m_strDebugLog != LogicInfo::getSingleton().m_strDebugLog) {
            m_strDebugLog = LogicInfo::getSingleton().m_strDebugLog;
            
            m_pLoadingUI->m_pTestLog->setString(m_strDebugLog.c_str());
        }
	}
    
    if (!LogicInfo::getSingleton().m_isLogin) {
        
        if (LogicInfo::getSingleton().m_isVerOK && ImgMgr::getSingleton().m_numsLoaded == ImgMgr::getSingleton().m_lstImg.size())
        {
            HttpClient::getSingleton().login();
            
            HttpClient::getSingleton().load();
            
            addLoadingProgress(0.1f);
        }
    }
}

void HelloWorld::onAsyncLoad()
{
    float p = m_loadingProgress + ((float)ImgMgr::getSingleton().m_numsLoaded / (float)ImgMgr::getSingleton().m_lstImg.size()) * 0.7f;
    
	LogicInfo::getSingleton().onNotifyLoading(p);
}

void HelloWorld::addLoadingProgress(float per)
{
    m_loadingProgress += per;
    onAsyncLoad();
}
