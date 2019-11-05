//
//  SaleLogoUI.cpp
//  GameClient
//
//  Created by owenlin on 14-2-24.
//
//

#include "SaleLogoUI.h"
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

SaleLogoUI::SaleLogoUI()
{
}

SaleLogoUI::~SaleLogoUI()
{
    
}

void SaleLogoUI::onExit()
{
    unschedule(schedule_selector(SaleLogoUI::click));
    CUINode::onExit();
}
void SaleLogoUI::onEnter()
{
    CUINode::onEnter();
}


bool SaleLogoUI::init()
{
    bool bRet = false;
    do{
        CC_BREAK_IF(!CUINode::init());
        {
            CCNode* pSaleLogoUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("shopsale_logo.ccbi", this);

            this->setTargetTouch(true);
            this->setContentSize(pSaleLogoUI->getContentSize());
            this->addChild(pSaleLogoUI);
            m_iRemainTime = 220310;
            refreshInfo();
            
            bRet = true;
        }
    }
    while (0);
    return bRet;
}

void SaleLogoUI::refreshInfo()
{
    if(m_iRemainTime > 0)
    {
        click(0);
        schedule(schedule_selector(SaleLogoUI::click), 1, kCCRepeatForever, 0);
    }
    else
    {
        m_pTimeLabel->setString("NOW");
    }
}
void SaleLogoUI::click(float ot)
{
    if(m_iRemainTime <= 0)
    {
        unschedule(schedule_selector(SaleLogoUI::click));
        refreshInfo();
        return;
    }

    int inputSeconds = (int)m_iRemainTime;
    int day = inputSeconds / 3600/24;
	int hours =  (inputSeconds - day*3600*24) / 3600;
	int minutes = ( inputSeconds - day*3600*24 - hours * 3600 ) / 60;
	int seconds = inputSeconds - day*3600*24 - hours * 3600  - minutes * 60;
    char strTime[128];
    if(day>0)
    {
        sprintf(strTime, "%dd %.2dh %.2dm",day,hours,minutes);
//        sprintf(strTime, "%dd %.2dh %.2dm %.2ds",day,hours,minutes,seconds);
    }
    else
    {
        sprintf(strTime, "%dh %.2dm %.2ds",hours,minutes,seconds);
    }
    
    m_pTimeLabel->setString(strTime);
    --m_iRemainTime;
}

#pragma mark - ccb

void SaleLogoUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideSaleLogoUI();
}
void SaleLogoUI::menuBuyCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideSaleLogoUI();
    PlayerScene::getSingleton().m_pGameWorld->showShop(1);
}

bool SaleLogoUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "m_pTimeLabel", CCLabelBMFont*, m_pTimeLabel);
    return true;
}

SEL_CCControlHandler SaleLogoUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", SaleLogoUI::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBuyCallback", SaleLogoUI::menuBuyCallback);
    return NULL;
}

