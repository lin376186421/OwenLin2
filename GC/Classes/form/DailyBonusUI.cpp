//
//  DailyBonusUI.cpp
//  GameClient
//
//  Created by Lee  on 14-1-6.
//
//

#include "DailyBonusUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/PlayerScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/HeroMgr.h"
#include "../coc/SkillMgr.h"
#include "../coc/HttpClient.h"
#include "../coc/GuideMgr.h"
#include "../coc/RandMgr.h"
#include "../coc/ConfigValMgr.h"

using namespace cocos2d;
USING_NS_COC;

DailyBonusUI::DailyBonusUI()
{
}

DailyBonusUI::~DailyBonusUI()
{
    m_pAnimationManager->setDelegate(NULL);
}

bool DailyBonusUI::init()
{
    bool bRet = false;
    do{
        CC_BREAK_IF(!CUINode::init());
        {
            
            CCNode* pDailyBonusUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("frame_login.ccbi", this, &m_pAnimationManager);
            m_pAnimationManager->setDelegate(this);
            
            this->setTargetTouch(true);
            this->setContentSize(pDailyBonusUI->getContentSize());
            this->addChild(pDailyBonusUI);
            
            refresh();
            bRet = true;
        }
    }
    while (0);
    return bRet;
}

void DailyBonusUI::refresh()
{
    int days = LogicInfo::getSingleton().m_signinDays+1;
    m_pDaysLabel->setString(CCStringFormat(ViewString::getValue("signin_tip").c_str(),days).c_str());
    if (days>7)
    {
        days = 7;
    }
    for (int i = 1; i<= 7; i++)
    {
        
        const SigninAwardInfo pAwardInfo = ConfigValMgr::getSingleton().getSigninAward(i-1);
        
        CCNode *pMainNode = m_pMainNode->getChildByTag(i);
        CCNode *pGrayNode1 = pMainNode->getChildByTag(1);
        CCNode *pLightNode1 = pMainNode->getChildByTag(3);
        CCNode *pGrayNode2 = pMainNode->getChildByTag(2);
        CCNode *pLightNode2 = pMainNode->getChildByTag(4);
        CCNode *pDone = pMainNode->getChildByTag(8);
        CCControlButton  *pButton = (CCControlButton *)pMainNode->getChildByTag(7);
        CCSprite *pType = (CCSprite *)pMainNode->getChildByTag(5);
        CCLabelBMFont *pValLabel = (CCLabelBMFont *)pMainNode->getChildByTag(6);
        pValLabel->setString(CCStringFormat("%d",pAwardInfo.val).c_str());
        if (std::strcmp(pAwardInfo.type.c_str(), "gem") == 0)
        {
            pType->setDisplayFrame(ImgMgr::getSingleton().getFrame("map_Gems_icon"));
        } else if (std::strcmp(pAwardInfo.type.c_str(), "gold") == 0)
        {
            pType->setDisplayFrame(ImgMgr::getSingleton().getFrame("map_Coins_icon"));
            
        } else if (std::strcmp(pAwardInfo.type.c_str(), "oil") == 0)
        {
            pType->setDisplayFrame(ImgMgr::getSingleton().getFrame("map_Elixir_icon"));
        }
        if (days > i)
        {
            pGrayNode1->setVisible(false);
            pGrayNode2->setVisible(false);
            pLightNode1->setVisible(true);
            pLightNode2->setVisible(true);
            pDone->setVisible(true);
            pButton->setVisible(false);
        } else if(days == i)
        {
            pGrayNode1->setVisible(false);
            pGrayNode2->setVisible(false);
            pLightNode1->setVisible(true);
            pLightNode2->setVisible(true);
            pDone->setVisible(false);
            pButton->setVisible(true);
        } else
        {
            pGrayNode1->setVisible(true);
            pGrayNode2->setVisible(true);
            pLightNode1->setVisible(false);
            pLightNode2->setVisible(false);
            pDone->setVisible(false);
            pButton->setEnabled(false);
        }
    }
}

#pragma mark - ccb

void DailyBonusUI::menuGetCallback(CCObject* pSender, CCControlEvent event)
{
    
    
    int days = LogicInfo::getSingleton().m_signinDays + 1;
    if (days > 7)
    {
        days = 7;
    }
    const SigninAwardInfo pAwardInfo = ConfigValMgr::getSingleton().getSigninAward(days-1);
    
    if (std::strcmp(pAwardInfo.type.c_str(), "gem") == 0)
    {
        LogicInfo::getSingleton().chgGem(pAwardInfo.val);
    } else if (std::strcmp(pAwardInfo.type.c_str(), "gold") == 0)
    {
        LogicInfo::getSingleton().chgGold(pAwardInfo.val);
        
    } else if (std::strcmp(pAwardInfo.type.c_str(), "oil") == 0)
    {
        LogicInfo::getSingleton().chgOil(pAwardInfo.val);
    }
    
    LogicInfo::getSingleton().onNotify(NOTIFY_BASEINFO);
    
    CCControlButton *pButton = (CCControlButton *)pSender;
    pButton->setEnabled(false);
    m_pAnimationManager->runAnimationsForSequenceNamed("frame_login_out");
    
    HttpClient::getSingleton().signin();
    
}

bool DailyBonusUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "MainNode", CCNode*, m_pMainNode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DaysLabel", CCLabelBMFont*, m_pDaysLabel);

    
    return true;
}

SEL_CCControlHandler DailyBonusUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuGetCallback", DailyBonusUI::menuGetCallback);
    return NULL;
}

void DailyBonusUI::completedAnimationSequenceNamed(const char *name)
{
    if (strcmp(name, "frame_login_out") == 0)
    {
        removeFromParentAndCleanup(true);
    }
}



