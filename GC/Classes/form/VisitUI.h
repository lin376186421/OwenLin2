//
//  VisitUI.h
//  GameClient
//
//  Created by chowray on 13-5-28.
//
//

#ifndef __GameClient__VisitUI__
#define __GameClient__VisitUI__

#include "../coc/SelectBuildingListener.h"
#include "CUINode.h"

using namespace cocos2d;
USING_NS_COC;

class VisitUI : public CUINode, public SelectBuildingListener
{
public:
	VisitUI();
	virtual ~VisitUI();
public:
    // implement the "static create()" method manually
    CREATE_FUNC(VisitUI);
public:
	bool init();
    void menuBackCallback(CCObject* pSender, CCControlEvent event);
    void menuInfoCallback(CCObject* pSender, CCControlEvent event);
    void menuRankingCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    virtual bool onSelect(bool isSelect, Building* pBuilding);
    void onRefurbishBuildingBtn();
    
    CCControlButton* m_pBtnInfo;
    
    CCLabelTTF* m_pName;
    CCLabelBMFont*  m_pLevel;
    CCLabelBMFont*  m_pCup;
        
    CCLabelBMFont*  m_pGoldCurNums;
    CCLabelBMFont*  m_pGoldMaxNums;
    CCLabelBMFont*  m_pElixirCurNums;
    CCLabelBMFont*  m_pElixirMaxNums;
    CCLabelBMFont*  m_pGemNums;
    CCLabelBMFont*  m_pWorkerNums;
    
    CCScale9Sprite* m_pProgressGold;
    CCScale9Sprite* m_pProgressElixir;
    
    CCSprite* m_pExpProgress;
    CCProgressTimer* m_pExpProgressTimer;
    
    CCSprite *m_pIconMilitary;
    CCSprite *m_pMiletaryName;
    
    vector<CCControlButton*> m_lstBuildingBtn;
};

#endif /* defined(__GameClient__VisitUI__) */
