//
//  ClanWarBattleUI.cpp
//  GameClient
//
//  Created by qsun on 14-9-4.
//
//

#include "ClanWarBattleUI.h"
#include "ClanWarBattleMapUI.h"
#include "../coc/ClanMgr.h"
#include "../coc/ClanWarMgr.h"
#include "../coc/PlayerScene.h"
#include "../GameWorldScene.h"

USING_NS_COC;

ClanWarBattleUI::ClanWarBattleUI(ClanCityInfo* pCityInfo)
: m_pLoadingUI(NULL)
, m_pBattleMapUI(NULL)
, m_pBattleLogUI(NULL)
, m_pCityInfo(pCityInfo)
, m_lastWarState(-1)
{
    LogicInfo::getSingleton().addListener(this);
}

ClanWarBattleUI::~ClanWarBattleUI()
{
    LogicInfo::getSingleton().removeListener(this);
    unscheduleUpdate();
}

ClanWarBattleUI* ClanWarBattleUI::create(ClanCityInfo* pCityInfo)
{
    ClanWarBattleUI *pRet = new ClanWarBattleUI(pCityInfo);
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

bool ClanWarBattleUI::init()
{
    if (CUINode::init()) {
        CCNode* pBattleUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_battle.ccbi", this);
        this->setContentSize(pBattleUI->getContentSize());
        this->setTargetTouch(true);
        this->addChild(pBattleUI);
        
        m_pAttackClanList = CCTableView::create(this, m_pAttackClanView->getContentSize());
        m_pAttackClanList->setDirection(kCCScrollViewDirectionVertical);
        m_pAttackClanList->setAnchorPoint(ccp(0, 0));
        m_pAttackClanList->setDelegate(this);
        m_pAttackClanView->addChild(m_pAttackClanList);
        
        m_pBattleMapUI = ClanWarBattleMapUI::create();
        m_pMapView->setContainer(m_pBattleMapUI);
        m_pMapView->setDelegate(this);
        
        m_pCityName->setString(m_pCityInfo->cityname.c_str());
        
        if (PlayerScene::getSingleton().m_backHomeView != BACK_HOME_CLANWAR_BATTLE) {

            ClanWarMgr::getSingleton().m_warAttackClanList.clear();
            
            ClanWarMgr::getSingleton().m_selectDefendIndex = 0;
            ClanWarMgr::getSingleton().m_selectAttackClanId = 0;
            HttpClient::getSingleton().getClanWarBattleInfo(m_pCityInfo->cityid, 0);
            
            m_pLoadingUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading3.ccbi", this);
            this->addChild(m_pLoadingUI, 10);
        }
        else {
            
            if (ClanWarMgr::getSingleton().m_selectAttackClanId > 0) {
                HttpClient::getSingleton().getClanWarBattleInfo(m_pCityInfo->cityid, ClanWarMgr::getSingleton().m_selectAttackClanId);
                
                m_pLoadingUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading1.ccbi", this);
                m_pLoadingUI->setPosition(ccp(m_pAttackClanView->getContentSize().width + (this->getContentSize().width-m_pAttackClanView->getContentSize().width)/2, this->getContentSize().height/2));
                this->addChild(m_pLoadingUI);
            }
        }
        
        if (ClanWarMgr::getSingleton().getWarState() >= CLANWAR_STATE_BATTLE_SCORE) {
            showBattleLog();
        }
        
        refreshInfo();
        scheduleUpdate();
    }
    
    return true;
}

void ClanWarBattleUI::onEnter()
{
    CUINode::onEnter();
    
    m_pAttackClanList->reloadData();
    m_pBattleMapUI->refreshMap(CLANWAR_CITY);
}

void ClanWarBattleUI::onExit()
{
    CUINode::onExit();
}

void ClanWarBattleUI::update(float dt)
{
    int time = getCurrentTime();
    
    if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_BATTLE_PREPARE) {
        m_pBattleEndTime->setString(formatTime(ClanWarMgr::getSingleton().m_warStartTime-time, 3, true).c_str());
    }
    else if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_BATTLE_START) {
        m_pBattleEndTime->setString(formatTime(ClanWarMgr::getSingleton().m_warEndTime-time, 3, true).c_str());
    }
    
    if (m_lastWarState != ClanWarMgr::getSingleton().getWarState()) {
        refreshInfo();
        
        m_lastWarState = ClanWarMgr::getSingleton().getWarState();
    }
}

void ClanWarBattleUI::refreshInfo()
{
    char str[128];
    
    ClanWarAttackClanInfo* pAttackClanInfo = ClanWarMgr::getSingleton().getAttackClanInfo(ClanWarMgr::getSingleton().m_selectAttackClanId);
    
    m_pAttackClanIcon->setVisible(false);
    m_pAttackClanName->setVisible(false);
    if (pAttackClanInfo) {
        
        sprintf(str, "clan_icon_%d", pAttackClanInfo->flag);
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pAttackClanIcon->setDisplayFrame(pFrame);
        m_pAttackClanName->setString(pAttackClanInfo->name.c_str());
        
        m_pAttackClanIcon->setVisible(true);
        m_pAttackClanName->setVisible(true);
    }
    
    sprintf(str, "clan_icon_%d", m_pCityInfo->clanInfo.flag);
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pDefendClanIcon->setDisplayFrame(pFrame);
    m_pDefendClanName->setString(m_pCityInfo->clanInfo.name.c_str());
    
    ClanWarAttackInfo warAttackInfo = ClanWarMgr::getSingleton().m_warAttackInfo;
    
    m_pCanAttackNumsTip->setVisible(false);
    m_pCanAttackNums->setVisible(false);
    if (pAttackClanInfo && pAttackClanInfo->id == ClanMgr::getSingleton().m_myclan.id) {
        m_pCanAttackNumsTip->setVisible(true);
        m_pCanAttackNums->setVisible(true);
        
        sprintf(str, "%d", warAttackInfo.canAttackNums);
        m_pCanAttackNums->setString(str);
    }
    
    sprintf(str, "%d/%d", warAttackInfo.curAttackNums, warAttackInfo.maxAttackNums);
    m_pAttackNums->setString(str);
    
    sprintf(str, "%d", warAttackInfo.winStar);
    m_pWinStar->setString(str);
    
    int totalstar = ClanWarMgr::getSingleton().m_warDefenseUserList.size()*3;
    
    float percent = warAttackInfo.winStar/(float)totalstar;
    changeProgress(m_pStarProgress, percent);
    
    percent = ClanWarMgr::getSingleton().m_warCityMinWinStar/(float)totalstar;
    m_pWinLine->setPositionX(m_pStarProgress->getPositionX() + percent * (m_pStarProgress->getPreferredSize().width*m_pStarProgress->getScaleX()));
    
    if (warAttackInfo.winStar < ClanWarMgr::getSingleton().m_warCityMinWinStar) {
        
        pFrame = ImgMgr::getSingleton().getFrame("star_b_0");
        ((CCSprite*)m_pWinLine->getChildByTag(0))->setDisplayFrame(pFrame);
    }
    else {
        pFrame = ImgMgr::getSingleton().getFrame("star_b_1");
        ((CCSprite*)m_pWinLine->getChildByTag(0))->setDisplayFrame(pFrame);
    }
    
    sprintf(str, "%d", ClanWarMgr::getSingleton().m_warCityMinWinStar);
    ((CCLabelBMFont*)m_pWinLine->getChildByTag(1))->setString(str);
    
    int time = getCurrentTime();
    
    if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_BATTLE_PREPARE) {
        m_pBattleTimeTip->setString(ViewString::getValue("citywar_state_start_time_tip").c_str());
        m_pBattleEndTime->setString(formatTime(ClanWarMgr::getSingleton().m_warStartTime-time, 3, true).c_str());
    }
    else if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_BATTLE_START) {
        m_pBattleTimeTip->setString(ViewString::getValue("citywar_state_end_time_tip").c_str());
        m_pBattleEndTime->setString(formatTime(ClanWarMgr::getSingleton().m_warEndTime-time, 3, true).c_str());
    }
    else {
        m_pBattleTimeTip->setString(ViewString::getValue("citywar_state_end_tip").c_str());
        m_pBattleEndTime->setVisible(false);
    }
    
    m_pBtnAttack->setVisible(true);
    if (ClanWarMgr::getSingleton().getWarState() != CLANWAR_STATE_BATTLE_START ||
        m_pCityInfo->cityid != ClanWarMgr::getSingleton().m_warSignupCityId ||
        (pAttackClanInfo && pAttackClanInfo->id != ClanMgr::getSingleton().m_myclan.id))
    {
        m_pBtnAttack->setVisible(false);
    }
    
    m_pBtnAttack->setEnabled(false);
    if (warAttackInfo.canAttackNums > 0) {
        m_pBtnAttack->setEnabled(true);
    }
    
    m_pBtnLog->getChildByTag(1)->setVisible(false);
    if (pAttackClanInfo && pAttackClanInfo->id == ClanMgr::getSingleton().m_myclan.id &&
        GroupWarMgr::getSingleton().m_warLogTipStarNums != pAttackClanInfo->winstar) {
        m_pBtnLog->getChildByTag(1)->setVisible(true);
    }
}

void ClanWarBattleUI::selectStage(int index)
{
    m_pBtnPrevPlayer->setEnabled(false);
    m_pBtnNextPlayer->setEnabled(false);
    
    if (ClanWarMgr::getSingleton().m_warDefenseUserList.size() > 0) {
        
        if (index > 0) {
            m_pBtnPrevPlayer->setEnabled(true);
        }
        if (index < ClanWarMgr::getSingleton().m_warDefenseUserList.size() - 1) {
            m_pBtnNextPlayer->setEnabled(true);
        }
        
        ClanWarUserInfo userInfo = ClanWarMgr::getSingleton().m_warDefenseUserList[index];
     
        char str[128];
        sprintf(str, "%d", index + 1);
        m_pRankingId->setString(str);
        
        m_pPlayerName->setString(userInfo.uname.c_str());
        
        sprintf(str, "%d", (int)userInfo.goldex);
        m_pLootGold->setString(str);
        
        sprintf(str, "%d", (int)userInfo.oilex);
        m_pLootOil->setString(str);
    }
}

void ClanWarBattleUI::showBattleLog()
{
    ClanWarMgr::getSingleton().m_warBattleLog.clear();
    
    if (m_pBattleLogUI == NULL) {
        m_pBattleLogUI = CUINode::createWithFile("clanwar_battlelog.ccbi", this);
        m_pBattleLogList = CCTableView::create(this, m_pBattleLogView->getContentSize());
        m_pBattleLogList->setDirection(kCCScrollViewDirectionVertical);
        m_pBattleLogList->setAnchorPoint(ccp(0, 0));
        m_pBattleLogList->setDelegate(this);
        m_pBattleLogView->addChild(m_pBattleLogList);
        
        CCLabelBMFont* pTitle = (CCLabelBMFont*)m_pBattleLogUI->getContainer()->getChildByTag(1);
        pTitle->setString(ViewString::getValue("citybattle_title").c_str());

        m_pBattleLogUI->setTargetTouch(true);
        m_pBattleLogUI->setPositionX(m_pAttackClanView->getContentSize().width);
        this->addChild(m_pBattleLogUI);
    }
    
    if (ClanWarMgr::getSingleton().m_selectAttackClanId > 0) {
        
        if (m_pLoadingUI == NULL) {
            m_pLoadingUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading1.ccbi", this);
            m_pLoadingUI->setPosition(ccp(m_pAttackClanView->getContentSize().width + (this->getContentSize().width-m_pAttackClanView->getContentSize().width)/2, this->getContentSize().height/2));
            this->addChild(m_pLoadingUI);
            
            m_pBattleLogView->getChildByTag(1)->setVisible(false);
        }
        
        if (ClanWarMgr::getSingleton().m_selectAttackClanId == ClanMgr::getSingleton().m_myclan.id) {
            
            ClanWarAttackClanInfo* pAttackClanInfo = ClanWarMgr::getSingleton().getAttackClanInfo(ClanWarMgr::getSingleton().m_selectAttackClanId);
            
            if (pAttackClanInfo) {
                ClanWarMgr::getSingleton().m_warLogTipStarNums = pAttackClanInfo->winstar;
            }
        }
        
         HttpClient::getSingleton().getClanWarBattleLog(m_pCityInfo->cityid, ClanWarMgr::getSingleton().m_selectAttackClanId);
    }
}

void ClanWarBattleUI::hideBattleLog()
{
    this->removeChild(m_pBattleLogUI);
    m_pBattleLogUI = NULL;
}

bool ClanWarBattleUI::onNotify(int notifyid, const void* pVal)
{
    if (notifyid == NOTIFY_CLANWAR_DEFENSEINFO) {
        
        if (m_pLoadingUI) {
            this->removeChild(m_pLoadingUI);
            m_pLoadingUI = NULL;
        }
        
        m_pBattleMapUI->refreshMap(CLANWAR_CITY);
    }
    else if (notifyid == NOTIFY_CLANWAR_ATTACKCLANS) {
        
        if (ClanWarMgr::getSingleton().m_warAttackClanList.size() > 0) {

            int selectAttackClanId = ClanWarMgr::getSingleton().m_selectAttackClanId;
            
            if (ClanWarMgr::getSingleton().m_selectAttackClanId == 0) {
                
                ClanWarAttackClanInfo* pAttackClanInfo = ClanWarMgr::getSingleton().getAttackClanInfo(ClanMgr::getSingleton().m_myclan.id);
                if (pAttackClanInfo == NULL) {
                    pAttackClanInfo = &ClanWarMgr::getSingleton().m_warAttackClanList[0];
                }
                
                ClanWarMgr::getSingleton().m_selectAttackClanId = pAttackClanInfo->id;
            }
            
            CCPoint offset = m_pAttackClanList->getContentOffset();
            
            m_pAttackClanList->reloadData();
            if (selectAttackClanId > 0) {
                m_pAttackClanList->setContentOffset(offset);
            }
        }
        
        if (ClanWarMgr::getSingleton().getWarState() >= CLANWAR_STATE_BATTLE_SCORE) {
            m_pBattleLogView->getChildByTag(1)->setVisible(false);
            HttpClient::getSingleton().getClanWarBattleLog(m_pCityInfo->cityid, ClanWarMgr::getSingleton().m_selectAttackClanId);
        }
        
        refreshInfo();
    }
    else if (notifyid == NOTIFY_CLANWAR_BATTLELOG) {
        
        if (m_pLoadingUI) {
            this->removeChild(m_pLoadingUI);
            m_pLoadingUI = NULL;
        }
        
        if (m_pBattleLogUI) {
            m_pBattleLogView->getChildByTag(1)->setVisible(false);
            if (ClanWarMgr::getSingleton().m_warBattleLog.size() == 0) {
                m_pBattleLogView->getChildByTag(1)->setVisible(true);
            }
            
            m_pBattleLogList->reloadData();
        }
    }
    else if (notifyid == NOTIFY_CLANWAR_BATTLEINFO) {
        
        refreshInfo();
    }
    else if (notifyid == NOTIFY_CLANWAR_ATTACKUSER)
    {
        int uid = *((int*)pVal);
        warAttackUser(uid);
    }
    
    return true;
}

CCTableViewCell* ClanWarBattleUI::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
    CCTableViewCell *cell = table->dequeueCell();
    if (cell == NULL) {
        cell = new CCTableViewCell();
        cell->autorelease();
    }
    cell->setIdx(idx);
    cell->setAnchorPoint(ccp(0, 0));
    
    if (table == m_pAttackClanList) {
        
        int index = ClanWarMgr::getSingleton().m_warAttackClanList.size() - idx - 1;
        
        ClanWarAttackClanInfo* pAttackClanInfo = &ClanWarMgr::getSingleton().m_warAttackClanList[index];
        
        ClanWarAttackClanItem* pAttackClanItem = ClanWarAttackClanItem::create(pAttackClanInfo->rankingid, table, pAttackClanInfo);
        pAttackClanItem->setTag(1);
        pAttackClanItem->setTouchControlView(table);
        cell->addChild(pAttackClanItem);
        
        if (pAttackClanInfo->id == ClanWarMgr::getSingleton().m_selectAttackClanId) {
            pAttackClanItem->onSelected(true);
        }
    }
    else if (table == m_pBattleLogList) {
        
        int index = ClanWarMgr::getSingleton().m_warBattleLog.size() - idx - 1;
        
        FightMsg msg = ClanWarMgr::getSingleton().m_warBattleLog[index];
        
        ClanWarBattleLogItem* pBattleLogItem = ClanWarBattleLogItem::create(msg, m_pBattleLogList->getViewSize());
        pBattleLogItem->setTag(1);
        pBattleLogItem->setTouchControlView(table);
        cell->addChild(pBattleLogItem);
    }
    
    return cell;
}

void ClanWarBattleUI::tableCellTouched(CCTableView *table, CCTableViewCell *cell)
{
    if (table == m_pAttackClanList) {
        
//        int count = table->getContainer()->getChildrenCount();
//        for (int i = 0; i < count; i++) {
//            
//            CCTableViewCell* pCell = (CCTableViewCell*)table->getContainer()->getChildren()->objectAtIndex(i);
//            ClanWarAttackClanItem* pAttackClanItem = (ClanWarAttackClanItem*)pCell->getChildByTag(1);
//            
//            if (pAttackClanItem->m_pAttackClanInfo->id == ClanWarMgr::getSingleton().m_selectAttackClanId) {
//                pAttackClanItem->onSelected(false);
//            }
//        }
        
        ClanWarAttackClanItem* pAttackClanItem = (ClanWarAttackClanItem*)cell->getChildByTag(1);
        pAttackClanItem->onSelected(true);
        
        ClanWarMgr::getSingleton().m_selectDefendIndex = 0;
        ClanWarMgr::getSingleton().m_selectAttackClanId = pAttackClanItem->m_pAttackClanInfo->id;
        
        HttpClient::getSingleton().getClanWarBattleInfo(m_pCityInfo->cityid, ClanWarMgr::getSingleton().m_selectAttackClanId);
        
        if (m_pBattleLogUI) {
            
            m_pBattleLogView->getChildByTag(1)->setVisible(false);
            
            HttpClient::getSingleton().getClanWarBattleLog(m_pCityInfo->cityid, ClanWarMgr::getSingleton().m_selectAttackClanId);
        }
        
        if (m_pLoadingUI == NULL) {
            m_pLoadingUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading1.ccbi", this);
            m_pLoadingUI->setPosition(ccp(m_pAttackClanView->getContentSize().width + (this->getContentSize().width-m_pAttackClanView->getContentSize().width)/2, this->getContentSize().height/2));
            this->addChild(m_pLoadingUI);
        }
    }
}

CCSize ClanWarBattleUI::cellSizeForTable(CCTableView *table)
{
    CCSize size = CCSizeMake(960, 74);
    if (table == m_pAttackClanList) {
        size = CCSizeMake(960, 74);
    }
    else if (table == m_pBattleLogList) {
        size = CCSizeMake(960, 88);
    }
    
    return size;
}

unsigned int ClanWarBattleUI::numberOfCellsInTableView(CCTableView *table)
{
    if (table == m_pAttackClanList) {
     
        return ClanWarMgr::getSingleton().m_warAttackClanList.size();
    }
    else if (table == m_pBattleLogList) {
        
        return ClanWarMgr::getSingleton().m_warBattleLog.size();
    }
    
    return 0;
}

void ClanWarBattleUI::scrollViewDidScroll(CCScrollView* view)
{
    if (view == m_pMapView) {
        if (view->getContentOffset().y >= 0) {
            view->getContainer()->setPosition(ccp(view->getContentOffset().x, 0));
        }
        else if(view->getContentOffset().y <= -(view->getContentSize().height - view->getViewSize().height))
        {
            view->getContainer()->setPosition(ccp(view->getContentOffset().x, -(view->getContentSize().height - view->getViewSize().height)));
        }
        else{
            if (fabs(m_dragOffset.x - view->getContentOffset().x) > 10) {
                m_dragOffset = view->getContentOffset();
            }
        }
    }
}

bool ClanWarBattleUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    m_dragOffset = m_pMapView->getContentOffset();
    
    return true;
}

void ClanWarBattleUI::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

void ClanWarBattleUI::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

void ClanWarBattleUI::warAttackUser(int uid)
{
    LogicInfo::getSingleton().findAttackUserEx(uid, ATTACK_TYPE_CLANWAR);
    
    PlayerScene::getSingleton().m_pGameWorld->hideClanWarMapUI();
    PlayerScene::getSingleton().m_pGameWorld->hideMainUI();
    
    PlayerScene::getSingleton().m_backHomeView = BACK_HOME_CLANWAR_BATTLE;
}

void ClanWarBattleUI::menuPrevPlayerCallback(CCObject* pSender, CCControlEvent event)
{
    int index = ClanWarMgr::getSingleton().m_selectDefendIndex;
    if (index > 0) {
        index --;
    }
    
    m_pBattleMapUI->onSelectStage(index);
}

void ClanWarBattleUI::menuNextPlayerCallback(CCObject* pSender, CCControlEvent event)
{
    int index = ClanWarMgr::getSingleton().m_selectDefendIndex;
    if (index < ClanWarMgr::getSingleton().m_warDefenseUserList.size() - 1) {
        index ++;
    }
    
    m_pBattleMapUI->onSelectStage(index);
}

void ClanWarBattleUI::menuLogInfoCallback(CCObject *pSender, CCControlEvent event)
{
    showBattleLog();
}

void ClanWarBattleUI::menuCloseLogCallback(CCObject *pSender, CCControlEvent event)
{
    hideBattleLog();
    
    if (!ClanWarMgr::getSingleton().isWarStateBattle()) {
        PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->hideClanWarBattleUI();
    }
}

void ClanWarBattleUI::menuAttackCallback(CCObject *pSender, CCControlEvent event)
{
    LogicInfo::getSingleton().refurbishFightSoldierInfo();
    if(LogicInfo::getSingleton().m_lstFightSoldier.size() == 0)
    {
        MessageUI::show(MSG_NO_SOLDIER, "", ViewString::getSingleton().getValue("no_soldier").c_str());
        return;
    }
    
    if (ClanWarMgr::getSingleton().m_selectAttackClanId != ClanMgr::getSingleton().m_myclan.id) {
        return;
    }

    int index = ClanWarMgr::getSingleton().m_selectDefendIndex;
    ClanWarUserInfo info = ClanWarMgr::getSingleton().m_warDefenseUserList[index];
    
    HttpClient::getSingleton().checkClanWarAttack(info.uid);
    
    PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_FINDMATCH);
}

void ClanWarBattleUI::menuVisitCallback(CCObject *pSender, CCControlEvent event)
{
    int index = ClanWarMgr::getSingleton().m_selectDefendIndex;
    ClanWarUserInfo info = ClanWarMgr::getSingleton().m_warDefenseUserList[index];
    
    LogicInfo::getSingleton().viewUserEx(info.uid);
    
    PlayerScene::getSingleton().m_pGameWorld->hideClanWarMapUI();
    
    PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_NORMAL);
    
    PlayerScene::getSingleton().m_backHomeView = BACK_HOME_CLANWAR_BATTLE;
}

void ClanWarBattleUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->hideClanWarBattleUI();
}

bool ClanWarBattleUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "AttackClanIcon", CCSprite*, m_pAttackClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "AttackClanName", CCLabelTTF*, m_pAttackClanName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DefendClanIcon", CCSprite*, m_pDefendClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DefendClanName", CCLabelTTF*, m_pDefendClanName);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CityName", CCLabelBMFont*, m_pCityName);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WinStar", CCLabelBMFont*, m_pWinStar);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "AttackNums", CCLabelBMFont*, m_pAttackNums);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CanAttackNumsTip", CCLabelBMFont*, m_pCanAttackNumsTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CanAttackNums", CCLabelBMFont*, m_pCanAttackNums);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "StarProgress", CCScale9Sprite*, m_pStarProgress);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WinLine", CCNode*, m_pWinLine);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleTimeTip", CCLabelBMFont*, m_pBattleTimeTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleEndTime", CCLabelBMFont*, m_pBattleEndTime);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LootInfo", CCNode*, m_pLootInfoNode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankingId", CCLabelBMFont*, m_pRankingId);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerName", CCLabelTTF*, m_pPlayerName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LootGold", CCLabelBMFont*, m_pLootGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LootOil", CCLabelBMFont*, m_pLootOil);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnAttack", CCControlButton*, m_pBtnAttack);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnPrevPlayer", CCControlButton*, m_pBtnPrevPlayer);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnNextPlayer", CCControlButton*, m_pBtnNextPlayer);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "AttackClanView", CCLayer*, m_pAttackClanView);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnLog", CCControlButton*, m_pBtnLog);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnCloseLog", CCControlButton*, m_pBtnCloseLog);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleLogView", CCLayer*, m_pBattleLogView);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "MapView", CCScrollView*, m_pMapView);
    
    return false;
}

SEL_CCControlHandler ClanWarBattleUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuPrevPlayerCallback", ClanWarBattleUI::menuPrevPlayerCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuNextPlayerCallback", ClanWarBattleUI::menuNextPlayerCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuLogInfoCallback", ClanWarBattleUI::menuLogInfoCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseLogCallback", ClanWarBattleUI::menuCloseLogCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuVisitCallback", ClanWarBattleUI::menuVisitCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuAttackCallback", ClanWarBattleUI::menuAttackCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", ClanWarBattleUI::menuCloseCallback);
    
    return NULL;
}


ClanWarAttackClanItem::ClanWarAttackClanItem(int rankingId, CCTableView* pTableView, ClanWarAttackClanInfo* pAttackClanInfo)
: m_pAttackClanInfo(pAttackClanInfo)
, m_rankingId(rankingId)
, m_pTableView(pTableView)
{
}

ClanWarAttackClanItem::~ClanWarAttackClanItem()
{
}

ClanWarAttackClanItem* ClanWarAttackClanItem::create(int rankingId, CCTableView* pTableView, ClanWarAttackClanInfo* pAttackClanInfo)
{
    ClanWarAttackClanItem *pRet = new ClanWarAttackClanItem(rankingId, pTableView, pAttackClanInfo);
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

bool ClanWarAttackClanItem::init()
{
    if (CUINode::init()) {
        CCNode* pAttackClanItem = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_battle_item_0.ccbi", this, m_pTableView->getContentSize());
        this->setContentSize(pAttackClanItem->getContentSize());
        this->addChild(pAttackClanItem);
        
        char str[128];
        sprintf(str, "%d", m_rankingId);
        m_pRankingId->setString(str);
        
        sprintf(str, "clan_icon_%d", m_pAttackClanInfo->flag);
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pClanIcon->setDisplayFrame(pFrame);
        
        
        
        m_pClanName->setString(m_pAttackClanInfo->name.c_str());
        if(ClanMgr::getSingleton().m_myclan.id ==  m_pAttackClanInfo->id)
        {
            m_pClanName->setColor(ccRED);
        }
        
        m_pLastTimeTip->setVisible(false);
        m_pLastStarTime->setVisible(false);
        if (m_pAttackClanInfo->laststartime > 0) {
            
            m_pLastStarTime->setString(formatTime(m_pAttackClanInfo->laststartime-ClanWarMgr::getSingleton().m_warStartTime, 3, true).c_str());
            
            m_pLastTimeTip->setVisible(true);
            m_pLastStarTime->setVisible(true);
        }
        
        sprintf(str, "%d", m_pAttackClanInfo->winstar);
        m_pWinStar->setString(str);
        
        if (m_pAttackClanInfo->winstar < ClanWarMgr::getSingleton().m_warCityMinWinStar) {
            m_pWinTip->setVisible(false);
        }
        
       
    }
    
    return true;
}

void ClanWarAttackClanItem::onSelected(bool isSelect)
{
    CCSpriteFrame* pFrame = NULL;
    if (isSelect) {
        
        pFrame = ImgMgr::getSingleton().getFrame("clanwar_clanitem_bg_1");
    }
    else {
        pFrame = ImgMgr::getSingleton().getFrame("clanwar_clanitem_bg_0");
    }
    
    m_pClanItemBg->setSpriteFrame(pFrame);
    m_pClanItemBg->setPreferredSize(this->getContentSize());
}

bool ClanWarAttackClanItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanItemBg", CCScale9Sprite*, m_pClanItemBg);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankingId", CCLabelBMFont*, m_pRankingId);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LastTimeTip", CCLabelBMFont*, m_pLastTimeTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LastStarTime", CCLabelBMFont*, m_pLastStarTime);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WinStar", CCLabelBMFont*, m_pWinStar);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WinTip", CCSprite*, m_pWinTip);
    
    return false;
}

SEL_CCControlHandler ClanWarAttackClanItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    return NULL;
}



ClanWarBattleLogItem::ClanWarBattleLogItem(FightMsg fightMsg, CCSize viewSize)
: m_fightMsg(fightMsg)
, m_viewSize(viewSize)
{
    
}

ClanWarBattleLogItem::~ClanWarBattleLogItem()
{
}

ClanWarBattleLogItem* ClanWarBattleLogItem::create(FightMsg fightMsg, CCSize viewSize)
{
    ClanWarBattleLogItem *pRet = new ClanWarBattleLogItem(fightMsg, viewSize);
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

bool ClanWarBattleLogItem::init()
{
    if (CUINode::init()) {
        
        CCNode* pBattleLogUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_battlelog_item.ccbi", this, m_viewSize);
        this->setContentSize(pBattleLogUI->getContentSize());
        this->addChild(pBattleLogUI);
        
        refreshInfo();
    }
    
    return true;
}

void ClanWarBattleLogItem::refreshInfo()
{
    char str[128];
    
    sprintf(str, "player_mr_%d", m_fightMsg.fsi.user[0].military);
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pAttackPlayerMilitary->setDisplayFrame(pFrame);
    
    m_pAttackPlayerName->setString(m_fightMsg.fsi.user[0].name);
    
    sprintf(str, "clan_icon_%d", m_fightMsg.fsi.user[0].clanflag);
    pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pAttackClanIcon->setDisplayFrame(pFrame);
    m_pAttackClanName->setString(m_fightMsg.fsi.user[0].clanname);

    sprintf(str, "player_mr_%d", m_fightMsg.fsi.user[1].military);
    pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pDefendPlayerMilitary->setDisplayFrame(pFrame);
    
    m_pDefendPlayerName->setString(m_fightMsg.fsi.user[1].name);
    
    sprintf(str, "clan_icon_%d", m_fightMsg.fsi.user[1].clanflag);
    pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pDefendClanIcon->setDisplayFrame(pFrame);
    m_pDefendClanName->setString(m_fightMsg.fsi.user[1].clanname);
    
    for (int i = 1; i <= m_fightMsg.star; i ++) {
        
        pFrame = ImgMgr::getSingleton().getFrame("star_b_1");
        ((CCSprite*)m_pBattleStars->getChildByTag(i))->setDisplayFrame(pFrame);
    }
    
    sprintf(str, "%d%%", (int)(m_fightMsg.fsi.progress * 100));
    m_pBattleProgress->setString(str);
    
    m_pBattleTime->setString(formatTimeAgo(m_fightMsg.time, false).c_str());
    
    m_pBattleWinTip->setVisible(false);
    m_pBattleFailedTip->setVisible(false);
    if (m_fightMsg.star > 0) {
        m_pBattleWinTip->setVisible(true);
    }
    else {
        m_pBattleFailedTip->setVisible(true);
    }
    
    m_pBtnReplay->setVisible(false);
    if (m_fightMsg.canReplay()) {
        m_pBtnReplay->setVisible(true);
    }
}

bool ClanWarBattleLogItem::checkTouchInside(CCObject* pSender)
{
    ClanWarBattleUI* pWarButtleUI = PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->m_pClanWarBattleUI;
    
    if (isTouchInside(((CCControlButton*)pSender)->getTouchPosition(), pWarButtleUI->m_pBtnCloseLog)) {
        pWarButtleUI->menuCloseLogCallback(pSender, CCControlEventTouchDown);
        return false;
    }
    
    if(!isTouchInside(((CCControlButton*)pSender)->getTouchPosition(), pWarButtleUI->m_pBattleLogView)) {
        return false;
    }
    return true;
}

void ClanWarBattleLogItem::menuReplayCallback(CCObject* pSender, CCControlEvent event)
{
    if (!checkTouchInside(pSender)) {
        return;
    }
    
    LogicInfo::getSingleton().m_fightReplayType = FIGHT_REPLAY_ATTACK;
    LogicInfo::getSingleton().m_winCups = m_fightMsg.attackCup;
    
    LogicInfo::getSingleton().chg2Replay(m_fightMsg, ATTACK_TYPE_CLANWAR);
    
    PlayerScene::getSingleton().m_pGameWorld->hideClanWarMapUI();
    
    PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_NORMAL);
}

bool ClanWarBattleLogItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "AttackPlayerMilitary", CCSprite*, m_pAttackPlayerMilitary);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "AttackPlayerName", CCLabelTTF*, m_pAttackPlayerName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "AttackClanIcon", CCSprite*, m_pAttackClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "AttackClanName", CCLabelTTF*, m_pAttackClanName);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DefendPlayerMilitary", CCSprite*, m_pDefendPlayerMilitary);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DefendPlayerName", CCLabelTTF*, m_pDefendPlayerName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DefendClanIcon", CCSprite*, m_pDefendClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DefendClanName", CCLabelTTF*, m_pDefendClanName);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleStars", CCSprite*, m_pBattleStars);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleProgress", CCLabelBMFont*, m_pBattleProgress);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleTime", CCLabelBMFont*, m_pBattleTime);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleWinTip", CCNode*, m_pBattleWinTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleFailedTip", CCNode*, m_pBattleFailedTip);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnReplay", CCControlButton*, m_pBtnReplay);
    
    return false;
}

SEL_CCControlHandler ClanWarBattleLogItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuReplayCallback", ClanWarBattleLogItem::menuReplayCallback);
    
    return NULL;
}

