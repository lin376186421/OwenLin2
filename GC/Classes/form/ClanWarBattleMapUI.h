//
//  ClanWarBattleMapUI.h
//  GameClient
//
//  Created by qsun on 14-9-11.
//
//

#ifndef __GameClient__ClanWarBattleMapUI__
#define __GameClient__ClanWarBattleMapUI__

#include "CUINode.h"
#include "CUIExt.h"
#include "../coc/BaseDef.h"

USING_NS_COC;

class ClanWarBattleMapUI : public CUINode{
    
public:
    ClanWarBattleMapUI();
    virtual ~ClanWarBattleMapUI();
    
    CREATE_FUNC(ClanWarBattleMapUI);
public:
    
    bool init();
    
    void refreshMap(int warCityType);
    
    void onEnter();
    void onExit();
    
    void saveMap();
    void loadMap();
    
    void onSelectStage(int index);
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    void draw();
    
    void drawMapNode(int index);
    
    CCRect getViewRect();
    bool isInViewRect(CCNode* pNode);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCScrollView* m_pView;
    
    CCPoint m_viewPos;
    CCRect m_viewRect;
    
    CCPoint m_dragOffset;
    
    CCSize m_mapNodeSize;
    
    int m_mapNodeNums;
    vector<CCPoint> m_mapNodePosVec;
    
    vector<CCNode*> m_mapNodeVec;
    
    vector<ClanWarUserInfo> m_warDefenseUserList;
    int m_warCityType;
    
    CCSprite* m_pAttackTarget;
    
    int m_selectIndex;
};

class ClanWarBattleMapNode : public CUINode {
    
public:
    
    ClanWarBattleMapNode(int index, int type, ClanWarUserInfo userInfo);
    ~ClanWarBattleMapNode();
    
    static ClanWarBattleMapNode* create(int index, int type, ClanWarUserInfo userInfo);
    
public:
    
    bool init();
    
    void refreshInfo();
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    int m_index;
    int m_type;
    ClanWarUserInfo m_userInfo;
    
    CCLabelBMFont* m_pRankingId;
    CCSprite* m_pTownHall;
    CCLabelTTF* m_pPlayerName;
    CCSprite* m_pDestroyStar;
    
    CCSprite* m_pDestroyStarProgress;
    CCLabelBMFont* m_pDestroyStarNums;
    CCLabelBMFont* m_pDefenseLevel;
};

#endif /* defined(__GameClient__ClanWarBattleMapUI__) */
