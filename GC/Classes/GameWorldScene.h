#ifndef __GAMEWORLD_SCENE_H__
#define __GAMEWORLD_SCENE_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "coc/ImgMgr.h"
#include "coc/AniObjMgr.h"
#include "form/BuildUI.h"
#include "form/MainUI.h"
#include "form/Shop.h"
#include "form/TypeShop.h"
#include "form/BarrackUI.h"
#include "form/FindMatchUI.h"
#include "form/MailUI.h"
#include "form/FightUI.h"
#include "form/InfoUI.h"
#include "form/AchievementUI.h"
#include "form/UpLevelUI.h"
#include "form/ScoreUI.h"
#include "form/MessageUI.h"
#include "form/ChgScene.h"
#include "form/ReplayUI.h"
#include "form/CreateUI.h"
#include "form/RankingUI.h"
#include "form/HeroUI.h"
#include "form/SoldierUI.h"
#include "form/PlayerUpLevelUI.h"
#include "form/LabUI.h"
#include "form/VisitUI.h"
#include "form/GuideUI.h"
#include "form/NotifyUI.h"
#include "form/RaidNotifyUI.h"
#include "form/BonusUI.h"
#include "form/SkillGlyphUI.h"
#include "form/SkillUI.h"
#include "form/SkillInfoUI.h"
#include "form/SettingUI.h"
#include "form/ClanUI.h"
#include "form/ClanInfoUI.h"
#include "form/SupportUI.h"
#include "form/SpinUI.h"
#include "form/DailyBonusUI.h"
#include "form/ChatUI.h"
#include "form/SaleLogoUI.h"
#include "form/GiftUI.h"
#include "form/EnvMapUI.h"
#include "form/MilitaryRankUI.h"
#include "form/LeagueUI.h"
#include "form/ClanWarMapUI.h"
#include "form/ClanWarScoreUI.h"
#include "form/GroupWarScoreUI.h"
#include "form/FBLoginUI.h"
#include "form/FBInviteUI.h"
#include "form/MainEditUI.h"
#include "form/FBShareUI.h"


class GameLayer;

class GameWorld : public cocos2d::CCLayer
{
public:
	GameWorld();
	virtual ~GameWorld();
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    //// there's no 'id' in cpp, so we recommand to return the exactly class pointer
    //static cocos2d::CCScene* scene();

	static void show();
    
    //// a selector callback
    //void menuCloseCallback(CCObject* pSender);

    //// a selector callback
    //void menuAttackCallback(CCObject* pSender);
    //// a selector callback
    //void menuShopCallback(CCObject* pSender);

    // implement the "static node()" method manually
    CREATE_FUNC(GameWorld);
public:
    
    void showBuildUI();
    
    void showFullScreenAds();
    
	void showShop(int shopid);

	void hideShop();

	void showTypeShop();

	void hideTypeShop();
    
    void showMainUI();
    
    void hideMainUI();
    
	void showBarrackUI();
    
	void hideBarrackUI();
    
	void showFindMatchUI();
    
	void hideFindMatchUI();
    
	void showFightUI();
    
	void hideFightUI();
    
	void showMailUI();
    
	void hideMailUI();
    
	void showAttackMsg();
    
	void hideAttackMsg();
    
    void showInfoUI();
    
    void hideInfoUI();
    
    void showUpLevelUI();
    
    void hideUpLevelUI();
    
    void showScoreUI();
    
    void hideScoreUI();

    void showChgScene(int mode);
    
    void hideChgScene();
    
    void showReplayUI();
    
    void hideReplayUI();
    
    void showTaskUI();
    
    void hideTaskUI();
    
    void showAchievementUI();
    
    void hideAchievementUI();
    
    void showCreateUI();
    
    void hideCreateUI();
    
    void showRankingUI();
    
    void hideRankingUI();
    
    void showHeroUI();
    
    void hideHeroUI();
    
    void showSoldierUI();
    
    void hideSoldierUI();
    
    void showPlayerLevelUpUI();
    
    void hidePlayerLevelUpUI();
    
    void showLabUI();
    
    void hideLabUI();
    
    void showVisitUI();
    
    void hideVisitUI();
    
    void showGuideUI();
    
    void hideGuideUI();
    
    void showNotifyUI();
    
    void hideNotifyUI();
    
    void showRaidNotifyUI();
    
    void hideRaidNotifyUI();
    
    void showBonusUI();
    
    void hideBonusUI();
    
    void showFBShareUI();
    void hideFBShareUI();
    
    
    void showSkillGlyphUI();
    
    void hideSkillGlyphUI();
    
    void showSkillUI();
    
    void hideSkillUI();
        
    void showSettingUI();
    void hideSettingUI();
    
    void showClanUI();
    void hideClanUI();
    
    void showClanInfoUI();
    void hideClanInfoUI();
    
    void showClanEditUI();
    void hideClanEditUI();
        
    void showChatUI();
    void hideChatUI();
    
    void showSupportUI();
    void hideSupportUI();
    
    void showSpinUI();
    void hideSpinUI();
    
    void showDailyBonusUI();
    void hideDailyBonusUI();
    
    void showSaleLogoUI();
    void hideSaleLogoUI();
    
    void showGiftUI();
    void hideGiftUI();
    
    void showFBLoginUI(int state);
    void hideFBLoginUI();
    
    void showFBInviteUI();
    void hideFBInviteUI();
    
    void showMilitaryRankUI();
    void hideMilitaryRankUI();
    
    void showMilitaryUpgradeUI();
    void hideMilitaryUpgradeUI();
    
    void showLeagueUI();
    void hideLeagueUI();
    
    void showClanWarMapUI();
    void hideClanWarMapUI();
    
    void showClanWarScoreUI();
    void hideClanWarScoreUI();
    
    void showGroupWarScoreUI();
    void hideGroupWarScoreUI();
        
    void showAnnouncementUI();
    void hideAnnouncementUI();
    
    void showEditUI();
    void hideEditUI();
    
	void onReleaseChgScene();
public:
    virtual void keyBackClicked();//鐩戝惉Android 杩斿洖閿?
    virtual void keyMenuClicked();//鐩戝惉Android 鑿滃崟閿?
    
public:
    MainUI*         m_pMainUI;
	TypeShop*       m_pTypeShop;
    BarrackUI*      m_pBarrackUI;
    FindMatchUI*    m_pFindMatchUI;
    MailUI*         m_pMailUI;
    FightUI*        m_pFightUI;
    InfoUI*         m_pInfoUI;
    UpLevelUI*      m_pUpLevelUI;
    ScoreUI*        m_pScoreUI;
    ReplayUI*       m_pReplayUI;
    AchievementUI*  m_pAchievementUI;
    CreateUI*       m_pCreateUI;
    RankingUI*      m_pRankingUI;
    HeroUI*         m_pHeroUI;
    SoldierUI*      m_pSoldierUI;
    PlayerUpLevelUI*m_pPlayerUpLevelUI;
    LabUI*          m_pLabUI;
    VisitUI*        m_pVisitUI;
    GuideUI*        m_pGuideUI;
    NotifyUI*       m_pNotifyUI;
    RaidNotifyUI*   m_pRaidNotifyUI;
    BonusUI*        m_pBonusUI;
    SkillGlyphUI*   m_pSkillGlyphUI;
    SkillUI*        m_pSkillUI;
    ClanUI*         m_pClanUI;
    ClanInfoUI*     m_pClanInfoUI;
    ClanEditUI*     m_pClanEditUI;
	Shop*           m_pShop;
    AnnouncementUI* m_pAnnouncementUI;
    
	ChgScene*	m_pChgScene;

	GameLayer*	m_pGameLayer;
    
    BuildUI*    m_pBuildUI;
    
    SettingUI* m_pSettingUI;
    
    SupportUI* m_pSupportUI;
    
    SpinUI     *m_pSpinUI;
    
    DailyBonusUI *m_pDailyBonusUI;
        
    SaleLogoUI *m_pSaleLogoUI;
    
    GiftUI *m_pGiftUI;
    
    FBLoginUI *m_pFBLoginUI;
    FBShareUI *m_pFBShareUI;
    FBInviteUI *m_pFBInviteUI;
    
    ChatUI* m_pChatUI;
    
    MilitaryRankUI *m_pMilitaryRankUI;
    
    MilitaryUpgradeUI *m_pMilitaryUpgradeUI;
    
    LeagueUI *m_pLeagueUI;
    
    ClanWarMapUI* m_pClanWarMapUI;
    
    ClanWarScoreUI* m_pClanWarScoreUI;
    
    GroupWarScoreUI* m_pGroupWarScoreUI;
    
    MainEditUI* m_pMainEditUI;
    
};

#endif  // __GAMEWORLD_SCENE_H__
