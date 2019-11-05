//
//  FBShareUI.h
//  GameClient
//
//  Created by chowray on 13-9-4.
//
//

#ifndef __GameClient__FBShareUI__
#define __GameClient__FBShareUI__

#include "CUINode.h"

#include "../coc/NotifyListener.h"

using namespace cocos2d;

class FBShareUI : public CUINode
{
public:
	FBShareUI();
	virtual ~FBShareUI();
public:
    CREATE_FUNC(FBShareUI);
//    static FBShareUI* create(int state);
public:
	bool init();
    
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    void menuFBShareCallback(CCObject* pSender, CCControlEvent event);
   
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCNode *m_pTipNode;
    CCScale9Sprite* m_pDescInputBg;
    CTextInputField* m_pDescField;
    
};


#endif /* defined(__GameClient__FBShareUI__) */
