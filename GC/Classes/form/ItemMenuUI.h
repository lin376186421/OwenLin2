//
//  ItemMenuUI.h
//  GameClient
//
//  Created by qsun on 14-3-13.
//
//

#ifndef __GameClient__ItemMenuUI__
#define __GameClient__ItemMenuUI__

#include "CUINode.h"
#include "../coc/NotifyListener.h"
#include "../coc/BaseDef.h"

using namespace coc;

class ItemMenuUI : public CUINode
{
public:
    ItemMenuUI(CCLayer* pTargetView);
    virtual ~ItemMenuUI();
    
    static ItemMenuUI* create(CCLayer* pTargetView);
public:
    bool init();
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    
    void resetMenu(int uid, const char* uname, int clanid, int clanjob = -1, bool isManage = false);

    void setClanInfo(string clanname, int clanflag, int clantype, int clancup, int clanneedcup);
    
    void adjuestPosition();
    
    void setEnabled(bool bEnabled) { m_bEnabled = bEnabled; }
    bool isEnabled() { return m_bEnabled; }
    
    void menuVisitCallback(CCObject* pSender, CCControlEvent event);
    void menuInviteCallback(CCObject* pSender, CCControlEvent event);
    void menuPromoteCallback(CCObject* pSender, CCControlEvent event);
    void menuDemoteCallback(CCObject* pSender, CCControlEvent event);
    void menuKickOutCallback(CCObject* pSender, CCControlEvent event);
    void menuVisitClanCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    CCLabelTTF* m_pPlayerName;
    
    CCControlButton* m_pBtnMenu[6];
    float m_menuPosY[6];
    
    CCSprite* m_pMenuArrow;
    CCScale9Sprite* m_pMenuView;
    
    CCNode* m_pMenuNode;
    CCSize m_viewSize;
    
    CCLayer* m_pTargetView;
    
    int m_uid;
    string m_uname;
    
    int m_clanid;
    string m_clanname;
    int m_clanflag;
    int m_clantype;
    int m_clancup;
    int m_clanjob;
    int m_clanneedcup;
    
    bool m_bEnabled;
};


#endif /* defined(__GameClient__ItemMenuUI__) */
