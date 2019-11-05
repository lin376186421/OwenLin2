#include "GameWorldScene.h"
#include "GameLayer.h"
#include "coc/ImgMgr.h"
#include "form/MainUI.h"
#include "form/BuildUI.h"
#include "coc/PlayerScene.h"
#include "coc/LogicInfo.h"
#include "coc/Building.h"
#include "form/FightUI.h"
#include "coc/HttpClient.h"
#include "coc/GuideMgr.h"
#include "coc/HeroMgr.h"
#include "coc/ClanMgr.h"
#include "coc/SocketClient.h"

using namespace cocos2d;

GameWorld::GameWorld()
	: m_pShop(NULL)
	, m_pGameLayer(NULL)
{
    m_pMainUI           = NULL;
    m_pBuildUI          = NULL;
	m_pTypeShop         = NULL;
    m_pShop             = NULL;
    m_pBarrackUI        = NULL;
    m_pFindMatchUI      = NULL;
    m_pMailUI           = NULL;
    m_pFightUI          = NULL;
    m_pInfoUI           = NULL;
    m_pUpLevelUI        = NULL;
    m_pScoreUI          = NULL;
    m_pReplayUI         = NULL;
    m_pAchievementUI    = NULL;
    m_pCreateUI         = NULL;
    m_pRankingUI        = NULL;
    m_pHeroUI           = NULL;
    m_pSoldierUI        = NULL;
    m_pPlayerUpLevelUI  = NULL;
    m_pLabUI            = NULL;
    m_pVisitUI          = NULL;
    m_pGuideUI          = NULL;
    m_pChgScene         = NULL;
    m_pNotifyUI         = NULL;
    m_pRaidNotifyUI     = NULL;
    m_pBonusUI          = NULL;
    m_pSkillGlyphUI     = NULL;
    m_pSkillUI          = NULL;
    m_pSettingUI        = NULL;
    m_pClanUI           = NULL;
    m_pClanInfoUI       = NULL;
    m_pClanEditUI       = NULL;
    m_pSupportUI        = NULL;
    m_pSpinUI           = NULL;
    m_pDailyBonusUI     = NULL;
    m_pSaleLogoUI       = NULL;
    m_pChatUI           = NULL;
    m_pGiftUI           = NULL;
    m_pMilitaryRankUI   = NULL;
    m_pMilitaryUpgradeUI= NULL;
    m_pLeagueUI         = NULL;
    m_pAnnouncementUI   = NULL;
    m_pClanWarMapUI     = NULL;
    m_pClanWarScoreUI   = NULL;
    m_pGroupWarScoreUI  = NULL;
    m_pFBLoginUI        = NULL;
    m_pFBInviteUI       = NULL;
    m_pMainEditUI       = NULL;
    m_pFBShareUI        = NULL;
}

GameWorld::~GameWorld()
{
}

// on "init" you need to initialize your instance
bool GameWorld::init()
{
    bool bRet = false;
    do 
    {
        //////////////////////////////////////////////////////////////////////////
        // super init first
        //////////////////////////////////////////////////////////////////////////

        CC_BREAK_IF(! CCLayer::init());
        
        this->setKeypadEnabled(true);

        PlayerScene::getSingleton().m_pGameWorld = this;
        
        showMainUI();
        showNotifyUI();
        showBuildUI();
        
        GuideMgr::getSingleton().init();
        LogicInfo::getSingleton().countMilitaryRank();
        
        if (GuideMgr::getSingleton().isGuideEnd()) {
            if (LogicInfo::getSingleton().m_iapGiftInfo.corflag == 0 || LogicInfo::getSingleton().m_iapGiftInfo.corflag == 2)
            {
                if(LogicInfo::getSingleton().m_iapGiftInfo.corflag == 2 && LogicInfo::getSingleton().m_iapGiftInfo.gotflag == 1)
                {
                    //当天已领取
                }
                else
                {
                    PlayerScene::getSingleton().m_pGameWorld->showGiftUI();
                }
            }
            else if(LogicInfo::getSingleton().m_signinDays >= 0)
            {
                if(LogicInfo::getSingleton().m_isSigninDays == 1)
                {
                    PlayerScene::getSingleton().m_pGameWorld->showDailyBonusUI();
                }
            }
        }
        
        if (LogicInfo::getSingleton().m_isReviewing || LogicInfo::getSingleton().m_iAdsOpen)
        {
            scheduleOnce(schedule_selector(GameWorld::showFullScreenAds), 0.5f);
//            showFullScreenAds();
        }
        
        if(FacebookMgr::getSingleton().m_isTipLogin && FacebookMgr::getSingleton().isOpenState() && FacebookMgr::getSingleton().isLogin() == false)
        {
            showFBLoginUI(0);
            FacebookMgr::getSingleton().m_isTipLogin = false;
            CCUserDefault::sharedUserDefault()->setBoolForKey("FB_LOGIN_TIP", true);
            CCUserDefault::sharedUserDefault()->flush();
        }
        
        CAppPlatform::sharedAppPlatform()->requestTapjoyConnect();
        CAppPlatform::sharedAppPlatform()->getAarkiPoint();
        CAppPlatform::sharedAppPlatform()->getTapJopPoint();
        CAppPlatform::sharedAppPlatform()->getSSAPoint();
        CAppPlatform::sharedAppPlatform()->getNativeXPoint();
        
        LogicInfo::getSingleton().checkUserCenterBind();
        
        HttpClient::getSingleton().updUserInfo();
        
        bRet = true;
    } while (0);
    
    return bRet;
}

void GameWorld::showFullScreenAds()
{
    if(LogicInfo::getSingleton().m_defaultFullAdsType == 1)
    {
        CAppPlatform::sharedAppPlatform()->showNativeXOfferAds(NATIVE_X_GAMELAUNCH);
    }
    else if(LogicInfo::getSingleton().m_defaultFullAdsType == 0)
    {
        CAppPlatform::sharedAppPlatform()->showChartBoostAds();
    }
}

void GameWorld::show()
{
	CCScene *scene = CCScene::create();
    
	GameLayer* pLayerGame = GameLayer::create();
	scene->addChild(pLayerGame);
    
    GameWorld *layer = GameWorld::create();
	scene->addChild(layer);
    
    layer->m_pGameLayer = pLayerGame;

	CCScene* pScene = CCDirector::sharedDirector()->getRunningScene();
	if(!pScene)
		CCDirector::sharedDirector()->runWithScene(scene);
	else
	{
		//CCDirector::sharedDirector()->pushScene(scene);
		//CCDirector::sharedDirector()->runWithScene(scene);
		CCDirector::sharedDirector()->replaceScene(scene);
	}
    
    PlayerScene::getSingleton().m_pRoot->setVisible(true);
}

void GameWorld::showShop(int shopid)
{
	std::vector<int> lst = LogicInfo::getSingleton().m_shoplst[shopid - 1];
    
    if (m_pTypeShop == NULL) {
        showTypeShop();
    }
    
    if(m_pShop == NULL)
	{
        ImgMgr::getSingleton().addSpriteFrames("shop");
        
        LogicInfo::getSingleton().m_shopid = shopid;
		m_pShop = Shop::create(shopid);

		addChild(m_pShop, 1);
	}
}

void GameWorld::hideShop()
{
	removeChild(m_pShop, true);
    m_pShop = NULL;
    
    if(m_pTypeShop == NULL)
    {
        ImgMgr::getSingleton().removeRes("shop");
    }
}

void GameWorld::showTypeShop()
{
    if(m_pTypeShop != NULL)
        return;

	ImgMgr::getSingleton().addSpriteFrames("shop");
        
	m_pTypeShop = TypeShop::create();
	addChild(m_pTypeShop, 1);
}

void GameWorld::hideTypeShop()
{
	removeChild(m_pTypeShop, true);
    
    m_pTypeShop = NULL;

	if(m_pShop == NULL)
    {
        ImgMgr::getSingleton().removeRes("shop");
    }
}

void GameWorld::showBarrackUI()
{
    if(m_pBarrackUI != NULL)
        return;
    
	if(PlayerScene::getSingleton().m_pCurBuilding != NULL && PlayerScene::getSingleton().m_pCurBuilding->m_pBuildingInfo != NULL && 
		PlayerScene::getSingleton().m_pCurBuilding->m_pBuildingInfo->barrackid >= 0)
	{
		LogicInfo::getSingleton().onOpenBarrack();
	}

	m_pBarrackUI = BarrackUI::create();

	addChild(m_pBarrackUI, 1);
}

void GameWorld::hideBarrackUI()
{
    if (m_pBarrackUI) {
        
        if(PlayerScene::getSingleton().m_pCurBuilding != NULL && PlayerScene::getSingleton().m_pCurBuilding->m_pBuildingInfo != NULL &&
           PlayerScene::getSingleton().m_pCurBuilding->m_pBuildingInfo->barrackid >= 0)
        {
            LogicInfo::getSingleton().onCloseBarrack();
        }
        
        removeChild(m_pBarrackUI, true);
        
        m_pBarrackUI = NULL;
        
        //    ImgMgr::getSingleton().removeRes("barrack.plist", "barrack.pvr.ccz");
        
        if(PlayerScene::getSingleton().m_pCurBuilding != NULL)
            PlayerScene::getSingleton().selectBuilding(*PlayerScene::getSingleton().m_pCurBuilding);
    }
}

void GameWorld::showFindMatchUI()
{
    if(m_pFindMatchUI != NULL)
        return;
    
    ImgMgr::getSingleton().loadImg("wars_bg");
    ImgMgr::getSingleton().addSpriteFrames("wars_map");
    
    m_pFindMatchUI = FindMatchUI::create();
	addChild(m_pFindMatchUI, 1);
    
    PlayerScene::getSingleton().m_pRoot->setVisible(false);
}

void GameWorld::hideFindMatchUI()
{
    removeChild(m_pFindMatchUI, true);
    
    m_pFindMatchUI = NULL;
    
    ImgMgr::getSingleton().removeRes("wars_bg");
    ImgMgr::getSingleton().removeRes("wars_map");
    
    PlayerScene::getSingleton().m_pRoot->setVisible(true);
}

void GameWorld::showScoreUI()
{
    if(m_pScoreUI != NULL)
        return;
        
    ImgMgr::getSingleton().addSpriteFrames("hero_head");
    
    int type = LogicInfo::getSingleton().m_curState;
    
    m_pScoreUI = ScoreUI::create(type);
	addChild(m_pScoreUI, 1);
}

void GameWorld::hideScoreUI()
{

    ImgMgr::getSingleton().removeRes("hero_head");

    removeChild(m_pScoreUI, true);
    
    m_pScoreUI = NULL;
}

void GameWorld::showAchievementUI()
{
    if(m_pAchievementUI != NULL)
        return;
    
    m_pAchievementUI = AchievementUI::create();
	addChild(m_pAchievementUI, 1);
}

void GameWorld::hideAchievementUI()
{
    removeChild(m_pAchievementUI, true);
    
    m_pAchievementUI = NULL;
}

void GameWorld::showInfoUI()
{
    if(m_pInfoUI != NULL)
        return;
    
    m_pInfoUI = InfoUI::create();
	addChild(m_pInfoUI, 1);
}

void GameWorld::hideInfoUI()
{
    removeChild(m_pInfoUI, true);
    
    m_pInfoUI = NULL;
}

void GameWorld::showUpLevelUI()
{
    if(m_pUpLevelUI != NULL)
        return;
    
    m_pUpLevelUI = UpLevelUI::create();
    
	addChild(m_pUpLevelUI, 1);
}

void GameWorld::hideUpLevelUI()
{
    removeChild(m_pUpLevelUI, true);
    
    m_pUpLevelUI = NULL;
}

void GameWorld::showFightUI()
{
    if(m_pFightUI != NULL)
        return;
    
    ImgMgr::getSingleton().addSpriteFrames("hero_skill");
    
    m_pFightUI = FightUI::create();
	m_pFightUI->setPosition(ccp(0.0f, 0.0f));
    
	addChild(m_pFightUI, 1);
}

void GameWorld::hideFightUI()
{
    ImgMgr::getSingleton().removeRes("hero_skill");
    
    if(m_pFightUI != NULL)
	{
		LogicInfo::getSingleton().removeListener(m_pFightUI);
     
		removeChild(m_pFightUI, true);
        
        m_pFightUI = NULL;
	}
}

void GameWorld::showRankingUI()
{
    if(m_pRankingUI != NULL)
        return;
    
    ImgMgr::getSingleton().addSpriteFrames("game_title");

    m_pRankingUI = RankingUI::create();
	addChild(m_pRankingUI, 1);
}

void GameWorld::hideRankingUI()
{
    ImgMgr::getSingleton().removeRes("game_title");

    removeChild(m_pRankingUI, true);
    
    m_pRankingUI = NULL;
}

void GameWorld::showSkillGlyphUI()
{
    if(m_pSkillGlyphUI != NULL)
        return;
    
    ImgMgr::getSingleton().addSpriteFrames("hero_skill");
    
    m_pSkillGlyphUI = SkillGlyphUI::create();
	addChild(m_pSkillGlyphUI, 1);
}

void GameWorld::hideSkillGlyphUI()
{
    ImgMgr::getSingleton().removeRes("hero_skill");
    
    removeChild(m_pSkillGlyphUI, true);
    m_pSkillGlyphUI = NULL;
}

void GameWorld::showSkillUI()
{
    if(m_pSkillUI != NULL)
        return;
    
    ImgMgr::getSingleton().addSpriteFrames("hero_skill");
    
    m_pSkillUI = SkillUI::create();
    
	addChild(m_pSkillUI, 1);
}

void GameWorld::hideSkillUI()
{
    ImgMgr::getSingleton().removeRes("hero_skill");
    
    removeChild(m_pSkillUI, true);
    
    m_pSkillUI = NULL;
}

void GameWorld::showSettingUI()
{
    if (m_pSettingUI == NULL)
    {
        m_pSettingUI = SettingUI::create();
        addChild(m_pSettingUI, 1);
    }
}

void GameWorld::hideSettingUI()
{
    removeChild(m_pSettingUI, true);
    m_pSettingUI = NULL;
}

void GameWorld::showChatUI()
{
    if (m_pChatUI == NULL) {
        
        m_pChatUI = ChatUI::create();
        this->addChild(m_pChatUI, 1);
    }
}

void GameWorld::hideChatUI()
{
    removeChild(m_pChatUI, true);
    m_pChatUI = NULL;
}

void GameWorld::showSupportUI()
{
    if (m_pSupportUI == NULL) {
        m_pSupportUI = SupportUI::create();
        addChild(m_pSupportUI, 1);
    }
}

void GameWorld::hideSupportUI()
{
    removeChild(m_pSupportUI, true);
    m_pSupportUI = NULL;
}

void GameWorld::showClanWarMapUI()
{
    ImgMgr::getSingleton().addSpriteFrames("national_flag");
    ImgMgr::getSingleton().addSpriteFrames("clanwar_map");
    
    if (m_pClanWarMapUI == NULL) {
        m_pClanWarMapUI = ClanWarMapUI::create();
        addChild(m_pClanWarMapUI, 1);
    }
        
    PlayerScene::getSingleton().m_pRoot->setVisible(false);
}

void GameWorld::hideClanWarMapUI()
{
    removeChild(m_pClanWarMapUI);
    m_pClanWarMapUI = NULL;
    
    ImgMgr::getSingleton().removeRes("national_flag");
    ImgMgr::getSingleton().removeRes("clanwar_map");
    
    PlayerScene::getSingleton().m_pRoot->setVisible(true);
}

void GameWorld::showClanWarScoreUI()
{
    ImgMgr::getSingleton().addSpriteFrames("clanwar_map");
    
    if (m_pClanWarScoreUI == NULL) {
        m_pClanWarScoreUI = ClanWarScoreUI::create();
        addChild(m_pClanWarScoreUI, 1);
    }
}

void GameWorld::hideClanWarScoreUI()
{
    if (m_pClanWarScoreUI) {
        removeChild(m_pClanWarScoreUI);
        m_pClanWarScoreUI = NULL;
    }
    
    if (m_pClanWarMapUI == NULL) {
         ImgMgr::getSingleton().removeRes("clanwar_map");
    }
}

void GameWorld::showGroupWarScoreUI()
{
    ImgMgr::getSingleton().addSpriteFrames("clanwar_map");
    
    if (m_pGroupWarScoreUI == NULL) {
        m_pGroupWarScoreUI = GroupWarScoreUI::create();
        addChild(m_pGroupWarScoreUI, 1);
    }
}

void GameWorld::hideGroupWarScoreUI()
{
    if (m_pGroupWarScoreUI) {
        removeChild(m_pGroupWarScoreUI);
        m_pGroupWarScoreUI = NULL;
    }
    
    if (m_pClanWarMapUI == NULL) {
        ImgMgr::getSingleton().removeRes("clanwar_map");
    }
}

void GameWorld::showAnnouncementUI()
{
    if (m_pAnnouncementUI == NULL) {
        m_pAnnouncementUI = AnnouncementUI::create();
        m_pAnnouncementUI->showAnnouncementWebView();
        addChild(m_pAnnouncementUI, 1);
    }
}

void GameWorld::hideAnnouncementUI()
{
    removeChild(m_pAnnouncementUI, true);
    m_pAnnouncementUI = NULL;
}

void GameWorld::showSpinUI()
{
    if (m_pSpinUI == NULL)
    {
        ImgMgr::getSingleton().addSpriteFrames("spin");
        m_pSpinUI = SpinUI::create();
        addChild(m_pSpinUI, 1);
    }
}

void GameWorld::hideSpinUI()
{
    removeChild(m_pSpinUI, true);
    m_pSpinUI = NULL;
    
    ImgMgr::getSingleton().removeRes("spin");

}

void GameWorld::showDailyBonusUI()
{
    if (m_pDailyBonusUI == NULL)
    {
        m_pDailyBonusUI = DailyBonusUI::create();
        addChild(m_pDailyBonusUI, 1);
    }
}

void GameWorld::hideDailyBonusUI()
{
    removeChild(m_pDailyBonusUI, true);
    m_pDailyBonusUI = NULL;
}

void GameWorld::showCreateUI()
{
    if(m_pCreateUI != NULL)
        return;

    ImgMgr::getSingleton().addSpriteFrames("hero_head");
    
    m_pCreateUI = CreateUI::create();
	m_pCreateUI->setPosition(ccp(0.0f, 0.0f));
    
    addChild(m_pCreateUI, 1);
}

void GameWorld::hideCreateUI()
{
    removeChild(m_pCreateUI, true);
 
    m_pCreateUI = NULL;

	ImgMgr::getSingleton().removeRes("hero_head");
}

void GameWorld::showMailUI()
{
    if(m_pMailUI != NULL)
        return;
    
    m_pMailUI = MailUI::create();
	addChild(m_pMailUI, 1);
}

void GameWorld::hideMailUI()
{
    removeChild(m_pMailUI, true);
    
    m_pMailUI = NULL;
}

void GameWorld::showMainUI()
{
    if(m_pMainUI == NULL) {
     	m_pMainUI = MainUI::create();
        addChild(m_pMainUI, 1);
        LogicInfo::getSingleton().countMilitaryRank();
    }
}

void GameWorld::hideMainUI()
{
	if(m_pMainUI != NULL)
	{
		PlayerScene::getSingleton().removeSelectBuildingListener(m_pMainUI);
		LogicInfo::getSingleton().removeListener(m_pMainUI);

		removeChild(m_pMainUI, true);
        
        m_pMainUI = NULL;
	}
    
    if (m_pMailUI) {
        hideMailUI();
    }
    
    if (m_pTypeShop) {
        hideTypeShop();
    }
    
    if (m_pShop) {
        hideShop();
    }
    
    if (m_pAchievementUI) {
        hideAchievementUI();
    }
    
    if (m_pBarrackUI) {
        hideBarrackUI();
    }
    
    if (m_pBonusUI) {
        hideBonusUI();
    }
    
    if (m_pChatUI) {
        hideChatUI();
    }
    
    if (m_pLeagueUI) {
        hideLeagueUI();
    }
    
    if (m_pMilitaryRankUI) {
        hideMilitaryRankUI();
    }
    
    if (m_pSettingUI) {
        hideSettingUI();
    }
    
    if (m_pInfoUI) {
        hideInfoUI();
    }
    
    if (m_pUpLevelUI) {
        hideUpLevelUI();
    }
    
    if (m_pFBLoginUI) {
        hideFBLoginUI();
    }
    
    if (m_pDailyBonusUI) {
        hideDailyBonusUI();
    }
}

void GameWorld::showBuildUI()
{
    if (m_pBuildUI == NULL) {
        m_pBuildUI = BuildUI::create();
        m_pBuildUI->setAnchorPoint(ccp(0, 0));
        addChild(m_pBuildUI, 1);
    }
}

void GameWorld::showEditUI()
{
    if(m_pMainEditUI != NULL)
        return;
    
    m_pMainEditUI = MainEditUI::create();
    
    addChild(m_pMainEditUI);
    
    if (m_pMainUI) {
        hideMainUI();
    }
    
    PlayerScene::getSingleton().setIsEditMode(true);
}

void GameWorld::hideEditUI()
{
    PlayerScene::getSingleton().setIsEditMode(false);

    PlayerScene::getSingleton().removeSelectBuildingListener(m_pMainEditUI);
    
    removeChild(m_pMainEditUI, true);
    
    m_pMainEditUI = NULL;
    
    showMainUI();
}

void GameWorld::showReplayUI()
{
    if(m_pReplayUI != NULL)
        return;
        
    m_pReplayUI = ReplayUI::create();
    
	addChild(m_pReplayUI, 1);
}

void GameWorld::hideReplayUI()
{
    removeChild(m_pReplayUI, true);
    
    m_pReplayUI = NULL;

	ImgMgr::getSingleton().removeRes("replay");

}

void GameWorld::showHeroUI()
{
    if (GuideMgr::getSingleton().val1 < GUIDE_HERO_SKILL) {
        return;
    }
    
    if(m_pHeroUI != NULL)
        return;
	
    ImgMgr::getSingleton().addSpriteFrames("hero_head");
    ImgMgr::getSingleton().addSpriteFrames("hero_skill");
    
    m_pHeroUI = HeroUI::create();
    
	addChild(m_pHeroUI, 1);
}

void GameWorld::hideHeroUI()
{
    removeChild(m_pHeroUI, true);
    
    m_pHeroUI = NULL;

    ImgMgr::getSingleton().removeRes("hero_head");
    ImgMgr::getSingleton().removeRes("hero_skill");
}

void GameWorld::showSoldierUI()
{
    if(m_pSoldierUI != NULL)
        return;
	
	ImgMgr::getSingleton().addSpriteFrames("soldierui");
    
    m_pSoldierUI = SoldierUI::create();
    
	addChild(m_pSoldierUI, 1);
}

void GameWorld::hideSoldierUI()
{
    removeChild(m_pSoldierUI, true);
    
    m_pSoldierUI = NULL;

	ImgMgr::getSingleton().removeRes("soldierui");
}

void GameWorld::showPlayerLevelUpUI()
{
    if(m_pPlayerUpLevelUI != NULL)
        return;
    ImgMgr::getSingleton().addSpriteFrames("hero_head");
    m_pPlayerUpLevelUI = PlayerUpLevelUI::create();
    
	addChild(m_pPlayerUpLevelUI, 1);
}

void GameWorld::hidePlayerLevelUpUI()
{
    if (m_pLabUI) {
        m_pLabUI->setVisible(true);
    }
    ImgMgr::getSingleton().removeRes("hero_head");
    removeChild(m_pPlayerUpLevelUI, true);
    
    m_pPlayerUpLevelUI = NULL;
}

void GameWorld::showClanUI()
{
    if(m_pClanUI != NULL)
        return;
    
    m_pClanUI = ClanUI::create();
    
	addChild(m_pClanUI, 1);
}

void GameWorld::hideClanUI()
{
    if (m_pClanUI) {
        removeChild(m_pClanUI, true);
        m_pClanUI = NULL;
    }
    
    hideClanInfoUI();
}

void GameWorld::showClanInfoUI()
{
    if(m_pClanInfoUI == NULL)
    {
        m_pClanInfoUI = ClanInfoUI::create(ClanMgr::getSingleton().m_otherclan);
        addChild(m_pClanInfoUI, 1);
    }
}

void GameWorld::hideClanInfoUI()
{
    if (m_pClanInfoUI) {
        removeChild(m_pClanInfoUI, true);
        m_pClanInfoUI = NULL;
    }
}

void GameWorld::showClanEditUI()
{
    if (m_pClanEditUI == NULL) {
        m_pClanEditUI = ClanEditUI::create();
        addChild(m_pClanEditUI, 1);
    }
}

void GameWorld::hideClanEditUI()
{
    if (m_pClanEditUI) {
        removeChild(m_pClanEditUI);
        m_pClanEditUI = NULL;
    }
}

void GameWorld::showLabUI()
{
    if(m_pLabUI != NULL)
        return;
    
    ImgMgr::getSingleton().addSpriteFrames("soldierui");
    
    m_pLabUI = LabUI::create();
    
	addChild(m_pLabUI, 1);
}

void GameWorld::hideLabUI()
{
    ImgMgr::getSingleton().removeRes("soldierui");
 
    if (m_pLabUI) {
        removeChild(m_pLabUI, true);
        m_pLabUI = NULL;
    }
}

void GameWorld::showFBShareUI()
{
    if(m_pFBShareUI != NULL)
        return;
    ImgMgr::getSingleton().loadImg("ui_reward_npc");
    m_pFBShareUI = FBShareUI::create();
    addChild(m_pFBShareUI, 1);
}
void GameWorld::hideFBShareUI()
{
    
    if(m_pFBShareUI)
    {
        removeChild(m_pFBShareUI);
        m_pFBShareUI = NULL;
        ImgMgr::getSingleton().removeRes("ui_reward_npc");
    }
    
}

void GameWorld::showVisitUI()
{
    if(m_pVisitUI != NULL)
        return;
    
    m_pVisitUI = VisitUI::create();
	m_pVisitUI->setPosition(ccp(0.0f, 0.0f));
    
	addChild(m_pVisitUI, 1);
}

void GameWorld::hideVisitUI()
{
    PlayerScene::getSingleton().removeSelectBuildingListener(m_pVisitUI);
    
    if (m_pVisitUI) {
        removeChild(m_pVisitUI, true);
        m_pVisitUI = NULL;
    }
}

void GameWorld::showGuideUI()
{
    if(m_pGuideUI != NULL){
        GuideMgr::getSingleton().m_pActiveBtn = m_pGuideUI->m_pBtnOK;
        return;
    }
    
    m_pGuideUI = GuideUI::create();
	m_pGuideUI->setPosition(ccp(0.0f, 0.0f));
    
	addChild(m_pGuideUI, 1);
}

void GameWorld::hideGuideUI()
{
    if (m_pGuideUI) {
        removeChild(m_pGuideUI, true);
        m_pGuideUI = NULL;
    }
}

void GameWorld::showNotifyUI()
{
    if(m_pNotifyUI != NULL)
        return;
    
    m_pNotifyUI = NotifyUI::create();
	addChild(m_pNotifyUI, 10);
}

void GameWorld::hideNotifyUI()
{
    if (m_pNotifyUI) {
        removeChild(m_pNotifyUI, true);
        m_pNotifyUI = NULL;
    }
}

void GameWorld::showRaidNotifyUI()
{
    if (!LogicInfo::getSingleton().isNormalMode()) {
        return;
    }
    
    if(m_pRaidNotifyUI != NULL)
        return;
    
    ImgMgr::getSingleton().addSpriteFrames("ui_attacked_npc");
    
    m_pRaidNotifyUI = RaidNotifyUI::create();
    
	addChild(m_pRaidNotifyUI, 1);
    
    if(m_pGiftUI != NULL) {
        m_pRaidNotifyUI->setVisible(false);
    }
}

void GameWorld::hideRaidNotifyUI()
{
    ImgMgr::getSingleton().removeRes("ui_attacked_npc");
    
    if (m_pRaidNotifyUI) {
        removeChild(m_pRaidNotifyUI, true);
        m_pRaidNotifyUI = NULL;
    }
}

void GameWorld::showBonusUI()
{
    if(m_pBonusUI != NULL)
        return;
    
    m_pBonusUI = BonusUI::create();
    
	addChild(m_pBonusUI, 1);
}

void GameWorld::hideBonusUI()
{
    if (m_pBonusUI) {
        removeChild(m_pBonusUI, true);
        m_pBonusUI = NULL;
    }
}

void GameWorld::showSaleLogoUI()
{
    if(m_pSaleLogoUI != NULL)
        return;
    
    ImgMgr::getSingleton().addSpriteFrames("shop_sale_item");
    m_pSaleLogoUI = SaleLogoUI::create();
    addChild(m_pSaleLogoUI, 1);
}

void GameWorld::hideSaleLogoUI()
{
    if(m_pSaleLogoUI != NULL)
    {
        removeChild(m_pSaleLogoUI, true);
        m_pSaleLogoUI = NULL;
        ImgMgr::getSingleton().removeRes("shop_sale_item");
    }
}

void GameWorld::showGiftUI()
{
    if(m_pGiftUI == NULL)
    {
        ImgMgr::getSingleton().loadImg("ui_reward_npc");
        ImgMgr::getSingleton().addSpriteFrames("reward");
        m_pGiftUI = GiftUI::create();
        addChild(m_pGiftUI, 1);
    }
}

void GameWorld::hideGiftUI()
{
    if(m_pGiftUI != NULL)
    {
        removeChild(m_pGiftUI, true);
        m_pGiftUI = NULL;
        ImgMgr::getSingleton().removeRes("ui_reward_npc");
        ImgMgr::getSingleton().removeRes("reward");
    }
}

void GameWorld::showFBLoginUI(int state)
{
    if(m_pFBLoginUI == NULL)
    {
        ImgMgr::getSingleton().addSpriteFrames("ui_friends");
        m_pFBLoginUI = FBLoginUI::create(state);
        addChild(m_pFBLoginUI, 1);
    }
}

void GameWorld::hideFBLoginUI()
{
    if(m_pFBLoginUI != NULL)
    {
        removeChild(m_pFBLoginUI, true);
        m_pFBLoginUI = NULL;
        ImgMgr::getSingleton().removeRes("ui_friends");
    }
}


void GameWorld::showFBInviteUI()
{
    if(m_pFBInviteUI == NULL)
    {
        m_pFBInviteUI = FBInviteUI::create();
        addChild(m_pFBInviteUI, 1);
    }
}

void GameWorld::hideFBInviteUI()
{
    if(m_pFBInviteUI != NULL)
    {
        removeChild(m_pFBInviteUI, true);
        m_pFBInviteUI = NULL;
    }
}

void GameWorld::showMilitaryRankUI()
{
    if(m_pMilitaryRankUI == NULL)
    {
        ImgMgr::getSingleton().loadImg("military_rank_people");
        m_pMilitaryRankUI = MilitaryRankUI::create();
        addChild(m_pMilitaryRankUI, 1);
    }
}

void GameWorld::hideMilitaryRankUI()
{
    if(m_pMilitaryRankUI != NULL)
    {
        removeChild(m_pMilitaryRankUI, true);
        m_pMilitaryRankUI = NULL;
        ImgMgr::getSingleton().removeRes("military_rank_people");
    }
}

void GameWorld::showMilitaryUpgradeUI()
{
    if(m_pMilitaryUpgradeUI == NULL)
    {
        m_pMilitaryUpgradeUI = MilitaryUpgradeUI::create();
        addChild(m_pMilitaryUpgradeUI, 1);
    }
}

void GameWorld::hideMilitaryUpgradeUI()
{
    if(m_pMilitaryUpgradeUI != NULL)
    {
        removeChild(m_pMilitaryUpgradeUI, true);
        m_pMilitaryUpgradeUI = NULL;
    }
}

void GameWorld::showLeagueUI()
{
    if(m_pLeagueUI != NULL)
        return;
    
    ImgMgr::getSingleton().addSpriteFrames("game_title");
    ImgMgr::getSingleton().loadImg("ui_attacked_npc");

    m_pLeagueUI = LeagueUI::create();
	addChild(m_pLeagueUI, 1);
}

void GameWorld::hideLeagueUI()
{
    ImgMgr::getSingleton().removeRes("game_title");
    ImgMgr::getSingleton().removeRes("ui_attacked_npc");

    removeChild(m_pLeagueUI, true);
    
    m_pLeagueUI = NULL;
}

void GameWorld::showChgScene(int mode)
{
	if(m_pChgScene != NULL)
		return;

    ImgMgr::getSingleton().loadImg("wars_bg");
    ImgMgr::getSingleton().addSpriteFrames("wars_map");
    
    m_pChgScene = ChgScene::create(mode);
	m_pChgScene->m_pDelegate = this;
	m_pChgScene->m_funcRelease = callfunc_selector(GameWorld::onReleaseChgScene);

	m_pChgScene->setPosition(ccp(0.0f, 0.0f));
	m_pChgScene->retain();
    
	addChild(m_pChgScene, 1);

	if(m_pGameLayer != NULL)
		m_pGameLayer->onChgScene();
}

void GameWorld::hideChgScene()
{
	if(m_pChgScene != NULL)
	{
		removeChild(m_pChgScene, true);

		HttpClient::getSingleton().removeListener(m_pChgScene);

        ImgMgr::getSingleton().removeRes("wars_bg");
        ImgMgr::getSingleton().removeRes("wars_map");
	}
    
    m_pGameLayer->clearTouch();
}

void GameWorld::onReleaseChgScene()
{
	if(m_pChgScene != NULL)
	{
		m_pChgScene->release();
		m_pChgScene = NULL;
	}
}

void GameWorld::keyBackClicked()
{    
    if (!LogicInfo::getSingleton().m_isExitMsg)
    {
         MessageUI::show(MSG_EXIT, NULL, NULL);
    }
}

void GameWorld::keyMenuClicked()
{
//    CCDirector::sharedDirector()->end();
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//    exit(0);
//#endif
}
