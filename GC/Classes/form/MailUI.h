//
//  MailUI.h
//  coc4
//
//  Created by chowray on 13-4-3.
//
//

#ifndef __coc4__MailUI__
#define __coc4__MailUI__

#include "../coc/NotifyListener.h"
#include "CUINode.h"
#include "ItemMenuUI.h"

using namespace cocos2d;
USING_NS_COC;

class MailUI : public CUINode, public CCTableViewDataSource, public CCTableViewDelegate, public NotifyListener
{
public:
    MailUI();
    virtual ~MailUI();
public:
    CREATE_FUNC(MailUI);
	virtual bool onNotify(int notifyid, const void* pVal = NULL);
public:
    bool init();
public:
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    
    void refreshPage();
    
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
    
    vector<FightMsg> m_lstLog;
        
    CCNode* m_pLoading;
    
    CCControlButton* m_pBtnPages[3];
    CCControlButton* m_pBtnClose;
    
    CCLayer* m_pView;
	CCTableView* m_pTableView;
    
    ItemMenuUI* m_pItemMenu;
    
    CCLabelBMFont* m_pNoTip;
    
    bool m_isDrag;
    CCPoint m_dragOffset;
};

class BattleLogItem : public CUINode, public CCScrollViewDelegate{
    
public:
    BattleLogItem(FightMsg msg);
    virtual ~BattleLogItem();

    static BattleLogItem* create(FightMsg msg);
public:
    bool init();
public:
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    
    bool checkTouchInside(CCObject* pSender);
    
    void refresh();
    
    void menuReplayCallback(CCObject* pSender, CCControlEvent event);
    void menuRevengeCallback(CCObject* pSender, CCControlEvent event);
    void menuShareCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view) {}
    
public:
    
    FightMsg m_fightMsg;
    
    CCLabelBMFont* m_pBattleTime;
    CCLabelTTF* m_pPlayerName;
    CCLabelBMFont* m_pPlayerCup;
    CCLabelBMFont* m_pGainGold;
    CCLabelBMFont* m_pGainElixir;
    CCLabelBMFont* m_pGainCup;
    CCLabelBMFont* m_pBattleProgress;
    CCLabelBMFont* m_pBattleResult;
    CCSprite* m_pIconMilitary;
    
    CCLabelTTF* m_pClanName;
    CCSprite* m_pClanIcon;
    
    CCLabelBMFont* m_pNotReplay;
    CCLabelBMFont* m_pNotRevenge;
    
    CCNode* m_pStars;
    
    CCScrollView* m_pTroopsView;
    
    CCScale9Sprite* m_pBattleResultBg;
    CCScale9Sprite* m_pLogBg;
    
    CCControlButton* m_pBtnReplay;
    CCControlButton* m_pBtnRevenge;
    CCControlButton* m_pBtnShare;
    
    bool m_isDrag;
    CCPoint m_dragOffset;
};

class MailItem : public CUINode {
    
public:
    MailItem(int index);
    virtual ~MailItem();
    
    static MailItem* create(int index);
public:
    bool init();
public:
    
    void refresh();
    
    void menuGetAwardsCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    int m_index;
    
    CCLabelTTF* m_pFromName;
    CCLabelTTF* m_pTitle;
    CCLabelTTF* m_pMessage;
    
    CCSprite* m_pEarnAwads[3];
    
    CCLabelBMFont* m_pTime;
    
    CCControlButton* m_pBtnGetAwards;
};

#endif /* defined(__coc4__MailUI__) */
