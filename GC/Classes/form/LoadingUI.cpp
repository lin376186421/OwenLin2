//
//  LoadingUI.cpp
//  GameClient
//
//  Created by chowray on 13-5-10.
//
//

#include "LoadingUI.h"

#include "../coc/ImgMgr.h"
#include "../coc/SoundMgr.h"
#include "../coc/LogicInfo.h"
#include "CAppPlatform.h"
#include "../coc/ResConfig.h"

LoadingUI::LoadingUI()
{
    
}

LoadingUI::~LoadingUI()
{
    LogicInfo::getSingleton().removeListener(this);
}

bool LoadingUI::init()
{
    bool bRet = false;
    do
    {
		CC_BREAK_IF(!CUINode::init());
        
        LogicInfo::getSingleton().addListener(this);
				
        CCNode* pLoadingUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("game_loading.ccbi", this);
        this->setContentSize(pLoadingUI->getContentSize());
        this->addChild(pLoadingUI);
        
        m_pTitle->setString("Loading. . .");
        
        int index = (rand() % 10);
        char str[128];
        sprintf(str, "loading_tip_%d", index);
        m_pTip->setString(ViewString::getSingleton().getValue(str).c_str());
        
        if (SERVICE_DEBUG == 0) {
            sprintf(str, "Version:%s", CAppPlatform::sharedAppPlatform()->getBundleVersion().c_str());
        }
        else if (SERVICE_DEBUG == 1) {
            sprintf(str, "Version:%s Dev", CAppPlatform::sharedAppPlatform()->getBundleVersion().c_str());
        }
        else if (SERVICE_DEBUG == 2) {
            sprintf(str, "Version:%s Debug", CAppPlatform::sharedAppPlatform()->getBundleVersion().c_str());
        }
        m_pVersion->setString(str);
        
        if (ResConfig::getSingleton().getIntValue("RES_TYPE") == 1)
        {
            float scale = CCDirector::sharedDirector()->getWinSize().width/m_pBg->getContentSize().width;
            m_pBg->setScale(scale);
        }
        SoundMgr::getSingleton().playBackgroundMusic("game_loading.mp3");
        
        if (NETWORK_TIME_DEBUG) {
            m_pTestLog = CCLabelBMFont::create("", "Font/tbFont_b.fnt", -1, kCCTextAlignmentCenter);
            m_pTestLog->setAnchorPoint(ccp(0.5, 1));
            m_pTestLog->setScale(0.5);
            m_pTestLog->setColor(ccRED);
            m_pTestLog->setPosition(ccp(this->getContentSize().width/2, this->getContentSize().height * 0.8));
            this->addChild(m_pTestLog);
        }
        
        //m_bModal = true;
		bRet = true;
	}while(0);
    
    refresh();
    
	return bRet;
}

void LoadingUI::refresh()
{
    changeProgress(m_pProgress, LogicInfo::getSingleton().m_loadingProgress);
}

bool LoadingUI::onNotify(int notifyid, const void* pVal)
{
    if(NOTIFY_LOADINGPROGRESS == notifyid)
    {
        refresh();
    }
    
	return true;
}

void LoadingUI::refreshLoadingState(const char* s)
{
    m_pTitle->setString(s);
}

void LoadingUI::refreshInfo(const char* s)
{
    m_pTip->setString(s);
}

bool LoadingUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Title", CCLabelBMFont*, m_pTitle);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Tip", CCLabelBMFont*, m_pTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Version", CCLabelBMFont*, m_pVersion);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Progress", CCScale9Sprite*, m_pProgress);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Background", CCSprite*, m_pBg);
    
    return true;
}

SEL_CCControlHandler LoadingUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    return NULL;
}