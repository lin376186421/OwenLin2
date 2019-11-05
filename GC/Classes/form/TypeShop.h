#ifndef __FORM_TYPESHOP_H__
#define __FORM_TYPESHOP_H__

#include "../coc/NotifyListener.h"
#include "CUINode.h"

using namespace cocos2d;
USING_NS_COC;

class TypeShop : public CUINode, public NotifyListener
{
public:
	TypeShop();
	virtual ~TypeShop();
public:
    // implement the "static create()" method manually
    CREATE_FUNC(TypeShop);
public:
	bool init();
public:
	virtual bool onNotify(int notifyid, const void* pVal = NULL);
public:
    // a selector callback
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    // a selector callback
    void menuBackCallback(CCObject* pSender, CCControlEvent event);

    // a selector callback
    void menuShopCallback1(CCObject* pSender, CCControlEvent event);
    // a selector callback
    void menuShopCallback2(CCObject* pSender, CCControlEvent event);
    // a selector callback
    void menuShopCallback3(CCObject* pSender, CCControlEvent event);
    // a selector callback
    void menuShopCallback4(CCObject* pSender, CCControlEvent event);
    // a selector callback
    void menuShopCallback5(CCObject* pSender, CCControlEvent event);
    // a selector callback
    void menuShopCallback6(CCObject* pSender, CCControlEvent event);
    // a selector callback
    void refreshMoney();
    void guide();
        
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
    CCLabelBMFont*  m_pGoldNums;
    CCLabelBMFont*  m_pElixirNums;
    CCLabelBMFont*  m_pGemNums;
    
    CCNode* m_pShopMenu;
    
    CCControlButton* m_pBtnClose;
};

#endif  // __FORM_TYPESHOP_H__