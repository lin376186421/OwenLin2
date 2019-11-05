//
//  ClanInfoUI.cpp
//  GameClient
//
//  Created by qsun on 14-3-13.
//
//

#include "ClanInfoUI.h"
#include "MessageUI.h"
#include "ClanUI.h"
#include "../coc/LogicInfo.h"
#include "../coc/ClanMgr.h"
#include "../coc/ImgMgr.h"
#include "../coc/PlayerScene.h"
#include "../GameWorldScene.h"

ClanInfoUI::ClanInfoUI(ClanInfo clanInfo)
: m_info(clanInfo)
{
    m_info.memberList.clear();
}

ClanInfoUI::~ClanInfoUI()
{
    LogicInfo::getSingleton().removeListener(this);
}

ClanInfoUI* ClanInfoUI::create(ClanInfo clanInfo)
{
    ClanInfoUI *pRet = new ClanInfoUI(clanInfo);
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

bool ClanInfoUI::init()
{
    if (CUINode::init()) {
        
        LogicInfo::getSingleton().addListener(this);
        
        m_pClanInfoBg = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_info_bg.ccbi", this);
        this->setContentSize(m_pClanInfoBg->getContentSize());
        this->addChild(m_pClanInfoBg);
        
        CCNode* m_pClanInfo = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_info.ccbi", this);
        this->setContentSize(m_pClanInfo->getContentSize());
        this->setTargetTouch(true);
        this->addChild(m_pClanInfo);
        
        m_pMembersView = CCTableView::create(this, m_pView->getContentSize());
        m_pMembersView->setDirection(kCCScrollViewDirectionVertical);
        m_pMembersView->setDelegate(this);
        m_pMembersView->reloadData();
        m_pView->addChild(m_pMembersView);
        
        m_pItemMenu = ItemMenuUI::create(m_pView);
        m_pItemMenu->setVisible(false);
        m_pView->addChild(m_pItemMenu);
        
        HttpClient::getSingleton().requestClanData(m_info.id);
    }
    
    refreshInfo();
    
    m_pBtnJoin->setEnabled(false);
    this->setTouchEnabled(true);
        
    return true;
}

bool ClanInfoUI::onNotify(int notifyid, const void *pVal)
{
    if (notifyid == NOTIFY_CLAN_REFRESH) {
        
        m_info = *((ClanInfo*)pVal);
        
        refreshInfo();
    }
    
    return true;
}

void ClanInfoUI::refreshInfo()
{
    m_pBtnJoin->setVisible(false);
    m_pBtnLeave->setVisible(false);
    m_pBtnEdit->setVisible(false);
        
    m_pBtnJoin->setEnabled(true);
    
    if (PlayerScene::getSingleton().m_pGameWorld->m_pMainUI) {
        
        if(m_info.jointype == 2 || m_info.needcupnums > LogicInfo::getSingleton().m_myInfo.cupex)
        {
            m_pBtnJoin->setEnabled(false);
        }
        
        if (ClanMgr::getSingleton().m_myclan.id == 0) {
            if (ClanMgr::getSingleton().m_castleLevel > 1) {
                m_pBtnJoin->setVisible(true);
            }
        }
        else if (ClanMgr::getSingleton().m_myclan.id == m_info.id) {
            
            if (ClanMgr::getSingleton().m_clanjob == CLAN_JOB_LEADER) {
                m_pBtnEdit->setVisible(true);
            }
            else if (ClanMgr::getSingleton().m_clanjob >= 0) {
                m_pBtnLeave->setVisible(true);
            }
            
            if (m_info.membercurnums == 1) {
                m_pBtnLeave->setVisible(true);
            }
        }
        
        if (m_info.membercurnums >= m_info.membermaxnums) {
            m_pBtnJoin->setEnabled(false);
        }
    }
    
    char str[128];
    m_pClanName->setString(m_info.name.c_str());
    sprintf(str, "%d", m_info.cupnums);
    m_pClanCup->setString(str);
    sprintf(str, "%d/%d", m_info.membercurnums, m_info.membermaxnums);
    m_pClanMembers->setString(str);
    m_pJoinType->setString(ClanType[m_info.jointype].c_str());
    sprintf(str, "%d", m_info.needcupnums);
    m_pRequiredTrophies->setString(str);
    m_pClanDesc->setString(m_info.description.c_str());
    sprintf(str, "clan_icon_%d", m_info.flag);
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pClanIcon->setDisplayFrame(pFrame);
    
    m_pClanNational->setVisible(false);
    m_pNationalName->setVisible(false);
    if (m_info.national > 0) {
        sprintf(str, "clan_national_%d", m_info.national);
        pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pClanNational->setDisplayFrame(pFrame);
        m_pClanNational->setVisible(true);
        
        m_pNationalName->setString(national[m_info.national].c_str());
        m_pNationalName->setVisible(true);
    }
    
    m_pMembersView->reloadData();
}

void ClanInfoUI::showClanEditUI()
{
    if (PlayerScene::getSingleton().m_pGameWorld->m_pClanUI) {
        PlayerScene::getSingleton().m_pGameWorld->m_pClanUI->showClanEditUI();
    }
    
}

void ClanInfoUI::hideClanEditUI()
{
    if (PlayerScene::getSingleton().m_pGameWorld->m_pClanUI) {
        PlayerScene::getSingleton().m_pGameWorld->m_pClanUI->hideClanEditUI();
    }
}

CCTableViewCell* ClanInfoUI::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
    CCTableViewCell *cell = table->dequeueCell();
    if (cell == NULL) {
        cell = new CCTableViewCell();
        cell->autorelease();
    }
    cell->setIdx(idx);
    
    int index = m_info.memberList.size() - idx - 1;
    ClanMember member = m_info.memberList[index];
    
    ClanMemberItem* item = ClanMemberItem::create(table, member);
    item->setTouchControlView(table);
    item->setTag(1);
    cell->addChild(item);
    
    return cell;
}

void ClanInfoUI::tableCellTouched(CCTableView *table, CCTableViewCell *cell)
{
    CCSize cellSize = cellSizeForTable(table);
    
    ClanMemberItem* item = (ClanMemberItem*)cell->getChildByTag(1);
    
    m_pItemMenu->resetMenu(item->m_info.uid, item->m_info.name.c_str(), item->m_info.clanid, item->m_info.clanjob, true);
    
    m_pItemMenu->setPositionX(0.4*cellSize.width);
    m_pItemMenu->setPositionY(cell->getIdx() * cellSize.height + table->getContentOffset().y + cellSize.height/2);
    
    m_pItemMenu->adjuestPosition();
}

CCSize ClanInfoUI::cellSizeForTable(CCTableView *table)
{
    CCSize size = CCSizeMake(960, 84);
    return size;
}

unsigned int ClanInfoUI::numberOfCellsInTableView(CCTableView *table)
{
    return m_info.memberList.size();
}

void ClanInfoUI::menuJoinCallback(CCObject* pSender, CCControlEvent event)
{
    MessageUI::show(MSG_CLAN_JOIN, NULL, ViewString::getValue("msg_clan_join").c_str(), ClanMgr::getSingleton().m_otherclan.id, -1);
}

void ClanInfoUI::menuEditCallback(CCObject* pSender, CCControlEvent event)
{
    showClanEditUI();
}

void ClanInfoUI::menuLeaveCallback(CCObject* pSender, CCControlEvent event)
{
    if (ClanMgr::getSingleton().m_clanjob == CLAN_JOB_LEADER) {
        MessageUI::show(MSG_CLAN_LEAVE, NULL, ViewString::getValue("msg_clan_dismiss").c_str(), 0, -1);
    }
    else {
        MessageUI::show(MSG_CLAN_LEAVE, NULL, ViewString::getValue("msg_clan_leave").c_str(), 0, -1);
    }
}

void ClanInfoUI::menuBackCallback(CCObject *pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideClanInfoUI();
}

bool ClanInfoUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanNational", CCSprite*, m_pClanNational);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "NationalName", CCLabelBMFont*, m_pNationalName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanDesc", CCLabelTTF*, m_pClanDesc);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanCup", CCLabelBMFont*, m_pClanCup);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanMembers", CCLabelBMFont*, m_pClanMembers);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "JoinType", CCLabelBMFont*, m_pJoinType);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RequiredTrophies", CCLabelBMFont*, m_pRequiredTrophies);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnJoin", CCControlButton*, m_pBtnJoin);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnEdit", CCControlButton*, m_pBtnEdit);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnLeave", CCControlButton*, m_pBtnLeave);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnBack", CCControlButton*, m_pBtnBack);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "View", CCLayer*, m_pView);
    
    return false;
}

SEL_CCControlHandler ClanInfoUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuJoinCallback", ClanInfoUI::menuJoinCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuLeaveCallback", ClanInfoUI::menuLeaveCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuEditCallback", ClanInfoUI::menuEditCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", ClanInfoUI::menuBackCallback);
    
    return NULL;
}

void ClanInfoUI::scrollViewDidScroll(CCScrollView *view)
{
    
}

ClanMemberItem::ClanMemberItem(CCScrollView* pScrollView, ClanMember member)
: m_pScrollView(pScrollView)
, m_info(member)
{
    
}

ClanMemberItem::~ClanMemberItem()
{
    
}

ClanMemberItem* ClanMemberItem::create(CCScrollView* pScrollView, ClanMember member)
{
    ClanMemberItem *pRet = new ClanMemberItem(pScrollView, member);
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

bool ClanMemberItem::init()
{
    if (CUINode::init()) {
        
        CCSize parentSize = m_pScrollView->getViewSize();
        CCNode* itemNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_members_item.ccbi", this, parentSize);
        this->setContentSize(itemNode->getContentSize());
        this->addChild(itemNode);
        
        CCSize contentSize = m_pItemBg->getContentSize();
        if (m_info.uid == LogicInfo::getSingleton().m_myInfo.uid) {
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("mayClan_list_2");
            m_pItemBg->setSpriteFrame(pFrame);
            
            m_pItemBg->setPreferredSize(contentSize);
        }
        
        char str[128];
        sprintf(str, "%d", m_info.currank);
        m_pRankingId->setString(str);
        m_pName->setString(m_info.name.c_str());
        sprintf(str, "%d", m_info.level);
        m_pLevel->setString(str);
        sprintf(str, "%d", m_info.curcup);
        m_pCup->setString(str);
        sprintf(str, "%d", m_info.sendsoldiers);
        m_pTroopsDonated->setString(str);
        sprintf(str, "%d", m_info.getsoldiers);
        m_pTroopsReceived->setString(str);
        sprintf(str, "%d", m_info.lastrank);
        m_pRankingChangeNum->setString(str);

        m_pClanPower->setString(ClanJob[m_info.clanjob].c_str());
        
        int rankoff = m_info.lastrank - m_info.currank;
        
        CCSpriteFrame* pFrame;
        if(rankoff < 0)
        {
            m_pRankingChangeTip->setColor(ccc3(255, 0, 0));
            pFrame = ImgMgr::getSingleton().getFrame("icon_fall");
            m_pRankingChangeNum->setColor(ccRED);
        }
        else if(rankoff > 0)
        {
            m_pRankingChangeTip->setColor(ccc3(0, 255, 0));
            pFrame = ImgMgr::getSingleton().getFrame("icon_rise");
            m_pRankingChangeNum->setColor(ccGREEN);
        }
        else
        {
            pFrame = ImgMgr::getSingleton().getFrame("icon_equal");
            m_pRankingChangeNum->setVisible(false);
        }
        m_pRankingChangeTip->setDisplayFrame(pFrame);
        
        sprintf(str, "%d", rankoff);
        m_pRankingChangeNum->setString(str);
        
        sprintf(str, "player_mr_%d", m_info.military);
        pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pMilitaryIcon->setDisplayFrame(pFrame);
        
        m_pWarInvalid->setVisible(false);
        if (m_info.warinvalid) {
            m_pWarInvalid->setVisible(true);
        }
        
        if (m_info.isonline) {
            pFrame = ImgMgr::getSingleton().getFrame("online");
            m_pOnLineIcon->setDisplayFrame(pFrame);
        }
        else {
            pFrame = ImgMgr::getSingleton().getFrame("offline");
            m_pOnLineIcon->setDisplayFrame(pFrame);
        }

        
        this->setTouchEnabled(true);
        
        return true;
    }
    
    return false;
}

bool ClanMemberItem::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    for (CCNode *c = this->m_pParent; c != NULL; c = c->getParent())
    {
        if (c->isVisible() == false)
        {
            return false;
        }
    }
    
    if (isTouchInside(pTouch)) {
        return true;
    }
    return false;
}

void ClanMemberItem::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

void ClanMemberItem::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

bool ClanMemberItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankingId", CCLabelBMFont*, m_pRankingId);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Level", CCLabelBMFont*, m_pLevel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Name", CCLabelTTF*, m_pName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Cup", CCLabelBMFont*, m_pCup);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanPower", CCLabelBMFont*, m_pClanPower);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TroopsDonated", CCLabelBMFont*, m_pTroopsDonated);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TroopsReceived", CCLabelBMFont*, m_pTroopsReceived);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankingChangeNum", CCLabelBMFont*, m_pRankingChangeNum);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankingChangeTip", CCSprite*, m_pRankingChangeTip);
        
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "OnLineIcon", CCSprite*, m_pOnLineIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WarInvalid", CCSprite*, m_pWarInvalid);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "IconMilitary", CCSprite*, m_pMilitaryIcon);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ItemBg", CCScale9Sprite*, m_pItemBg);
    
    return false;
}

SEL_CCControlHandler ClanMemberItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    return NULL;
}