//
//  ChatMgr.cpp
//  GameClient
//
//  Created by qsun on 14-3-10.
//
//

#include "ChatMgr.h"
#include "ClanMgr.h"
#include "PlayerScene.h"
#include "../form/ChatUI.h"
#include "../GameWorldScene.h"
#include "SocketClient.h"

NS_COC_BEGIN

ChatMgr& ChatMgr::getSingleton()
{
	static ChatMgr s_mgr;
	return s_mgr;
}

ChatMgr::ChatMgr()
{
    
}

void ChatMgr::requestSoldier(ChatData chatData, int gem)
{
#if(COC_CLAN == 1)
    SocketClient::getSingleton().m_modClan.requestSoldier(chatData.message.c_str(), gem);
#endif
}

void ChatMgr::sendHelpSoldier(int uid, int sodierId, int donateLeft)
{
#if(COC_CLAN == 1)
    SocketClient::getSingleton().m_modClan.giveSoldier(uid, sodierId, donateLeft);
#endif
}

void ChatMgr::joinClan(int clanid)
{
#if(COC_CLAN == 1)
    SocketClient::getSingleton().m_modClan.joinClan(clanid);
#endif
}

void ChatMgr::sendInvite(int uid)
{
    SocketClient::getSingleton().m_modClan.inviteJoin(uid);
}

void ChatMgr::acceptInvite(int chatid)
{
    SocketClient::getSingleton().m_modClan.acceptInvite(chatid);
}

void ChatMgr::rejectInvite(int chatid)
{
    SocketClient::getSingleton().m_modClan.rejectInvite(chatid);
}

void ChatMgr::sendShareReplay(int battleid, const char* message)
{
    SocketClient::getSingleton().m_modClan.shareReplay(battleid, message);
}

void ChatMgr::sendClanChat(ChatData chatData)
{
#if(COC_CLAN == 1)
    SocketClient::getSingleton().m_modClan.sendChat(chatData.message.c_str());
#endif
}

void ChatMgr::sendWorldChat(ChatData chatData)
{
#if(COC_CLAN == 1)
	SocketClient::getSingleton().m_modWorldChat.sendChat(chatData.message.c_str());
#endif
}

void ChatMgr::onRecvWorldChat(ChatData chatData)
{
#if(COC_CLAN == 1)
    
    if (m_mapWorldChat.find(chatData.cid) != m_mapWorldChat.end()) {
        return;
    }
    
    chatData.isShow = false;
    m_mapWorldChat.insert(pair<int, ChatData>(chatData.cid, chatData));
    
    if (m_mapWorldChat.size() > CHAT_SHOW_MAXNUMS) {
        m_mapWorldChat.erase(m_mapWorldChat.begin());
    }
    
    if (PlayerScene::getSingleton().m_pGameWorld) {
        if (PlayerScene::getSingleton().m_pGameWorld->m_pChatUI) {
            PlayerScene::getSingleton().m_pGameWorld->m_pChatUI->addWorldChat(chatData);
        }

        if (PlayerScene::getSingleton().m_pGameWorld->m_pMainUI) {
            PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->countChatNums(0, chatData);
        }
    }
#endif
}

void ChatMgr::onRecvClanChat(ChatData chatData)
{
#if(COC_CLAN == 1)
    
    if (ClanMgr::getSingleton().m_castleLevel <= 1) {
        return;
    }
    
    if (m_mapClanChat.find(chatData.cid) != m_mapClanChat.end()) {
        return;
    }
    
    chatData.isShow = false;
    m_mapClanChat.insert(pair<int, ChatData>(chatData.cid, chatData));
    
    if (m_mapClanChat.size() > CHAT_SHOW_MAXNUMS) {
        m_mapClanChat.erase(m_mapClanChat.begin());
    }
    
    if (chatData.type == CHAT_TYPE_HELP) {
     
        map<int, ClanTroopsData>::iterator it = m_mapClanTroopsData.find(chatData.uid);
        if (it != m_mapClanTroopsData.end()) {
            
            if (it->second.cid > 0) { //update request soldier chat
                map<int, ChatData>::iterator chatIt = m_mapClanChat.find(it->second.cid);
                if (chatIt != m_mapClanChat.end()) {
                    
                    if (PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pChatUI)
                    {
                        PlayerScene::getSingleton().m_pGameWorld->m_pChatUI->deleteClanChat(chatIt->second, true);
                    }
                    m_mapClanChat.erase(chatIt);
                }
            }
            
            it->second.cid = chatData.cid;
        }
    }
    
    if (PlayerScene::getSingleton().m_pGameWorld) {
        
        if (PlayerScene::getSingleton().m_pGameWorld->m_pChatUI) {
            PlayerScene::getSingleton().m_pGameWorld->m_pChatUI->addClanChat(chatData);
        }

        if (PlayerScene::getSingleton().m_pGameWorld->m_pMainUI) {
            PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->countChatNums(1, chatData);
        }
    }
    
#endif
}

void ChatMgr::onRecvClanSoldier(ClanTroopsData troopsData) {
#if(COC_CLAN == 1)
    
    map<int, ClanTroopsData>::iterator it = m_mapClanTroopsData.find(troopsData.uid);
    if (it != m_mapClanTroopsData.end()) {

        troopsData.cid = it->second.cid;
        it->second = troopsData;
        
        map<int, ChatData>::iterator it1 = m_mapClanChat.find(troopsData.cid);
        if (it1 != m_mapClanChat.end()) {

            if (PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pChatUI && it1->second.isShow) {
                PlayerScene::getSingleton().m_pGameWorld->m_pChatUI->updateClanChat(it1->second);
            }
        }
    }
    else{
        m_mapClanTroopsData.insert(pair<int, ClanTroopsData>(troopsData.uid, troopsData));
    }
#endif
}

void ChatMgr::onRecvUpdateChat(ChatData chatData)
{
    map<int, ChatData>::iterator it = m_mapClanChat.find(chatData.cid);
    if (it != m_mapClanChat.end()) {
        
        if (PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pChatUI && it->second.isShow) {
            PlayerScene::getSingleton().m_pGameWorld->m_pChatUI->updateClanChat(chatData);
        }
        
        it->second = chatData;
    }
}

void ChatMgr::onRecvDeleteChat(int chatId)
{
    map<int, ChatData>::iterator it = m_mapClanChat.find(chatId);
    if (it != m_mapClanChat.end()) {
        
        if (PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pChatUI && it->second.isShow) {
            PlayerScene::getSingleton().m_pGameWorld->m_pChatUI->deleteClanChat(it->second);
        }
        
        m_mapClanChat.erase(it);
    }
}

void ChatMgr::onClearClanChat()
{
    m_mapClanChat.clear();
    m_mapClanTroopsData.clear();
    
    if(PlayerScene::getSingleton().m_pGameWorld != NULL && PlayerScene::getSingleton().m_pGameWorld->m_pChatUI != NULL)
    {
        PlayerScene::getSingleton().m_pGameWorld->m_pChatUI->clearClanChat();
    }
}

ClanTroopsData ChatMgr::getClanTroopsData(int uid)
{
    ClanTroopsData troopsData;
    map<int, ClanTroopsData>::iterator it = m_mapClanTroopsData.find(uid);
    if (it != m_mapClanTroopsData.end()) {
        troopsData = it->second;
    }
    return troopsData;
}

ChatMgr::~ChatMgr()
{
    
}

NS_COC_END