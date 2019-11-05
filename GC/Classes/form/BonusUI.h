//
//  BonusUI.h
//  GameClient
//
//  Created by chowray on 13-9-4.
//
//

#ifndef __GameClient__BonusUI__
#define __GameClient__BonusUI__

#include "CUINode.h"
#include "../coc/ConfigValMgr.h"


using namespace cocos2d;

class BonusUI : public CUINode, public CCTableViewDataSource, public CCTableViewDelegate
{
public:
	BonusUI();
	virtual ~BonusUI();
public:
    CREATE_FUNC(BonusUI);
public:
	bool init();
    
    void refreshInfo();
    
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    
    virtual void scrollViewDidScroll(CCScrollView* view) {};
    virtual void scrollViewDidZoom(CCScrollView* view) {};
    void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
    CCSize cellSizeForTable(CCTableView *table);
    CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
    unsigned int numberOfCellsInTableView(CCTableView *table);

   
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCLayer* m_pItemView;
    
    vector<BonusItemInfo> m_bonusList;
    
};

class BonusItem : public CUINode {
    
public:
    static BonusItem* create(int id);
    
    BonusItem(int id);
    ~BonusItem();
    
public:
    bool init();
    void refreshInfo();
    
    void menuEarnCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
    
    CCSprite* m_pIcon;
    CCSprite* m_pEarnTypeIcon;

    CCLabelBMFont* m_pName;
    CCLabelBMFont* m_pDesc;
    CCLabelBMFont* m_pNums;
    CCLabelBMFont* m_pGems;
    
    CCControlButton* m_pBtnEarn;
    
    int m_bonusId;
};

#endif /* defined(__GameClient__BonusUI__) */
