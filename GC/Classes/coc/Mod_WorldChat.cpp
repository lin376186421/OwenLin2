#include "Mod_WorldChat.h"
#include "SocketClient.h"
#include "ChatMgr.h"

NS_COC_BEGIN

static char* _MOD_NAME = "worldchat";

void Mod_WorldChat::sendChat(const char* str)
{
	Json::Value val;

	val["mod"] = _MOD_NAME;
	val["cmd"] = "chat";
	val["chat"] = str;

	SocketClient::getSingleton().send(val);
}

bool Mod_WorldChat::onMessage(const Json::Value& val)
{
	if("worldchat" == val["mod"].asString())
	{
		if("ochat" == val["cmd"].asString())
		{
            ChatData chatData;
            chatData.type = CHAT_TYPE_NORMAL;
            chatData.cid = getAsInt(val["msgid"]);
            chatData.uid = getAsInt(val["uid"]);
            chatData.uname = val["uname"].asString();
            chatData.military = getAsInt(val["military"]);
            chatData.message = val["chat"].asString();
            chatData.time = getAsInt(val["chattime"]);
            chatData.clanInfo.encodeInfo(val);
            
            ChatMgr::getSingleton().onRecvWorldChat(chatData);
		}
	}

	return false;
}

NS_COC_END