//
//  ClanUI.cpp
//  GameClient
//
//  Created by chowray on 13-11-27.
//
//

#include "ClanUI.h"
#include "CUserData.h"
#include "../zephyr/Zephyr.h"
#include "../coc/PlayerScene.h"
#include "GameWorldScene.h"
#include "../coc/SoundMgr.h"
#include "../coc/LogicInfo.h"
#include "../coc/ClanMgr.h"
#include "../coc/HttpClient.h"

USING_NS_COC;
USING_NS_CC_ZEPHYR;

ClanUI::ClanUI()
: m_pListView(NULL)
, m_pSearchListView(NULL)
, m_pClanInfoUI(NULL)
, m_pClanEditUI(NULL)
{
    
}

ClanUI::~ClanUI()
{
    LogicInfo::getSingleton().removeListener(this);
}

bool ClanUI::init()
{
    if (CUINode::init()) {
        
        LogicInfo::getSingleton().addListener(this);
        
        CCNode* m_pClanBg = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_bg.ccbi", this);
        this->setContentSize(m_pClanBg->getContentSize());
        this->setTargetTouch(true);
        
        this->addChild(m_pClanBg);
        
        if (ClanMgr::getSingleton().m_myclan.id == 0) {
            
            m_pPagesUI[0] = NULL;
            
            m_pPagesUI[1] = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_list.ccbi", this);
            this->addChild(m_pPagesUI[1]);
            
            m_pPagesUI[2] = ClanEditUI::create();
            ((ClanEditUI*)m_pPagesUI[2])->m_pClanEditBg->setVisible(false);
            this->addChild(m_pPagesUI[2]);
            
            m_pPagesUI[3] = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_search.ccbi", this);
            this->addChild(m_pPagesUI[3]);
            
            m_pBtnPages[0]->setVisible(false);
            
            m_pageIndex = 1;
            
            HttpClient::getSingleton().getClanRanking();
            
            CCTableView* pClanListView = CCTableView::create(this, m_pListView->getContentSize());
            pClanListView->setDirection(kCCScrollViewDirectionVertical);
            pClanListView->setAnchorPoint(ccp(0, 0));
            pClanListView->setDelegate(this);
            pClanListView->setTag(1);
            m_pListView->addChild(pClanListView);
        }
        else
        {
            m_pPagesUI[0] = ClanInfoUI::create(ClanMgr::getSingleton().m_myclan);
            m_pClanInfoUI = (ClanInfoUI*)m_pPagesUI[0];
            this->addChild(m_pPagesUI[0]);
            
            m_pPagesUI[1] = NULL;
            m_pPagesUI[2] = NULL;
            
            m_pPagesUI[3] = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_search.ccbi", this);
            this->addChild(m_pPagesUI[3]);
            
            m_pageIndex = 0;
            
            m_pBtnPages[1]->setVisible(false);
            m_pBtnPages[2]->setVisible(false);
            m_pBtnPages[3]->setPosition(m_pBtnPages[2]->getPosition());
            
            m_pClanInfoUI->m_pClanInfoBg->setVisible(false);
            m_pClanInfoUI->m_pBtnBack->setVisible(false);
            m_pClanInfoUI->setTargetTouch(false);
            
//            m_curClan = ClanMgr::getSingleton().m_myclan;
        }
        
        CCTableView* pSearchListView = CCTableView::create(this, m_pSearchListView->getContentSize());
        pSearchListView->setDirection(kCCScrollViewDirectionVertical);
        pSearchListView->setAnchorPoint(ccp(0, 0));
        pSearchListView->setDelegate(this);
        pSearchListView->setTag(1);
        m_pSearchListView->addChild(pSearchListView);
        
        m_pFieldSearchName = CTextInputField::create(m_pFieldSearchNameBg->getContentSize(), CCScale9Sprite::create());
        m_pFieldSearchName->setAnchorPoint(ccp(0, 0));
        m_pFieldSearchName->setFontColor(ccBLACK);
        m_pFieldSearchName->setReturnType(kKeyboardReturnTypeDefault);
        m_pFieldSearchNameBg->addChild(m_pFieldSearchName);
        
        m_pLoading = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading1.ccbi", this);
        m_pLoading->setPosition(ccp(m_pView->getContentSize().width/2, m_pView->getContentSize().height/2));
        m_pLoading->setVisible(false);
        this->addChild(m_pLoading);
        
        m_isDrag = false;
        
        refreshPage();
    }
    
    return true;
}

void ClanUI::refreshPage()
{
    for (int i = 0; i < 4; i++) {
        m_pBtnPages[i]->setEnabled(true);
        
        if (m_pPagesUI[i]) {
            m_pPagesUI[i]->setVisible(false);
        }
    }
    m_pBtnPages[m_pageIndex]->setEnabled(false);
    m_pPagesUI[m_pageIndex]->setVisible(true);
    
    if (ClanMgr::getSingleton().m_myclan.id == 0) {
   
        if (m_pageIndex == 1) {
            ((CCTableView*)m_pListView->getChildByTag(1))->reloadData();
        }
    }
}

void ClanUI::showClanEditUI()
{
    m_pPagesUI[m_pageIndex]->setVisible(false);
    
    if (m_pClanEditUI == NULL) {
        m_pClanEditUI = ClanEditUI::create();
        m_pClanEditUI->m_pClanEditBg->setVisible(false);
        this->addChild(m_pClanEditUI);
    }    
}

void ClanUI::hideClanEditUI()
{
    m_pPagesUI[m_pageIndex]->setVisible(true);
    
    if (m_pClanEditUI) {
        this->removeChild(m_pClanEditUI);
        m_pClanEditUI = NULL;
    }
}

bool ClanUI::onNotify(int notifyid, const void* pVal)
{
	if(NOTIFY_CLAN_RANKING == notifyid)
	{
        m_pLoading->setVisible(false);
        
        ((CCTableView*)(m_pListView->getChildByTag(1)))->reloadData();
    }
    else if (NOTIFY_CLAN_SEARCH == notifyid)
    {
        m_pLoading->setVisible(false);
        
        ((CCTableView*)(m_pSearchListView->getChildByTag(1)))->reloadData();
    }
    else if (NOTIFY_CLAN_CHANGE == notifyid)
    {
        ((ClanInfoUI*)m_pPagesUI[0])->refreshInfo();
    }
    else if(NOTIFY_MYCLANJOBCHG == notifyid)
    {

    }
    
    return true;
}

void ClanUI::menuPageCallback(CCObject *pSender, CCControlEvent event)
{
    int page = ((CCNode*)pSender)->getTag();
    
    m_pLoading->setVisible(false);
    menuBackCallback(pSender, event);
    
    m_pageIndex = page;
    refreshPage();
}

void ClanUI::menuSearchCallback(CCObject* pSender, CCControlEvent event)
{
    if (strlen(m_pFieldSearchName->getText()) > 0) {
 
        ClanMgr::getSingleton().m_pClanSearchLst.clear();
        ((CCTableView*)(m_pSearchListView->getChildByTag(1)))->reloadData();
        
        m_pLoading->setVisible(true);
        HttpClient::getSingleton().searchClan(m_pFieldSearchName->getText());
    }
}

void ClanUI::menuBackCallback(CCObject *pSender, CCControlEvent event)
{
    if (m_pClanEditUI) {
        hideClanEditUI();
    }
}

void ClanUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideClanUI();
}

bool ClanUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    char str[128];
    for (int i = 0; i < 4; i++) {
        sprintf(str, "BtnPage%d", i);
        CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, str, CCControlButton*, m_pBtnPages[i]);
    }
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "View", CCLayer*, m_pView);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "FieldSearchNameBg", CCScale9Sprite*, m_pFieldSearchNameBg);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnCreate", CCControlButton*, m_pBtnCreate);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnEdit", CCControlButton*, m_pBtnEdit);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnSearch", CCControlButton*, m_pBtnSearch);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnBack", CCControlButton*, m_pBtnBack);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ListView", CCLayer*, m_pListView);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "SearchListView", CCLayer*, m_pSearchListView);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "MembersView", CCLayer*, m_pMembersView);
    
    return false;
}

SEL_CCControlHandler ClanUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuPageCallback", ClanUI::menuPageCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", ClanUI::menuBackCallback);
        
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSearchCallback", ClanUI::menuSearchCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", ClanUI::menuCloseCallback);
    
    return NULL;
}

bool ClanUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    m_isDrag = false;
    
    return true;
}

void ClanUI::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

void ClanUI::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    m_isDrag = false;
}

CCTableViewCell* ClanUI::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
    CCTableViewCell *cell = table->dequeueCell();
    if (cell == NULL) {
        cell = new CCTableViewCell();
        cell->autorelease();
    }
    cell->setIdx(idx);
    cell->setAnchorPoint(ccp(0, 0));
    
    if (m_pListView != NULL && table == m_pListView->getChildByTag(1)) {
        
        int index = ClanMgr::getSingleton().m_pClanRankingLst.size() - idx - 1;
        ClanInfo clanInfo = ClanMgr::getSingleton().m_pClanRankingLst[index];
        
        ClanItem* item = ClanItem::create(table, clanInfo);
        item->setTag(1);
        item->setTouchControlView(table);
        cell->addChild(item);
    }
    else if (m_pSearchListView != NULL && table == m_pSearchListView->getChildByTag(1)) {
        int index = ClanMgr::getSingleton().m_pClanSearchLst.size() - idx - 1;
        ClanInfo clanInfo = ClanMgr::getSingleton().m_pClanSearchLst[index];
        
        ClanItem* item = ClanItem::create(table, clanInfo);
        item->setTag(1);
        item->setTouchControlView(table);
        cell->addChild(item);
    }
    
    return cell;
    
}

CCSize ClanUI::cellSizeForTable(CCTableView *table)
{
    CCSize size = CCSizeMake(960, 84);
        
    return size;
}

unsigned int ClanUI::numberOfCellsInTableView(CCTableView *table)
{
    if (m_pListView != NULL && table == m_pListView->getChildByTag(1)) {
        return ClanMgr::getSingleton().m_pClanRankingLst.size();
    }
    else if (m_pSearchListView != NULL && table == m_pSearchListView->getChildByTag(1)) {
        return ClanMgr::getSingleton().m_pClanSearchLst.size();
    }
    
    return 0;
}

void ClanUI::scrollViewDidScroll(CCScrollView* view)
{

}

void ClanUI::tableCellTouched(CCTableView *table, CCTableViewCell *cell)
{
    ClanItem* pClanItem = (ClanItem*)cell->getChildByTag(1);
    
    ClanMgr::getSingleton().m_otherclan = pClanItem->m_info;
    PlayerScene::getSingleton().m_pGameWorld->showClanInfoUI();
}

ClanItem::ClanItem(CCScrollView* pScrollView, ClanInfo clanInfo)
: m_pScrollView(pScrollView)
, m_info(clanInfo)
{
    
}

ClanItem::~ClanItem()
{
    
}

ClanItem* ClanItem::create(CCScrollView* pScrollView, ClanInfo clanInfo)
{
    ClanItem *pRet = new ClanItem(pScrollView, clanInfo);
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

bool ClanItem::init()
{
    if (CUINode::init()) {
        
        CCSize parentSize = m_pScrollView->getViewSize();
        
        CCNode* itemNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_list_item.ccbi", this, parentSize);
        this->setContentSize(itemNode->getContentSize());
        this->addChild(itemNode);
        
        m_pClanName->setString(m_info.name.c_str());
        
        m_pJoinType->setString(ClanType[m_info.jointype].c_str());
        
        char str[128];
        sprintf(str, "%d/%d", m_info.membercurnums, m_info.membermaxnums);
        m_pClanMemberNums->setString(str);
        
        sprintf(str, "%d", m_info.cupnums);
        m_pClanCup->setString(str);
        
        sprintf(str, "clan_icon_%d", m_info.flag);
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        
        m_pClanIcon->setDisplayFrame(pFrame);
        
        m_pClanNational->setVisible(false);
        if (m_info.national > 0) {
            sprintf(str, "clan_national_%d", m_info.national);
            pFrame = ImgMgr::getSingleton().getFrame(str);
            m_pClanNational->setDisplayFrame(pFrame);
            m_pClanNational->setVisible(true);
        }
        
        return true;
    }
    
    return false;
}

bool ClanItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "JoinType", CCLabelBMFont*, m_pJoinType);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanCup", CCLabelBMFont*, m_pClanCup);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanMemberNums", CCLabelBMFont*, m_pClanMemberNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanNational", CCSprite*, m_pClanNational);
    
    return false;
}

SEL_CCControlHandler ClanItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    return NULL;
}
