//
//  SkillXZ.cpp
//  GameClient
//
//  Created by chowray on 13-9-26.
//
//

#include "SkillUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/PlayerScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/HeroMgr.h"
#include "../coc/SkillMgr.h"
#include "../coc/HttpClient.h"
#include "../coc/GuideMgr.h"

using namespace cocos2d;

SkillUI::SkillUI()
{
    m_listNums = 12;
    m_selectSkill = 1000100;
}

SkillUI::~SkillUI()
{
    LogicInfo::getSingleton().removeListener(this);
}

bool SkillUI::init()
{
    bool bRet = false;
    
    do{
        
        CC_BREAK_IF(!CUINode::init());
        {
            
            LogicInfo::getSingleton().addListener(this);
            
            CCNode* pSkillUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("hero_skill.ccbi", this);
            this->setTargetTouch(true);
            this->setContentSize(pSkillUI->getContentSize());
            this->addChild(pSkillUI);
            
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("ui_skill_back");
            for (int index = 0; index < 3; index++) {
                setControlButtonFrameForAllState(m_pBtnSkill[index], pFrame);
            }
            
            CCScrollView* pScrollView = CCScrollView::create(m_pItemView->getContentSize());
            pScrollView->setDirection(kCCScrollViewDirectionHorizontal);
            
            CCSize itemSize(120*getResolutionScale(), 194*getResolutionScale());
            float offX = 10;
            float posX = itemSize.width/2 + offX/2;
            
            for (int index = 0; index < m_listNums; index++) {
                
                SkillItem* pSkillItem = SkillItem::create((10001+index) * 100);
                pSkillItem->setPosition(posX + index*(itemSize.width+offX), m_pItemView->getContentSize().height/2);
                
                m_skillList.push_back(pSkillItem);
                pScrollView->addChild(pSkillItem);
            }
            pScrollView->setContentSize(CCSizeMake((itemSize.width+offX)*m_listNums, m_pItemView->getContentSize().height));
            m_pItemView->addChild(pScrollView);
            
            
            if(PlayerScene::getSingleton().m_pGameWorld->m_pHeroUI == NULL)
                m_pBtnBack->setVisible(false);
            
            m_pSkillInfoUI = NULL;
        }
        
        refresh();
        bRet = true;
    }
    while (0);
    
    return bRet;
}

void SkillUI::refresh()
{
    char str[128];
    CCSpriteFrame* pFrame = NULL;
    
    if(HeroMgr::getSingleton().m_myHero.skill0 != 0)
    {
        sprintf(str, "ui_skill_%d", HeroMgr::getSingleton().m_myHero.skill0/100);
        pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pBtnSkill[0]->setTag(HeroMgr::getSingleton().m_myHero.skill0);
    }
    else
    {
        pFrame = ImgMgr::getSingleton().getFrame("ui_skill_back");
        m_pBtnSkill[0]->setTag(0);
    }
    setControlButtonFrameForAllState(m_pBtnSkill[0], pFrame);
    
    if(HeroMgr::getSingleton().m_myHero.skill1 != 0)
    {
        sprintf(str, "ui_skill_%d", HeroMgr::getSingleton().m_myHero.skill1 / 100);
        pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pBtnSkill[1]->setTag(HeroMgr::getSingleton().m_myHero.skill1);
    }
    else
    {
        pFrame = ImgMgr::getSingleton().getFrame("ui_skill_back");
        m_pBtnSkill[1]->setTag(0);
    }
    setControlButtonFrameForAllState(m_pBtnSkill[1], pFrame);
    
    if(HeroMgr::getSingleton().m_myHero.skill2 != 0)
    {
        sprintf(str, "ui_skill_%d", HeroMgr::getSingleton().m_myHero.skill2 / 100);
        pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pBtnSkill[2]->setTag(HeroMgr::getSingleton().m_myHero.skill2);
    }
    else
    {
        pFrame = ImgMgr::getSingleton().getFrame("ui_skill_back");
        m_pBtnSkill[2]->setTag(0);
    }
    setControlButtonFrameForAllState(m_pBtnSkill[2], pFrame);
    
    for(int i = 0; i < SkillMgr::getSingleton().m_skillNums; ++i)
    {
        m_skillList[i]->refresh();
    }
}

void SkillUI::selectSkill(int skill)
{
    int skill0 = HeroMgr::getSingleton().m_myHero.skill0;
    int skill1 = HeroMgr::getSingleton().m_myHero.skill1;
    int skill2 = HeroMgr::getSingleton().m_myHero.skill2;
    
    if (skill == skill0 || skill == skill1 || skill == skill2) {
        return;
    }
    
    CCScaleTo* scaleTo1 = CCScaleTo::create(0.1, m_pBtnSkill[0]->getScale() + 0.2);
    CCScaleTo* scaleTo2 = CCScaleTo::create(0.1, m_pBtnSkill[0]->getScale());
    CCSequence* sequence = CCSequence::create(scaleTo1, scaleTo2, NULL);
    
    if (skill0 <= 0) {
        skill0 = skill;
        HttpClient::getSingleton().selectSkill(skill0, 0);
        m_pBtnSkill[0]->runAction(sequence);
    }
    else if(skill1 <= 0) {
        skill1 = skill;
        HttpClient::getSingleton().selectSkill(skill1, 1);
        m_pBtnSkill[1]->runAction(sequence);
    }
    else if (skill2 <= 0) {
        skill2 = skill;
        HttpClient::getSingleton().selectSkill(skill2, 2);
        m_pBtnSkill[2]->runAction(sequence);
    }
    else{
        skill0 = skill1;
        skill1 = skill2;
        skill2 = skill;
        
        HttpClient::getSingleton().selectSkill(skill0, 0);
        HttpClient::getSingleton().selectSkill(skill1, 1);
        HttpClient::getSingleton().selectSkill(skill2, 2);

        for (int i = 0; i < 2; i++) {
            
            CCMoveTo* moveTo = CCMoveTo::create(0.1, m_pBtnSkill[i]->getPosition());
            
            m_pBtnSkill[i]->setPosition(m_pBtnSkill[i+1]->getPosition());
            m_pBtnSkill[i]->runAction(moveTo);
        }
        
        m_pBtnSkill[2]->runAction(sequence);
    }
    
    refresh();
        
    if(PlayerScene::getSingleton().m_pGameWorld->m_pHeroUI != NULL)
    {
        PlayerScene::getSingleton().m_pGameWorld->m_pHeroUI->refresh();
    }
        
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next();
    }
}

bool SkillUI::onNotify(int notifyid, const void* pVal)
{
    
    return true;
}

void SkillUI::showSkillInfoUI(int skillId, bool isTrain)
{
    if (m_pSkillInfoUI == NULL) {
        
        m_pSkillInfoUI = SkillInfoUI::create(skillId, isTrain);
        this->addChild(m_pSkillInfoUI);
        if(PlayerScene::getSingleton().m_pGameWorld->m_pSkillUI)
            PlayerScene::getSingleton().m_pGameWorld->m_pSkillUI->m_pBtnClose->setVisible(true);
    }
}

void SkillUI::hideSkillInfoUI()
{
    this->removeChild(m_pSkillInfoUI, true);
    m_pSkillInfoUI = NULL;
    if(PlayerScene::getSingleton().m_pGameWorld->m_pSkillUI)
        PlayerScene::getSingleton().m_pGameWorld->m_pSkillUI->m_pBtnClose->setVisible(true);
}

void SkillUI::menuBackCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideSkillUI();
    PlayerScene::getSingleton().m_pGameWorld->m_pHeroUI->setVisible(true);
    
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next();
    }
}

void SkillUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideSkillUI();
    PlayerScene::getSingleton().m_pGameWorld->hideHeroUI();
}

bool SkillUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ItemView", CCLayer*, m_pItemView);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TrainGems", CCLabelBMFont*, m_pTrainGems);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LeapGems", CCLabelBMFont*, m_pLeapGems);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnTrain", CCControlButton*, m_pBtnTrain);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnLeap", CCControlButton*, m_pBtnLeap);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnBack", CCControlButton*, m_pBtnBack);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnClose", CCControlButton*, m_pBtnClose);
    
    char str[128];
    for (int index = 0; index < 3; index++) {
        sprintf(str, "BtnSkill%d", index+1);
        CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, str, CCControlButton*, m_pBtnSkill[index]);
    }
    
    return true;
}

SEL_CCControlHandler SkillUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", SkillUI::menuBackCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", SkillUI::menuCloseCallback);
    
    return NULL;
}

SkillItem* SkillItem::create(int skillId)
{
    SkillItem* pSkillItem = new SkillItem(skillId);
    if (pSkillItem && pSkillItem->init()) {
        pSkillItem->autorelease();
        
        return pSkillItem;
    }
    
    return NULL;
}

SkillItem::SkillItem(int skillId){
    m_skillId = skillId;
}

SkillItem::~SkillItem()
{
    
}

bool SkillItem::init()
{
    CUINode::init();
    
    CCNode* pItem = CUINodeCache::sharedUINodeCache()->UINodeFromFile("skill_item", this);
    this->setContentSize(pItem->getContentSize());
    this->setScale(getResolutionScale());
    this->addChild(pItem);
    
    int index = (m_skillId/100)%10000 - 1;
    int skillId = m_skillId + HeroMgr::getSingleton().m_myHero.bskill[index];
    
    char str[128];
    sprintf(str, "ui_skill_%d", m_skillId/100);
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pBtnSkill->setBackgroundSpriteFrameForState(pFrame, CCControlStateNormal);
    m_pBtnSkill->setBackgroundSpriteFrameForState(pFrame, CCControlStateHighlighted);
    
    sprintf(str, "ui_skill_%d_0", m_skillId/100);
    pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pBtnSkill->setBackgroundSpriteFrameForState(pFrame, CCControlStateDisabled);
    
    const SkillInfo* pInfo = SkillMgr::getSingleton().getNode(skillId);
    if (pInfo) {
        m_pSkillName->setString(pInfo->name.c_str());
    }
    
    if (index >= SkillMgr::getSingleton().m_skillNums) {
        m_pBtnSkill->setEnabled(false);
        m_pBtnSkill->getChildByTag(1)->setVisible(false);
        m_pBtnSkill->getChildByTag(2)->setVisible(false);
        m_pSkillName->setVisible(false);
        m_pSkillLevel->setVisible(false);
        m_pBtnTrain->setEnabled(false);
    }
    
    return true;
}

void SkillItem::refresh()
{
    int index = (m_skillId/100)%10000 - 1;
    int levelId = m_skillId + HeroMgr::getSingleton().m_myHero.bskill[index];
    
    const SkillInfo* pBaseInfo = SkillMgr::getSingleton().getNode(m_skillId+1);
    const SkillInfo* pInfo = SkillMgr::getSingleton().getNode(levelId);
    
    if(HeroMgr::getSingleton().getHeroLevel() >= pBaseInfo->needlevel) //unlock level
    {
        m_pBtnSkill->getChildByTag(1)->setVisible(false);

        char str[128];
        sprintf(str, "Lv.%d", HeroMgr::getSingleton().m_myHero.bskill[index]);
        m_pSkillLevel->setString(str);
    }
    else
    {
        m_pBtnSkill->getChildByTag(1)->setVisible(true);
        m_pSkillLevel->setString("");
        
        m_pBtnTrain->setEnabled(false);
    }

    if (pInfo == NULL || HeroMgr::getSingleton().m_myHero.bskill[index] >= pInfo->maxlevel) {
        m_pBtnTrain->setEnabled(false);
    }
}

void SkillItem::menuInfoCallback(CCObject* pSender, CCControlEvent event)
{
    if (GuideMgr::getSingleton().isEquVal(GUIDE_HERO_SKILL)) {
        return;
    }
    
    PlayerScene::getSingleton().m_pGameWorld->m_pSkillUI->showSkillInfoUI(m_skillId, false);
}

void SkillItem::menuTrainCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->m_pSkillUI->showSkillInfoUI(m_skillId, true);
}

void SkillItem::menuSkillCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->m_pSkillUI->m_selectSkill = m_skillId;
    
    const SkillInfo* pSkillInfo = SkillMgr::getSingleton().getNode(m_skillId+1);
    
    if(HeroMgr::getSingleton().getHeroLevel() >= pSkillInfo->needlevel)
    {
        PlayerScene::getSingleton().m_pGameWorld->m_pSkillUI->selectSkill(m_skillId);
    }
}

bool SkillItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "SkillName", CCLabelBMFont*, m_pSkillName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "SkillLevel", CCLabelBMFont*, m_pSkillLevel);
        
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnSkill", CCControlButton*, m_pBtnSkill);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnTrain", CCControlButton*, m_pBtnTrain);
    
    return true;
}

SEL_CCControlHandler SkillItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuInfoCallback", SkillItem::menuInfoCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuTrainCallback", SkillItem::menuTrainCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSkillCallback", SkillItem::menuSkillCallback);
    
    return NULL;
}
