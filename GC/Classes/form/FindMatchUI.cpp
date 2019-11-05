//
//  FindMatchUI.cpp
//  coc4
//
//  Created by chowray on 13-4-3.
//
//

#include "FindMatchUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/HttpClient.h"
#include "../coc/LogicInfo.h"
#include "../coc/PlayerScene.h"
#include "../coc/GuideMgr.h"
#include "../coc/EnvMapMgr.h"

FindMatchUI::FindMatchUI()
{
    
}

FindMatchUI::~FindMatchUI()
{
    LogicInfo::getSingleton().removeListener(this);
}

bool FindMatchUI::init()
{
    bool bRet = false;
    do
    {
        CC_BREAK_IF(!CUINode::init());
        {
            LogicInfo::getSingleton().addListener(this);
            CCNode* pFindMatchUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("find_match.ccbi", this);
            
            this->setContentSize(pFindMatchUI->getContentSize());
            this->setTargetTouch(true);
            this->addChild(pFindMatchUI);
            
            m_pEnvMapUI = EnvMapUI::create();
            m_pMapView->setContainer(m_pEnvMapUI);
            m_pMapView->setDelegate(this);
            
//            CCNode* pEnvMapUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("envmap_levels.ccbi", this);
//            m_pEnvMapUI->addChild(pEnvMapUI, -1, 999);
            
            m_pLootInfo->setPositionY(0);
            int posY = m_pMapView->boundingBox().origin.y;
            m_pLootInfo->setPositionY(posY);

            char str[128];
            
            sprintf(str, "%d", LogicInfo::getSingleton().getFindGold());
            ((CCLabelBMFont*)m_pBtnFind->getChildByTag(1))->setString(str);
            
            if (LEAGUE_ON)
            {
                if (LeagueMgr::getSingleton().isInLeague())
                {
                    sprintf(str, "icon_clanlevel_%d", LeagueMgr::getSingleton().m_myGroup.levelid);
                    CCSprite *pLeagueIcon = (CCSprite *)m_pBtnLeague->getChildByTag(1);
                    CCSpriteFrame* pLeagueIconFrame = ImgMgr::getSingleton().getFrame(str);
                    pLeagueIcon->setDisplayFrame(pLeagueIconFrame);
                    
                    sprintf(str, "%d", LeagueMgr::getSingleton().getSearchCost());
                    ((CCLabelBMFont*)m_pBtnLeague->getChildByTag(2))->setString(str);
                    
                    if (!LeagueMgr::getSingleton().canAttack())
                    {
                        m_pBtnLeague->setEnabled(false);
                    }
                }
                else
                {
                    m_pBtnLeague->setEnabled(false);
                }
            }

            m_isDrag = false;
            refresh();

            this->schedule(schedule_selector(FindMatchUI::timeUpdate), 1.0f);
        }
        
        bRet = true;
    }while (0);
    
    return bRet;
}

bool FindMatchUI::onNotify(int notifyid, const void* pVal)
{
    if (notifyid == NOTIFY_PVE_MAP) {
        refresh();
    }
    else if (notifyid == NOTIFY_PVE_ADD_LIFE)
    {
        m_pLife->setString(CCStringFormat("%d/%d", LogicInfo::getSingleton().m_pveInfo.curlife, LogicInfo::getSingleton().m_pveInfo.maxlife).c_str());
    }
    
    return true;
}

void FindMatchUI::timeUpdate(float dt)
{
    if(LogicInfo::getSingleton().m_pveInfo.curlife < LogicInfo::getSingleton().m_pveInfo.maxlife)
    {
        time_t curtime = getCurrentTime();
        int passtime = curtime - LogicInfo::getSingleton().m_pveInfo.last_lifetime;
        
        LogicInfo::getSingleton().m_pveInfo.lifetime -= passtime;
        LogicInfo::getSingleton().m_pveInfo.last_lifetime = curtime;
        
        if (LogicInfo::getSingleton().m_pveInfo.lifetime < 0) {
            LogicInfo::getSingleton().m_pveInfo.lifetime = LogicInfo::getSingleton().m_pveInfo.lifetime_off;
            
            HttpClient::getSingleton().addPVELife();
            
            if (LogicInfo::getSingleton().m_pveInfo.curlife < LogicInfo::getSingleton().m_pveInfo.maxlife) {
                LogicInfo::getSingleton().m_pveInfo.curlife++;
                m_pLife->setString(CCStringFormat("%d/%d", LogicInfo::getSingleton().m_pveInfo.curlife, LogicInfo::getSingleton().m_pveInfo.maxlife).c_str());
            }
        }
        m_pLifeTime->setString(formatTime(LogicInfo::getSingleton().m_pveInfo.lifetime).c_str());
        m_pLifeNode->setVisible(true);
    }
    
    if (LogicInfo::getSingleton().m_pveInfo.curlife >= LogicInfo::getSingleton().m_pveInfo.maxlife)
    {
        m_pLifeNode->setVisible(false);
    }
    
    if (LogicInfo::getSingleton().m_pveInfo.restime > 0) {
        time_t curtime = getCurrentTime();
        int passtime = curtime - LogicInfo::getSingleton().m_pveInfo.last_restime;
        
        LogicInfo::getSingleton().m_pveInfo.restime -= passtime;
        
        LogicInfo::getSingleton().m_pveInfo.last_restime = curtime;
        
        m_pRefreshTime->setString(formatTime(LogicInfo::getSingleton().m_pveInfo.restime).c_str());
    }
}

void FindMatchUI::refresh()
{
    m_pLifeNode->setVisible(false);
    if (!LogicInfo::getSingleton().m_bEnvMapInit) {
        return;
    }
    
    timeUpdate(0);

    int curstar = 0;
    
    int openStage = -1;
    bool stageLock = false;
    
    int count = EnvMapMgr::getSingleton().getEnvStageNums();
    for (int index = 0; index < count; index++) {
        int stage = EnvMapMgr::getSingleton().getEnvStageId(index);
        EnvStageInfo info = *EnvMapMgr::getSingleton().getEnvStageInfo(stage);
        
        if ((info.star > 0 || (!stageLock && stage - 1 == openStage)) ||
            (stage >= HARD_STAGE && openStage / SMALL_STAGE_NUMS > stage - HARD_STAGE)) {

            if (stage < HARD_STAGE) {  //normal stage
                if (stage >= openStage) {
                    openStage = stage;
                }
            }

            if (info.star <= 0) {
                stageLock = true;
            }
            
            curstar += info.star;
        }
        
    }
    
    if (m_pEnvMapUI) {
        m_pEnvMapUI->m_openStage = openStage;
    }
    
    if (PVE_VIDEO_DEBUG) {
        m_pEnvMapUI->m_openStage = 999;
    }
    
    char str[128];
    
    sprintf(str, "%d/%d", curstar, 3*count);
    m_pStar->setString(str);
    
    sprintf(str, "%d/%d", LogicInfo::getSingleton().m_pveInfo.curlife, LogicInfo::getSingleton().m_pveInfo.maxlife);
    m_pLife->setString(str);
}

bool FindMatchUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    m_isDrag = false;
    m_dragOffset = m_pMapView->getContentOffset();
    
    return true;
}

void FindMatchUI::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

void FindMatchUI::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    if (m_isDrag) {
        m_isDrag = false;
        return;
    }
}

void FindMatchUI::addFullLife()
{
    int offlife = LogicInfo::getSingleton().m_pveInfo.maxlife - LogicInfo::getSingleton().m_pveInfo.curlife;
    
    if (offlife > 0) {
        int gems = (offlife-1) * LogicInfo::getSingleton().m_pveInfo.lifetime_off/60;
        
        gems += (LogicInfo::getSingleton().m_pveInfo.lifetime/60 + 1);
        
        if (LogicInfo::getSingleton().m_myInfo.gemex < gems) {
            MessageUI::show(MSG_NOGEM, NULL, NULL);
        }
        else{
            
            char str[128];
            sprintf(str, ViewString::getValue("pve_full_life").c_str(), gems);
            
            MessageUI::show(MSG_RESTORE_PVE_LIFE, NULL, str, gems, -1);
        }
    }
}

void FindMatchUI::onSelectStage(int stage)
{
    if (stage < 0) {
        m_pLootInfo->stopAllActions();

        int posY = m_pMapView->boundingBox().origin.y;
        CCMoveTo* pMoveTo = CCMoveTo::create(0.2, ccp(m_pLootInfo->getPosition().x, posY));
        m_pLootInfo->runAction(pMoveTo);
    }
    else {
        m_pLootInfo->stopAllActions();
        int posY = m_pMapView->boundingBox().origin.y;
        CCMoveTo* pMoveTo1 = CCMoveTo::create(0.2, ccp(m_pLootInfo->getPosition().x, posY));
        CCMoveTo* pMoveTo2 = CCMoveTo::create(0.2, ccp(m_pLootInfo->getPosition().x, posY + m_pLootInfo->getContentSize().height + 5));
        CCSequence* pSequence = CCSequence::create(pMoveTo1, pMoveTo2, NULL);
        m_pLootInfo->runAction(pSequence);
        
        EnvStageInfo* info = EnvMapMgr::getSingleton().getEnvStageInfo(stage);
        
        char str[128];
        sprintf(str, "%d", info->gold);
        m_pLootGold->setString(str);
        sprintf(str, "%d", info->oil);
        m_pLootOil->setString(str);
    }
    
}

void FindMatchUI::menuAddLifeCallback(CCObject *pSender, CCControlEvent event)
{
    addFullLife();
}

void FindMatchUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
	(PlayerScene::getSingleton().m_pGameWorld)->hideFindMatchUI();
}

void FindMatchUI::menuFindCallback(CCObject* pSender, CCControlEvent event)
{
    LogicInfo::getSingleton().refurbishFightSoldierInfo();
    if(LogicInfo::getSingleton().m_lstFightSoldier.size() == 0)
    {
        MessageUI::show(MSG_NO_SOLDIER, "", ViewString::getSingleton().getValue("no_soldier").c_str());
        
        return;
    }
    
    if(LogicInfo::getSingleton().m_myInfo.goldex < LogicInfo::getSingleton().getFindGold())
    {        
        char message[512];
        int value = LogicInfo::getSingleton().getFindGold() - LogicInfo::getSingleton().m_myInfo.goldex;
        sprintf(message, ViewString::getSingleton().getValue("msg_nogold_message").c_str(), value);
        
        MessageUI::show(MSG_NOGOLD, ViewString::getSingleton().getValue("msg_nogold_title").c_str(), message, value, GETGEM_FINDMATCH_PVP);
        
        return;
    }
    
    if(GuideMgr::getSingleton().isGuideEnd())
    {
        if(LogicInfo::getSingleton().m_myInfo.lastProtectTime > 0)
        {
            MessageUI::show(MSG_HAVE_PROTECT, "", ViewString::getSingleton().getValue("have_protect").c_str(), GETGEM_FINDMATCH_PVP);
            return;
        }
    }
    
	(PlayerScene::getSingleton().m_pGameWorld)->showChgScene(CHANGE_TYPE_FINDMATCH);

    if(GuideMgr::getSingleton().isEquVal(GUIDE_ATTACK_GOBLIN, 20))
    {
        LogicInfo::getSingleton().findAttackUserEx(1511);
//        LogicInfo::getSingleton().attackEnv(0);
    }
    else if(GuideMgr::getSingleton().isEquVal(GUIDE_RAVEN_GOBLIN, 20))
    {
        LogicInfo::getSingleton().findAttackUserEx(1512);
//        LogicInfo::getSingleton().attackEnv(1);
    }
    else
    {
        LogicInfo::getSingleton().findAttackUser();
    }

    GuideMgr::getSingleton().m_pActiveBtn = NULL;
    
	(PlayerScene::getSingleton().m_pGameWorld)->hideFindMatchUI();
}

void FindMatchUI::menuLeagueCallback(cocos2d::CCObject *pSender, CCControlEvent event)
{
    LogicInfo::getSingleton().refurbishFightSoldierInfo();
    if(LogicInfo::getSingleton().m_lstFightSoldier.size() == 0)
    {
        MessageUI::show(MSG_NO_SOLDIER, "", ViewString::getSingleton().getValue("no_soldier").c_str());
        return;
    }
    
    if(LogicInfo::getSingleton().m_myInfo.goldex < LeagueMgr::getSingleton().getSearchCost())
    {
        char message[512];
        int value = LeagueMgr::getSingleton().getSearchCost() - LogicInfo::getSingleton().m_myInfo.goldex;
        sprintf(message, ViewString::getSingleton().getValue("msg_nogold_message").c_str(), value);
        
        MessageUI::show(MSG_NOGOLD, ViewString::getSingleton().getValue("msg_nogold_title").c_str(), message, value, GETGEM_FINDMATCH_LEAGUE);
        
        return;
    }
    
    GuideMgr::getSingleton().m_pActiveBtn = NULL;
    
    LogicInfo::getSingleton().findAttackUser(ATTACK_TYPE_LEAGUE);
    PlayerScene::getSingleton().m_pGameWorld->hideFindMatchUI();
    
    PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_FINDMATCH);
}

bool FindMatchUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "MapView", CCScrollView*, m_pMapView);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Star", CCLabelBMFont*, m_pStar);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Life", CCLabelBMFont*, m_pLife);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LifeTime", CCLabelBMFont*, m_pLifeTime);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnAddLife", CCControlButton*, m_pBtnAddLife);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LifeNode", CCNode*, m_pLifeNode);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RefreshTime", CCLabelBMFont*, m_pRefreshTime);
    
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LootInfo", CCNode*, m_pLootInfo);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LootGold", CCLabelBMFont*, m_pLootGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LootOil", CCLabelBMFont*, m_pLootOil);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnFindMatch", CCControlButton*, m_pBtnFind);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnLeagueMatch", CCControlButton*, m_pBtnLeague);

    return NULL;
}

SEL_CCControlHandler FindMatchUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuAddLifeCallback", FindMatchUI::menuAddLifeCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", FindMatchUI::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuFindCallback", FindMatchUI::menuFindCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuLeagueCallback", FindMatchUI::menuLeagueCallback);

    
    return NULL;
}

void FindMatchUI::scrollViewDidScroll(CCScrollView* view)
{
    if (view->getContentOffset().x >= 0) {
        view->getContainer()->setPosition(ccp(0, view->getContentOffset().y));;
    }
    else if(view->getContentOffset().x <= -(view->getContentSize().width - view->getViewSize().width))
    {
        view->getContainer()->setPosition(ccp(-(view->getContentSize().width - view->getViewSize().width), view->getContentOffset().y));
    }
    else{
        if (fabs(m_dragOffset.x - view->getContentOffset().x) > 10) {
            m_isDrag = true;
            m_dragOffset = view->getContentOffset();
        }
    }
}
