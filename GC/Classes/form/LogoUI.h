//
//  LogoUI.h
//  GameClient
//
//  Created by qsun on 13-10-30.
//
//

#ifndef __GameClient__LogoUI__
#define __GameClient__LogoUI__

#include <iostream>
#include "CUINode.h"

class LogoUI : public CUINode, public CCBAnimationManagerDelegate
{
public:
    LogoUI();
    virtual ~LogoUI();
public:
    CREATE_FUNC(LogoUI);
public:
    bool init();
    
    virtual void completedAnimationSequenceNamed(const char *name);
    
    CCBAnimationManager* m_pLogoAction;
    
    bool m_isActionEnd;
};

#endif /* defined(__GameClient__LogoUI__) */
