//
//  GiftUI.h
//  GameClient
//
//  Created by owenlin on 14-3-12.
//
//

#ifndef __GameClient__GiftUI__
#define __GameClient__GiftUI__

#include <iostream>
#include "CUINode.h"

using namespace cocos2d;

enum
{
    Tag_dayLabel = 1,
    Tag_gemLabel = 2,
    Tag_boxSprite = 3,
    Tag_markSprite = 4,
};

enum
{
    Tag_buyBtn = 101,
    Tag_totalGemLabel = 102,
    Tag_percentLabl = 103,
    Tag_bestValueSprite = 104,
    Tag_gemSprite = 105,
    Tag_priceLabel = 106,
};


class GiftUI : public CUINode
{
public:
    GiftUI();
    virtual ~GiftUI();
    
    CREATE_FUNC(GiftUI);
public:
    
    bool init();
        
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    

    
    void menuGoCallback(CCObject* pSender, CCControlEvent event);
    void menuClaimCallback(CCObject* pSender, CCControlEvent event);
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    void menuBuyCallback(CCObject* pSender, CCControlEvent event);
    void menuGetGemCallback(CCObject* pSender, CCControlEvent event);
    
    void refreshInfo();
    void click(float ot);
    
    std::vector<int> m_iProductIdList;


public:
    CCLabelBMFont *m_pTimeLabel;
    CCNode *m_pClaimNode;
    CCNode *m_pTipNode;
    CCControlButton *m_pCloseBtn;
    CCLabelBMFont *m_pTitleLabel;
    CCLabelBMFont *m_pinfoLabel;
    
    
};
#endif /* defined(__GameClient__GiftUI__) */
