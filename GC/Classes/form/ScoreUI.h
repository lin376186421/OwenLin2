//
//  ScoreUI.h
//  coc4
//
//  Created by chowray on 13-4-11.
//
//

#ifndef __coc4__ScoreUI__
#define __coc4__ScoreUI__

#include "CUINode.h"
#include "../coc/BaseDef.h"

using namespace cocos2d;
USING_NS_COC;

enum ScoreUIType {
    SCORE_TYPE_ATTACK = LOGICSTATE_ATTACK,
    SCORE_TYPE_REPLAY = LOGICSTATE_REPLAY
};

class ScoreUI : public CUINode
{
public:
    ScoreUI(int type);
    virtual ~ScoreUI();

    static ScoreUI* create(int type);
    
public:
    bool init();
    
public:
    
    void menuReplayCallback(CCObject *pSender, CCControlEvent event);
    void menuBackCallback(CCObject* pSender, CCControlEvent event);
    void menuBtnFBShareCallback(CCObject* pSender, CCControlEvent event);

    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:

    int m_type;
    
    CCSprite* m_pBattleStars;
    CCLabelBMFont* m_pBattleProgress;
    
    CCLabelTTF* m_pRivalName;
    CCLabelBMFont* m_pRivalGold;
    CCLabelBMFont* m_pRivalElixir;
    
    CCLabelTTF *m_pPlayerName;
    
    CCLabelTTF* m_pRivalClanName;
    CCSprite* m_pRivalClanIcon;
    
    CCLabelTTF* m_pClanName;
    CCSprite* m_pClanIcon;
    
    CCLabelBMFont* m_pGainGold;
    CCLabelBMFont* m_pGainElixir;
    CCLabelBMFont* m_pGainCup;
    CCLabelBMFont* m_pBattleResult;
    CCLabelBMFont* m_pHeroExp;
    
    CCControlButton* m_pBtnFBShare;
    
    CCSprite* m_pCupIcon;
    CCSprite* m_pHeroIcon;
    
    CCNode* m_pHeroNode;
    
    CCLayer* m_pTroopsView;
    
    CCSprite *m_pAttackMilitaryIcon;
    CCSprite *m_pDefendMilitaryIcon;
};

#endif /* defined(__coc4__ScoreUI__) */
