//
//  FBShareUI.cpp
//  GameClient
//
//  Created by chowray on 13-9-4.
//
//

#include "FBShareUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/PlayerScene.h"
#include "../coc/ConfigValMgr.h"
#include "CAppPlatform.h"

using namespace cocos2d;
USING_NS_COC;


FBShareUI::FBShareUI()
{
}

FBShareUI::~FBShareUI()
{
    
}

bool FBShareUI::init()
{
	bool bRet = false;
    do
    {
		CC_BREAK_IF(!CUINode::init());
        
        
        CCNode *pCCBNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("facebook_share.ccbi", this);
        this->setContentSize(pCCBNode->getContentSize());
        this->setTargetTouch(true);
        
        m_pDescField = CTextInputField::create(m_pDescInputBg->getContentSize(), CCScale9Sprite::create());
        m_pDescField->setAnchorPoint(ccp(0, 0));
        m_pDescField->setFontSize(20);
        m_pDescField->setFontColor(ccBLACK);
        m_pDescField->setInputMode(kTextFieldInputModeMultiLine);
        
        m_pDescField->setText(FacebookMgr::getSingleton().m_shareMessaage.c_str());
        
        m_pDescInputBg->addChild(m_pDescField);
        
        
        
        this->addChild(pCCBNode);
        
        
        
        if(FacebookMgr::getSingleton().m_shareFreeGem && FacebookMgr::getSingleton().m_shareType == FBShare_Base)
            m_pTipNode->setVisible(true);
        else
            m_pTipNode->setVisible(false);
        
		bRet = true;
	}while(0);
    
	return bRet;
    
}

// a selector callback
void FBShareUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideFBShareUI();
    FacebookMgr::getSingleton().m_pCurBtn = NULL;
}

void FBShareUI::menuFBShareCallback(CCObject* pSender, CCControlEvent event)
{
    string message;
    if(strlen(m_pDescField->getText()) > 0)
    {
        message = string(m_pDescField->getText());
    }
    PlayerScene::getSingleton().m_pGameWorld->hideFBShareUI();
    
    if(FacebookMgr::getSingleton().m_pCurBtn)
        FacebookMgr::getSingleton().m_pCurBtn->setVisible(false);
    
    FacebookMgr::getSingleton().fbShare(message);
    
    if(FacebookMgr::getSingleton().m_pCurBtn)
        FacebookMgr::getSingleton().m_pCurBtn->setVisible(true);
    
    FacebookMgr::getSingleton().m_pCurBtn = NULL;
}



bool FBShareUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TipNode", CCNode*, m_pTipNode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "InputBg", CCScale9Sprite*, m_pDescInputBg);
    
    return true;
}

SEL_CCControlHandler FBShareUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", FBShareUI::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuFBShareCallback", FBShareUI::menuFBShareCallback);
    
    
    return NULL;
}
