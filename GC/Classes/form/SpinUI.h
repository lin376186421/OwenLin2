//
//  SpinUI.h
//  GameClient
//
//  Created by Lee  on 13-12-27.
//
//

#ifndef __GameClient__SpinUI__
#define __GameClient__SpinUI__

#include <iostream>

#include "../coc/NotifyListener.h"
#include "CUINode.h"

using namespace cocos2d;
USING_NS_COC;

class SpinUI : public CUINode, public NotifyListener,public CCBAnimationManagerDelegate
{
public:
    SpinUI();
    virtual ~SpinUI();
    CREATE_FUNC(SpinUI);
    bool onNotify(int notifyid, const void* pVal = NULL);
    bool init();
    virtual void onExit();
    void update(float dt);
    void refresh(const void* pVal);

    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    void completedAnimationSequenceNamed(const char *name);
    void menuSpinCallback(CCObject* pSender, CCControlEvent event);
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);

    CCNode* m_pSpinUICCB;
    CCNode *m_pLoading;
    CCBAnimationManager* m_pAnimationManager;

    CCNode *m_pMainNode;
    CCControlButton *m_pSpinBtn;
    CCControlButton *m_pCloseBtn;

    CCLabelBMFont *m_pTBonusLabel;
    CCLabelBMFont *m_pCostLabel;
    
    //奖池总奖金
    int m_nTBonus;
    int m_nCost;
    
    int m_nGetAwardIndex;
    int m_nLastAwardIndex;
    int m_nAwardSpriteState;

    CCSprite *m_pLastAwardSprite;
    
    //run
    void beginRun();
    void runEnd();
    void runOut();
    void runIn(bool bIsBeginRun);
    void playSound();
    void cmpBonus();
    void flyEnd(CCNode *pSender);
    void fly();
    float   m_fSpeed;
    float   m_fCurRotation;
    int     m_nLastRotation;
    int     m_nEndRotation;

    //light
    CCArray *m_pLights;
    CCSprite *m_pLastLight;
    int  m_nLightIndex;
    void updateLightAction(float dt);
    void initLights();
    
    
    float m_fSoundDt;
    
};



#endif /* defined(__GameClient__SpinUI__) */
