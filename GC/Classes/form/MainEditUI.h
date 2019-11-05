//
//  MainEditUI.h
//  GameClient
//
//  Created by Li Chao Jun on 14-10-16.
//
//

#ifndef __GameClient__MainEditUI__
#define __GameClient__MainEditUI__

#include "Shop.h"
#include "../coc/SelectBuildingListener.h"
#include "../coc/NetListener.h"
#include "../coc/NotifyListener.h"
#include "CUINode.h"

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;
USING_NS_COC;

class EditBuilding;

class MainEditUI : public CUINode, public SelectBuildingListener, public CCScrollViewDelegate
{
public:
    MainEditUI();
    virtual ~ MainEditUI();
public:
    CREATE_FUNC(MainEditUI);
public:
	bool init();
    virtual void onExit();
    virtual void onEnter();
    
public:
    vector<CCControlButton*> m_lstBuildingBtn;
    
public:
    
    bool onNotify(int notifyid, const void* pVal);
    bool onSelect(bool isSelect, Building* pBuilding);
    
    void playerEffect(int i);
    void onRefurbishBuildingBtn();
    
    Building* buildWall();
    Building* buildBuilding(int buildingid, int lx, int ly);
    
    void setIsEraseModeOn(bool isOn);
    
    void onRemoveOther(UserBuildingInfo* pBuildingInfo);
    
    void onRemoveOtherOK(UserBuildingInfo* pBuildingInfo);
    
    void addBuilding2EditList(Building* pBuilding);
    void removeBuildingFromEditList(EditBuilding *pEditBuilding);
    void layoutListWithEditBuilding(EditBuilding *pEditBuilding);
    
    EditBuilding* getEditBuilding(int building);
    int getBuildingCount(int buildingid);
    
    void editSave();
    void editExit();
    
    void checkEditSaveEnable();
    
    void menuSaveCallback(CCObject* pSender, CCControlEvent event);
    void menuExitCallback(CCObject* pSender, CCControlEvent event);
    void menuRemoveAllCallback(CCObject* pSender, CCControlEvent event);
    void menuEraseModeCallback(CCObject* pSender, CCControlEvent event);
    
    void onSelectRow(CCObject* pSender, CCControlEvent event);
    void onSelectRotate(CCObject* pSender, CCControlEvent event);
    
    void menuRemoveCallback(CCObject* pSender, CCControlEvent event);
    void menuRemoveBuildingCallback(CCObject* pSender, CCControlEvent event);
    void menuFastCallback(CCObject* pSender, CCControlEvent event);
    
    void scrollViewDidScroll(CCScrollView* view);
    void scrollViewDidZoom(CCScrollView* view);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCSprite *m_pRemoveAllArrowSprite;
    CCLabelBMFont *m_pRemoveAllInfoLabel;
    CCLabelBMFont *m_pDragInfoLabel;
    CCLayer* m_pEditView;
    
    CCScrollView *m_pScrollView;
    CCLayer *m_pBuildingView;
    
    CCControlButton *m_pBtnEraseMode;
    CCControlButton *m_pBtnSave;
    CCControlButton *m_pBtnRemoveByGold;
    CCControlButton *m_pBtnRemoveByElixir;
    CCControlButton *m_pBtnRotate;
    CCControlButton *m_pBtnRow;
    CCControlButton *m_pBtnRemoveBuilding;
    CCControlButton *m_pBtnFast;
    
    bool m_isEraseMode;
    
	std::list<UserBuildingInfo*> m_lstBuildingSrc;
	std::list<UserBuildingInfo*> m_lstBuilding;
    
    CCArray *m_plistArray;
    
    bool m_isEdited;
};

#endif /* defined(__GameClient__MainEditUI__) */
