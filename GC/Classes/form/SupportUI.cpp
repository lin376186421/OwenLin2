//
//  SupportUI.cpp
//  GameClient
//
//  Created by qsun on 13-12-3.
//
//

#include "SupportUI.h"
#include "GameWorldScene.h"
#include "../coc/PlayerScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/HttpClient.h"

USING_NS_COC;

SupportUI::SupportUI()
{
    
}

SupportUI::~SupportUI()
{
}

bool SupportUI::init()
{
    if (CUINode::init()) {
        CCNode* pSupportUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("support.ccbi", this);
        
        this->setContentSize(pSupportUI->getContentSize());
        this->setTargetTouch(true);
    
        m_pEmailField = CTextInputField::create(m_pEmailInputBg->getContentSize(), CCScale9Sprite::create());
        m_pEmailField->setAnchorPoint(ccp(0, 0));
        m_pEmailField->setFontColor(ccBLACK);
        m_pEmailField->setInputMode(kTextFieldInputModeEmailAddr);
        m_pEmailInputBg->addChild(m_pEmailField);
        
        m_pDescField = CTextInputField::create(m_pDescInputBg->getContentSize(), CCScale9Sprite::create());
        m_pDescField->setAnchorPoint(ccp(0, 0));
        m_pDescField->setFontSize(20);
        m_pDescField->setFontColor(ccBLACK);
        m_pDescField->setInputMode(kTextFieldInputModeMultiLine);
        m_pDescInputBg->addChild(m_pDescField);
        
        this->addChild(pSupportUI);
        
        string uid = CCStringFormat("%d", LogicInfo::getSingleton().m_myInfo.uid);
        m_pUniqueId->setString(uid.c_str());
        
        m_pBtnForum->setVisible(false);
    }
    
    return true;
}

void SupportUI::menuForumCallback(CCObject *pSender, CCControlEvent event)
{
    
}

void SupportUI::menuSubmitCallback(CCObject *pSender, CCControlEvent event)
{
    if (strlen(m_pEmailField->getText()) > 0 && strlen(m_pDescField->getText()) > 0) {
        HttpClient::getSingleton().addBug(m_pDescField->getText(), m_pEmailField->getText());
        
        MessageUI::show(MSG_NORMAL, ViewString::getValue("bug_submit_title").c_str(), ViewString::getValue("bug_submit_message").c_str());
        
        PlayerScene::getSingleton().m_pGameWorld->hideSupportUI();
    }
}

void SupportUI::menuBackCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideSupportUI();
}

bool SupportUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UniqueId", CCLabelBMFont*, m_pUniqueId);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "EmailInputBg", CCScale9Sprite*, m_pEmailInputBg);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DescInputBg", CCScale9Sprite*, m_pDescInputBg);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnForum", CCControlButton*, m_pBtnForum);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnSubmit", CCControlButton*, m_pBtnSubmit);
    return false;
}

SEL_CCControlHandler SupportUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuForumCallback", SupportUI::menuForumCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSubmitCallback", SupportUI::menuSubmitCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", SupportUI::menuBackCallback);

    return NULL;
}

AnnouncementUI::AnnouncementUI()
{
    
}

AnnouncementUI::~AnnouncementUI()
{
    
}
void AnnouncementUI::onExit()
{
    CAppPlatform::sharedAppPlatform()->hideAnnouncement();
    CUINode::onExit();
}
void AnnouncementUI::onEnter()
{
    CUINode::onEnter();
}
bool AnnouncementUI::init()
{
    if (CUINode::init()) {
        CCNode* pSupportUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("announcement.ccbi", this);
        
        this->setContentSize(pSupportUI->getContentSize());
        this->setTargetTouch(true);
        this->addChild(pSupportUI);
    
        m_pLoading = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading1.ccbi", this);
        m_pLoading->setPosition(ccp(m_pWebNode->getContentSize().width/2, m_pWebNode->getContentSize().height/2));
        m_pWebNode->addChild(m_pLoading);
        
        //强制性公告 隐藏按钮
        if(LogicInfo::getSingleton().m_announcementType == ANNOUNCEMENT_TYPE_FORCE)
            m_pBackBtn->setVisible(false);
    }
    
    return true;
}
void AnnouncementUI::showAnnouncementWebView()
{
    CAppPlatform::sharedAppPlatform()->showAnnouncement();
}

void AnnouncementUI::menuBackCallback(CCObject* pSender, CCControlEvent event)
{
    if(PlayerScene::getSingleton().m_pGameWorld == NULL)
    {
        removeFromParentAndCleanup(true);
    }
    else
    {
        PlayerScene::getSingleton().m_pGameWorld->hideAnnouncementUI();
    }
}

bool AnnouncementUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BackBtn", CCControlButton*, m_pBackBtn);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WebNode", CCNode*, m_pWebNode);
    return false;
}

SEL_CCControlHandler AnnouncementUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", AnnouncementUI::menuBackCallback);
    
    return NULL;
}
