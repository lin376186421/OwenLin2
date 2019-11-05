//
//  ChatUI.cpp
//  GameClient
//
//  Created by chowray on 14-1-7.
//
//

#include <algorithm>
#include "ChatUI.h"
#include "SimpleAudioEngine.h"
#include "CAppPlatform.h"
#include "CUINodeCache.h"
#include "zlib.h"
#include "base32.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/PlayerScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/HttpClient.h"
#include "../coc/Building.h"
#include "../coc/BuildingMgr.h"
#include "../coc/SoundMgr.h"
#include "../coc/HeroMgr.h"
#include "../coc/ExperienceLevels.h"
#include "../coc/GuideMgr.h"
#include "../coc/Person.h"
#include "../coc/PersonMgr.h"
#include "../coc/ClanMgr.h"
#include "../coc/ChatMgr.h"
#include "../coc/SocketClient.h"
#include "../Utils/Base64.h"

using namespace cocos2d;
using namespace coc;

int requestHelpUserId = -1;

ChatUI::ChatUI()
{
}

ChatUI::~ChatUI()
{
    ImgMgr::getSingleton().removeRes("clan_icon_big");
    
    LogicInfo::getSingleton().removeListener(this);
    SocketClient::getSingleton().removeListener(this);
}

bool ChatUI::init()
{
	bool bRet = false;
    do
    {
        CC_BREAK_IF(!CUINode::init());
        {
            LogicInfo::getSingleton().addListener(this);
            SocketClient::getSingleton().addListener(this);
            
#if(COC_CLAN == 1)
            
            m_pChatNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_chat.ccbi", this);
            this->setContentSize(m_pChatNode->getContentSize());
            this->setTargetTouch(true);
            
            this->addChild(m_pChatNode);
            
            m_pWorldChatUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_worldroom.ccbi", this, m_pChatView->getContentSize());
            m_pClanChatUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_clanroom.ccbi", this, m_pChatView->getContentSize());
            
            m_pChatView->addChild(m_pWorldChatUI);
            m_pChatView->addChild(m_pClanChatUI);
            
            m_pClanChatView->setContentSize(ccp(m_pClanChatView->getViewSize().width, 0));
            m_pWorldChatView->setContentSize(ccp(m_pWorldChatView->getViewSize().width, 0));
            
            m_pClanChatView->setDelegate(this);
            m_pWorldChatView->setDelegate(this);
            
            m_pWorldChatField = CTextInputField::create(m_pWorldEditBg->getContentSize(), CCScale9Sprite::create());
            m_pWorldChatField->setFontColor(ccBLACK);
            m_pWorldChatField->setAnchorPoint(ccp(0, 0));
            m_pWorldChatField->setPosition(ccp(0, 0));
            m_pWorldChatField->setMaxLength(80);
            m_pWorldChatField->setReturnType(kKeyboardReturnTypeDone);
            m_pWorldEditBg->addChild(m_pWorldChatField);
            
            m_pClanChatField = CTextInputField::create(m_pClanEditBg->getContentSize(), CCScale9Sprite::create());
            m_pClanChatField->setFontColor(ccBLACK);
            m_pClanChatField->setAnchorPoint(ccp(0, 0));
            m_pClanChatField->setPosition(ccp(0, 0));
            m_pClanChatField->setMaxLength(80);
            m_pClanChatField->setReturnType(kKeyboardReturnTypeDone);
            m_pClanEditBg->addChild(m_pClanChatField);

            m_pDonateUI = DonateUI::create();
            m_pDonateUI->setVisible(false);
            m_pClanChatUI->addChild(m_pDonateUI, 10);
            
            m_pageIndex = 1;
            m_pBtnPages[m_pageIndex]->setEnabled(false);
            m_pWorldChatUI->setVisible(false);
            
            m_pItemMenu = ItemMenuUI::create(m_pClanChatView);
            m_pItemMenu->setVisible(false);
            m_pChatView->addChild(m_pItemMenu);
            
            m_isChatViewOpen = false;
            
            refreshPage();
            
            map<int, ChatData>::iterator it = ChatMgr::getSingleton().m_mapWorldChat.begin();
            for (; it != ChatMgr::getSingleton().m_mapWorldChat.end(); it++) {
                addWorldChat(it->second);
            }
        
            it = ChatMgr::getSingleton().m_mapClanChat.begin();
            for (; it != ChatMgr::getSingleton().m_mapClanChat.end(); it++) {
                addClanChat(it->second);
            }
            
            scrollToTop();
            m_isAutoScrollTop = true;
            
            this->schedule(schedule_selector(ChatUI::updateTime), 1000);
            
            #endif
        }
		bRet = true;
	}while(0);
    
	return bRet;
}

void ChatUI::onEnter()
{
    CUINode::onEnter();
    
    m_isChatViewOpen = true;
    refreshChatList();
}

bool ChatUI::onStateChange(int state)
{
    if (state == WebSocket::kStateOpen) {
//        m_pDisconnectTip->setVisible(false);
    }
    else {
//        m_pDisconnectTip->setVisible(true);
    }
    
    return true;
}

void ChatUI::updateTime(float dt)
{
    if (m_isChatViewOpen) {
        
        if (m_pageIndex == 0) {
            int count = m_pWorldChatView->getContainer()->getChildrenCount();
            
            for (int i = 0; i < count; i++) {
                ChatItem* pChatItem = (ChatItem*)m_pWorldChatView->getContainer()->getChildren()->objectAtIndex(i);
                pChatItem->m_pChatTime->setString(formatTimeAgo(pChatItem->m_chatData.time, false).c_str());
            }
        }
        else if (m_pageIndex == 1) {
            int count = m_pClanChatView->getContainer()->getChildrenCount();
            
            for (int i = 0; i < count; i++) {
                ChatItem* pChatItem = (ChatItem*)m_pClanChatView->getContainer()->getChildren()->objectAtIndex(i);
                pChatItem->m_pChatTime->setString(formatTimeAgo(pChatItem->m_chatData.time, false).c_str());
            }
        }
    }
}

bool ChatUI::onNotify(int notifyid, const void* pVal)
{
    if(notifyid == NOTIFY_MYCLANCHG)
    {
        int state = *(int*)pVal;
        
        if(state == 0)
        {
            //退出帮派
            if(ClanMgr::getSingleton().m_myclan.id == 0)
                return true;
            
            ClanMgr::getSingleton().m_myclan.id = 0;
            
            if(PlayerScene::getSingleton().m_pGameWorld->m_pClanUI != NULL)
                PlayerScene::getSingleton().m_pGameWorld->hideClanUI();
            
            if(PlayerScene::getSingleton().m_pGameWorld->m_pMainUI != NULL)
                PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->onSelect(false, NULL);
            
            clearClanChat();
        }
        else
        {
            m_pClanIcon->setVisible(true);
            m_pClanName->setVisible(true);
            
            char str[128];
            sprintf(str, "clan_icon_%d", ClanMgr::getSingleton().m_myclan.flag);
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
            m_pClanIcon->setDisplayFrame(pFrame);
            m_pClanName->setString(ClanMgr::getSingleton().m_myclan.name.c_str());
        }
    }
    
	return true;
}

bool ChatUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    if (!isTouchInside(pTouch, m_pDonateUI)) {
        m_pDonateUI->setVisible(false);
    }
    
    m_dragOffset = m_pClanChatView->getContentOffset();
    m_isDrag = false;
    
    return true;
}

void ChatUI::scrollViewDidScroll(CCScrollView* view)
{
    refreshChatList();
    
    if (m_pageIndex == 0) {
        if (fabs(m_dragOffset.y - m_pWorldChatView->getContentOffset().y) > 10) {
            m_isDrag = true;
            
            m_isAutoScrollTop = false;
        }
        
        if (m_pWorldChatView->getContentOffset().y <= m_pWorldChatView->getViewSize().height - m_pWorldChatView->getContentSize().height) {
            m_isAutoScrollTop = true;
        }
    }
    else if (m_pageIndex == 1) {
        if (fabs(m_dragOffset.y - m_pClanChatView->getContentOffset().y) > 10) {
            m_isDrag = true;
            
            m_isAutoScrollTop = false;
        }
        
        if (m_pClanChatView->getContentOffset().y <= m_pClanChatView->getViewSize().height - m_pClanChatView->getContentSize().height) {
            m_isAutoScrollTop = true;
        }
    }
}

//bool isDragView = false;
//CCPoint touchDownPos;
//
//void ChatUI::menuOpenCallback(CCObject* pSender, CCControlEvent event)
//{
//    CCPoint location = ((CCControlButton*)pSender)->getTouchPosition();
//    location = CCDirector::sharedDirector()->convertToGL(location);
//    float offX = 0;//((CCControlButton*)pSender)->getPosition().x - m_pChatView->getContentSize().width;
//    
//    if (event == CCControlEventTouchDown) {
//        touchDownPos = location;
//        isDragView = false;
//    }
//    
//    if (event == CCControlEventTouchUpInside || event == CCControlEventTouchUpOutside) {
//        if(m_pChatView->numberOfRunningActions() == 0)
//        {
//            if (isDragView) {
//                if ((location.x - offX) >= m_pChatView->getContentSize().width / 2) { //open
//                    CCMoveTo *moveTo = CCMoveTo::create(0.12f, CCPoint(0, 0));
//                    m_pChatNode->runAction(moveTo);
//                    m_isChatViewOpen = true;
//                    
//                    setControlButtonFrameForAllState(m_pBtnOpen, ImgMgr::getSingleton().getFrame("map_Clanbt_0_1"));
//                }else{
//                    CCMoveTo *moveTo = CCMoveTo::create(0.12f, CCPoint(-m_pChatView->getContentSize().width, 0));
//                    m_pChatNode->runAction(moveTo);
//                    m_isChatViewOpen = false;
//                    
//                    setControlButtonFrameForAllState(m_pBtnOpen, ImgMgr::getSingleton().getFrame("map_Clanbt_0_0"));
//                }
//            }else{
//                if (m_isChatViewOpen) {
//                    CCMoveTo *moveTo = CCMoveTo::create(0.12f, CCPoint(-m_pChatView->getContentSize().width, 0));
//                    m_pChatNode->runAction(moveTo);
//                    
//                    setControlButtonFrameForAllState(m_pBtnOpen, ImgMgr::getSingleton().getFrame("map_Clanbt_0_0"));
//                }else{
//                    CCMoveTo *moveTo = CCMoveTo::create(0.12f, CCPoint(0, 0));
//                    m_pChatNode->runAction(moveTo);
//                    
//                    setControlButtonFrameForAllState(m_pBtnOpen, ImgMgr::getSingleton().getFrame("map_Clanbt_0_1"));
//                }
//                m_isChatViewOpen = !m_isChatViewOpen;
//            }
//            
//            isDragView = false;
//        }
//    }else if (event == CCControlEventTouchDragInside || event == CCControlEventTouchDragOutside){
//        
//        if (fabs(location.x - touchDownPos.x) >= 10) {
//            isDragView = true;
//        }
//        
//        if (isDragView) {
//            if ((location.x - offX) <= m_pChatView->getContentSize().width && location.x >= offX) {
//                m_pChatNode->setPosition(CCPoint(location.x - offX - m_pChatView->getContentSize().width, 0));
//            }else{
//                if ((location.x - offX) > m_pChatView->getContentSize().width) {
//                    m_pChatNode->setPosition(CCPoint(0, 0));
//                }else{
//                    m_pChatNode->setPosition(CCPoint(-m_pChatView->getContentSize().width, 0));
//                }
//            }
//        }
//    }
//}

void ChatUI::menuPageCallback(CCObject *pSender, CCControlEvent event)
{
    int page = ((CCNode*)pSender)->getTag();
    
    m_pageIndex = page;
    refreshPage();
    
    scrollToTop();
    
    m_isAutoScrollTop = true;
}

void ChatUI::menuSendWorldChat(CCObject* pSender, CCControlEvent event)
{
    std::string str = m_pWorldChatField->getText();
    
    if(str.empty())
    {
        return;
    }
    
    ChatData chatData;
    chatData.type = CHAT_TYPE_NORMAL;
    chatData.uid = LogicInfo::getSingleton().m_myInfo.uid;
    chatData.uname = LogicInfo::getSingleton().m_myInfo.uname;
    chatData.message = str;
    
    ChatMgr::getSingleton().sendWorldChat(chatData);
    
    m_pWorldChatField->setText("");
    
    m_pChatTip->setVisible(false);
    
    m_isAutoScrollTop = true;
}

void ChatUI::menuSendClanChat(CCObject* pSender, CCControlEvent event)
{    
    std::string str = m_pClanChatField->getText();
    
    if(str.empty())
    {
        return;
    }
    
    ChatData chatData;
    chatData.type = CHAT_TYPE_NORMAL;
    chatData.uid = LogicInfo::getSingleton().m_myInfo.uid;
    chatData.uname = LogicInfo::getSingleton().m_myInfo.uname;
    chatData.message = str;
    
    ChatMgr::getSingleton().sendClanChat(chatData);
    
    m_pClanChatField->setText("");
    
    m_isAutoScrollTop = true;
}

void ChatUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    m_pDonateUI->setVisible(false);
    
    PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->countChatNums(m_pageIndex);
    PlayerScene::getSingleton().m_pGameWorld->hideChatUI();
}

void ChatUI::addWorldChat(ChatData chatData)
{
    map<int, ChatData>::iterator it = ChatMgr::getSingleton().m_mapWorldChat.find(chatData.cid);
    (&it->second)->isShow = true;
    
    CCSize viewSize = m_pWorldChatView->getContentSize();
    
    int posY = viewSize.height;
    int viewW = viewSize.width;
    int viewH = 0;
    
    {
        viewH = 116;
        
        //adjust view height
        int textH = 14;
        int line = 3;
        CCLabelTTF* pText = CCLabelTTF::create(chatData.message.c_str(), "Font/Candal.ttf", 14);
        
        if (pText->getContentSize().height < textH*line) {
            int offH = textH*line - pText->getContentSize().height;
            viewH -= offH;
        }
        
        viewH *= getResolutionScale();
    }
    
    ChatCell cell;
    cell.chatData = chatData;
    cell.viewRect = CCRectMake(0, posY, viewW, viewH);
    cell.pView = NULL;
    
    m_mapWorldChat.insert(pair<int, ChatCell>(chatData.cid, cell));
    
    viewH += viewSize.height;
    
    if (m_mapWorldChat.size() > CHAT_SHOW_MAXNUMS) {
        
        map<int, ChatCell>::iterator it = m_mapWorldChat.begin();
        if (it->second.pView) {
            it->second.pView->removeFromParent();
        }
        
        viewH -= it->second.viewRect.size.height;
                
        m_mapWorldChat.erase(m_mapWorldChat.begin());
    }
    
    m_pWorldChatView->setContentSize(CCSizeMake(viewW, viewH));
    
    if (m_isAutoScrollTop) {
        scrollToTop();
    }

    refreshChatList();
    
    m_pChatTip->setVisible(false);
}

void ChatUI::addClanChat(ChatData chatData)
{
    map<int, ChatData>::iterator it = ChatMgr::getSingleton().m_mapClanChat.find(chatData.cid);
    (&it->second)->isShow = true;
    
    CCSize viewSize = m_pClanChatView->getContentSize();
    
    int posY = viewSize.height;
    int viewW = viewSize.width;
    int viewH = 0;
    
    {
        if (chatData.type == CHAT_TYPE_NORMAL) {
            viewH = 116;
            
            //adjust view height
            int textH = 14;
            int line = 3;
            CCLabelTTF* pText = CCLabelTTF::create(chatData.message.c_str(), "Font/Candal.ttf", 14);
            
            if (pText->getContentSize().height < textH*line) {
                int offH = textH*line - pText->getContentSize().height;
                viewH -= offH;
            }
        }
        else if (chatData.type == CHAT_TYPE_JOIN_CLAN) {
            viewH = 170;
        }
        else if (chatData.type == CHAT_TYPE_HELP) {
            viewH = 150;
        }
        else if (chatData.type == CHAT_TYPE_VIDEO) {
            viewH = 182;
        }
        else if (chatData.type == CHAT_TYPE_INVATE) {
            viewH = 170;
        }
        
        viewH *= getResolutionScale();
    }

    ChatCell cell;
    cell.chatData = chatData;
    cell.viewRect = CCRectMake(0, posY, viewW, viewH);
    cell.pView = NULL;
    
    m_mapClanChat.insert(pair<int, ChatCell>(chatData.cid, cell));
    
    viewH += viewSize.height;
   
    if (m_mapClanChat.size() > CHAT_SHOW_MAXNUMS) {
        map<int, ChatCell>::iterator it = m_mapClanChat.begin();
        
        if (it->second.pView) {
            it->second.pView->removeFromParent();
        }
        
        viewH -= it->second.viewRect.size.height;
        
        m_mapClanChat.erase(it);
    }
    
    m_pClanChatView->setContentSize(CCSizeMake(viewW, viewH));
    
    if (m_isAutoScrollTop) {
        scrollToTop();
    }
    
    refreshChatList();
}

void ChatUI::updateWorldChat(ChatData chatData)
{
    map<int, ChatCell>::iterator it = m_mapWorldChat.find(chatData.cid);
    if (it != m_mapWorldChat.end()) {
        if (it->second.pView) {
            it->second.pView-> m_chatData = chatData;
            it->second.pView->refresh();
        }
    }
}

void ChatUI::updateClanChat(ChatData chatData)
{
    map<int, ChatCell>::iterator it = m_mapClanChat.find(chatData.cid);
    if (it != m_mapClanChat.end()) {
        if (it->second.pView) {
            it->second.pView->m_chatData = chatData;
            it->second.pView->refresh();
        }
    }
}

void ChatUI::deleteClanChat(ChatData chatData, bool isUpdate)
{
    map<int, ChatCell>::iterator it = m_mapClanChat.find(chatData.cid);
    if (it != m_mapClanChat.end()) {
        
        ChatItem* pChatItem = it->second.pView;
        
        if (pChatItem) {
            
            if (isUpdate) {
                m_pClanChatView->getContainer()->removeChild(pChatItem);
                m_mapClanChat.erase(it);
                
                refreshChatList();
            }
            else {
                pChatItem->addDeleteFadeOutAction();
            }
        }
        else {
            
            map<int, ChatCell>::iterator it = m_mapClanChat.find(chatData.cid);
            
            m_pClanChatView->getContainer()->removeChild(this);
            m_mapClanChat.erase(it);
            
            CCPoint offset = m_pClanChatView->getContentOffset();
            offset.y += getContentSize().height;
            
            m_pClanChatView->setContentOffset(offset);
            
            refreshChatList();
        }
    }
}

void ChatUI::refreshPage()
{
    for (int i = 0; i < 2; i++)
    {
        m_pBtnPages[i]->setEnabled(true);
    }
    
    if (m_pageIndex == 0) {
                
        m_pClanChatUI->setVisible(false);
        m_pWorldChatUI->setVisible(true);
    }
    else if (m_pageIndex == 1) {
        
        m_pClanChatUI->setVisible(true);
        m_pWorldChatUI->setVisible(false);
        
        char str[64];
        if (ClanMgr::getSingleton().m_myclan.id == 0) {
            
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("clan_icon_big");
            ((CCSprite*)m_pClanTip->getChildByTag(1))->setDisplayFrame(pFrame);
            
            m_pClanIcon->setVisible(false);
            m_pClanName->setVisible(false);
            m_pBtnInfo->setVisible(false);
            
            m_pClanChatField->setEnabled(false);
            m_pBtnClanChat->setEnabled(false);
            
            if (m_mapClanChat.size() > 0) {
                m_pClanTip->setVisible(false);
            }
            else {
                m_pClanTip->setVisible(true);
            }
        }
        else {
            m_pClanName->setString(ClanMgr::getSingleton().m_myclan.name.c_str());
            
            sprintf(str, "clan_icon_%d", ClanMgr::getSingleton().m_myclan.flag);
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
            m_pClanIcon->setDisplayFrame(pFrame);
            
            m_pClanChatField->setEnabled(true);
            m_pBtnClanChat->setEnabled(true);
            
            m_pClanTip->setVisible(false);
        }
        
    }
    
    PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->countChatNums(m_pageIndex);
    
    m_pBtnPages[m_pageIndex]->setEnabled(false);
    
    refreshChatList();
}

void ChatUI::scrollToTop()
{
    if (m_pageIndex == 0) {
        int offsetY = 0;
        if (m_pWorldChatView->getContentSize().height < m_pWorldChatView->getViewSize().height) {
            offsetY = m_pWorldChatView->getViewSize().height - m_pWorldChatView->getContentSize().height;
        }
        else {
            offsetY = -(m_pWorldChatView->getContentSize().height - m_pWorldChatView->getViewSize().height);
        }
        
        m_pWorldChatView->setContentOffset(ccp(0, offsetY));
    }
    else if (m_pageIndex == 1) {
        int offsetY = 0;
        if (m_pClanChatView->getContentSize().height < m_pClanChatView->getViewSize().height) {
            offsetY = m_pClanChatView->getViewSize().height - m_pClanChatView->getContentSize().height;
        }
        else {
            offsetY = -(m_pClanChatView->getContentSize().height - m_pClanChatView->getViewSize().height);
        }
        
        m_pClanChatView->setContentOffset(ccp(0, offsetY));
    }
}

void ChatUI::refreshChatList()
{
    if (m_isChatViewOpen) {
        if (m_pageIndex == 0) {
            float height = 0;
            CCRect viewRect = m_pWorldChatView->boundingBox();
            
            map<int, ChatCell>::iterator it = m_mapWorldChat.begin();
            for (; it != m_mapWorldChat.end(); it++) {
                
                ChatItem* pChatItem = it->second.pView;
                
                int posY = height;
                it->second.viewRect.origin.y = posY;
                
                CCRect itemRect = it->second.viewRect;
                itemRect.origin.y += m_pWorldChatView->getContentOffset().y;
                
                if (itemRect.intersectsRect(viewRect)) {
                    if (pChatItem) {
                        pChatItem->setPositionY(posY);
                    }
                    else {
                        
                        ChatData chatData = it->second.chatData;
                        
                        ChatItem* pItem = ChatItem::create(this, 0, chatData);
                        pItem->setPosition(ccp(0, posY));
                        pItem->setViewSize(it->second.viewRect.size);
                        pItem->setTouchControlView(m_pWorldChatView);
                        it->second.pView = pItem;
                        
                        m_pWorldChatView->addChild(it->second.pView);
                        
                        m_pClanTip->setVisible(false);
                    }
                }
                else {
                    if (pChatItem && !pChatItem->m_isDelete) {
                        
                        pChatItem->removeFromParent();
                        it->second.pView = NULL;
                    }
                }
                
                height += it->second.viewRect.size.height;
            }
            
            m_pWorldChatView->setContentSize(CCSizeMake(m_pWorldChatView->getContentSize().width, height));
        }
        else if (m_pageIndex == 1) {
            float height = 0;
            CCRect viewRect = m_pClanChatView->boundingBox();
            
            map<int, ChatCell>::iterator it = m_mapClanChat.begin();
            for (; it != m_mapClanChat.end(); it++) {
                
                ChatItem* pChatItem = it->second.pView;
                
                int posY = height;
                it->second.viewRect.origin.y = posY;
                
                CCRect itemRect = it->second.viewRect;
                itemRect.origin.y += m_pClanChatView->getContentOffset().y;
                
                if (itemRect.intersectsRect(viewRect)) {
                    if (pChatItem) {
                        pChatItem->setPositionY(posY);
                    }
                    else {
                        
                        ChatData chatData = it->second.chatData;
                        
                        ChatItem* pItem = ChatItem::create(this, 1, chatData);
                        pItem->setPosition(ccp(0, posY));
                        pItem->setViewSize(it->second.viewRect.size);
                        pItem->setTouchControlView(m_pClanChatView);
                        it->second.pView = pItem;
                        
                        m_pClanChatView->addChild(it->second.pView);
                        
                        m_pClanTip->setVisible(false);
                    }
                }
                else {
                    if (pChatItem && !pChatItem->m_isDelete) {
                        pChatItem->removeFromParent();
                        it->second.pView = NULL;
                    }
                }
                
                height += it->second.viewRect.size.height;
            }
            
            m_pClanChatView->setContentSize(CCSizeMake(m_pClanChatView->getContentSize().width, height));
        }
    }
}

void ChatUI::joinClanChat()
{
    char str[128];
    sprintf(str, "clan_icon_%d", ClanMgr::getSingleton().m_myclan.flag);
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pClanIcon->setDisplayFrame(pFrame);
    m_pClanName->setString(ClanMgr::getSingleton().m_myclan.name.c_str());
    
    m_pClanIcon->setVisible(true);
    m_pClanName->setVisible(true);
    m_pBtnInfo->setVisible(true);
}

void ChatUI::clearClanChat()
{
    m_mapClanChat.clear();
    m_pClanChatView->getContainer()->removeAllChildren();
    
    CCSize viewSize = m_pClanChatView->getContentSize();
    m_pClanChatView->setContentSize(CCSizeMake(viewSize.width, 0));
}

void ChatUI::menuInfoCallback(CCObject* pSender, CCControlEvent event)
{
    if(ClanMgr::getSingleton().m_myclan.id > 0)
    {
        PlayerScene::getSingleton().m_pGameWorld->showClanUI();
    }
}

bool ChatUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnPageWorld", CCControlButton*, m_pBtnPages[0]);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnPageClan", CCControlButton*, m_pBtnPages[1]);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanChatView", CCScrollView*, m_pClanChatView);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WorldChatView", CCScrollView*, m_pWorldChatView);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ChatTip", CCNode*, m_pChatTip);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnInfo", CCControlButton*, m_pBtnInfo);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnOpen", CCControlButton*, m_pBtnOpen);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WorldEditBg", CCScale9Sprite*, m_pWorldEditBg);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanEditBg", CCScale9Sprite*, m_pClanEditBg);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanTip", CCNode*, m_pClanTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnClanChat", CCControlButton*, m_pBtnClanChat);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnWorldChat", CCControlButton*, m_pBtnWorldChat);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DisconnectTip", CCLabelBMFont*, m_pDisconnectTip);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ChatView", CCLayer*, m_pChatView);
    
    return true;
}

SEL_CCControlHandler ChatUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
//    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuOpenCallback", ChatUI::menuOpenCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuPageCallback", ChatUI::menuPageCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSendWorldChat", ChatUI::menuSendWorldChat);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSendClanChat", ChatUI::menuSendClanChat);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", ChatUI::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuInfoCallback", ChatUI::menuInfoCallback);
    
    return NULL;
    
}


ChatItem::ChatItem(ChatUI* pChatUI, int type, ChatData chatData)
: m_pChatUI(pChatUI)
, m_chatData(chatData)
, m_pMemberType(NULL)
, m_pView(NULL)
, m_type(type)
, m_isDelete(false)
{

}

ChatItem::~ChatItem()
{
    
}

ChatItem* ChatItem::create(ChatUI* pChatUI, int type, ChatData chatData)
{
    ChatItem *pRet = new ChatItem(pChatUI, type, chatData);
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool ChatItem::init()
{
    if (CUINode::init()) {
        
        CCSize parentSize = m_pChatUI->m_pChatView->getContentSize();
        
        if(m_chatData.type == CHAT_TYPE_NORMAL)
        {
            m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_chat_item0.ccbi", this, parentSize);
        }
        else if (m_chatData.type == CHAT_TYPE_JOIN_CLAN)
        {
            m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_chat_item1.ccbi", this, parentSize);
        }
        else if (m_chatData.type == CHAT_TYPE_HELP)
        {
            m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_chat_item2.ccbi", this, parentSize);
        }
        else if (m_chatData.type == CHAT_TYPE_VIDEO)
        {
            m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_chat_item3.ccbi", this, parentSize);
        }
        else if (m_chatData.type == CHAT_TYPE_INVATE)
        {
            m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clan_chat_item1.ccbi", this, parentSize);
        }
        
        m_pMessage->setString(m_chatData.message.c_str());
        m_pPlayerName->setString(m_chatData.uname.c_str());

        if (m_pMemberType && m_type == 1){
            m_pMemberType->setString(ClanJob[m_chatData.memberType].c_str());
        }
        
        this->setContentSize(m_pView->getContentSize());
        this->addChild(m_pView);
        
        this->setTouchEnabled(true);
        
        refresh();
        
        return true;
    }
    
    return false;
}

void ChatItem::refresh()
{
    m_pChatTime->setString(formatTimeAgo(m_chatData.time, false).c_str());
    
    m_troopsData = ChatMgr::getSingleton().getClanTroopsData(m_chatData.uid);
    if (m_chatData.type == CHAT_TYPE_NORMAL) {
        
        m_pClanIcon->setVisible(false);
        m_pClanName->setVisible(false);
        if (m_type == 0) {
            
            if (m_chatData.clanInfo.clanid > 0) {
                char str[128];
                sprintf(str, "clan_icon_%d", m_chatData.clanInfo.clanflag);
                CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
                m_pClanIcon->setDisplayFrame(pFrame);
                m_pClanName->setString(m_chatData.clanInfo.clanname.c_str());
                
                m_pClanIcon->setVisible(true);
                m_pClanName->setVisible(true);
            }
        }
        
        if (m_type == 0) {
            m_pMemberType->setVisible(false);
        }
    }
    else if (m_chatData.type == CHAT_TYPE_JOIN_CLAN) {
        
        if (m_chatData.status == 1 || ClanMgr::getSingleton().m_clanjob < CLAN_JOB_ELDER) {
            m_pBtnAccept->setVisible(false);
            m_pBtnReject->setVisible(false);
            
            m_pStatusMessage->setString("");
        }
        
        if (m_chatData.status == 1) {
            m_pStatusMessage->setString(m_chatData.statusMessage.c_str());
        }
    }
    else if (m_chatData.type == CHAT_TYPE_HELP) {
        
        char str[128];
        sprintf(str, "%d/%d", m_troopsData.curspace, m_troopsData.maxspace);
        m_pHouseSpace->setString(str);
        
        float per = (float)m_troopsData.curspace/(float)m_troopsData.maxspace;
        m_pSpaceProg->setScaleX(m_pSpaceProgBg->getScaleX() * per);
        
        if (m_chatData.uid == LogicInfo::getSingleton().m_myInfo.uid || m_troopsData.donateNums >= 5 || m_troopsData.curspace >= m_troopsData.maxspace) {
            m_pBtnDonate->setVisible(false);
        }
    }
    else if (m_chatData.type == CHAT_TYPE_VIDEO) {
		
        Json::Value logInfo = m_chatData.battleLog;
        
        m_fightMsg.msgid = getAsInt(logInfo["battleid"]);
        m_fightMsg.star = getAsInt(logInfo["star"]);
        m_fightMsg.attackCup = getAsInt(logInfo["atkcup"]);
        m_fightMsg.defendCup = getAsInt(logInfo["defcup"]);
        m_fightMsg.time = getAsInt(logInfo["battletime"]);
        m_fightMsg.isAlreadyAttack = getAsInt(logInfo["isattack"]);
        m_fightMsg.clientver = getAsInt(logInfo["clientver"]);
        
        {
            unsigned char* pDest = (unsigned char*)malloc(sizeof(m_fightMsg.fsi) * 2);
            unsigned char* pDest1 = (unsigned char*)malloc(sizeof(m_fightMsg.fsi) * 2);
            
            if (m_fightMsg.clientver < COC_ATTACK_VER_FIX) {
                std::string str = logInfo["result"].asString();
                
                FightScoreInfo fsi;
                
                unsigned long destLen = DecodeLengthBase32(str.length());
                DecodeBase32(str.c_str(), sizeof(fsi) * 2, pDest);
                
                unsigned long destLen1 = sizeof(fsi) * 2;
                uncompress(pDest1, &destLen1, pDest, destLen);
                
                memcpy(&fsi, pDest1, sizeof(fsi));
                
                m_fightMsg.fsi = fsi;
            }
            else if (m_fightMsg.clientver < COC_ATTACK_VER_FIX2) {
                std::string str = logInfo["result2"].asString();
                
                FightScoreInfoEx fsi;
                
                unsigned long destLen = DecodeLengthBase32(str.length());
                DecodeBase32(str.c_str(), sizeof(fsi) * 2, pDest);
                
                unsigned long destLen1 = sizeof(fsi) * 2;
                uncompress(pDest1, &destLen1, pDest, destLen);
                memcpy(&fsi, pDest1, sizeof(fsi));
                
                m_fightMsg.fsi = fsi;
            }
            else if (m_fightMsg.clientver < COC_ATTACK_VER_FIX3) {
                std::string str = logInfo["result2"].asString();
                
                FightScoreInfoEx2 fsi;
                
                unsigned long destLen = DecodeLengthBase32(str.length());
                DecodeBase32(str.c_str(), sizeof(fsi) * 2, pDest);
                
                unsigned long destLen1 = sizeof(fsi) * 2;
                uncompress(pDest1, &destLen1, pDest, destLen);
                memcpy(&fsi, pDest1, sizeof(fsi));
                
                m_fightMsg.fsi = fsi;
            }
            else if (m_fightMsg.clientver < COC_ATTACK_VER_FIX4) {
                
                std::string str = logInfo["result"].asString();
                
                unsigned long destLen = DecodeLengthBase32(str.length());
                DecodeBase32(str.c_str(), sizeof(m_fightMsg.fsi) * 2, pDest);
                
                unsigned long destLen1 = sizeof(m_fightMsg.fsi) * 2;
                uncompress(pDest1, &destLen1, pDest, destLen);
                memcpy(&m_fightMsg.fsi, pDest1, sizeof(m_fightMsg.fsi));
            }
            else if (m_fightMsg.clientver < COC_ATTACK_VER_FIX5) {
                
                std::string str = logInfo["result"].asString();
                
                unsigned long destLen = 0;
                Base64::Decode((unsigned char*)str.c_str(), strlen(str.c_str()), pDest, destLen);
                
                unsigned long destLen1 = sizeof(m_fightMsg.fsi) * 2;
                uncompress(pDest1, &destLen1, (unsigned char*)pDest, destLen);
                memcpy(&m_fightMsg.fsi, pDest1, sizeof(m_fightMsg.fsi));
            }
            else {
                std::string str = logInfo["result2"].asString();
                
                unsigned long destLen = 0;
                Base64::Decode((unsigned char*)str.c_str(), strlen(str.c_str()), pDest, destLen);
                
                unsigned long destLen1 = sizeof(m_fightMsg.fsi) * 2;
                uncompress(pDest1, &destLen1, (unsigned char*)pDest, destLen);
                memcpy(&m_fightMsg.fsi, pDest1, sizeof(m_fightMsg.fsi));
            }
            
            free(pDest);
            free(pDest1);
            
        }
        
        int fighttype = 0;
        
        FightUserInfoEx2 userInfo;
        if (m_fightMsg.fsi.user[0].uid == m_chatData.uid) {
            userInfo = m_fightMsg.fsi.user[1];
            fighttype = 1;
        }
        else {
            userInfo = m_fightMsg.fsi.user[0];
            fighttype = 0;
        }
        
        m_pRivalName->setString(userInfo.name);
        
        char str[128];
        sprintf(str, "player_mr_%d", userInfo.military);
        CCSpriteFrame *pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pRivalMilitary->setDisplayFrame(pFrame);
        
        m_pClanIcon->setVisible(false);
        m_pClanName->setVisible(false);
        if (userInfo.clanid > 0) {
            
            char str[128];
            sprintf(str, "clan_icon_%d", userInfo.clanflag);
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
            m_pClanIcon->setDisplayFrame(pFrame);
            m_pClanName->setString(userInfo.clanname);
            
            m_pClanIcon->setVisible(true);
            m_pClanName->setVisible(true);
        }
        
        for (int star = 1; star <= 3; star ++) {
            if (m_fightMsg.star >= star)
            {
                CCSpriteFrame* spriteFrame = ImgMgr::getSingleton().getFrame("star_s_1");
                CCSprite* pStar = (CCSprite*)m_pBattleStars->getChildByTag(star);
                pStar->setDisplayFrame(spriteFrame);
            }
        }
        
        int wincup = 0;
        if (fighttype == 0) {
            wincup = m_fightMsg.defendCup;
            m_pFightAgainst->setString(ViewString::getValue("share_defend_against").c_str());
        }
        else {
            wincup = m_fightMsg.attackCup;
            m_pFightAgainst->setString(ViewString::getValue("share_attack_against").c_str());
        }
        
        sprintf(str, "%d", wincup);
        m_pGainCup->setString(str);
        
        sprintf(str, "%d%%", (int)(m_fightMsg.fsi.progress * 100));
        m_pBattleProgress->setString(str);
        
        if (!m_fightMsg.canReplay()) {
            m_pBtnReplay->setVisible(false);
        }
    }

    char str[128];
    sprintf(str, "player_mr_%d", m_chatData.military);
    CCSpriteFrame *frame = ImgMgr::getSingleton().getFrame(str);
    m_pMilitaryIcon->setDisplayFrame(frame);
}

void ChatItem::setViewSize(CCSize viewSize)
{
    int offY = viewSize.height - this->getContentSize().height;
    if(m_chatData.type == 0) {
        m_pChatTime->setPositionY(m_pChatTime->getPositionY()-offY);
        m_pSplitLine->setPositionY(m_pSplitLine->getPositionY()-offY);
        
        m_pView->setPositionY(m_pView->getPositionY()+offY);
    }
    
    this->setContentSize(viewSize);
}

bool ChatItem::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    if  (m_pChatUI->m_pageIndex == m_type)
    {
        if (isTouchInside(pTouch)) {
            
            m_dragOffset = m_pChatUI->m_pClanChatView->getContentOffset();
            m_isDrag = false;
            return true;
        }
    }
    
    return false;
}

void ChatItem::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    if (fabs(m_dragOffset.y - m_pChatUI->m_pClanChatView->getContentOffset().y) > 10) {
        m_isDrag = true;
    }
}

void ChatItem::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    if (m_isDrag) {
        return;
    }
    
    CCSize cellSize = this->getContentSize();
    
    ItemMenuUI* pItemMenuUI = m_pChatUI->m_pItemMenu;
    
    if (m_type == 0) {
        pItemMenuUI->resetMenu(m_chatData.uid, m_chatData.uname.c_str(), m_chatData.clanInfo.clanid, ClanMgr::getSingleton().m_clanjob);
        pItemMenuUI->setClanInfo(m_chatData.clanInfo.clanname, m_chatData.clanInfo.clanflag, 0, 0, 0);
    }
    else {
        pItemMenuUI->resetMenu(m_chatData.uid, m_chatData.uname.c_str(), ClanMgr::getSingleton().m_myclan.id);
    }
    
    pItemMenuUI->setPositionX(0.4*cellSize.width);
    
    if(m_type == 0)
    {
        pItemMenuUI->setPositionY(this->getPositionY() + m_pChatUI->m_pWorldChatView->getContentOffset().y + cellSize.height/2);
    }
    else {
        pItemMenuUI->setPositionY(this->getPositionY() + m_pChatUI->m_pClanChatView->getContentOffset().y + cellSize.height/2);
    }

    pItemMenuUI->adjuestPosition();
}

void fadeOutAllChildrenAction(CCNode* pNode, float dt)
{
    CCObject* pObj;
    CCArray* pChildren = pNode->getChildren();
    CCARRAY_FOREACH(pChildren, pObj)
    {
        CCNode* pChild = (CCNode*)pObj;
        
        CCFadeOut* pFadeOut = CCFadeOut::create(dt);
        pChild->runAction(pFadeOut);
        
        if (pChild->getChildrenCount() > 0) {
            fadeOutAllChildrenAction(pChild, dt);
        }
    }
}

void ChatItem::addDeleteFadeOutAction()
{
    fadeOutAllChildrenAction(this, 1.0);
    
    CCFadeOut* pFadeOut = CCFadeOut::create(1.0);
    CCCallFunc* pCallFunc = CCCallFunc::create(this, callfunc_selector(ChatItem::onDeleteActionCallback));
    
    CCSequence* pSequence = CCSequence::create(pFadeOut, pCallFunc, NULL);
    this->runAction(pSequence);
    
    m_isDelete = true;
    
    if (m_chatData.type == CHAT_TYPE_INVATE) {
        m_pBtnAccept->setEnabled(false);
        m_pBtnReject->setEnabled(false);
    }
}

void ChatItem::onDeleteActionCallback()
{
    ChatUI* pChatUI = PlayerScene::getSingleton().m_pGameWorld->m_pChatUI;
    
    map<int, ChatCell>::iterator it = pChatUI->m_mapClanChat.find(m_chatData.cid);
    
    this->removeFromParent();
    pChatUI->m_mapClanChat.erase(it);
    
    CCPoint offset = pChatUI->m_pClanChatView->getContentOffset();
    offset.y += getContentSize().height;
    
    pChatUI->m_pClanChatView->setContentOffset(offset);
    
    pChatUI->refreshChatList();
}

bool ChatItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerName", CCLabelTTF*, m_pPlayerName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RivalName", CCLabelTTF*, m_pRivalName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Message", CCLabelTTF*, m_pMessage);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RivalMilitary", CCSprite*, m_pRivalMilitary);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "StatusMessage", CCLabelTTF*, m_pStatusMessage);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "HouseSpace", CCLabelBMFont*, m_pHouseSpace);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "SpaceProg", CCSprite*, m_pSpaceProg);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "SpaceProgBg", CCSprite*, m_pSpaceProgBg);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleStars", CCNode*, m_pBattleStars);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleProgress", CCLabelBMFont*, m_pBattleProgress);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "FightAgainst", CCLabelBMFont*, m_pFightAgainst);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GainCup", CCLabelBMFont*, m_pGainCup);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "MemberType", CCLabelBMFont*, m_pMemberType);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ChatTime", CCLabelBMFont*, m_pChatTime);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "SplitLine", CCSprite*, m_pSplitLine);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "IconMilitary", CCSprite*, m_pMilitaryIcon);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnReplay", CCControlButton*, m_pBtnReplay);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnDonate", CCControlButton*, m_pBtnDonate);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnReject", CCControlButton*, m_pBtnReject);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnAccept", CCControlButton*, m_pBtnAccept);
    
    return false;
}

SEL_CCControlHandler ChatItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuDonateCallback", ChatItem::menuDonateCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuRejectCallback", ChatItem::menuRejectCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuAcceptCallback", ChatItem::menuAcceptCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuReplayCallback", ChatItem::menuReplayCallback);
    
    return NULL;
}

void ChatItem::menuDonateCallback(CCObject* pSender, CCControlEvent event)
{
    m_pChatUI->m_pDonateUI->setVisible(true);
    
    float offY = m_pChatUI->m_pClanChatView->getContentOffset().y;
    
    float posX = m_pChatUI->getContentSize().width * 0.4;
    float posY = this->getPositionY() + offY - this->getContentSize().height/2;
    
    if (posY < 0) {
        posY = 0;
    }
    
    requestHelpUserId = m_chatData.uid;
    
    m_pChatUI->m_pDonateUI->setPosition(ccp(posX, posY));
    m_pChatUI->m_pDonateUI->refresh(m_troopsData);
}

void ChatItem::menuRejectCallback(CCObject* pSender, CCControlEvent event)
{
    if (m_chatData.type == CHAT_TYPE_JOIN_CLAN) {
         ClanMgr::getSingleton().rejectJoin(m_chatData.cid, m_chatData.uid);
    }
    else if (m_chatData.type == CHAT_TYPE_INVATE) {
        ChatMgr::getSingleton().rejectInvite(m_chatData.cid);
    }
}

void ChatItem::menuAcceptCallback(CCObject* pSender, CCControlEvent event)
{
    if (m_chatData.type == CHAT_TYPE_JOIN_CLAN) {
        ClanMgr::getSingleton().acceptJoin(m_chatData.cid, m_chatData.uid);
    }
    else if (m_chatData.type == CHAT_TYPE_INVATE) {
        ChatMgr::getSingleton().acceptInvite(m_chatData.cid);
    }
}

void ChatItem::menuReplayCallback(CCObject* pSender, CCControlEvent event)
{
    if (m_fightMsg.fsi.user[0].uid == m_chatData.uid)
    {
        LogicInfo::getSingleton().m_fightReplayType = FIGHT_REPLAY_ATTACK;
        LogicInfo::getSingleton().m_winCups = m_fightMsg.attackCup;
    }
    else
    {
        LogicInfo::getSingleton().m_fightReplayType = FIGHT_REPLAY_DEFENSE;
        LogicInfo::getSingleton().m_winCups = m_fightMsg.defendCup;
    }

    LogicInfo::getSingleton().chg2Replay(m_fightMsg, ATTACK_TYPE_PVP);
    
	PlayerScene::getSingleton().m_pGameWorld->hideChatUI();
    PlayerScene::getSingleton().m_pGameWorld->hideMainUI();
    
    PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_NORMAL);
}

DonateUI::DonateUI()
{
    
}

DonateUI::~DonateUI()
{
    
}

bool DonateUI::init()
{
    if (CUINode::init()) {
        
        CCNode* pDonateNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("donate_bg.ccbi", this);

        this->setContentSize(CCDirector::sharedDirector()->getWinSize());
        this->setTargetTouch(true);

        this->addChild(pDonateNode);
        
        CCPoint pos(10, m_pDonateView->getContentSize().height - 10);
        
        for(int i = 0; i < 10; ++i)
        {
            int sid = 10000 + (i+1) * 100 + 1;
            int sodierId = LogicInfo::getSingleton().getMySoldierLevel(sid);
            
            DonateItem* donateItem = DonateItem::create(sodierId);
            donateItem->setTag(sodierId/100);
            donateItem->setPositionX(pos.x + (i % 5) * (donateItem->getContentSize().width + 10));
            donateItem->setPositionY(pos.y - (i / 5) * donateItem->getContentSize().height);
            
            m_pDonateView->addChild(donateItem);
        }
    }
    
    return true;
}

void DonateUI::refresh(ClanTroopsData troopsData)
{
    //reset donate info
    for (int i = 0; i < m_pDonateView->getChildrenCount(); i++) {
        DonateItem* pDonateItem = (DonateItem*)m_pDonateView->getChildren()->objectAtIndex(i);
        
        pDonateItem->m_donateNums = 0;
        pDonateItem->m_donateLeft = 0;
    }
        
    LogicInfo::getSingleton().refurbishFightSoldierInfo();
    list<FightSoldierInfoEx> lstSoldier = LogicInfo::getSingleton().m_lstFightSoldier;
    for(std::list<FightSoldierInfoEx>::iterator it = lstSoldier.begin(); it != lstSoldier.end(); ++it)
    {
        if (it->pid < 30000) {
            
            DonateItem* pDonateItem = (DonateItem*)m_pDonateView->getChildByTag((it->pid / 100));
            
            pDonateItem->m_sodierId = it->pid;
            pDonateItem->m_donateLeft = it->numsex;
        }
    }
    
    for (int i = 0; i < troopsData.lstSoldier.size(); i++) {
        int uid = troopsData.lstSoldier[i].uid;
        int sid = troopsData.lstSoldier[i].sid;
        if (uid == LogicInfo::getSingleton().m_myInfo.uid && sid != 0) {
            
            DonateItem* pDonateItem = (DonateItem*)m_pDonateView->getChildByTag(sid/100);
            pDonateItem->m_donateNums ++;
        }
    }
    
    for (int i = 0; i < m_pDonateView->getChildrenCount(); i++) {
        DonateItem* pDonateItem = (DonateItem*)m_pDonateView->getChildren()->objectAtIndex(i);
        pDonateItem->refresh();
        
        SoldierInfo info = *PersonMgr::getSingleton().getSoldierInfo(pDonateItem->m_sodierId);
        if ((troopsData.maxspace - troopsData.curspace) < info.housingSpaceex) {
            pDonateItem->m_pBtnDonate->setEnabled(false);
        }
    }
    
    char str[32];
    sprintf(str, "%d", troopsData.donateNums);
    m_pDonateNums->setString(str);
    
    if (troopsData.donateNums >= 5 || troopsData.curspace >= troopsData.maxspace) {
        this->setVisible(false);
    }

}

bool DonateUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    if (isVisible()) {
        if (!isTouchInside(pTouch)) {
            this->setVisible(false);
        }
        
        return true;
    }
    
    return false;
}

void DonateUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    this->setVisible(false);
}

bool DonateUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DonateNums", CCLabelBMFont*, m_pDonateNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DonateView", CCLayer*, m_pDonateView);
    
    return false;
}

SEL_CCControlHandler DonateUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", DonateUI::menuCloseCallback);
    return NULL;
}


DonateItem::DonateItem(int sodierId)
: m_donateLeft(0)
, m_donateNums(0)
, m_sodierId(sodierId)
{

}

DonateItem::~DonateItem()
{
    
}

DonateItem* DonateItem::create(int sodierId)
{
    DonateItem *pRet = new DonateItem(sodierId);
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool DonateItem::init()
{
    if (CUINode::init()) {
        
        CCNode* itemNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("donate_item.ccbi", this);
        this->setContentSize(itemNode->getContentSize());
        this->addChild(itemNode);
        
        char str[128];
        sprintf(str, "player1_%d", m_sodierId/100);
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        sprintf(str, "player1_%d_0", m_sodierId/100);
        CCSpriteFrame* pDisableFrame = ImgMgr::getSingleton().getFrame(str);
        
        m_pBtnDonate->setBackgroundSpriteFrameForState(pFrame, CCControlStateNormal);
        m_pBtnDonate->setBackgroundSpriteFrameForState(pFrame, CCControlStateHighlighted);
        m_pBtnDonate->setBackgroundSpriteFrameForState(pDisableFrame, CCControlStateDisabled);
        
        sprintf(str, "%d", m_donateLeft);
        m_pDonateLeft->setString(str);
        
        sprintf(str, "%d", m_donateNums);
        m_pDonateNums->setString(str);
        
        return true;
    }
    
    return false;
}

void DonateItem::refresh()
{
    char str[128];
    
    sprintf(str, "%d", m_donateLeft);
    m_pDonateLeft->setString(str);
    
    if (m_donateNums > 0) {
        sprintf(str, "%d", m_donateNums);
        m_pDonateNums->setString(str);
        
        m_pDonateNums->setVisible(true);
    }
    else{
        m_pDonateNums->setVisible(false);
    }
    
    if (m_donateLeft > 0) {
        m_pBtnDonate->setEnabled(true);
    }
    else{
        m_pBtnDonate->setEnabled(false);
    }
}

void DonateItem::menuDonateCallback(CCObject *pSender, CCControlEvent event)
{
    if(m_donateLeft > 0)
    {
        ChatMgr::getSingleton().sendHelpSoldier(requestHelpUserId, m_sodierId, m_donateLeft);
    }
    
    refresh();
}

bool DonateItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DonateNums", CCLabelBMFont*, m_pDonateNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DonateLeft", CCLabelBMFont*, m_pDonateLeft);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnDonate", CCControlButton*, m_pBtnDonate);
        
    return false;
}

SEL_CCControlHandler DonateItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuDonateCallback", DonateItem::menuDonateCallback);
    return NULL;
}
