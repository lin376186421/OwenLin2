//
//  ClanWarMapUI.h
//  GameClient
//
//  Created by qsun on 14-9-4.
//
//

#ifndef __GameClient__ClanWarMapUI__
#define __GameClient__ClanWarMapUI__

#include "../coc/BaseDef.h"
#include "CUINode.h"
#include "ClanWarBattleUI.h"
#include "ClanWarSignupUI.h"
#include "ClanWarHelpUI.h"
#include "GroupWarBattleUI.h"
#include "../coc/NotifyListener.h"

USING_NS_COC;

class ClanCityUI;
class ClanCityMenuUI;

class ClanWarMapUI : public CUINode, public NotifyListener{
    
public:
    ClanWarMapUI();
    virtual ~ClanWarMapUI();
    
    CREATE_FUNC(ClanWarMapUI);
    
    virtual bool onNotify(int notifyid, const void* pVal = NULL);
public:
    
    bool init();
        
    void update(float dt);
    
    void refreshInfo();
    
    void onFocusCity();
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
    void menuHelpCallback(CCObject* pSender, CCControlEvent event);
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    
    void menuCityCallback(CCObject* pSender, ClanCityUI* pCityNode);
    
    void menuCityWarCallback(CCObject* pSender, ClanCityUI* pCityNode);
    
    void showClanWarBattleUI();
    void hideClanWarBattleUI();
        
    void showClanWarSignupUI();
    void hideClanWarSignupUI();
    
    void showGroupWarBattleUI();
    void hideGroupWarBattleUI();
    
    void showClanWarHelpUI(int type);
    void hideClanWarHelpUI();
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
        
public:
    CCScrollView* m_pMapView;
    
    CUINode* m_pLoadingUI;
    
    CCControlButton* m_pBtnClanWar;
    CCLabelBMFont* m_pClanWarStateTip;
    CCLabelBMFont* m_pClanWarStateTime;
    
    CCControlButton* m_pBtnGroupWar;
    CCLabelBMFont* m_pGroupWarStateTip;
    CCLabelBMFont* m_pGroupWarStateTime;
    
    ClanCityMenuUI* m_pCityMenuUI;
    ClanWarBattleUI* m_pClanWarBattleUI;
    ClanWarSignupUI* m_pClanWarSignupUI;
    ClanWarHelpUI* m_pClanWarHelpUI;
    
    GroupWarBattleUI* m_pGroupWarBattleUI;
    
    CCControlButton* m_pBtnClose;
    CCControlButton* m_pBtnCityWar;
    
    ClanCityInfo* m_pSelectCityInfo;
    
    CCNode* m_pClanWarTipUI;
    
    bool m_bProcScaleAni;
    
    int m_lastClanWarState;
    int m_lastGroupWarState;
};

class ClanCityUI : public CUINode {
public:
    ClanCityUI(ClanCityInfo* pCityInfo);
    virtual ~ClanCityUI();
    
    static ClanCityUI* create(ClanCityInfo* pCityInfo);
    
public:
    bool init();
    
    bool checkTouchInside(CCObject* pSender);
    
    void menuCityCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    ClanCityInfo* m_pCityInfo;
    
    CCLabelBMFont* m_pCityName;
    CCSprite* m_pCityFlag;
    
    CCLabelTTF* m_pClanName;
    CCSprite* m_pClanIcon;
    
    CCSprite* m_pClanWarState;
};

class ClanCityMenuUI : public CUINode{
    
public:
    ClanCityMenuUI();
    virtual ~ClanCityMenuUI();
    
    CREATE_FUNC(ClanCityMenuUI);
    
public:
    bool init();
    
    void setCityInfo(ClanCityInfo* pCityInfo);
    
    void menuClanInfoCallback(CCObject* pSender, CCControlEvent event);

    void menuSignupCallback(CCObject* pSender, CCControlEvent event);
    void menuBattleCallback(CCObject* pSender, CCControlEvent event);
    void menuDailyGemsCallback(CCObject* pSender, CCControlEvent event);
    void menuRewardInfoCallback(CCObject *pSender, CCControlEvent event);
    
    void menuCloseRewardInfoCallback(CCObject *pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCNode* m_pView;
    ClanCityInfo* m_pCityInfo;
    
    vector<CCNode*> m_cityNodeList;
    
    CCLabelBMFont* m_pTitle;
    
    CCSprite* m_pClanNational;
    CCSprite* m_pClanIcon;
    CCLabelTTF* m_pClanName;
    CCLabelBMFont* m_pNationalName;
    CCLabelBMFont* m_pWinGems;
    CCLabelBMFont* m_pProductGems;
    
    CUINode* m_pRewardInfoUI;
    
    CCControlButton* m_pBtnSignup;
    CCControlButton* m_pBtnEnterBattle;
    CCControlButton* m_pBtnCheckBattle;
    CCControlButton* m_pBtnDailyGems;
};

#endif /* defined(__GameClient__ClanWarMapUI__) */
