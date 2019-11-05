//
//  NotifyUI.h
//  GameClient
//
//  Created by chowray on 13-7-3.
//
//

#ifndef __GameClient__NotifyUI__
#define __GameClient__NotifyUI__

#include "CUINode.h"

using namespace cocos2d;

class NotifyUI : public CCLayer
{
public:
	NotifyUI();
	virtual ~NotifyUI();
public:
    // implement the "static create()" method manually
    CREATE_FUNC(NotifyUI);
public:
	bool init();
    
    void addNote(const char* str);
//    void actionCallback(CCObject* pSender);
public:
    std::vector<CCLabelTTF*> m_pLabelLst;
    
    int     curIndex;
};

#endif /* defined(__GameClient__NotifyUI__) */
