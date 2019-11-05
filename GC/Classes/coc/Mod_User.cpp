#include "Mod_User.h"
#include "SocketClient.h"
#include "ClanMgr.h"
#include "ChatMgr.h"
#include "LogicInfo.h"

NS_COC_BEGIN

static char* _MOD_NAME = "user";

bool Mod_User::onMessage(const Json::Value& val)
{
	if("user" == val["mod"].asString())
	{
		if("loginret" == val["cmd"].asString()) {
			int isok = getAsInt(val["ret"]);
            
			SocketClient::getSingleton().m_isLogin = (isok != 0);

			return true;
		}
        else if ("ochat" == val["cmd"].asString()) {
            ChatData chatData;
            chatData.cid = getAsInt(val["msgid"]);
            chatData.type = getAsInt(val["type"]);
            chatData.uid = getAsInt(val["uid"]);
            chatData.uname = val["uname"].asString();
            chatData.military = getAsInt(val["military"]);
            chatData.message = val["chat"].asString();
            chatData.time = getAsInt(val["chattime"]);
            chatData.clanInfo.encodeInfo(val);
            
            chatData.status = getAsInt(val["state"]);
            chatData.statusMessage = val["statemsg"].asString();
            
            ChatMgr::getSingleton().onRecvClanChat(chatData);
        }
		else if("keeplive" == val["cmd"].asString())
		{
			keeplive();
		}
        else if ("updres" == val["cmd"].asString())
        {
            int gem = getAsInt(val["gem"]);
            if (gem != 0) {
                LogicInfo::getSingleton().chgGem(gem);
            }
            
            int gold = getAsInt(val["gold"]);
            if (gold != 0) {
                LogicInfo::getSingleton().chgGold(gold);
            }
            
            int oil = getAsInt(val["oil"]);
            if (oil != 0) {
                LogicInfo::getSingleton().chgOil(oil);
            }
        }
        
	}

	return false;
}

void Mod_User::login(int uid, int sid)
{
	Json::Value val;

	val["mod"] = _MOD_NAME;
	val["cmd"] = "login";
	val["uid"] = uid;
	val["sid"] = sid;

	SocketClient::getSingleton().send(val);
}

void Mod_User::keeplive()
{
	Json::Value val;

	val["mod"] = _MOD_NAME;
	val["cmd"] = "keeplive";

	SocketClient::getSingleton().send(val);
}

NS_COC_END