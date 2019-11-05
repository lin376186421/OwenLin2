//
//  FBLoginUI.h
//  GameClient
//
//  Created by chowray on 13-9-4.
//
//

#ifndef __GameClient__FBLoginUI__
#define __GameClient__FBLoginUI__

#include "CUINode.h"

#include "../coc/NotifyListener.h"

using namespace cocos2d;

class FBLoginUI : public CUINode, public coc::NotifyListener
{
public:
	FBLoginUI(int state);
	virtual ~FBLoginUI();
public:
//    CREATE_FUNC(FBLoginUI);
    static FBLoginUI* create(int state);
public:
	bool init();
    virtual bool onNotify(int notifyid, const void* pVal = NULL);
    void refreshInfo();
    
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    void menuFBLoginCallback(CCObject* pSender, CCControlEvent event);
    void menuFBSwithCallback(CCObject* pSender, CCControlEvent event);
   
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCControlButton* m_pBtnClose;
    CCControlButton* m_pBtnFBLogin;
    CCControlButton* m_pBtnFBSwith;
    CCNode *m_pLoginNode;
    CCNode *m_pSwithNode;
    CCLabelTTF *m_pUserName;
    CCNode *m_pCCBNode;
    CCSprite *m_pGemTip;
    int m_state;
    
};


#endif /* defined(__GameClient__FBLoginUI__) */
