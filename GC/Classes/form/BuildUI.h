#ifndef __FORM_BUILDUI_H__
#define __FORM_BUILDUI_H__

#include "Shop.h"
#include "../coc/NotifyListener.h"

using namespace cocos2d;
USING_NS_COC;

class BuildUI : public CUINode, public NotifyListener
{
public:
	BuildUI();
	virtual ~BuildUI();
public:
    // implement the "static create()" method manually
    CREATE_FUNC(BuildUI);
public:
	bool init();
	virtual bool onNotify(int notifyid, const void* pVal = NULL);
public:
    // a selector callback
    void menuOKCallback(CCObject* pSender, CCControlEvent event);
    // a selector callback
    void menuCancelCallback(CCObject* pSender, CCControlEvent event);
    
    virtual void setVisible(bool visible);
    
public:
    
    CCControlButton* m_pBtnOK;
    CCControlButton* m_pBtnCancel;
};

#endif  // __FORM_BUILDUI_H__