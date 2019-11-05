//
//  CUINode.cpp
//  GameClient
//
//  Created by qsun on 13-8-30.
//
//

#include "CUINode.h"
#include "NGNKit.h"

#include "../coc/PlayerScene.h"
#include "GameWorldScene.h"
USING_NS_COC;

const int TOUCH_PRIORITY_SPACE = 10000;
static int BASE_TOUCH_PRIORITY = 0;

map<int, int> touchPriorityIdMap;
CUINode::CUINode()
{
    if(PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pAnnouncementUI)
    {
        PlayerScene::getSingleton().m_pGameWorld->hideAnnouncementUI();
    }
    
}

CUINode::~CUINode()
{
    
}

CUINode *CUINode::create()
{
    CUINode *pRet = new CUINode();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

CUINode* CUINode::create(CCSize contentSize)
{
    CUINode* node = CUINode::create();
    node->setContentSize(contentSize);
    return node;
}

CUINode* CUINode::createWithFile(const char *fileName, CCNode* pTarget, CCBAnimationManager **pAnimationManager)
{
    CUINode* pUINode = CUINode::create();
    
    CCNode* pNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile(fileName, pTarget, pAnimationManager);
    pUINode->setContentSize(pNode->getContentSize());
    pUINode->addChild(pNode);
    pUINode->setContainer(pNode);
    return pUINode;
}

CUINode* CUINode::createWithFile(const char *fileName, CCNode* pTarget, CCSize parentSize, CCBAnimationManager**pAnimationManager)
{
    CUINode* pUINode = CUINode::create();
    CCNode* pNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile(fileName, pTarget, parentSize, pAnimationManager);
    pUINode->setContentSize(pNode->getContentSize());
    pUINode->addChild(pNode);
    pUINode->setContainer(pNode);

    return pUINode;
}

bool CUINode::init()
{
    m_touchControlView = NULL;
    m_pContainer = NULL;
    m_targetTouch = false;
    
    m_isTouchDispatcher = false;
    
    return true;
}

void CUINode::onEnter()
{
    CCLayer::onEnter();
    
    for (CCNode *c = this->m_pParent; c != NULL; c = c->getParent())
    {
        if (c->isRunning() == false && isKindOfClass(c, CUINode))
        {
            return;
        }
    }
    
    int touchPriorityId = 0;
    if (touchPriorityIdMap.size() == 0) {
        touchPriorityId = BASE_TOUCH_PRIORITY;
    }else{
        map<int, int>::iterator it = (--touchPriorityIdMap.end());
        touchPriorityId = it->first + 1;
    }
    
    if (!m_touchControlView) {
        
        m_touchPriorityId = touchPriorityId;
        
        m_isTouchDispatcher = true;
        m_touchPriority = BASE_TOUCH_PRIORITY - touchPriorityId * TOUCH_PRIORITY_SPACE;
        
        touchPriorityIdMap.insert(pair<int, int>(m_touchPriorityId, -1));
        
        this->setTouchPriority(m_touchPriority--);
    }
    else {
        this->setTouchPriority(m_touchControlView->getTouchPriority()+1);
    }
    
    this->registerAllChildTouchDispatcher(this);
}

void CUINode::onExit()
{
    if (m_isTouchDispatcher) {
        touchPriorityIdMap.erase(m_touchPriorityId);
    }
    CCLayer::onExit();
}


void CUINode::setContainer(cocos2d::CCNode *pNode)
{
    m_pContainer = pNode;
}

CCNode *CUINode::getContainer()
{
    return m_pContainer;
}

bool CUINode::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    if (!this->isVisible()) {
        return false;
    }
    if(isTouchInside(pTouch))
    {
        return true;
    }
    return false;
}

void CUINode::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

void CUINode::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

void CUINode::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

void CUINode::addChild(CCNode *child, int zOrder, int tag)
{
    if (this->isRunning()) {
        registerAllChildTouchDispatcher(child);
    }
    
    CCLayer::addChild(child, zOrder, tag);
}

void CUINode::addChild(CCNode *child, int zOrder)
{
    this->addChild(child, zOrder, child->getTag());
}

void CUINode::addChild(CCNode *child)
{
    this->addChild(child, child->getZOrder(), child->getTag());
}

bool CUINode::isTouchInside(CCTouch* touch, CCRect rect)
{
    CCPoint touchLocation = touch->getLocation(); // Get the touch position
    touchLocation = this->getParent()->convertToNodeSpace(touchLocation);
    return rect.containsPoint(touchLocation);
}

bool CUINode::isTouchInside(CCPoint touchLocation, CCRect rect)
{
    touchLocation = this->getParent()->convertToNodeSpace(touchLocation);
    return rect.containsPoint(touchLocation);
}

bool CUINode::isTouchInside(CCTouch* touch, CCNode* pNode)
{
    CCPoint touchLocation = touch->getLocation();
    
    CCRect bBox;
    if (pNode == NULL) {
        touchLocation = this->getParent()->convertToNodeSpace(touchLocation);
        bBox = this->boundingBox();
    }
    else {
        
        if (!pNode->isVisible()) {
            return false;
        }
        
        touchLocation = pNode->getParent()->convertToNodeSpace(touchLocation);
        bBox = pNode->boundingBox();
    }
    return bBox.containsPoint(touchLocation);
}

bool CUINode::isTouchInside(CCPoint touchLocation, CCNode* pNode)
{
    CCRect bBox;
    if (pNode == NULL) {
        touchLocation = this->getParent()->convertToNodeSpace(touchLocation);
        bBox = this->boundingBox();
    }
    else {
        
        if (!pNode->isVisible()) {
            return false;
        }
        
        touchLocation = pNode->getParent()->convertToNodeSpace(touchLocation);
        bBox = pNode->boundingBox();
    }
    
    return bBox.containsPoint(touchLocation);
}

void CUINode::setTargetTouch(bool targetTouch)
{
    m_targetTouch = targetTouch;
    if (targetTouch) {
        this->setTouchEnabled(true);
    }
    else{
        this->setTouchEnabled(false);
    }
}

void CUINode::setTouchPriorityId(int touchPriorityId)
{
    if (m_isTouchDispatcher) {
        touchPriorityIdMap.erase(m_touchPriorityId);
    }
    m_touchPriorityId = touchPriorityId;
    
    if (!m_touchControlView) {
        m_touchPriority = BASE_TOUCH_PRIORITY - touchPriorityId * TOUCH_PRIORITY_SPACE;
    }
}

void CUINode::setTouchPriority(int touchPriority)
{
    CCLayer::setTouchPriority(touchPriority);

    m_touchPriority = touchPriority;
}

void CUINode::setTouchControlView(CCLayer *uiNode)
{
    m_touchControlView = uiNode;
}

void CUINode::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, getTouchPriority(), m_targetTouch);
}

void CUINode::registerAllChildTouchDispatcher(CCNode *pNode)
{
    int count = pNode->getChildren() ? pNode->getChildren()->count() : 0;
    for (int index = 0; index < count; index++) {
        CCNode* pChildren = (CCNode*)pNode->getChildren()->objectAtIndex(index);
        
        if (isKindOfClass(pChildren, CUINode)) {
            if (((CUINode*)pChildren)->m_touchControlView) {
                ((CUINode*)pChildren)->setTouchPriority(((CUINode*)pChildren)->m_touchControlView->getTouchPriority()+1);
                ((CUINode*)pChildren)->registerAllChildTouchDispatcher(pChildren);
                continue;
            }
        }
        
        if (isKindOfClass(pChildren, CCLayer)) {
            
            if (m_touchControlView) {
                ((CCLayer*)pChildren)->setTouchPriority(m_touchControlView->getTouchPriority()+1);
            }else{
                ((CCLayer*)pChildren)->setTouchPriority(m_touchPriority--);
            }
        }
        
        int childCount = pChildren->getChildren() ? pChildren->getChildren()->count() : 0;
        if (childCount > 0) {
            registerAllChildTouchDispatcher(pChildren);
        }
        
        if (isKindOfClass(pChildren, CCScrollView)) {  //move ScrollView to end
            ((CCLayer*)pChildren)->setTouchPriority(m_touchPriority--);
        }
    }
}

void CUINode::tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell)
{
    cell->removeAllChildren();
}

bool CUINode::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    return true;
}

SEL_MenuHandler CUINode::onResolveCCBCCMenuItemSelector(CCObject * pTarget, const char* pSelectorName)
{
    return NULL;
}

SEL_CCControlHandler CUINode::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    return NULL;
}


