//
//  GroupWarScoreUI.h
//  GameClient
//
//  Created by qsun on 14-10-30.
//
//

#ifndef __GameClient__GroupWarScoreUI__
#define __GameClient__GroupWarScoreUI__

#include "CUINode.h"
#include "CUIExt.h"
#include "../coc/BaseDef.h"

USING_NS_COC;

class GroupWarScoreUI : public CUINode, public CCBAnimationManagerDelegate {
    
public:
    GroupWarScoreUI();
    virtual ~GroupWarScoreUI();
    
    CREATE_FUNC(GroupWarScoreUI);
public:
    
    bool init();
    
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
    
    CCNode* m_pView;
    
    CCLabelBMFont* m_pCityName;
    
    CCLabelBMFont* m_pTitle;
    CCLabelBMFont* m_pMessage;
    CCLabelBMFont* m_pResultTip;
    
    CCLabelBMFont* m_pDailyGemTip;
    
    CCSprite* m_pWarRanking;
    CCSprite* m_pWarStar;
    CCSprite* m_pRankingRewardGems;
    CCSprite* m_pStarRewardGems;
    
    CCControlButton* m_pBtnFBShare;
};


#endif /* defined(__GameClient__GroupWarScoreUI__) */
