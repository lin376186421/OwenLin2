//
//  GroupWarBattleUI.cpp
//  GameClient
//
//  Created by qsun on 14-10-20.
//
//

#include "GroupWarBattleUI.h"
#include "ClanWarBattleMapUI.h"
#include "../coc/ClanMgr.h"
#include "../coc/GroupWarMgr.h"
#include "../coc/PlayerScene.h"
#include "../GameWorldScene.h"

USING_NS_COC;

GroupWarBattleUI::GroupWarBattleUI(ClanCityInfo* pCityInfo)
: m_pLoadingUI(NULL)
, m_pBattleMapUI(NULL)
, m_pBattleLogUI(NULL)
, m_pCityInfo(pCityInfo)
, m_lastWarState(-1)
{
    LogicInfo::getSingleton().addListener(this);
}

GroupWarBattleUI::~GroupWarBattleUI()
{
    LogicInfo::getSingleton().removeListener(this);
    unscheduleUpdate();
}

GroupWarBattleUI* GroupWarBattleUI::create(ClanCityInfo* pCityInfo)
{
    GroupWarBattleUI *pRet = new GroupWarBattleUI(pCityInfo);
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

bool GroupWarBattleUI::init()
{
    if (CUINode::init()) {
        CCNode* pBattleUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("groupwar_battle.ccbi", this);
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
        
        if (PlayerScene::getSingleton().m_backHomeView != BACK_HOME_GROUPWAR_BATTLE) {
            
            GroupWarMgr::getSingleton().m_warAttackClanList.clear();
            
            GroupWarMgr::getSingleton().m_selectDefendIndex = 0;
            GroupWarMgr::getSingleton().m_selectAttackClanId = 0;
            HttpClient::getSingleton().getGroupWarBattleInfo(m_pCityInfo->cityid, 0);
            
            m_pLoadingUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading3.ccbi", this);
            this->addChild(m_pLoadingUI, 10);
        }
        else {
            
            if (GroupWarMgr::getSingleton().m_selectAttackClanId > 0) {
                HttpClient::getSingleton().getGroupWarBattleInfo(m_pCityInfo->cityid, GroupWarMgr::getSingleton().m_selectAttackClanId);
                
                m_pLoadingUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading1.ccbi", this);
                m_pLoadingUI->setPosition(ccp(m_pAttackClanView->getContentSize().width + (this->getContentSize().width-m_pAttackClanView->getContentSize().width)/2, this->getContentSize().height/2));
                this->addChild(m_pLoadingUI);
            }
        }
        
        m_pStarLevelNode->setVisible(false);
        m_pAttackClanNums->setString("");
        
        if (GroupWarMgr::getSingleton().getWarState() >= GROUPWAR_STATE_BATTLE_SCORE) {
            showBattleLog();
        }
        
        refreshInfo();
        scheduleUpdate();
    }
    
    return true;
}

void GroupWarBattleUI::onEnter()
{
    CUINode::onEnter();
    
    m_pAttackClanList->reloadData();
    m_pBattleMapUI->refreshMap(GROUPWAR_CITY);
}

void GroupWarBattleUI::onExit()
{
    CUINode::onExit();
}

void GroupWarBattleUI::update(float dt)
{
    int time = getCurrentTime();
    
    if (GroupWarMgr::getSingleton().getWarState() == GROUPWAR_STATE_BATTLE_PREPARE) {
        m_pBattleEndTime->setString(formatTime(GroupWarMgr::getSingleton().m_warStartTime-time, 3, true).c_str());
    }
    else if (GroupWarMgr::getSingleton().getWarState() == GROUPWAR_STATE_BATTLE_START) {
        m_pBattleEndTime->setString(formatTime(GroupWarMgr::getSingleton().m_warEndTime-time, 3, true).c_str());
    }
    
    if (m_lastWarState != GroupWarMgr::getSingleton().getWarState()) {
        refreshInfo();
        
        m_lastWarState = GroupWarMgr::getSingleton().getWarState();
    }
}

void GroupWarBattleUI::refreshInfo()
{
    char str[128];
    
    ClanWarAttackClanInfo* pAttackClanInfo = GroupWarMgr::getSingleton().getAttackClanInfo(GroupWarMgr::getSingleton().m_selectAttackClanId);
    
    sprintf(str, "clan_icon_%d", m_pCityInfo->clanInfo.flag);
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pClanIcon->setDisplayFrame(pFrame);
    m_pClanName->setString(m_pCityInfo->clanInfo.name.c_str());
    
    ClanWarAttackInfo warAttackInfo = GroupWarMgr::getSingleton().m_warAttackInfo;
    
    sprintf(str, "%d", warAttackInfo.winStar);
    m_pWinStar->setString(str);
    
    int totalstar = GroupWarMgr::getSingleton().m_warStarScore[2].starNums;
    
    float percent = warAttackInfo.winStar/(float)totalstar;
    changeProgress(m_pStarProgress, percent);
    
    for (int i = 0; i < 3; i ++) {
        if (warAttackInfo.winStar < GroupWarMgr::getSingleton().m_warStarScore[i].starNums) {
            pFrame = ImgMgr::getSingleton().getFrame("league_item_0");
        }
        else {
            pFrame = ImgMgr::getSingleton().getFrame("league_item_1");
        }
        
        ((CCSprite*)m_pBtnStarLevel[i]->getChildByTag(1))->setDisplayFrame(pFrame);

        percent = GroupWarMgr::getSingleton().m_warStarScore[i].starNums/(float)totalstar;
        m_pBtnStarLevel[i]->setPositionX(m_pStarProgress->getPositionX() + percent * (m_pStarProgress->getPreferredSize().width*m_pStarProgress->getScaleX()));
    }
    
    int time = getCurrentTime();
    
    if (GroupWarMgr::getSingleton().getWarState() == GROUPWAR_STATE_BATTLE_PREPARE) {
        m_pBattleTimeTip->setString(ViewString::getValue("citywar_state_start_time_tip").c_str());
        m_pBattleEndTime->setString(formatTime(GroupWarMgr::getSingleton().m_warStartTime-time, 3, true).c_str());
    }
    else if (GroupWarMgr::getSingleton().getWarState() == GROUPWAR_STATE_BATTLE_START) {
        m_pBattleTimeTip->setString(ViewString::getValue("citywar_state_end_time_tip").c_str());
        m_pBattleEndTime->setString(formatTime(GroupWarMgr::getSingleton().m_warEndTime-time, 3, true).c_str());
    }
    else {
        m_pBattleTimeTip->setString(ViewString::getValue("citywar_state_end_tip").c_str());
        m_pBattleEndTime->setVisible(false);
    }
    
    m_pBtnAttack->setVisible(false);
    if (GroupWarMgr::getSingleton().getWarState() == GROUPWAR_STATE_BATTLE_START &&
        (pAttackClanInfo && pAttackClanInfo->id == ClanMgr::getSingleton().m_myclan.id))
    {
        m_pBtnAttack->setVisible(true);
    }
    
    m_pBtnLog->getChildByTag(1)->setVisible(false);
    if (pAttackClanInfo && pAttackClanInfo->id == ClanMgr::getSingleton().m_myclan.id &&
        GroupWarMgr::getSingleton().m_warLogTipStarNums != pAttackClanInfo->winstar) {
        m_pBtnLog->getChildByTag(1)->setVisible(true);
    }
}

void GroupWarBattleUI::selectStage(int index)
{
    m_pBtnPrevPlayer->setEnabled(false);
    m_pBtnNextPlayer->setEnabled(false);
    
    if (GroupWarMgr::getSingleton().m_warDefenseUserList.size() > 0) {
        
        if (index > 0) {
            m_pBtnPrevPlayer->setEnabled(true);
        }
        if (index < GroupWarMgr::getSingleton().m_warDefenseUserList.size() - 1) {
            m_pBtnNextPlayer->setEnabled(true);
        }
        
        ClanWarUserInfo userInfo = GroupWarMgr::getSingleton().m_warDefenseUserList[index];
        GroupWarMgr::getSingleton().m_warUserInfo = userInfo;
        
        char str[128];
        sprintf(str, "%d", index + 1);
        m_pRankingId->setString(str);
        
        m_pPlayerName->setString(userInfo.uname.c_str());
        
        sprintf(str, "%d", (int)userInfo.goldex);
        m_pLootGold->setString(str);
        
        sprintf(str, "%d", (int)userInfo.oilex);
        m_pLootOil->setString(str);
        
        sprintf(str, "LV %d, %d%%", userInfo.defenseLevel, (int)(userInfo.defenseRatio*100));
        m_pDefenseLevel->setString(str);
    }
}

void GroupWarBattleUI::showBattleLog()
{
    GroupWarMgr::getSingleton().m_warBattleLog.clear();
    
    if (m_pBattleLogUI == NULL) {
        m_pBattleLogUI = CUINode::createWithFile("clanwar_battlelog.ccbi", this);
        m_pBattleLogList = CCTableView::create(this, m_pBattleLogView->getContentSize());
        m_pBattleLogList->setDirection(kCCScrollViewDirectionVertical);
        m_pBattleLogList->setAnchorPoint(ccp(0, 0));
        m_pBattleLogList->setDelegate(this);
        m_pBattleLogView->addChild(m_pBattleLogList);
        
        CCLabelBMFont* pTitle = (CCLabelBMFont*)m_pBattleLogUI->getContainer()->getChildByTag(1);
        pTitle->setString(ViewString::getValue("cityinvasion_title").c_str());
        
        m_pBattleLogUI->setTargetTouch(true);
        m_pBattleLogUI->setPositionX(m_pAttackClanView->getContentSize().width);
        this->addChild(m_pBattleLogUI);
    }
    
    if (GroupWarMgr::getSingleton().m_selectAttackClanId > 0) {
        
        if (m_pLoadingUI == NULL) {
            m_pLoadingUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading1.ccbi", this);
            m_pLoadingUI->setPosition(ccp(m_pAttackClanView->getContentSize().width + (this->getContentSize().width-m_pAttackClanView->getContentSize().width)/2, this->getContentSize().height/2));
            this->addChild(m_pLoadingUI);
            
            m_pBattleLogView->getChildByTag(1)->setVisible(false);
        }
        
        if (GroupWarMgr::getSingleton().m_selectAttackClanId == ClanMgr::getSingleton().m_myclan.id) {
            
            ClanWarAttackClanInfo* pAttackClanInfo = GroupWarMgr::getSingleton().getAttackClanInfo(GroupWarMgr::getSingleton().m_selectAttackClanId);
            
            if (pAttackClanInfo) {
                GroupWarMgr::getSingleton().m_warLogTipStarNums = pAttackClanInfo->winstar;
            }
        }
        
        HttpClient::getSingleton().getGroupWarBattleLog(m_pCityInfo->cityid, GroupWarMgr::getSingleton().m_selectAttackClanId);
    }
}

void GroupWarBattleUI::hideBattleLog()
{
    this->removeChild(m_pBattleLogUI);
    m_pBattleLogUI = NULL;
}

bool GroupWarBattleUI::onNotify(int notifyid, const void* pVal)
{
    if (notifyid == NOTIFY_GROUPWAR_DEFENSEINFO) {
        
        if (m_pLoadingUI) {
            this->removeChild(m_pLoadingUI);
            m_pLoadingUI = NULL;
        }
        
        m_pBattleMapUI->refreshMap(GROUPWAR_CITY);
    }
    else if (notifyid == NOTIFY_GROUPWAR_ATTACKCLANS) {
        
        if (GroupWarMgr::getSingleton().m_warAttackClanList.size() > 0) {
            
            int selectAttackClanId = GroupWarMgr::getSingleton().m_selectAttackClanId;
            
            if (GroupWarMgr::getSingleton().m_selectAttackClanId == 0) {
                
                ClanWarAttackClanInfo* pAttackClanInfo = GroupWarMgr::getSingleton().getAttackClanInfo(ClanMgr::getSingleton().m_myclan.id);
                if (pAttackClanInfo == NULL) {
                    pAttackClanInfo = &GroupWarMgr::getSingleton().m_warAttackClanList[0];
                }
                
                GroupWarMgr::getSingleton().m_selectAttackClanId = pAttackClanInfo->id;
            }

            CCPoint offset = m_pAttackClanList->getContentOffset();
            
            m_pAttackClanList->reloadData();
            if (selectAttackClanId > 0) {
                m_pAttackClanList->setContentOffset(offset);
            }
        }
        
        if (GroupWarMgr::getSingleton().getWarState() >= GROUPWAR_STATE_BATTLE_SCORE) {
            m_pBattleLogView->getChildByTag(1)->setVisible(false);
            HttpClient::getSingleton().getGroupWarBattleLog(m_pCityInfo->cityid, GroupWarMgr::getSingleton().m_selectAttackClanId);
        }
        
        char str[128];
        int attackClanNums = *((int*)pVal);
        
        sprintf(str, "%d", attackClanNums);
        m_pAttackClanNums->setString(str);
        
        refreshInfo();
    }
    else if (notifyid == NOTIFY_GROUPWAR_BATTLELOG) {
        
        if (m_pLoadingUI) {
            this->removeChild(m_pLoadingUI);
            m_pLoadingUI = NULL;
        }
        
        if (m_pBattleLogUI) {
            m_pBattleLogView->getChildByTag(1)->setVisible(false);
            if (GroupWarMgr::getSingleton().m_warBattleLog.size() == 0) {
                m_pBattleLogView->getChildByTag(1)->setVisible(true);
            }
            
            m_pBattleLogList->reloadData();
        }
    }
    else if (notifyid == NOTIFY_GROUPWAR_BATTLEINFO) {
        
        refreshInfo();
    }
    else if (notifyid == NOTIFY_GROUPWAR_ATTACKUSER)
    {
    }
    
    return true;
}

CCTableViewCell* GroupWarBattleUI::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
    CCTableViewCell *cell = table->dequeueCell();
    if (cell == NULL) {
        cell = new CCTableViewCell();
        cell->autorelease();
    }
    cell->setIdx(idx);
    cell->setAnchorPoint(ccp(0, 0));
    
    if (table == m_pAttackClanList) {
        
        int rankIdx = numberOfCellsInTableView(table) - idx;
        
        int emptyCell = 21;
        
        if (rankIdx == emptyCell) {
            //empty cell
        }
        else{
            
            rankIdx = rankIdx - 1;
            if (rankIdx >= emptyCell) {
                rankIdx -= 1;
            }
        
            ClanWarAttackClanInfo* pAttackClanInfo = &GroupWarMgr::getSingleton().m_warAttackClanList[rankIdx];
            
            GroupWarAttackClanItem* pAttackClanItem = GroupWarAttackClanItem::create(pAttackClanInfo->rankingid, table, pAttackClanInfo);
            pAttackClanItem->setTag(1);
            pAttackClanItem->setTouchControlView(table);
            cell->addChild(pAttackClanItem);
            
            if (pAttackClanInfo->id == GroupWarMgr::getSingleton().m_selectAttackClanId) {
                pAttackClanItem->onSelected(true);
            }
        }
    }
    else if (table == m_pBattleLogList) {
        
        int index = GroupWarMgr::getSingleton().m_warBattleLog.size() - idx - 1;
        
        GroupWarFightLog msg = GroupWarMgr::getSingleton().m_warBattleLog[index];
        
        GroupWarBattleLogItem* pBattleLogItem = GroupWarBattleLogItem::create(index, msg, m_pBattleLogList->getViewSize());
        pBattleLogItem->setTag(1);
        pBattleLogItem->setTouchControlView(table);
        cell->addChild(pBattleLogItem);
    }
    
    return cell;
}

void GroupWarBattleUI::tableCellTouched(CCTableView *table, CCTableViewCell *cell)
{
    if (table == m_pAttackClanList) {
        
        m_pBtnAttack->setVisible(false);
        
        GroupWarAttackClanItem* pAttackClanItem = (GroupWarAttackClanItem*)cell->getChildByTag(1);
        pAttackClanItem->onSelected(true);
        
        GroupWarMgr::getSingleton().m_selectDefendIndex = 0;
        GroupWarMgr::getSingleton().m_selectAttackClanId = pAttackClanItem->m_pAttackClanInfo->id;
        
        HttpClient::getSingleton().getGroupWarBattleInfo(m_pCityInfo->cityid, GroupWarMgr::getSingleton().m_selectAttackClanId);
        
        if (m_pBattleLogUI) {
            
            m_pBattleLogView->getChildByTag(1)->setVisible(false);
            
            HttpClient::getSingleton().getGroupWarBattleLog(m_pCityInfo->cityid, GroupWarMgr::getSingleton().m_selectAttackClanId);
        }
        
        if (m_pLoadingUI == NULL) {
            m_pLoadingUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading1.ccbi", this);
            m_pLoadingUI->setPosition(ccp(m_pAttackClanView->getContentSize().width + (this->getContentSize().width-m_pAttackClanView->getContentSize().width)/2, this->getContentSize().height/2));
            this->addChild(m_pLoadingUI);
        }
    }
}

CCSize GroupWarBattleUI::cellSizeForTable(CCTableView *table)
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

unsigned int GroupWarBattleUI::numberOfCellsInTableView(CCTableView *table)
{
    if (table == m_pAttackClanList) {
        
        int size = GroupWarMgr::getSingleton().m_warAttackClanList.size();
        
        if (size > 20) { // top 20
            size += 1;
        }
        
        return size;
    }
    else if (table == m_pBattleLogList) {
        
        return GroupWarMgr::getSingleton().m_warBattleLog.size();
    }
    
    return 0;
}

void GroupWarBattleUI::scrollViewDidScroll(CCScrollView* view)
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

bool GroupWarBattleUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    m_dragOffset = m_pMapView->getContentOffset();
    
    return true;
}

void GroupWarBattleUI::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

void GroupWarBattleUI::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    m_pStarLevelNode->setVisible(false);
}

void GroupWarBattleUI::menuStarLevelCallback(CCObject* pSender, CCControlEvent event)
{
    int tag = ((CCNode*)pSender)->getTag();
 
    int posX = ((CCNode*)pSender)->getPositionX();
    int offX = 50;
    
    if (event == CCControlEventTouchDown) {
        
        CCSprite* pArrow = ((CCSprite*)m_pStarLevelNode->getChildByTag(0));
        
        if (posX >= m_pStarProgress->getContentSize().width*m_pStarProgress->getScaleX() - m_pStarLevelNode->getContentSize().width) {
            
            pArrow->setFlipX(true);
            pArrow->setPositionX(m_pStarLevelNode->getContentSize().width+pArrow->getContentSize().width*pArrow->getScaleX());
            
            offX = -offX - m_pStarLevelNode->getContentSize().width;
        }
        else {
            pArrow->setFlipX(false);
            pArrow->setPositionX(0);
        }
        
        m_pStarLevelNode->setPositionX(posX + offX);
        
        char str[128];
        sprintf(str, "%d", GroupWarMgr::getSingleton().m_warStarScore[tag-1].starLevel);
        ((CCLabelBMFont*)m_pStarLevelNode->getChildByTag(1))->setString(str);
        
        sprintf(str, "%d", GroupWarMgr::getSingleton().m_warStarScore[tag-1].starNums);
        ((CCLabelBMFont*)m_pStarLevelNode->getChildByTag(2))->setString(str);
        
        sprintf(str, "%d", GroupWarMgr::getSingleton().m_warStarScore[tag-1].rewardGems);
        ((CCLabelBMFont*)m_pStarLevelNode->getChildByTag(3))->setString(str);
        
        m_pStarLevelNode->setVisible(true);
    }
}

void GroupWarBattleUI::menuPrevPlayerCallback(CCObject* pSender, CCControlEvent event)
{
    int index = GroupWarMgr::getSingleton().m_selectDefendIndex;
    if (index > 0) {
        index --;
    }
    
    m_pBattleMapUI->onSelectStage(index);
}

void GroupWarBattleUI::menuNextPlayerCallback(CCObject* pSender, CCControlEvent event)
{
    int index = GroupWarMgr::getSingleton().m_selectDefendIndex;
    if (index < GroupWarMgr::getSingleton().m_warDefenseUserList.size() - 1) {
        index ++;
    }
    
    m_pBattleMapUI->onSelectStage(index);
}

void GroupWarBattleUI::menuLogInfoCallback(CCObject *pSender, CCControlEvent event)
{
    showBattleLog();
}

void GroupWarBattleUI::menuCloseLogCallback(CCObject *pSender, CCControlEvent event)
{
    hideBattleLog();
    
    if (!GroupWarMgr::getSingleton().isWarStateBattle()) {
        PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->hideGroupWarBattleUI();
    }
}

void GroupWarBattleUI::menuAttackCallback(CCObject *pSender, CCControlEvent event)
{
    LogicInfo::getSingleton().refurbishFightSoldierInfo();
    if(LogicInfo::getSingleton().m_lstFightSoldier.size() == 0)
    {
        MessageUI::show(MSG_NO_SOLDIER, "", ViewString::getSingleton().getValue("no_soldier").c_str());
        return;
    }
    
    if (GroupWarMgr::getSingleton().m_selectAttackClanId != ClanMgr::getSingleton().m_myclan.id) {
        return;
    }
    
    ClanWarUserInfo userInfo = GroupWarMgr::getSingleton().m_warUserInfo;
    
    LogicInfo::getSingleton().findAttackUserEx(userInfo.uid, ATTACK_TYPE_GROUPWAR);
    
    PlayerScene::getSingleton().m_pGameWorld->hideClanWarMapUI();
    PlayerScene::getSingleton().m_pGameWorld->hideMainUI();
    
    PlayerScene::getSingleton().m_backHomeView = BACK_HOME_GROUPWAR_BATTLE;
    
    PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_FINDMATCH);
}

void GroupWarBattleUI::menuVisitCallback(CCObject *pSender, CCControlEvent event)
{
    int index = GroupWarMgr::getSingleton().m_selectDefendIndex;
    ClanWarUserInfo info = GroupWarMgr::getSingleton().m_warDefenseUserList[index];
    
    LogicInfo::getSingleton().viewUserEx(info.uid, ATTACK_TYPE_GROUPWAR);
    
    PlayerScene::getSingleton().m_pGameWorld->hideClanWarMapUI();
    
    PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_NORMAL);
    
    PlayerScene::getSingleton().m_backHomeView = BACK_HOME_GROUPWAR_BATTLE;
}

void GroupWarBattleUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->hideGroupWarBattleUI();
}

bool GroupWarBattleUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CityName", CCLabelBMFont*, m_pCityName);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "AttackClanNums", CCLabelBMFont*, m_pAttackClanNums);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WinStar", CCLabelBMFont*, m_pWinStar);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "StarProgress", CCScale9Sprite*, m_pStarProgress);
    
    char str[128];
    for (int i = 0; i < 3; i ++) {
        sprintf(str, "BtnStarLevel%d", i+1);
        CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, str, CCControlButton*, m_pBtnStarLevel[i]);
    }
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "StarLevelNode", CCNode*, m_pStarLevelNode);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleTimeTip", CCLabelBMFont*, m_pBattleTimeTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleEndTime", CCLabelBMFont*, m_pBattleEndTime);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LootInfo", CCNode*, m_pLootInfoNode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankingId", CCLabelBMFont*, m_pRankingId);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerName", CCLabelTTF*, m_pPlayerName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LootGold", CCLabelBMFont*, m_pLootGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LootOil", CCLabelBMFont*, m_pLootOil);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DefenseLevel", CCLabelBMFont*, m_pDefenseLevel);
    
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

SEL_CCControlHandler GroupWarBattleUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuStarLevelCallback", GroupWarBattleUI::menuStarLevelCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuPrevPlayerCallback", GroupWarBattleUI::menuPrevPlayerCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuNextPlayerCallback", GroupWarBattleUI::menuNextPlayerCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuLogInfoCallback", GroupWarBattleUI::menuLogInfoCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseLogCallback", GroupWarBattleUI::menuCloseLogCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuVisitCallback", GroupWarBattleUI::menuVisitCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuAttackCallback", GroupWarBattleUI::menuAttackCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", GroupWarBattleUI::menuCloseCallback);
    
    return NULL;
}


GroupWarAttackClanItem::GroupWarAttackClanItem(int rankingId, CCTableView* pTableView, ClanWarAttackClanInfo* pAttackClanInfo)
: m_pAttackClanInfo(pAttackClanInfo)
, m_rankingId(rankingId)
, m_pTableView(pTableView)
{
}

GroupWarAttackClanItem::~GroupWarAttackClanItem()
{
}

GroupWarAttackClanItem* GroupWarAttackClanItem::create(int rankingId, CCTableView* pTableView, ClanWarAttackClanInfo* pAttackClanInfo)
{
    GroupWarAttackClanItem *pRet = new GroupWarAttackClanItem(rankingId, pTableView, pAttackClanInfo);
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

bool GroupWarAttackClanItem::init()
{
    if (CUINode::init()) {
        CCNode* pAttackClanItem = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_battle_item_0.ccbi", this, m_pTableView->getContentSize());
        this->setContentSize(pAttackClanItem->getContentSize());
        this->addChild(pAttackClanItem);
        
        char str[128];
        
        sprintf(str, "%d", m_rankingId);
        m_pRankingId->setString(str);
        
        if (m_rankingId == 0) {
            m_pRankingId->setVisible(false);
        }
        
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
            
            m_pLastStarTime->setString(formatTime(m_pAttackClanInfo->laststartime-GroupWarMgr::getSingleton().m_warStartTime, 3, true).c_str());
            
            m_pLastTimeTip->setVisible(true);
            m_pLastStarTime->setVisible(true);
        }
        
        sprintf(str, "%d", m_pAttackClanInfo->winstar);
        m_pWinStar->setString(str);
        
        m_pWinTip->setVisible(false);
        if (m_pAttackClanInfo->winstar > 0) {
            
            if (m_rankingId == 1) {
                
                CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("clan_iconitem_1");
                ((CCSprite*)m_pWinTip->getChildByTag(1))->setDisplayFrame(pFrame);
                
                m_pWinTip->setVisible(true);
            }
            else if (m_rankingId == 2 || m_rankingId == 3){
                
                CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("clan_iconitem_0");
                ((CCSprite*)m_pWinTip->getChildByTag(1))->setDisplayFrame(pFrame);
                
                m_pWinTip->setVisible(true);
            }
        }
    }
    
    return true;
}

void GroupWarAttackClanItem::onSelected(bool isSelect)
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

bool GroupWarAttackClanItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
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

SEL_CCControlHandler GroupWarAttackClanItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    return NULL;
}


GroupWarBattleLogItem::GroupWarBattleLogItem(int rankingId, GroupWarFightLog fightLog, CCSize viewSize)
: m_rankingId(rankingId)
, m_fightLog(fightLog)
, m_viewSize(viewSize)
{
    
}

GroupWarBattleLogItem::~GroupWarBattleLogItem()
{
}

GroupWarBattleLogItem* GroupWarBattleLogItem::create(int rankingId, GroupWarFightLog fightLog, CCSize viewSize)
{
    GroupWarBattleLogItem *pRet = new GroupWarBattleLogItem(rankingId, fightLog, viewSize);
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

bool GroupWarBattleLogItem::init()
{
    if (CUINode::init()) {
        
        CCNode* pBattleLogUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("groupwar_battlelog_item.ccbi", this, m_viewSize);
        this->setContentSize(pBattleLogUI->getContentSize());
        this->addChild(pBattleLogUI);
        
        refreshInfo();
    }
    
    return true;
}

void GroupWarBattleLogItem::refreshInfo()
{
    char str[128];
    
    sprintf(str, "%d", m_rankingId+1);
    m_pRankingId->setString(str);
    
    sprintf(str, "player_mr_%d", m_fightLog.user.military);
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pPlayerMilitary->setDisplayFrame(pFrame);

    m_pPlayerName->setString(m_fightLog.user.name.c_str());
  
    sprintf(str, "%d", m_fightLog.winstar);
    m_pWinStar->setString(str);
    
    sprintf(str, "%d", m_fightLog.battlenums);
    m_pBattleNums->setString(str);
    
    m_pBattleTime->setString(formatTime(m_fightLog.laststartime-GroupWarMgr::getSingleton().m_warStartTime, 3, true).c_str());
}

bool GroupWarBattleLogItem::checkTouchInside(CCObject* pSender)
{
    GroupWarBattleUI* pWarButtleUI = PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->m_pGroupWarBattleUI;
    
    if (isTouchInside(((CCControlButton*)pSender)->getTouchPosition(), pWarButtleUI->m_pBtnCloseLog)) {
        pWarButtleUI->menuCloseLogCallback(pSender, CCControlEventTouchDown);
        return false;
    }
    
    if(!isTouchInside(((CCControlButton*)pSender)->getTouchPosition(), pWarButtleUI->m_pBattleLogView)) {
        return false;
    }
    return true;
}

void GroupWarBattleLogItem::menuReplayCallback(CCObject* pSender, CCControlEvent event)
{

}

bool GroupWarBattleLogItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankingId", CCLabelBMFont*, m_pRankingId);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerMilitary", CCSprite*, m_pPlayerMilitary);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerName", CCLabelTTF*, m_pPlayerName);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WinStar", CCLabelBMFont*, m_pWinStar);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleNums", CCLabelBMFont*, m_pBattleNums);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleTime", CCLabelBMFont*, m_pBattleTime);
    
    return false;
}

SEL_CCControlHandler GroupWarBattleLogItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuReplayCallback", GroupWarBattleLogItem::menuReplayCallback);
    
    return NULL;
}

