#ifndef __COC_MOD_USER_H__
#define __COC_MOD_USER_H__

#include "BaseDef.h"
#include "NetModBase.h"

NS_COC_BEGIN

class Mod_User : public NetModBase{
	friend class SocketClient;
private:
	Mod_User() {}
	virtual ~Mod_User() {}
public:
	void login(int uid, int sid);
    
	void keeplive();
private:
	virtual bool onMessage(const Json::Value& val);
};

NS_COC_END

#endif // __COC_MOD_USER_H__