//
//  LoadingUI.h
//  GameClient
//
//  Created by chowray on 13-5-10.
//
//

#ifndef __GameClient__LoadingUI__
#define __GameClient__LoadingUI__

#include "../coc/NotifyListener.h"
#include "CUINode.h"

using namespace cocos2d;
USING_NS_COC;

class LoadingUI : public CUINode, public NotifyListener
{
public:
    LoadingUI();
    virtual ~LoadingUI();
public:
    CREATE_FUNC(LoadingUI);
public:
	virtual bool onNotify(int notifyid, const void* pVal = NULL);
public:
    bool init();
    void refresh();
    void refreshLoadingState(const char* s);
    void refreshInfo(const char* s);
        
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);

public:
    
    CCLabelBMFont*  m_pTitle;
    CCLabelBMFont*  m_pTip;
    
    CCLabelBMFont*  m_pTestLog;
    
    CCLabelBMFont*  m_pVersion;
    
    CCScale9Sprite* m_pProgress;
    CCSprite *m_pBg;
};

#endif /* defined(__GameClient__LoadingUI__) */
