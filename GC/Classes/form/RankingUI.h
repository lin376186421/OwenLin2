//
//  RankingUI.h
//  GameClient
//
//  Created by chowray on 13-5-14.
//
//

#ifndef __GameClient__RankingUI__
#define __GameClient__RankingUI__

#include "../coc/BaseDef.h"
#include "../coc/NotifyListener.h"
#include "CUINode.h"
#include "ClanInfoUI.h"
#include "ItemMenuUI.h"
#include "../coc/LeagueMgr.h"

//using namespace cocos2d;
USING_NS_COC;

class RankingUI : public CUINode, public CCTableViewDataSource, public CCTableViewDelegate, public NotifyListener
{
public:
    RankingUI();
    virtual ~RankingUI();
public:
    CREATE_FUNC(RankingUI);
	virtual bool onNotify(int notifyid, const void* pVal = NULL);
public:
    
    bool init();
    
    void addPageNode(int nPageIndex);
    void refreshPage();
    
    void menuBackCallback(cocos2d::CCObject *pSender, CCControlEvent event);
    void menuPageCallback(CCObject* pSender, CCControlEvent event);
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    virtual void scrollViewDidScroll(CCScrollView* view) {};
    virtual void scrollViewDidZoom(CCScrollView* view) {}
    void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
    CCSize cellSizeForTable(CCTableView *table);
    CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
    unsigned int numberOfCellsInTableView(CCTableView *table);
    
public:
    CCNode* m_pLoading;
    
    CCNode* m_pPageNode;
    
    CCLayer* m_pView;
	CCTableView* m_pTableView;
    CCScrollView* m_pScrollView;

    CCControlButton* m_pBtnPages[2];
    
    ItemMenuUI* m_pItemMenu;
};

class PlayerItem: public CUINode {
    
public:
    PlayerItem(RankUserInfo info, int rankIndex, CCSize size);
    virtual ~PlayerItem();
    
    static PlayerItem* create(RankUserInfo info, int rankIndex, CCSize size);
public:
    
    bool init();
    
    void refresh();
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
    
    CCSize m_size;
    
    CCSprite* m_pRankingIcon;
    CCLabelBMFont* m_pRankingId;
    CCLabelTTF* m_pPlayerName;
    CCLabelBMFont* m_pPlayerLevel;
    CCLabelBMFont* m_pPlayerCup;
    CCLabelBMFont* m_pDetailLabel;
    
    CCSprite* m_pClanIcon;
    CCLabelTTF* m_pClanName;
    
    CCControlButton* m_pBtnLookup;
    
    CCScale9Sprite* m_pItemBg;
    CCSprite* m_pRankingChangeTip;
    CCLabelBMFont* m_pRankingChangeNum;
    CCSprite *m_pIconMilitary;
    
    RankUserInfo m_info;
    
    int m_rankIndex;
};


#endif /* defined(__GameClient__RankingUI__) */
