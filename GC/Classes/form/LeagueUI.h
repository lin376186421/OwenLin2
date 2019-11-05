//
//  LeagueUI.h
//  GameClient
//
//  Created by Lee on 14-6-23.
//
//

#ifndef __GameClient__LeagueUI__
#define __GameClient__LeagueUI__

#include <iostream>
#include "../coc/BaseDef.h"
#include "../coc/NotifyListener.h"
#include "CUINode.h"
#include "ItemMenuUI.h"
#include "../coc/LeagueMgr.h"

//using namespace cocos2d;
USING_NS_COC;

class LeagueUI : public CUINode, public CCTableViewDataSource, public CCTableViewDelegate, public NotifyListener
{
public:
    LeagueUI();
    virtual ~LeagueUI();

    CREATE_FUNC(LeagueUI);
public:
    
    bool init();
    
    virtual bool onNotify(int notifyid, const void* pVal = NULL);
    
    void addPageNode(int nPageIndex);
    void refreshPage();
    void refreshHelpPage(int nIndex);

    void showGetAwardUI();
    void hideGetAwardUI();
    void update(float dt);
    
    void refreshLeagueInfo();
    void refreshLeaguePage1();
    void refreshLeaguePage0();
    void refreshBattleReward();

    void menuPageCallback(CCObject* pSender, CCControlEvent event);
    void menuHelpCallback(CCObject* pSender, CCControlEvent event);

    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    void menuBackCallback(CCObject* pSender, CCControlEvent event);
    void menuBtnFBShareCallback(CCObject* pSender, CCControlEvent event);

    void menuGetAwardCallback(CCObject* pSender, CCControlEvent event);
    
    void menuLeagueMatchCallback(CCObject* pSender, CCControlEvent event);
    void menuLeagueInfoCallback(CCObject* pSender, CCControlEvent event);

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
    
    CCControlButton* m_pBtnPages[3];
    CCControlButton* m_pBtnHelps[3];

    ItemMenuUI* m_pItemMenu;
    
    CCNode* m_pLeagueInfoUI;
    CUINode *m_pGetAwardUI;
    
    
    CCLabelBMFont *m_pTimeCount;
    CCLabelBMFont *m_pTimeDesLabel;
    
    CCControlButton* m_pBtnLeagueMatch;
    CCControlButton* m_pBtnBack;

    CCNode *m_pClearingNode;
    CCLabelBMFont *m_pWinNumLabel;
    CCLabelBMFont *m_pGroupInfoLabel;
    
    CCLabelBMFont *m_pCurMaxGoldLabel;
    CCLabelBMFont *m_pCurMaxOilLabel;
//    CCLabelBMFont *m_pNextMaxGoldLabel;
//    CCLabelBMFont *m_pNextMaxOilLabel;
    CCLabelBMFont *m_pNextTittle;

    CCLabelBMFont *m_pCurNameLabel;
    CCLabelBMFont *m_pNextNameLabel;
    
    CCLabelBMFont *m_pCurGroupLabel;
    
    CCNode *m_pEndLBNode;
    CCNode *m_pNextLBNode;
    CCNode *m_pClearingLBNode;
    
    CCLabelBMFont *m_pLeagueTipInfo;
    
    CCSprite *m_pCurIcon;
    CCSprite *m_pNextIcon;
    
    int m_endTime;
    
    bool m_isLeagueOk;
    int pageIndex;
    int m_helpIndex;

};


class LeaguePlayerItem: public CUINode {
    
public:
    LeaguePlayerItem(LeagueMgr::PlayerInfo info,CCSize size);
    virtual ~LeaguePlayerItem();
    
    static LeaguePlayerItem* create(LeagueMgr::PlayerInfo info,CCSize size);
public:
    
    bool init();
    
    void refresh();
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
    
    CCSize m_size;
    CCLabelBMFont *m_pRankingId;
    CCLabelBMFont *m_pPlayerLevel;
    CCLabelTTF    *m_pPlayerName;
    CCLabelBMFont *m_pWinNum;
    CCLabelBMFont *m_pStars;
    CCSprite      *m_pClanIcon;
    CCLabelTTF    *m_pClanName;
    CCSprite      *m_pRankFlag;
    CCSprite      *m_pLeagueLevelIcon;
    CCScale9Sprite* m_pItemBg;
    
    LeagueMgr::PlayerInfo m_info;
    
};
#endif /* defined(__GameClient__LeagueUI__) */
