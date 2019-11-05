//
//  RaidNotifyUI.cpp
//  GameClient
//
//  Created by chowray on 13-8-25.
//
//

#include "RaidNotifyUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/PlayerScene.h"
#include "../coc/LogicInfo.h"

using namespace cocos2d;
USING_NS_COC;

RaidNotifyUI::RaidNotifyUI()
{
    
}

RaidNotifyUI::~RaidNotifyUI()
{
    
}

bool RaidNotifyUI::init()
{
    bool bRet = false;
    
    do{
        
        CC_BREAK_IF(!CUINode::init());
        
        {
            CCNode* notifyUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("raid_notify.ccbi", this);
            this->setTargetTouch(true);
            this->setContentSize(notifyUI->getContentSize());
            this->addChild(notifyUI);
            
            CCScrollView* pScrollView = CCScrollView::create(m_pItemView->getContentSize());
            pScrollView->setDirection(kCCScrollViewDirectionVertical);
            pScrollView->setContentSize(pScrollView->getViewSize());
            
            m_pItemView->addChild(pScrollView);
            
            int count = 0;
            for(int i = 0; i < LogicInfo::getSingleton().m_vecDefenseLog.size(); ++i)
            {
                FightMsg msg = LogicInfo::getSingleton().m_vecDefenseLog[i];
                if(msg.msgid != -1 && msg.time > LogicInfo::getSingleton().m_lastLoginTime - 60)
                {
                    count++;
                }
            }
            
            int offY = 56 + 5;
            float posY = offY * count;
            if (posY < pScrollView->getViewSize().height) {
                posY = pScrollView->getViewSize().height;
            }
            
            char str[128];
            int score = 0;
            int index = 0;
            for(int i = 0; i < LogicInfo::getSingleton().m_vecDefenseLog.size(); ++i)
            {
                FightMsg msg = LogicInfo::getSingleton().m_vecDefenseLog[i];
                
                if(msg.msgid != -1 && msg.time >= LogicInfo::getSingleton().m_lastLoginTime - 60)
                {
                    CCNode* pItem = CCNode::create();
                    pItem->setPosition(0, posY-(index+1)*offY);
                    
                    CCSprite* pIcon = ImgMgr::getSingleton().loadSprite("map_Level_icon");
                    
                    sprintf(str, "%d", msg.fsi.user[0].curlevel);
                    CCLabelBMFont* pLevel = CCLabelBMFont::create(str, "Font/tbFont_b.fnt");
                    pLevel->setPosition(ccp(pIcon->getContentSize().width/2, pIcon->getContentSize().height/2));
                    pLevel->setScale(0.5);
                    pIcon->addChild(pLevel);
                    pIcon->setPosition(ccp(pIcon->getContentSize().width/2 + 5, 27));
                    
                    CCLabelTTF* pName = CCLabelTTF::create(msg.fsi.user[0].name, "Font/Candal.ttf", 20);
                    pName->setHorizontalAlignment(kCCTextAlignmentLeft);
                    pName->setAnchorPoint(ccp(0, 0.5));
                    pName->setPosition(ccp(pIcon->getContentSize().width + 10, 29));
                    
                    CCLabelBMFont* pTime = CCLabelBMFont::create(formatTimeAgo(msg.time).c_str(), "Font/tbFont_s.fnt");
                    pTime->setAnchorPoint(ccp(1, 0.5));
                    pTime->setColor(ccBLACK);
                    pTime->setScale(0.4);
                    pTime->setPosition(ccp(pScrollView->getViewSize().width - 10, 28));
                    
                    pItem->addChild(pIcon);
                    pItem->addChild(pName);
                    pItem->addChild(pTime);
                    
                    pScrollView->addChild(pItem);
                    
                    score += msg.defendCup;

                    index++;
                }
            }
            
            if (posY > pScrollView->getViewSize().height) {
                pScrollView->setContentSize(CCSizeMake(pScrollView->getViewSize().width, posY));
                pScrollView->setContentOffset(ccp(0, -(posY-pScrollView->getViewSize().height)));
                pScrollView->setTouchEnabled(true);
            }
            
            if (score>0) {
                sprintf(str, "+%d", score);
            }else{
                sprintf(str, "%d", score);
            }

            m_pCup->setString(str);
        }
        bRet = true;
    }
    while (0);
    
    return bRet;
    
}

// a selector callback
void RaidNotifyUI::menuCloseCallback(CCObject* pSender, CCControlEvent cce)
{
    PlayerScene::getSingleton().m_pGameWorld->hideRaidNotifyUI();
}

bool RaidNotifyUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Cup", CCLabelBMFont*, m_pCup);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ItemView", CCLayer*, m_pItemView);
    
    return NULL;
}

SEL_CCControlHandler RaidNotifyUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", RaidNotifyUI::menuCloseCallback);
    return NULL;
}
        