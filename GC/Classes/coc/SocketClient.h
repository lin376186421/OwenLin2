#ifndef __COC_SOCKETCLIENT_H__
#define __COC_SOCKETCLIENT_H__

#include "BaseDef.h"
#include "network/websocket.h"
#include "NetModBase.h"
#include "Mod_User.h"
#include "Mod_WorldChat.h"
#include "Mod_Clan.h"
#include "SocketListener.h"

NS_COC_BEGIN

class SocketClient : public WebSocket::Delegate{
public:
	typedef std::vector<NetModBase*>	_ModList;
public:
	static SocketClient& getSingleton();
private:
	SocketClient();
	virtual ~SocketClient();
public:
	bool init();

    bool connect();
        
    bool send(Json::Value val);

	void addMod(NetModBase* pMod);
    
    bool isConnect();
    
    bool isDisconnect();
public:
	void onNeedLogin();

	void addListener(SocketListener* pListener);
    void removeListener(SocketListener* pListener);
    
    void onStateChange();
private:
	virtual void onOpen(WebSocket* ws);
	virtual void onMessage(WebSocket* ws, const WebSocket::Data& data);
	virtual void onClose(WebSocket* ws);
	virtual void onError(WebSocket* ws, const WebSocket::ErrorCode& error);
    
public:
	WebSocket*		m_socket;
    int             m_timeOut;
    
	bool			m_isConnected;
	bool			m_isLogin;
        
	_ModList		m_lstMod;

	Mod_User		m_modUser;
	Mod_WorldChat	m_modWorldChat;
	Mod_Clan		m_modClan;
    
    list<SocketListener*> m_listener;
    
    pthread_mutex_t m_mutex;
    pthread_t m_mutexThreadId;
};

NS_COC_END

#endif // __COC_SOCKETCLIENT_H__