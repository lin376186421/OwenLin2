//
//  RankingUI.cpp
//  GameClient
//
//  Created by chowray on 13-5-14.
//
//

#include "RankingUI.h"
#include "ClanUI.h"
#include "../coc/ImgMgr.h"
#include "../coc/LogicInfo.h"
#include "../coc/PlayerScene.h"
#include "../coc/GoldEmitter.h"
#include "../GameWorldScene.h"

#include "../coc/ClanMgr.h"

using namespace cocos2d;
using namespace coc;

int pageIndex = 1;

RankingUI::RankingUI()
{
    
}

RankingUI::~RankingUI()
{
    LogicInfo::getSingleton().removeListener(this);
}

bool RankingUI::init()
{
    bool bRet = false;
    do
    {
		CC_BREAK_IF(!CUINode::init());
        {
			LogicInfo::getSingleton().addListener(this);
            
            CCNode* rankingBg = CUINodeCache::sharedUINodeCache()->UINodeFromFile("ranking_bg.ccbi", this);//
            this->setContentSize(rankingBg->getContentSize());
            this->setTargetTouch(true);
            this->addChild(rankingBg);
            
            m_pPageNode = NULL;
            m_pView = NULL;
            m_pLoading = NULL;
            
            if(COC_CLAN == 0) {
                m_pBtnPages[0]->setVisible(false);
                m_pBtnPages[1]->setPosition(m_pBtnPages[0]->getPosition());
                pageIndex = 2;
            }
            
            addPageNode(pageIndex);
            
            refreshPage();
        }
		bRet = true;
	}while(0);
    
	return bRet;
}


void RankingUI::addPageNode(int nPageIndex)
{
    if (m_pLoading)
    {
        m_pLoading->removeFromParentAndCleanup(true);
        m_pLoading = NULL;
    }
    
    if (m_pPageNode)
    {
        this->removeChild(m_pPageNode);
        m_pPageNode = NULL;
        m_pView = NULL;
    }
    
    if (nPageIndex == 1)
    {
        m_pPageNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("ranking_clan.ccbi", this);
        
        m_pTableView = CCTableView::create(this, m_pView->getContentSize());
        m_pTableView->setDirection(kCCScrollViewDirectionVertical);
        m_pTableView->setDelegate(this);
        m_pTableView->reloadData();
        m_pTableView->setAnchorPoint(ccp(0, 0));
        m_pView->addChild(m_pTableView);
    }
    else if (nPageIndex == 2)
    {
        m_pPageNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("ranking_player.ccbi", this);
        
        m_pTableView = CCTableView::create(this, m_pView->getContentSize());
        m_pTableView->setDirection(kCCScrollViewDirectionVertical);
        m_pTableView->setDelegate(this);
        m_pTableView->reloadData();
        m_pTableView->setAnchorPoint(ccp(0, 0));
        m_pView->addChild(m_pTableView);
    }
    
    if (m_pPageNode!=NULL)
    {
        this->addChild(m_pPageNode);
    }
    m_pLoading = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading1.ccbi", this);
    m_pLoading->setVisible(false);
    
    if (m_pView)
    {
        m_pItemMenu = ItemMenuUI::create(m_pView);
        m_pItemMenu->setVisible(false);
        m_pView->addChild(m_pItemMenu);
        
        m_pLoading->setPosition(ccp(m_pView->getContentSize().width/2, m_pView->getContentSize().height/2));
        m_pView->addChild(m_pLoading,10);
    }
    else
    {
        m_pLoading->setPosition(ccp(getContentSize().width/2, getContentSize().height/2));
        addChild(m_pLoading,10);
    }
}

void RankingUI::refreshPage()
{
    for (int i = 0; i < 2; i++)
    {
        m_pBtnPages[i]->setEnabled(true);
    }
    m_pBtnPages[pageIndex-1]->setEnabled(false);
    
   if (pageIndex == 1)
    {
        if (ClanMgr::getSingleton().m_pClanRankingLst.size() == 0)
        {
            m_pLoading->setVisible(true);
            HttpClient::getSingleton().getClanRanking();
        }
    }else if (pageIndex == 2)
    {
        if (LogicInfo::getSingleton().m_lstCupRank.size() == 0)
        {
            m_pLoading->setVisible(true);
            LogicInfo::getSingleton().requestRank();
        }
    }
}

void RankingUI::menuPageCallback(CCObject *pSender, CCControlEvent event)
{
    int tag = ((CCNode*)pSender)->getTag();
    
    if (pageIndex != tag) {
        
        pageIndex = tag;
        addPageNode(pageIndex);
        
        refreshPage();
        
        menuBackCallback(pSender, event);
    }
}

void RankingUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    (PlayerScene::getSingleton().m_pGameWorld)->hideRankingUI();
}

void RankingUI::menuBackCallback(cocos2d::CCObject *pSender, CCControlEvent event)
{
    
}

bool RankingUI::onNotify(int notifyid, const void* pVal)
{
   if (pageIndex == 1)
    {
        if (NOTIFY_CLAN_RANKING == notifyid)
        {
            m_pLoading->setVisible(false);
            m_pTableView->reloadData();
        }
    }
    else if (pageIndex == 2)
    {
        if(NOTIFY_RANKEND == notifyid)
        {
            m_pLoading->setVisible(false);
            m_pTableView->reloadData();
        }
    }
    
    return true;
}

bool RankingUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankingView", CCLayer*, m_pView);
    
    char name[128];
    for (int i = 0; i < 2; i++)
    {
        sprintf(name, "BtnPage%d", i);
        CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, name, CCControlButton*, m_pBtnPages[i]);
    }
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ScrollView", CCScrollView*, m_pScrollView);

    return false;
}

SEL_CCControlHandler RankingUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuPageCallback", RankingUI::menuPageCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", RankingUI::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", RankingUI::menuBackCallback);

    return NULL;
}

CCSize RankingUI::cellSizeForTable(CCTableView *table)
{
    return CCSizeMake(960, 70 + 8);
}

CCTableViewCell* RankingUI::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
    CCTableViewCell *cell = table->dequeueCell();
    if (cell == NULL) {
        cell = new CCTableViewCell();
        cell->autorelease();
    }
    
    cell->setIdx(idx);
    cell->setAnchorPoint(ccp(0, 0));
    
    int rankIdx = numberOfCellsInTableView(table) - idx;
    
    int emptyCell[2] = {10+1, 100+2};
    
    if (rankIdx == emptyCell[0] || rankIdx == emptyCell[1]) {
        //empty cell
    }
    else{
        
        rankIdx = rankIdx - 1;
        if (rankIdx >= emptyCell[1]) {
            rankIdx -= 2;
        }else if (rankIdx >= emptyCell[0]){
            rankIdx -= 1;
        }
        
        if (pageIndex == 0)
        {
            LeagueMgr::PlayerInfo info = LeagueMgr::getSingleton().m_myGroup.playerLst[rankIdx];
            
//                LeaguePlayerItem *item = LeaguePlayerItem::create(info, m_pView->getContentSize());
//                item->setTag(1);
//                item->setTouchControlView(table);
//                cell->addChild(item);
        }
        else if (pageIndex == 1)
        {
            ClanInfo clanInfo = ClanMgr::getSingleton().m_pClanRankingLst[rankIdx];
            
            ClanItem* item = ClanItem::create(table, clanInfo);
            item->setTag(1);
            item->setTouchControlView(table);
            cell->addChild(item);
        }
        else if (pageIndex == 2)
        {
            if (rankIdx < LogicInfo::getSingleton().m_lstCupRank.size()) {
                
                RankUserInfo info = LogicInfo::getSingleton().m_lstCupRank[rankIdx];
                PlayerItem *item = PlayerItem::create(info, rankIdx, m_pView->getContentSize());
                
                item->setTag(1);
                item->setTouchControlView(table);
                cell->addChild(item);
            }
        }
    }
    
    return cell;
    
}

void RankingUI::tableCellTouched(CCTableView *table, CCTableViewCell *cell)
{
    if (pageIndex == 0)
    {
//        CCSize cellSize = cellSizeForTable(table);
//        LeaguePlayerItem* item = (LeaguePlayerItem*)cell->getChildByTag(1);
//        if(item) {
//            m_pItemMenu->resetMenu(item->m_info.uid, item->m_info.uname.c_str(), -1);
//            m_pItemMenu->setPositionX(0.4*cellSize.width);
//            m_pItemMenu->setPositionY(cell->getIdx() * cellSize.height + table->getContentOffset().y + cellSize.height/2);
//            m_pItemMenu->adjuestPosition();
//        }
    }
    else if (pageIndex == 1) {
        
        ClanItem* item = (ClanItem*)cell->getChildByTag(1);
        if(item) {
        
            ClanMgr::getSingleton().m_otherclan = item->m_info;
            PlayerScene::getSingleton().m_pGameWorld->showClanInfoUI();
        }
    }
    else if (pageIndex == 2) {
        CCSize cellSize = cellSizeForTable(table);
        
        PlayerItem* item = (PlayerItem*)cell->getChildByTag(1);
        if(item) {
            m_pItemMenu->resetMenu(item->m_info.uid, item->m_info.name.c_str(), item->m_info.clanid, ClanMgr::getSingleton().m_clanjob);
            m_pItemMenu->setClanInfo(item->m_info.clanname, item->m_info.clanflag, item->m_info.clantype, item->m_info.clancup, item->m_info.clanneedcup);
            m_pItemMenu->setPositionX(0.4*cellSize.width);
            m_pItemMenu->setPositionY(cell->getIdx() * cellSize.height + table->getContentOffset().y + cellSize.height/2);
        
            m_pItemMenu->adjuestPosition();
        }
    }
}

unsigned int RankingUI::numberOfCellsInTableView(CCTableView *table)
{
    int size = 0;
    
    if (pageIndex == 0) {
        
        size = LeagueMgr::getSingleton().m_myGroup.playerLst.size();
    }
    else if (pageIndex == 1) {
        
        size = ClanMgr::getSingleton().m_pClanRankingLst.size();
    }
    else if (pageIndex == 2) {
        
        size = LogicInfo::getSingleton().m_lstCupRank.size();
    }
    
    if (size > 100) { // top 100
        size += 2;
    }
    else if (size > 10) { //top 10
        size += 1;
    }
    
    return size;
}

PlayerItem::PlayerItem(RankUserInfo info, int rankIndex, CCSize size)
: m_info(info)
, m_size(size)
, m_rankIndex(rankIndex)
{
}

PlayerItem::~PlayerItem()
{
    
}

PlayerItem* PlayerItem::create(RankUserInfo info, int rankIndex, CCSize size)
{
    PlayerItem *pRet = new PlayerItem(info, rankIndex, size);
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

bool PlayerItem::init()
{
    if (CUINode::init()) {
        
        CCNode* itemNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("ranking_playeritem.ccbi", this, m_size);
        this->setContentSize(itemNode->getContentSize());
        this->addChild(itemNode);
        
        refresh();
        return true;
    }
    
    return false;
}

void PlayerItem::refresh()
{
    char str[128];
    
    m_pRankingIcon->setVisible(false);
    if (m_info.currank <= 3) {
        sprintf(str, "icon_top_%d", m_info.currank-1);
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pRankingIcon->setDisplayFrame(pFrame);
        m_pRankingIcon->setVisible(true);
    }
    
    sprintf(str, "player_mr_%d",m_info.military);
    CCSpriteFrame *frame = ImgMgr::getSingleton().getFrame(str);
    m_pIconMilitary->setDisplayFrame(frame);
    
    sprintf(str, "%d", m_info.currank);
    m_pRankingId->setString(str);
    
    sprintf(str, "%d", m_info.curlevel);
    m_pPlayerLevel->setString(str);
    
    m_pPlayerName->setString(m_info.name.c_str());
    
    sprintf(str, "%d", m_info.curcup);
    m_pPlayerCup->setString(str);
    
    CCSize contentSize = m_pItemBg->getContentSize();
    if (m_info.uid == LogicInfo::getSingleton().m_myInfo.uid) { //self
        m_pItemBg->setSpriteFrame(ImgMgr::getSingleton().getFrame("mayClan_list_2"));
        m_pDetailLabel->setVisible(false);
    }else {
        if (m_rankIndex > 0) {
            RankUserInfo preRank = LogicInfo::getSingleton().m_lstCupRank[m_rankIndex-1];
            if (preRank.uid == LogicInfo::getSingleton().m_myInfo.uid) {
                m_pItemBg->setSpriteFrame(ImgMgr::getSingleton().getFrame("mayClan_list_1"));
            }
        }else if (m_rankIndex < LogicInfo::getSingleton().m_lstCupRank.size() - 1) {
            RankUserInfo nextRank = LogicInfo::getSingleton().m_lstCupRank[m_rankIndex+1];
            if (nextRank.uid == LogicInfo::getSingleton().m_myInfo.uid) {
                m_pItemBg->setSpriteFrame(ImgMgr::getSingleton().getFrame("mayClan_list_0"));
            }
        }
    }
    
    m_pItemBg->setPreferredSize(contentSize);
    
    int rankoff = m_info.lastrank - m_info.currank;
    
    CCSpriteFrame* pFrame;
    if(rankoff < 0)
    {
        if (rankoff < -999) {
            rankoff = -999;
        }
        m_pRankingChangeTip->setColor(ccc3(255, 0, 0));
        pFrame = ImgMgr::getSingleton().getFrame("icon_fall");
    }
    else if(rankoff > 0)
    {
        if (rankoff > 999) {
            rankoff = 999;
        }
        m_pRankingChangeTip->setColor(ccc3(0, 255, 0));
        pFrame = ImgMgr::getSingleton().getFrame("icon_rise");
    }
    else
    {
        pFrame = ImgMgr::getSingleton().getFrame("icon_equal");
        m_pRankingChangeNum->setVisible(false);
    }
    m_pRankingChangeTip->setDisplayFrame(pFrame);
    
    sprintf(str, "%d", rankoff);
    m_pRankingChangeNum->setString(str);
    
    m_pClanIcon->setVisible(false);
    m_pClanName->setVisible(false);
    if (m_info.clanid > 0) {
        
        sprintf(str, "clan_icon_%d", m_info.clanflag);
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pClanIcon->setDisplayFrame(pFrame);
        m_pClanIcon->setVisible(true);
        
        m_pClanName->setString(m_info.clanname.c_str());
        m_pClanName->setVisible(true);
    }
}

bool PlayerItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankingIcon", CCSprite*, m_pRankingIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankingId", CCLabelBMFont*, m_pRankingId);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerName", CCLabelTTF*, m_pPlayerName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerLevel", CCLabelBMFont*, m_pPlayerLevel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerCup", CCLabelBMFont*, m_pPlayerCup);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ItemBg", CCScale9Sprite*, m_pItemBg);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnLookup", CCControlButton*, m_pBtnLookup);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DetaiLabel", CCLabelBMFont*, m_pDetailLabel);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankingChangeTip", CCSprite*, m_pRankingChangeTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankingChangeNum", CCLabelBMFont*, m_pRankingChangeNum);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "IconMilitary", CCSprite*, m_pIconMilitary);
    
    return false;
}

SEL_CCControlHandler PlayerItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    return NULL;
}
