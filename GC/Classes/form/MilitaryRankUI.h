//
//  MilitaryRankUI.h
//  GameClient
//
//  Created by Lee on 14-5-5.
//
//

#ifndef __GameClient__MilitaryRankUI__
#define __GameClient__MilitaryRankUI__

#include <iostream>

#include "CUINode.h"

using namespace cocos2d;


class MilitaryRankUI : public CUINode
{
public:
    MilitaryRankUI();
    virtual ~MilitaryRankUI();
    
    CREATE_FUNC(MilitaryRankUI);
public:
    
    bool init();
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    
    void refreshInfo();

    
    
public:
    CCLabelBMFont *m_pGlobalLabel;
    CCLabelBMFont *m_pMilitaryRankLabel;
    CCNode* m_pMilitaryRankUI;
    
};

class MilitaryUpgradeUI : public CUINode
{
public:
    MilitaryUpgradeUI();
    virtual ~MilitaryUpgradeUI();
    
    CREATE_FUNC(MilitaryUpgradeUI);
public:
    
    bool init();
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    void menuOkCallback(CCObject* pSender, CCControlEvent event);
    void menuBtnFBShareCallback(CCObject* pSender, CCControlEvent event);

    
    void refreshInfo();
    
    void playEffect(float ot);
    
public:
    CCSprite *m_pLastIcon;
    CCSprite *m_pNowIcon;
    CCLabelBMFont *m_pInfoLabel;
    
    CCControlButton*    m_pBtnFBShare;
};

#endif /* defined(__GameClient__MilitaryRankUI__) */
