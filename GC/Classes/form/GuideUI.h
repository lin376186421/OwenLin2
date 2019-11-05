//
//  GuideUI.h
//  GameClient
//
//  Created by chowray on 13-6-27.
//
//

#ifndef __GameClient__GuideUI__
#define __GameClient__GuideUI__

#include "CUINode.h"
#include "../coc/GuideMgr.h"

using namespace cocos2d;

class GuideUI : public CUINode, public CCBAnimationManagerDelegate
{
public:
	GuideUI();
	virtual ~GuideUI();
public:
    // implement the "static create()" method manually
    CREATE_FUNC(GuideUI);
public:
	bool init();
    
    void setChat();
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    void menuOKCallback(CCObject* pSender, CCControlEvent cce);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    void completedAnimationSequenceNamed(const char *name);
public:
    CCLabelBMFont* m_pTip;
    CCControlButton* m_pBtnOK;
    
    CCBAnimationManager* m_pAnimationManager;
    
    GuideItemInfo* m_info;

    bool isEnd;
};

#endif /* defined(__GameClient__GuideUI__) */
