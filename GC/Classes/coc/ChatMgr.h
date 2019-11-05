//
//  ChatMgr.h
//  GameClient
//
//  Created by qsun on 14-3-10.
//
//

#ifndef __GameClient__ChatMgr__
#define __GameClient__ChatMgr__

#include <algorithm>
#include "BaseDef.h"
#include "Mod_WorldChat.h"
#include "Mod_Clan.h"

NS_COC_BEGIN

#define CHAT_SHOW_MAXNUMS 50

class ChatMgr
{
public:
	
    ChatMgr();
    ~ChatMgr();
public:
	static ChatMgr& getSingleton();
    
    void requestSoldier(ChatData chatData, int gem);
    void sendHelpSoldier(int uid, int sodierId, int donateLeft);
    void joinClan(int clanid);
    
    void sendInvite(int uid);
    void acceptInvite(int chatid);
    void rejectInvite(int chatid);
    
    void sendShareReplay(int battleid, const char* message);
    
    void sendClanChat(ChatData chatData);
    void sendWorldChat(ChatData chatData);
    
    void onRecvWorldChat(ChatData chatData);
    void onRecvClanChat(ChatData chatData);
    void onRecvUpdateChat(ChatData chatData);
    void onRecvDeleteChat(int chatId);
    
    void onClearClanChat();
    
    void onRecvClanSoldier(ClanTroopsData troopsData);
    
    ClanTroopsData getClanTroopsData(int uid);
    
public:
    //Mod_WorldChat modWorldChat;
    //Mod_Clan modClan;
    
    map<int, ChatData> m_mapWorldChat;
    map<int, ChatData> m_mapClanChat;
    map<int, ClanTroopsData> m_mapClanTroopsData;
};

NS_COC_END

#endif /* defined(__GameClient__ChatMgr__) */
