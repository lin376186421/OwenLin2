//
//  MessageUI.cpp
//  coc4
//
//  Created by chowray on 13-4-9.
//
//

#include "MessageUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/BaseDef.h"
#include "../coc/LogicInfo.h"
#include "../coc/PlayerScene.h"
#include "../coc/Building.h"
#include "../coc/FacebookMgr.h"
#include "../coc/GuideMgr.h"
#include "../coc/HeroMgr.h"
#include "../coc/HttpClient.h"
#include "../coc/ClanMgr.h"
#include "../coc/ChatMgr.h"
#include "../AppDelegate.h"
#include "CAppPlatform.h"
#include "HelloWorldScene.h"

using namespace cocos2d;

MessageUI::MessageUI()
{
    
}

MessageUI::~MessageUI()
{
    unschedule(schedule_selector(MessageUI::scheduleUpdate));
    
    if (m_messageId == MSG_VILLAGE_BEATTACK) {
        ImgMgr::getSingleton().removeRes("ui_attacked_npc");
    }
    if(m_messageId == MSG_NOGEM && LogicInfo::getSingleton().m_isShowFreeGemInBonus && GuideMgr::getSingleton().isGuideEnd())
    {
        ImgMgr::getSingleton().removeRes("ui_reward_npc");
    }
    
    LogicInfo::getSingleton().removeListener(this);
}

bool MessageUI::init()
{
    bool bRet = false;
    
    do{
        
        CC_BREAK_IF(!CUINode::init());
        
        LogicInfo::getSingleton().addListener(this);
        
        m_pView = NULL;
        m_isBaseMsg = false;
        m_pAnimationManager = NULL;
        
        m_ticker = 0;
        
        this->setContentSize(CCDirector::sharedDirector()->getWinSize());
        this->setTargetTouch(true);
        
        schedule(schedule_selector(MessageUI::scheduleUpdate), 1.0);
        bRet = true;
    }
    while (0);
    
    return bRet;
    
}

void MessageUI::onEnter()
{
    CUINode::onEnter();
    
    this->setTouchPriorityId(999);
    this->registerAllChildTouchDispatcher(this);
    
    if (PlayerScene::getSingleton().m_pGameWorld) {
        if (PlayerScene::getSingleton().m_pGameWorld->m_pGuideUI) {
            PlayerScene::getSingleton().m_pGameWorld->m_pGuideUI->setVisible(false);
        }
    }
    if (GuideMgr::getSingleton().m_pArrow) {
        GuideMgr::getSingleton().m_pArrow->setVisible(false);
    }
    
    GuideMgr::getSingleton().m_pMessageUI = this;
}

void MessageUI::onExit()
{
    CUINode::onExit();
    
    if (PlayerScene::getSingleton().m_pGameWorld) {
        if (PlayerScene::getSingleton().m_pGameWorld->m_pGuideUI) {
            PlayerScene::getSingleton().m_pGameWorld->m_pGuideUI->setVisible(true);
        }
    }
    
    if (GuideMgr::getSingleton().m_pArrow) {
        GuideMgr::getSingleton().m_pArrow->setVisible(true);
    }
    
    GuideMgr::getSingleton().m_pMessageUI = NULL;
}

bool MessageUI::onNotify(int notifyid, const void* pVal)
{
    if (pVal) {
        int state = *(int*)pVal;
        if(notifyid == m_messageId && state == NOTIFY_MESSAGE_CLOSE)
        {
            this->removeFromParent();
        }
    }
    
    if (m_messageId == MSG_PURCHASE_WAIT) {
        
        if (notifyid == NOTIFY_PAY_OK) {
            
            int gem = *(int*)pVal;
            m_value = gem;
            
            m_pAnimationManager->runAnimationsForSequenceNamed("message_success");
            char str[128];
            sprintf(str, "%d", gem);
            m_pGem->setString(str);
            
            m_pMessage->setString(ViewString::getValue("order_confirm_successed").c_str());
        }
        else if (notifyid == NOTIFY_PAY_FAILED) {
            m_pAnimationManager->runAnimationsForSequenceNamed("message_failed");
            
            m_pMessage->setString(ViewString::getValue("order_confirm_failed").c_str());
        }
    }
    
    return true;
}


void MessageUI::scheduleUpdate(float ot)
{
    if (m_messageId == MSG_VILLAGE_BEATTACK) {
        m_value --;
        
        m_ticker += ot;
        
        m_pTime->setString(formatTime(m_value).c_str());
        
        if (m_value <= 0) {
            HttpClient::getSingleton().login();
            this->removeFromParent();
        }
        else if (m_ticker > 30) {
            m_ticker = 0;
            
            HttpClient::getSingleton().login();
        }
    }
    else if(m_messageId == MSG_PURCHASE_WAIT)
    {
    }
}

void MessageUI::reload()
{
    if (m_pView) {
        removeChild(m_pView);
        m_pView = NULL;
    }
    
    m_pBtnOK = NULL;
    m_pBtnCancel = NULL;
    m_pBtnGem = NULL;
    m_pBtnClose = NULL;
    m_pBtnFreeGem = NULL;
}

// a selector callback
void MessageUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    this->removeFromParent();
    
    GuideMgr::getSingleton().m_pMessageUI = NULL;
    
    if (m_messageId == MSG_EXIT) {
        LogicInfo::getSingleton().m_isExitMsg = false;
    }
    
    if(GuideMgr::getSingleton().m_pActiveBtn == pSender)
        GuideMgr::getSingleton().refresh(GuideMgr::getSingleton().val1, GuideMgr::getSingleton().val2);
}

void MessageUI::menuFreeGemCallback(CCObject* pSender, CCControlEvent event)
{
    this->removeFromParent();
    CAppPlatform::sharedAppPlatform()->showNativeXOfferAds(NATIVE_X_NOT_GEMS);
}


// a selector callback
void MessageUI::menuOKCallback(CCObject* pSender, CCControlEvent event)
{
    if(m_messageId == MSG_BUILDING_STOP)
    {
        if(HeroMgr::getSingleton().m_isSelect)
        {
            HeroMgr::getSingleton().cancelUpgrade();
        }
        else
        {
            Building* pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
            if(pBuilding != NULL)
            {
                LogicInfo::getSingleton().cancelBuild(*pBuilding);
                PlayerScene::getSingleton().onBuildingSelect(false, NULL);
            }
        }
    }
    else if(m_messageId == MSG_EXIT)
    {
        CCDirector::sharedDirector()->end();
        ((AppDelegate*)CCApplication::sharedApplication())->applicationWillTerminate();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        exit(0);
#endif
    }
    else if(m_messageId == MSG_END_BATTLE)
    {
        LogicInfo::getSingleton().onStopAttack(false);
        
        PlayerScene::getSingleton().m_pGameWorld->hideFightUI();
        PlayerScene::getSingleton().m_pGameWorld->showScoreUI();
    }
    else if (m_messageId == MSG_NOGEM)
    {
        PlayerScene::getSingleton().m_pGameWorld->hideShop();
        PlayerScene::getSingleton().m_pGameWorld->showShop(1);
    }
    else if(m_messageId == MSG_HAVE_PROTECT)
    {
        if(LogicInfo::getSingleton().m_pGetGemType == GETGEM_FINDMATCH_PVP)
        {
            LogicInfo::getSingleton().findAttackUser();
            
            PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_FINDMATCH);
            PlayerScene::getSingleton().m_pGameWorld->hideFindMatchUI();
        }
        else
        {
            FightMsg msg = *((FightMsg*)m_pObject);
            
            HttpClient::getSingleton().requestRevenge(msg.fsi.user[0].uid, msg.msgid);
        }
    }
    else if (m_messageId == MSG_SHARE_REPLAY)
    {
        FightMsg msg = *((FightMsg*)m_pObject);
        const char* message = m_pInput->getText();
        
        ChatMgr::getSingleton().sendShareReplay(msg.msgid, message);
    }
    else if(m_messageId == MSG_RATE)
    {
        CAppPlatform::sharedAppPlatform()->rateApp();
    }
    else if(m_messageId == MSG_RATE_GEM)
    {
        LogicInfo::getSingleton().requestGradeGem();
        
        CCUserDefault::sharedUserDefault()->setBoolForKey(RATE_FREE_GEMS, true);
        CCUserDefault::sharedUserDefault()->flush();
        
    }
    else if(m_messageId == MSG_REPLAY)
    {
        LogicInfo::getSingleton().stopReplay();
        
        PlayerScene::getSingleton().m_pGameWorld->hideReplayUI();
        
        PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_NORMAL);
        
        LogicInfo::getSingleton().goHome();
    }
    else if(m_messageId == MSG_NETWORK_FAIL)
    {
        HelloWorld::showEx();
    }
    else if(m_messageId == MSG_ERROR)
    {
        HelloWorld::showEx();
    }
    else if (m_messageId == MSG_USERID_LOGIN)
    {
        LogicInfo::getSingleton().rebindGameCenter(true);
    }
    else if (m_messageId == MSG_USERID_CHANGED)
    {
        if (!GuideMgr::getSingleton().isGuideEnd()) {
            LogicInfo::getSingleton().rebindGameCenter(true);
        }
        else
        {
            MessageUI::show(MSG_USERID_CONFIRM, ViewString::getValue("userid_confirm_title").c_str(), ViewString::getValue("userid_confirm_message").c_str());
        }
    }
    else if (m_messageId == MSG_USERID_CONFIRM)
    {
        if (strcmp(m_pInput->getText(), "CONFIRM") == 0) {
            LogicInfo::getSingleton().rebindGameCenter(true);
        }
    }
    else if(m_messageId == MSG_TIMES_OUT)
    {
        LogicInfo::getSingleton().m_pMsg = -1;
        HelloWorld::showEx();
    }
    else if(m_messageId == MSG_CLAN_LEAVE)
    {
        ClanMgr::getSingleton().leaveClan();
        
        PlayerScene::getSingleton().m_pGameWorld->hideClanUI();
        PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->onSelect(false, NULL);
    }
    else if(m_messageId == MSG_CLAN_JOIN)
    {
        if(ClanMgr::getSingleton().m_otherclan.jointype == 0)
        {
            ClanMgr::getSingleton().askJoin(m_value);
            
            if (PlayerScene::getSingleton().m_pGameWorld->m_pClanInfoUI) {
                PlayerScene::getSingleton().m_pGameWorld->m_pClanInfoUI->m_pBtnJoin->setEnabled(false);
            }
        }
        else if(ClanMgr::getSingleton().m_otherclan.jointype == 1)
        {
            ChatMgr::getSingleton().joinClan(m_value);
        }
    }
    else if(m_messageId == MSG_REQUEST_TROOPS_AGAIN)
    {
        if(LogicInfo::getSingleton().m_myInfo.gemex < m_value)
        {
            MessageUI::show(MSG_NOGEM, NULL, NULL);
        }
        else
        {
            MessageUI::show(MSG_REQUEST_TROOPS, ViewString::getValue("request_troops_title").c_str(), NULL, m_value, -1);
        }
    }
    else if (m_messageId == MSG_PVE_NOT_LIFE)
    {
        LogicInfo::getSingleton().goHome();
    }
    else if (m_messageId == MSG_REQUEST_TROOPS)
    {
        int gem = m_value;
        const char* message = m_pInput->getText();
        ClanMgr::getSingleton().sendHelp(message, gem);
        
        CCUserDefault::sharedUserDefault()->setStringForKey("REQUEST_TROOPS_MESSAGE", string(message));
        CCUserDefault::sharedUserDefault()->flush();
    }
    else if (m_messageId == MSG_PURCHASE_WAIT)
    {
        LogicInfo::getSingleton().addGem(m_value);
    }
    else if (m_messageId == MSG_ADD_GEM)
    {
        LogicInfo::getSingleton().addGem(m_value);
    }
    else if (m_messageId == MSG_GET_GEM)
    {
        LogicInfo::getSingleton().addGem(m_value);
        
        if (m_type != 0) {
            HttpClient::getSingleton().requestGetGem(m_type);
        }
    }
    else if (m_messageId == MSG_BUY_GOLD)
    {
        int gem = m_value;
        if(LogicInfo::getSingleton().getGoldEx(gem))
        {
            MessageUI::show(MSG_BUY_OK, "", ViewString::getSingleton().getValue("buy_gold_ok").c_str(), -1);
        }
        else
        {
            MessageUI::show(MSG_NOGEM, "", NULL, -1);
        }
    }
    else if (m_messageId == MSG_BUY_OIL)
    {
        int gem = m_value;
        if(LogicInfo::getSingleton().getOilEx(gem))
        {
            MessageUI::show(MSG_BUY_OK, "", ViewString::getSingleton().getValue("buy_oil_ok").c_str(), -1);
        }
        else
        {
            MessageUI::show(MSG_NOGEM, "", NULL, -1);
        }
    }
    else if (m_messageId == MSG_BUY_SHIELD)
    {
        int shieldTime = m_value;
        
        if(LogicInfo::getSingleton().payProtectTime(shieldTime))
        {
            MessageUI::show(MSG_BUY_OK, "", ViewString::getSingleton().getValue("buy_shield_ok").c_str(), -1);
        }
        else
        {
            MessageUI::show(MSG_NOGEM, "", NULL, -1);
        }
    }
    else if (m_messageId == MSG_CLAN_NOT_NATIONAL)
    {
        PlayerScene::getSingleton().m_pGameWorld->showClanEditUI();
    }
    else if (m_messageId == MSG_EDIT_SAVE)
    {
        PlayerScene::getSingleton().m_pGameWorld->m_pMainEditUI->editSave();
    }
    else if (m_messageId == MSG_EDIT_EXIT)
    {
        PlayerScene::getSingleton().m_pGameWorld->m_pMainEditUI->editExit();
    }
    else if (m_messageId == MSG_UPGRADE_WALLS)
    {
        int need = 0;
        int ret = LogicInfo::getSingleton().upgradeWalls(PlayerScene::getSingleton().m_lstUpgradeWall, need);
        if(ret <= 0)
        {
            // msg
            MessageUI::show(ret, "", "", need, GETGEM_UPLEVEL_WALLS);
        }
    }
    
    
    if(m_messageId == MSG_FB_SHARE_TIP)
    {
        this->removeFromParent();
        if(PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pMainUI)
        {
            CCControlButton *pBtn = PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->m_pBtnFBShare;
            pBtn->setVisible(false);//分享时隐藏分享按钮
            FacebookMgr::getSingleton().m_shareType = FBShare_Base;
            FacebookMgr::getSingleton().fbShare(FacebookMgr::getSingleton().m_shareMessaage);
            pBtn->setVisible(true);
        }
        if (m_messageId == MSG_ERROR) {
            LogicInfo::getSingleton().m_isErrorMsg = false;
        }
    }
    
    this->removeFromParent();
    
    if (m_messageId == MSG_ERROR) {
        LogicInfo::getSingleton().m_isErrorMsg = false;
    }
}

void MessageUI::menuGemCallback(CCObject* pSender, CCControlEvent event)
{
    if(m_messageId == MSG_NOGOLD)
    {
        if(LogicInfo::getSingleton().m_pGetGemType == GETGEM_FINDMATCH_PVP)
        {
            if(LogicInfo::getSingleton().getGoldEx(LogicInfo::getSingleton().m_pGetGemNums))
            {
                if(GuideMgr::getSingleton().isGuideEnd())
                {
                    if(LogicInfo::getSingleton().m_myInfo.lastProtectTime > 0)
                    {
                        this->removeFromParent();
                        MessageUI::show(MSG_HAVE_PROTECT, "", ViewString::getSingleton().getValue("have_protect").c_str(), 1);
                        return;
                    }
                }
                PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_FINDMATCH);
                
                LogicInfo::getSingleton().findAttackUser();
                
                PlayerScene::getSingleton().m_pGameWorld->hideFightUI();
                PlayerScene::getSingleton().m_pGameWorld->hideFindMatchUI();
            }
        }
        else if (LogicInfo::getSingleton().m_pGetGemType == GETGEM_FINDMATCH_LEAGUE)
        {
            if(LogicInfo::getSingleton().getGoldEx(LogicInfo::getSingleton().m_pGetGemNums))
            {
                LogicInfo::getSingleton().findAttackUser(ATTACK_TYPE_LEAGUE);
                PlayerScene::getSingleton().m_pGameWorld->hideLeagueUI();
                
                PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_FINDMATCH);
                
                PlayerScene::getSingleton().m_pGameWorld->hideFightUI();
                PlayerScene::getSingleton().m_pGameWorld->hideFindMatchUI();
            }
        }
        else if(LogicInfo::getSingleton().m_pGetGemType == GETGEM_CREATE_CLAN)
        {
            if(LogicInfo::getSingleton().getGoldEx(LogicInfo::getSingleton().m_pGetGemNums))
            {
                ClanUI* pClanUI = PlayerScene::getSingleton().m_pGameWorld->m_pClanUI;
                ((ClanEditUI*)pClanUI->m_pPagesUI[pClanUI->m_pageIndex])->menuCreateCallback(NULL, NULL);
            }
            
            PlayerScene::getSingleton().m_pGameWorld->hideClanUI();
        }
        else if (LogicInfo::getSingleton().m_pGetGemType == GETGEM_UPLEVEL_HERO)
        {
            if(LogicInfo::getSingleton().getGoldEx(LogicInfo::getSingleton().m_pGetGemNums))
            {
                int need = 0;
                HeroMgr::getSingleton().upLevelHero(need);
                LogicInfo::getSingleton().m_showinfo_soldierid = 0;
                PlayerScene::getSingleton().m_pGameWorld->hideHeroUI();
            }
        }
        else if(LogicInfo::getSingleton().m_pGetGemType == GETGEM_BUILD_BUILDING)
        {
            if(LogicInfo::getSingleton().getGoldEx(LogicInfo::getSingleton().m_pGetGemNums))
            {
                Building* pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
                if(pBuilding->m_pBuildingInfo != NULL)
                {
                    int need = 0;
                    LogicInfo::getSingleton().buildBuilding(*pBuilding, need);
                }
            }
        }
        else if(LogicInfo::getSingleton().m_pGetGemType == GETGEM_UPLEVEL_WALLS)
        {
            if(LogicInfo::getSingleton().getGoldEx(LogicInfo::getSingleton().m_pGetGemNums)) {
                if (PlayerScene::getSingleton().m_lstUpgradeWall.size() > 0) {
                    int need = 0;
                    LogicInfo::getSingleton().upgradeWalls(PlayerScene::getSingleton().m_lstUpgradeWall, need);
                }
            }
        }
        else if(LogicInfo::getSingleton().m_pGetGemType == GETGEM_UPLEVEL_BUILDING)
        {
            if(LogicInfo::getSingleton().getGoldEx(LogicInfo::getSingleton().m_pGetGemNums))
            {
                Building* pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
                if(pBuilding->m_pBuildingInfo != NULL)
                {
                    int need = 0;
                    LogicInfo::getSingleton().upLevel(*pBuilding, need);
                }
            }
        }
        else if(LogicInfo::getSingleton().m_pGetGemType == GETGEM_REMOVE_BUILDING)
        {
            if(LogicInfo::getSingleton().getGoldEx(LogicInfo::getSingleton().m_pGetGemNums))
            {
                Building* pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
                if(pBuilding->m_pBuildingInfo != NULL)
                {
                    int need;
                    LogicInfo::getSingleton().removeBuilding(*pBuilding, need);
                }
            }
        }
    }
    else if (m_messageId == MSG_NOELIXIR)
    {
        if(LogicInfo::getSingleton().m_pGetGemType == GETGEM_BUILD_BUILDING)
        {
            if(LogicInfo::getSingleton().getOilEx(LogicInfo::getSingleton().m_pGetGemNums))
            {
                Building* pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
                if(pBuilding->m_pBuildingInfo != NULL)
                {
                    int need = 0;
                    LogicInfo::getSingleton().buildBuilding(*pBuilding, need);
                }
            }
        }
        else if (LogicInfo::getSingleton().m_pGetGemType == GETGEM_UPLEVEL_HERO)
        {
            if(LogicInfo::getSingleton().getOilEx(LogicInfo::getSingleton().m_pGetGemNums))
            {
                int need = 0;
                HeroMgr::getSingleton().upLevelHero(need);
                LogicInfo::getSingleton().m_showinfo_soldierid = 0;
                PlayerScene::getSingleton().m_pGameWorld->hideHeroUI();
            }
        }
        else if(LogicInfo::getSingleton().m_pGetGemType == GETGEM_UPLEVEL_BUILDING)
        {
            if(LogicInfo::getSingleton().getOilEx(LogicInfo::getSingleton().m_pGetGemNums))
            {
                Building* pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
                if(pBuilding->m_pBuildingInfo != NULL)
                {
                    int need = 0;
                    LogicInfo::getSingleton().upLevel(*pBuilding, need);
                }
            }
        }
        else if(LogicInfo::getSingleton().m_pGetGemType == GETGEM_REMOVE_BUILDING)
        {
            if(LogicInfo::getSingleton().getOilEx(LogicInfo::getSingleton().m_pGetGemNums))
            {
                Building* pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
                if(pBuilding->m_pBuildingInfo != NULL)
                {
                    int need;
                    LogicInfo::getSingleton().removeBuilding(*pBuilding, need);
                }
            }
        }
        else if (LogicInfo::getSingleton().m_pGetGemType == GETGEM_UPLEVEL_SOLDIER)
        {
            if (LogicInfo::getSingleton().getOilEx(LogicInfo::getSingleton().m_pGetGemNums)) {
                int need = 0;
                LogicInfo::getSingleton().addUpgradeSoldier(LogicInfo::getSingleton().getMySoldierLevel(LogicInfo::getSingleton().m_showinfo_soldierid) + 1, need);
                
                LogicInfo::getSingleton().m_showinfo_soldierid = 0;
                PlayerScene::getSingleton().m_pGameWorld->hidePlayerLevelUpUI();
                
                PlayerScene::getSingleton().m_pGameWorld->m_pLabUI->refreshInfo();
            }
        }
    }
    else if (m_messageId == MSG_FILL_HERO_HP)
    {
        bool ret = HeroMgr::getSingleton().fastHeroHP();
        if(!ret)
        {
            if(LogicInfo::getSingleton().isAttackMode())
            {
                PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI->addNote(ViewString::getValue("not_enough_gems").c_str());
            }
            else {
                show(MSG_NOGEM, NULL, NULL);
            }
        }
        else{
            LogicInfo::getSingleton().onNotify(NOTIFY_HERO_FULL_HP);
        }
        PlayerScene::getSingleton().onHeroSelect();
    }
    else if (m_messageId == MSG_CHANGE_GLYPH_SKILL)
    {
        PlayerScene::getSingleton().m_pGameWorld->m_pSkillGlyphUI->changeSkill();
    }
    else if (m_messageId == MSG_RESTORE_PVE_LIFE)
    {
        int life = LogicInfo::getSingleton().m_pveInfo.maxlife - LogicInfo::getSingleton().m_pveInfo.curlife;
        
        HttpClient::getSingleton().addPVELife(life, m_value);
        
        LogicInfo::getSingleton().chgGem(-m_value);
        LogicInfo::getSingleton().onNotify(NOTIFY_BASEINFO);
    }
    else if (m_messageId == MSG_REMOVE_ORNAMENT)
    {
        int need = 0;
        int ret = LogicInfo::getSingleton().removeOrnament(*PlayerScene::getSingleton().m_pCurBuilding, need);
        if(ret <= 0)
        {
            // msg
            MessageUI::show(ret, "", "", need, GETGEM_REMOVE_BUILDING);
        }
    }
    else
    {
        if(HeroMgr::getSingleton().m_isSelect)
        {
            if(m_messageId == MSG_NOWORKER)
            {
                if(m_pLastBuilding != NULL)
                {
                    bool ret = LogicInfo::getSingleton().fastBuilding(*m_pLastBuilding);
                    if(!ret)
                    {
                        this->removeFromParent();
                        return;
                    }
                    int need = 0;
                    int result = HeroMgr::getSingleton().upLevelHero(need);
                    if(result <= 0)
                    {
                        MessageUI::show(result, NULL, NULL, need, GETGEM_UPLEVEL_HERO);
                    }
                    else
                    {
                        LogicInfo::getSingleton().m_showinfo_soldierid = 0;
                        PlayerScene::getSingleton().m_pGameWorld->hideHeroUI();
                    }
                }
            }
            else if(m_messageId == MSG_BUILDING_FAST)
            {
                HeroMgr::getSingleton().finishUpgrade(true);
            }
        }
        else
        {
            Building* pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
            if(pBuilding == NULL)
            {
                return;
            }
            
            if(m_messageId == MSG_NOWORKER)
            {
                if(m_pLastBuilding != NULL)
                {
                    bool ret = LogicInfo::getSingleton().fastBuilding(*m_pLastBuilding);
                    if(!ret)
                    {
                        this->removeFromParent();
                        return;
                    }
                    
                    if(LogicInfo::getSingleton().m_pGetGemType == GETGEM_BUILD_BUILDING)
                    {
                        int need = 0;
                        
                        LogicInfo::getSingleton().buildBuilding(*pBuilding, need);
                    }
                    else if(LogicInfo::getSingleton().m_pGetGemType == GETGEM_UPLEVEL_BUILDING)
                    {
                        int need = 0;
                        
                        LogicInfo::getSingleton().upLevel(*pBuilding, need);
                    }
                    else if(LogicInfo::getSingleton().m_pGetGemType == GETGEM_REMOVE_BUILDING)
                    {
                        int need = 0;
                        
                        LogicInfo::getSingleton().removeBuilding(*pBuilding, need);
                    }
                }
            }
            else if(m_messageId == MSG_BUILDING_FAST)
            {
                LogicInfo::getSingleton().fastBuilding(*pBuilding);
            }
        }
    }
    
    this->removeFromParent();
}

MessageUI* MessageUI::show(int msgid, const char* title, const char* message)
{
    return MessageUI::show(msgid, title, message, 0, MSG_NORMAL);
}

MessageUI* MessageUI::show(int msgid, const char* title, const char* message, int type)
{
    return MessageUI::show(msgid, title, message, 0, type);
}

//type 0:升级 1:新建 2:新查找 3:升级兵种 4:快速除草 5:去除保护时间 6:创建帮派
MessageUI* MessageUI::show(int msgid, const char* title, const char* message, int value, int type)
{
    MessageUI* pMsg = MessageUI::create();
    
    pMsg->m_messageId = msgid;
    pMsg->m_value = value;
    pMsg->m_type = type;
    
    if(msgid == MSG_NETWORK_FAIL || msgid == MSG_VILLAGE_BEATTACK || msgid == MSG_ERROR || msgid == NOTIFY_NEW_USERLOGIN || msgid == MSG_USERID_LOGIN || msgid == NOTIFY_USERLOGIN_INFO || msgid == MSG_USERID_CHANGED)
    {
        pMsg->m_isBaseMsg = true;
    }
    
    if (msgid == MSG_ERROR || msgid == MSG_NETWORK_FAIL) {
        
        if (LogicInfo::getSingleton().m_isErrorMsg) {
            return NULL;
        }
        
        LogicInfo::getSingleton().m_isErrorMsg = true;
    }
    else if (msgid == MSG_EXIT) {
        LogicInfo::getSingleton().m_isExitMsg = true;
    }
    
    LogicInfo::getSingleton().m_pMsg = msgid;
    LogicInfo::getSingleton().m_pGetGemType = type;
    
    pMsg->reload();
    
    if(msgid >= 0)
    {
        if (msgid == MSG_PURCHASE_WAIT) {
            
            pMsg->m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("message_box_5.ccbi", pMsg, &(pMsg->m_pAnimationManager));
            pMsg->addChild(pMsg->m_pView);
            pMsg->m_pAnimationManager->runAnimationsForSequenceNamed("message_waiting");
        }
        else if (msgid == MSG_ADD_GEM || msgid == MSG_GET_GEM) {
            pMsg->m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("message_box_4.ccbi", pMsg);
            
            char str[128];
            sprintf(str, "%d", value);
            pMsg->m_pGem->setString(str);
            
            pMsg->addChild(pMsg->m_pView);
        }
        else if (msgid == MSG_REQUEST_TROOPS || msgid == MSG_SHARE_REPLAY) {
            pMsg->m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("message_box_3.ccbi", pMsg);
            {
                pMsg->m_pInput = CTextInputField::create(pMsg->m_pInputBg->getContentSize(), CCScale9Sprite::create());
                pMsg->m_pInput->setFontColor(ccBLACK);
                pMsg->m_pInput->setAnchorPoint(ccp(0, 0));
                pMsg->m_pInput->setPosition(ccp(2, 2));
                pMsg->m_pInput->setReturnType(kKeyboardReturnTypeDone);
                pMsg->m_pInput->setDelegate(pMsg);
                pMsg->m_pInput->setMaxLength(120);
                pMsg->m_pInputBg->addChild(pMsg->m_pInput);
                
                if (msgid == MSG_REQUEST_TROOPS) {
                    string text = CCUserDefault::sharedUserDefault()->getStringForKey("REQUEST_TROOPS_MESSAGE", ViewString::getValue("request_troops_message"));
                    
                    pMsg->m_pInput->setText(text.c_str());
                }
                else if (msgid == MSG_SHARE_REPLAY) {
                    pMsg->m_pInput->setText("");
                }
            }
            pMsg->addChild(pMsg->m_pView);
        }
        else if (msgid == MSG_USERID_CONFIRM) {
            pMsg->m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("message_box_2.ccbi", pMsg);
            {
                pMsg->m_pInput = CTextInputField::create(pMsg->m_pInputBg->getContentSize(), CCScale9Sprite::create());
                pMsg->m_pInput->setFontColor(ccBLACK);
                pMsg->m_pInput->setAnchorPoint(ccp(0, 0));
                pMsg->m_pInput->setPosition(ccp(2, 2));
                pMsg->m_pInput->setReturnType(kKeyboardReturnTypeDone);
                pMsg->m_pInput->setDelegate(pMsg);
                pMsg->m_pInputBg->addChild(pMsg->m_pInput);
                
                pMsg->m_pBtnOK->setEnabled(false);
            }
            pMsg->addChild(pMsg->m_pView);
            
            char str[128];
            sprintf(str, "%d", LogicInfo::getSingleton().m_userlevelGameCenterBind);
            pMsg->m_pPlayerLevel->setString(str);
            pMsg->m_pPlayerName->setString(LogicInfo::getSingleton().m_unameGameCenterBind.c_str());
        }
        else if (msgid == MSG_VILLAGE_BEATTACK)
        {
            ImgMgr::getSingleton().addSpriteFrames("ui_attacked_npc");
            
            pMsg->m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("message_beattack.ccbi", pMsg);
            pMsg->addChild(pMsg->m_pView);
            
            pMsg->m_pTime->setString(formatTime(value).c_str());
        }
        else if(msgid == MSG_BUILDING_FAST || msgid == MSG_FILL_HERO_HP || msgid == MSG_CHANGE_GLYPH_SKILL || msgid == MSG_RESTORE_PVE_LIFE || msgid == MSG_REMOVE_ORNAMENT) //need gems
        {
            pMsg->m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("message_box_1.ccbi", pMsg);
            pMsg->addChild(pMsg->m_pView);
            
            char tmp[512];
            sprintf(tmp, "%d", value);
            pMsg->m_pGem->setString(tmp);
        }
        else
        {
            pMsg->m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("message_box_0.ccbi", pMsg);
            pMsg->addChild(pMsg->m_pView);
            
            if(msgid == MSG_NORMAL || msgid == NOTIFY_PAY_ERROR || msgid == MSG_BUY_OK || msgid == MSG_NO_SOLDIER || msgid == MSG_ERROR || msgid == MSG_RATE_GEM || msgid == MSG_REPLAY || msgid == MSG_NETWORK_FAIL || msgid == MSG_TIMES_OUT || msgid == MSG_PVE_NOT_LIFE || msgid == MSG_CLAN_NOT_NATIONAL)
            {
                pMsg->m_pBtnCancel->setVisible(false);
                pMsg->m_pBtnOK->setPositionX(pMsg->m_pView->getContentSize().width/2);
            }
        }
        
        pMsg->m_pTitle->setString(title);
        if (message != NULL) {
            pMsg->m_pMessage->setString(message);
        }
        if(msgid == MSG_FB_SHARE_TIP)
        {
            pMsg->m_pTitle->setString("Facebook");
            if(FacebookMgr::getSingleton().m_shareFreeGem)
                pMsg->m_pMessage->setString(ViewString::getValue("fb_share_freegem_message").c_str());
            else
                pMsg->m_pMessage->setString(ViewString::getValue("fb_share_message").c_str());
        }
        if(msgid == MSG_ADD_GEM && LogicInfo::getSingleton().m_addGemType != ADD_GEM_NORMAL)
        {
            pMsg->m_pTitle->setString("Free Gems!");
            char adName[124];
            if(LogicInfo::getSingleton().m_addGemType == ADD_GEM_TAPJOY)
                sprintf(adName, "Tapjoy");
            else if(LogicInfo::getSingleton().m_addGemType == ADD_GEM_FLURRLY)
                sprintf(adName, "Flurry");
            else if(LogicInfo::getSingleton().m_addGemType == ADD_GEM_CHARTBOOST)
                sprintf(adName, "Chartboost");
            else if(LogicInfo::getSingleton().m_addGemType == ADD_GEM_AARKI)
                sprintf(adName, "Aarki");
            else if(LogicInfo::getSingleton().m_addGemType == ADD_GEM_SSA)
                sprintf(adName, "Supersnoic");
            else if(LogicInfo::getSingleton().m_addGemType == ADD_GEM_NATIVE_X)
                sprintf(adName, "NativeX");
            CCString *addGemStr = CCString::createWithFormat(ViewString::getValue("add_gem_string").c_str(),adName);
            pMsg->m_pMessage->setString(addGemStr->getCString());
            if(LogicInfo::getSingleton().m_addGemType == ADD_GEM_FB_LOGIN)
            {
                pMsg->m_pTitle->setString("Congratulations!");
                pMsg->m_pMessage->setString(ViewString::getValue("fb_login_freegem_message").c_str());
            }
            if(LogicInfo::getSingleton().m_addGemType == ADD_GEM_FB_INVITE)
            {
                pMsg->m_pTitle->setString("Congratulations!");
                CCString *inviteStr = CCString::createWithFormat(ViewString::getValue("fb_invite_freegem_message").c_str(),pMsg->m_value,FacebookMgr::getSingleton().m_inviteCount);
                pMsg->m_pMessage->setString(inviteStr->getCString());
            }
            if(LogicInfo::getSingleton().m_addGemType == ADD_GEM_FB_SHARE)
            {
                pMsg->m_pTitle->setString("Congratulations!");
                CCString *inviteStr = CCString::createWithFormat(ViewString::getValue("fb_share_succeed_message").c_str(),5);
                pMsg->m_pMessage->setString(inviteStr->getCString());
            }
        }
    }
    else
    {
        if (msgid == MSG_NOWORKER || msgid == MSG_NOGOLD || msgid == MSG_NOELIXIR) { //need gems
            pMsg->m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("message_box_1.ccbi", pMsg);
            pMsg->addChild(pMsg->m_pView);
        }
        else
        {
            if(msgid == MSG_NOGEM && LogicInfo::getSingleton().m_isShowFreeGemInBonus && GuideMgr::getSingleton().isGuideEnd())
            {
                ImgMgr::getSingleton().loadImg("ui_reward_npc");
                pMsg->m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("message_box_6.ccbi", pMsg);
            }
            else
            {
                pMsg->m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("message_box_0.ccbi", pMsg);
            }
            pMsg->addChild(pMsg->m_pView);
            
            if (msgid == MSG_ERROR || msgid == MSG_DATE_ERROR || msgid == MSG_DATA_NOTSYNC || msgid == MSG_NETWORK_FAIL) {
                pMsg->m_pBtnCancel->setVisible(false);
                pMsg->m_pBtnOK->setPositionX(pMsg->m_pView->getContentSize().width/2);
            }
        }
        
        char tmp[1024];
        pMsg->m_pTitle->setString(title);
        pMsg->m_pMessage->setString("");
        if (message != NULL) {
            pMsg->m_pMessage->setString(message);
        }
        
        if(msgid == MSG_NOGOLD)
        {
            sprintf(tmp, ViewString::getSingleton().getValue("msg_nogold_message").c_str(), value);
            
            pMsg->m_pTitle->setString(ViewString::getSingleton().getValue("msg_nogold_title").c_str());
            pMsg->m_pMessage->setString(tmp);
            
            int gem = LogicInfo::getSingleton().countOther2Gem(value);
            LogicInfo::getSingleton().m_pGetGemNums = gem;
            
            sprintf(tmp, "%d", gem);
            pMsg->m_pGem->setString(tmp);
            
        }
        else if(msgid == MSG_NOELIXIR)
        {
            sprintf(tmp, ViewString::getSingleton().getValue("msg_nooil_message").c_str(), value);
            
            pMsg->m_pTitle->setString(ViewString::getSingleton().getValue("msg_nooil_title").c_str());
            pMsg->m_pMessage->setString(tmp);
            
            int gem = LogicInfo::getSingleton().countOther2Gem(value);
            LogicInfo::getSingleton().m_pGetGemNums = gem;
            
            sprintf(tmp, "%d", gem);
            pMsg->m_pGem->setString(tmp);
        }
        else if(msgid == MSG_NOGEM)
        {
            if(LogicInfo::getSingleton().m_isShowFreeGemInBonus && GuideMgr::getSingleton().isGuideEnd())
            {
                pMsg->m_pTitle->setString(ViewString::getSingleton().getValue("not_enough_message_gem").c_str());
            }
            
            pMsg->m_pMessage->setString(ViewString::getSingleton().getValue("msg_nogem").c_str());
        }
        else if(msgid == MSG_NOWORKER)
        {
            pMsg->m_pMessage->setString(ViewString::getSingleton().getValue("msg_noworker").c_str());
            
            pMsg->m_pLastBuilding = LogicInfo::getSingleton().countFastBuildingGem(value);
            LogicInfo::getSingleton().m_pGetGemNums = value;
            
            sprintf(tmp, "%d", value);
            pMsg->m_pGem->setString(tmp);
            //            pMsg->m_pMoney->setString(str);
        }
    }
    
    pMsg->refreshMsg();
    
    CCDirector::sharedDirector()->getRunningScene()->addChild(pMsg, 20);
    
    return pMsg;
}

void MessageUI::refreshMsg()
{
    if(LogicInfo::getSingleton().m_pMsg == MSG_BUILDING_STOP)
    {
        m_pMessage->setString(ViewString::getSingleton().getValue("msg_break").c_str());
        m_pBtnOK->setVisible(true);
        m_pBtnCancel->setVisible(true);
    }
    else if(LogicInfo::getSingleton().m_pMsg == MSG_BUILDING_FAST)
    {
        m_pMessage->setString(ViewString::getSingleton().getValue("msg_speed").c_str());
        
        char str[128];
        sprintf(str, "%d", LogicInfo::getSingleton().countCurBuildingGem());
        if(HeroMgr::getSingleton().m_isSelect)
            sprintf(str, "%d", LogicInfo::getSingleton().countBuildingHeroGem());
        m_pGem->setString(str);
    }
    else if(LogicInfo::getSingleton().m_pMsg == MSG_NOGEM)
    {
        m_pMessage->setString(ViewString::getSingleton().getValue("msg_nogem").c_str());
    }
    else if(LogicInfo::getSingleton().m_pMsg == MSG_EXIT)
    {
        m_pMessage->setString(ViewString::getSingleton().getValue("msg_quit").c_str());
        m_pBtnOK->setVisible(true);
        m_pBtnCancel->setVisible(true);
    }
    else if(LogicInfo::getSingleton().m_pMsg == MSG_END_BATTLE)
    {
        m_pMessage->setString(ViewString::getSingleton().getValue("msg_end_battle").c_str());
        m_pBtnOK->setVisible(true);
        m_pBtnCancel->setVisible(true);
    }
}

bool MessageUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Title", CCLabelBMFont*, m_pTitle);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Message", CCLabelTTF*, m_pMessage);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Gem", CCLabelBMFont*, m_pGem);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnCancel", CCControlButton*, m_pBtnCancel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnClose", CCControlButton*, m_pBtnClose);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnOK", CCControlButton*, m_pBtnOK);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnGem", CCControlButton*, m_pBtnGem);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnFreeGem", CCControlButton*, m_pBtnFreeGem);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "InputBg", CCScale9Sprite*, m_pInputBg);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerLevel", CCLabelBMFont*, m_pPlayerLevel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerName", CCLabelTTF*, m_pPlayerName);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Time", CCLabelBMFont*, m_pTime);
    
    return false;
}

SEL_CCControlHandler MessageUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", MessageUI::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuOKCallback", MessageUI::menuOKCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuGemCallback", MessageUI::menuGemCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuFreeGemCallback", MessageUI::menuFreeGemCallback);
    
    return NULL;
}

void MessageUI::textInputFieldTextChanged(CTextInputField* TextInputField, const std::string& text)
{
    if (m_messageId == MSG_USERID_CONFIRM) {
        if (strcmp(text.c_str(), "CONFIRM") == 0) {
            m_pBtnOK->setEnabled(true);
        }else{
            m_pBtnOK->setEnabled(false);
        }
    }
}
