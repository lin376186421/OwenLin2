//
//  CreateUI.h
//  GameClient
//
//  Created by chowray on 13-5-13.
//
//

#ifndef __GameClient__CreateUI__
#define __GameClient__CreateUI__

#include "CUINode.h"

using namespace cocos2d;

class CreateUI : public CUINode
{
public:
    CreateUI();
    virtual ~CreateUI();
public:
    CREATE_FUNC(CreateUI);
public:
    bool init();
    void menuOKCallback(CCObject* pSender, CCControlEvent cce);
    void menuBoyCallback(CCObject* pSender, CCControlEvent cce);
    void menuGirlCallback(CCObject* pSender, CCControlEvent cce);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
    CCScale9Sprite* m_pInputBg;
    
    CCControlButton* m_pBtnHeroBoy;
    CCControlButton* m_pBtnHeroGirl;
    
    CCControlButton* m_pBtnOK;
    
    CCSprite* m_pHero;
    CTextInputField* m_pInput;
    
    CCNode* m_pView;
    
    int     m_sex;  //0 男 1 女
};

#endif /* defined(__GameClient__CreateUI__) */
