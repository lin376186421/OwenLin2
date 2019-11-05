#include "SocketClient.h"
#include "LogicInfo.h"
#include "ChatMgr.h"
#include "PlayerScene.h"
#include "GameWorldScene.h"
#include "../form/NotifyUI.h"
#include "../form/MessageUI.h"

NS_COC_BEGIN

#define SOCKET_TIME_OUT 15

int s_socketClock = 0;

static pthread_t s_socketThread;
static void* SocketThread(void *data);

SocketClient& SocketClient::getSingleton()
{
	static SocketClient s_client;
    
	return s_client;
}

SocketClient::SocketClient() 
: m_isConnected(false)
, m_socket(NULL)
{
	addMod(&m_modUser);
	addMod(&m_modWorldChat);
	addMod(&m_modClan);
    
    pthread_create(&s_socketThread, NULL, SocketThread, NULL);
    pthread_detach(s_socketThread);
}

SocketClient::~SocketClient()
{
    m_socket->close();
}

bool SocketClient::init()
{
    if (m_socket == NULL) {
        
        connect();
    }

	return true;
}

void SocketClient::addListener(SocketListener* pListener)
{
    m_listener.push_back(pListener);
}

void SocketClient::removeListener(SocketListener* pListener)
{
    for (list<SocketListener*>::iterator it = m_listener.begin(); it != m_listener.end(); it ++) {
        if ((*it) == pListener) {
            pListener->m_isRelease = true;
            (*it) = NULL;
        }
    }
}

void SocketClient::onStateChange()
{
    for (list<SocketListener*>::iterator it = m_listener.begin(); it != m_listener.end();) {
        if ((*it) != NULL) {
            (*it)->onStateChange(m_socket->getReadyState());
        }
        
        if ((*it) == NULL) {
            it = m_listener.erase(it);
        }
        else {
            ++it;
        }
    }
}

static void* SocketThread(void *data)
{
    while (true) {
        
        s_socketClock += 1;
        
        if (s_socketClock >= SOCKET_TIME_OUT) {
            if (SocketClient::getSingleton().isDisconnect()) {
                if (SocketClient::getSingleton().m_socket == NULL) {
                    SocketClient::getSingleton().connect();
                }
                else{
                    SocketClient::getSingleton().m_socket->close();
                }
            }
            s_socketClock = 0;
        }
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        Sleep(1000);
#else
        sleep(1);
#endif // _MSC_VER
        
    }
    
    return NULL;
}

bool SocketClient::connect()
{    
    s_socketClock = 0;
    
    if (m_socket == NULL) {
        m_socket = new WebSocket();
    }
    
    if(!m_socket->init(*this, SOCKET_SERVER_URL))
    {
        CC_SAFE_DELETE(m_socket);
        
        return false;
    }
    
    return true;
}

bool SocketClient::send(Json::Value val)
{
    if (m_socket) {
        
        val["military"] = LogicInfo::getSingleton().m_myInfo.military;
        
        Json::FastWriter w;
        std::string strmsg = w.write(val);
        
        m_socket->send(strmsg);
        
        return true;
    }
    
    if (isDisconnect()) {
        if (PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI) {
            PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI->addNote(ViewString::getValue("socket_disconnect_tip").c_str());
        }
    }
    
    return false;
}

bool SocketClient::isConnect()
{
    return m_isConnected;
}

bool SocketClient::isDisconnect()
{
    return !m_isConnected;
}

void SocketClient::onNeedLogin()
{
	if(LogicInfo::getSingleton().m_myInfo.sessionid <= 0)
		return;

	if(!m_isConnected)
		return;

	m_modUser.login(LogicInfo::getSingleton().m_myInfo.uid, LogicInfo::getSingleton().m_myInfo.sessionid);
}

void SocketClient::onOpen(WebSocket* ws)
{
	CCLOG("socketclient connected.");

	m_isLogin = false;
	m_isConnected = true;

	onNeedLogin();
    
    onStateChange();
}

void SocketClient::onClose(WebSocket* ws)
{
	m_isConnected = false;
	m_isLogin = false;
    
    if (ws->getReadyState() == WebSocket::kStateClosed) {
        
        if (ws == m_socket) {
            m_socket = NULL;
            
            connect();
        }
        
        onStateChange();
        
        //sleep 1s wait socket quit thread
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        Sleep(1000);
#else
        sleep(1);
#endif
        CC_SAFE_DELETE(ws);
        
        CCLOG("socket closed.");
    }
}

void SocketClient::onError(WebSocket* ws, const WebSocket::ErrorCode& error)
{
    if (error == WebSocket::kErrorConnectionFailure)
    {
        ws->close();
    }
    
    CCLOG("socket error, error code: %d", error);
}

void SocketClient::onMessage(WebSocket* ws, const WebSocket::Data& data)
{
	std::string str(data.bytes, data.len);

	Json::Reader r;
	Json::Value val;
        
	if(r.parse(str, val))
	{
		for(_ModList::iterator it = m_lstMod.begin(); it != m_lstMod.end(); ++it)
		{
			(*it)->onMessage(val);
		}
	}
    
    CCLOG("socket recv message -> %s", str.c_str());
}

void SocketClient::addMod(NetModBase* pMod)
{
	m_lstMod.push_back(pMod);
}

NS_COC_END