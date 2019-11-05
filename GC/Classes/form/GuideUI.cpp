//
//  GuideUI.cpp
//  GameClient
//
//  Created by chowray on 13-6-27.
//
//

#include "GuideUI.h"
#include "../coc/PlayerScene.h"
#include "../coc/GuideMgr.h"
#include "../coc/HeroMgr.h"
#include "GameWorldScene.h"

using namespace cocos2d;
using namespace coc;

GuideUI::GuideUI()
: m_info(NULL)
, m_pBtnOK(NULL)
, m_pAnimationManager(NULL)
{
    
}

GuideUI::~GuideUI()
{
    if (m_pAnimationManager) {
        m_pAnimationManager->setDelegate(NULL);
    }
}

bool GuideUI::init()
{
    if (CUINode::init())
    {
        CCSize winSize = CCDirector::sharedDirector()->getWinSize();
        
        this->setTargetTouch(true);
        this->setTouchEnabled(true);
        this->setContentSize(winSize);
        
        setChat();
    }
    
    return true;
}

bool GuideUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    return true;
}

void GuideUI::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

void GuideUI::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    if (GuideMgr::getSingleton().isGuideEnd()) {
        PlayerScene::getSingleton().m_pGameWorld->hideGuideUI();
        return;
    }
    
    if (m_pBtnOK == NULL || !m_pBtnOK->isVisible() || m_pBtnOK->getScale() == 0) {
        if (m_pAnimationManager->getRunningSequenceName() == NULL && !m_info->animOut.empty()) {
            m_pAnimationManager->runAnimationsForSequenceNamed(m_info->animOut.c_str());
        }
        
        if (GuideMgr::getSingleton().isEquVal(GUIDE_ATTACK_GOBLIN, 20) || GuideMgr::getSingleton().isEquVal(GUIDE_RAVEN_GOBLIN, 30)) {
            PlayerScene::getSingleton().m_pGameWorld->hideGuideUI();
            GuideMgr::getSingleton().next();
        }
    }
}

void GuideUI::menuOKCallback(CCObject* pSender, CCControlEvent cce)
{
    PlayerScene::getSingleton().m_pGameWorld->hideGuideUI();
    GuideMgr::getSingleton().next();
}

void GuideUI::setChat()
{
    if(GuideMgr::getSingleton().isEquVal(GUIDE_NEW_WELCOME, 0))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("1_0");
    }
    else if(GuideMgr::getSingleton().isEquVal(GUIDE_NEW_WELCOME, 10))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("1_1");
    }
    else if(GuideMgr::getSingleton().isEquVal(GUIDE_NEW_WELCOME, 20))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("1_2");
    }
    else if(GuideMgr::getSingleton().isEquVal(GUIDE_BUILD_CANNON, 0))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("2_0");
    }
    else if (GuideMgr::getSingleton().isEquVal(GUIDE_BUILD_CANNON, 40))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("2_1");
    }
    else if (GuideMgr::getSingleton().isEquVal(GUIDE_DEFEND_TOWNHALL, 0))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("3_0");
    }
    else if (GuideMgr::getSingleton().isEquVal(GUIDE_DEFEND_TOWNHALL, 10))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("3_1");
    }
    else if (GuideMgr::getSingleton().isEquVal(GUIDE_DEFEND_TOWNHALL, 30))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("3_2");
    }
    else if (GuideMgr::getSingleton().isEquVal(GUIDE_DEFEND_TOWNHALL, 40))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("3_3");
    }
    else if(GuideMgr::getSingleton().isEquVal(GUIDE_ATTACK_GOBLIN, 0)) //attack goblin
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("4_0");
    }
    else if (GuideMgr::getSingleton().isEquVal(GUIDE_ATTACK_GOBLIN, 20))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("4_1");
    }
    else if (GuideMgr::getSingleton().isEquVal(GUIDE_ATTACK_GOBLIN, 40))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("4_2");
    }
    else if (GuideMgr::getSingleton().isEquVal(GUIDE_ADD_BUILDER, 0))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("5_0");
    }
    else if (GuideMgr::getSingleton().isEquVal(GUIDE_BUILD_ELIXIR_COLLECT, 0))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("6_0");
    }
    else if (GuideMgr::getSingleton().isEquVal(GUIDE_BUILD_ELIXIR_COLLECT, 10))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("6_1");
    }
    else if (GuideMgr::getSingleton().isEquVal(GUIDE_BUILD_ELIXIR_STORAGE, 0))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("7_0");
    }
    else if (GuideMgr::getSingleton().isEquVal(GUIDE_BUILD_ELIXIR_STORAGE, 10))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("7_1");
    }
    else if(GuideMgr::getSingleton().isEquVal(GUIDE_BUILD_GOLD_STORAGE, 0))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("8_0");
    }
    else if(GuideMgr::getSingleton().isEquVal(GUIDE_BUILD_GOLD_STORAGE, 70))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("8_1");
    }
    else if(GuideMgr::getSingleton().isEquVal(GUIDE_BUILD_BARRACK, 0))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("9_0");
    }
    else if(GuideMgr::getSingleton().isEquVal(GUIDE_TRAIN_SOLDIER, 0))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("10_0");
    }
    else if(GuideMgr::getSingleton().isEquVal(GUIDE_RAVEN_GOBLIN, 0))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("11_0");
    }
    else if(GuideMgr::getSingleton().isEquVal(GUIDE_RAVEN_GOBLIN, 30))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("11_1");
    }
    else if(GuideMgr::getSingleton().isEquVal(GUIDE_MAKE_NAME, 0))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("12_0");
    }
    else if(GuideMgr::getSingleton().isEquVal(GUIDE_UPGRADE_TOWNHALL, 0))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("13_0");
    }
    else if(GuideMgr::getSingleton().isEquVal(GUIDE_UPGRADE_TOWNHALL, 60))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("13_1");
    }
    else if (GuideMgr::getSingleton().isEquVal(GUIDE_ACHIEVEMENT, 0))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("14_0");
    }
    else if (GuideMgr::getSingleton().isEquVal(GUIDE_FINAL_HINT, 0))
    {
        m_info = GuideMgr::getSingleton().getGuideItemInfo("99");
    }
    
    if (m_info) {
        
        CCNode* pGuideUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile(m_info->file.c_str(), this, &m_pAnimationManager);
        this->setContentSize(pGuideUI->getContentSize());
        m_pAnimationManager->setDelegate(this);
        if (!m_info->tip.empty()) {
            m_pTip->setString(ViewString::getValue(m_info->tip.c_str()).c_str());
        }
        if (!m_info->animIn.empty()) {
            m_pAnimationManager->runAnimationsForSequenceNamed(m_info->animIn.c_str());
        }
        
        this->addChild(pGuideUI);
    }
}

bool GuideUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Tip", CCLabelBMFont*, m_pTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnOK", CCControlButton*, m_pBtnOK);
    
    return NULL;
}

SEL_CCControlHandler GuideUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuOKCallback", GuideUI::menuOKCallback);
    return NULL;
}

void GuideUI::completedAnimationSequenceNamed(const char *name)
{
    if (strcmp(name, m_info->animOut.c_str()) == 0) {
        PlayerScene::getSingleton().m_pGameWorld->hideGuideUI();
        GuideMgr::getSingleton().next();
    }
}
