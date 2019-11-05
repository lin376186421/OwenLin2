//
//  LabUI.cpp
//  GameClient
//
//  Created by chowray on 13-5-27.
//
//

#include "LabUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/HttpClient.h"
#include "../coc/PlayerScene.h"
#include "../coc/Building.h"
#include "../coc/LogicInfo.h"
#include "../coc/PersonMgr.h"

#include "../coc/BaseDef.h"

using namespace cocos2d;
USING_NS_COC;

LabUI::LabUI()
{
    
}

LabUI::~LabUI()
{
    
}

bool LabUI::init()
{
    bool bRet = false;
    
    do
    {
        CC_BREAK_IF(!CUINode::init());
        
        {
            
            CCNode* labUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("laboratory_research.ccbi", this);
            
            this->setTargetTouch(true);
            this->setContentSize(labUI->getContentSize());
            this->addChild(labUI);
            
            m_lstLab.push_back(PERSON_BARBARIAN);
            m_lstLab.push_back(PERSON_ARCHER);
            m_lstLab.push_back(PERSON_GOBLIN);
            m_lstLab.push_back(PERSON_GIANT);
            m_lstLab.push_back(PERSON_WALL_BREAKER);
            m_lstLab.push_back(PERSON_BALLOON);
            m_lstLab.push_back(PERSON_WIZARD);
            m_lstLab.push_back(PERSON_HEALER);
            m_lstLab.push_back(PERSON_DRAGON);
            m_lstLab.push_back(PERSON_PEKKA);

            CCSize itemSize(120, 124);
            
            float posX = ((m_pItemView->getContentSize().width / 5) - itemSize.width)/2 + itemSize.width/2;
            float posY = m_pItemView->getContentSize().height - (m_pItemView->getContentSize().height/2 - itemSize.height)/2 - itemSize.height + itemSize.height/2;
            float offX = m_pItemView->getContentSize().width / 5;
            float offY = m_pItemView->getContentSize().height / 2;
            
            for (int index = 0; index < m_lstLab.size(); index++) {
                
                CCNode* pNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("laboratory_item.ccbi", this);
                
                CCControlButton* pItem = (CCControlButton*)pNode->getChildren()->objectAtIndex(0);
                pItem->setTag(m_lstLab[index]);
                pItem->setPosition(posX + (index/2)*offX, posY - (index%2)*offY);
             
                int level = 1;
                
                if(LogicInfo::getSingleton().getMySoldierLevel(m_lstLab[index]*100+1) != 0)
                    level = LogicInfo::getSingleton().getMySoldierLevel(m_lstLab[index]*100+1)%10;
                for(int i = 0; i < level && level > 1; ++i)
                {
                    CCSprite* pStar = ImgMgr::getSingleton().loadSprite("player_star");
                    pStar->setPosition(ccp(18 + 17 * i, 60));
                    pStar->setTag(1000+i);
                    pItem->addChild(pStar, 10);
                }
                
                m_pItemView->addChild(pNode);
            }

            refreshInfo();
            refreshTime(0);
        }
        
        bRet = true;
    } while (0);
    
    this->schedule(schedule_selector(LabUI::refreshTime));
    return bRet;
}

void LabUI::refreshInfo()
{
    char str[128];
    
    int soldierId = LogicInfo::getSingleton().getCurUpgradeSoldier();
    
    if (soldierId == -1) {

        m_pTitle->setString(ViewString::getSingleton().getValue("lab_title_choose").c_str());
        
        m_pUpgradingNode->setVisible(false);
        m_pUpgradeTip->setVisible(true);
    }
    else
    {
        m_pTitle->setString(ViewString::getSingleton().getValue("lab_title_upgrading").c_str());
        
        m_pUpgradingNode->setVisible(true);
        m_pUpgradeTip->setVisible(false);
        
        int level = 1;
        
        if(LogicInfo::getSingleton().getMySoldierLevel(soldierId) != 0)
            level = LogicInfo::getSingleton().getMySoldierLevel(soldierId)%10;
        
        const SoldierInfo* info = PersonMgr::getSingleton().getSoldierInfo(soldierId);
        
        sprintf(str, "soldier_%d", soldierId / 100);
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pUpgradeIcon->setDisplayFrame(pFrame);
        
        sprintf(str, ViewString::getSingleton().getValue("info_title").c_str(), info->name.c_str(), level+1);
        m_pUpgradeName->setString(str);
        
        int totaltime = 0;
        for(int i = 0; i < LogicInfo::getSingleton().m_soldierupgrade.nums; ++i)
        {
            if(LogicInfo::getSingleton().m_soldierupgrade.lasttime[i] > 0)
                totaltime += LogicInfo::getSingleton().m_soldierupgrade.lasttime[i];
        }
        
        int gem = totaltime / 240;
        sprintf(str, "%d", gem);
        ((CCLabelBMFont*)m_pBtnFinish->getChildByTag(1))->setString(str);
    }
    
    for (int index = 0; index < m_lstLab.size(); index++) {
        
        CUINode* pNode = (CUINode*)m_pItemView->getChildren()->objectAtIndex(index);
        
        CCControlButton* pItem = (CCControlButton*)pNode->getChildren()->objectAtIndex(0);
        
        sprintf(str, "player1_%d", m_lstLab[index]);
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        pItem->setBackgroundSpriteFrameForState(pFrame, CCControlStateNormal);
        pItem->setBackgroundSpriteFrameForState(pFrame, CCControlStateHighlighted);
        sprintf(str, "player1_%d_0", m_lstLab[index]);
        pFrame = ImgMgr::getSingleton().getFrame(str);
        pItem->setBackgroundSpriteFrameForState(pFrame, CCControlStateDisabled);
        
        if(soldierId != -1 && soldierId/100 != m_lstLab[index])
        {
            pItem->setEnabled(false);
        }
        else{
            pItem->setEnabled(true);
        }
        
        const SoldierInfo* info = PersonMgr::getSingleton().getSoldierInfo(LogicInfo::getSingleton().getMySoldierLevel(m_lstLab[index] * 100 + 1) + 1);
        
        if (info == NULL) {
            info = PersonMgr::getSingleton().getSoldierInfo(LogicInfo::getSingleton().getMySoldierLevel(m_lstLab[index] * 100 + 1));
        }
        
        pItem->getChildByTag(1)->setVisible(false);
        if(LogicInfo::getSingleton().getSoldierlevel(index) == LogicInfo::getSingleton().getSoldierMaxLevel(index))
        {
            ((CCLabelBMFont*)pItem->getChildByTag(1))->setString("Max\nLevel");
            pItem->getChildByTag(1)->setVisible(true);
            pItem->getChildByTag(2)->setVisible(false);
            pItem->setEnabled(false);
        }
        else if(info->laboratoryLevelex > LogicInfo::getSingleton().m_myLabLevel)
        {
            sprintf(str, ViewString::getSingleton().getValue("lab_request_level").c_str(), (int)info->laboratoryLevelex);
            
            ((CCLabelBMFont*)pItem->getChildByTag(1))->setString(str);
            pItem->getChildByTag(1)->setVisible(true);
            pItem->getChildByTag(2)->setVisible(false);
            pItem->setEnabled(false);
        }
        
        CCLabelBMFont* pCost = (CCLabelBMFont*)(pItem->getChildByTag(2))->getChildByTag(1);
        sprintf(str, "%d", (int)info->upgradeCost);
        pCost->setString(str);
        if(info->upgradeCost > LogicInfo::getSingleton().getMoney(MONEY_ELIXIR))
        {
            pCost->setColor(ccRED);
        }
        
        int level = 1;
        
        if(LogicInfo::getSingleton().getMySoldierLevel(m_lstLab[index]*100+1) != 0)
            level = LogicInfo::getSingleton().getMySoldierLevel(m_lstLab[index]*100+1)%10;
        for(int i = 0; i < level && level > 1; ++i)
        {
            if (pItem->getChildByTag(1000+i) == NULL) {
                CCSprite* pStar = ImgMgr::getSingleton().loadSprite("player_star");
                pStar->setTag(1000+i);
                pStar->setPosition(ccp(18 + 17 * i, 60));
                pItem->addChild(pStar, 10);
            }
        }
    }
}

void LabUI::refreshTime(float dt)
{
    if(LogicInfo::getSingleton().m_soldierupgrade.soldierid == 0)
        return;
    
    int totaltime = 0;
    for(int i = 0; i < LogicInfo::getSingleton().m_soldierupgrade.nums; ++i)
    {
        if(LogicInfo::getSingleton().m_soldierupgrade.lasttime[i] > 0)
            totaltime += LogicInfo::getSingleton().m_soldierupgrade.lasttime[i];
    }
    
    m_pUpgradeTime->setString(formatTime(totaltime, 3, true).c_str());
}

void LabUI::menuFinishCallback(CCObject *pSender, CCControlEvent event)
{
    int ret = LogicInfo::getSingleton().fastSoldierUpgrade();
    if(ret > 0)
    {
        MessageUI::show(MSG_NOGEM, NULL, NULL, ret, -1);
    }
    
    refreshInfo();
}

void LabUI::menuInfoCallback(CCObject *pSender, CCControlEvent event)
{
    if(LogicInfo::getSingleton().m_showinfo_soldierid > 0)
        return;
 
    int tag = ((CCNode*)pSender)->getTag();
    
    LogicInfo::getSingleton().m_showinfo_soldierid = tag * 100 + 1;
    
    this->setVisible(false);
    PlayerScene::getSingleton().m_pGameWorld->showPlayerLevelUpUI();
}

// a selector callback
void LabUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideLabUI();
    
    if (PlayerScene::getSingleton().m_pCurBuilding) {
        PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->onSelect(true, PlayerScene::getSingleton().m_pCurBuilding);
    }

}

bool LabUI::onNotify(int notifyid, const void* pVal)
{
    if(NOTIFY_BASEINFO == notifyid)
    {
        refreshInfo();
    }
    return true;
}

bool LabUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Title", CCLabelBMFont*, m_pTitle);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UpgradeTime", CCLabelBMFont*, m_pUpgradeTime);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UpgradeName", CCLabelBMFont*, m_pUpgradeName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UpgradeIcon", CCSprite*, m_pUpgradeIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UpgradingNode", CCNode*, m_pUpgradingNode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UpgradeTip", CCNode*, m_pUpgradeTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnFinish", CCControlButton*, m_pBtnFinish);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ItemView", CCLayer*, m_pItemView);
        
    return NULL;
}

SEL_CCControlHandler LabUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", LabUI::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuInfoCallback", LabUI::menuInfoCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuFinishCallback", LabUI::menuFinishCallback);
    
    return NULL;
}