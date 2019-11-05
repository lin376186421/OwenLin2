//
//  SkillGlyphUI.h
//  GameClient
//
//  Created by chowray on 13-9-23.
//
//

#ifndef __GameClient__SkillGlyphUI__
#define __GameClient__SkillGlyphUI__

#include "../coc/NotifyListener.h"
#include "CUINode.h"

using namespace cocos2d;
USING_NS_COC;

class SkillGlyphUI : public CUINode, public NotifyListener, public CCScrollViewDelegate
{
public:
    SkillGlyphUI();
    virtual ~SkillGlyphUI();
public:
    CREATE_FUNC(SkillGlyphUI);
public:
    bool init();
    bool onNotify(int notifyid, const void* pVal = NULL);
public:
    
    void refresh();
    
    void menuItemCallback(CCObject* pSender, CCControlEvent event);
    void menuChangeCallback(CCObject* pSender, CCControlEvent event);
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    
    void changeSkill();
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view){};
public:
    
    CCLabelBMFont* m_pName;
    CCLabelBMFont* m_pInfo;
    CCSprite* m_pIcon;

    CCLabelBMFont* m_pSpinGems;

    CCScrollView* m_pItemView;
    
    CCControlButton* m_pBtnChange;
    
    vector<CCControlButton*> m_skillList;
    
    int m_glyphNums;
    int m_selectId;

    bool m_isDrag;
    CCPoint m_dragOffset;
};

#endif /* defined(__GameClient__SkillGlyphUI__) */
