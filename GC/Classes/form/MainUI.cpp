#include <algorithm>
#include "MainUI.h"

#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/PlayerScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/HttpClient.h"
#include "../coc/Building.h"
#include "../coc/BuildingMgr.h"
#include "SimpleAudioEngine.h"
#include "../coc/SoundMgr.h"
#include "../coc/HeroMgr.h"
#include "../coc/ExperienceLevels.h"
#include "../coc/GuideMgr.h"
#include "../coc/Person.h"
#include "../coc/ClanMgr.h"
#include "../coc/ClanWarMgr.h"
#include "../coc/ConfigValMgr.h"
#include "../coc/MilitaryRankMgr.h"
#include "../coc/ChatMgr.h"
#include "../coc/PersonMgr.h"
#include "CAppPlatform.h"
#include "CDevice.h"
#include "CUINodeCache.h"

using namespace cocos2d;
using namespace coc;

MainUI::MainUI()
: m_gold(-1)
, m_oil(-1)
, m_pActGold(NULL)
, m_pActOil(NULL)
, m_pActGem(NULL)
, m_pMoreGameUI(NULL)
, m_pLeagueStartUI(NULL)
, m_pCityWarStartUI(NULL)
, m_pGetAwardUI(NULL)
, m_pAdsLoadingUI(NULL)
, m_pAdsLoadingTip(NULL)
//    , m_pGuideArrow(NULL)
//	: m_pShop(NULL)
{
}

MainUI::~MainUI()
{
    LogicInfo::getSingleton().removeListener(this);
}
void MainUI::onExit()
{
    CUINode::onExit();
}
void MainUI::onEnter()
{
    CUINode::onEnter();
}

bool MainUI::init()
{
	bool bRet = false;
    do
    {
        CC_BREAK_IF(!CUINode::init());
        {
            CCNode* mainNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("main_ui.ccbi", this);
            this->addChild(mainNode);
            
            CCNode* menuNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("main_button.ccbi", this);
            mainNode->addChild(menuNode);
            
            for (int i = 0; i < menuNode->getChildrenCount(); i ++) {
                CCNode* itemNode = (CCNode*)menuNode->getChildren()->objectAtIndex(i);
                itemNode->setVisible(false);
            }
            
			LogicInfo::getSingleton().addListener(this);
			PlayerScene::getSingleton().addSelectBuildingListener(this);
            
            if (GuideMgr::getSingleton().isGuiding()) {
                GuideMgr::getSingleton().setMainUIButton(this, true);
            }
            
            SoundMgr::getSingleton().stopBackgroundMusic();
            SoundMgr::getSingleton().playBackgroundMusic("back.mp3");
            CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(1);
        }
        m_pBtnFreeBonus->setVisible(true);
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        m_pBtnFreeBonus->setVisible(true);
#endif
        
        vector<BonusItemInfo> bonusList = ConfigValMgr::getSingleton().filterBonusInfo("bonus");
        if(bonusList.size()<=0)
        {
            m_pBtnFreeBonus->setVisible(false);
        }
        
        int lastTownHallLevel = CCUserDefault::sharedUserDefault()->getIntegerForKey("LastTownHallLevel", 0);
        
        if (lastTownHallLevel <= 0)
        {
            CCUserDefault::sharedUserDefault()->setIntegerForKey("LastTownHallLevel", 1);
        }
        else if (lastTownHallLevel != LogicInfo::getSingleton().m_myBaseLevel)
        {
            CCUserDefault::sharedUserDefault()->setBoolForKey("shopTips1", true);
            CCUserDefault::sharedUserDefault()->setBoolForKey("shopTips2", true);
            CCUserDefault::sharedUserDefault()->setBoolForKey("shopTips3", true);
            CCUserDefault::sharedUserDefault()->setIntegerForKey("LastTownHallLevel", LogicInfo::getSingleton().m_myBaseLevel);
            CCUserDefault::sharedUserDefault()->flush();
        }
        LogicInfo::getSingleton().m_bShowShopTips1 = CCUserDefault::sharedUserDefault()->getBoolForKey("shopTips1");
        LogicInfo::getSingleton().m_bShowShopTips2 = CCUserDefault::sharedUserDefault()->getBoolForKey("shopTips2");
        LogicInfo::getSingleton().m_bShowShopTips3 = CCUserDefault::sharedUserDefault()->getBoolForKey("shopTips3");
        LogicInfo::getSingleton().m_bClickedMoreTips = CCUserDefault::sharedUserDefault()->getBoolForKey("clickedMoreTips");
        
        CCNode *webTip = m_pBtnAnnouncement->getChildByTag(1);
		if (webTip) {
			if (LogicInfo::getSingleton().m_webUrlTip)
			{
				webTip->setVisible(true);
			}
			else
			{
				webTip->setVisible(false);
			}
		}
        
        schedule(schedule_selector(MainUI::setProtectTimeStr), 1.0f);
        schedule(schedule_selector(MainUI::updateMoney), 0.08);
        
        m_goldOff = 0;
        m_oilOff = 0;
        m_gemOff = 0;
        
        m_gold = LogicInfo::getSingleton().m_myInfo.goldex;
        m_oil = LogicInfo::getSingleton().m_myInfo.oilex;
        m_gem = LogicInfo::getSingleton().m_myInfo.gemex;
        
        CCSprite* pExpSprite = ImgMgr::getSingleton().loadSprite("player_level_progress");
        
        m_pExpProgressTimer = CCProgressTimer::create(pExpSprite);
        m_pExpProgressTimer->setType(kCCProgressTimerTypeRadial);
        m_pExpProgressTimer->setPosition(ccp(m_pExpProgress->getContentSize().width/2, m_pExpProgress->getContentSize().height/2));
        m_pExpProgress->addChild(m_pExpProgressTimer);
        
        m_pBtnMail->getChildByTag(1)->setVisible(false);
        m_pBtnAchievement->getChildByTag(1)->setVisible(false);
        
        if(LogicInfo::getSingleton().m_announcementType == ANNOUNCEMENT_TYPE_NO)
            m_pBtnAnnouncement->setVisible(false);
        
        if(FacebookMgr::getSingleton().m_shareFreeGem == false)
        {
            char key[124];
            sprintf(key, "FBShareTip%d",LogicInfo::getSingleton().m_myInfo.uid);
            if(CCUserDefault::sharedUserDefault()->getBoolForKey(key) == false)
            {
                FacebookMgr::getSingleton().m_shareFreeGem = true;
                FacebookMgr::getSingleton().m_isFirstShare = true;
            }
        }
        
        CCNode *pShareTip = m_pBtnFBShare->getChildByTag(1);
        if(FacebookMgr::getSingleton().m_shareFreeGem)
        {
            pShareTip->setVisible(true);
        }
        else
        {
            pShareTip->setVisible(false);
        }
        
        if(LogicInfo::getSingleton().m_pendingNativeXGem > 0)
        {
            LogicInfo::getSingleton().m_addGemType = ADD_GEM_NATIVE_X;
            LogicInfo::getSingleton().requestAdsGem(LogicInfo::getSingleton().m_pendingNativeXGem);
        }
        
        refreshInfo();
        
        countChatNums(-1);
        
        showBtnTip();
        
		bRet = true;
	}while(0);
    
	return bRet;
}

void MainUI::showBtnTip()
{
    CCNode *pMilitaryTip = m_pBtnMilitaryRank->getChildByTag(2);
    CCNode *pLeagueTip = m_pBtnLeague->getChildByTag(2);
    CCNode *pClanWarTip = m_pBtnClanWar->getChildByTag(2);
    
    pLeagueTip->setVisible(false);
    pMilitaryTip->setVisible(false);
    pClanWarTip->setVisible(false);
    
    if(GuideMgr::getSingleton().isGuideEnd())
    {
        if(CCUserDefault::sharedUserDefault()->getBoolForKey("MILITARY_TIP") == false)
        {
            pMilitaryTip->setVisible(true);
        }
        else if(CCUserDefault::sharedUserDefault()->getBoolForKey("LEAGUE_TIP") == false)
        {
            pLeagueTip->setVisible(true);
        }
        else if(CCUserDefault::sharedUserDefault()->getBoolForKey("CLANWAR_TIP") == false)
        {
            pClanWarTip->setVisible(true);
            
            ImgMgr::getSingleton().addSpriteFrames("ui_attacked_npc");
            
            CCNode* pClanWarHelp = CUINodeCache::sharedUINodeCache()->UINodeFromFile("groupwar_help_0.ccbi", this);
            GuideMgr::getSingleton().addGuideUI(pClanWarHelp);
        }
    }
    
    CCNode* pBonuTip = m_pBtnFreeBonus->getChildByTag(1);
    pBonuTip->setVisible(false);
    
    if(CCUserDefault::sharedUserDefault()->getBoolForKey("FB_LOGIN") == false && FacebookMgr::getSingleton().isLogin() == false && FacebookMgr::getSingleton().m_state == 1)
    {
        pBonuTip->setVisible(true);
    }
    
    if(CCUserDefault::sharedUserDefault()->getBoolForKey("FB_INVITE") == false && FacebookMgr::getSingleton().isLogin() == true && FacebookMgr::getSingleton().m_state == 1)
    {
        pBonuTip->setVisible(true);
    }
    
    if(!FacebookMgr::getSingleton().isOpenState())
    {
        pBonuTip->setVisible(false);
    }
    
    if(FacebookMgr::getSingleton().isShare() == false)
        m_pBtnFBShare->setVisible(false);
}

void MainUI::update(float dt)
{
    
}

void MainUI::updateMoney(float dt)
{
    UserInfo myInfo = LogicInfo::getSingleton().m_myInfo;
    
    if (m_gold != myInfo.goldex) {
        
        if (m_goldOff == 0 || m_goldOff < abs(myInfo.goldex-m_gold)/10) {
            m_goldOff = abs(myInfo.goldex - m_gold)/10;
            if (m_goldOff == 0) {
                m_goldOff = 1;
            }
        }
        
        if (m_gold < myInfo.goldex) {
            
            m_gold += m_goldOff;
            if (m_gold >= myInfo.goldex) {
                m_gold = myInfo.goldex;
                m_goldOff = 0;
            }
        }
        else if (m_gold > myInfo.goldex) {
            
            m_gold -= m_goldOff;
            if (m_gold <= myInfo.goldex) {
                m_gold = myInfo.goldex;
                m_goldOff = 0;
            }
        }
        
        if(m_pActGold == NULL) {
            m_pActGold = setChgAni(m_pIconGold);
            m_pActGold->retain();
        }
        
        char str[128];
        sprintf(str, "%d", m_gold);
        m_pGoldCurNums->setString(str);
        
        if (m_gold >= LogicInfo::getSingleton().m_myInfo.maxgoldex) {
            m_pGoldFullTip->setVisible(true);
        }
        else {
            m_pGoldFullTip->setVisible(false);
        }
    }
    
    if (m_oil != myInfo.oilex) {
        
        if (m_oilOff == 0 || m_oilOff < abs(myInfo.oilex-m_oil)/10) {
            m_oilOff = abs(myInfo.oilex - m_oil)/10;
            if (m_oilOff == 0) {
                m_oilOff = 1;
            }
        }
        
        if (m_oil < myInfo.oilex) {
            m_oil += m_oilOff;
            if (m_oil >= myInfo.oilex) {
                m_oil = myInfo.oilex;
                m_oilOff = 0;
            }
        }
        else if (m_oil > myInfo.oilex) {
            m_oil -= m_oilOff;
            if (m_oil <= myInfo.oilex) {
                m_oil = myInfo.oilex;
                m_oilOff = 0;
            }
        }
        
        if(m_pActOil == NULL) {
            m_pActOil = setChgAni(m_pIconElixir);
            m_pActOil->retain();
        }
        
        char str[128];
        sprintf(str, "%d", m_oil);
        m_pElixirCurNums->setString(str);
        
        if (m_oil >= LogicInfo::getSingleton().m_myInfo.maxoilex) {
            m_pOilFullTip->setVisible(true);
        }
        else {
            m_pOilFullTip->setVisible(false);
        }
    }
    
    if (m_gem != myInfo.gemex) {
        
        if (m_gemOff == 0 || m_gemOff < abs(myInfo.gemex-m_gem)/10) {
            m_gemOff = abs(myInfo.gemex - m_gem)/10;
            if (m_gemOff == 0) {
                m_gemOff = 1;
            }
        }
        
        if (m_gem < myInfo.gemex) {
            m_gem += m_gemOff;
            if (m_gem >= myInfo.gemex) {
                m_gem = myInfo.gemex;
                m_gemOff = 0;
            }
        }
        else if (m_gem > myInfo.gemex) {
            m_gem -= m_gemOff;
            if (m_gem <= myInfo.gemex) {
                m_gem = myInfo.gemex;
                m_gemOff = 0;
            }
        }
        
        if (m_pActGem == NULL) {
            m_pActGem = setChgAni(m_pIconGem);
            m_pActGem->retain();
        }
        
        char str[128];
        sprintf(str, "%d", m_gem);
        m_pGemNums->setString(str);
    }
    
    if(m_pActGold != NULL && m_pActGold->isDone()) {
		m_pActGold->release();
		m_pActGold = NULL;
	}
    
	if(m_pActOil != NULL && m_pActOil->isDone()) {
		m_pActOil->release();
		m_pActOil = NULL;
	}
    
    if (m_pActGem != NULL && m_pActGem->isDone()) {
        m_pActGem->release();
        m_pActGem = NULL;
    }
}

void MainUI::showLeagueStartUI()
{
    if (GuideMgr::getSingleton().isGuiding()) {
        return;
    }
    
    int leagueid = CUserData::getUserDataInt(LEAGUE_ID);
    if(m_pLeagueStartUI != NULL || !LeagueMgr::getSingleton().isBattleState() || leagueid == LeagueMgr::getSingleton().m_leagueid || !LeagueMgr::getSingleton().isInLeague())
        return;
    
    CUserData::setUserDataForKey(LEAGUE_ID, LeagueMgr::getSingleton().m_leagueid);
    
    ImgMgr::getSingleton().loadImg("ui_reward_npc");
    m_pLeagueStartUI = CUINode::createWithFile("league_start.ccbi", this);
    m_pLeagueStartUI->setTargetTouch(true);
	PlayerScene::getSingleton().m_pGameWorld->addChild(m_pLeagueStartUI, 1);
}

void MainUI::hideLeagueStartUI()
{
    PlayerScene::getSingleton().m_pGameWorld->removeChild(m_pLeagueStartUI, true);
    ImgMgr::getSingleton().removeRes("ui_reward_npc");
    m_pLeagueStartUI = NULL;
}

void MainUI::showAdsLoadingUI()
{
    if(m_pAdsLoadingUI != NULL)
        return;
    m_pAdsLoadingUI = CUINode::createWithFile("advertisement_loading.ccbi", this);
    PlayerScene::getSingleton().m_pGameWorld->addChild(m_pAdsLoadingUI, 10);
    m_pAdsLoadingUI->setTargetTouch(true);
    m_pAdsLoadingTip->setString(ViewString::getValue("ads_loading_wait").c_str());
}

void MainUI::hideAdsLoadingUI()
{
    if(m_pAdsLoadingUI)
    {
        PlayerScene::getSingleton().m_pGameWorld->removeChild(m_pAdsLoadingUI, true);
    }
    m_pAdsLoadingUI = NULL;
    m_pAdsLoadingTip = NULL;
}

void MainUI::showGetLeagueAwardUI()
{
    if (m_pGetAwardUI!=NULL || !LeagueMgr::getSingleton().m_myGroup.myAward.hasRank)
        return;
    
    SoundMgr::getSingleton().playEffect("military_effect.mp3");
    char str[128];
    m_pGetAwardUI = CUINode::createWithFile("league_result.ccbi", this);
    CCNode *pRoot = m_pGetAwardUI->getContainer()->getChildByTag(1);
    sprintf(str, "%d",LeagueMgr::getSingleton().m_myGroup.myAward.rank);
    CCLabelBMFont *pRankLabel = (CCLabelBMFont *)pRoot->getChildByTag(1);
    pRankLabel->setString(str);
    
    CCNode *pBtnShare = pRoot->getChildByTag(999);
    if(FacebookMgr::getSingleton().isShare() == false)
        pBtnShare->setVisible(false);
    
    CCNode *pAwardNode   = pRoot->getChildByTag(2);
    CCNode *pNoAwardNode = pRoot->getChildByTag(3);
    
    CCSprite *pIcon = (CCSprite *)pRoot->getChildByTag(4);
    sprintf(str, "icon_clanlevel_%d", LeagueMgr::getSingleton().m_myGroup.myAward.levelid);
    CCSpriteFrame* pCurFrame = ImgMgr::getSingleton().getFrame(str);
    pIcon->setDisplayFrame(pCurFrame);
    
    if (LeagueMgr::getSingleton().m_myGroup.myAward.hasAward)
    {
        pNoAwardNode->setVisible(false);
        pAwardNode->setVisible(true);
        CCLabelBMFont *pGemLabel = (CCLabelBMFont *)pAwardNode->getChildByTag(1);
        CCLabelBMFont *pMedalLabel = (CCLabelBMFont *)pAwardNode->getChildByTag(2);
        LeagueMgr::ResInfo awardRes = LeagueMgr::getSingleton().m_myGroup.myAward.resInfo;
        for (int j = 0; j<awardRes.types; j++)
        {
            int type = awardRes.resType[j];
            int num  = awardRes.num[j];
            sprintf(str, "%d",num);
            if (type == MONEY_GEM)
            {
                pGemLabel->setString(str);
            }
            else if (type == LEAGUE_MEDAL)
            {
                pMedalLabel->setString(str);
            }
        }
    }
    else
    {
        pNoAwardNode->setVisible(true);
        pAwardNode->setVisible(false);
    }
    
    PlayerScene::getSingleton().m_pGameWorld->addChild(m_pGetAwardUI,1);
    m_pGetAwardUI->setTargetTouch(true);
}

void MainUI::hideGetLeagueAwardUI()
{
    if (m_pGetAwardUI)
    {
        m_pGetAwardUI->removeFromParentAndCleanup(true);
        m_pGetAwardUI = NULL;
    }
}

void MainUI::showCityWarStartUI()
{
    if (GuideMgr::getSingleton().isGuiding()) {
        return;
    }
    
    if (ClanWarMgr::getSingleton().isWarStateOpen()) {
        int warid = ClanWarMgr::getSingleton().m_warId;
        
        if (warid != CUserData::getUserDataInt(CLANWAR_ID)) {
            
            if (ClanWarMgr::getSingleton().m_warSignupCityId > 0) { //clan war 已报名
                if (m_pCityWarStartUI == NULL) {
                    m_pCityWarStartUI = CUINode::createWithFile("clanwar_start.ccbi", this);
                    m_pCityWarStartUI->setTargetTouch(true);
                    
                    PlayerScene::getSingleton().m_pGameWorld->addChild(m_pCityWarStartUI, 1);
                }
                
                CCNode* pView = m_pCityWarStartUI->getContainer()->getChildByTag(1);
                ((CCLabelBMFont*)pView->getChildByTag(1))->setString(ViewString::getValue("clanwar_signup_title").c_str());
                
                ((CCLabelBMFont*)pView->getChildByTag(2))->setString(ViewString::getValue("clanwar_signup_message").c_str());
                
                CUserData::setUserDataForKey(CLANWAR_ID, warid);
            }
            else if (ClanMgr::getSingleton().m_clanjob == CLAN_JOB_LEADER && ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_SIGNUP) {
                
                if (m_pCityWarStartUI == NULL) {
                    m_pCityWarStartUI = CUINode::createWithFile("clanwar_start.ccbi", this);
                    m_pCityWarStartUI->setTargetTouch(true);

                    PlayerScene::getSingleton().m_pGameWorld->addChild(m_pCityWarStartUI, 1);
                }
                
                CCNode* pView = m_pCityWarStartUI->getContainer()->getChildByTag(1);
                ((CCLabelBMFont*)pView->getChildByTag(1))->setString(ViewString::getValue("clanwar_start_title").c_str());
                ((CCLabelBMFont*)pView->getChildByTag(2))->setString(ViewString::getValue("clanwar_start_message").c_str());
                
                CUserData::setUserDataForKey(CLANWAR_ID, warid);
            }
        }
    }
    else if (GroupWarMgr::getSingleton().isWarStateOpen()) {
        
         int warid = GroupWarMgr::getSingleton().m_warId;
        
        if (warid != CUserData::getUserDataInt(GROUPWAR_ID)) {
            
            if (m_pCityWarStartUI == NULL) {
                m_pCityWarStartUI = CUINode::createWithFile("clanwar_start.ccbi", this);
                m_pCityWarStartUI->setTargetTouch(true);

                PlayerScene::getSingleton().m_pGameWorld->addChild(m_pCityWarStartUI, 1);
            }
            
            CCNode* pView = m_pCityWarStartUI->getContainer()->getChildByTag(1);
            ((CCLabelBMFont*)pView->getChildByTag(1))->setString(ViewString::getValue("groupwar_start_title").c_str());
            ((CCLabelBMFont*)pView->getChildByTag(2))->setString(ViewString::getValue("groupwar_start_message").c_str());
                        
            CUserData::setUserDataForKey(GROUPWAR_ID, warid);
        }
    }
}

void MainUI::hideCityWarStartUI()
{
    if (m_pCityWarStartUI) {
        PlayerScene::getSingleton().m_pGameWorld->removeChild(m_pCityWarStartUI);
        m_pCityWarStartUI = NULL;
    }
}

void MainUI::menuItemCallback(CCObject *pSender, CCControlEvent event)
{
    
}

void MainUI::menuAttackCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->showFindMatchUI();
    
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next();
    }
}

void MainUI::menuAnnouncementCallback(CCObject* pSender, CCControlEvent event)
{
    if(LogicInfo::getSingleton().m_webUrlTip)
    {
        CCNode *webTip = m_pBtnAnnouncement->getChildByTag(1);
        webTip->setVisible(false);
        LogicInfo::getSingleton().m_webUrlTip = false;
    }

    PlayerScene::getSingleton().m_pGameWorld->showAnnouncementUI();
}

void MainUI::menuShopCallback(CCObject* pSender, CCControlEvent event)
{
	PlayerScene::getSingleton().m_pGameWorld->showShop(SHOP_TYPE_TREASURE);
    
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next();
    }
}

void MainUI::menuInfoCallback(CCObject* pSender, CCControlEvent event)
{
    if (PlayerScene::getSingleton().m_pCurBuilding) {
        
        PlayerScene::getSingleton().m_pCurBuilding->m_state = BUILDING_CTRL_SELECTED;
        PlayerScene::getSingleton().m_pGameWorld->showInfoUI();
    }
}

void MainUI::menuBuildCallback(CCObject* pSender, CCControlEvent event)
{
    if (PlayerScene::getSingleton().m_pCurBuilding) {
        
        PlayerScene::getSingleton().m_pCurBuilding->m_state = BUILDING_CTRL_SELECTED;
        PlayerScene::getSingleton().m_pGameWorld->showUpLevelUI();
        
        if (GuideMgr::getSingleton().isGuiding()) {
            GuideMgr::getSingleton().next();
        }
    }
}

void MainUI::menuUpgradeCallback(CCObject* pSender, CCControlEvent event)
{
    if(HeroMgr::getSingleton().m_isSelect)
    {
        LogicInfo::getSingleton().m_showinfo_soldierid = HeroMgr::getSingleton().m_myHero.heroid;
        PlayerScene::getSingleton().m_pGameWorld->showHeroUI();
        return;
    }
    if (PlayerScene::getSingleton().m_pCurBuilding) {
        
        PlayerScene::getSingleton().m_pCurBuilding->m_state = BUILDING_CTRL_SELECTED;
        PlayerScene::getSingleton().m_pGameWorld->showUpLevelUI();
        
        if (GuideMgr::getSingleton().isGuiding()) {
            GuideMgr::getSingleton().next();
        }
    }
}

void MainUI::menuUpgradeRowCallback(CCObject* pSender, CCControlEvent event)
{
    if (PlayerScene::getSingleton().m_lstSelectWall.empty()) {
        return;
    }
    
    MessageUI::show(MSG_UPGRADE_WALLS, ViewString::getValue("upgrade_walls_title").c_str(),
                    CCStringFormat(ViewString::getSingleton().getValue("upgrade_walls_message").c_str(), PlayerScene::getSingleton().getUpgradeWallsMoney()).c_str(), 0, 0);
}

void MainUI::menuTroopsCallback(CCObject* pSender, CCControlEvent event)
{
    if (PlayerScene::getSingleton().m_pCurBuilding) {
        
        PlayerScene::getSingleton().m_pCurBuilding->m_state = BUILDING_CTRL_SELECTED;
        PlayerScene::getSingleton().m_pGameWorld->showBarrackUI();
        
        if (GuideMgr::getSingleton().isGuiding()) {
            GuideMgr::getSingleton().next();
        }
    }
}

void MainUI::menuCancelCallback(CCObject* pSender, CCControlEvent event)
{
    MessageUI::show(MSG_BUILDING_STOP, NULL, NULL);
}

void MainUI::menuFastCallback(CCObject* pSender, CCControlEvent event)
{
    LogicInfo::getSingleton().onFastBuild();
}

void MainUI::menuRemoveCallback(CCObject* pSender, CCControlEvent event)
{
    if (PlayerScene::getSingleton().m_pCurBuilding) {
   
        LogicInfo::getSingleton().onRemoveOther(*PlayerScene::getSingleton().m_pCurBuilding);
    }
}

void MainUI::menuClanCallback(CCObject* pSender, CCControlEvent event)
{
    (PlayerScene::getSingleton().m_pGameWorld)->showClanUI();
}

void MainUI::menuHelpClanCallback(CCObject* pSender, CCControlEvent event)
{
    if (ClanMgr::getSingleton().m_myCurSpace < ClanMgr::getSingleton().m_myMaxSpace) {
        if(ClanMgr::getSingleton().m_askCD > 0)
        {
            int gem = ClanMgr::getSingleton().m_askCD / (60 * 4);
            if(gem <= 0)
                gem = 1;
            
            MessageUI::show(MSG_REQUEST_TROOPS_AGAIN, ViewString::getValue("request_troops_again_title").c_str(), ViewString::getValue("request_troops_again_message").c_str(), gem, -1);
        }
        else
        {
            MessageUI::show(MSG_REQUEST_TROOPS, ViewString::getValue("request_troops_title").c_str(), NULL);
        }
    }
    else {
        PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI->addNote(ViewString::getValue("clan_castle_full_tip").c_str());
    }
}

void MainUI::menuHeroCallback(CCObject* pSender, CCControlEvent event)
{
    (PlayerScene::getSingleton().m_pGameWorld)->showHeroUI();
    
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next();
    }
}

void MainUI::rateApp(CCObject* pSender, CCControlEvent event)
{
    CAppPlatform::sharedAppPlatform()->rateApp();
    
    //    m_pBtn_rate->setVisible(false);
}

void MainUI::getRateGem(CCObject* pSender, CCControlEvent event)
{
    MessageUI::show(MSG_RATE_GEM, "", ViewString::getSingleton().getValue("rate_gem").c_str(), -1);
    
    //    m_pBtn_GetGem->setVisible(false);
}

void MainUI::menuBonusCallback(CCObject* pSender, CCControlEvent event)
{
   
    (PlayerScene::getSingleton().m_pGameWorld)->showBonusUI();
}

void MainUI::menuSettingCallback(CCObject *pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->showSettingUI();
}

void MainUI::menuChatCallback(CCObject *pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->showChatUI();
}

void MainUI::menuResearchCallback(CCObject* pSender, CCControlEvent event)
{
    (PlayerScene::getSingleton().m_pGameWorld)->showLabUI();
}

void MainUI::menuTaskCallback(CCObject* pSender, CCControlEvent event)
{
    (PlayerScene::getSingleton().m_pGameWorld)->showAchievementUI();
    
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next();
    }
}

void MainUI::menuMailCallback(CCObject* pSender, CCControlEvent event)
{
    GameWorld* pGameWorld = PlayerScene::getSingleton().m_pGameWorld;
    
    pGameWorld->showMailUI();
}

void MainUI::menuRankingCallback(CCObject* pSender, CCControlEvent event)
{
    GameWorld* pGameWorld = PlayerScene::getSingleton().m_pGameWorld;
    
    pGameWorld->showRankingUI();
}

void MainUI::menuMilitaryRankCallback(CCObject *pSender, CCControlEvent event)
{
    GameWorld* pGameWorld = PlayerScene::getSingleton().m_pGameWorld;
    pGameWorld->showMilitaryRankUI();
    
    if(CCUserDefault::sharedUserDefault()->getBoolForKey("MILITARY_TIP") == false)
    {
        CCUserDefault::sharedUserDefault()->setBoolForKey("MILITARY_TIP", true);
        CCUserDefault::sharedUserDefault()->flush();
        CCNode *pMilitaryTip = m_pBtnMilitaryRank->getChildByTag(2);
        pMilitaryTip->setVisible(false);
    }
}

void MainUI::menuLeagueCallback(CCObject *pSender, CCControlEvent event)
{
    GameWorld* pGameWorld = PlayerScene::getSingleton().m_pGameWorld;
    
    pGameWorld->showLeagueUI();
    
    if(CCUserDefault::sharedUserDefault()->getBoolForKey("LEAGUE_TIP") == false)
    {
        CCUserDefault::sharedUserDefault()->setBoolForKey("LEAGUE_TIP", true);
        CCUserDefault::sharedUserDefault()->flush();
        CCNode *pLeagueTip = m_pBtnLeague->getChildByTag(2);
        pLeagueTip->setVisible(false);

    }
}

void MainUI::menuLeagueStartUIColseCallback(CCObject *pSender, CCControlEvent event)
{
    hideLeagueStartUI();
}

void MainUI::menuClanWarStartColseCallback(CCObject *pSender, CCControlEvent event)
{
    hideCityWarStartUI();
}

void MainUI::menuGoLeagueCallback(CCObject *pSender, CCControlEvent event)
{
    hideLeagueStartUI();
    GameWorld* pGameWorld = PlayerScene::getSingleton().m_pGameWorld;
    pGameWorld->showLeagueUI();
}

void MainUI::menuGetAwardCallback(CCObject *pSender, CCControlEvent event)
{
    HttpClient::getSingleton().getLeagueAward(LeagueMgr::getSingleton().m_myGroup.myAward.awardid);
    hideGetLeagueAwardUI();
    char str[128];
    if (LeagueMgr::getSingleton().m_myGroup.myAward.hasAward)
    {
        LeagueMgr::ResInfo awardRes = LeagueMgr::getSingleton().m_myGroup.myAward.resInfo;
        for (int j = 0; j<awardRes.types; j++)
        {
            int type = awardRes.resType[j];
            int num  = awardRes.num[j];
            sprintf(str, "%d",num);
            if (type == MONEY_GEM)
            {
                LogicInfo::getSingleton().chgMoney(MONEY_GEM, num);
            }
            else if (type == LEAGUE_MEDAL)
            {
                LogicInfo::getSingleton().m_myInfo.medal+=num;
            }
        }
    }
    
    LeagueMgr::getSingleton().m_myGroup.myAward.hasRank = false;
}

void MainUI::menuClanWarCallback(CCObject *pSender, CCControlEvent event)
{
    if(CCUserDefault::sharedUserDefault()->getBoolForKey("CLANWAR_TIP") == false)
    {
        CCNode *pClanWarTip = m_pBtnClanWar->getChildByTag(2);
        pClanWarTip->setVisible(false);
        
        GuideMgr::getSingleton().removeGuideUI();
        ImgMgr::getSingleton().removeRes("ui_attacked_npc");
        
        CCUserDefault::sharedUserDefault()->setBoolForKey("CLANWAR_TIP", true);
        CCUserDefault::sharedUserDefault()->flush();
    }
    
    if (m_pCityWarStartUI) {
        hideCityWarStartUI();
    }
    
    PlayerScene::getSingleton().m_pGameWorld->showClanWarMapUI();
}

void MainUI::menuWorkerCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->showShop(2);
    
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next();
    }
    
    LogicInfo::getSingleton().m_bShowShopTips2 = false;
    
    CCUserDefault::sharedUserDefault()->setBoolForKey("shopTips2", false);
    CCUserDefault::sharedUserDefault()->flush();
}

void MainUI::menuShieldCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->showShop(6);
}

void MainUI::menuAddGoldCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->showShop(1);
    
    Shop* pShop = PlayerScene::getSingleton().m_pGameWorld->m_pShop;
    pShop->m_pScrollView->setContentOffset(ccp(0, (pShop->m_pScrollView->getViewSize().height - pShop->m_pScrollView->getContentSize().height) * 0.2));
}

void MainUI::menuAddElixirCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->showShop(1);
    
    Shop* pShop = PlayerScene::getSingleton().m_pGameWorld->m_pShop;
    pShop->m_pScrollView->setContentOffset(ccp(0, (pShop->m_pScrollView->getViewSize().height - pShop->m_pScrollView->getContentSize().height) * 0.2));
}

void MainUI::menuBtnFBShareCallback(CCObject* pSender, CCControlEvent event)
{
    if(FacebookMgr::getSingleton().isLogin() == false)
    {
        PlayerScene::getSingleton().m_pGameWorld->showFBLoginUI(0);
            return;
    }
    if(FacebookMgr::getSingleton().m_shareFreeGem)
    {
        char key[124];
        sprintf(key, "FBShareTip%d",LogicInfo::getSingleton().m_myInfo.uid);
        if(CCUserDefault::sharedUserDefault()->getBoolForKey(key) == false)
        {
            CCUserDefault::sharedUserDefault()->setBoolForKey(key, true);
            CCUserDefault::sharedUserDefault()->flush();
        }
        CCNode *pShareTip = m_pBtnFBShare->getChildByTag(1);
        pShareTip->setVisible(false);
    }
//    MessageUI::show(MSG_FB_SHARE_TIP, NULL, NULL);
    FacebookMgr::getSingleton().m_pCurBtn = m_pBtnFBShare;
    FacebookMgr::getSingleton().m_shareType = FBShare_Base;
    PlayerScene::getSingleton().m_pGameWorld->showFBShareUI();
}

void MainUI::menuAddGemCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->showShop(1);
}

void MainUI::menuTrainCallback(CCObject* pSender, CCControlEvent event)
{
    if(HeroMgr::getSingleton().m_isSelect) {
        PlayerScene::getSingleton().m_pGameWorld->showHeroUI();
    }
}

void MainUI::menuTrainSkillCallback(CCObject* pSender, CCControlEvent event)
{
    if(HeroMgr::getSingleton().m_isSelect) {
        PlayerScene::getSingleton().m_pGameWorld->showSkillUI();
    }
}

void MainUI::menuHeroHealCallback(CCObject *pSender, CCControlEvent event)
{
    int gems = HeroMgr::getSingleton().getFillHeroHPGems();
    if (gems > 0) {
        
        char str[256];
        sprintf(str, ViewString::getSingleton().getValue("recover_hero_hp_message").c_str(), gems);
        MessageUI::show(MSG_FILL_HERO_HP, ViewString::getSingleton().getValue("recover_hero_hp_title").c_str(), str, gems, -1);
    }
}

void MainUI::menuSpinCallback(CCObject *pSender, CCControlEvent event)
{
    if(m_pMoreGameUI == NULL)
    {
        ImgMgr::getSingleton().loadImg("ui_reward_npc");
        ImgMgr::getSingleton().loadImg("moregame");
        ImgMgr::getSingleton().addSpriteFrames("shop");
        m_pMoreGameUI = CUINode::create();
        CCNode* pMoreGameUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("moregame.ccbi", this);
        m_pMoreGameUI->setTargetTouch(true);
        m_pMoreGameUI->setContentSize(pMoreGameUI->getContentSize());
        m_pMoreGameUI->addChild(pMoreGameUI);
        PlayerScene::getSingleton().m_pGameWorld->addChild(m_pMoreGameUI, 2);
        
        LogicInfo::getSingleton().m_bClickedMoreTips = true;
        m_pBtnSpin->getChildByTag(1)->setVisible(false);
        CCUserDefault::sharedUserDefault()->setBoolForKey("clickedMoreTips", true);
        CCUserDefault::sharedUserDefault()->flush();
    }
//    (PlayerScene::getSingleton().m_pGameWorld)->showSpinUI();
}

void MainUI::menuGoCallback(CCObject* pSender, CCControlEvent event)
{
    CAppPlatform::sharedAppPlatform()->getMoregame();
}

void MainUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    if(m_pMoreGameUI != NULL)
    {
        m_pMoreGameUI->removeFromParent();
        m_pMoreGameUI = NULL;
        ImgMgr::getSingleton().removeRes("ui_reward_npc");
        ImgMgr::getSingleton().removeRes("moregame");
        ImgMgr::getSingleton().removeRes("shop");
    }
}

void MainUI::menuAdLoadingCloseCallback(CCObject* pSender, CCControlEvent event)
{
    hideAdsLoadingUI();
}

void MainUI::refreshInfo()
{
    char str[128];
    
    UserInfo myInfo = LogicInfo::getSingleton().m_myInfo;
    
    sprintf(str, "uid_%d", myInfo.uid);
    if(myInfo.uname != str)
    {
        m_pName->setString(myInfo.uname.c_str());
    }
    else {
        m_pName->setString("");
    }
    
    sprintf(str, "%d", myInfo.curlevel);
    m_pLevel->setString(str);
    
    sprintf(str, "%d", (int)myInfo.cupex);
    m_pCup->setString(str);
    
    sprintf(str, "%d", (int)myInfo.goldex);
    m_pGoldCurNums->setString(str);
    
    sprintf(str, "%d", (int)myInfo.maxgoldex);
    m_pGoldMaxNums->setString(str);
    
    sprintf(str, "%d", (int)myInfo.oilex);
    m_pElixirCurNums->setString(str);
    
    sprintf(str, "%d", (int)myInfo.maxoilex);
    m_pElixirMaxNums->setString(str);
    
    sprintf(str, "%d", (int)myInfo.gemex);
    m_pGemNums->setString(str);
    
    float percent = (float)myInfo.exp / ExperienceLevels::getSingleton().getCurExp(myInfo.curlevel);
    m_pExpProgressTimer->setPercentage(percent*100);
    
    percent = (float)myInfo.goldex / myInfo.maxgoldex;
    changeProgress(m_pProgressGold, percent);
    
    percent = (float)myInfo.oilex / myInfo.maxoilex;
    changeProgress(m_pProgressElixir, percent);
    
    sprintf(str, "%d/%d", (int)(myInfo.totalworkernumsex - myInfo.buildbuildingnumsex), (int)myInfo.totalworkernumsex);
    m_pWorkerNums->setString(str);
    
    sprintf(str, "player_mr_%d",myInfo.military);
    CCSpriteFrame *frame = ImgMgr::getSingleton().getFrame(str);
    m_pIconMilitary->setDisplayFrame(frame);
    
    sprintf(str, "player_cn_%d",myInfo.military);
    frame = ImgMgr::getSingleton().getFrame(str);
    m_pMiletaryName->setDisplayFrame(frame);
    
    if(LeagueMgr::getSingleton().isInLeague() && LeagueMgr::getSingleton().isBattleState())
    {
        m_pBtnLeague->getChildByTag(11)->setVisible(false);
        m_pBtnLeague->getChildByTag(12)->setVisible(true);
    }
    else
    {
        m_pBtnLeague->getChildByTag(11)->setVisible(true);
        m_pBtnLeague->getChildByTag(12)->setVisible(false);
    }
    
    if(ClanWarMgr::getSingleton().isWarStateOpen() || GroupWarMgr::getSingleton().isWarStateOpen())
    {
        m_pBtnClanWar->getChildByTag(11)->setVisible(false);
        m_pBtnClanWar->getChildByTag(12)->setVisible(true);
    }
    else
    {
        m_pBtnClanWar->getChildByTag(11)->setVisible(true);
        m_pBtnClanWar->getChildByTag(12)->setVisible(false);
    }
    
    setProtectTimeStr(0.0f);
    
    countShopNums();
    
    if (m_gold >= LogicInfo::getSingleton().m_myInfo.maxgoldex) {
        m_pGoldFullTip->setVisible(true);
    }
    else {
        m_pGoldFullTip->setVisible(false);
    }
    
    if (m_oil >= LogicInfo::getSingleton().m_myInfo.maxoilex) {
        m_pOilFullTip->setVisible(true);
    }
    else {
        m_pOilFullTip->setVisible(false);
    }
    
    CCNode *pShareTip = m_pBtnFBShare->getChildByTag(1);
    if(FacebookMgr::getSingleton().m_shareFreeGem)
    {
        pShareTip->setVisible(true);
    }
    else
    {
        pShareTip->setVisible(false);
    }
}

void MainUI::refreshHeroInfo()
{
    int time = HeroMgr::getSingleton().getHeroLastTime();
    
    if (time > 0) {
        m_pHeroHPTime->setString(formatTime(time).c_str());
        m_pHeroHPTime->setVisible(true);
        
        char str[128];
        int gems = HeroMgr::getSingleton().getFillHeroHPGems();
        sprintf(str, "%d", gems);
        ((CCLabelBMFont*)m_pBtnHeroHeal->getChildByTag(2))->setString(str);
        
        if (HeroMgr::getSingleton().m_isSelect) {
            ((CCLabelBMFont*)m_pBtnHeroHeal->getChildByTag(1))->setString(str);
        }
        
    }else{
        m_pHeroHPTime->setVisible(false);
        m_pBtnHeroHeal->setVisible(false);
        
        if (HeroMgr::getSingleton().m_isSelect) {
            m_pBtnHeroHeal->setEnabled(false);
        }
    }
    
    change2Progress(m_pHeroHpProgress, (float)HeroMgr::getSingleton().m_myHero.hp/(float)HeroMgr::getSingleton().m_myHero.maxhp);
}

bool MainUI::onSelect(bool isSelect, Building* pBuilding)
{
    for (int i = 0; i< m_lstBuildingBtn.size(); i++) {
        m_lstBuildingBtn[i]->setVisible(false);
    }
    
    m_lstBuildingBtn.clear();
    
    CCControlButton* pBtnRemove = NULL;
    
	if(isSelect)
	{
        UserBuildingInfo* pUBI = pBuilding->m_pBuildingInfo;
        
#if(COC_CLAN == 0)
        if(pUBI->buildingid / 100 == 305)
            return false;
#endif
        
        if(pUBI->state == BUILDING_STATE_BUILD || pUBI->state == BUILDING_STATE_UPGRADING)
        {
            if(pUBI->buildingid > 90000)
            {
                m_lstBuildingBtn.push_back(m_pBtnFast);
            }
            else
            {
                m_lstBuildingBtn.push_back(m_pBtnInfo);
                m_lstBuildingBtn.push_back(m_pBtnCancel);
                m_lstBuildingBtn.push_back(m_pBtnFast);
                
                if (GuideMgr::getSingleton().isGuiding() && GuideMgr::getSingleton().m_pActiveBuilding == pBuilding) {
                    GuideMgr::getSingleton().next();
                }
            }
        }
        else if(pUBI->state == BUILDING_STATE_NORMAL)
        {
            if(pUBI->buildingid >= 30200 && pUBI->buildingid <= 30299)
            {
                m_lstBuildingBtn.push_back(m_pBtnInfo);
                m_lstBuildingBtn.push_back(m_pBtnUpgrade);
                m_lstBuildingBtn.push_back(m_pBtnTroops);
                
                if (GuideMgr::getSingleton().isGuiding() && GuideMgr::getSingleton().m_pActiveBuilding == pBuilding) {
                    GuideMgr::getSingleton().next();
                }
            }
            else if(pUBI->buildingid > 90000)
            {
                //                m_lstBuildingBtn.push_back(m_pBtnInfo);
                pBtnRemove = m_pBtnRemoveByGold;
                m_lstBuildingBtn.push_back(m_pBtnRemoveByGold);
            }
            else if(pUBI->buildingid > 60000)
            {
                m_pBtnSell->getChildByTag(1)->setVisible(false);
                m_pBtnSell->getChildByTag(2)->setVisible(false);
                
                pBtnRemove = m_pBtnSell;
                m_lstBuildingBtn.push_back(m_pBtnSell);
                
                if (PlayerScene::getSingleton().m_pCurBuilding->m_info.removecost > 0) {
                    m_pBtnSell->getChildByTag(1)->setVisible(true);
                    m_pBtnSell->getChildByTag(2)->setVisible(true);
                }
            }
            else if(pUBI->buildingid > 50000)
            {
                //                m_lstBuildingBtn.push_back(m_pBtnInfo);
                //m_lstBuildingBtn.push_back(m_pBtnInfo);
            }
            else if(pUBI->buildingid >= 20200 && pUBI->buildingid <= 20299)
            {
                m_lstBuildingBtn.push_back(m_pBtnInfo);
            }
            else if(pUBI->buildingid >= 30500 && pUBI->buildingid <= 30599)
            {
                if(pUBI->buildingid == 30501)
                {
                    m_lstBuildingBtn.push_back(m_pBtnInfo);
                    m_lstBuildingBtn.push_back(m_pBtnBuild);
                }
                else
                {
                    m_lstBuildingBtn.push_back(m_pBtnInfo);
                    m_lstBuildingBtn.push_back(m_pBtnUpgrade);
                    
                    if(ClanMgr::getSingleton().m_myclan.id > 0)
                    {
                        if (ClanMgr::getSingleton().m_myCurSpace < ClanMgr::getSingleton().m_myMaxSpace) {
                            
                            m_lstBuildingBtn.push_back(m_pBtnClanTroopsRequest);
                            
                            if(ClanMgr::getSingleton().m_askCD > 0)
                            {
                                m_pBtnClanTroopsRequest->getChildByTag(1)->setVisible(true);
                                m_pBtnClanTroopsRequest->getChildByTag(2)->setVisible(true);
                                
                                char str[128];
                                
                                int gem = (ClanMgr::getSingleton().m_askCD - 1) / (60 * 4);
                                if(gem <= 0)
                                    gem = 1;
                                
                                sprintf(str, "%d", gem);
                                
                                ((CCLabelBMFont*)m_pBtnClanTroopsRequest->getChildByTag(1))->setString(str);;
                            }
                            else
                            {
                                m_pBtnClanTroopsRequest->getChildByTag(1)->setVisible(false);
                                m_pBtnClanTroopsRequest->getChildByTag(2)->setVisible(false);
                            }
                            
                        }
                        else {
                            
                            m_lstBuildingBtn.push_back(m_pBtnClanTroopsFull);
                        }
                    }
                    
                    m_lstBuildingBtn.push_back(m_pBtnClan);
                }
            }
            else if(pUBI->buildingid >= 30300 && pUBI->buildingid <= 30399)
            {
                m_lstBuildingBtn.push_back(m_pBtnInfo);
                if(LogicInfo::getSingleton().canUpgradeLaboratry(*PlayerScene::getSingleton().m_pCurBuilding))
                {
                    m_lstBuildingBtn.push_back(m_pBtnUpgrade);
                }
                
                m_lstBuildingBtn.push_back(m_pBtnResearch);
                
            }
            else if(pUBI->buildingid >= 10300 && pUBI->buildingid <= 10399)
            {
                m_lstBuildingBtn.push_back(m_pBtnInfo);
                m_lstBuildingBtn.push_back(m_pBtnUpgrade);
                if(PlayerScene::getSingleton().canSelectWallLine())
                {
                    m_lstBuildingBtn.push_back(m_pBtnRow);
                }
            }
            else
            {
                m_lstBuildingBtn.push_back(m_pBtnInfo);
                m_lstBuildingBtn.push_back(m_pBtnUpgrade);
                
                if (GuideMgr::getSingleton().isGuiding() && GuideMgr::getSingleton().m_pActiveBuilding == pBuilding) {
                    GuideMgr::getSingleton().next();
                }
            }
        }
        
        playerEffect(pUBI->buildingid / 100);
        
        if(m_pBtnUpgrade != NULL)
        {
            BuildingInfo info = PlayerScene::getSingleton().m_pCurBuilding->m_info;
            const BuildingInfo* tmp = BuildingMgr::getSingleton().getBuildingInfo(info.buildingid + 1);
            if(tmp == NULL)
            {
                
                std::vector<CCControlButton*>::iterator btnUpgrade = std::find(m_lstBuildingBtn.begin(), m_lstBuildingBtn.end(), m_pBtnUpgrade);
                if(btnUpgrade != m_lstBuildingBtn.end())
                {
                    m_pBtnUpgrade->setVisible(false);
                    m_lstBuildingBtn.erase(btnUpgrade);
                }
            }
        }
        
        onRefurbishBuildingBtn();
        
        if (m_pBtnBuild->isVisible()) {
            
            char str[128];
            
            BuildingInfo info = PlayerScene::getSingleton().m_pCurBuilding->m_info;
            const BuildingInfo* tmp = BuildingMgr::getSingleton().getBuildingInfo(info.buildingid + 1);
            
            sprintf(str, "%d", (int)tmp->buildcost);
            
            CCLabelBMFont* pLabelCost = (CCLabelBMFont*)m_pBtnBuild->getChildByTag(1);
            pLabelCost->setString(str);
            
            if(info.buildres == MONEY_GOLD)
            {
                if(tmp->buildcost > LogicInfo::getSingleton().m_myInfo.goldex)
                {
                    pLabelCost->setColor(ccRED);
                }
                else
                {
                    pLabelCost->setColor(ccWHITE);
                }
                
                CCSpriteFrame* pSpriteFrame = ImgMgr::getSingleton().getFrame("map_Coins_icon");
                ((CCSprite*)m_pBtnBuild->getChildByTag(3))->setDisplayFrame(pSpriteFrame);
            }
            else if(info.buildres == MONEY_ELIXIR)
            {
                if(tmp->buildcost > LogicInfo::getSingleton().m_myInfo.oilex)
                {
                    pLabelCost->setColor(ccRED);
                }
                else
                {
                    pLabelCost->setColor(ccWHITE);
                }
                CCSpriteFrame* pSpriteFrame = ImgMgr::getSingleton().getFrame("map_Elixir_icon");
                ((CCSprite*)m_pBtnBuild->getChildByTag(3))->setDisplayFrame(pSpriteFrame);
            }
        }
        else if(m_pBtnUpgrade->isVisible())
        {
            char str[128];
            
            BuildingInfo info = PlayerScene::getSingleton().m_pCurBuilding->m_info;
            const BuildingInfo* tmp = BuildingMgr::getSingleton().getBuildingInfo(info.buildingid + 1);
            
            sprintf(str, "%d", (int)tmp->buildcost);
            
            CCLabelBMFont* pLabelCost = (CCLabelBMFont*)m_pBtnUpgrade->getChildByTag(1);
            pLabelCost->setString(str);
            
            if(info.buildres == MONEY_GOLD)
            {
                if(tmp->buildcost > LogicInfo::getSingleton().m_myInfo.goldex)
                {
                    pLabelCost->setColor(ccRED);
                }
                else
                {
                    pLabelCost->setColor(ccWHITE);
                }
                
                CCSpriteFrame* pSpriteFrame = ImgMgr::getSingleton().getFrame("map_Coins_icon");
                ((CCSprite*)m_pBtnUpgrade->getChildByTag(3))->setDisplayFrame(pSpriteFrame);
            }
            else if(info.buildres == MONEY_ELIXIR)
            {
                if(tmp->buildcost > LogicInfo::getSingleton().m_myInfo.oilex)
                {
                    pLabelCost->setColor(ccRED);
                }
                else
                {
                    pLabelCost->setColor(ccWHITE);
                }
                CCSpriteFrame* pSpriteFrame = ImgMgr::getSingleton().getFrame("map_Elixir_icon");
                ((CCSprite*)m_pBtnUpgrade->getChildByTag(3))->setDisplayFrame(pSpriteFrame);
            }
        }
        else if(pBtnRemove != NULL)
        {
            char str[128];
            
            BuildingInfo info = PlayerScene::getSingleton().m_pCurBuilding->m_info;
            sprintf(str, "%d", (int)info.removecost);
            
            CCLabelBMFont* pLabelCost = (CCLabelBMFont*)pBtnRemove->getChildByTag(1);
            pLabelCost->setString(str);
            
            if(info.buildres == MONEY_GOLD)
            {
                if(info.removecost > LogicInfo::getSingleton().m_myInfo.goldex)
                {
                    pLabelCost->setColor(ccRED);
                }
                else
                {
                    pLabelCost->setColor(ccWHITE);
                }
            }
            else if(info.buildres == MONEY_ELIXIR)
            {
                if(info.removecost > LogicInfo::getSingleton().m_myInfo.oilex)
                {
                    pLabelCost->setColor(ccRED);
                }
                else
                {
                    pLabelCost->setColor(ccWHITE);
                }
            }
            else if(info.buildres == MONEY_GEM)
            {
                if(info.removecost > LogicInfo::getSingleton().m_myInfo.gemex)
                {
                    pLabelCost->setColor(ccRED);
                }
                else
                {
                    pLabelCost->setColor(ccWHITE);
                }
            }
        }
        else if(m_pBtnFast->isVisible() == true)
        {
            char str[128];
            
            int i = PlayerScene::getSingleton().m_pCurBuilding->m_lasttime / 240;
            if(i < 1)
                i = 1;
            
            sprintf(str, "%d", i);
            
            CCLabelBMFont* pLabelCost = (CCLabelBMFont*)m_pBtnFast->getChildByTag(1);
            pLabelCost->setString(str);
        }
	}
    
	return true;
}

void MainUI::onSelectHero(bool isSelect)
{
    if (!GuideMgr::getSingleton().isGuideEnd()) {
        return;
    }
    
    for (int i = 0; i< m_lstBuildingBtn.size(); i++) {
        m_lstBuildingBtn[i]->setVisible(false);
    }
    m_lstBuildingBtn.clear();
    
    
    int state = HeroMgr::getSingleton().m_heroState;
    if (isSelect) {
        char str[128];
        m_lstBuildingBtn.push_back(m_pBtnTrain);
        
        if(state == HERO_STATE_NORMAL)
        {
            const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(HeroMgr::getSingleton().m_myHero.heroid + 1);
            if(pInfo != NULL)
            {
                m_lstBuildingBtn.push_back(m_pBtnUpgrade);
                sprintf(str, "%d", (int)pInfo->upgradeCost);
                CCLabelBMFont* pLabelCost = (CCLabelBMFont*)m_pBtnUpgrade->getChildByTag(1);
                pLabelCost->setString(str);
                
                if(pInfo->upgradeRes == MONEY_GOLD)
                {
                    if(pInfo->upgradeCost > LogicInfo::getSingleton().m_myInfo.goldex)
                    {
                        pLabelCost->setColor(ccRED);
                    }
                    else
                    {
                        pLabelCost->setColor(ccWHITE);
                    }
                    
                    CCSpriteFrame* pSpriteFrame = ImgMgr::getSingleton().getFrame("map_Coins_icon");
                    ((CCSprite*)m_pBtnUpgrade->getChildByTag(3))->setDisplayFrame(pSpriteFrame);
                }
                else if(pInfo->upgradeRes == MONEY_ELIXIR)
                {
                    if(pInfo->upgradeCost > LogicInfo::getSingleton().m_myInfo.oilex)
                    {
                        pLabelCost->setColor(ccRED);
                    }
                    else
                    {
                        pLabelCost->setColor(ccWHITE);
                    }
                    CCSpriteFrame* pSpriteFrame = ImgMgr::getSingleton().getFrame("map_Elixir_icon");
                    ((CCSprite*)m_pBtnUpgrade->getChildByTag(3))->setDisplayFrame(pSpriteFrame);
                }
            }
            
            m_lstBuildingBtn.push_back(m_pBtnTrainSkill);
            
            if(HeroMgr::getSingleton().m_isFreeTrain)
                m_pBtnTrainSkill->getChildByTag(1)->setVisible(true);
            else
                m_pBtnTrainSkill->getChildByTag(1)->setVisible(false);
            
        }
        else if(state == HERO_STATE_UPGRADING)
        {
            m_lstBuildingBtn.push_back(m_pBtnFast);
            CCLabelBMFont* pLabelCost = (CCLabelBMFont*)m_pBtnFast->getChildByTag(1);
            sprintf(str, "%d",LogicInfo::getSingleton().countBuildingHeroGem());
            pLabelCost->setString(str);
            m_lstBuildingBtn.push_back(m_pBtnCancel);
        }
        
        
        
        if (HeroMgr::getSingleton().m_myHero.hp < HeroMgr::getSingleton().m_myHero.maxhp) {
            m_lstBuildingBtn.push_back(m_pBtnHeroHeal);
        }
        
        int gems = HeroMgr::getSingleton().getFillHeroHPGems();
        
        sprintf(str, "%d", gems);
        
        ((CCLabelBMFont*)m_pBtnHeroHeal->getChildByTag(1))->setString(str);
        
        onRefurbishBuildingBtn();
        
        SoundMgr::getSingleton().playEffect(CCString::createWithFormat("player_deploy_%d.m4a",HeroMgr::getSingleton().m_myHero.heroid/100)->getCString());
    }
    
    HeroMgr::getSingleton().m_isSelect = isSelect;
}

void MainUI::playerEffect(int i)
{
    if(i == 302)
    {
        SoundMgr::getSingleton().playEffect("select_barrack.mp3");
    }
    else if(i == 206)
    {
        SoundMgr::getSingleton().playEffect("select_gold.mp3");
    }
    else if(i == 204)
    {
        SoundMgr::getSingleton().playEffect("select_oil.mp3");
    }
    else if(i > 200 && i < 300)
    {
        SoundMgr::getSingleton().playEffect("select_build.mp3");
    }
    else if(i > 100 && i < 200)
    {
        SoundMgr::getSingleton().playEffect("select_attack.mp3");
    }
}

void MainUI::onRefurbishBuildingBtn()
{
	if(m_lstBuildingBtn.empty())
		return;
    
	int nums = m_lstBuildingBtn.size();
	int sw = CCDirector::sharedDirector()->getWinSize().width;
	int bw = m_lstBuildingBtn[0]->getContentSize().width;
	int ox;
    
	if(nums == 1)
		ox = (sw - bw) / 2 + bw/2;
	else
		ox = (sw - bw * nums - (nums - 1) * 10) / 2 + bw/2;
    
	for(int i = 0; i < nums; ++i)
	{
        m_lstBuildingBtn[i]->setOpacity(0);
		m_lstBuildingBtn[i]->setPosition(ccp(ox, 60));
        m_lstBuildingBtn[i]->setVisible(true);
        
        CCFadeIn* fi1 = CCFadeIn::create(0.5f * (i + 1) * 0.3f);
        CCActionInterval* action1 = CCSequence::create(fi1, NULL);
        
        CCMoveTo* mt1 = CCMoveTo::create(0.0f, ccp(ox, 60));
        CCMoveTo* mt2 = CCMoveTo::create(0.3f * (i + 1) * 0.2f, ccp(ox, 120));
        CCMoveTo* mt3 = CCMoveTo::create(0.1f, ccp(ox, 100));
        //        CCCallFuncN* callFunc = CCCallFuncN::create(m_lstBuildingBtn[i], callfuncN_selector(MainUI::actionEndCallback));
        CCActionInterval* action2 = CCSequence::create(mt1, mt2, mt3, NULL);
        
        m_lstBuildingBtn[i]->runAction(action1);
        m_lstBuildingBtn[i]->runAction(action2);
        
		ox += bw + 10;
	}
    
    GuideMgr::getSingleton().refreshArrowPos();
}

void MainUI::actionEndCallback(CCNode *pSender)
{
}

bool MainUI::onNotify(int notifyid, const void* pVal)
{
	if(NOTIFY_BASEINFO == notifyid)
	{
		refreshInfo();
	}
    else if (NOTIFY_FB_SHARE_FINISH == notifyid)
    {
        CCNode *pShareTip = m_pBtnFBShare->getChildByTag(1);
        if(FacebookMgr::getSingleton().m_shareFreeGem)
        {
            pShareTip->setVisible(true);
        }
        else
        {
            pShareTip->setVisible(false);
        }
    }
    else if(NOTIFY_ADS_LOADINF_FAILD == notifyid)
    {
        if(m_pAdsLoadingTip)
        {
            m_pAdsLoadingTip->setString(ViewString::getValue("ads_loading_faild").c_str());
            
            scheduleOnce(schedule_selector(MainUI::hideAdsLoadingUI), 3.0);
        }
    }
    else if(NOTIFY_GUIDE_REFRESH == notifyid)
	{
        guide();
    }
    else if(NOTIFY_UPLEVEL == notifyid)
    {
        CCNode* levelUP = CUINodeCache::sharedUINodeCache()->UINodeFromFile("levelup_am.ccbi", this);
        this->addChild(levelUP);
    }
    else if(NOTIFY_ACHIEVEMENT_OK == notifyid)
    {
        countAchievementNums();
    }
    else if(NOTIFY_MAIL_BATTLELOG == notifyid)
	{
        //        //test
        //        for (int i = 0; i < 2; i ++) {
        //            FightMsg msg;
        //            msg.fsi.user[0].curlevel = 11;
        //            msg.fsi.user[0].name[0] = 'B';
        //            msg.fsi.user[0].name[1] = 'e';
        //            msg.fsi.user[0].name[2] = 'n';
        //            msg.fsi.user[0].name[3] = 's';
        //            msg.fsi.user[0].name[4] = 'o';
        //            msg.fsi.user[0].name[5] = 'n';
        //            msg.fsi.user[0].name[6] = '\0';
        //            msg.time = getCurrentTime();
        //
        //            LogicInfo::getSingleton().m_vecDefenseLog.push_back(msg);
        //        }
        
        if (LogicInfo::getSingleton().m_bShowRaidNotify) {
            
            countMailNums(-1);
            
            for(int i = 0; i < LogicInfo::getSingleton().m_vecDefenseLog.size(); ++i)
            {
                FightMsg msg = LogicInfo::getSingleton().m_vecDefenseLog[i];
                
                if(msg.msgid != -1)
                {
                    if(msg.time >= LogicInfo::getSingleton().m_lastLoginTime - 60)
                    {
                        PlayerScene::getSingleton().m_pGameWorld->showRaidNotifyUI();
                        break;
                    }
                }
            }
            
            LogicInfo::getSingleton().m_bShowRaidNotify = false;
        }
    }
    else if(NOTIFY_NOHERO == notifyid)
    {
        //        if(HeroMgr::getSingleton().myHero.heroid == 0)
        //        {
        //            PlayerScene::getSingleton().m_pGameWorld->showCreateUI();
        //        }
    }
    else if (NOTIFY_HEROHPCHG == notifyid)
    {
        refreshInfo();
    }
    else if (NOTIFY_REVENGE_USER == notifyid)
    {
        LogicInfo::getSingleton().revengeAttackUserEx(LogicInfo::getSingleton().m_revengeUserId, LogicInfo::getSingleton().m_revengeBattleId);
        
        PlayerScene::getSingleton().m_pGameWorld->hideMailUI();
        PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_NORMAL);
    }
    else if (NOTIFY_GROUPWAR_INFO == notifyid) {

        showCityWarStartUI();
    }
    
	return true;
}


void MainUI::actionCallback(CCNode* pSender)
{
    CCSprite *pSprite = (CCSprite *) pSender;
    pSprite->release();
}

CCAction* MainUI::setChgAni(CCSprite* spr)
{
	CCPoint pt1 = spr->getPosition();
	CCPoint pt2 = spr->getPosition();
    
	pt1.x -= spr->getContentSize().width * 0.1f / 2;
	pt1.y -= spr->getContentSize().height * 0.1f / 2;
    
	CCMoveTo* mv1 = CCMoveTo::create(0.08f, pt1);
	CCMoveTo* mv2 = CCMoveTo::create(0.08f, pt2);
    
	CCScaleTo* sc1 = CCScaleTo::create(0.08f, spr->getScale() + 0.1);
	CCScaleTo* sc2 = CCScaleTo::create(0.08f, spr->getScale());
    
	CCFiniteTimeAction* action1 = CCSequence::create(sc1, sc2, sc1, sc2, sc1, sc2, NULL);
	CCFiniteTimeAction* action2 = CCSequence::create(mv1, mv2, mv1, mv2, mv1, mv2, NULL);
    
	CCAction* pAct = spr->runAction(action1);
	spr->runAction(action2);
    
	return pAct;
}

std::string MainUI::getProtectTimeStr()
{
    std::string timeStr = "None";
    
    if(LogicInfo::getSingleton().m_myInfo.lastProtectTime <= 0){
        LogicInfo::getSingleton().m_myInfo.lastProtectTime = 0;
        return timeStr;
    }
    
    m_protectTime = LogicInfo::getSingleton().m_myInfo.lastProtectTime;;
    return formatTime((int64_t)m_protectTime);
}

void MainUI::setProtectTimeStr(float fDelta)
{
    if (fDelta > 0) {
        LogicInfo::getSingleton().m_myInfo.lastProtectTime --;
    }
    
    m_pShieldTime->setString(getProtectTimeStr().c_str());
}

void MainUI::countShopNums()
{
    int nums = 0;
    char str[128];
    
    if(LogicInfo::getSingleton().m_bShowShopTips1)
        nums += LogicInfo::getSingleton().showNums(2);
    if(LogicInfo::getSingleton().m_bShowShopTips2)
        nums += LogicInfo::getSingleton().showNums(3);
    if(LogicInfo::getSingleton().m_bShowShopTips3)
        nums += LogicInfo::getSingleton().showNums(4);
    
    CCSprite* pShopTip = (CCSprite*)m_pBtnShop->getChildByTag(1);
    
    if(!LogicInfo::getSingleton().m_bShowShopTips1 && !LogicInfo::getSingleton().m_bShowShopTips2 && !LogicInfo::getSingleton().m_bShowShopTips3)
    {
        pShopTip->setVisible(false);
    }
    else
    {
        pShopTip->setVisible(true);
    }
    
    if(nums > 0)
    {
        if(nums > 99)
            nums = 99;
        pShopTip->setVisible(true);
        sprintf(str, "%d", nums);
        
        CCLabelBMFont* pTipNums = (CCLabelBMFont*)pShopTip->getChildByTag(1);
        pTipNums->setString(str);
    }
    else
    {
        pShopTip->setVisible(false);
    }
    
    CCSprite* pMoreTips = (CCSprite*)m_pBtnSpin->getChildByTag(1);
    if (LogicInfo::getSingleton().m_bClickedMoreTips)
    {
        pMoreTips->setVisible(false);
    }
    else
    {
        pMoreTips->setVisible(true);
    }
}

void MainUI::countAchievementNums()
{
    CCSprite* pAchievementTip = (CCSprite*)m_pBtnAchievement->getChildByTag(1);
    
    int nums = LogicInfo::getSingleton().countAchievementNums();
    if(nums == 0)
    {
        pAchievementTip->setVisible(false);
    }
    else
    {
        char str[128];
        sprintf(str, "%d", nums);
        CCLabelBMFont* pTipNums = (CCLabelBMFont*)pAchievementTip->getChildByTag(1);
        pTipNums->setString(str);
        pAchievementTip->setVisible(true);
    }
}

int MainUI::countMailNums(int type)
{
    int nums = 0;
    int readtime = 0;
    
    if (type == -1 || type == 0) {
        readtime = CCUserDefault::sharedUserDefault()->getIntegerForKey("DEFENSE_LOG_READ_TIME", 0);
        for(int i = 0; i < LogicInfo::getSingleton().m_vecDefenseLog.size(); ++i)
        {
            FightMsg msg = LogicInfo::getSingleton().m_vecDefenseLog[i];
            
            if(msg.msgid != -1 && msg.time > readtime)
            {
                nums ++;
            }
        }
    }
    
    if (type == -1 || type == 1) {
        readtime = CCUserDefault::sharedUserDefault()->getIntegerForKey("ATTACK_LOG_READ_TIME", 0);
        for(int i = 0; i < LogicInfo::getSingleton().m_vecAttackLog.size(); ++i)
        {
            FightMsg msg = LogicInfo::getSingleton().m_vecAttackLog[i];
            
            if(msg.msgid != -1 && msg.time > readtime)
            {
                nums ++;
            }
        }
    }
    
    if (type == -1 || type == 2) {
        readtime = CCUserDefault::sharedUserDefault()->getIntegerForKey("NEWS_READ_TIME", 0);
        for (int i = 0; i < LogicInfo::getSingleton().m_vecMail.size(); ++i) {
            MailInfo* mailInfo = &LogicInfo::getSingleton().m_vecMail[i];
            
            if (mailInfo->time > readtime) {
                nums ++;
            }
        }
    }
    
    if (type == -1) {
        if (nums > 0) {
            char str[128];
            sprintf(str, "%d", nums);
            
            m_pBtnMail->getChildByTag(1)->setVisible(true);
            ((CCLabelBMFont*)(m_pBtnMail->getChildByTag(1)->getChildByTag(1)))->setString(str);
        }
        else
        {
            m_pBtnMail->getChildByTag(1)->setVisible(false);
        }
    }
 
    return nums;
}

int MainUI::countChatNums(int type)
{
    int count = 0;
    
    int readtime1 = CCUserDefault::sharedUserDefault()->getIntegerForKey("WORLD_CHAT_READ_TIME", 0);
    map<int, ChatData>::iterator it1 = ChatMgr::getSingleton().m_mapWorldChat.begin();
    for (; it1 != ChatMgr::getSingleton().m_mapWorldChat.end(); it1 ++) {
        
        if (it1->second.time > readtime1) {
            
            if (type == 0) {
                readtime1 = it1->second.time;
            }
            else {
                count ++;
            }
        }
    }
    
    if (type == 0) {
        CCUserDefault::sharedUserDefault()->setIntegerForKey("WORLD_CHAT_READ_TIME", readtime1);
        CCUserDefault::sharedUserDefault()->flush();
    }
    
    int readtime2 = CCUserDefault::sharedUserDefault()->getIntegerForKey("CLAN_CHAT_READ_TIME", 0);
    map<int, ChatData>::iterator it2 = ChatMgr::getSingleton().m_mapClanChat.begin();
    for (; it2 != ChatMgr::getSingleton().m_mapClanChat.end(); it2 ++) {
        
        if (it2->second.time > readtime2) {
            
            if (type == 1) {
                readtime2 = it2->second.time;
            }
            else {
                count ++;
            }
        }
    }
    
    if (type == 1) {
        CCUserDefault::sharedUserDefault()->setIntegerForKey("CLAN_CHAT_READ_TIME", readtime2);
        CCUserDefault::sharedUserDefault()->flush();
    }
    
    m_pBtnChat->getChildByTag(1)->setVisible(false);
    if (count > 0) {
        m_pBtnChat->getChildByTag(1)->setVisible(true);
    }
    
    return count;
}

void MainUI::countChatNums(int type, ChatData chatData)
{
    if (m_pBtnChat->getChildByTag(1)->isVisible()) {
        return;
    }
    
    if (type == 0) {
        
        int readtime = CCUserDefault::sharedUserDefault()->getIntegerForKey("WORLD_CHAT_READ_TIME", 0);
        if (chatData.time > readtime) {
            m_pBtnChat->getChildByTag(1)->setVisible(true);
        }
    }
    else if (type == 1) {
        
        int readtime = CCUserDefault::sharedUserDefault()->getIntegerForKey("CLAN_CHAT_READ_TIME", 0);
        if (chatData.time > readtime) {
            m_pBtnChat->getChildByTag(1)->setVisible(true);
        }
    }

}

void MainUI::guide()
{
    
}

void MainUI::onSelectRow(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().selectWallLine();
    
    while(!m_lstBuildingBtn.empty())
    {
        m_lstBuildingBtn[m_lstBuildingBtn.size() - 1]->setVisible(false);
        m_lstBuildingBtn.pop_back();
    }

    int upgradeCost = PlayerScene::getSingleton().getUpgradeWallsMoney();
    if (upgradeCost > 0) {
        char str[128];
        sprintf(str, "%d", upgradeCost);
        
        CCLabelBMFont* pLabelCost = (CCLabelBMFont*)m_pBtnUpgradeRow->getChildByTag(1);
        pLabelCost->setString(str);
        
        if(upgradeCost > LogicInfo::getSingleton().m_myInfo.goldex)
        {
            pLabelCost->setColor(ccRED);
        }
        else
        {
            pLabelCost->setColor(ccWHITE);
        }
        
        CCSpriteFrame* pSpriteFrame = ImgMgr::getSingleton().getFrame("map_Coins_icon");
        ((CCSprite*)m_pBtnUpgrade->getChildByTag(3))->setDisplayFrame(pSpriteFrame);
        
        m_lstBuildingBtn.push_back(m_pBtnUpgradeRow);
    }
    
    m_lstBuildingBtn.push_back(m_pBtnRotate);
    
    onRefurbishBuildingBtn();
}

void MainUI::onSelectRotate(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().rotationSelectWallLine();
    
    while(!m_lstBuildingBtn.empty())
    {
        m_lstBuildingBtn[m_lstBuildingBtn.size()-1]->setVisible(false);
        m_lstBuildingBtn.pop_back();
    }
    
    if (PlayerScene::getSingleton().canBuildWallLine()) {
        
        int upgradeCost = PlayerScene::getSingleton().getUpgradeWallsMoney();
        if (upgradeCost > 0) {
            char str[128];
            sprintf(str, "%d", upgradeCost);
            
            CCLabelBMFont* pLabelCost = (CCLabelBMFont*)m_pBtnUpgradeRow->getChildByTag(1);
            pLabelCost->setString(str);
            
            if(upgradeCost > LogicInfo::getSingleton().m_myInfo.goldex)
            {
                pLabelCost->setColor(ccRED);
            }
            else
            {
                pLabelCost->setColor(ccWHITE);
            }
            
            CCSpriteFrame* pSpriteFrame = ImgMgr::getSingleton().getFrame("map_Coins_icon");
            ((CCSprite*)m_pBtnUpgrade->getChildByTag(3))->setDisplayFrame(pSpriteFrame);
            
            m_lstBuildingBtn.push_back(m_pBtnUpgradeRow);
        }
    }
    
    m_lstBuildingBtn.push_back(m_pBtnRotate);
    
    onRefurbishBuildingBtn();
}

void MainUI::menuEditCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().unselectCurBuilding();
    PlayerScene::getSingleton().m_pGameWorld->showEditUI();
}

bool MainUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerName", CCLabelTTF*, m_pName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerLevel", CCLabelBMFont*, m_pLevel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerCup", CCLabelBMFont*, m_pCup);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ProgressGold", CCScale9Sprite*, m_pProgressGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ProgressElixir", CCScale9Sprite*, m_pProgressElixir);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GoldCurNums", CCLabelBMFont*, m_pGoldCurNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GoldMaxNums", CCLabelBMFont*, m_pGoldMaxNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ElixirCurNums", CCLabelBMFont*, m_pElixirCurNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ElixirMaxNums", CCLabelBMFont*, m_pElixirMaxNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GemNums", CCLabelBMFont*, m_pGemNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WorkerNums", CCLabelBMFont*, m_pWorkerNums);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ShieldTime", CCLabelBMFont*, m_pShieldTime);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "militaryName", CCSprite*, m_pMiletaryName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "IconMilitary", CCSprite*, m_pIconMilitary);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "IconGold", CCSprite*, m_pIconGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "IconElixir", CCSprite*, m_pIconElixir);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "IconGem", CCSprite*, m_pIconGem);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "IconLevel", CCSprite*, m_pIconLevel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ExpProgress", CCSprite*, m_pExpProgress);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GoldFullTip", CCScale9Sprite*, m_pGoldFullTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "OilFullTip", CCScale9Sprite*, m_pOilFullTip);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "HeroLevel", CCLabelBMFont*, m_pHeroLevel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "HeroHPTime", CCLabelBMFont*, m_pHeroHPTime);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "HeroHpProgress", CCScale9Sprite*, m_pHeroHpProgress);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "HeroExpProgress", CCScale9Sprite*, m_pHeroExpProgress);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "HeroExp", CCLabelBMFont*, m_pHeroExp);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "AdLoadingTip", CCLabelBMFont*, m_pAdsLoadingTip);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnShop", CCControlButton*, m_pBtnShop);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnAttack", CCControlButton*, m_pBtnAttack);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnLeague", CCControlButton*, m_pBtnLeague);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnInfo", CCControlButton*, m_pBtnInfo);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnTroops", CCControlButton*, m_pBtnTroops);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnBuild", CCControlButton*, m_pBtnBuild);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnUpgrade", CCControlButton*, m_pBtnUpgrade);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnCancel", CCControlButton*, m_pBtnCancel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnFinish", CCControlButton*, m_pBtnFast);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnResearch", CCControlButton*, m_pBtnResearch);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnRemoveByGold", CCControlButton*, m_pBtnRemoveByGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnRemoveByElixir", CCControlButton*, m_pBtnRemoveByElixir);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnMail", CCControlButton*, m_pBtnMail);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnAchievement", CCControlButton*, m_pBtnAchievement);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnFreeBonus", CCControlButton*, m_pBtnFreeBonus);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnAddWorker", CCControlButton*, m_pBtnAddWorker);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnRanking", CCControlButton*, m_pBtnRanking);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnAddShield", CCControlButton*, m_pBtnAddShield);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnAddGold", CCControlButton*, m_pBtnAddGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnAddElixir", CCControlButton*, m_pBtnAddElixir);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnAddGem", CCControlButton*, m_pBtnAddGem);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnHeroHeal", CCControlButton*, m_pBtnHeroHeal);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnSetting", CCControlButton*, m_pBtnSetting);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnClan", CCControlButton*, m_pBtnClan);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnMilitaryRank", CCControlButton*, m_pBtnMilitaryRank);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnAnnouncement", CCControlButton*, m_pBtnAnnouncement);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnSell", CCControlButton*, m_pBtnSell);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnUpgradeRow", CCControlButton*, m_pBtnUpgradeRow);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnRotate", CCControlButton*, m_pBtnRotate);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnRow", CCControlButton*, m_pBtnRow);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnClanTroopsRequest", CCControlButton*, m_pBtnClanTroopsRequest);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnClanTroopsFull", CCControlButton*, m_pBtnClanTroopsFull);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnChat", CCControlButton*, m_pBtnChat);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnSpin", CCControlButton*, m_pBtnSpin);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnTrain", CCControlButton*, m_pBtnTrain);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnTrainSkill", CCControlButton*, m_pBtnTrainSkill);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "InfoNode", CCNode*, m_pInfoNode);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnFBShare", CCControlButton*, m_pBtnFBShare);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanWarBtn", CCControlButton*, m_pBtnClanWar);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnEdit", CCControlButton*, m_pBtnEdit);
    
    return true;
}

SEL_CCControlHandler MainUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuItemCallback", MainUI::menuItemCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuAttackCallback", MainUI::menuAttackCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuShopCallback", MainUI::menuShopCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuShieldCallback", MainUI::menuShieldCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuWorkerCallback", MainUI::menuWorkerCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuTaskCallback", MainUI::menuTaskCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuRankingCallback", MainUI::menuRankingCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuInfoCallback", MainUI::menuInfoCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuTroopsCallback", MainUI::menuTroopsCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBuildCallback", MainUI::menuBuildCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuUpgradeCallback", MainUI::menuUpgradeCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCancelCallback", MainUI::menuCancelCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuFinishCallback", MainUI::menuFastCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuResearchCallback", MainUI::menuResearchCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuRemoveCallback", MainUI::menuRemoveCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuClanCallback", MainUI::menuClanCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuHelpClanCallback", MainUI::menuHelpClanCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuAddGoldCallback", MainUI::menuAddGoldCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuAddElixirCallback", MainUI::menuAddElixirCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuAddGemCallback", MainUI::menuAddGemCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuMailCallback", MainUI::menuMailCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBonusCallback", MainUI::menuBonusCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuHeroCallback", MainUI::menuHeroCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSettingCallback", MainUI::menuSettingCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuChatCallback", MainUI::menuChatCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuHeroHealCallback", MainUI::menuHeroHealCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuTrainCallback", MainUI::menuTrainCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSpinCallback", MainUI::menuSpinCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuClanWarCallback", MainUI::menuClanWarCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "onSelectRotate", MainUI::onSelectRotate);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "onSelectRow", MainUI::onSelectRow);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuGoCallback", MainUI::menuGoCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", MainUI::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuMilitaryRankCallback", MainUI::menuMilitaryRankCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuLeagueCallback", MainUI::menuLeagueCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuLeagueStartUIColseCallback", MainUI::menuLeagueStartUIColseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuGoLeagueCallback", MainUI::menuGoLeagueCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuGetAwardCallback", MainUI::menuGetAwardCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuAnnouncementCallback", MainUI::menuAnnouncementCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBtnFBShareCallback", MainUI::menuBtnFBShareCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuUpgradeRowCallback", MainUI::menuUpgradeRowCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuEditCallback", MainUI::menuEditCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuTrainSkillCallback", MainUI::menuTrainSkillCallback);

    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuAdLoadingCloseCallback", MainUI::menuAdLoadingCloseCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuClanWarStartColseCallback", MainUI::menuClanWarStartColseCallback);

    return NULL;
}
