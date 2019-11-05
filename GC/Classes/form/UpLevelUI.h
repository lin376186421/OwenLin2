//
//  UpLevelUI.h
//  coc4
//
//  Created by chowray on 13-4-8.
//
//

#ifndef __coc4__UpLevelUI__
#define __coc4__UpLevelUI__

#include "InfoUI.h"

using namespace cocos2d;

class UpLevelUI : public InfoUI
{
public:
    UpLevelUI();
    virtual ~UpLevelUI();
public:
    CREATE_FUNC(UpLevelUI);
public:
    
    bool init();

    void update(float dt);
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    // a selector callback
    void menuBackCallback(CCObject* pSender, CCControlEvent event);
    // a selector callback
    void menuUpCallback(CCObject* pSender, CCControlEvent event);
    void refreshInfo();
    
    void refreshTownHall();
    void refreshBarrack();
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
    
    CCLabelBMFont* m_pUpgradeTime;
    CCLabelBMFont* m_pUnlockTip;
    CCLabelBMFont* m_pNote;
    CCLabelBMFont* m_pUpgradeDesc;
    CCControlButton* m_pBtnUpgrade;
    
    CCNode* m_pUnUpgradeTip;
    
    CCLayer* m_pItemView;
    CCScrollView* m_pScrollView;

    CCNode* m_pTownHallItems;
};

#endif /* defined(__coc4__UpLevelUI__) */
