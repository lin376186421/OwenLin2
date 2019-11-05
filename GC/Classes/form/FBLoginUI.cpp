//
//  FBLoginUI.cpp
//  GameClient
//
//  Created by chowray on 13-9-4.
//
//

#include "FBLoginUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/PlayerScene.h"
#include "../coc/ConfigValMgr.h"
#include "CAppPlatform.h"
#include "CFacebookService.h"

using namespace cocos2d;
USING_NS_COC;

//state 0未登录fb  1已登录 2提示登录
FBLoginUI::FBLoginUI(int state)
{
    m_state = state;
}

FBLoginUI::~FBLoginUI()
{
    LogicInfo::getSingleton().removeListener(this);
}
FBLoginUI* FBLoginUI::create(int state)
{
    FBLoginUI *pRet = new FBLoginUI(state);
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}
bool FBLoginUI::init()
{
	bool bRet = false;
    do
    {
		CC_BREAK_IF(!CUINode::init());
        
        LogicInfo::getSingleton().addListener(this);
        
        m_pCCBNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("facebook_Login.ccbi", this);
        this->setContentSize(m_pCCBNode->getContentSize());
        this->setTargetTouch(true);
        this->addChild(m_pCCBNode);
        
        if(FacebookMgr::getSingleton().m_isGetFreeGemLogin)
            m_pGemTip->setVisible(true);
        else
            m_pGemTip->setVisible(false);
                
        refreshInfo();
        
        
		bRet = true;
	}while(0);
    
	return bRet;
    
}
bool FBLoginUI::onNotify(int notifyid, const void* pVal)
{
    if (NOTIFY_FB_LOGIN_OK == notifyid)
    {
        m_state = 1;
        refreshInfo();
    }
    else if (NOTIFY_FB_LOGIN_OUT_OK == notifyid)
    {
        m_state = 0;
        refreshInfo();
    }
    
    return true;
}
void FBLoginUI::refreshInfo()
{
    m_pLoginNode = m_pCCBNode->getChildByTag(2);
    m_pSwithNode = m_pCCBNode->getChildByTag(1);
    
    if(m_state == 0)
    {
        m_pLoginNode->setVisible(true);
        m_pSwithNode->setVisible(false);
    }
    else if(m_state == 1)
    {
        m_pLoginNode->setVisible(false);
        m_pSwithNode->setVisible(true);
        CCLOG("facebook --- %s",FacebookMgr::getSingleton().m_fbUser.fname.c_str());
        m_pUserName->setString(FacebookMgr::getSingleton().m_fbUser.fname.c_str());
    }
    else if (m_state == 3)
    {
        
    }
}

// a selector callback
void FBLoginUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideFBLoginUI();
}

void FBLoginUI::menuFBSwithCallback(CCObject* pSender, CCControlEvent event)
{
    CFacebookService::sharedFaceBook()->fbLoginOut();
//    CAppPlatform::sharedAppPlatform()->fbLogin();
//    PlayerScene::getSingleton().m_pGameWorld->hideFBLoginUI();
}

void FBLoginUI::menuFBLoginCallback(CCObject* pSender, CCControlEvent event)
{
    CFacebookService::sharedFaceBook()->fbLogin();
//    PlayerScene::getSingleton().m_pGameWorld->hideFBLoginUI();
}

bool FBLoginUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnClose", CCControlButton*, m_pBtnClose);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnFBLogin", CCControlButton*, m_pBtnFBLogin);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnFBSwith", CCControlButton*, m_pBtnFBSwith);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UserName", CCLabelTTF*, m_pUserName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GemTip", CCSprite*, m_pGemTip);
    return true;
}

SEL_CCControlHandler FBLoginUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", FBLoginUI::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuFBLoginCallback", FBLoginUI::menuFBLoginCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuFBSwithCallback", FBLoginUI::menuFBSwithCallback);
    
    
    return NULL;
}
