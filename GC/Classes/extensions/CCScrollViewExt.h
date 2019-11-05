//
//  CCScrollViewExt.h
//  GameClient
//
//  Created by qsun on 13-9-18.
//
//

#ifndef __GameClient__CCScrollViewExt__
#define __GameClient__CCScrollViewExt__

#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

enum ScrollDirection {
    SCROLL_HORIZONTAL = 0,
    SCROLL_VERTICAL = 1,
    SCROLL_BOTH = 2,
    };

class CCScrollViewExt : public CCScrollView {
    
public:
    static void updateScrollView(CCScrollView* pScrollView, int col = 1, int dir = SCROLL_VERTICAL, CCPoint space=ccp(0, 0), CCPoint offset=ccp(0, 10));
    
    static void updateScrollView(CCScrollView* pScrollView, CCNode* pContainerLayer, int col = 1, int dir = SCROLL_VERTICAL, CCPoint space=ccp(0, 10), CCPoint offset=ccp(0, 0));
};

#endif /* defined(__GameClient__CCScrollViewEx__) */
