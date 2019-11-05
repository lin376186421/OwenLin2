//
//  ClanWarHelpUI.cpp
//  GameClient
//
//  Created by qsun on 14-9-18.
//
//

#include "ClanWarHelpUI.h"
#include "../coc/PlayerScene.h"
#include "../GameWorldScene.h"

ClanWarHelpUI::ClanWarHelpUI(int type)
: m_type(type)
, m_pPageNode(NULL)
, m_pageIndex(0)
{
    for (int i = 0; i < 3; i++) {
        m_pBtnPages[i] = NULL;
    }
    
    for (int i = 0; i < 5; i ++) {
        
        m_pBtnHelp[i] = NULL;
        m_pHelpNode[i] = NULL;
    }
}

ClanWarHelpUI::~ClanWarHelpUI()
{
    
}

ClanWarHelpUI* ClanWarHelpUI::create(int type)
{
    ClanWarHelpUI *pRet = new ClanWarHelpUI(type);
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

bool ClanWarHelpUI::init()
{
    if (CUINode::init()) {
        
        CCNode* pHelpUI = NULL;
        if (m_type == 0) {
            pHelpUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_help_bg.ccbi", this);
        }
        else if (m_type == 1) {
            pHelpUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("groupwar_help.ccbi", this);
        }

        this->setContentSize(pHelpUI->getContentSize());
        this->setTargetTouch(true);
        this->addChild(pHelpUI);
        
        addPageNode();
    }
    
    return true;
}

void ClanWarHelpUI::addPageNode()
{
    if (m_pPageNode) {
        this->removeChild(m_pPageNode);
        m_pPageNode = NULL;
    }
    
    if (m_type == 0) {
        
        if (m_pageIndex == 0) {
            m_pPageNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_help_0.ccbi", this);
            this->addChild(m_pPageNode);
        }
        else if (m_pageIndex == 1) {
            m_pPageNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_help_1.ccbi", this);
            this->addChild(m_pPageNode);
        }
    }
    
    refreshPage();
}

void ClanWarHelpUI::refreshPage()
{
    for (int i = 0; i < 2; i ++) {
        if (m_pBtnPages[i]) {
            m_pBtnPages[i]->setEnabled(true);
        }
    }
    
    if (m_pBtnPages[m_pageIndex]) {
        m_pBtnPages[m_pageIndex]->setEnabled(false);
    }
    
    m_pBtnHelp[0]->setEnabled(false);
    m_pHelpNode[0]->setVisible(true);
}

void ClanWarHelpUI::menuHelpCallback(CCObject* pSender, CCControlEvent event)
{
    int index = ((CCNode*)pSender)->getTag();
    
    for (int i = 0; i < 5; i ++) {
        
        if (m_pBtnHelp[i]) {
            m_pBtnHelp[i]->setEnabled(true);
        }
        if (m_pHelpNode[i]) {
            m_pHelpNode[i]->setVisible(false);
        }
    }
    m_pBtnHelp[index]->setEnabled(false);
    m_pHelpNode[index]->setVisible(true);
}

void ClanWarHelpUI::menuPageCallback(CCObject* pSender, CCControlEvent event)
{
    int tag = ((CCNode*)pSender)->getTag();
    
    if (tag != m_pageIndex) {
        m_pageIndex = tag;
        addPageNode();
    }
}

void ClanWarHelpUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->hideClanWarHelpUI();
}

bool ClanWarHelpUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    char str[128];
    
    for (int i = 0; i < 2; i ++) {
        sprintf(str, "BtnPage%d", i);
        CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, str, CCControlButton*, m_pBtnPages[i]);
    }
    
    for (int i = 0; i < 5; i ++) {
        sprintf(str, "BtnHelp%d", i);
        CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, str, CCControlButton*, m_pBtnHelp[i]);
    }
    
    for (int i = 0; i < 5; i ++) {
        sprintf(str, "HelpNode%d", i);
        CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, str, CCNode*, m_pHelpNode[i]);
    }
    
    return false;
}

SEL_CCControlHandler ClanWarHelpUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuPageCallback", ClanWarHelpUI::menuPageCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuHelpCallback", ClanWarHelpUI::menuHelpCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", ClanWarHelpUI::menuCloseCallback);
    
    return NULL;
}