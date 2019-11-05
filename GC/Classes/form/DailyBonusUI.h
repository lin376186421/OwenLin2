//
//  DailyBonusUI.h
//  GameClient
//
//  Created by Lee  on 14-1-6.
//
//

#ifndef __GameClient__DailyBonusUI__
#define __GameClient__DailyBonusUI__

#include <iostream>
#include "../coc/NotifyListener.h"
#include "CUINode.h"

using namespace cocos2d;


class DailyBonusUI : public CUINode, public CCBAnimationManagerDelegate
{
public:
    DailyBonusUI();
    virtual ~DailyBonusUI();
    
    CREATE_FUNC(DailyBonusUI);
    
public:
    bool init();
        
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    void menuGetCallback(CCObject* pSender, CCControlEvent event);
    void completedAnimationSequenceNamed(const char *name);
    
    CCBAnimationManager* m_pAnimationManager;
    CCNode *m_pMainNode;
    CCLabelBMFont *m_pDaysLabel;
    
    int m_nContinueLoginDays;
    void refresh();
    
};

#endif /* defined(__GameClient__DailyBonusUI__) */
