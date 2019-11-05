#ifndef __FORM_CHGSCENE_H__
#define __FORM_CHGSCENE_H__

#include "../coc/NetListener.h"
#include "CUINode.h"

using namespace cocos2d;
USING_NS_COC;

enum ChangeSceneType {
    CHANGE_TYPE_NORMAL = 0,
    CHANGE_TYPE_FINDMATCH = 1,
    CHANGE_TYPE_BACKHOME = 2,
};

class ChgScene : public CUINode, public NetListener
{
public:
	ChgScene();
	virtual ~ChgScene();
public:
    // implement the "static create()" method manually
    static ChgScene* create(int mode);
public:
	bool init(int mode);

    void onIdle(float dt);
    
	virtual bool onProc(const Json::Value& val);
    
	void OnSplashTimer(float dt);

	void onFinish();
    
    void menuBackCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
	bool		m_isTimeFinish;
	bool		m_isMsgFinish;
    
    CCLabelBMFont*    m_pTitle;
    CCControlButton*  m_pBtnBack;
    
    CCLabelBMFont* m_pTestLog;
    
    int         m_type;
    
    string m_strDebugLog;
};

#endif  // __FORM_CHGSCENE_H__