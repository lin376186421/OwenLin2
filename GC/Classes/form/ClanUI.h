//
//  ClanUI.h
//  GameClient
//
//  Created by chowray on 13-11-27.
//
//

#ifndef __GameClient__ClanUI__
#define __GameClient__ClanUI__

#include "CUINode.h"
#include "ClanInfoUI.h"
#include "ClanEditUI.h"
#include "ItemMenuUI.h"
#include "../coc/NotifyListener.h"
#include "../coc/BaseDef.h"

USING_NS_COC;

class ClanUI : public CUINode, public NotifyListener, public CCTableViewDataSource, public CCTableViewDelegate
{
public:
    ClanUI();
    virtual ~ClanUI();
public:
    CREATE_FUNC(ClanUI);
public:
    bool init();
    
    virtual bool onNotify(int notifyid, const void* pVal = NULL);
    
    void showClanEditUI();
    void hideClanEditUI();
    
    void refreshPage();
        
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    void menuPageCallback(CCObject* pSender, CCControlEvent event);
        
    void menuSearchCallback(CCObject* pSender, CCControlEvent event);
    
    void menuBackCallback(CCObject* pSender, CCControlEvent event);
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

    CCLayer* m_pView;
    
    ClanInfoUI* m_pClanInfoUI;
    ClanEditUI* m_pClanEditUI;
    
    CCControlButton* m_pBtnCreate;
    CCControlButton* m_pBtnEdit;
    
    CCControlButton* m_pBtnSearch;
    CCControlButton* m_pBtnBack;
    
    CCScale9Sprite* m_pFieldSearchNameBg;
    CTextInputField* m_pFieldSearchName;
    
    CCLayer* m_pListView;
    CCLayer* m_pSearchListView;
    CCLayer* m_pMembersView;
        
    CCNode* m_pLoading;
    
//    ClanInfo m_curClan;
    CCNode* m_pPagesUI[4];
    CCControlButton* m_pBtnPages[4];
        
    bool m_isDrag;
    CCPoint m_dragOffset;
    
    int m_pageIndex;
};

class ClanItem : public CUINode
{
public:
    ClanItem(CCScrollView* pScrollView, ClanInfo clanInfo);
    virtual ~ClanItem();
    
    static ClanItem* create(CCScrollView* pScrollView, ClanInfo clanInfo);
public:
    bool init();
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCScrollView* m_pScrollView;
    
    CCLabelTTF*     m_pClanName;
    CCLabelBMFont*  m_pJoinType;
    CCLabelBMFont*  m_pClanCup;
    CCLabelBMFont*  m_pClanMemberNums;
    CCSprite*       m_pClanIcon;
    CCSprite*       m_pClanNational;
    
    ClanInfo m_info;
};

#endif /* defined(__GameClient__ClanUI__) */
