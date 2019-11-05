/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYR_NET_TCPCLIENT_H__
#define __ZEPHYR_NET_TCPCLIENT_H__

#include "../ZephyrBaseDef.h"
#include "NetKernel.h"
#include "MsgDecoderKey_Simple.h"
#include "../Lock.h"
#include "../Thread.h"

NS_CC_ZEPHYR_BEGIN

class TCPClient : public NetKernel{
	friend void* __func_ioctrl(void* pTCPClient);
	//friend void* __func_handlemsg(void* pTCPClient);
	friend void* __func_connect(void* pTCPClient);
public:
	static void initSystem();

	static void releaseSystem();
public:
	TCPClient(MsgProc* pProc, MsgDecoder* pDecoder);
	virtual ~TCPClient();
public:
	//! 端连接接口
	//! 客户端如果连接到服务器，也会调用该接口
	virtual void onPeerConnect(void* pPeer);
	//! 端断开连接接口
	//! 客户端如果和服务器断开连接，也会调用该接口
	virtual void onPeerDisconnect(void* pPeer);
	//! New Peer
	virtual void* newPeer() { return NULL; }
	//! Delete Peer
	virtual void deletePeer(void* pPeer) {}
	//! 断开连接
	virtual void cutPeer(void* pPeer);

	//! Set Peer's Data
	virtual void setPeerData(void* pPeer, void* pData) { m_pData = pData; }
	//! Get Peer's Data
	virtual void* getPeerData(void* pPeer) { return m_pData; }

	//! SendMsg
	virtual int sendMsg(void* pPeer, void* pMsg, int msgSize);
	//! SendMsgEx
	virtual int sendMsgEx(void* pPeer, void* pMsg, int msgSize, bool bEncode);
public:
	//! 初始化
	int init(const IPAddr& addr);
	//! 单独开线程初始化
	int initEx(const IPAddr& addr);
	//! 释放
	void release();

	//! 启动服务
	void start();
	//! 停止服务
	void stop(bool bWait);

	//! 是否已经连接
	bool isConnected() const { return m_bConnected; }

	const IPAddr& getAddr() const {
		return m_addr;
	}

	int getBuffSize() const { return m_sizeBuff; }

	void getBuffSize(int size) { m_sizeBuff = size; }
protected:
	//! Connect
	int connect(const IPAddr& addr);
	//! 接收到一个消息
	virtual void onRecvMsg(void* pMsg, int msgLen);
protected:
	//! Func IOCtrl
	int func_IOCtrl();
	//! Func Connect
	int func_Connect();
	////! Func HandleMsg
	//int Func_HandleMsg();
protected:
	int								m_socket;		//! socket

	Thread							m_threadConnect;
	Thread							m_threadIOCtrl;
	//ZephyrProc::Thread				m_threadHandleMsg;

	MsgDecoderKey_Simple			m_msgKey;

	IPAddr							m_addr;

	//NetStream_RecvBlock				m_recvBuff;

	bool							m_bConnected;

	void*							m_pData;

	uint8							m_ui8RecvMsgID;
	uint8							m_ui8SendMsgID;

	int								m_sizeBuff;

	bool							m_bDisconnectExitIOThread;

	LockImp							m_lockSendMsg;
};

NS_CC_ZEPHYR_END

#endif // __ZEPHYR_NET_TCPCLIENT_H__
