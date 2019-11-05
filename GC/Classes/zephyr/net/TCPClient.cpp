/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "TCPClient.h"
#include "../Zephyr.h"
#include "BaseMsgDef.h"
#include "../Lock.h"

NS_CC_ZEPHYR_BEGIN

void TCPClient::initSystem()
{
#if (CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM)
	WORD ver = MAKEWORD(2, 0);
	WSADATA wsadata;

	if(WSAStartup(ver, &wsadata))
		return ;

	if(LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 0)
	{
		releaseSystem();

		return ;
	}
#endif
}

void TCPClient::releaseSystem()
{
#if (CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM)

	WSACleanup();

#endif
}

void* __func_ioctrl(void* pTCPClient)
{
	CCAssert(pTCPClient != NULL, "__func_ioctrl");

	TCPClient* pNetKernel = static_cast<TCPClient*>(pTCPClient);

	pNetKernel->func_IOCtrl();

	return NULL;
}

void* __func_connect(void* pTCPClient)
{
	CCAssert(pTCPClient != NULL, "__func_connect");

	TCPClient* pNetKernel = static_cast<TCPClient*>(pTCPClient);

	pNetKernel->func_Connect();

	return 0;
}

TCPClient::TCPClient(MsgProc* pProc, MsgDecoder* pDecoder)
	: NetKernel(pProc, pDecoder)
	, m_socket(0)
	, m_threadIOCtrl(__func_ioctrl, this)
	, m_threadConnect(__func_connect, this)
//		, m_threadHandleMsg(__func_handlemsg, this)
	, m_bConnected(false)
	, m_pData(NULL)
	, m_sizeBuff(32 * 1024)
	, m_ui8RecvMsgID(0x7b)
	, m_ui8SendMsgID(0x7b)
	, m_bDisconnectExitIOThread(true)
{
}

TCPClient::~TCPClient()
{
}

//! 端连接接口
//! 客户端如果连接到服务器，也会调用该接口
void TCPClient::onPeerConnect(void* pPeer)
{
	m_ui8RecvMsgID = 0x7b;
	m_ui8SendMsgID = 0x7b;

	//sockaddr_in addr;
	//int size = sizeof(addr);
	//getsockname(m_socket, (sockaddr*)&addr, &size);
	//m_addr.port = addr.sin_port;
	//m_addr.ip.ip = addr.sin_addr.S_un.S_addr;

	m_bConnected = true;

	PCMsg_PeerConnect msg;
	buildPCMsg_PeerConnect(msg, m_addr, true);
	onRecvMsg(&msg, msg.getSize());
}

//! 端断开连接接口
//! 客户端如果和服务器断开连接，也会调用该接口
void TCPClient::onPeerDisconnect(void* pPeer)
{
	m_bConnected = false;

	PCMsg_PeerDisconnect msg;
	buildPCMsg_PeerDisconnect(msg, m_addr);
	onRecvMsg(&msg, msg.getSize());
}

//! 断开连接
void TCPClient::cutPeer(void* pPeer)
{
#if (CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM)
	closesocket(m_socket);
#else
	close(m_socket);
#endif
}

//! SendMsg
int TCPClient::sendMsg(void* pPeer, void* pMsg, int msgSize)
{
	AutoLock<LockImp> lock(m_lockSendMsg);

	((MsgHead*)pMsg)->m_ui8MsgID = m_ui8SendMsgID++;

	encode(pMsg, msgSize, m_msgKey);

	int len = 0;
		
	do{
		int ret = send(m_socket, ((char*)pMsg + len), msgSize - len, 0);
		if(ret <= 0)
		{
			//int err = WSAGetLastError();

//			__OutputLog(ZSTR("Msg"), ZSTR("NetKernel_TCPClient %d sendmsg %d len is %d err is %d."), m_socket, msgSize, len, err);

			return 0;
		}

		len += ret;
	}while(len < msgSize);

	return len;
}

//! SendMsgEx
int TCPClient::sendMsgEx(void* pPeer, void* pMsg, int msgSize, bool bEncode)
{
	AutoLock<LockImp> lock(m_lockSendMsg);

	((MsgHead*)pMsg)->m_ui8MsgID = m_ui8SendMsgID++;

	if(bEncode)
		encode(pMsg, msgSize, m_msgKey);

	int len = 0;
		
	do{
		int ret = send(m_socket, ((char*)pMsg + len), msgSize - len, 0);
		if(ret <= 0)
		{
			//int err = WSAGetLastError();

			//__OutputLog(ZSTR("Msg"), ZSTR("NetKernel_TCPClient %d sendmsg %d len is %d err is %d."), m_socket, msgSize, len, err);

			return 0;
		}

		len += ret;
	}while(len < msgSize);

	return len;
}

//! 初始化
int TCPClient::init(const IPAddr& addr)
{
	m_addr = addr;

	return connect(m_addr);
}

//! Connect
int TCPClient::connect(const IPAddr& addr)
{
//		m_threadHandleMsg.Start();

	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(m_socket <= 0)
	{
		release();

		return -1;
	}

_reconnect:
	sockaddr_in saddr;

	memset(&saddr, 0 , sizeof(saddr));

	saddr.sin_family = AF_INET;
	saddr.sin_port = addr.port;
#if (CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM)
	saddr.sin_addr.S_un.S_addr = addr.ip.ip;
#else
	saddr.sin_addr.s_addr = addr.ip.ip;
#endif

	if(::connect(m_socket, (sockaddr*)&saddr, sizeof(saddr)) != 0)
	{
		//int err = WSAGetLastError();

		//if(err == 0x274c)
		{
#if (CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM)
			Sleep(100);
#else
			sleep(100);
#endif
			//Sleep(100);

			goto _reconnect;
		}

		release();

		PCMsg_PeerConnect msg;
		buildPCMsg_PeerConnect(msg, addr, false);

		onRecvMsg(&msg, msg.getSize());

		return -1;
	}

	//const char chOpt = 1;
	//setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, &chOpt, sizeof(char));

	//int nNetTimeout = 30000;		//! 1秒
	//setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&nNetTimeout, sizeof(int));
	//setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&nNetTimeout, sizeof(int));

	//! 发送和发送缓冲
	int nSize = m_sizeBuff;

	setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char*)&nSize, sizeof(int));
	setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char*)&nSize, sizeof(int));

	onPeerConnect(NULL);

	return 0;
}

//! 单独开线程初始化
int TCPClient::initEx(const IPAddr& addr)
{
	if(m_threadConnect.isRunning() || m_socket != 0)
		return -1;

	m_addr = addr;

	m_threadConnect.start();

	return 0;
}

//! 释放
void TCPClient::release()
{
	if(m_socket != 0)
	{
#if (CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM)
		closesocket(m_socket);
#else
		close(m_socket);
#endif
		m_socket = 0;
	}

	//m_recvBuff.Release();
	m_bConnected = false;
	m_pData = NULL;
}

//! 启动服务
void TCPClient::start()
{
	m_threadIOCtrl.start();
}

//! 停止服务
void TCPClient::stop(bool bWait)
{
	if(m_socket != 0)
	{
#if (CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM)
		closesocket(m_socket);
#else
		close(m_socket);
#endif

		m_socket = 0;
	}

	if(bWait)
	{
		m_threadConnect.waitForExitThread();
		m_threadIOCtrl.waitForExitThread();
		//m_threadHandleMsg.WaitForExitThread();
	}
	else
	{
		m_threadConnect.exitThread();
		m_threadIOCtrl.exitThread();
		//m_threadHandleMsg.ExitThread();
	}
}

//! Func IOCtrl
int TCPClient::func_IOCtrl()
{
	//return 0;
__thread_start:

	if(!m_bConnected)
		goto __thread_endex;

	while(true)
	{
		if(m_socket == 0)
		{
			break;
		}

		char buf[MAX_PKG_SIZE];
		int len = 0;
		int iCurLen = 0;

		//__OutputLog(ZNET3_LOG_LEVEL_MSG, ZSTR("ioctrl 0 %d"), ZephyrProc::VTime::GetTime());

		iCurLen = recv(m_socket, buf, sizeof(MsgHead), 0);
		if(iCurLen <= 0)
		{
			goto __thread_end;
		}

		//__OutputLog(ZNET3_LOG_LEVEL_MSG, ZSTR("ioctrl 1 %d"), ZephyrProc::VTime::GetTime());

		len += iCurLen;

		if(iCurLen == 0)
		{
			goto __thread_end;
		}
		else if(iCurLen < sizeof(MsgHead))
		{
			do{
				iCurLen = recv(m_socket, buf + len, sizeof(MsgHead) - len, 0);
				if(iCurLen <= 0)
				{
					goto __thread_end;
				}

				if(iCurLen == 0)
				{
					goto __thread_end;
				}

				len += iCurLen;
			}while(len < sizeof(MsgHead));
		}

		if(((MsgHead*)buf)->getSize() > MAX_PKG_SIZE)
		{
			goto __thread_end;
		}

		if(((MsgHead*)buf)->getSize() > len)
		{
			do{
				iCurLen = recv(m_socket, buf + len, ((MsgHead*)buf)->getSize() - len, 0);
				if(iCurLen <= 0)
				{
					goto __thread_end;
				}

				if(iCurLen == 0)
				{
					goto __thread_end;
				}

				len += iCurLen;
			}while(((MsgHead*)buf)->getSize() > len);
		}

		//__OutputLog(ZNET3_LOG_LEVEL_MSG, ZSTR("ioctrl 3 %d"), ZephyrProc::VTime::GetTime());

		decode(buf, len, m_msgKey);

		if(((MsgHead*)buf)->m_ui8MsgID != m_ui8RecvMsgID)
		{
			cutPeer(NULL);

			continue ;
		}

		++m_ui8RecvMsgID;

		//__OutputLog(ZNET3_LOG_LEVEL_MSG, ZSTR("ioctrl 4 %d"), ZephyrProc::VTime::GetTime());

		onRecvMsg(buf, len);

		//__OutputLog(ZNET3_LOG_LEVEL_MSG, ZSTR("ioctrl 5 %d"), ZephyrProc::VTime::GetTime());

		//m_recvStream.In(buf, len);

		//m_threadHandleMsg.WakeUp();

		//Decode(buf, len, m_msgKey);

		//HandleMessage(NULL, buf, len);

		if(m_threadIOCtrl._isEnd())
		{
			m_bDisconnectExitIOThread = true;

			break;
		}
	}

__thread_end:

	if(m_socket != 0)
	{
#if (CC_PLATFORM_WIN32 == CC_TARGET_PLATFORM)
		closesocket(m_socket);
#else
		close(m_socket);
#endif
	}

	onPeerDisconnect(NULL);

__thread_endex:
	if(!m_bDisconnectExitIOThread)
	{
		m_threadIOCtrl._sleep(10);

		goto __thread_start;
	}

	return 0;
}

//! Func Connect
int TCPClient::func_Connect()
{
	connect(m_addr);

	return 0;
}

//! 接收到一个消息
void TCPClient::onRecvMsg(void* pMsg, int msgLen)
{
	onHandleMessage(NULL, pMsg, msgLen);
}

NS_CC_ZEPHYR_END
