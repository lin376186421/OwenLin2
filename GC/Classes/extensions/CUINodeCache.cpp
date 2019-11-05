//
//  CUINodeCache.cpp
//  GameClient
//
//  Created by qsun on 13-8-30.
//
//

#include "CUINodeCache.h"
#include "NGNKit.h"

static CUINodeCache* s_sharedUINodeCache;
CUINodeCache* CUINodeCache::sharedUINodeCache()
{
    if (s_sharedUINodeCache == NULL) {
        s_sharedUINodeCache = new CUINodeCache();
        s_sharedUINodeCache->init();
    }
    
    return s_sharedUINodeCache;
}

void CUINodeCache::init()
{
    m_uiNodeCache = CCDictionary::create();
    m_uiNodeCache->retain();
}

//CCNode* CUINodeCache::UINodeByName(const char *name)
//{
//    CCNode *node = (CCNode*)m_uiNodeCache->objectForKey(name);
//    return node;
//}
//
//CCNode* CUINodeCache::addUINodeFromFile(const char* fileName, CCObject* target)
//{
//    return this->addUINodeFromFile(fileName, target, fileName);
//}
//
//CCNode* CUINodeCache::addUINodeFromFile(const char *fileName, CCObject *target, const char *name, bool targetTouch)
//{
//    CCNode* node = (CCNode*)m_uiNodeCache->objectForKey(name);
//    
//    if (node == NULL) {
//        
//        node = UINodeFromFile(fileName, target, targetTouch);
//        m_uiNodeCache->setObject(node, name);
//    }
//    
//    return node;
//}

CCNode* CUINodeCache::UINodeFromFile(const char *fileName, CCNode *target, CCBAnimationManager **pAnimationManager)
{
    return this->UINodeFromFile(fileName, target, CCDirector::sharedDirector()->getWinSize(), pAnimationManager);
}

CCNode* CUINodeCache::UINodeFromFile(const char *fileName, CCNode *target, CCSize parentSize, CCBAnimationManager **pAnimationManager)
{
    string filePath(fileName);
    filePath.insert(0, "ccb/");
    
    CCNodeLoaderLibrary *nodeLoaderLibrary = CCNodeLoaderLibrary::newDefaultCCNodeLoaderLibrary();
    
    CCBReader *ccbReader = new CCBReader(nodeLoaderLibrary);
    ccbReader->setResolutionScale(getResolutionScale());

    CCNode* pNode = ccbReader->readNodeGraphFromFile(filePath.c_str(), target, parentSize);

    CCBAnimationManager* _animationManager = ccbReader->getAnimationManager();
    
    if (pAnimationManager != NULL) {
        *pAnimationManager = _animationManager;
//        (*pAnimationManager)->setDelegate((CCBAnimationManagerDelegate*)target);
        (*pAnimationManager)->retain();
    }
    
    ccbReader->release();
        
    return pNode;
}

void CUINodeCache::UINodeTextLocal(CCNode *node)
{
    
}

void CUINodeCache::removeAllNodes()
{
    
    CCArray* keys = m_uiNodeCache->allKeys();
    
    for (int index = 0; index < keys->count(); index ++) {
        
        const char* name = ((CCString*)keys->objectAtIndex(index))->getCString();
        removeNodeByName(name, true);
    }
}

void CUINodeCache::removeNodeByName(const char* name, bool clearFromParent)
{
    
    CCNode* node = (CCNode*)m_uiNodeCache->objectForKey(name);
    
    if (node && node->getParent()) {
        
        node->getParent()->removeChild(node, true);
    }
    
    m_uiNodeCache->removeObjectForKey(name);
}

void CUINodeCache::removeNode(CCNode* node, bool clearFromParent)
{
    if (node) {
        const char *uiName = "";
        CCArray* keys = m_uiNodeCache->allKeys();
        
        for (int index = 0; index < keys->count(); index ++) {
            const char* key = ((CCString*)keys->objectAtIndex(index))->getCString();
            
            CCNode* uiNode = (CCNode*)m_uiNodeCache->objectForKey(key);
            if (uiNode == node) {
                uiName = key;
                break;
            }
        }
        
        removeNodeByName(uiName, clearFromParent);
    }
}

CUINodeCache::~CUINodeCache()
{
    removeAllNodes();
    m_uiNodeCache->release();
}

