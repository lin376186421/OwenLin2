//
//  ClanWarHelpUI.h
//  GameClient
//
//  Created by qsun on 14-9-18.
//
//

#ifndef __GameClient__ClanWarHelpUI__
#define __GameClient__ClanWarHelpUI__

#include "../coc/BaseDef.h"
#include "CUINode.h"

USING_NS_COC;

class ClanWarHelpUI : public CUINode {
    
public:
    ClanWarHelpUI(int type);
    virtual ~ClanWarHelpUI();
    
    static ClanWarHelpUI* create(int type);
    
public:
    
    bool init();
    
    void addPageNode();
    void refreshPage();
    
    void menuHelpCallback(CCObject* pSender, CCControlEvent event);
    
    void menuPageCallback(CCObject* pSender, CCControlEvent event);
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    int m_type;
    
    int m_pageIndex;
    
    CCNode* m_pPageNode;
    
    CCControlButton* m_pBtnPages[2];
    
    CCControlButton* m_pBtnHelp[5];
    CCNode* m_pHelpNode[5];
};

#endif /* defined(__GameClient__ClanWarHelpUI__) */
