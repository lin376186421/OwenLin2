//
//  AchievementUI.h
//  GameClient
//
//  Created by chowray on 13-5-3.
//
//

#ifndef __GameClient__AchievementUI__
#define __GameClient__AchievementUI__

#include "../coc/NotifyListener.h"
#include "CUINode.h"

using namespace cocos2d;
USING_NS_COC;

class AchievementUI : public CUINode, public CCTableViewDataSource, public CCTableViewDelegate, public NotifyListener
{
public:
    AchievementUI();
    virtual ~AchievementUI();
public:
    CREATE_FUNC(AchievementUI);
public:
    bool init();
public:
    
    void refresh();
    
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    void menuGameCenterCallback(CCObject* pSender, CCControlEvent event);
    bool onNotify(int notifyid, const void* pVal = NULL);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    virtual void scrollViewDidScroll(CCScrollView* view) {};
    virtual void scrollViewDidZoom(CCScrollView* view) {}
    void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
    CCSize cellSizeForTable(CCTableView *table);
    CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
    unsigned int numberOfCellsInTableView(CCTableView *table);
public:
    CCLabelBMFont* m_pCompleted;
    CCLayer* m_pView;
    
    CCNode* m_pGameCenterNode;
    
    CCTableView* m_pTableView;
    
    CCNode* m_pLoading;
        
    vector<int> m_lstAchievement;
};

class AchievementItem : public CUINode{

public:
    static AchievementItem* create(int id);
    
    AchievementItem(int id);
    virtual ~AchievementItem();
public:
    bool init();
    
    void refresh();
    
    void getRewardsCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    int m_cellIndex;
    
    CCSprite* m_pCompleteStars;
    CCLabelBMFont* m_pName;
    CCLabelBMFont* m_pDesc;
    
    CCScale9Sprite* m_pProgressCompletedBg;
    CCScale9Sprite* m_pProgressCompleted;
    CCSize m_proCompletedSize;
    CCLabelBMFont* m_pCompleted;
    CCLabelBMFont* m_pRewardsExp;
    CCLabelBMFont* m_pRewardsGems;
    
    CCLabelBMFont* m_pCompletedTip;
    
    CCNode* m_pStatusNode;
    
    CCControlButton* m_pGetReward;
    
    CCTableView* m_pTableView;
    
private:
    int m_achievementId;
};

#endif /* defined(__GameClient__AchievementUI__) */
