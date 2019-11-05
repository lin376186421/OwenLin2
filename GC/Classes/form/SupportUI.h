//
//  SupportUI.h
//  GameClient
//
//  Created by qsun on 13-12-3.
//
//

#ifndef __GameClient__SupportUI__
#define __GameClient__SupportUI__

#include "CUINode.h"

class SupportUI : public CUINode
{
public:
    SupportUI();
    virtual ~SupportUI();
public:
    CREATE_FUNC(SupportUI);
public:
    bool init();
    
    void menuForumCallback(CCObject* pSender, CCControlEvent event);
    void menuSubmitCallback(CCObject* pSender, CCControlEvent event);
    void menuBackCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCScale9Sprite* m_pEmailInputBg;
    CCScale9Sprite* m_pDescInputBg;
    
    CCLabelBMFont* m_pUniqueId;
    
    CCControlButton* m_pBtnForum;
    CCControlButton* m_pBtnSubmit;
    
    CTextInputField* m_pEmailField;
    CTextInputField* m_pDescField;
    
};
class AnnouncementUI : public CUINode
{
public:
    AnnouncementUI();
    virtual ~AnnouncementUI();
public:
    CREATE_FUNC(AnnouncementUI);
public:
    bool init();
    
    virtual void onExit();
    virtual void onEnter();
    void menuBackCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCNode *m_pWebNode;
    CCNode *m_pLoading;
    CCControlButton *m_pBackBtn;
    void showAnnouncementWebView();
    
};
#endif /* defined(__GameClient__SupportUI__) */
