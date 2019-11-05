//
//  ClanInfoUI.h
//  GameClient
//
//  Created by qsun on 14-3-13.
//
//

#ifndef __GameClient__ClanInfoUI__
#define __GameClient__ClanInfoUI__

#include "CUINode.h"
#include "ClanEditUI.h"
#include "ItemMenuUI.h"
#include "../coc/NotifyListener.h"
#include "../coc/BaseDef.h"

using namespace coc;

const int CLAN_TROPHIES_MAX   = 3600;
const std::string ClanType[3] = {"Anyone Can Join", "Invite only", "Closed"};
const std::string ClanJob[4] = {"Member", "First Sergeant", "XO", "Commander"};

class ClanInfoUI : public CUINode, public NotifyListener, public CCTableViewDataSource, public CCTableViewDelegate
{
public:
    ClanInfoUI(ClanInfo clanInfo);
    virtual ~ClanInfoUI();
public:
    static ClanInfoUI* create(ClanInfo clanInfo);
    
    virtual bool onNotify(int notifyid, const void* pVal);
    
public:
    bool init();
    
    void showClanEditUI();
    void hideClanEditUI();
    
    void refreshInfo();
    
    void menuJoinCallback(CCObject* pSender, CCControlEvent event);
    void menuEditCallback(CCObject* pSender, CCControlEvent event);
    void menuLeaveCallback(CCObject* pSender, CCControlEvent event);
    void menuBackCallback(CCObject* pSender, CCControlEvent event);
    
    CCSize cellSizeForTable(CCTableView *table);
    CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
    unsigned int numberOfCellsInTableView(CCTableView *table);
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view){};
    virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCNode *m_pClanInfoBg;
    
    CCSprite* m_pClanNational;
    CCLabelBMFont* m_pNationalName;
    
    CCSprite* m_pClanIcon;
    CCLabelTTF* m_pClanName;
    CCLabelTTF* m_pClanDesc;
    CCLabelBMFont* m_pClanCup;
    CCLabelBMFont* m_pClanMembers;
    CCLabelBMFont* m_pJoinType;
    CCLabelBMFont* m_pRequiredTrophies;
    
    CCControlButton* m_pBtnJoin;
    CCControlButton* m_pBtnEdit;
    CCControlButton* m_pBtnLeave;
    CCControlButton* m_pBtnBack;
    
    CCTableView* m_pMembersView;
    CUINode* m_pClanEditUI;
    
    CCLayer* m_pView;
    
    ItemMenuUI* m_pItemMenu;
    
    ClanInfo m_info;
};


class ClanMemberItem : public CUINode
{
public:
    ClanMemberItem(CCScrollView* pScrollView, ClanMember member);
    virtual ~ClanMemberItem();
    
    static ClanMemberItem* create(CCScrollView* pScrollView, ClanMember member);
public:
    bool init();
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCScrollView* m_pScrollView;
    
    CCLabelBMFont*      m_pRankingId;
    CCLabelBMFont*      m_pLevel;
    CCLabelTTF*         m_pName;
    CCLabelBMFont*      m_pCup;
    CCLabelBMFont*      m_pClanPower;
    CCLabelBMFont*      m_pTroopsDonated;
    CCLabelBMFont*      m_pTroopsReceived;
    CCSprite*           m_pRankingChangeTip;
    CCLabelBMFont*      m_pRankingChangeNum;
    
    CCSprite*           m_pMilitaryIcon;
    CCSprite*           m_pWarInvalid;
    
    CCSprite*           m_pOnLineIcon;
    
    CCScale9Sprite*     m_pItemBg;
        
    ClanMember m_info;
};

#endif /* defined(__GameClient__ClanInfoUI__) */
