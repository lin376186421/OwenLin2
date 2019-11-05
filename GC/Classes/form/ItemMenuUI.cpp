//
//  ItemMenuUI.cpp
//  GameClient
//
//  Created by qsun on 14-3-13.
//
//

#include "ItemMenuUI.h"
#include "../coc/ChatMgr.h"
#include "../coc/ClanMgr.h"
#include "../coc/LogicInfo.h"
#include "../coc/PlayerScene.h"
#include "../coc/SocketClient.h"
#include "../GameWorldScene.h"

const std::string PromoteTip[3][2] = {
    {"Promote to First Sergeant", ""},
    {"Promote to XO", "Demote to Member"},
    {"Promote to Commander", "Demote to First Sergeant"}};

ItemMenuUI::ItemMenuUI(CCLayer* pTargetView)
: m_pTargetView(pTargetView)
{
    
}

ItemMenuUI::~ItemMenuUI()
{
    
}

ItemMenuUI* ItemMenuUI::create(CCLayer *pTargetView)
{
    ItemMenuUI *pRet = new ItemMenuUI(pTargetView);
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

bool ItemMenuUI::init()
{
    if (CUINode::init()) {
        
        m_pMenuNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("member_menu.ccbi", this);
        this->setContentSize(m_pMenuNode->getContentSize());
        this->setTargetTouch(true);
        this->addChild(m_pMenuNode);
        
        for (int i = 0; i < 6; i++) {
            m_menuPosY[i] = m_pBtnMenu[i]->getPositionY();
        }
    }
    
    m_viewSize = m_pMenuView->getContentSize();
    m_bEnabled = false;
    
    return true;
}

bool ItemMenuUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    this->setEnabled(true);
    this->setVisible(false);

    return false;
}

void ItemMenuUI::resetMenu(int uid, const char* uname, int clanid, int clanjob, bool isManage)
{
    if (!isEnabled()) {
        return;
    }
    
    if (uid == m_uid || uid == LogicInfo::getSingleton().m_myInfo.uid) {
        m_uid = -1;
        this->setVisible(false);
        return;
    }
    
    m_uid = uid;
    m_uname = string(uname);
    m_clanid = clanid;
    m_clanjob = clanjob;
    
    int menuNum = 6;
    
    for (int i = 0; i < menuNum; i ++) {
        m_pBtnMenu[i]->setVisible(false);
    }
    
    m_pPlayerName->setString(uname);
    
    m_pBtnMenu[0]->setVisible(true);
    
    if (isManage) { 

        if (clanid == ClanMgr::getSingleton().m_myclan.id) {
            ClanMember* pMyInfo = ClanMgr::getSingleton().getClanMember(LogicInfo::getSingleton().m_myInfo.uid);
            ClanMember* pMember = ClanMgr::getSingleton().getClanMember(m_uid);
            
            if (pMember->clanjob < pMyInfo->clanjob) {
                if (pMember->clanjob != CLAN_JOB_LEADER) {
                    m_pBtnMenu[1]->setVisible(true);
                    ((CCLabelBMFont*)m_pBtnMenu[1]->getChildByTag(1))->setString(PromoteTip[pMember->clanjob][0].c_str());
                }
                if (pMember->clanjob != CLAN_JOB_MEMBER) {
                    m_pBtnMenu[2]->setVisible(true);
                    ((CCLabelBMFont*)m_pBtnMenu[2]->getChildByTag(1))->setString(PromoteTip[pMember->clanjob][1].c_str());
                }
                
                m_pBtnMenu[3]->setVisible(true);
            }
        }
    }
    else if (clanid != ClanMgr::getSingleton().m_myclan.id){
        
        if (clanid <= 0 && ClanMgr::getSingleton().m_myclan.id > 0 && m_clanjob >= CLAN_JOB_ELDER) { //invite
            m_pBtnMenu[4]->setVisible(true);
        }
        else if (clanid > 0) {  //visit
            m_pBtnMenu[5]->setVisible(true);
        }
    }

    int nums = 0;
    float posY = m_pBtnMenu[0]->getPositionY();
    for (int i = 0; i < menuNum; i++) {
        if (m_pBtnMenu[i]->isVisible()) {
            nums++;
            
            m_pBtnMenu[i]->setPositionY(posY);
            if (nums < menuNum) {
                posY = m_menuPosY[nums];
            }
        }
    }
    
    float offY = m_pBtnMenu[0]->getPositionY() - m_pBtnMenu[1]->getPositionY() - m_pBtnMenu[0]->getContentSize().height;
    float menuH = m_pBtnMenu[0]->getContentSize().height;
    
    float viewH = m_viewSize.height - (menuH + offY) * (menuNum - nums);
    
    m_pMenuView->setPreferredSize(CCSizeMake(m_viewSize.width, viewH));
    m_pMenuNode->setPositionY(-(m_viewSize.height - viewH) - viewH/2);
    
    m_pMenuArrow->setPositionY((m_viewSize.height - viewH) + viewH/2);
    m_pMenuArrow->setVisible(true);
    
    this->setVisible(true);
    this->setEnabled(false);
    this->setContentSize(CCSizeMake(m_viewSize.width, viewH));
}

void ItemMenuUI::setClanInfo(string clanname, int clanflag, int clantype, int clancup, int clanneedcup)
{
    m_clanname = clanname;
    m_clanflag = clanflag;
    m_clantype = clantype;
    m_clancup = clancup;
    m_clanneedcup = clanneedcup;
}

void ItemMenuUI::adjuestPosition()
{
    CCPoint pos = getPosition();
    
    CCPoint basePos = this->convertToWorldSpace(CCPointZero);
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    if ((basePos.y - this->getContentSize().height/2) < 0) {
        pos.y = this->getParent()->convertToNodeSpace(ccp(0, this->getContentSize().height/2)).y;
        
        float offy = fabs(pos.y - this->getPositionY());
        if (offy > this->getContentSize().height/3) {
            m_pMenuArrow->setVisible(false);
        }
        else {
            m_pMenuArrow->setPositionY(m_pMenuArrow->getPositionY()-offy);
        }
        
    }
    else if ((basePos.y + this->getContentSize().height/2) > winSize.height) {
        pos.y = this->getParent()->convertToNodeSpace(ccp(0, winSize.height - this->getContentSize().height/2)).y;
        float offy = fabs(pos.y - this->getPositionY());
        if (offy > this->getContentSize().height/3) {
            m_pMenuArrow->setVisible(false);
        }
        else {
            m_pMenuArrow->setPositionY(m_pMenuArrow->getPositionY()+offy);
        }
    }
    
    this->setPosition(pos);
}

void ItemMenuUI::menuVisitCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_NORMAL);
	LogicInfo::getSingleton().viewUserEx(m_uid);
    
    if (PlayerScene::getSingleton().m_pGameWorld->m_pMainUI) {
        PlayerScene::getSingleton().m_pGameWorld->hideMainUI();
    }
    if (PlayerScene::getSingleton().m_pGameWorld->m_pClanUI) {
        PlayerScene::getSingleton().m_pGameWorld->hideClanUI();
    }
    if (PlayerScene::getSingleton().m_pGameWorld->m_pChatUI) {
        PlayerScene::getSingleton().m_pGameWorld->hideChatUI();
    }
    if (PlayerScene::getSingleton().m_pGameWorld->m_pRankingUI) {
        PlayerScene::getSingleton().m_pGameWorld->hideRankingUI();
    }
    if (PlayerScene::getSingleton().m_pGameWorld->m_pMailUI) {
        PlayerScene::getSingleton().m_pGameWorld->hideMailUI();
    }
    if (PlayerScene::getSingleton().m_pGameWorld->m_pLeagueUI) {
        PlayerScene::getSingleton().m_pGameWorld->hideLeagueUI();
    }
    if (PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI) {
        PlayerScene::getSingleton().m_pGameWorld->hideClanWarMapUI();
    }
    
    PlayerScene::getSingleton().m_pGameWorld->hideClanInfoUI();
    
    this->setVisible(false);
}

void ItemMenuUI::menuInviteCallback(CCObject *pSender, CCControlEvent event)
{
    ChatMgr::getSingleton().sendInvite(m_uid);
    
    this->setVisible(false);
}

void ItemMenuUI::menuPromoteCallback(CCObject* pSender, CCControlEvent event)
{
    ClanMgr::getSingleton().chgMemberJob(m_uid, 1, m_clanjob);
    
    this->setVisible(false);
}

void ItemMenuUI::menuDemoteCallback(CCObject* pSender, CCControlEvent event)
{
    ClanMgr::getSingleton().chgMemberJob(m_uid, -1, m_clanjob);
    
    this->setVisible(false);
}

void ItemMenuUI::menuKickOutCallback(CCObject* pSender, CCControlEvent event)
{
    ClanMgr::getSingleton().kickOut(m_uid, m_uname);
    
    this->setVisible(false);
}

void ItemMenuUI::menuVisitClanCallback(CCObject *pSender, CCControlEvent event)
{
    ClanMgr::getSingleton().m_otherclan.id = m_clanid;
    ClanMgr::getSingleton().m_otherclan.name = m_clanname;
    ClanMgr::getSingleton().m_otherclan.flag = m_clanflag;
    ClanMgr::getSingleton().m_otherclan.jointype = m_clantype;
    ClanMgr::getSingleton().m_otherclan.cupnums = m_clancup;
    ClanMgr::getSingleton().m_otherclan.needcupnums = m_clanneedcup;
    ClanMgr::getSingleton().m_otherclan.membercurnums = 0;

    PlayerScene::getSingleton().m_pGameWorld->showClanInfoUI();
    
    this->setVisible(false);
}

bool ItemMenuUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerName", CCLabelTTF*, m_pPlayerName);
    
    char str[128];
    for (int i = 0; i < 6; i++) {
        sprintf(str, "BtnMenu%d", i);
        CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, str, CCControlButton*, m_pBtnMenu[i]);
    }
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "MenuView", CCScale9Sprite*, m_pMenuView);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "MenuArrow", CCSprite*, m_pMenuArrow);
    
    return false;
}

SEL_CCControlHandler ItemMenuUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuVisitCallback", ItemMenuUI::menuVisitCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuPromoteCallback", ItemMenuUI::menuPromoteCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuDemoteCallback", ItemMenuUI::menuDemoteCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuKickOutCallback", ItemMenuUI::menuKickOutCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuInviteCallback", ItemMenuUI::menuInviteCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuVisitClanCallback", ItemMenuUI::menuVisitClanCallback);
    
    return NULL;
}
