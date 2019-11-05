//
//  CRectLayer.h
//  GameClient
//
//  Created by qsun on 13-12-9.
//
//

#ifndef __GameClient__CRectLayer__
#define __GameClient__CRectLayer__

#include "cocos2d.h"

USING_NS_CC;

class CRectLayer : public CCLayer {
    
public:
    
    static CRectLayer* create(CCSize size, bool bClipToBounds = true);
    
    bool initWithSize(CCSize size, bool bClipToBounds = true);
    
    /**
     * clip this view so that outside of the visible bounds can be hidden.
     */
    void beforeDraw();
    /**
     * retract what's done in beforeDraw so that there's no side effect to
     * other nodes.
     */
    void afterDraw();
    
    virtual void visit();
    
public:
    bool m_bClipToBounds;
    CCSize m_tViewSize;
    
    bool m_bScissorRestored;
    CCRect m_tParentScissorRect;
    
    CCRect getViewRect();
};

#endif /* defined(__GameClient__CRectLayer__) */
