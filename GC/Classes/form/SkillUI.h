//
//  SkillXZ.h
//  GameClient
//
//  Created by chowray on 13-9-26.
//
//

#ifndef __GameClient__SkillXZ__
#define __GameClient__SkillXZ__

#include "../coc/NotifyListener.h"
#include "CUINode.h"

using namespace cocos2d;
USING_NS_COC;

class SkillItem;

class SkillUI : public CUINode, public NotifyListener
{
public:
    SkillUI();
    virtual ~SkillUI();
public:
    CREATE_FUNC(SkillUI);
    bool onNotify(int notifyid, const void* pVal = NULL);
public:
    bool init();
public:

    void refresh();
    
    void selectSkill(int skill);
    
    void showSkillInfoUI(int skillId, bool isTrain);
    void hideSkillInfoUI();
        
    void menuBackCallback(CCObject* pSender, CCControlEvent event);
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
    CCControlButton* m_pBtnSkill[3];
    
    CCLayer* m_pItemView;
    
    std::vector<SkillItem*> m_skillList;
    
    CCLabelBMFont* m_pTrainGems;
    CCLabelBMFont* m_pLeapGems;
    
    CCControlButton* m_pBtnTrain;
    CCControlButton* m_pBtnLeap;
    
    CCControlButton* m_pBtnBack;
    CCControlButton* m_pBtnClose;
    
    int m_selectSkill;
    int m_listNums;

    CUINode* m_pSkillInfoUI;
};

class SkillItem : public CUINode {
    
public:
    SkillItem(int skillId);
    virtual ~SkillItem();
public:
    static SkillItem* create(int skillId);
public:
    bool init();
    
    void refresh();
    
    void menuInfoCallback(CCObject* pSender, CCControlEvent event);
    void menuTrainCallback(CCObject* pSender, CCControlEvent event);
    void menuSkillCallback(CCObject* pSender, CCControlEvent event);
public:
    
    int m_skillId;
    
    CCLabelBMFont* m_pSkillName;
    CCLabelBMFont* m_pSkillLevel;
    
    CCControlButton* m_pBtnSkill;
    CCControlButton* m_pBtnTrain;
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
};

#endif /* defined(__GameClient__SkillXZ__) */
