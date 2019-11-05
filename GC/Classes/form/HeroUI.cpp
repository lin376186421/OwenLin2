//
//  HeroUI.cpp
//  GameClient
//
//  Created by chowray on 13-5-21.
//
//

#include "HeroUI.h"
#include "InfoUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/PlayerScene.h"
#include "../coc/Building.h"
#include "../coc/BuildingMgr.h"
#include "../coc/BaseDef.h"
#include "../coc/LogicInfo.h"
#include "../coc/HeroMgr.h"
#include "../coc/SkillMgr.h"

#include "../coc/PersonMgr.h"
#include "../coc/GuideMgr.h"

using namespace cocos2d;
USING_NS_COC;

HeroUI::HeroUI()
{
    m_isHeroUplevel = false;
    if(LogicInfo::getSingleton().m_showinfo_soldierid == HeroMgr::getSingleton().m_myHero.heroid)
        m_isHeroUplevel = true;
    
    m_pUpgradeItemNode = NULL;
}

HeroUI::~HeroUI()
{
    LogicInfo::getSingleton().m_showinfo_soldierid = 0;
    LogicInfo::getSingleton().removeListener(this);
}

bool HeroUI::init()
{
    bool bRet = false;
    
    do{
        
        CC_BREAK_IF(!CUINode::init());
        
        {
            LogicInfo::getSingleton().addListener(this);
            
            
            
            CCNode* pHeroUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("hero_info.ccbi", this);
            this->setTargetTouch(true);
            this->setContentSize(pHeroUI->getContentSize());
            this->addChild(pHeroUI);
            
            
            CCNode *bgNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("upgrade_item_2.ccbi", this);
            this->addChild(bgNode);
            
            m_pUpgradeItemNode = bgNode;
            
            if(m_isHeroUplevel == false)
                bgNode->setVisible(false);
            else
                bgNode->setVisible(true);
            
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("ui_skill_back");
            m_pSkill1->setDisplayFrame(pFrame);
            m_pSkill2->setDisplayFrame(pFrame);
            m_pSkill3->setDisplayFrame(pFrame);
            m_pSkill4->setDisplayFrame(pFrame);
            
            m_selectSkill = 1;
            
            m_pUnUpgradeTip->setVisible(false);
            char str[128];
            
            if (!HeroMgr::getSingleton().canLevelUp() && m_isHeroUplevel) {
                
                int townLevel = LogicInfo::getSingleton().m_myBaseLevel;
                const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(HeroMgr::getSingleton().m_myHero.heroid + 1);
                if(pInfo == NULL)
                {
//                    m_bMaxLevel = true;
                }
                else if(pInfo->needtownhall >= townLevel)
                {
                    sprintf(str, ViewString::getValue("hero_unupgrade_tip").c_str(), pInfo->needtownhall);
                    ((CCLabelBMFont*)m_pUnUpgradeTip->getChildByTag(1))->setString(str);
                    
                    m_pUnUpgradeTip->setVisible(true);
                    m_pHeroInfoLabel->setVisible(false);
                    m_pBtnUpgrade->setEnabled(false);
                } else
                {
                    m_pUnUpgradeTip->setVisible(false);
                    m_pHeroInfoLabel->setVisible(true);
                }
            }
        }
        
        if(HeroMgr::getSingleton().m_heroState == HERO_STATE_UPGRADING && !m_isHeroUplevel)
        {
            m_pBtnChange->setEnabled(false);
            m_pBtnRefresh->setEnabled(false);
        }
        
        refresh();
        
        
        
        bRet = true;
    }
    while (0);
    
    return bRet;
}

void HeroUI::onEnter()
{
    CUINode::onEnter();
    refresh();
}

void HeroUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
	PlayerScene::getSingleton().m_pGameWorld->hideHeroUI();
    
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next();
    }
}

void HeroUI::menuChangeCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->showSkillUI();
    PlayerScene::getSingleton().m_pGameWorld->m_pHeroUI->setVisible(false);
    
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next();
    }
}

void HeroUI::menuRefreshCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->showSkillGlyphUI();
    
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next();
    }
}

void HeroUI::menuSkillCallback(CCObject *pSender, CCControlEvent event)
{
    CCLOG("touch %d",event);
    int tag = ((CCNode*)pSender)->getTag();
    m_selectSkill = tag;
    
    const SkillInfo* pSkillInfo;
    pSkillInfo = NULL;
    if (m_selectSkill == 1 && HeroMgr::getSingleton().m_myHero.skill0 != 0) {
        pSkillInfo = SkillMgr::getSingleton().getNode(HeroMgr::getSingleton().m_myHero.skill0+1);
    }else if(m_selectSkill == 2 && HeroMgr::getSingleton().m_myHero.skill1 != 0) {
        pSkillInfo = SkillMgr::getSingleton().getNode(HeroMgr::getSingleton().m_myHero.skill1+1);
    }else if(m_selectSkill == 3 && HeroMgr::getSingleton().m_myHero.skill2 != 0) {
        pSkillInfo = SkillMgr::getSingleton().getNode(HeroMgr::getSingleton().m_myHero.skill2+1);
    }
    else if(m_selectSkill == 4 && HeroMgr::getSingleton().m_myHero.skill3 != 0) {
        pSkillInfo = SkillMgr::getSingleton().getNode(HeroMgr::getSingleton().m_myHero.skill3 + 1);
    }

    
    if (pSkillInfo != NULL)
    {
        refresh();
    }
    else
    {
        m_pHeroIcon->setVisible(true);
        m_pSkillName->getParent()->setVisible(false);
        m_pGlyphName->getParent()->setVisible(false);
        return;
    }
    
    bool touching = true;
    if(event == CCControlEventTouchDown)
        touching = true;
    else if (event >= CCControlEventTouchUpInside || event == CCControlEventTouchDragOutside )
        touching = false;
    
    if(touching)
    {
        m_pHeroIcon->setVisible(false);
        
        if(m_isHeroUplevel == true)
            m_pUpgradeItemNode->setVisible(false);
//        m_p
        if(tag < 4)
        {
            m_pSkillName->getParent()->setVisible(true);
            m_pGlyphName->getParent()->setVisible(false);
        }
        else
        {
            m_pSkillName->getParent()->setVisible(false);
            m_pGlyphName->getParent()->setVisible(true);
        }
    }
    else
    {
        m_pHeroIcon->setVisible(true);
        if(m_isHeroUplevel == true)
            m_pUpgradeItemNode->setVisible(true);
        m_pSkillName->getParent()->setVisible(false);
        m_pGlyphName->getParent()->setVisible(false);
    }
    
    
    
    
}

void HeroUI::refresh()
{
    char str[128];
    
    sprintf(str, "hero_%d", HeroMgr::getSingleton().m_myHero.heroid / 100);
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pHeroIcon->setDisplayFrame(pFrame);
    
    
    const SoldierInfo* info = PersonMgr::getSingleton().getSoldierInfo(HeroMgr::getSingleton().m_myHero.heroid);
  
    
    if (!m_isHeroUplevel) {
        if(HeroMgr::getSingleton().m_heroState == HERO_STATE_UPGRADING)
            info = PersonMgr::getSingleton().getSoldierInfo(HeroMgr::getSingleton().m_myHero.heroid-1);
        
        sprintf(str, "Commander");
        m_pLevel->setString(str);
        
        sprintf(str, "HP: %d/%d", HeroMgr::getSingleton().m_myHero.hp, HeroMgr::getSingleton().m_myHero.maxhp);
        ((CCLabelBMFont*)m_pHeroAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
        changeProgress(((CCScale9Sprite*)m_pHeroAtt2->getChildByTag(ATT_TAG_PROGRESS)), (float)HeroMgr::getSingleton().m_myHero.hp/(float)HeroMgr::getSingleton().m_myHero.maxhp);
        
        sprintf(str, "HP: %d/%d", HeroMgr::getSingleton().m_myHero.hp, HeroMgr::getSingleton().m_myHero.maxhp);
        ((CCLabelBMFont*)m_pHeroAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
        
        sprintf(str, ViewString::getSingleton().getValue("playerupinfo_damage").c_str(), (int)info->damageex);
        ((CCLabelBMFont*)m_pHeroAtt3->getChildByTag(ATT_TAG_VALUE))->setString(str);
    }
    else {
        sprintf(str, ViewString::getSingleton().getValue("upgrade_title").c_str(), (HeroMgr::getSingleton().m_myHero.heroid % 100) + 1);
        m_pLevel->setString(str);
        
        const SoldierInfo* info2 = PersonMgr::getSingleton().getSoldierInfo(HeroMgr::getSingleton().m_myHero.heroid+1);
        if(info2 == NULL)
            m_pBtnUpgrade->setVisible(false);
        const SoldierInfo* maxInfo = PersonMgr::getSingleton().getSoldierMaxInfo(HeroMgr::getSingleton().m_myHero.heroid);
        
        
        sprintf(str, ViewString::getSingleton().getValue("playeruplevel_hitpoint").c_str(), (int)info->hpex, (int)(info2->hpex-info->hpex));
        ((CCLabelBMFont*)m_pHeroAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
        
        float percent = (float)info->hpex / maxInfo->hpex;
        changeProgress(((CCScale9Sprite*)m_pHeroAtt2->getChildByTag(ATT_TAG_PROGRESS)), percent);
        
        percent = (float)info2->hpex / maxInfo->hpex;
        changeProgress(((CCScale9Sprite*)m_pHeroAtt2->getChildByTag(ATT_TAG_PROGRESS_ADD)), percent);
        
       
        
        sprintf(str, ViewString::getSingleton().getValue("playeruplevel_damage").c_str(), (int)info->damageex, (int)(info2->damageex-info->damageex));
        ((CCLabelBMFont*)m_pHeroAtt3->getChildByTag(ATT_TAG_VALUE))->setString(str);
        
        percent = (float)info->damageex / maxInfo->damageex;
        changeProgress(((CCScale9Sprite*)m_pHeroAtt3->getChildByTag(ATT_TAG_PROGRESS)), percent);
        
        percent = (float)info2->damageex / maxInfo->damageex;
        changeProgress(((CCScale9Sprite*)m_pHeroAtt3->getChildByTag(ATT_TAG_PROGRESS_ADD)), percent);
        
        
        int totleTime = info2->upgradeTimeHour * 3600;
        
        m_pUpgradeTime->setString(formatTime(totleTime).c_str());
        
        sprintf(str, "%d", (int)info2->upgradeCost);
        CCLabelBMFont* moneyLabel = ((CCLabelBMFont*)m_pBtnUpgrade->getChildByTag(1));
        moneyLabel->setString(str);
        
        if(info2->upgradeCost > LogicInfo::getSingleton().getMoney(info2->upgradeRes))
        {
            moneyLabel->setColor(ccRED);
        }
        
        
        if(info2->upgradeRes == MONEY_GOLD)
        {
            CCSpriteFrame* pSpriteFrame = ImgMgr::getSingleton().getFrame("map_Coins_icon");
            ((CCSprite*)m_pBtnUpgrade->getChildByTag(2))->setDisplayFrame(pSpriteFrame);
            
            if(info2->upgradeCost > LogicInfo::getSingleton().m_myInfo.maxgoldex)
            {
                ((CCLabelBMFont*)m_pUnUpgradeTip->getChildByTag(1))->setString(ViewString::getValue("uplevel_error_gold").c_str());
                
                m_pUnUpgradeTip->setVisible(true);
                m_pHeroInfoLabel->setVisible(false);
                m_pBtnUpgrade->setEnabled(false);
            }
        }
        else if(info2->upgradeRes == MONEY_ELIXIR)
        {
            CCSpriteFrame* pSpriteFrame = ImgMgr::getSingleton().getFrame("map_Elixir_icon");
            ((CCSprite*)m_pBtnUpgrade->getChildByTag(2))->setDisplayFrame(pSpriteFrame);
            if(info2->upgradeCost > LogicInfo::getSingleton().m_myInfo.maxoilex)
            {
                ((CCLabelBMFont*)m_pUnUpgradeTip->getChildByTag(1))->setString(ViewString::getValue("uplevel_error_oil").c_str());
                
                m_pUnUpgradeTip->setVisible(true);
                m_pHeroInfoLabel->setVisible(false);
                m_pBtnUpgrade->setEnabled(false);
            }
        }
        
        
    }

    if(HeroMgr::getSingleton().m_myHero.skill0 != 0)
    {
        sprintf(str, "ui_skill_%d", HeroMgr::getSingleton().m_myHero.skill0/100);
        pFrame = ImgMgr::getSingleton().getFrame(str);
        if(pFrame != NULL)
            m_pSkill1->setDisplayFrame(pFrame);
        int index = (HeroMgr::getSingleton().m_myHero.skill0/100)%10000 - 1;
        sprintf(str, "Lv.%d", HeroMgr::getSingleton().m_myHero.bskill[index]);
        ((CCLabelBMFont *)m_pSkill1->getChildren()->objectAtIndex(0))->setString(str);
    }
    else
    {
        ((CCLabelBMFont *)m_pSkill1->getChildren()->objectAtIndex(0))->setVisible(false);
    }
    
    if(HeroMgr::getSingleton().m_myHero.skill1 != 0)
    {
        sprintf(str, "ui_skill_%d", HeroMgr::getSingleton().m_myHero.skill1/100);
        pFrame = ImgMgr::getSingleton().getFrame(str);
        if(pFrame != NULL)
            m_pSkill2->setDisplayFrame(pFrame);
        
        int index = (HeroMgr::getSingleton().m_myHero.skill1/100)%10000 - 1;
        sprintf(str, "Lv.%d", HeroMgr::getSingleton().m_myHero.bskill[index]);
        ((CCLabelBMFont *)m_pSkill2->getChildren()->objectAtIndex(0))->setString(str);
    }
    else
    {
        ((CCLabelBMFont *)m_pSkill2->getChildren()->objectAtIndex(0))->setVisible(false);
    }
    
    if(HeroMgr::getSingleton().m_myHero.skill2 != 0)
    {
        sprintf(str, "ui_skill_%d", HeroMgr::getSingleton().m_myHero.skill2/100);
        pFrame = ImgMgr::getSingleton().getFrame(str);
        if(pFrame != NULL)
            m_pSkill3->setDisplayFrame(pFrame);
        
        int index = (HeroMgr::getSingleton().m_myHero.skill2/100)%10000 - 1;
        sprintf(str, "Lv.%d", HeroMgr::getSingleton().m_myHero.bskill[index]);
        ((CCLabelBMFont *)m_pSkill3->getChildren()->objectAtIndex(0))->setString(str);
    }
    else
    {
        ((CCLabelBMFont *)m_pSkill3->getChildren()->objectAtIndex(0))->setVisible(false);
    }
    
    const SkillInfo* pSkillInfo = SkillMgr::getSingleton().getNode(HeroMgr::getSingleton().m_myHero.skill3 + 1);
    if(pSkillInfo != NULL)
    {
        sprintf(str, "ui_skill_%d", HeroMgr::getSingleton().m_myHero.skill3/100);
        pFrame = ImgMgr::getSingleton().getFrame(str);
        if(pFrame != NULL)
            m_pSkill4->setDisplayFrame(pFrame);
        
        ((CCLabelBMFont *)m_pSkill4->getChildren()->objectAtIndex(0))->setVisible(false);

        
        m_pGlyphName->setString(pSkillInfo->name.c_str());
        m_pGlyphDesc->setString(pSkillInfo->info.c_str());
    }
    else
    {
        ((CCLabelBMFont *)m_pSkill4->getChildren()->objectAtIndex(0))->setVisible(false);
    }
    
    pSkillInfo = NULL;
    if (m_selectSkill == 1 && HeroMgr::getSingleton().m_myHero.skill0 != 0) {
        pSkillInfo = SkillMgr::getSingleton().getNode(HeroMgr::getSingleton().m_myHero.skill0+1);
    }else if(m_selectSkill == 2 && HeroMgr::getSingleton().m_myHero.skill1 != 0) {
        pSkillInfo = SkillMgr::getSingleton().getNode(HeroMgr::getSingleton().m_myHero.skill1+1);
    }else if(m_selectSkill == 3 && HeroMgr::getSingleton().m_myHero.skill2 != 0) {
        pSkillInfo = SkillMgr::getSingleton().getNode(HeroMgr::getSingleton().m_myHero.skill2+1);
    }

    if (pSkillInfo != NULL) {
        m_pSkillName->setString(pSkillInfo->name.c_str());
        m_pSkillDesc->setString(pSkillInfo->info.c_str());
    }
}

void HeroUI::menuUpgradeCallback(CCObject* pSender, CCControlEvent event)
{
    int need = 0;
    int ret = HeroMgr::getSingleton().upLevelHero(need);
    if(ret <= 0)
    {
        MessageUI::show(ret, NULL, NULL, need, GETGEM_UPLEVEL_HERO);
    }
    else
    {
        LogicInfo::getSingleton().m_showinfo_soldierid = 0;
        PlayerScene::getSingleton().m_pGameWorld->hideHeroUI();
    }
}

bool HeroUI::onNotify(int notifyid, const void* pVal)
{
	if(NOTIFY_HERO == notifyid)
	{
        refresh();
	}
    
    return true;
}

bool HeroUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "HeroIcon", CCSprite*, m_pHeroIcon);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Level", CCLabelBMFont*, m_pLevel);
//    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "HeroAtt1", CCNode*, m_pHeroAtt1);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "HeroAtt2", CCNode*, m_pHeroAtt2);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "HeroAtt3", CCNode*, m_pHeroAtt3);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GlyphInfoNode", CCNode*, m_pGlyphInfoNode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "SkillInfoNode", CCNode*, m_pSkillInfoNode);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Skill1", CCSprite*, m_pSkill1);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Skill2", CCSprite*, m_pSkill2);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Skill3", CCSprite*, m_pSkill3);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Skill4", CCSprite*, m_pSkill4);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GlyphName", CCLabelBMFont*, m_pGlyphName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GlyphDesc", CCLabelBMFont*, m_pGlyphDesc);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "SkillName", CCLabelBMFont*, m_pSkillName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "SkillDesc", CCLabelBMFont*, m_pSkillDesc);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "HeroInfoLabel", CCLabelBMFont*, m_pHeroInfoLabel);

    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnChange", CCControlButton*, m_pBtnChange);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnRefresh", CCControlButton*, m_pBtnRefresh);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UnUpgradeTip", CCNode*, m_pUnUpgradeTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UpgradeTip", CCNode*, m_pUpgradeTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UpgradeTime", CCLabelBMFont*, m_pUpgradeTime);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnUpgrade", CCControlButton*, m_pBtnUpgrade);
    

    return true;
}

SEL_CCControlHandler HeroUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSkillCallback", HeroUI::menuSkillCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuChangeCallback", HeroUI::menuChangeCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuRefreshCallback", HeroUI::menuRefreshCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", HeroUI::menuCloseCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuUpgradeCallback", HeroUI::menuUpgradeCallback);
    
    return NULL;
}
