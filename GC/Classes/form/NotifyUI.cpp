//
//  NotifyUI.cpp
//  GameClient
//
//  Created by chowray on 13-7-3.
//
//

#include "NotifyUI.h"
#include "../coc/GuideMgr.h"
#include "../coc/PlayerScene.h"

USING_NS_COC;
using namespace cocos2d;

NotifyUI::NotifyUI()
{
    
}

NotifyUI::~NotifyUI()
{
    
}

bool NotifyUI::init()
{
    bool bRet = false;
    do
    {
		CC_BREAK_IF(!CCLayer::init());
        
        int sw = CCDirector::sharedDirector()->getWinSize().width;
        int sh = CCDirector::sharedDirector()->getWinSize().height;
        
        curIndex = 0;
        for(int i = 0; i < 8; ++i)
        {
            CCLabelTTF* pLabel = CCLabelTTF::create("", "Font/Candal.ttf", 20);
            m_pLabelLst.push_back(pLabel);
            pLabel->setPosition(ccp(sw/2, sh - 33 * (i + 1)));
            
            this->addChild(pLabel);
        }
        
        bRet = true;
    }while(0);
    
    return bRet;
}

void NotifyUI::addNote(const char* str)
{
    int sh = CCDirector::sharedDirector()->getWinSize().height;
    
    m_pLabelLst[curIndex]->setString(str);
    m_pLabelLst[curIndex]->setPositionY(sh - 33 * 8);
    m_pLabelLst[curIndex]->setColor(ccRED);
    m_pLabelLst[curIndex]->setOpacity(255);

    CCDelayTime* delayTime = CCDelayTime::create(3.0f);
    CCFadeOut* fadeOut = CCFadeOut::create(1.0f);
    CCActionInterval* action1 = CCSequence::create(delayTime, fadeOut, NULL);
    
    m_pLabelLst[curIndex]->stopAllActions();
    m_pLabelLst[curIndex]->runAction(action1);
    
    for(int i = 0; i < 8; ++i)
    {
        if(i != curIndex)
        {
            m_pLabelLst[i]->setPositionY(m_pLabelLst[i]->getPositionY() + 33);
        }
    }

    curIndex ++;
    if(curIndex >= 8)
        curIndex = 0;
}

