#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "coc/NetListener.h"
#include "coc/NotifyListener.h"
#include "SimpleAudioEngine.h"

USING_NS_COC;

class LogoUI;
class LoadingUI;

class HelloWorld : public cocos2d::CCLayer, public NotifyListener, public NetListener
{
public:
    
    HelloWorld();
    
	virtual ~HelloWorld();
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    bool onNotify(int notifyid, const void* pVal = NULL);
    
public:
    
    void onEnter();
    
    void onExit();

    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
	void OnLogoStart(float dt);

	void onIdle(float dt);
    //// a selector callback
    //void menuCloseCallback(CCObject* pSender);

    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);
    
	//! 如果返回false，表示拦截消息
	virtual bool onProc(const Json::Value& val);
    
    static bool show();
    static void showEx();
    
    static void pause();
    
    void showLogoUI();
    void hideLogoUI();
    
    void showLoadingUI();
    
    void hideLoadingUI();

	void chg2Game();

	void onAsyncLoad();
    
    void onLogoEnd();
    
    void addLoadingProgress(float per);
        
public:
    
    LogoUI* m_pLogoUI;
    LoadingUI* m_pLoadingUI;

	string	m_strLoadingInfo;
    string  m_strDebugLog;
    
    float m_loadingProgress;
};

#endif  // __HELLOWORLD_SCENE_H__
