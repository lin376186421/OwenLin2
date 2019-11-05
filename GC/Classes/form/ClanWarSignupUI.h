//
//  ClanWarSignupUI.h
//  GameClient
//
//  Created by qsun on 14-9-4.
//
//

#ifndef __GameClient__ClanWarSignupUI__
#define __GameClient__ClanWarSignupUI__

#include "CUINode.h"
#include "../coc/BaseDef.h"
#include "../coc/NotifyListener.h"

USING_NS_COC;

class ClanWarSignupUI : public CUINode, public NotifyListener, public CCTableViewDataSource, public CCTableViewDelegate {
    
public:
    ClanWarSignupUI(ClanCityInfo* m_pCityInfo);
    virtual ~ClanWarSignupUI();
    
    static ClanWarSignupUI* create(ClanCityInfo* info);
    
    virtual bool onNotify(int notifyid, const void* pVal = NULL);
public:
    
    bool init();
    
    void update(float dt);
    
    void refreshInfo();
    
    void menuSignupCallback(CCObject* pSender, CCControlEvent event);
    void menuClanInfoCallback(CCObject* pSender, CCControlEvent event);
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    
    CCSize cellSizeForTable(CCTableView *table);
    CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
    unsigned int numberOfCellsInTableView(CCTableView *table);
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view){};
    virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCNode* m_pLoading;
    
    ClanCityInfo* m_pCityInfo;
    
    CCLabelBMFont* m_pCityName;
    
    CCSprite* m_pClanIcon;
    CCLabelTTF* m_pClanName;
    CCSprite* m_pClanNational;
    CCLabelBMFont* m_pRewardGems;
    
    CCLabelBMFont* m_pSignupEndTime;
    CCLabelBMFont* m_pSignupNums;
    
    CCLabelBMFont* m_pWinTip;
    CCLabelBMFont* m_pWinStar;
    
    CCControlButton* m_pBtnSignup;
    
    CCLayer* m_pView;    
    CCTableView* m_pSignupClansView;
};

class ClanWarSignupItem : public CUINode
{
public:
    ClanWarSignupItem(ClanInfo clanInfo, CCTableView* pTableView);
    virtual ~ClanWarSignupItem();
    
    static ClanWarSignupItem* create(ClanInfo clanInfo, CCTableView* pTableView);
    
public:
    
    bool init();
    
    void refreshInfo();
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCTableView* m_pTableView;
    
    ClanInfo m_clanInfo;
    
    CCSprite* m_pClanIcon;
    CCLabelTTF* m_pClanName;
    CCSprite* m_pClanNational;
    CCLabelBMFont* m_pClanMemberNums;
    CCLabelBMFont* m_pClanCup;
    
    CCLabelBMFont* m_pSignupTime;
};

#endif /* defined(__GameClient__ClanWarSignupUI__) */
