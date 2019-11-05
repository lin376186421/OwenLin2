//
//  HeroUI.h
//  GameClient
//
//  Created by chowray on 13-5-21.
//
//

#ifndef __GameClient__HeroUI__
#define __GameClient__HeroUI__

#include "../coc/NotifyListener.h"
#include "CUINode.h"

using namespace cocos2d;
USING_NS_COC;

class HeroUI : public CUINode, public NotifyListener
{
public:
    HeroUI();
    virtual ~HeroUI();
public:
    CREATE_FUNC(HeroUI);
	virtual bool onNotify(int notifyid, const void* pVal = NULL);
public:
    bool init();
    
    void onEnter();
public:
    // a selector callback
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    // a selector callback
    void menuChangeCallback(CCObject* pSender, CCControlEvent event);
    // a selector callback
    void menuRefreshCallback(CCObject* pSender, CCControlEvent event);
    
    void menuSkillCallback(CCObject* pSender, CCControlEvent event);
    
    void menuUpgradeCallback(CCObject* pSender, CCControlEvent event);
    
    
    void refresh();
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCLabelBMFont* m_pLevel;
    
//    CCNode* m_pHeroAtt1;
    CCNode* m_pHeroAtt2;
    CCNode* m_pHeroAtt3;
    
    CCSprite* m_pSkill1;
    CCSprite* m_pSkill2;
    CCSprite* m_pSkill3;
    CCSprite* m_pSkill4;
    
    CCControlButton* m_pBtnChange;
    CCControlButton* m_pBtnRefresh;
        
    CCSprite* m_pHeroIcon;
    
    CCLabelBMFont* m_pSkillName;
    CCLabelBMFont* m_pSkillDesc;
    CCLabelBMFont* m_pGlyphName;
    CCLabelBMFont* m_pGlyphDesc;
    
    CCNode* m_pUnUpgradeTip;
    CCNode* m_pUpgradeTip;
    
    CCLabelBMFont* m_pUpgradeTime;
    CCControlButton* m_pBtnUpgrade;
    
    CCNode* m_pGlyphInfoNode;
    CCNode* m_pSkillInfoNode;
    
    CCNode *m_pUpgradeItemNode;
    
    CCLabelBMFont *m_pHeroInfoLabel;

    int m_selectSkill;
//    bool m_bMaxLevel;
    
    bool m_isHeroUplevel;
};

#endif /* defined(__GameClient__HeroUI__) */
