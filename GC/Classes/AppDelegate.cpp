#include "cocos2d.h"
#include "CCEGLView.h"
#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "coc/LogicInfo.h"
#include "CAppPlatform.h"
#include "coc/SoundMgr.h"
#include "CFacebookService.h"

using namespace CocosDenshion;

USING_NS_COC;
USING_NS_CC;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
    SimpleAudioEngine::end();
    applicationWillTerminate();
}

bool AppDelegate::applicationDidFinishLaunching()
{
    NGNKit::sharedKit()->init();
    CAppPlatform::sharedAppPlatform()->initPlatform();
    CFacebookService::sharedFaceBook()->init();
    
    // initialize director
    CCDirector *pDirector = CCDirector::sharedDirector();
    pDirector->setOpenGLView(CCEGLView::sharedOpenGLView());
    pDirector->setProjection(kCCDirectorProjection2D);

    //enable High Resource Mode(2x, such as iphone4) and maintains low resource on other devices.
    //pDirector->enableRetinaDisplay(true);
    //pDirector->setContentScaleFactor(2.0f);
    CCEGLView *pEGLView = CCEGLView::sharedOpenGLView();
    
    int w = pDirector->getWinSizeInPixels().width;
    int h = pDirector->getWinSizeInPixels().height;
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if(h == 1536)
    {
        w = 1024;
        h = 768;
    }else if (h == 320){
        w = 960;
        h = 640;
    }
#else
	float fw = pDirector->getWinSizeInPixels().width / 960.0f;
	float fh = pDirector->getWinSizeInPixels().height / 640;
    
	if(fw >= fh)
	{
		w = pDirector->getWinSizeInPixels().width * 640 / pDirector->getWinSizeInPixels().height;
		h = 640;
	}
	else
	{
		w = 960;
		h = pDirector->getWinSizeInPixels().height * 960 / pDirector->getWinSizeInPixels().width;
	}
#endif

	pEGLView->setDesignResolutionSize(w, h, kResolutionShowAll);
    
    // turn on display FPS
//    pDirector->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

    CAppPlatform::sharedAppPlatform()->cancelAllNotifications();
    
    LogicInfo::getSingleton().onResume();
    
    // create a scene. it's an autorelease object
    CCScene *pScene = HelloWorld::scene();
    pDirector->runWithScene(pScene);
    
    CAppPlatform::sharedAppPlatform()->bindDeviceUserId();
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    LogicInfo::getSingleton().onPause();

    CCDirector::sharedDirector()->stopAnimation();

    SoundMgr::getSingleton().pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
	CAppPlatform::sharedAppPlatform()->cancelAllNotifications();

    CCDirector::sharedDirector()->startAnimation();

    SoundMgr::getSingleton().resumeBackgroundMusic();
    
    LogicInfo::getSingleton().onResume();
}

// The function Called when the application is about to terminate.
void AppDelegate::applicationWillTerminate()
{
    LogicInfo::getSingleton().onExit();
    CCLOG("application will terminate!");
}
