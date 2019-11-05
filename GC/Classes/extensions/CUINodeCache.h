//
//  CUINodeCache.h
//  GameClient
//
//  Created by qsun on 13-8-30.
//
//

#ifndef __GameClient__CUINodeCache__
#define __GameClient__CUINodeCache__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CUINode.h"

#define ALERT_TYPE_CONNECT_SERVER 0
#define ALERT_TYPE_MESSAGE1 1
#define ALERT_TYPE_MESSAGE2 2
#define ALERT_TYPE_MESSAGE3 3

#define LOADING_NODE_TAG 10001

#define ALERT_TYPE_RATE 4

#define ALERT_CONNECT_NAME "NetworkConnectAlert"
#define ALERT_MESSAGE_NAME "MessageAlert"

#define ALERT_MESSAGE_TITLE_TAG 0
#define ALERT_MESSAGE_TEXT_TAG 1
#define ALERT_BUTTON_CLOSE_TAG 2
#define ALERT_BUTTON_OK_TAG 3
#define ALERT_LOADING_TAG 11

using namespace std;
using namespace cocos2d;
using namespace cocos2d::extension;

class CUINode;
class CUINodeCache : public CCObject{
    
protected:
    CCDictionary *m_uiNodeCache;
public:
    
    static CUINodeCache* sharedUINodeCache();
    
    ~CUINodeCache();
    
    void init();
    
//    CCNode* UINodeByName(const char* name);
//    CCNode* addUINodeFromFile(const char* fileName, CCObject* target);
//    CCNode* addUINodeFromFile(const char* fileName, CCObject* target, const char* name, bool targetTouch = false);
    
    CCNode* UINodeFromFile(const char *fileName, CCNode *target, CCBAnimationManager **pAnimationManager = NULL);
    CCNode* UINodeFromFile(const char *fileName, CCNode* target, CCSize parentSize, CCBAnimationManager**pAnimationManager = NULL);
    
    void addUINode(CCNode *node, const char* name);
    
    void removeNodeByName(const char* name, bool clearFromParent);
    void removeNode(CCNode* node, bool clearFromParent);
    void removeAllNodes(void);
    
    void UINodeTextLocal(CCNode* node);
};

#endif /* defined(__GameClient__CUINodeCache__) */
