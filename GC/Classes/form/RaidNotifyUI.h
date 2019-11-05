//
//  RaidNotifyUI.h
//  GameClient
//
//  Created by chowray on 13-8-25.
//
//

#ifndef __GameClient__RaidNotifyUI__
#define __GameClient__RaidNotifyUI__

#include "CUINode.h"

using namespace cocos2d;

class RaidNotifyUI : public CUINode
{
public:
    RaidNotifyUI();
    virtual ~RaidNotifyUI();
public:
    CREATE_FUNC(RaidNotifyUI);
public:
    bool init();
public:
    // a selector callback
    void menuCloseCallback(CCObject* pSender, CCControlEvent cce);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCLabelBMFont* m_pCup;
    CCLayer* m_pItemView;
};

#endif /* defined(__GameClient__ZhanBao__) */
