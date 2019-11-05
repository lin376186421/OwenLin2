//
//  BarrackUI.cpp
//  coc4
//
//  Created by chowray on 13-3-27.
//
//

#include "BarrackUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/HttpClient.h"
#include "../coc/PlayerScene.h"
#include "../coc/Building.h"
#include "../coc/LogicInfo.h"
#include "../coc/PersonMgr.h"
#include "../coc/GuideMgr.h"

#include "../coc/SoundMgr.h"
#include "../GameWorldScene.h"

using namespace cocos2d;
using namespace coc;

BarrackUI::BarrackUI()
{
    
}

BarrackUI::~BarrackUI()
{
    LogicInfo::getSingleton().removeListener(this);
}

bool BarrackUI::init()
{
    bool bRet = false;
    
    do
    {
        CC_BREAK_IF(!CUINode::init());
        {
            LogicInfo::getSingleton().addListener(this);
            
            CCNode* barrackUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("barracks_train.ccbi", this);
            this->setContentSize(barrackUI->getContentSize());
            this->setTargetTouch(true);
            
            this->addChild(barrackUI);
            
            CCSize soldierSize(120, 120);
            
            float posX = m_pTrainView->getContentSize().width - soldierSize.width;
            float posY = 0;
            float offX = 0;
            float offY = 0;
            
            for (int index = 0; index < 5 ; index++) {
                SoldierItem* pSoldierItem = SoldierItem::create(-1, true);
                pSoldierItem->m_pBarrackUI = this;
                
                pSoldierItem->setPositionX(posX - index*(soldierSize.width + 5));
                m_pTrainView->addChild(pSoldierItem);
            }
            
            soldierSize = ccp(120, 124);

            posX = ((m_pSoldierView->getContentSize().width / 5) - soldierSize.width)/2;
            posY = m_pSoldierView->getContentSize().height - (m_pSoldierView->getContentSize().height/2 - soldierSize.height)/2 - soldierSize.height;
            offX = m_pSoldierView->getContentSize().width / 5;
            offY = m_pSoldierView->getContentSize().height / 2;
            
            for (int index = 0; index < 10; index ++) {
                SoldierItem* pSoldierItem = SoldierItem::create(10000 + (index+1) * 100 + 1, false);
                pSoldierItem->m_pBarrackUI = this;
                
                pSoldierItem->setPosition(posX + (index%5)*offX, posY);
                m_pSoldierView->addChild(pSoldierItem);
                
                m_lstSoldier.push_back(pSoldierItem);
                if ((index+1) % 5 == 0) {
                    posY -= offY;
                }
            }
        }

        bRet = true;
    } while (0);
    
    CCNode* pNode = m_pBackLayer->getParent();
    pNode->removeChild(m_pBackLayer, false);
    
    CRectLayer* pRectLayer = CRectLayer::create(m_pBackLayer->getContentSize());
    
    pRectLayer->setContentSize(m_pBackLayer->getContentSize());
    pRectLayer->setPosition(m_pBackLayer->getPosition());
    pRectLayer->setZOrder(m_pBackLayer->getZOrder());
    pRectLayer->setAnchorPoint(ccp(0.5, 0.5));
    pRectLayer->ignoreAnchorPointForPosition(false);
    
    m_pBackLayer->setPosition(ccp(pRectLayer->getContentSize().width/2, pRectLayer->getContentSize().height/2));
    
    m_pMaskSprite = CCScale9Sprite::createWithSpriteFrame(ImgMgr::getSingleton().getFrame("uibg_b_2"));
    m_pMaskSprite->setPreferredSize(m_pBackLayer->getContentSize());
    m_pMaskSprite->setPosition(m_pBackLayer->getPosition());
    m_pMaskSprite->setOpacity(0);
    
    pRectLayer->addChild(m_pBackLayer);
    pRectLayer->addChild(m_pMaskSprite, m_pBackLayer->getZOrder()+1);
    
    pNode->addChild(pRectLayer);
    
    m_barrackPos = m_pBackLayer->getPosition();
    
    m_pBarrackInfo = PlayerScene::getSingleton().m_pCurBuilding->getBarrackInfo();
    
    m_pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
    
    int count = 0;
    for (int index = 0; index < LogicInfo::getSingleton().m_mapBarrack.size(); index++) {
        BarrackInfo* pInfo = &LogicInfo::getSingleton().m_mapBarrack[index];
        if (!pInfo->isUpgrade) {
            count ++;
        }
    }
    
    if (count <= 1) {
        m_pBtnPrev->setVisible(false);
        m_pBtnNext->setVisible(false);
    }
    
    refreshQueue();
    this->schedule(schedule_selector(BarrackUI::refreshBarrack), 1.0);
    
    return bRet;
}

void BarrackUI::menuPrevBarrackCallback(CCObject* pSender, CCControlEvent event)
{
    if (m_pBackLayer->numberOfRunningActions() > 0) {
        return;
    }
    
    CCMoveTo* pMoveTo1 = CCMoveTo::create(0.12, ccp(m_barrackPos.x + m_pBackLayer->getContentSize().width, m_barrackPos.y));
    CCCallFunc* callFunc = CCCallFunc::create(this, callfunc_selector(BarrackUI::barrackMoveCallFunc));
    CCMoveTo* pMoveTo2 = CCMoveTo::create(0.12, m_barrackPos);
    
    CCSequence* pSequence = CCSequence::create(pMoveTo1, callFunc, pMoveTo2, NULL);
    m_pBackLayer->runAction(pSequence);
    
    CCFadeIn* pFadeIn = CCFadeIn::create(0.2);
    CCFadeOut* pFadeOut = CCFadeOut::create(0.2);
    
    pSequence = CCSequence::create(pFadeIn, pFadeOut, NULL);
    m_pMaskSprite->runAction(pSequence);
}

void BarrackUI::menuNextBarrackCallback(CCObject* pSender, CCControlEvent event)
{
    if (m_pBackLayer->numberOfRunningActions() > 0) {
        return;
    }
    
    CCMoveTo* pMoveTo1 = CCMoveTo::create(0.12, ccp(m_barrackPos.x - m_pBackLayer->getContentSize().width, m_barrackPos.y));
    CCCallFunc* callFunc = CCCallFunc::create(this, callfunc_selector(BarrackUI::barrackMoveCallFunc));
    CCMoveTo* pMoveTo2 = CCMoveTo::create(0.12, m_barrackPos);
    
    CCSequence* pSequence = CCSequence::create(pMoveTo1, callFunc, pMoveTo2, NULL);
    m_pBackLayer->runAction(pSequence);
    
    CCFadeIn* pFadeIn = CCFadeIn::create(0.2);
    CCFadeOut* pFadeOut = CCFadeOut::create(0.2);
    
    pSequence = CCSequence::create(pFadeIn, pFadeOut, NULL);
    m_pMaskSprite->runAction(pSequence);
}

void BarrackUI::prevBarrack()
{
    int index = 0;
    
    bool find = false;
    bool next = false;
    
    while (!next) {
        BarrackInfo* pInfo = &LogicInfo::getSingleton().m_mapBarrack[index];
        if (pInfo->barrackid == m_pBarrackInfo->barrackid && !pInfo->isUpgrade) {
            find = true;
        }
        
        index--;
        if (index < 0) {
            index = LogicInfo::getSingleton().m_mapBarrack.size() - 1;
        }
        if (find && !LogicInfo::getSingleton().m_mapBarrack[index].isUpgrade) {
            next = true;
        }
        
        if (next) {
            m_pBarrackInfo = &LogicInfo::getSingleton().m_mapBarrack[index];
        }
    }
    
    for(PlayerScene::_BuildingList::iterator it = PlayerScene::getSingleton().m_lstBuilding.begin(); it != PlayerScene::getSingleton().m_lstBuilding.end(); ++it)
    {
        if((*it)->getBarrackInfo() != NULL && (*it)->getBarrackInfo()->barrackid == m_pBarrackInfo->barrackid)
        {
            m_pBuilding = (*it);
            break;
        }
    }
    
    refreshQueue();
}

void BarrackUI::nextBarrack()
{
    int index = 0;
    
    bool find = false;
    bool next = false;

    while (!next) {
        BarrackInfo* pInfo = &LogicInfo::getSingleton().m_mapBarrack[index];
        if (pInfo->barrackid == m_pBarrackInfo->barrackid) {
            find = true;
        }
        
        index ++;
        if (index > LogicInfo::getSingleton().m_mapBarrack.size() - 1) {
            index = 0;
        }
        if (find && !LogicInfo::getSingleton().m_mapBarrack[index].isUpgrade) {
            next = true;
        }
        
        if (find && next) {
            m_pBarrackInfo = &LogicInfo::getSingleton().m_mapBarrack[index];
        }
    }
    
    for(PlayerScene::_BuildingList::iterator it = PlayerScene::getSingleton().m_lstBuilding.begin(); it != PlayerScene::getSingleton().m_lstBuilding.end(); ++it)
    {
        if((*it)->getBarrackInfo() != NULL && (*it)->getBarrackInfo()->barrackid == m_pBarrackInfo->barrackid)
        {
            m_pBuilding = (*it);
            break;
        }
    }
    
    refreshQueue();
}

void BarrackUI::barrackMoveCallFunc()
{
    if (m_pBackLayer->getPosition().x < 0) {
        m_pBackLayer->setPositionX(m_barrackPos.x + m_pBackLayer->getContentSize().width);

        nextBarrack();
    }else{
        m_pBackLayer->setPositionX(m_barrackPos.x - m_pBackLayer->getContentSize().width);
        
        prevBarrack();
    }
}

// a selector callback
void BarrackUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
	PlayerScene::getSingleton().m_pGameWorld->hideBarrackUI();
}

void BarrackUI::menuFinishTrainCallback(CCObject* pSender, CCControlEvent event)
{
    int need = LogicInfo::getSingleton().fastBarrack(m_pBarrackInfo, m_pBuilding);
    
    if (need > 0) {
        MessageUI::show(MSG_NOGEM, NULL, NULL);
        return;
    }
    
    if (GuideMgr::getSingleton().isGuiding()) {
        
        PlayerScene::getSingleton().m_pGameWorld->hideBarrackUI();
        GuideMgr::getSingleton().next();
        return;
    }
        
    refreshQueue();
}

void BarrackUI::refreshQueue()
{
    BarrackInfo* pInfo = m_pBarrackInfo;
    
    if(pInfo == NULL || pInfo->lstSoldier.size() == 0)
    {
        m_pTrainNode->setVisible(false);
        m_pTroopsFullTip->setVisible(false);
    }else{
        m_pTrainNode->setVisible(true);
    }
    
    int totleTime = 0;
    int trainSpace = 0;
    
    for (int index = 0; index < 5; index ++) {
        ((SoldierItem*)m_pTrainView->getChildren()->objectAtIndex(index))->setVisible(false);
    }
    
    for (int index = 0; index < m_lstSoldier.size(); index++) {
        m_lstSoldier[index]->refresh();
    }
    
    char str[128];
    if(pInfo != NULL)
    {
        int i = 0;
        for(std::list<BarrackSoldierInfo>::iterator it = pInfo->lstSoldier.begin(); it != pInfo->lstSoldier.end(); ++it)
        {
            if(i < 5)
            {
                SoldierItem* pSoldierItem = (SoldierItem*)m_pTrainView->getChildren()->objectAtIndex(i);
                pSoldierItem->setVisible(true);
                pSoldierItem->m_soldierId = it->pid;
                pSoldierItem->setTrainState(CCControlStateNormal);
                
                sprintf(str, "%dx", (int)it->numsex);
                pSoldierItem->m_pTrainNums->setString(str);
                
                pSoldierItem->m_pTrainTime->setVisible(false);
                pSoldierItem->m_pTrainProgress->setVisible(false);
                pSoldierItem->m_pTrainProgressBg->setVisible(false);
            }
            
            trainSpace += it->numsex * PersonMgr::getSingleton().getSoldierInfo(it->pid)->housingSpaceex;
            totleTime += it->needtimeex * it->numsex;
            
            if(i == 0)
            {
                totleTime -= it->needtimeex;
                
                int lasttime = pInfo->lasttime;
                
                SoldierItem* pSoldierItem = (SoldierItem*)m_pTrainView->getChildren()->objectAtIndex(i);
                
                pSoldierItem->m_pTrainTime->setVisible(true);
                pSoldierItem->m_pTrainProgress->setVisible(true);
                pSoldierItem->m_pTrainProgressBg->setVisible(true);
                pSoldierItem->m_pTrainTime->setString(formatTime(lasttime).c_str());
                changeProgress(pSoldierItem->m_pTrainProgress, (1 - (float)pInfo->lasttime / it->needtimeex));
            }
            
            ++i;
        }
        
    }
    
    m_pBtnFinishTrain->setVisible(false);
    
    if(pInfo->lstSoldier.size() > 0)
    {
        totleTime += pInfo->lasttime;
        m_pBtnFinishTrain->setVisible(true);
        
        m_pTotalTime->setString(formatTime(totleTime).c_str());
        
        int gem = totleTime / 60 / 4;
        if (gem < 1) gem = 1;
        
        sprintf(str, "%d", gem);
        ((CCLabelBMFont*)m_pBtnFinishTrain->getChildByTag(1))->setString(str);
    }
    
    sprintf(str, ViewString::getSingleton().getValue("barrack_title").c_str(), trainSpace, (int)pInfo->maxnumsex);
    m_pTrainNums->setString(str);
    
    sprintf(str, "%d/%d", LogicInfo::getSingleton().m_curSoldier, LogicInfo::getSingleton().m_maxSoldier);
    m_pTroopsSpace->setString(str);
    
    int curNums = LogicInfo::getSingleton().countTroopSpace();
    
    if(!pInfo->lstSoldier.empty())
    {
        if(curNums + trainSpace > LogicInfo::getSingleton().m_maxSoldier)
        {
            m_pBtnFinishTrain->setEnabled(false);
            m_pTroopsFullTip->setVisible(true);
        }
        else{
            m_pBtnFinishTrain->setEnabled(true);
            m_pTroopsFullTip->setVisible(false);
        }
    }
    
    if (GuideMgr::getSingleton().isGuiding()) {
        if (curNums >= 20) {
            PlayerScene::getSingleton().m_pGameWorld->hideBarrackUI();
            GuideMgr::getSingleton().next();
        }
    }
    
}

bool BarrackUI::onNotify(int notifyid, const void* pVal)
{
    //	if(NOTIFY_GUIDE_REFRESH == notifyid)
    //	{
    //        guide();
    //    }
    
    return true;
}

void BarrackUI::refreshBarrack(float fDelta)
{
    refreshQueue();
}

bool BarrackUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TrainNums", CCLabelBMFont*, m_pTrainNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TotalTime", CCLabelBMFont*, m_pTotalTime);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TroopsSpace", CCLabelBMFont*, m_pTroopsSpace);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TroopsFullTip", CCNode*, m_pTroopsFullTip);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "SoldierView", CCLayer*, m_pSoldierView);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TrainView", CCLayer*, m_pTrainView);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TrainNode", CCNode*, m_pTrainNode);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BackLayer", CCLayer*, m_pBackLayer);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnPrev", CCControlButton*, m_pBtnPrev);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnNext", CCControlButton*, m_pBtnNext);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnFinishTrain", CCControlButton*, m_pBtnFinishTrain);
    
    return NULL;
}

SEL_CCControlHandler BarrackUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuPrevBarrackCallback", BarrackUI::menuPrevBarrackCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuNextBarrackCallback", BarrackUI::menuNextBarrackCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuFinishTrainCallback", BarrackUI::menuFinishTrainCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", BarrackUI::menuCloseCallback);
    
    return NULL;
}


SoldierItem::SoldierItem(int id, bool isTrain)
{
    m_soldierId = id;
    m_isTrain = isTrain;
}

SoldierItem::~SoldierItem()
{
    
}

SoldierItem* SoldierItem::create(int id, bool isTrain)
{
    SoldierItem *pRet = new SoldierItem(id, isTrain);
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

bool SoldierItem::init()
{
    if (CUINode::init()) {
        
        CCNode* soldierNode = NULL;
        if (!m_isTrain) {
            soldierNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("soldier_item.ccbi", this);
            
            int level = 1;
            if(LogicInfo::getSingleton().getMySoldierLevel(m_soldierId) != 0)
            {
                level = (LogicInfo::getSingleton().getMySoldierLevel(m_soldierId)) % 10;
            }
            
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("player_star");
            for(int i = 0; i < level && level > 1; ++i)
            {
                CCSprite* pStar = CCSprite::createWithSpriteFrame(pFrame);
                pStar->setAnchorPoint(ccp(0, 0));
                pStar->setPosition(ccp(8 + 17 * i, 50));
                m_pBtnTrain->addChild(pStar, m_pUnLockTip->getZOrder()-1);
            }
        }else{
            soldierNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("train_item.ccbi", this);
        }
        
        m_pBtnTrain->setTouchEnabled(false);
        this->setContentSize(soldierNode->getContentSize());
        this->setAnchorPoint(ccp(0.5, 0.5));
        this->setTargetTouch(true);
        this->addChild(soldierNode);
        
        m_isUnLocked = true;
        
        return true;
    }
    
    return false;
}

void SoldierItem::onEnter()
{
    CUINode::onEnter();
    
    m_boundsRect = boundingBox();
}

void SoldierItem::onExit()
{
    this->unschedule(schedule_selector(SoldierItem::onHoldTouchCallback));
    
    CUINode::onExit();
}

void SoldierItem::initScheduleTimer()
{
    m_fScheduleDelay   = 0.5;
    m_fScheduleElapsed = 0.0;
    m_nScheduleTimes   = 0;
}

void SoldierItem::refresh()
{
    char str[128];
    if (!m_isTrain) {
        
        BuildingInfo& binfo = m_pBarrackUI->m_pBuilding->m_info;
        BarrackInfo* pInfo = m_pBarrackUI->m_pBarrackInfo;
        
        const SoldierInfo* info = PersonMgr::getSingleton().getSoldierInfo(LogicInfo::getSingleton().getMySoldierLevel(m_soldierId));
        
        if(binfo.buildingid % 100 < info->barrackLevelex)
        {
            m_isUnLocked = true;
            m_pUnLockTip->setVisible(true);
            m_pTrainCost->setVisible(false);
            m_pTrainCostNum->setVisible(false);
            setTrainState(CCControlStateDisabled);
            
            sprintf(str, ViewString::getSingleton().getValue("barrack_level").c_str(), (int)info->barrackLevelex);
            m_pUnLockTip->setString(str);
        }
        else
        {
            m_isUnLocked = false;
            m_pUnLockTip->setVisible(false);
            m_pTrainCost->setVisible(true);
            m_pTrainCostNum->setVisible(true);
            setTrainState(CCControlStateNormal);
            int nums = 0;
            for(list<BarrackSoldierInfo>::iterator it = pInfo->lstSoldier.begin(); it != pInfo->lstSoldier.end(); ++it)
            {
                nums += it->numsex * PersonMgr::getSingleton().getSoldierInfo(it->pid)->housingSpaceex;
            }
            
            if(nums + info->housingSpaceex > pInfo->maxnumsex)
            {
                this->setTrainState(CCControlStateDisabled);
            }
            
            sprintf(str, "%d", (int)info->trainingCost);
            m_pTrainCostNum->setString(str);
            if(info->trainingCost > LogicInfo::getSingleton().getMoney(info->trainingRes))
                m_pTrainCostNum->setColor(ccRED);
            else
                m_pTrainCostNum->setColor(ccWHITE);
            
        }
    }
}

void SoldierItem::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, getTouchPriority(), false);
}

bool SoldierItem::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    if (!this->isVisible()) {
        return true;
    }
    if (m_state == CCControlStateDisabled)
    {
        return true;
    }
    
    if (m_pBarrackUI->m_pBackLayer->numberOfRunningActions() > 0) {
        return true;
    }
    
    initScheduleTimer();
    
    if (isTouchInBounds(pTouch->getStartLocation()))
    {
        if (!m_isTrain)
        {
            if (!m_isUnLocked)
            {
                this->setScale(0.9);
                
                if(this->addSoldierToQueue())
                {
                    this->schedule(schedule_selector(SoldierItem::onHoldTouchCallback));
                }
            }
        }
        else
        {
            this->deleteSoldier();
            this->schedule(schedule_selector(SoldierItem::onHoldTouchCallback));
        }
        SoundMgr::getSingleton().playButtonSound(NULL);

    }
    
    return true;
}

void SoldierItem::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

void SoldierItem::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    if (isTouchInBounds(pTouch->getStartLocation()))
    {
        if (!m_isTrain)
        {
            this->setScale(1.0);
        }
        this->unschedule(schedule_selector(SoldierItem::onHoldTouchCallback));
    }
}

void SoldierItem::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    if (!m_isTrain)
    {
        this->setScale(1.0);
    }
    this->unschedule(schedule_selector(SoldierItem::onHoldTouchCallback));
}

void SoldierItem::onHoldTouchCallback(float t)
{
    if (!this->isVisible()) {
        this->unschedule(schedule_selector(SoldierItem::onHoldTouchCallback));
        return;
    }
    
    if (m_pBarrackUI->m_pBackLayer->numberOfRunningActions() > 0) {
        this->unschedule(schedule_selector(SoldierItem::onHoldTouchCallback));
        return;
    }
    
    m_fScheduleElapsed+=t;
    if (m_fScheduleElapsed > m_fScheduleDelay)
    {
        int nLoopTimes;
        if (m_nScheduleTimes<10)
        {
            nLoopTimes = 1;
        } else if(m_nScheduleTimes<22)
        {
            nLoopTimes = 3;
        }
        else if(m_nScheduleTimes<42)
        {
            nLoopTimes = 4;
        } else
        {
            nLoopTimes = 5;
        }
        for (int i = 0; i<nLoopTimes; i++)
        {
            if (!m_isTrain) {
                if(!addSoldierToQueue()){
                    this->unschedule(schedule_selector(SoldierItem::onHoldTouchCallback));
                    break;
                }
            }else{
                deleteSoldier();
            }
            m_nScheduleTimes++;
            m_fScheduleElapsed = 0.0;
            m_fScheduleDelay = 0.1;
        }
        SoundMgr::getSingleton().playButtonSound(NULL);
    }
}

bool SoldierItem::addSoldierToQueue()
{
    BarrackInfo* pInfo = m_pBarrackUI->m_pBarrackInfo;
    const SoldierInfo* info = PersonMgr::getSingleton().getSoldierInfo(LogicInfo::getSingleton().getMySoldierLevel(m_soldierId));
    
    int nums = 0;
    for(std::list<BarrackSoldierInfo>::iterator it = pInfo->lstSoldier.begin(); it != pInfo->lstSoldier.end(); ++it)
    {
        nums += it->numsex * PersonMgr::getSingleton().getSoldierInfo(it->pid)->housingSpaceex;
    }
    
    if(nums + info->housingSpaceex > pInfo->maxnumsex)
    {
        setTrainState(CCControlStateDisabled);
        return false;
    }
    
    if(LogicInfo::getSingleton().addSoldier2Barrack(pInfo->barrackid, LogicInfo::getSingleton().getMySoldierLevel(m_soldierId)) == MSG_NOELIXIR)
    {
        (PlayerScene::getSingleton().m_pGameWorld)->m_pNotifyUI->addNote(ViewString::getSingleton().getValue("barrack_nooil").c_str());
        
        GuideMgr::getSingleton().m_pActiveBtn = NULL;
        return false;
    }
    
    m_pBarrackUI->refreshQueue();
    
    if(LogicInfo::getSingleton().m_curSoldier >= LogicInfo::getSingleton().m_maxSoldier)
    {
        if (GuideMgr::getSingleton().isGuiding()) {
            
            GuideMgr::getSingleton().next();
            return false;
        }
    }
    
    return true;
}

bool SoldierItem::deleteSoldier()
{
    BarrackInfo* pInfo = m_pBarrackUI->m_pBarrackInfo;
    LogicInfo::getSingleton().delSoldier2Barrack(pInfo->barrackid, m_soldierId);
    
    m_pBarrackUI->refreshQueue();
    
    return true;
}

void SoldierItem::setTrainState(CCControlState state)
{
    char str[128];
    if (state == CCControlStateNormal) {
        sprintf(str, "player1_%d", m_soldierId / 100);
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pBtnTrain->setBackgroundSpriteFrameForState(pFrame, CCControlStateNormal);
    }
    else if(state == CCControlStateDisabled){
        
        if (!m_isUnLocked) {
            sprintf(str, "player1_%d_0", m_soldierId / 100);
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
            m_pBtnTrain->setBackgroundSpriteFrameForState(pFrame, CCControlStateNormal);
        }else{
            sprintf(str, "player1_%d_1", m_soldierId / 100);
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
            m_pBtnTrain->setBackgroundSpriteFrameForState(pFrame, CCControlStateNormal);
        }
    }
    
    m_state = state;
}

void SoldierItem::menuInfoCallback(CCObject* pSender, CCControlEvent event)
{
    if (m_pBtnTrain == GuideMgr::getSingleton().m_pActiveBtn) {
        return;
    }
    
    if (m_pBarrackUI->m_pBackLayer->numberOfRunningActions() > 0) {
        return;
    }
    
    LogicInfo::getSingleton().m_showinfo_soldierid = m_soldierId;
    
    m_pBarrackUI->setVisible(false);
    (PlayerScene::getSingleton().m_pGameWorld)->showSoldierUI();
}

bool SoldierItem::isTouchInBounds(CCPoint touchLocation)
{
    touchLocation = this->getParent()->convertToNodeSpace(touchLocation);
    return m_boundsRect.containsPoint(touchLocation);
}

bool SoldierItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TrainCost", CCSprite*, m_pTrainCost);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TrainCostNum", CCLabelBMFont*, m_pTrainCostNum);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UnLockTip", CCLabelBMFont*, m_pUnLockTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TrainNums", CCLabelBMFont*, m_pTrainNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TrainProgress", CCSprite*, m_pTrainProgress);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TrainProgressBg", CCSprite*, m_pTrainProgressBg);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TrainTime", CCLabelBMFont*, m_pTrainTime);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnInfo", CCControlButton*, m_pBtnInfo);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnTrain", CCControlButton*, m_pBtnTrain);
    
    return NULL;
}

SEL_CCControlHandler SoldierItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuInfoCallback", SoldierItem::menuInfoCallback);
    return NULL;
}