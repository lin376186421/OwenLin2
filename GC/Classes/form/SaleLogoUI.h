//
//  SaleLogoUI.h
//  GameClient
//
//  Created by owenlin on 14-2-24.
//
//

#ifndef __GameClient__SaleLogoUI__
#define __GameClient__SaleLogoUI__

#include <iostream>
#include "../coc/NotifyListener.h"
#include "CUINode.h"

using namespace cocos2d;


class SaleLogoUI : public CUINode
{
public:
    SaleLogoUI();
    virtual ~SaleLogoUI();
    CREATE_FUNC(SaleLogoUI);
    bool init();
    
    virtual void onExit();
    virtual void onEnter();
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    void menuBuyCallback(CCObject* pSender, CCControlEvent event);
    
    void refreshInfo();
    void click(float ot);
    
public:
    CCLabelBMFont *m_pTimeLabel;
    double m_iRemainTime;
    
};

#endif /* defined(__GameClient__SaleLogoUI__) */
