//
//  ClanWarBattleUI.h
//  GameClient
//
//  Created by qsun on 14-9-4.
//
//

#ifndef __GameClient__ClanWarBattleUI__
#define __GameClient__ClanWarBattleUI__

#include "CUINode.h"
#include "CUIExt.h"
#include "ClanWarBattleMapUI.h"
#include "../coc/NotifyListener.h"

class ClanWarAttackClanItem;

class ClanWarBattleUI : public CUINode, public NotifyListener, public CCTableViewDataSource, public CCTableViewDelegate{
    
public:
    ClanWarBattleUI(ClanCityInfo* pCityInfo);
    virtual ~ClanWarBattleUI();
    
    static ClanWarBattleUI* create(ClanCityInfo* pCityInfo);
    
    virtual bool onNotify(int notifyid, const void* pVal = NULL);
public:
    
    bool init();
    
    void onEnter();
    void onExit();
    
    void update(float dt);
    
    void refreshInfo();
    
    void selectStage(int index);
    
    void showBattleLog();
    void hideBattleLog();
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    void warAttackUser(int uid);
    
    void menuPrevPlayerCallback(CCObject* pSender, CCControlEvent event);
    void menuNextPlayerCallback(CCObject* pSender, CCControlEvent event);
    
    void menuLogInfoCallback(CCObject* pSender, CCControlEvent event);
    
    void menuCloseLogCallback(CCObject* pSender, CCControlEvent event);
    void menuAttackCallback(CCObject* pSender, CCControlEvent event);
    void menuVisitCallback(CCObject* pSender, CCControlEvent event);
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    CCSize cellSizeForTable(CCTableView *table);
    CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
    unsigned int numberOfCellsInTableView(CCTableView *table);
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view){};
    virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
    
public:
    
    ClanCityInfo* m_pCityInfo;
    
    CUINode* m_pBattleLogUI;
    
    CCLabelBMFont* m_pCityName;
    
    CCSprite* m_pAttackClanIcon;
    CCLabelTTF* m_pAttackClanName;
    
    CCSprite* m_pDefendClanIcon;
    CCLabelTTF* m_pDefendClanName;
    
    CCLabelBMFont* m_pWinStar;
    CCLabelBMFont* m_pAttackNums;
    
    CCScale9Sprite* m_pStarProgress;
    
    CCNode* m_pWinLine;
    
    CCLabelBMFont* m_pCanAttackNumsTip;
    CCLabelBMFont* m_pCanAttackNums;
    
    CCLabelBMFont* m_pBattleTimeTip;
    CCLabelBMFont* m_pBattleEndTime;
    
    CCNode* m_pLoadingUI;
    
    CCScrollView* m_pMapView;
    ClanWarBattleMapUI* m_pBattleMapUI;
    
    CCLayer* m_pAttackClanView;
    CCTableView* m_pAttackClanList;
    
    CCLayer* m_pBattleLogView;
    CCTableView* m_pBattleLogList;
    
    CCNode* m_pLootInfoNode;
    CCLabelBMFont* m_pRankingId;
    CCLabelTTF* m_pPlayerName;
    CCLabelBMFont* m_pLootGold;
    CCLabelBMFont* m_pLootOil;
    
    CCControlButton* m_pBtnPrevPlayer;
    CCControlButton* m_pBtnNextPlayer;
    CCControlButton* m_pBtnAttack;
    CCControlButton* m_pBtnVisit;

    CCControlButton* m_pBtnLog;
    CCControlButton* m_pBtnCloseLog;
        
    CCNode* m_pMapNode;
    
    int m_lastWarState;
    
    CCPoint m_dragOffset;
};

class ClanWarAttackClanItem : public CUINode {
    
public:
    
    ClanWarAttackClanItem(int rankingId, CCTableView* pTableView, ClanWarAttackClanInfo* pAttackClanInfo);
    ~ClanWarAttackClanItem();
    
    static ClanWarAttackClanItem* create(int rankingId, CCTableView* pTableView, ClanWarAttackClanInfo* pAttackClanInfo);
    
public:
    
    bool init();
    
    void onSelected(bool isSelect);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCTableView* m_pTableView;
    
    int m_rankingId;
    
    CCScale9Sprite* m_pClanItemBg;
    
    ClanWarAttackClanInfo* m_pAttackClanInfo;
    
    CCLabelBMFont* m_pRankingId;
    CCSprite* m_pClanIcon;
    
    CCLabelTTF* m_pClanName;
    CCLabelBMFont* m_pLastTimeTip;
    CCLabelBMFont* m_pLastStarTime;
    CCLabelBMFont* m_pWinStar;
    
    CCSprite* m_pWinTip;
};

class ClanWarBattleLogItem : public CUINode {
    
public:
    ClanWarBattleLogItem(FightMsg fightMsg, CCSize viewSize);
    ~ClanWarBattleLogItem();
    
    static ClanWarBattleLogItem* create(FightMsg fightMsg, CCSize viewSize);
    
public:
    
    bool init();
    
    void refreshInfo();
    
    bool checkTouchInside(CCObject* pSender);
    
    void menuReplayCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    FightMsg m_fightMsg;
    
    CCSize m_viewSize;
    
    CCSprite* m_pAttackPlayerMilitary;
    CCLabelTTF* m_pAttackPlayerName;
    CCSprite* m_pAttackClanIcon;
    CCLabelTTF* m_pAttackClanName;
    
    CCSprite* m_pDefendPlayerMilitary;
    CCLabelTTF* m_pDefendPlayerName;
    CCSprite* m_pDefendClanIcon;
    CCLabelTTF* m_pDefendClanName;
    
    CCSprite* m_pBattleStars;
    CCLabelBMFont* m_pBattleProgress;
    
    CCLabelBMFont* m_pBattleTime;
    
    CCNode* m_pBattleWinTip;
    CCNode* m_pBattleFailedTip;
    
    CCControlButton* m_pBtnReplay;
};

#endif /* defined(__GameClient__ClanWarBattleUI__) */
