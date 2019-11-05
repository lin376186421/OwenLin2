//
//  FBInviteUI.h
//  GameClient
//
//  Created by chowray on 13-9-4.
//
//

#ifndef __GameClient__FBInviteUI__
#define __GameClient__FBInviteUI__

#include "CUINode.h"

#include "../coc/NotifyListener.h"
#include "../coc/FacebookMgr.h"


using namespace cocos2d;

class FBInviteUI : public CUINode, public CCTableViewDataSource, public CCTableViewDelegate,public coc::NotifyListener
{
public:
	FBInviteUI();
	virtual ~FBInviteUI();
public:
    CREATE_FUNC(FBInviteUI);
//    static FBInviteUI* create();
public:
	bool init();
    virtual bool onNotify(int notifyid, const void* pVal = NULL);
    void refreshInfo();
    
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    void menuCancelCallback(CCObject* pSender, CCControlEvent event);
    void menuInviteCallback(CCObject* pSender, CCControlEvent event);
    void menuMoreCallback(CCObject* pSender, CCControlEvent event);
    void menuAllCallback(CCObject* pSender, CCControlEvent event);

   
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    virtual void scrollViewDidScroll(CCScrollView* view) {};
    virtual void scrollViewDidZoom(CCScrollView* view) {};
    void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
    CCSize cellSizeForTable(CCTableView *table);
    CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
    unsigned int numberOfCellsInTableView(CCTableView *table);
    
    
    bool changSelCount(bool isAdd);
    
public:
    
    CCNode *m_pMoreNode;
    
    CCNode *m_pCCBNode;
    
    CCNode *m_pLoading;
    
    CCLayer* m_pView;
    
    CCTableView* m_pTableView;
    
    CCLabelBMFont *m_pNOFriends;
    
    CCLabelBMFont *m_pFreeGem;
    CCLabelBMFont *m_pSelCount;
    
    CCControlButton *m_pBtnAll;
    
    CCControlButton *m_pBtnInvite;
    
    int  m_selCount;

    
    bool m_isReady;
    
    
};

class FBUserItem: public CUINode , public NotifyListener{
    
public:
    FBUserItem(CCSize parentSize, FBInviteUI *FBInviteLayer,FBFriendInfo *info);
    virtual ~FBUserItem();
    virtual void onExit();
    virtual void onEnter();
    virtual bool onNotify(int notifyid, const void* pVal = NULL);
    static FBUserItem* create(CCSize parentSize, FBInviteUI *FBInviteLayer,FBFriendInfo *info);
public:
    
    bool init();
    
    void refresh();
    
    void touchMySelf();
    
    void getImageComplete();
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    
    void menuInviteCallback(CCObject* pSender, CCControlEvent event);
    

    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    
    CCLabelTTF *m_pFBName;
    CCSprite *m_pFBSprite;
    
   
    
    CCSprite *m_pMarkSprite;
    
    FBFriendInfo *m_fbInfo;
    FBInviteUI *m_pFBInviteLayer;
    
    CCNode *m_pItemNode;
    
    CCSprite *m_pAddFBSprite;
    
    CCSize m_size;
    
    float m_sca;
    
    bool m_isGetImg;
    
    
    
    
    
    
};


#endif /* defined(__GameClient__FBInviteUI__) */
