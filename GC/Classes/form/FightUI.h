//
//  FightUI.h
//  coc4
//
//  Created by chowray on 13-4-3.
//
//

#ifndef __coc4__FightUI__
#define __coc4__FightUI__

#include "../coc/NetListener.h"
#include "../coc/NotifyListener.h"
#include "CUINode.h"

using namespace cocos2d;
USING_NS_COC;

class FightUI : public CUINode, public NotifyListener
{
public:
    FightUI();
    virtual ~FightUI();
public:
    CREATE_FUNC(FightUI);
public:
    bool init();
    
	virtual bool onNotify(int notifyid, const void* pVal = NULL);
    
    void refreshInfo();
    
    void showStars();
    
    void exit(float fDelta);
    
    void end();
    
    void chgMode(int mode);
    
    void MutUpdate(float fDelta);
    
    void onTimer(float time);
    
    CCAction* setChgAni(CCSprite* spr);
	//virtual bool onProc(const Json::Value& val);
public:
    // a selector callback
    void menuEndFightCallback(CCObject* pSender, CCControlEvent event);
    // a selector callback
    void menuFindCallback(CCObject* pSender, CCControlEvent event);
    
    void menuSoldierCallback(CCObject* pSender, CCControlEvent event);
    void menuClanSoldierCallback(CCObject* pSender, CCControlEvent event);
    
    void menuHeroCallback(CCObject* pSender, CCControlEvent event);
    
    void menuSkill1Callback(CCObject* pSender, CCControlEvent event);
    void menuSkill2Callback(CCObject* pSender, CCControlEvent event);
    void menuSkill3Callback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
    
    CCLabelTTF* m_pRivalName;
    CCLabelBMFont* m_pRivalGold;
    CCLabelBMFont* m_pRivalElixir;
    CCLabelBMFont* m_pWinCup;
    CCLabelBMFont* m_pLoseCup;
    
    CCLabelTTF* m_pRivalClanName;
    CCSprite* m_pRivalClanIcon;
    
    CCLabelTTF* m_pClanName;
    CCSprite* m_pClanIcon;
    
    CCScale9Sprite* m_pProgressGold;
    CCScale9Sprite* m_pProgressElixir;
    
    CCLabelBMFont* m_pPlayerGold;
    CCLabelBMFont* m_pPlayerMaxGold;
    CCLabelBMFont* m_pPlayerElixir;
    CCLabelBMFont* m_pPlayerMaxElixir;
    
    CCLabelBMFont* m_pBattleTime;
    CCLabelBMFont* m_pBattleTimeTip;
    CCLabelBMFont* m_pDropSoldierTip;
    
    CCLabelBMFont* m_pBattleProgress;
    CCSprite* m_pBattleStars;
    
    CCSprite* m_pIconGold;
    CCSprite* m_pIconElixir;
    
    CCNode* m_pCupNode;
    CCNode *m_pLootNode;
    
    CCControlButton* m_pBtnEndBattle;
    CCControlButton* m_pBtnFindMatch;
    
    CCNode* m_pClanSoldierNode;
    CCControlButton* m_pBtnClanSoldier;
    
    CCLayer* m_pTroopsView;
    CCLayer* m_pSoldierView;
    
    CCAction*       m_pActGold;
	CCAction*       m_pActOil;
    
    CCControlButton* pSoldierSelected;
    
    int				m_mode;
    
    long long		m_i64RefurbishTime;
	bool			m_bNeedRefurbish;
    
	int				m_lastWaitTimeMs;
	int				m_lastFightTimeMs;
    
    int             m_starnum;
    bool            m_half;
    
    CCControlButton* m_pBtnHero;
    
    CCControlButton* m_pSkill1;
    CCControlButton* m_pSkill2;
    CCControlButton* m_pSkill3;
    
    CCProgressTimer* m_pPT1;
    CCProgressTimer* m_pPT2;
    CCProgressTimer* m_pPT3;
        
    CCBAnimationManager* m_pAnimationManager;
    bool            m_isSkillOn;
    bool            m_isHeroDroped;
    
    int             m_time1;
    int             m_time2;
    int             m_time3;
    
    CCSprite *m_pIconMilitary;
};

#endif /* defined(__coc4__FightUI__) */
