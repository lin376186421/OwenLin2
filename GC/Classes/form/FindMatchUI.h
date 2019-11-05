//
//  FindMatchUI.h
//  coc4
//
//  Created by chowray on 13-4-3.
//
//

#ifndef __coc4__FindMatchUI__
#define __coc4__FindMatchUI__

#include "CUINode.h"
#include "EnvMapUI.h"
#include "../coc/NotifyListener.h"

using namespace cocos2d;
using namespace coc;

class FindMatchUI : public CUINode, public NotifyListener, public CCScrollViewDelegate
{
public:
    FindMatchUI();
    virtual ~FindMatchUI();
public:
    CREATE_FUNC(FindMatchUI);
    
    virtual bool onNotify(int notifyid, const void* pVal = NULL);
public:
    bool init();

    void refresh();
    void timeUpdate(float dt);
    
    void addFullLife();
    void onSelectStage(int stage);
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    void menuAddLifeCallback(CCObject* pSender, CCControlEvent event);
    void menuFindCallback(CCObject* pSender, CCControlEvent event);
    void menuLeagueCallback(CCObject* pSender, CCControlEvent event);

    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view){};
    
public:
    
    CCScrollView* m_pMapView;
    
    EnvMapUI* m_pEnvMapUI;
    
    CCLabelBMFont* m_pStar;
    CCLabelBMFont* m_pLife;
    CCLabelBMFont* m_pLifeTime;
    
    CCLabelBMFont* m_pRefreshTime;
    
    CCNode* m_pLifeNode;
    CCControlButton* m_pBtnAddLife;
    
    CCNode* m_pLootInfo;
    CCLabelBMFont* m_pLootGold;
    CCLabelBMFont* m_pLootOil;
    
    CCControlButton* m_pBtnFind;
    CCControlButton* m_pBtnLeague;

    bool m_isDrag;
    CCPoint m_dragOffset;
};

#endif /* defined(__coc4__FindMatchUI__) */
