//
//  CUINode.h
//  GameClient
//
//  Created by qsun on 13-8-30.
//
//

#ifndef __GameClient__CUINode__
#define __GameClient__CUINode__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CUINodeCache.h"
#include "CUIExt.h"
#include "CTextInputField.h"
#include "CRectLayer.h"
#include "NGNKit.h"

using namespace cocos2d;
using namespace cocos2d::extension;

#define CCB_MEMBERVARIABLEASSIGNER_BIND(TARGET, MEMBERVARIABLENAME, MEMBERVARIABLETYPE, MEMBERVARIABLE) \
if (pTarget == TARGET && 0 == strcmp(pMemberVariableName, (MEMBERVARIABLENAME))) { \
MEMBERVARIABLE = NULL;\
MEMBERVARIABLETYPE pOldVar = MEMBERVARIABLE; \
MEMBERVARIABLE = dynamic_cast<MEMBERVARIABLETYPE>(pNode); \
CC_ASSERT(MEMBERVARIABLE); \
if (pOldVar != MEMBERVARIABLE) { \
CC_SAFE_RELEASE(pOldVar); \
} \
return true; \
}

class CUINode : public CCLayer, public CCBMemberVariableAssigner, public CCBSelectorResolver {
public:
    
	CUINode();
	~CUINode();
    
	bool init();
public:
    
    void onEnter();
    void onExit();
    
    static CUINode* create(void);
    
    static CUINode* create(CCSize contentSize);
    static CUINode* createWithFile(const char *fileName, CCNode* pTarget, CCBAnimationManager **pAnimationManager = NULL);
    static CUINode* createWithFile(const char *fileName, CCNode* pTarget, CCSize parentSize, CCBAnimationManager**pAnimationManager = NULL);
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
    void addChild(CCNode* child);
    void addChild(CCNode* child, int zOrder);
    void addChild(CCNode* child, int zOrder, int tag);
    
    bool isTouchInside(CCTouch* touch, CCRect rect);
    bool isTouchInside(CCPoint touchLocation, CCRect rect);
    bool isTouchInside(CCTouch* touch, CCNode* pNode = NULL);
    bool isTouchInside(CCPoint touchLocation, CCNode* pNode = NULL);
    
    void registerWithTouchDispatcher();
    void registerAllChildTouchDispatcher(CCNode *pNode);
    
    void setTargetTouch(bool targetTouch);
    void setTouchPriorityId(int touchPriorityId);
    void setTouchPriority(int touchPriority);
    void setTouchControlView(CCLayer* uiNode);
    
    void tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_MenuHandler onResolveCCBCCMenuItemSelector(CCObject * pTarget, const char* pSelectorName);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    int m_touchPriority;
    int m_touchPriorityId;
    CCNode *getContainer();
    void setContainer(CCNode *pNode);
    
private:
    bool m_targetTouch;
    
    int m_isTouchDispatcher;

    CCLayer* m_touchControlView;
    
    CCNode *m_pContainer;
};

#endif /* defined(__GameClient__CUINode__) */
