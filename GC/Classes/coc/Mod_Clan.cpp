 #include "Mod_Clan.h"
#include "SocketClient.h"
#include "ChatMgr.h"
#include "PersonMgr.h"
#include "LogicInfo.h"
#include "ClanMgr.h"
#include "PlayerScene.h"
#include "GameWorldScene.h"

NS_COC_BEGIN

static char* _MOD_NAME = "clan";

static bool GIVE_SOLDIER_STATE = 0;
static int GIVE_SOLDIER_STATE_TIME = 0;

void Mod_Clan::sendChat(const char* str)
{
	Json::Value val;

	val["mod"] = _MOD_NAME;
	val["cmd"] = "chat";
	val["chat"] = str;

	SocketClient::getSingleton().send(val);
}

void Mod_Clan::createClan(int clanid, const char* name)
{
	Json::Value val;

	val["mod"] = _MOD_NAME;
	val["cmd"] = "create";
	val["clanid"] = clanid;
	val["clanname"] = name;

	SocketClient::getSingleton().send(val);
}

void Mod_Clan::joinClan(int clanid)
{
	Json::Value val;

	val["mod"] = _MOD_NAME;
	val["cmd"] = "join";
	val["clanid"] = clanid;

	SocketClient::getSingleton().send(val);
}

void Mod_Clan::inviteJoin(int destuid)
{
    Json::Value val;
    
	val["mod"] = _MOD_NAME;
	val["cmd"] = "invite_join";
    val["uid"] = destuid;
    
	SocketClient::getSingleton().send(val);
}

void Mod_Clan::acceptJoin(int chatid, int destuid)
{
    Json::Value val;
    
	val["mod"] = _MOD_NAME;
	val["cmd"] = "join_accept";
    val["msgid"] = chatid;
    val["uid"] = destuid;
	val["uname"] = LogicInfo::getSingleton().m_myInfo.uname;
    
	SocketClient::getSingleton().send(val);
}

void Mod_Clan::rejectJoin(int chatid, int destuid)
{
    Json::Value val;
    
	val["mod"] = _MOD_NAME;
	val["cmd"] = "join_reject";
    val["msgid"] = chatid;
    val["uid"] = destuid;
	val["uname"] = LogicInfo::getSingleton().m_myInfo.uname;
    
	SocketClient::getSingleton().send(val);
}

void Mod_Clan::acceptInvite(int chatid)
{
    Json::Value val;
    
	val["mod"] = _MOD_NAME;
	val["cmd"] = "invite_accept";
    val["msgid"] = chatid;
    
	SocketClient::getSingleton().send(val);
}

void Mod_Clan::rejectInvite(int chatid)
{
    Json::Value val;
    
	val["mod"] = _MOD_NAME;
	val["cmd"] = "invite_reject";
    val["msgid"] = chatid;
    
	SocketClient::getSingleton().send(val);
}

void Mod_Clan::leftClan(int clanid)
{
	Json::Value val;

	val["mod"] = _MOD_NAME;
	val["cmd"] = "left";
	val["clanid"] = clanid;

	SocketClient::getSingleton().send(val);
}

void Mod_Clan::kickUser(int destuid, string uname)
{
	Json::Value val;

	val["mod"] = _MOD_NAME;
	val["cmd"] = "kick";
	val["destuid"] = destuid;
    val["uname"] = uname;

	SocketClient::getSingleton().send(val);
}

void Mod_Clan::chgUserRight(int destuid, string uname, int rightcode)
{
	Json::Value val;

	val["mod"] = _MOD_NAME;
	val["cmd"] = "chgrightcode";
	val["destuid"] = destuid;
    val["uname"] = uname;
	val["rightcode"] = rightcode;

	SocketClient::getSingleton().send(val);
}

void Mod_Clan::requestSoldier(const char* chat, int gem)
{
	Json::Value val;

	val["mod"] = _MOD_NAME;
	val["cmd"] = "requestsoldier";
	val["chat"] = chat;
    val["gem"] = gem;

	SocketClient::getSingleton().send(val);
}

bool Mod_Clan::onMessage(const Json::Value& val)
{
	if(val["mod"].asString() == "clan")
	{
		if(val["cmd"].asString() == "ochat")
		{
            ChatData chatData;
            chatData.cid = getAsInt(val["msgid"]);
            chatData.type = getAsInt(val["type"]);
            chatData.uid = getAsInt(val["uid"]);
            chatData.uname = val["uname"].asString();
            chatData.military = getAsInt(val["military"]);
            chatData.memberType = getAsInt(val["rightcode"]);
            chatData.message = val["chat"].asString();
            chatData.time = getAsInt(val["chattime"]);
            chatData.clanInfo.encodeInfo(val);
            
            chatData.status = getAsInt(val["state"]);
  
            chatData.statusMessage = val["statemsg"].asString();
            
            chatData.battleLog = val["battlelog"];
                        
            ChatMgr::getSingleton().onRecvClanChat(chatData);
		}
		else if(val["cmd"].asString() == "castleinfo")
		{
            ClanTroopsData troopsData;
            troopsData.cid = -1;
            troopsData.uid = getAsInt(val["uid"]);
            troopsData.maxspace = getAsInt(val["maxspace"]);
            troopsData.curspace = getAsInt(val["curspace"]);;
            troopsData.donateNums = 0;
            
            Json::Value troops = val["info"];
            
            int nums = troops.size();
            for (int i = 0; i < nums; i ++) {
                
                ClanSoldierInfo info;
                info.uid = getAsInt(troops[i]["uid"]);
                info.sid = getAsInt(troops[i]["soldier"]);
                
                troopsData.lstSoldier.push_back(info);
                if (info.uid == LogicInfo::getSingleton().m_myInfo.uid) {
                    troopsData.donateNums ++;
                }
            }
            
            ChatMgr::getSingleton().onRecvClanSoldier(troopsData);
            
            if (troopsData.uid == LogicInfo::getSingleton().m_myInfo.uid) {
                ClanMgr::getSingleton().onRefreshCastleSoldier(val);
            }
		}
        else if(val["cmd"].asString() == "givesoldier")
        {
            int uid = getAsInt(val["uid"]);
            string uname = val["uname"].asString();
            int sid = getAsInt(val["soldier"]);
            
            ClanMgr::getSingleton().giveSoldier(uid, uname, sid);
            
            GIVE_SOLDIER_STATE = 0;
        }
        else if (val["cmd"].asString() == "getsoldier")
        {
            int uid = getAsInt(val["uid"]);
            string uname = val["uname"].asString();
            int sid = getAsInt(val["soldier"]);
            
            ClanMgr::getSingleton().getSoldier(uid, uname, sid);
        }
        else if (val["cmd"].asString() == "delchat")
        {
            int cid = getAsInt(val["msgid"]);
            
            ChatMgr::getSingleton().onRecvDeleteChat(cid);
        }
        else if (val["cmd"].asString() == "updchat")
        {
            int cid = getAsInt(val["msgid"]);
            
            map<int, ChatData>::iterator it = ChatMgr::getSingleton().m_mapClanChat.find(cid);
            if (it != ChatMgr::getSingleton().m_mapClanChat.end()) {
                it->second.status = getAsInt(val["state"]);
                it->second.statusMessage = val["statemsg"].asString();
                
                ChatMgr::getSingleton().onRecvUpdateChat(it->second);
            }
        }
        else if (val["cmd"].asString() == "joinok")
        {
            ClanMgr::getSingleton().onProc(val);
        }
        else if (val["cmd"].asString() == "leftclan")
        {
            ClanMgr::getSingleton().leftClan();
        }
        else if (val["cmd"].asString() == "notifymsg")
        {
            int type = getAsInt(val["type"]);
            string msg = val["msg"].asString();
            
            if (type == 0) {
                if (PlayerScene::getSingleton().m_pGameWorld) {
                    PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI->addNote(msg.c_str());
                }
            }
            else if (type == 1) {
                MessageUI::show(MSG_NORMAL, NULL, msg.c_str());
            }
        }
        else if (val["cmd"].asString() == "chguser")
        {            
            int uid = getAsInt(val["uid"]);
            int state = getAsInt(val["state"]);
            
            ClanMgr::getSingleton().onMemberStage(uid, state);
        }
        else if (val["cmd"].asString() == "requesthelp")
        {
            ClanMgr::getSingleton().onRequestHelp();
        }
	}

	return false;
}

void Mod_Clan::levelupCastle(int castlelevel)
{
	Json::Value val;

	val["mod"] = _MOD_NAME;
	val["cmd"] = "castlelevelup";
	val["castlelevel"] = castlelevel;

	SocketClient::getSingleton().send(val);
}

void Mod_Clan::giveSoldier(int destuid, int soldierid, int donateLeft)
{
    if (getCurrentTime() - GIVE_SOLDIER_STATE_TIME >= 30) {
        GIVE_SOLDIER_STATE = 0;
    }
    else if (GIVE_SOLDIER_STATE == 1 && donateLeft < 5) {
        return;
    }
    
	Json::Value val;

	val["mod"] = _MOD_NAME;
	val["cmd"] = "addsoldier";
	val["destuid"] = destuid;
	val["soldierid"] = soldierid;

    SocketClient::getSingleton().send(val);
    
    if (SocketClient::getSingleton().isConnect() && donateLeft < 5) {
        GIVE_SOLDIER_STATE = 1;
        GIVE_SOLDIER_STATE_TIME = getCurrentTime();
    }
}

void Mod_Clan::resetCastleInfo()
{
	Json::Value val;

	val["mod"] = _MOD_NAME;
	val["cmd"] = "resetcastle";

	SocketClient::getSingleton().send(val);
}

void Mod_Clan::shareReplay(int battleid, const char* message)
{
    Json::Value val;
    
    val["mod"] = _MOD_NAME;
    val["cmd"] = "share_replay";
    val["battleid"] = battleid;
    val["chat"] = message;
    
	SocketClient::getSingleton().send(val);
}

NS_COC_END