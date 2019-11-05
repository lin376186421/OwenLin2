//
//  PlayerUpLevelUI.cpp
//  GameClient
//
//  Created by chowray on 13-5-27.
//
//

#include "PlayerUpLevelUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/PlayerScene.h"
#include "../coc/Building.h"
#include "../coc/BuildingMgr.h"
#include "../coc/BaseDef.h"
#include "../coc/LogicInfo.h"
#include "../coc/PersonMgr.h"
#include "../coc/HeroMgr.h"

using namespace cocos2d;
USING_NS_COC;

PlayerUpLevelUI::PlayerUpLevelUI()
{
    
}

PlayerUpLevelUI::~PlayerUpLevelUI()
{
    
}

bool PlayerUpLevelUI::init()
{
    bool bRet = false;
    
    do{
        
        CC_BREAK_IF(!CUINode::init());
        
        CCNode* upgradeUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("upgrade_item_1.ccbi", this);
        
        this->setTargetTouch(true);
        this->setContentSize(upgradeUI->getContentSize());
        this->addChild(upgradeUI);
        
        
        
        refreshInfo();

        char str[128];
        
        int soldierId = LogicInfo::getSingleton().getMySoldierLevel(LogicInfo::getSingleton().m_showinfo_soldierid);
        
        const SoldierInfo* info = PersonMgr::getSingleton().getSoldierInfo(soldierId);
        
        const SoldierInfo* info2 = PersonMgr::getSingleton().getSoldierInfo(soldierId+1);
        
        const SoldierInfo* maxInfo = PersonMgr::getSingleton().getSoldierMaxInfo(soldierId);
        
        float percent = (float)info2->damageex / maxInfo->damageex;
        changeProgress((CCScale9Sprite*)m_pBuildingAtt1->getChildByTag(ATT_TAG_PROGRESS_ADD), percent);
        
        sprintf(str, ViewString::getSingleton().getValue("playeruplevel_damage").c_str(), abs((int)info->damageex), abs((int)(info2->damageex-info->damageex)));
        ((CCLabelBMFont*)m_pBuildingAtt1->getChildByTag(ATT_TAG_VALUE))->setString(str);
        
        percent = (float)info2->hpex / maxInfo->hpex;
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS_ADD), percent);
        
        sprintf(str, ViewString::getSingleton().getValue("playeruplevel_hitpoint").c_str(), (int)info->hpex, (int)(info2->hpex-info->hpex));
        ((CCLabelBMFont*)m_pBuildingAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
        
        percent = (float)info2->trainingCost / maxInfo->trainingCost;
        changeProgress((CCScale9Sprite*)m_pBuildingAtt3->getChildByTag(ATT_TAG_PROGRESS_ADD), percent);
        
        sprintf(str, ViewString::getSingleton().getValue("playeruplevel_trainingcost").c_str(), (int)info->trainingCost, info2->trainingCost-info->trainingCost);
        ((CCLabelBMFont*)m_pBuildingAtt3->getChildByTag(ATT_TAG_VALUE))->setString(str);
        
        int totleTime = info2->upgradeTimeHour * 3600;
                
        m_pUpgradeTime->setString(formatTime(totleTime).c_str());
        
        sprintf(str, "%d", (int)info2->upgradeCost);
        CCLabelBMFont* moneyLabel = ((CCLabelBMFont*)m_pBtnUpgrade->getChildByTag(1));
        moneyLabel->setString(str);
        
        if(info2->upgradeCost > LogicInfo::getSingleton().getMoney(MONEY_ELIXIR))
        {
            moneyLabel->setColor(ccRED);
        }
        m_pDesc->setVisible(false);
        
        if (LogicInfo::getSingleton().getCurUpgradeSoldier() / 100 == soldierId / 100) {
            m_pBtnUpgrade->setEnabled(false);
        }
        
        bRet = true;
    }
    while (0);
    
    m_pBuildingAtt1->getChildByTag(ATT_TAG_PROGRESS_ADD)->setVisible(true);
    m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS_ADD)->setVisible(true);
    m_pBuildingAtt3->getChildByTag(ATT_TAG_PROGRESS_ADD)->setVisible(true);
    
    return bRet;
}

// a selector callback
void PlayerUpLevelUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    LogicInfo::getSingleton().m_showinfo_soldierid = 0;
	PlayerScene::getSingleton().m_pGameWorld->hidePlayerLevelUpUI();
}

void PlayerUpLevelUI::menuBackCallback(CCObject *pSender, CCControlEvent event)
{
    
    LogicInfo::getSingleton().m_showinfo_soldierid = 0;
    PlayerScene::getSingleton().m_pGameWorld->hidePlayerLevelUpUI();
    
}

void PlayerUpLevelUI::menuUpgradeCallback(CCObject* pSender, CCControlEvent event)
{
    const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(LogicInfo::getSingleton().getMySoldierLevel(LogicInfo::getSingleton().m_showinfo_soldierid) + 1);
    if(pInfo->upgradeCost > LogicInfo::getSingleton().m_myInfo.maxoilex)
    {
        PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI->addNote(ViewString::getSingleton().getValue("uplevel_error_oil").c_str());
        return;
    }
    
	int need = 0;
    int ret = LogicInfo::getSingleton().addUpgradeSoldier(LogicInfo::getSingleton().getMySoldierLevel(LogicInfo::getSingleton().m_showinfo_soldierid) + 1, need);
    
    if(ret == 1)
    {
        LogicInfo::getSingleton().m_showinfo_soldierid = 0;
        PlayerScene::getSingleton().m_pGameWorld->hidePlayerLevelUpUI();
    }
    else if(ret == MSG_NOELIXIR)
    {
        MessageUI::show(ret, "", "", need, GETGEM_UPLEVEL_SOLDIER);
    }
    
    PlayerScene::getSingleton().m_pGameWorld->m_pLabUI->refreshInfo();
}

bool PlayerUpLevelUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    SoldierUI::onAssignCCBMemberVariable(pTarget, pMemberVariableName, pNode);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UpgradeTime", CCLabelBMFont*, m_pUpgradeTime);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnUpgrade", CCControlButton*, m_pBtnUpgrade);
    return true;
}

SEL_CCControlHandler PlayerUpLevelUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", PlayerUpLevelUI::menuBackCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", PlayerUpLevelUI::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuUpgradeCallback", PlayerUpLevelUI::menuUpgradeCallback);
    
    return NULL;
}
