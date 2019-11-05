//
//  BarrackUI.h
//  coc4
//
//  Created by chowray on 13-3-27.
//
//

#ifndef __FORM_BARRACKUI_H__
#define __FORM_BARRACKUI_H__

#include "../coc/NotifyListener.h"
#include "../coc/BaseDef.h"
#include "CUINode.h"

using namespace cocos2d;
USING_NS_COC;

class SoldierItem;

class BarrackUI : public CUINode, public NotifyListener
{
public:
    BarrackUI();
    virtual ~BarrackUI();
public:
    CREATE_FUNC(BarrackUI);
public:
    bool init();
    
	virtual bool onNotify(int notifyid, const void* pVal = NULL);
//	virtual void draw();
public:
    // a selector callback
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    
    void menuPrevBarrackCallback(CCObject* pSender, CCControlEvent event);
    void menuNextBarrackCallback(CCObject* pSender, CCControlEvent event);

    void prevBarrack();
    void nextBarrack();
    
    void refreshQueue();
    void refreshBarrack(float fDelta);
    
    void menuFinishTrainCallback(CCObject* pSender, CCControlEvent event);
    
    void barrackMoveCallFunc();
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);

public:
    CCLabelBMFont* m_pTrainNums;
    CCLabelBMFont* m_pTotalTime;
    CCLabelBMFont* m_pTroopsSpace;
    CCNode* m_pTroopsFullTip;
    
    CCLayer* m_pSoldierView;
    CCLayer* m_pTrainView;
    
    CCNode* m_pTrainNode;
    
    CCLayer* m_pBackLayer;
    CCScale9Sprite* m_pMaskSprite;

    CCControlButton* m_pBtnPrev;
    CCControlButton* m_pBtnNext;
    
    CCControlButton* m_pBtnFinishTrain;
    
    CCPoint m_barrackPos;
    
public:
    int m_trainingnums;

    BarrackInfo* m_pBarrackInfo;
    Building* m_pBuilding;
    
    std::vector<SoldierItem*> m_lstSoldier;
};

class SoldierItem : public CUINode{
public:
    SoldierItem(int id, bool isTrain);
    virtual ~SoldierItem();
public:
    static SoldierItem* create(int id, bool isTrain);

public:
    bool init();
    
    void registerWithTouchDispatcher();
    
    void onEnter();
    void onExit();
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
    void menuInfoCallback(CCObject* pSender, CCControlEvent event);
    
    void onHoldTouchCallback(float t);
    
    bool addSoldierToQueue();
    bool deleteSoldier();
    
    void refresh();
    
    void setTrainState(CCControlState state);
    
    bool isTouchInBounds(CCPoint touchLocation);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    int m_soldierId;
    
    bool m_isTrain;
    bool m_isUnLocked;
    
    CCRect m_boundsRect;
    
    float m_fScheduleDelay;
    float m_fScheduleElapsed;
    int m_nScheduleTimes;
    void initScheduleTimer();

    BarrackUI* m_pBarrackUI;
    
    CCSprite* m_pTrainCost;
    CCLabelBMFont* m_pTrainCostNum;
    CCLabelBMFont* m_pUnLockTip;

    CCLabelBMFont* m_pTrainNums;
    CCSprite* m_pTrainProgress;
    CCSprite* m_pTrainProgressBg;
    CCLabelBMFont* m_pTrainTime;
    
    CCControlButton* m_pBtnInfo;
    CCControlButton* m_pBtnTrain;
    
    CCControlState m_state;
};

#endif // __FORM_BARRACKUI_H__
