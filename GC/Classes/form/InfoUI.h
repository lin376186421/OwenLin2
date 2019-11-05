//
//  InfoUI.h
//  coc4
//
//  Created by chowray on 13-4-7.
//
//

#ifndef __coc4__InfoUI__
#define __coc4__InfoUI__

#include "../coc/BaseDef.h"
#include "CUINode.h"

using namespace cocos2d;

enum InfoAttTag {
    ATT_TAG_ICON = 1,
    ATT_TAG_PROGRESS_ADD = 2,
    ATT_TAG_PROGRESS = 3,
    ATT_TAG_VALUE = 4
};

class InfoUI : public CUINode
{
public:
    InfoUI();
    virtual ~InfoUI();
public:
    CREATE_FUNC(InfoUI);
public:
    bool init();
    
    void update(float dt);
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    // a selector callback
    void menuBackCallback(CCObject* pSender, CCControlEvent event);
    void refreshInfo();
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
    CCLabelBMFont* m_pTitle;
    CCLabelBMFont* m_pName;
    CCSprite*  m_pIcon;
    CCNode* m_pBuildingAtt1;
    CCNode* m_pBuildingAtt2;
    CCNode* m_pBuildingAtt3;
    
    CCNode* m_pAppendAtt[4];
    
    CCLabelBMFont* m_pTroopsTitle;
    CCLayer* m_pTroopsView;
    CCScrollView* m_pScrollView;
    
    CCLabelBMFont* m_pDesc;
    
    bool m_bUpgrade;
};

#endif /* defined(__coc4__InfoUI__) */
