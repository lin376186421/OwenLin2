//
//  LabUI.h
//  GameClient
//
//  Created by chowray on 13-5-27.
//
//

#ifndef __GameClient__LabUI__
#define __GameClient__LabUI__

#include "../coc/NotifyListener.h"
#include "CUINode.h"

using namespace cocos2d;
USING_NS_COC;

class LabUI : public CUINode, public NotifyListener
{
public:
    LabUI();
    virtual ~LabUI();
public:
    CREATE_FUNC(LabUI);
public:
    bool init();
public:
    // a selector callback
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    
    void menuFinishCallback(CCObject* pSender, CCControlEvent event);
    
    void menuInfoCallback(CCObject* pSender, CCControlEvent event);
    
    void refreshInfo();
    void refreshTime(float dt);
    
    bool onNotify(int notifyid, const void* pVal = NULL);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
    
    CCLabelBMFont* m_pTitle;
    
    CCLabelBMFont* m_pUpgradeTime;
    CCLabelBMFont* m_pUpgradeName;
    CCSprite* m_pUpgradeIcon;
    CCControlButton* m_pBtnFinish;
    
    CCNode* m_pUpgradingNode;
    CCNode* m_pUpgradeTip;
    
    CCLayer* m_pItemView;
        
    vector<int>    m_lstLab;
};

#endif /* defined(__GameClient__LabUI__) */
