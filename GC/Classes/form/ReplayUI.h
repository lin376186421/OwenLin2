//
//  ReplayUI.h
//  GameClient
//
//  Created by chowray on 13-4-27.
//
//

#ifndef __GameClient__ReplayUI__
#define __GameClient__ReplayUI__

#include "../coc/NotifyListener.h"
#include "CUINode.h"

using namespace cocos2d;
USING_NS_COC;

class ReplayUI : public CUINode, public NotifyListener
{
public:
    ReplayUI();
    virtual ~ReplayUI();
public:
    CREATE_FUNC(ReplayUI);
public:
	virtual bool onNotify(int notifyid, const void* pVal = NULL);
public:
    bool init();
    void refreshInfo();
    
    void updateBattleTime(float dt);
public:
    
    void showStars();
    
    void menuBackCallback(CCObject* pSender, CCControlEvent event);
    void menuSpeedCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
    
    CCLabelTTF* m_pRivalName;
    CCLabelBMFont* m_pRivalGold;
    CCLabelBMFont* m_pRivalElixir;
    
    CCLabelTTF* m_pRivalClanName;
    CCSprite* m_pRivalClanIcon;
    
    CCLabelTTF* m_pPlayerName;
    CCLabelBMFont* m_pGainGold;
    CCLabelBMFont* m_pGainElixir;
    
    CCLabelTTF* m_pClanName;
    CCSprite* m_pClanIcon;
    
    CCSprite* m_pBattleStars;
    CCLabelBMFont* m_pBattleProgress;
    CCControlButton* m_pBtnReplaySpeed;
    CCLabelBMFont* m_pBattleTime;
    
    CCSprite *m_pAttackMilitaryIcon;
    CCSprite *m_pDefendMilitaryIcon;
    
    int             m_starnum;
    bool            m_half;
};

#endif /* defined(__GameClient__ReplayUI__) */
