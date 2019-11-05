//
//  ClanWarSignupUI.cpp
//  GameClient
//
//  Created by qsun on 14-9-4.
//
//

#include "ClanWarSignupUI.h"
#include "ClanInfoUI.h"
#include "../coc/ImgMgr.h"
#include "../coc/ClanMgr.h"
#include "../coc/ViewString.h"
#include "../coc/ClanWarMgr.h"
#include "../coc/HttpClient.h"
#include "../coc/LogicInfo.h"
#include "../coc/PlayerScene.h"
#include "../GameWorldScene.h"

ClanWarSignupUI::ClanWarSignupUI(ClanCityInfo* pCityInfo)
: m_pCityInfo(pCityInfo)
, m_pLoading(NULL)
, m_pSignupClansView(NULL)
{
    LogicInfo::getSingleton().addListener(this);
}

ClanWarSignupUI::~ClanWarSignupUI()
{
    LogicInfo::getSingleton().removeListener(this);
    
    unscheduleUpdate();
}

ClanWarSignupUI* ClanWarSignupUI::create(ClanCityInfo* pCityInfo)
{
    ClanWarSignupUI *pRet = new ClanWarSignupUI(pCityInfo);
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

bool ClanWarSignupUI::init()
{
    if (CUINode::init()) {
        
        CCNode* pSignupUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_signup.ccbi", this);
        this->setContentSize(pSignupUI->getContentSize());
        this->setTargetTouch(true);
        this->addChild(pSignupUI);
        
        m_pLoading = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading1.ccbi", this);
        m_pLoading->setPosition(ccp(m_pView->getContentSize().width/2, m_pView->getContentSize().height/2));
        m_pView->addChild(m_pLoading);
                
        m_pSignupClansView = CCTableView::create(this, m_pView->getContentSize());
        m_pSignupClansView->setDirection(kCCScrollViewDirectionVertical);
        m_pSignupClansView->setAnchorPoint(ccp(0, 0));
        m_pSignupClansView->setDelegate(this);
        m_pView->addChild(m_pSignupClansView);

        HttpClient::getSingleton().getClanWarSignList(m_pCityInfo->cityid);
        
        refreshInfo();

        scheduleUpdate();
    }
    
    return true;
}

void ClanWarSignupUI::update(float dt)
{
    int endtime = ClanWarMgr::getSingleton().m_warPrepareTime - getCurrentTime();
    
    if (endtime < 0) {
        endtime = 0;
    }
    
    m_pSignupEndTime->setString(formatTime(endtime, 3, true).c_str());
}

bool ClanWarSignupUI::onNotify(int notifyid, const void* pVal)
{
    if (notifyid == NOTIFY_CLAN_REFRESH) {
        
        if (m_pCityInfo->clanInfo.id == ClanMgr::getSingleton().m_myclan.id) {
            m_pCityInfo->clanInfo = ClanMgr::getSingleton().m_myclan;
        }
        else {
            m_pCityInfo->clanInfo = ClanMgr::getSingleton().m_otherclan;
        }
        
        m_pLoading->setVisible(false);
        refreshInfo();
    }
    else if (notifyid == NOTIFY_CLAN_CHANGE) {

    }
    else if (notifyid == NOTIFY_CLANWAR_SIGNSUCCESS) {
        int signnums = *((int*)pVal);
        
        m_pCityInfo->cursignnums = signnums;
        
        refreshInfo();
    }
    else if (notifyid == NOTIFY_CLANWAR_ATTACKCLANS) {
        
        m_pCityInfo->cursignnums = ClanWarMgr::getSingleton().m_warAttackClanList.size();
        
        m_pLoading->setVisible(false);
        refreshInfo();
    }
    
    return true;
}

void ClanWarSignupUI::refreshInfo()
{
    char str[128];
    sprintf(str, "%d/%d", m_pCityInfo->cursignnums, m_pCityInfo->maxsignnums);
    
    m_pSignupNums->setString(str);
    
    ClanInfo clanInfo = m_pCityInfo->clanInfo;
    
    m_pCityName->setString(m_pCityInfo->cityname.c_str());
    
    m_pClanName->setString(clanInfo.name.c_str());
    sprintf(str, "%d", clanInfo.cupnums);

    sprintf(str, "clan_icon_%d", clanInfo.flag);
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pClanIcon->setDisplayFrame(pFrame);
    sprintf(str, "clan_national_%d", clanInfo.national);
    pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pClanNational->setDisplayFrame(pFrame);
    
    sprintf(str, "%d", m_pCityInfo->wingems);
    m_pRewardGems->setString(str);
    
    if (m_pCityInfo->clanInfo.id == ClanMgr::getSingleton().m_myclan.id) {
        m_pWinTip->setString(ViewString::getValue("citywar_defend_win_tip").c_str());
    }
    else {
        m_pWinTip->setString(ViewString::getValue("citywar_attack_win_tip").c_str());
    }
    
    sprintf(str, "%d", ClanWarMgr::getSingleton().m_warMinWinStar);
    m_pWinStar->setString(str);

    m_pBtnSignup->setEnabled(false);
    if (m_pCityInfo->cursignnums < m_pCityInfo->maxsignnums
        && ClanWarMgr::getSingleton().m_warSignupCityId == 0
        && ClanWarMgr::getSingleton().m_warDefendCityId == 0
        && ClanMgr::getSingleton().m_myclan.membercurnums >= ClanWarMgr::getSingleton().m_warClanMinMember
        && ClanMgr::getSingleton().m_clanjob == CLAN_JOB_LEADER) {
        m_pBtnSignup->setEnabled(true);
    }
    
    m_pSignupClansView->reloadData();
}

void ClanWarSignupUI::menuClanInfoCallback(CCObject *pSender, CCControlEvent event)
{
    ClanMgr::getSingleton().m_otherclan = m_pCityInfo->clanInfo;
    
    PlayerScene::getSingleton().m_pGameWorld->showClanInfoUI();
}

CCTableViewCell* ClanWarSignupUI::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
    CCTableViewCell *cell = table->dequeueCell();
    if (cell == NULL) {
        cell = new CCTableViewCell();
        cell->autorelease();
    }
    cell->setIdx(idx);
    cell->setAnchorPoint(ccp(0, 0));
    
    int index = ClanWarMgr::getSingleton().m_warAttackClanList.size() - idx - 1;
    ClanInfo info = ClanWarMgr::getSingleton().m_warAttackClanList[index];
    
    ClanWarSignupItem* item = ClanWarSignupItem::create(info, m_pSignupClansView);
    
    item->setTouchControlView(table);
    item->setTag(1);
    cell->addChild(item);
    
    return cell;
}

void ClanWarSignupUI::tableCellTouched(CCTableView *table, CCTableViewCell *cell)
{
   
}

CCSize ClanWarSignupUI::cellSizeForTable(CCTableView *table)
{
    CCSize size = CCSizeMake(960, 84);
    return size;
}

unsigned int ClanWarSignupUI::numberOfCellsInTableView(CCTableView *table)
{
    return ClanWarMgr::getSingleton().m_warAttackClanList.size();
}

void ClanWarSignupUI::scrollViewDidScroll(CCScrollView* view)
{
    
}

void ClanWarSignupUI::menuSignupCallback(CCObject *pSender, CCControlEvent event)
{
    if (ClanMgr::getSingleton().m_myclan.national <= 0) {
        MessageUI::show(MSG_CLAN_NOT_NATIONAL, NULL, ViewString::getValue("clan_national_select_tip").c_str());
    }
    else {
        HttpClient::getSingleton().clanWarSignup(m_pCityInfo->cityid);
    }
}

void ClanWarSignupUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->hideClanWarSignupUI();
}

bool ClanWarSignupUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CityName", CCLabelBMFont*, m_pCityName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanNational", CCSprite*, m_pClanNational);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "SignupEndTime", CCLabelBMFont*, m_pSignupEndTime);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "SignupNums", CCLabelBMFont*, m_pSignupNums);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RewardGems", CCLabelBMFont*, m_pRewardGems);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WinTip", CCLabelBMFont*, m_pWinTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WinStar", CCLabelBMFont*, m_pWinStar);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "View", CCLayer*, m_pView);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnSignup", CCControlButton*, m_pBtnSignup);
    
    return false;
}

SEL_CCControlHandler ClanWarSignupUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSignupCallback", ClanWarSignupUI::menuSignupCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuClanInfoCallback", ClanWarSignupUI::menuClanInfoCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", ClanWarSignupUI::menuCloseCallback);
    
    return NULL;
}

ClanWarSignupItem::ClanWarSignupItem(ClanInfo clanInfo, CCTableView* pTableView)
: m_clanInfo(clanInfo)
, m_pTableView(pTableView)
{
}

ClanWarSignupItem::~ClanWarSignupItem()
{
    
}

ClanWarSignupItem* ClanWarSignupItem::create(ClanInfo clanInfo, CCTableView* pTableView)
{
    ClanWarSignupItem *pRet = new ClanWarSignupItem(clanInfo, pTableView);
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

bool ClanWarSignupItem::init()
{
    if (CUINode::init()) {
        
        CCNode* pSignupItem = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_signup_item.ccbi", this, m_pTableView->getContentSize());
        this->setContentSize(pSignupItem->getContentSize());
        this->addChild(pSignupItem);
        
        refreshInfo();
    }
    
    return true;
}

void ClanWarSignupItem::refreshInfo()
{
    char str[128];
    
    m_pClanName->setString(m_clanInfo.name.c_str());
    
    sprintf(str, "clan_icon_%d", m_clanInfo.flag);
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pClanIcon->setDisplayFrame(pFrame);
    
    sprintf(str, "clan_national_%d", m_clanInfo.national);
    pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pClanNational->setDisplayFrame(pFrame);
    
    sprintf(str, "%d", m_clanInfo.membercurnums);
    m_pClanMemberNums->setString(str);
    
    sprintf(str, "%d", m_clanInfo.cupnums);
    m_pClanCup->setString(str);
}

bool ClanWarSignupItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanNational", CCSprite*, m_pClanNational);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanMemberNums", CCLabelBMFont*, m_pClanMemberNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanCup", CCLabelBMFont*, m_pClanCup);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "SignupTime", CCLabelBMFont*, m_pSignupTime);
   
    return false;
}

SEL_CCControlHandler ClanWarSignupItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    return NULL;
}