//
//  ClanEditUI.h
//  GameClient
//
//  Created by qsun on 14-3-13.
//
//

#ifndef __GameClient__ClanEditUI__
#define __GameClient__ClanEditUI__

#include "CUINode.h"
#include "ClanInfoUI.h"
#include "../coc/NotifyListener.h"
#include "../coc/BaseDef.h"

using namespace coc;

class ClanEditUI : public CUINode, public NotifyListener, public CCScrollViewDelegate
{
public:
    ClanEditUI();
    virtual ~ClanEditUI();

    CREATE_FUNC(ClanEditUI);
    
    virtual bool onNotify(int notifyid, const void* pVal = NULL);
public:
    bool init();
    
    void showIconEditUI();
    void hideIconEditUI();
    
    void showNationalEditUI();
    void hideNationalEditUI();
    
    void menuBrowseIconCallback(CCObject* pSender, CCControlEvent event);
    void menuBrowseNationalCallback(CCObject* pSender, CCControlEvent event);
    void menuIconCallback(CCObject* pSender, CCControlEvent event);
    void menuNationalCallback(CCObject* pSender, CCControlEvent event);
    void menuFrontTypeCallback(CCObject* pSender, CCControlEvent event);
    void menuNextTypeCallback(CCObject* pSender, CCControlEvent event);
    void menuFrontTrophiesCallback(CCObject* pSender, CCControlEvent event);
    void menuNextTrophiesCallback(CCObject* pSender, CCControlEvent event);
    
    void menuCreateCallback(CCObject* pSender, CCControlEvent event);
    void menuEditCallback(CCObject *pSender, CCControlEvent event);
    
    void menuBackCallback(CCObject* pSender, CCControlEvent event);
    
    void scrollViewDidScroll(CCScrollView* view);
    void scrollViewDidZoom(CCScrollView* view) {};
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CUINode* m_pWaitLoading;
    
    CCNode *m_pClanEditBg;
    
    CCSprite* m_pClanIcon;
    CCSprite* m_pClanNational;
    CCLabelBMFont* m_pJoinType;
    CCLabelBMFont* m_pRequiredTrophies;
    
    CCLabelBMFont* m_pNationalName;
    
    CCScale9Sprite* m_pFieldNameBg;
    CCScale9Sprite* m_pFieldDescBg;
    CTextInputField* m_pFieldName;
    CTextInputField* m_pFieldDesc;
    
    CCControlButton* m_pBtnBack;
    CCControlButton* m_pBtnEdit;
    CCControlButton* m_pBtnCreate;
    
    CCNode* m_pIconEditUI;
    CCScrollView* m_pIconListView;
    
    CCNode* m_pNationalEditUI;
    CCScrollView* m_pNationalListView;
    
    CCNode* m_pEditUI;
    
    int m_joinType;
    int m_selectIcon;
    int m_selectNational;
    int m_requiredTrophies;
    
    CCPoint m_dragOffset;
    bool m_isDrag;
};

#endif /* defined(__GameClient__ClanCreateUI__) */
