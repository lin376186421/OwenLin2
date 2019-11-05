#ifndef __FORM_MAINUI_H__
#define __FORM_MAINUI_H__

#include "Shop.h"
#include "ChatUI.h"
#include "../coc/SelectBuildingListener.h"
#include "../coc/NetListener.h"
#include "../coc/NotifyListener.h"
#include "CUINode.h"

#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;
USING_NS_COC;

class MainUI : public CUINode, public SelectBuildingListener, public NotifyListener
{
public:
	MainUI();
	virtual ~MainUI();
public:
    // implement the "static create()" method manually
    CREATE_FUNC(MainUI);
public:
	bool init();
    virtual void onExit();
    virtual void onEnter();
public:
    //virtual bool onProc(const Json::Value& val);

	virtual bool onNotify(int notifyid, const void* pVal = NULL);
public:
	   
	virtual bool onSelect(bool isSelect, Building* pBuilding);

	void onRefurbishBuildingBtn();
    void refreshInfo();
    
    void onSelectHero(bool isSelect);
    void refreshHeroInfo();
    
	CCAction* setChgAni(CCSprite* spr);
    
    void update(float dt);
    void updateMoney(float dt);
    
    void showLeagueStartUI();
    void hideLeagueStartUI();
    
    void showGetLeagueAwardUI();
    void hideGetLeagueAwardUI();
    
    void showCityWarStartUI();
    void hideCityWarStartUI();
    
    void showAdsLoadingUI();
    void hideAdsLoadingUI();
    
public:
    
    void menuItemCallback(CCObject* pSender, CCControlEvent event);
    
    void menuAttackCallback(CCObject* pSender, CCControlEvent event);
    
    void menuShopCallback(CCObject* pSender, CCControlEvent event);
    
    void menuInfoCallback(CCObject* pSender, CCControlEvent event);
    
    void menuBuildCallback(CCObject* pSender, CCControlEvent event);
    
    void menuUpgradeCallback(CCObject* pSender, CCControlEvent event);
    
    void menuTroopsCallback(CCObject* pSender, CCControlEvent event);
    
    void menuCancelCallback(CCObject* pSender, CCControlEvent event);
    
    void menuFastCallback(CCObject* pSender, CCControlEvent event);
    
    void menuRemoveCallback(CCObject* pSender, CCControlEvent event);
    
    void menuHeroCallback(CCObject* pSender, CCControlEvent event);
    
    void menuResearchCallback(CCObject* pSender, CCControlEvent event);
    
    void menuWorkerCallback(CCObject* pSender, CCControlEvent event);
    
    void menuShieldCallback(CCObject* pSender, CCControlEvent event);
    
    void menuMailCallback(CCObject* pSender, CCControlEvent event);
    
    void menuAddGoldCallback(CCObject* pSender, CCControlEvent event);
    
    void menuAddElixirCallback(CCObject* pSender, CCControlEvent event);
    
    void menuAddGemCallback(CCObject* pSender, CCControlEvent event);
    
    void menuBonusCallback(CCObject* pSender, CCControlEvent event);
    
    void menuAnnouncementCallback(CCObject* pSender, CCControlEvent event);
    
    void menuBtnFBShareCallback(CCObject* pSender, CCControlEvent event);
    
    void menuUpgradeRowCallback(CCObject* pSender, CCControlEvent event);
    
    void menuAdLoadingCloseCallback(CCObject* pSender, CCControlEvent event);
    
    void menuClanCallback(CCObject* pSender, CCControlEvent event);
    
    void menuHelpClanCallback(CCObject* pSender, CCControlEvent event);
    
    void menuChatCallback(CCObject* pSender, CCControlEvent event);
        
    void menuSettingCallback(CCObject* pSender, CCControlEvent event);
    
    void menuTrainSkillCallback(CCObject* pSender, CCControlEvent event);
    void menuTrainCallback(CCObject* pSender, CCControlEvent event);
    void menuHeroHealCallback(CCObject* pSender, CCControlEvent event);
    
    void menuSpinCallback(CCObject* pSender, CCControlEvent event);
    
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    void menuGoCallback(CCObject* pSender, CCControlEvent event);
    
    void onSelectRow(CCObject* pSender, CCControlEvent event);
    void onSelectRotate(CCObject* pSender, CCControlEvent event);

    void rateApp(CCObject* pSender, CCControlEvent event);
    void getRateGem(CCObject* pSender, CCControlEvent event);
    
    void actionCallback(CCNode* pSender);

    void menuRankingCallback(CCObject* pSender, CCControlEvent event);
    void menuMilitaryRankCallback(CCObject* pSender, CCControlEvent event);
    
    //league
    void menuLeagueCallback(CCObject* pSender, CCControlEvent event);
    void menuLeagueStartUIColseCallback(CCObject* pSender, CCControlEvent event);
    void menuGoLeagueCallback(CCObject* pSender, CCControlEvent event);
    void menuGetAwardCallback(CCObject* pSender, CCControlEvent event);

    void menuClanWarStartColseCallback(CCObject* pSender, CCControlEvent event);
    
    void menuClanWarCallback(CCObject* pSender, CCControlEvent event);
    
    void menuTaskCallback(CCObject* pSender, CCControlEvent event);
    void playerEffect(int i);
    
    void countShopNums();
    void countAchievementNums();
    int countMailNums(int type);
    
    int countChatNums(int type);
    void countChatNums(int type, ChatData chatData);
    
    void showBtnTip();
    
    void guide();
    void setProtectTimeStr(float fDelta);
    std::string getProtectTimeStr();
        
    void actionEndCallback(CCNode* pSender);
    
    void menuEditCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
	CCControlButton*	m_pBtnInfo;
	CCControlButton*	m_pBtnTroops;
	CCControlButton*	m_pBtnCancel;
	CCControlButton*	m_pBtnFast;
    CCControlButton*    m_pBtnBuild;
    CCControlButton*    m_pBtnUpgrade;
	CCControlButton*	m_pBtnRemoveByGold;
	CCControlButton*	m_pBtnRemoveByElixir;
	CCControlButton*	m_pBtnResearch;
	CCControlButton*	m_pBtnAddWorker;
    CCControlButton*	m_pBtnAddShield;
    CCControlButton*    m_pBtnAddGold;
    CCControlButton*    m_pBtnAddElixir;
    CCControlButton*    m_pBtnAddGem;
    CCControlButton*    m_pBtnMail;
    CCControlButton*    m_pBtnAchievement;
    CCControlButton*    m_pBtnFreeBonus;
    CCControlButton*    m_pBtnRanking;
    CCControlButton*    m_pBtnHeroHeal;
    CCControlButton*    m_pBtnSetting;
    CCControlButton*    m_pBtnMilitaryRank;
    CCControlButton*    m_pBtnSell;
    CCControlButton*    m_pBtnUpgradeRow;
    
    CCControlButton*    m_pBtnTrainSkill;

    CCControlButton*    m_pBtnTrain;
    
    CCControlButton*    m_pBtnRotate;
    CCControlButton*    m_pBtnRow;

    CCControlButton*    m_pBtnClan;
    CCControlButton*    m_pBtnClanTroopsRequest;
    CCControlButton*    m_pBtnClanTroopsFull;
    
    CCControlButton*    m_pBtnChat;
    
    CCControlButton*    m_pBtnSpin;
    
    CCControlButton*    m_pBtnAnnouncement;
    
    CCControlButton*    m_pBtnFBShare;
    
    CCControlButton*    m_pBtnEdit;
    
    CCNode* m_pInfoNode;
    
    CCLabelBMFont*  m_pGoldCurNums;
    CCLabelBMFont*  m_pGoldMaxNums;
    CCLabelBMFont*  m_pElixirCurNums;
    CCLabelBMFont*  m_pElixirMaxNums;
    CCLabelBMFont*  m_pGemNums;
    CCLabelBMFont*  m_pWorkerNums;
    CCLabelBMFont*  m_pLevel;
    CCLabelBMFont*  m_pCup;

    CCScale9Sprite* m_pProgressGold;
    CCScale9Sprite* m_pProgressElixir;
    
    CCScale9Sprite* m_pGoldFullTip;
    CCScale9Sprite* m_pOilFullTip;
    
    CCScale9Sprite* m_pHeroHpProgress;
    CCScale9Sprite* m_pHeroExpProgress;
    CCLabelBMFont* m_pHeroExp;
    
    CCLabelBMFont* m_pHeroLevel;
    CCLabelBMFont* m_pHeroHPTime;
    
    CCLabelTTF* m_pName;
    CCLabelBMFont* m_pShieldTime;
    
    CCSprite *m_pMiletaryName;
    CCSprite* m_pIconMilitary;
    CCSprite* m_pIconGold;
    CCSprite* m_pIconElixir;
    CCSprite* m_pIconGem;
    CCSprite* m_pIconLevel;
    CCSprite* m_pExpProgress;
    CCProgressTimer* m_pExpProgressTimer;
    
    CCControlButton* m_pBtnShop;
    CCControlButton* m_pBtnAttack;
    CCControlButton* m_pBtnLeague;
    
    CCControlButton* m_pBtnClanWar;

    CUINode* m_pMoreGameUI;
    CUINode* m_pLeagueStartUI;
    CUINode* m_pGetAwardUI;
    CUINode* m_pCityWarStartUI;
    
    CUINode* m_pAdsLoadingUI;
    CCLabelBMFont *m_pAdsLoadingTip;

	std::vector<CCControlButton*>		m_lstBuildingBtn;

	int			m_gold;
	int			m_oil;
    int         m_gem;
    int         m_goldOff;
    int         m_oilOff;
    int         m_gemOff;
    int         m_protectTime;

	CCAction*	m_pActGold;
	CCAction*	m_pActOil;
    CCAction*   m_pActGem;
};

#endif  // __FORM_MAINUI_H__