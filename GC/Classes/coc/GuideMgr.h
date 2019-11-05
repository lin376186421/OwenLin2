//
//  GuideMgr.h
//  GameClient
//
//  Created by chowray on 13-6-6.
//
//

#ifndef __GameClient__GuideMgr__
#define __GameClient__GuideMgr__

#include "BaseDef.h"
#include "cocos-ext.h"
#include "CUINode.h"
#include "../form/MainUI.h"
#include "../form/MessageUI.h"
#include "../form/EnvMapUI.h"
#include "BaseCSV.h"
#include "CAppPlatform.h"

NS_COC_BEGIN

enum GUIDE_TYPE {
    GUIDE_NEW_WELCOME = 0,
    GUIDE_BUILD_CANNON = 1,
    GUIDE_DEFEND_TOWNHALL = 2,
    GUIDE_ATTACK_GOBLIN = 3,
    GUIDE_ADD_BUILDER = 4,
    GUIDE_BUILD_ELIXIR_COLLECT = 5,
    GUIDE_BUILD_ELIXIR_STORAGE = 6,
    GUIDE_BUILD_GOLD_STORAGE = 7,
    GUIDE_BUILD_BARRACK = 8,
    GUIDE_TRAIN_SOLDIER = 9,
    GUIDE_RAVEN_GOBLIN = 10,
    GUIDE_MAKE_NAME = 11,
    GUIDE_UPGRADE_TOWNHALL = 12,
    GUIDE_ACHIEVEMENT = 13,
    
    GUIDE_HERO_SKILL = 20,
    
    GUIDE_FINAL_HINT = 99,
    GUIDE_END = 999,
};

struct GuideItemInfo {
        
    string file;
    string animIn;
    string animOut;
    string tip;
};

class GuideMgr : public CCLayer, public BaseCSV
{
public:
    GuideMgr();
    ~GuideMgr();
    
public:
    static GuideMgr& getSingleton();
    
    bool init();
    
    void setVal(int val);
    
	virtual void onLoadEnd();
    
    void registerWithTouchDispatcher();
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
    int getGuideVer()
    {        
        CCUserDefault::sharedUserDefault()->setIntegerForKey("GUIDE_VER", 2);
        CCUserDefault::sharedUserDefault()->flush();
        return 2;
    }
    
    void check(int i1, bool isInit);
    
    void refresh(int i1, int i2);
    bool isEquVal(int i1);
    bool isEquVal(int i1, int i2);
    
    void reset();
    
    void next();
    void hideArrow();
    
    void refreshArrowPos();
    
    void setMainUIButton(MainUI* pMainUI, bool isVisible);
    
    void addGuideUI(CCNode* pGuideUI);
    void removeGuideUI(CCNode* pGuideUI);
    void removeGuideUI();
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
  
	bool isGuideEnd() { return val1 == 999; }
    bool isGuiding() { return  val1 != 999; }
    
    GuideItemInfo* getGuideItemInfo(string key);
public:
    int val1;
    int val2;
    
    CCNode* m_pArrow;
    CCNode* m_pFinger;
    
    CCNode* m_pActiveBtn;
    Building* m_pActiveBuilding;
    MessageUI* m_pMessageUI;
    
    CCBAnimationManager* m_pArrowAnimation;
    
    vector<CCControlButton*> m_btnLst;
    
    CCNode* m_pGuideUI;
    
    bool m_isGuideEnd;
    
    bool m_isInitOk;
    
    float scaleY;
    
    std::map<string, GuideItemInfo> m_map;
};

NS_COC_END

#endif /* defined(__GameClient__GuideMgr__) */
