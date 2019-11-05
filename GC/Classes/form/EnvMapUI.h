//
//  EnvMapUI.h
//  GameClient
//
//  Created by qsun on 14-3-20.
//
//

#ifndef __GameClient__EnvMapUI__
#define __GameClient__EnvMapUI__

#include "CUINode.h"

const int SMALL_STAGE_NUMS = 5;
const int HARD_STAGE = 1000;

struct EnvMapNode{
    int type;
    int stage;
    CCPoint pos;
    int flag;
};

class EnvMapUI : public CUINode {
    
public:
    EnvMapUI();
    virtual ~EnvMapUI();
public:
    CREATE_FUNC(EnvMapUI);
public:
    bool init();
    
    void onEnter();
    void onExit();
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    void update(float dt);
    void draw();
    
    void drawStage(EnvMapNode mapNode);
    
    CCRect getViewRect();

    bool isInViewRect(CCNode* pNode);
    
    void viewDidScroll();
    
public:
    
    void saveMap();
    void loadMap();
    
    void onSelectStage(EnvMapNode stageNode);
    
    void menuAttackCallback(CCObject* pSender, CCControlEvent event);
    void menuVideoCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
    
    CCSprite* m_pStage;
    CCSprite* m_pPoint;
    
    CCNode* m_pStageStar;
    CCLabelBMFont* m_pStageName;
    
    vector<EnvMapNode> m_mapNodeVec;
    vector<EnvMapNode> m_stageNodeVec;
    
    CCScrollView* m_pView;
    
    CCNode* m_pCurLevel;
    CCControlButton* m_pBtnAttack;
    CCControlButton* m_pBtnVideo;

    CCPoint m_viewPos;
    CCRect m_viewRect;
    
    CCNode* m_pMapBgItem;
    CCPoint m_mapBgItemPos[6];
    
    int m_openStage;
    EnvMapNode m_curStage;
};

#endif /* defined(__GameClient__EnvMapUI__) */
