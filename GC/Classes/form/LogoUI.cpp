//
//  LogoUI.cpp
//  GameClient
//
//  Created by qsun on 13-10-30.
//
//

#include "LogoUI.h"
#include "../coc/SoundMgr.h"

LogoUI::LogoUI()
: m_isActionEnd(false)
, m_pLogoAction(NULL)
{
    
}

LogoUI::~LogoUI()
{
    if (m_pLogoAction) {
        m_pLogoAction->setDelegate(this);
    }
}

bool LogoUI::init()
{
    if (CUINode::init()) {
        CCNode* pLogoUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("logo.ccbi", this, &m_pLogoAction);
        
        m_pLogoAction->setDelegate(this);
           this->addChild(pLogoUI);
        
        SoundMgr::getSingleton().playEffect("logo.mp3");
    }
    
    return true;
}

void LogoUI::completedAnimationSequenceNamed(const char *name)
{
    m_isActionEnd = true;
}