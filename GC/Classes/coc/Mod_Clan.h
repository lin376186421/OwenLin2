#ifndef __COC_MOD_CLAN_H__
#define __COC_MOD_CLAN_H__

#include "BaseDef.h"
#include "NetModBase.h"

NS_COC_BEGIN

class Mod_Clan : public NetModBase{
	friend class SocketClient;
private:
	Mod_Clan() {}
	virtual ~Mod_Clan() {}
public:
	void sendChat(const char* str);

	void createClan(int clanid, const char* name);

	void joinClan(int clanid);
    
    void inviteJoin(int destuid);
    
    void acceptJoin(int chatid, int destuid);
    
    void rejectJoin(int chatid, int destuid);
    
    void acceptInvite(int chatid);
    
    void rejectInvite(int chatid);

	void leftClan(int clanid);

	void kickUser(int destuid, string uname);

	void chgUserRight(int destuid, string uname, int rightcode);

	void requestSoldier(const char* chat, int gem);

	void levelupCastle(int castlelevel);

	void giveSoldier(int destuid, int soldierid, int donateLeft);

	void resetCastleInfo();
    
    void shareReplay(int battleid, const char* message);
public:
	virtual bool onMessage(const Json::Value& val);
};

NS_COC_END

#endif // __COC_MOD_CLAN_H__