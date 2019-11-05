//
//  ClanEditUI.cpp
//  GameClient
//
//  Created by qsun on 14-3-13.
//
//

#include "ClanEditUI.h"
#include "MessageUI.h"
#include "../coc/ImgMgr.h"
#include "../coc/PlayerScene.h"
#include "../coc/ClanMgr.h"
#include "../GameWorldScene.h"


ClanEditUI::ClanEditUI()
: m_pIconEditUI(NULL)
, m_pNationalEditUI(NULL)
, m_pWaitLoading(NULL)
{
    LogicInfo::getSingleton().addListener(this);
}

ClanEditUI::~ClanEditUI()
{
    LogicInfo::getSingleton().removeListener(this);
}

bool ClanEditUI::init()
{
    if (CUINode::init()) {
        
        m_pClanEditBg = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_edit_bg.ccbi", this);
        this->setContentSize(m_pClanEditBg->getContentSize());
        this->addChild(m_pClanEditBg);
        
        m_pEditUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_create.ccbi", this);
        this->setContentSize(m_pEditUI->getContentSize());
        this->addChild(m_pEditUI);
        
        m_pFieldName = CTextInputField::create(m_pFieldNameBg->getContentSize(), CCScale9Sprite::create());
        m_pFieldName->setAnchorPoint(ccp(0, 0));
        m_pFieldName->setMaxLength(16);
        m_pFieldName->setFontColor(ccBLACK);
        m_pFieldName->setPlaceHolder("name...");
        m_pFieldNameBg->addChild(m_pFieldName);
        
        m_pFieldDesc = CTextInputField::create(m_pFieldDescBg->getContentSize(), CCScale9Sprite::create());
        m_pFieldDesc->setAnchorPoint(ccp(0, 0));
        m_pFieldDesc->setFontSize(18);
        m_pFieldDesc->setFontColor(ccBLACK);
        m_pFieldDesc->setMaxLength(128);
        m_pFieldDesc->setInputMode(kTextFieldInputModeMultiLine);
        m_pFieldDesc->setPlaceHolder("description...");
        m_pFieldDescBg->addChild(m_pFieldDesc);
    }
    
    m_selectIcon = 0;
    m_joinType = 0;
    m_requiredTrophies = 300;
    
    m_pBtnCreate->setVisible(true);
    m_pBtnEdit->setVisible(false);
    
    m_selectNational = 4;  //default USA
    
    if (ClanMgr::getSingleton().m_myclan.id != 0) {
        m_selectIcon = ClanMgr::getSingleton().m_myclan.flag;
        m_selectNational = ClanMgr::getSingleton().m_myclan.national;
        m_joinType = ClanMgr::getSingleton().m_myclan.jointype;
        m_requiredTrophies = ClanMgr::getSingleton().m_myclan.needcupnums;
        
        m_pFieldName->setText(ClanMgr::getSingleton().m_myclan.name.c_str());
        m_pFieldDesc->setText(ClanMgr::getSingleton().m_myclan.description.c_str());
        
        m_pBtnCreate->setVisible(false);
        m_pBtnEdit->setVisible(true);
        
        if (m_selectNational <= 0) {
            m_selectNational = 4;
        }
    }
    
    char str[128];
    sprintf(str, "clan_icon_%d", m_selectIcon);
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pClanIcon->setDisplayFrame(pFrame);
    
    sprintf(str, "clan_national_%d", m_selectNational);
    
    pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pClanNational->setDisplayFrame(pFrame);
    
    m_pNationalName->setString(national[m_selectNational].c_str());
    
    m_pJoinType->setString(ClanType[m_joinType].c_str());
    
    sprintf(str, "%d", m_requiredTrophies);
    m_pRequiredTrophies->setString(str);
    
    return true;
}

bool ClanEditUI::onNotify(int notifyid, const void* pVal)
{
    if (notifyid == NOTIFY_CLAN_CHANGE) {
        
        ClanUI* pClanUI = PlayerScene::getSingleton().m_pGameWorld->m_pClanUI;
        if (pClanUI) {
            
            pClanUI->m_pClanInfoUI->m_info = ClanMgr::getSingleton().m_myclan;
            pClanUI->m_pClanInfoUI->refreshInfo();
            pClanUI->hideClanEditUI();
        }
        
        PlayerScene::getSingleton().m_pGameWorld->hideClanEditUI();
    }
    
    return true;
}

void ClanEditUI::showIconEditUI()
{
    m_pEditUI->setVisible(false);
    if (m_pIconEditUI == NULL) {
        m_pIconEditUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_icon_edit.ccbi", this);
        this->addChild(m_pIconEditUI);
        
        m_pIconListView->setDelegate(this);
        
        m_pIconListView->setContentOffset(ccp(0, m_pIconListView->getViewSize().height - m_pIconListView->getContentSize().height));
    }
}

void ClanEditUI::hideIconEditUI()
{
    m_pEditUI->setVisible(true);
    if (m_pIconEditUI) {
        this->removeChild(m_pIconEditUI);
        m_pIconEditUI = NULL;
        
        m_pIconListView = NULL;
    }
}

void ClanEditUI::showNationalEditUI()
{
    m_pEditUI->setVisible(false);
    if (m_pNationalEditUI == NULL) {
        m_pNationalEditUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_national_edit.ccbi", this);
        this->addChild(m_pNationalEditUI);
        
        m_pNationalListView->setDelegate(this);
        
        m_pNationalListView->setContentOffset(ccp(0, m_pNationalListView->getViewSize().height - m_pNationalListView->getContentSize().height));
    }
}

void ClanEditUI::hideNationalEditUI()
{
    m_pEditUI->setVisible(true);
    if (m_pNationalEditUI) {
        this->removeChild(m_pNationalEditUI);
        m_pNationalEditUI = NULL;
        
        m_pNationalListView = NULL;
    }
}

void ClanEditUI::menuBrowseIconCallback(CCObject* pSender, CCControlEvent event)
{
    showIconEditUI();
}

void ClanEditUI::menuBrowseNationalCallback(CCObject *pSender, CCControlEvent event)
{
    showNationalEditUI();
}

void ClanEditUI::menuFrontTypeCallback(CCObject* pSender, CCControlEvent event)
{
    m_joinType--;
    
    if(m_joinType < 0)
        m_joinType = 2;
    
    m_pJoinType->setString(ClanType[m_joinType].c_str());
}

void ClanEditUI::menuNextTypeCallback(CCObject* pSender, CCControlEvent event)
{
    m_joinType++;
    
    if(m_joinType > 2)
        m_joinType = 0;
    
    m_pJoinType->setString(ClanType[m_joinType].c_str());
}

void ClanEditUI::menuFrontTrophiesCallback(CCObject* pSender, CCControlEvent event)
{
    m_requiredTrophies -= 100;
    
    if(m_requiredTrophies < 0)
        m_requiredTrophies = CLAN_TROPHIES_MAX;
    
    char str[128];
    sprintf(str, "%d", m_requiredTrophies);
    
    m_pRequiredTrophies->setString(str);
}

void ClanEditUI::menuNextTrophiesCallback(CCObject* pSender, CCControlEvent event)
{
    m_requiredTrophies += 100;
    
    if(m_requiredTrophies > CLAN_TROPHIES_MAX)
        m_requiredTrophies = 0;
    
    char str[128];
    sprintf(str, "%d", m_requiredTrophies);
    
    m_pRequiredTrophies->setString(str);
}

void ClanEditUI::menuIconCallback(CCObject *pSender, CCControlEvent event)
{
    if (event == CCControlEventTouchDown) {
        
        m_dragOffset = m_pIconListView->getContentOffset();
        m_isDrag = false;
    }
    else {
        
        if (m_isDrag) {
            return;
        }
    
        int tag = ((CCNode*)pSender)->getTag();
        
        m_selectIcon = tag - 1;
        
        char str[128];
        sprintf(str, "clan_icon_%d", m_selectIcon);
        
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pClanIcon->setDisplayFrame(pFrame);
        
        hideIconEditUI();
    }
}

void ClanEditUI::menuNationalCallback(CCObject *pSender, CCControlEvent event)
{
    if (event == CCControlEventTouchDown) {
        
        m_dragOffset = m_pNationalListView->getContentOffset();
        m_isDrag = false;
    }
    else {
        
        if (m_isDrag) {
            return;
        }
    
        int tag = ((CCNode*)pSender)->getTag();
        
        m_selectNational = tag;
        
        char str[128];
        sprintf(str, "clan_national_%d", m_selectNational);
        
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pClanNational->setDisplayFrame(pFrame);
        
        m_pNationalName->setString(national[m_selectNational].c_str());
        
        hideNationalEditUI();
    }
}

void ClanEditUI::menuCreateCallback(CCObject* pSender, CCControlEvent event)
{
    if(strlen(m_pFieldName->getText()) < 4)
    {
        MessageUI::show(MSG_NORMAL, "", ViewString::getSingleton().getValue("msg_clanname_error").c_str(), 0, -1);
        return;
    }
    
    int need;
    int ret = ClanMgr::getSingleton().createClan(need, m_pFieldName->getText(), "", m_pFieldDesc->getText(), m_joinType, m_requiredTrophies, m_selectIcon, m_selectNational);
    
    if(ret != 1)
    {
        char message[512];
        sprintf(message, ViewString::getSingleton().getValue("msg_nogold_message").c_str(), need);
        
        MessageUI::show(MSG_NOGOLD, ViewString::getSingleton().getValue("msg_nogold_title").c_str(), message, need, GETGEM_CREATE_CLAN);
    }
    else
    {
        PlayerScene::getSingleton().m_pGameWorld->hideClanUI();
        
        PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->onSelect(false, NULL);
    }
    
}

void ClanEditUI::menuEditCallback(CCObject *pSender, CCControlEvent event)
{
    if(strlen(m_pFieldName->getText()) < 4)
    {
        MessageUI::show(MSG_NORMAL, "", ViewString::getValue("msg_clanname_error").c_str(), 0, -1);
        return;
    }
    
    ClanMgr::getSingleton().m_myclan.national = m_selectNational;
    ClanMgr::getSingleton().m_myclan.name = m_pFieldName->getText();
    ClanMgr::getSingleton().m_myclan.description = m_pFieldDesc->getText();
    ClanMgr::getSingleton().m_myclan.flag = m_selectIcon;
    ClanMgr::getSingleton().m_myclan.jointype = m_joinType;
    ClanMgr::getSingleton().m_myclan.needcupnums = m_requiredTrophies;
    
    HttpClient::getSingleton().editClan(ClanMgr::getSingleton().m_myclan);
    
    ClanMgr::getSingleton().updateClan(ClanMgr::getSingleton().m_myclan.id, m_selectIcon, ClanMgr::getSingleton().m_myclan.name.c_str());
    
    m_pWaitLoading = CUINode::createWithFile("loading2.ccbi", this);
    m_pWaitLoading->setTargetTouch(true);
    this->addChild(m_pWaitLoading);
}

void ClanEditUI::menuBackCallback(CCObject* pSender, CCControlEvent event)
{
    if (m_pIconEditUI) {
        hideIconEditUI();
    }
    
    if (m_pNationalEditUI) {
        hideNationalEditUI();
    }
    
    if (PlayerScene::getSingleton().m_pGameWorld->m_pClanUI) {
        PlayerScene::getSingleton().m_pGameWorld->m_pClanUI->menuBackCallback(pSender, event);
    }
}

void ClanEditUI::scrollViewDidScroll(CCScrollView* view)
{
    if (view == m_pIconListView) {
        
        if (fabs(view->getContentOffset().y - m_dragOffset.y) > 10) {
            m_isDrag = true;
        }
    }
    else if (view == m_pNationalListView) {
        
        if (fabs(view->getContentOffset().y - m_dragOffset.y) > 10) {
            m_isDrag = true;
        }
    }
}

bool ClanEditUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanNational", CCSprite*, m_pClanNational);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "NationalName", CCLabelBMFont*, m_pNationalName);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "JoinType", CCLabelBMFont*, m_pJoinType);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RequiredTrophies", CCLabelBMFont*, m_pRequiredTrophies);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "FieldNameBg", CCScale9Sprite*, m_pFieldNameBg);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "FieldDescBg", CCScale9Sprite*, m_pFieldDescBg);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnBack", CCControlButton*, m_pBtnBack);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnCreate", CCControlButton*, m_pBtnCreate);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnEdit", CCControlButton*, m_pBtnEdit);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "IconListView", CCScrollView*, m_pIconListView);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "NationalListView", CCScrollView*, m_pNationalListView);
    
    return false;
}

SEL_CCControlHandler ClanEditUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBrowseIconCallback", ClanEditUI::menuBrowseIconCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBrowseNationalCallback", ClanEditUI::menuBrowseNationalCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuIconCallback", ClanEditUI::menuIconCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuNationalCallback", ClanEditUI::menuNationalCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuFrontTypeCallback", ClanEditUI::menuFrontTypeCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuNextTypeCallback", ClanEditUI::menuNextTypeCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuFrontTrophiesCallback", ClanEditUI::menuFrontTrophiesCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuNextTrophiesCallback", ClanEditUI::menuNextTrophiesCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", ClanEditUI::menuBackCallback);

    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuEditCallback", ClanEditUI::menuEditCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCreateCallback", ClanEditUI::menuCreateCallback);
    
    return NULL;
}

