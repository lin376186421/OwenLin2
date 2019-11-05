//
//  soldierUI.h
//  GameClient
//
//  Created by chowray on 13-5-21.
//
//

#ifndef __GameClient__soldierUI__
#define __GameClient__soldierUI__

#include "CUINode.h"

using namespace cocos2d;

class SoldierUI : public CUINode
{
public:
    SoldierUI();
    virtual ~SoldierUI();
public:
    CREATE_FUNC(SoldierUI);
public:
    bool init();
public:
    
    void refreshInfo();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    void menuBackCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
    
    CCLabelBMFont* m_pTitle;
    CCSprite*  m_pIcon;
    CCNode* m_pBuildingAtt1;
    CCNode* m_pBuildingAtt2;
    CCNode* m_pBuildingAtt3;
    
    CCNode* m_pAppendAtt[6];
    
    CCLabelBMFont* m_pDesc;
};

#endif /* defined(__GameClient__soldierUI__) */
