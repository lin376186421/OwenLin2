#ifndef __FORM_SHOP_H__
#define __FORM_SHOP_H__

#include "../coc/NotifyListener.h"
#include "CUINode.h"

using namespace cocos2d;
USING_NS_COC;

class Shop : public CUINode, public CCTableViewDataSource, public CCTableViewDelegate, public NotifyListener
{
public:
	Shop(int id);
	virtual ~Shop();
public:
    // implement the "static create()" method manually
    static Shop* create(int id);
public:
	bool init();
public:
    
	virtual bool onNotify(int notifyid, const void* pVal = NULL);
public:
    
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    void menuBackCallback(CCObject* pSender, CCControlEvent event);
    void menuItemCallback(CCObject* pSender, CCControlEvent event);
    
    void showFreeGemsShop();
    
    void refreshMoney();
    
    void refreshShopItem(int idx);
        
    int getValue(int index);
        
    int getNums(int index);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view){};
    void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
    CCSize cellSizeForTable(CCTableView *table);
    CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
    unsigned int numberOfCellsInTableView(CCTableView *table);
    
    bool checkTouchInside(CCNode* pSender);
        
public:
    CCLabelBMFont*  m_pGoldNums;
    CCLabelBMFont*  m_pElixirNums;
    CCLabelBMFont*  m_pGemNums;
    
    CCNode* m_pShopNode;
    CCLayer* m_pShopView;
    
    CCScrollView* m_pScrollView;
    CCTableView* m_pTableView;
    
    CCNode* m_pShopItemNode;
    
    CCSize m_cellSize;
    
    bool m_isDrag;
    CCPoint m_dragOffset;
    int     m_shopId;
private:
    std::vector<int> m_shopList;
    
    bool m_isFreeGemOn;
    int m_nMaxItems;
};

#endif  // __FORM_SHOP_H__