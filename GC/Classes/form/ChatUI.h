//
//  ChatUI.h
//  GameClient
//
//  Created by chowray on 14-1-7.
//
//

#ifndef __GameClient__ChatUI__
#define __GameClient__ChatUI__

#include "CUINode.h"
#include "ItemMenuUI.h"
#include "../coc/NotifyListener.h"
#include "../coc/SocketListener.h"
#include "../coc/BaseDef.h"

#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;
using namespace coc;

class ChatItem;
class DonateUI;
class DonateItem;

struct ChatCell {
    ChatData chatData;
    CCRect viewRect;
    ChatItem* pView;
        
    ChatCell(){
        viewRect = CCRectZero;
        pView = NULL;
    }
    
    ~ChatCell(){
        
    }
};

/*
 * Chat UI
 */
class ChatUI : public CUINode, public NotifyListener, public SocketListener, public CCScrollViewDelegate
{
public:
	ChatUI();
	virtual ~ChatUI();
public:
    // implement the "static create()" method manually
    CREATE_FUNC(ChatUI);
public:
    //virtual bool onProc(const Json::Value& val);
    
	virtual bool onNotify(int notifyid, const void* pVal = NULL);
    virtual bool onStateChange(int state);
public:
    
    bool init();
    void onEnter();
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    
    void updateTime(float dt);
    
    void addClanChat(ChatData chatData);
    void addWorldChat(ChatData chatData);
    void updateWorldChat(ChatData chatData);
    void updateClanChat(ChatData chatData);
    void deleteClanChat(ChatData chatData, bool isUpdate = false);
    
    void refreshPage();
    void refreshChatList();
        
    void joinClanChat();
    void clearClanChat();
    
    void scrollToTop();
    
//    void menuOpenCallback(CCObject* pSender, CCControlEvent event);
    void menuSendWorldChat(CCObject* pSender, CCControlEvent event);
    void menuSendClanChat(CCObject* pSender, CCControlEvent event);
    void menuPageCallback(CCObject* pSender, CCControlEvent event);
    void menuInfoCallback(CCObject* pSender, CCControlEvent event);
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view){};
public:
    
    CCNode* m_pChatNode;
    CCLayer* m_pChatView;
    
    CCNode* m_pChatTip;
    
    DonateUI* m_pDonateUI;
    
    CCNode* m_pWorldChatUI;
    CCNode* m_pClanChatUI;
    
    CCScrollView* m_pWorldChatView;
    CCScrollView* m_pClanChatView;
    
    CCScale9Sprite* m_pWorldEditBg;
    CCScale9Sprite* m_pClanEditBg;
    
    CTextInputField* m_pWorldChatField;
    CTextInputField* m_pClanChatField;
    
    CCNode* m_pClanTip;
    
    CCSprite* m_pClanIcon;
    CCLabelTTF* m_pClanName;
        
    CCControlButton* m_pBtnClanChat;
    CCControlButton* m_pBtnWorldChat;
    CCControlButton* m_pBtnInfo;
    
    CCControlButton* m_pBtnOpen;
    
    CCControlButton* m_pBtnPages[2];
    
    ItemMenuUI* m_pItemMenu;
    
    CCLabelBMFont* m_pDisconnectTip;
    
    map<int, ChatCell> m_mapWorldChat;
    map<int, ChatCell> m_mapClanChat;
    
    int m_pageIndex;
    bool m_isChatViewOpen;
    
    bool m_isAutoScrollTop;
    
    CCPoint m_dragOffset;
    bool m_isDrag;
};

/*
 * Chat Item
 */
class ChatItem: public CUINode{
    
public:
    ChatItem(ChatUI* pChatUI, int type, ChatData chatData);
    virtual ~ChatItem();
    
    static ChatItem* create(ChatUI* pChatUI, int type, ChatData chatData);
public:
    
    bool init();
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    
    void refresh();
    void setViewSize(CCSize viewSize);
    
    void addDeleteFadeOutAction();
    void onDeleteActionCallback();
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
    void menuDonateCallback(CCObject* pSender, CCControlEvent event);
    void menuRejectCallback(CCObject* pSender, CCControlEvent event);
    void menuAcceptCallback(CCObject* pSender, CCControlEvent event);
    void menuReplayCallback(CCObject* pSender, CCControlEvent event);
public:
    
    CCLabelTTF* m_pPlayerName;
    CCLabelTTF* m_pRivalName;
    
    CCSprite* m_pClanIcon;
    CCLabelTTF* m_pClanName;
    
    CCSprite* m_pRivalMilitary;
    
    CCLabelTTF* m_pMessage;
    
    CCLabelTTF* m_pStatusMessage;
    
    CCLabelBMFont* m_pHouseSpace;
    CCSprite* m_pSpaceProg;
    CCSprite* m_pSpaceProgBg;
    
    CCLabelBMFont* m_pBattleProgress;
    CCNode* m_pBattleStars;
    
    CCLabelBMFont* m_pFightAgainst;
    
    CCLabelBMFont* m_pGainCup;
    CCLabelBMFont* m_pMemberType;
    
    CCLabelBMFont* m_pChatTime;
    
    CCSprite* m_pSplitLine;
    
    CCSprite* m_pMilitaryIcon;
    
    CCControlButton*    m_pBtnDonate;
    CCControlButton*    m_pBtnReject;
    CCControlButton*    m_pBtnAccept;
    CCControlButton*    m_pBtnReplay;
        
    ChatUI* m_pChatUI;
    CCNode* m_pView;
    
    ChatData        m_chatData;
    ClanTroopsData  m_troopsData;
    
    int m_type;
    
    CCPoint m_dragOffset;
    bool m_isDrag;
    
    FightMsg m_fightMsg;
    
    bool m_isDelete;
};

/*
 * Donate UI
 */
class DonateUI : public CUINode {
public:
	DonateUI();
	~DonateUI();
public:
    CREATE_FUNC(DonateUI);
public:
    
    bool init();
    
    void refresh(ClanTroopsData troopsData);
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    
    void menuCloseCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
    
public:
    CCLayer* m_pDonateView;
    CCLabelBMFont* m_pDonateNums;
};

/*
 * Donate Item
 */
class DonateItem: public CUINode {
    
public:
    DonateItem(int sodierId);
    virtual ~DonateItem();
    
    static DonateItem* create(int sodierId);
public:
    
    bool init();
    void refresh();
    
    void menuDonateCallback(CCObject* pSender, CCControlEvent event);
    
    bool onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode);
    SEL_CCControlHandler onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName);
public:
    CCLabelBMFont* m_pDonateNums;
    CCLabelBMFont* m_pDonateLeft;
    
    CCControlButton* m_pBtnDonate;
    
    int m_donateLeft;
    int m_donateNums;
    int m_houseSpace;
    
    int m_sodierId;
};

#endif /* defined(__GameClient__ChatUI__) */
