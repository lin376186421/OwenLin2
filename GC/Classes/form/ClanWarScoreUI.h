//
//  ClanWarScoreUI.h
//  GameClient
//
//  Created by qsun on 14-9-22.
//
//

#ifndef __GameClient__ClanWarScoreUI__
#define __GameClient__ClanWarScoreUI__

#include "CUINode.h"
#include "CUIExt.h"
#include "../coc/BaseDef.h"

USING_NS_COC;

class ClanWarScoreUI : public CUINode, public CCBAnimationManagerDelegate {
    
public:
    ClanWarScoreUI();
    virtual ~ClanWarScoreUI();
    
    CREATE_FUNC(ClanWarScoreUI);
public:
    
    bool init();
    
    void actionUpdate(float dt);
    
    void refreshInfo();
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    void menuOkCallback(CCObject* pSender, CCControlEvent event);
    
    void menuBtnFBShareCallback(CCObject* pSender, CCControlEvent event);

    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    void completedAnimationSequenceNamed(const char *name);
    
public:
    
    CCNode* m_pResultAnim;
    CCNode* m_pView;
    
    float m_actionDelay;
    
    CCBAnimationManager* m_pAnimationManager;
    
    CCSprite* m_pClanIcon;
    CCLabelTTF* m_pClanName;
    
    CCLabelBMFont* m_pCityName;
    
    CCLabelBMFont* m_pTitle;
    CCLabelBMFont* m_pMessage;
    CCLabelBMFont* m_pResultTip;
    
    CCSprite* m_pWarRanking;
    CCSprite* m_pRewardGems;
    
    CCControlButton* m_pBtnFBShare;
};

#endif /* defined(__GameClient__ClanWarScoreUI__) */
