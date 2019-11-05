//
//  SkillInfoUI.h
//  GameClient
//
//  Created by qsun on 13-11-5.
//
//

#ifndef __GameClient__SkillInfoUI__
#define __GameClient__SkillInfoUI__

#include "CUINode.h"

using namespace cocos2d;

class SkillInfoUI : public CUINode , public CCBAnimationManagerDelegate{
public:
    SkillInfoUI(int id, bool isTrain);
    virtual ~SkillInfoUI();
public:
    static SkillInfoUI* create(int id, bool isTrain);
public:
    bool init();
    
    void refresh();
    
    void menuTrainCallback(CCObject* pSender, CCControlEvent event);
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    void completedAnimationSequenceNamed(const char *name);
    
public:
    CCLabelBMFont* m_pName;
    CCLabelBMFont* m_pLevel;
    CCLabelBMFont* m_pMaxLevel;
    CCLabelBMFont* m_pExp;
    CCLabelBMFont* m_pCoolDown;
    CCLabelBMFont* m_pDuration;
    CCLabelBMFont* m_pDesc;
    CCLabelBMFont* m_pUpgradeDesc;
    
    CCScale9Sprite* m_pExpProgress;
    
    CCSprite* m_pIcon;
    
    CCControlButton* m_pBtnLeap;
    CCControlButton* m_pBtnTrain;
    
    CCLabelBMFont* m_pUnUpgradeTip;
    CCLabelBMFont* m_pUnLockTip;
    
    CCControlButton *m_pBtnBack;
    
    CCNode* m_pUnUpgradeNode;
    
    CCNode* m_pInfoNode;
    CCNode* m_pUpgradeNode;
    
    CCNode* m_pTrainNode;
    CCBAnimationManager* m_pTrainAnimation;
    
    int m_skillId;
    bool m_isTrain;
    
    int m_trainGems;
    int m_leapGems;
};

#endif /* defined(__GameClient__SkillInfoUI__) */
