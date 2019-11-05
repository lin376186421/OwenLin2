//
//  MessageUI.h
//  coc4
//
//  Created by chowray on 13-4-9.
//
//

#ifndef __coc4__MessageUI__
#define __coc4__MessageUI__

#include "../coc/BaseDef.h"
#include "../coc/NotifyListener.h"
#include "CUINode.h"

using namespace cocos2d;
USING_NS_COC;

class MessageUI : public CUINode, public NotifyListener, public CTextInputFieldDelegate
{
public:
    MessageUI();
    virtual ~MessageUI();
public:
    CREATE_FUNC(MessageUI);
public:
    bool init();
    
    void onEnter();
    void onExit();
    
    virtual bool onNotify(int notifyid, const void* pVal = NULL);
    
public:
    // a selector callback
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    // a selector callback
    void menuOKCallback(CCObject* pSender, CCControlEvent event);

    void menuGemCallback(CCObject* pSender, CCControlEvent event);
    
    void menuFreeGemCallback(CCObject* pSender, CCControlEvent event);
    
    void reload();
    void refreshMsg();
    
    void scheduleUpdate(float ot);
        
    void setObject(void* pObj) { m_pObject = pObj; }

    static MessageUI* show(int msgid, const char* title, const char* message);
    static MessageUI* show(int msgid, const char* title, const char* message, int type);
    static MessageUI* show(int msgid, const char* title, const char* message, int value, int type);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    void textInputFieldTextChanged(CTextInputField* TextInputField, const std::string& text);
    void textInputFieldReturn(CTextInputField* TextInputField){};
public:
    
    int m_messageId;
    int m_value;
    int m_type;
    
    int m_ticker;
    
    CCLabelBMFont* m_pTitle;
    CCLabelTTF* m_pMessage;
    CCControlButton* m_pBtnOK;
    CCControlButton* m_pBtnCancel;
    CCControlButton* m_pBtnClose;
    
    CCLabelBMFont* m_pGem;
    CCControlButton* m_pBtnGem;
    CCControlButton* m_pBtnFreeGem;
    
    CCScale9Sprite* m_pInputBg;
    CTextInputField* m_pInput;
    CCLabelBMFont* m_pPlayerLevel;
    CCLabelTTF* m_pPlayerName;
    
    CCLabelBMFont* m_pTime;
    
    CCNode* m_pView;
    
    Building* m_pCurBuilding;
    Building* m_pLastBuilding;
    
    CCBAnimationManager* m_pAnimationManager;
    
    void* m_pObject;
    bool m_isBaseMsg;
};

#endif /* defined(__coc4__MessageUI__) */
