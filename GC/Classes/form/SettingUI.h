//
//  SettingUI.h
//  GameClient
//
//  Created by qsun on 13-11-14.
//
//

#ifndef __GameClient__SettingUI__
#define __GameClient__SettingUI__

#include "CUINode.h"

class SettingUI : public CUINode, public CCBAnimationManagerDelegate
{
public:
    SettingUI();
    virtual ~SettingUI();
public:
    CREATE_FUNC(SettingUI);
public:
    bool init();
    
    void refreshInfo();
    
    
    void menuMusicCallback(CCObject* pSender, CCControlEvent event);
    void menuSoundCallback(CCObject* pSender, CCControlEvent event);
    void menuSupportCallback(CCObject *pSender, CCControlEvent event);
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    void menuAboutCallback(CCObject* pSender, CCControlEvent event);
    void menuFBCallback(CCObject* pSender, CCControlEvent event);
    void menuGoogleCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    void completedAnimationSequenceNamed(const char *name);
    void menuHideCallback(CCObject* pSender, CCControlEvent event);
    
public:
    CCControlButton* m_pBtnMusic;
    CCControlButton* m_pBtnSound;
    
    CCControlButton* m_pBtnSupport;
    CCControlButton* m_pBtnAbout;
    
    CCControlButton* m_pBtnFB;
    
    CCSprite* m_pIconGmail;
    CCSprite* m_pIconGameCenter;
    
    CCLabelBMFont *m_googleLabel;
    CCControlButton *m_googleButton;
    CCSprite *m_googleGemSprite;
    
    CCNode *m_pAboutNode;
    CCBAnimationManager* m_pAnimationManager;
    
    CCLabelBMFont *m_pFBLabel;
};


#endif /* defined(__GameClient__SettingUI__) */
