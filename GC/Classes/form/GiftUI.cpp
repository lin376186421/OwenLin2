//
//  GiftUI.cpp
//  GameClient
//
//  Created by owenlin on 14-3-12.
//
//

#include "GiftUI.h"
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



GiftUI::GiftUI()
{
}

GiftUI::~GiftUI()
{
    unschedule(schedule_selector(GiftUI::click));
}

bool GiftUI::init()
{
    bool bRet = false;
    do{
        CC_BREAK_IF(!CUINode::init());
        {
            CCNode* pGiftUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("reward.ccbi", this);
            this->setTargetTouch(true);
            this->setContentSize(pGiftUI->getContentSize());
            this->addChild(pGiftUI);
            refreshInfo();
            bRet = true;
        }
    }
    while (0);
    return bRet;
}

void GiftUI::refreshInfo()
{
    unschedule(schedule_selector(GiftUI::click));
    IapGiftInfo m_iapGiftInfo = LogicInfo::getSingleton().m_iapGiftInfo;
    char str[500];
    if(LogicInfo::getSingleton().m_iapGiftInfo.corflag == 0)
    {
        m_pClaimNode->setVisible(false);
        m_pTipNode->setVisible(true);
        m_pCloseBtn->setVisible(true);
        m_pTitleLabel->setString(ViewString::getValue("first_package_title").c_str());
        
        std::map <int,IapHighlightInfo>::iterator it = m_iapGiftInfo.corIapMap.begin();
        int productID = it->first;
        IapHighlightInfo *info = &(it->second);
        int num = LogicInfo::getSingleton().getIAPInfoFromProductID(productID)->gem;
        
        sprintf(str, ViewString::getValue("gift_packages_tip").c_str(),100*info->totalGem/num+100);
        m_pinfoLabel->setString(str);
        
        int index = 0;
        m_iProductIdList.clear();
        for(map<int, IapHighlightInfo>::iterator it = m_iapGiftInfo.corIapMap.begin();it != m_iapGiftInfo.corIapMap.end(); ++it)
        {
            index +=1;
            const InPurchaseProductInfo *m_inPurchaseInfo = LogicInfo::getSingleton().getIAPInfoFromProductID(it->first);
            IapHighlightInfo *m_iapGiftInfo = &(it->second);
            CCNode *infoNode = m_pTipNode->getChildByTag(index);
            m_iProductIdList.push_back(it->first);
            
            
            CCSprite *bestValue = (CCSprite *)infoNode->getChildByTag(Tag_bestValueSprite);
            if(m_iapGiftInfo->Highlight == 0)
                bestValue->setVisible(false);
            else
                bestValue->setVisible(true);
            
            CCSprite *gemSprite = (CCSprite *)infoNode->getChildByTag(Tag_gemSprite);
            sprintf(str, "ui_gems_%d",index);
            CCSpriteFrame *frame = ImgMgr::getSingleton().getFrame(str);
            gemSprite->setDisplayFrame(frame);
            
            CCLabelBMFont *priceLabel = (CCLabelBMFont *)infoNode->getChildByTag(Tag_priceLabel);
            sprintf(str, "%s",m_inPurchaseInfo->strPrice.c_str());
            priceLabel->setString(str);
            
            
            int num = m_inPurchaseInfo->gem;
            sprintf(str, "%d%%",100*m_iapGiftInfo->totalGem/num+100);
            CCLabelBMFont *percentLabel = (CCLabelBMFont *)infoNode->getChildByTag(Tag_percentLabl);
            percentLabel->setString(str);
            
            CCLabelBMFont *totalGemLabel = (CCLabelBMFont *)infoNode->getChildByTag(Tag_totalGemLabel);
            sprintf(str, "%i",num);
            totalGemLabel->setString(str);
            
        }
        
        click(0);
        schedule(schedule_selector(GiftUI::click), 1, kCCRepeatForever, 0);
    }
    else if (LogicInfo::getSingleton().m_iapGiftInfo.corflag == 1)
    {
        PlayerScene::getSingleton().m_pGameWorld->hideGiftUI();
    }
    else if(LogicInfo::getSingleton().m_iapGiftInfo.corflag == 2)
    {
        m_pClaimNode->setVisible(true);
        m_pTipNode->setVisible(false);
        m_pCloseBtn->setVisible(false);//待修改
        m_pTitleLabel->setString(ViewString::getValue("recharge_package_title").c_str());
        
        for(int i = 1; i <= LogicInfo::getSingleton().m_iapGiftInfo.totalDays; i++)
        {
            CCControlButton *btn = (CCControlButton *)m_pClaimNode->getChildByTag(i);
            btn->setTouchEnabled(false);
            
            CCSprite *markSprite = (CCSprite *)btn->getChildByTag(Tag_markSprite);
            markSprite->setVisible(false);
            CCSpriteFrame *frame = NULL;
            if(i < m_iapGiftInfo.gotDays)
            {
                markSprite->setVisible(false);
                frame = ImgMgr::getSingleton().getFrame("reward_btn_3");
                btn->setBackgroundSpriteFrameForState(frame, CCControlStateNormal);
            }
            else if(i == m_iapGiftInfo.gotDays)
            {
                markSprite->setVisible(true);
                frame = ImgMgr::getSingleton().getFrame("reward_btn_1");
                btn->setBackgroundSpriteFrameForState(frame, CCControlStateNormal);
            }
            else if (i > m_iapGiftInfo.gotDays)
            {
                markSprite->setVisible(false);
                frame = ImgMgr::getSingleton().getFrame("reward_btn_2");
                btn->setBackgroundSpriteFrameForState(frame, CCControlStateNormal);
            }
            
            sprintf(str, "Day %d",i);
            CCLabelBMFont *dayLabel = (CCLabelBMFont *)btn->getChildByTag(Tag_dayLabel);
            dayLabel->setString(str);
            
            CCLabelBMFont *gemLabel = (CCLabelBMFont *)btn->getChildByTag(Tag_gemLabel);
            sprintf(str, "%i",m_iapGiftInfo.corclaim);
            gemLabel->setString(str);
            
           
            
        }
    }
}
void GiftUI::click(float ot)
{
    if(LogicInfo::getSingleton().m_iapGiftInfo.last_time <= 0)
    {
        LogicInfo::getSingleton().m_iapGiftInfo.timeEnd();
        refreshInfo();
        return;
    }
    
    int inputSeconds = (int)LogicInfo::getSingleton().m_iapGiftInfo.last_time;
    
    m_pTimeLabel->setString(formatTime(inputSeconds, true).c_str());
    --LogicInfo::getSingleton().m_iapGiftInfo.last_time;
}


#pragma mark - ccb

void GiftUI::menuGoCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideGiftUI();
    PlayerScene::getSingleton().m_pGameWorld->showShop(1);
}
void GiftUI::menuClaimCallback(CCObject* pSender, CCControlEvent event)
{
    LogicInfo::getSingleton().chgGem(LogicInfo::getSingleton().m_iapGiftInfo.corclaim);
    LogicInfo::getSingleton().onNotify(NOTIFY_BASEINFO);
    HttpClient::getSingleton().getIapClaim(LogicInfo::getSingleton().m_iapGiftInfo.buyId);
    PlayerScene::getSingleton().m_pGameWorld->hideGiftUI();
}
void GiftUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideGiftUI();
}
void GiftUI::menuBuyCallback(CCObject* pSender, CCControlEvent event)
{
    CCNode *node = (CCNode *)pSender;
    int productID = m_iProductIdList[node->getParent()->getTag()-1];
    LogicInfo::getSingleton().InPurchase(productID);
}
void GiftUI::menuGetGemCallback(CCObject* pSender, CCControlEvent event)
{
}
bool GiftUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "timeLabel", CCLabelBMFont*, m_pTimeLabel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "titleLabel", CCLabelBMFont*, m_pTitleLabel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "infoLabel", CCLabelBMFont*, m_pinfoLabel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "claimNode", CCNode*, m_pClaimNode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "tipNode", CCNode*, m_pTipNode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "closeBtn", CCControlButton*, m_pCloseBtn);
    return true;
}

SEL_CCControlHandler GiftUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuGetGemCallback", GiftUI::menuGetGemCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuClaimCallback", GiftUI::menuClaimCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", GiftUI::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBuyCallback", GiftUI::menuBuyCallback);
    return NULL;
}

