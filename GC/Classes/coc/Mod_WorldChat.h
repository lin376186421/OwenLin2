#ifndef __COC_MOD_WORLDCHAT_H__
#define __COC_MOD_WORLDCHAT_H__

#include "BaseDef.h"
#include "NetModBase.h"

NS_COC_BEGIN

class Mod_WorldChat : public NetModBase{
	friend class SocketClient;
private:
	Mod_WorldChat() {}
	virtual ~Mod_WorldChat() {}
public:
	void sendChat(const char* str);
public:
	virtual bool onMessage(const Json::Value& val);
};

NS_COC_END

#endif // __COC_MOD_WORLDCHAT_H__