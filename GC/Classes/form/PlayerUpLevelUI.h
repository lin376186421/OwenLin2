//
//  PlayerUpLevelUI.h
//  GameClient
//
//  Created by chowray on 13-5-27.
//
//

#ifndef __GameClient__PlayerUpLevelUI__
#define __GameClient__PlayerUpLevelUI__

#include "CUINode.h"
#include "SoldierUI.h"

using namespace cocos2d;

class PlayerUpLevelUI : public SoldierUI
{
public:
    PlayerUpLevelUI();
    virtual ~PlayerUpLevelUI();
public:
    CREATE_FUNC(PlayerUpLevelUI);
public:
    bool init();
public:
    // a selector callback
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    void menuBackCallback(CCObject* pSender, CCControlEvent event);
    void menuUpgradeCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
    
    CCLabelBMFont* m_pUpgradeTime;
    CCControlButton* m_pBtnUpgrade;
};

#endif /* defined(__GameClient__PlayerUpLevelUI__) */
