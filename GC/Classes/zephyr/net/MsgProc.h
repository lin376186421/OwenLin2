/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYR_NET_MSGPROC_H__
#define __ZEPHYR_NET_MSGPROC_H__

#include "../ZephyrBaseDef.h"

NS_CC_ZEPHYR_BEGIN

class MsgProc{
public:
	MsgProc() : m_pNetKernel(NULL) {}
	virtual ~MsgProc() {}
public:
	//! 消息处理接口
	virtual int onHandleMessage(void* pPeer, void* pMsg, int msgSize) = 0;
public:
	//! Set NetKernel
	void setNetKernel(NetKernel* pNetKernel) {
		m_pNetKernel = pNetKernel;
	}

	//! SendMsg
	int sendMsg(void* pPeer, void* pMsg, int msgSize);
	//! SendMsgEx
	int sendMsgEx(void* pPeer, void* pMsg, int msgSize, bool bEncode);
	//! Cur Peer
	void cutPeer(void* pPeer);

	void deletePeer(void* pPeer);

	//! Set Peer's Data
	void setPeerData(void* pPeer, void* pData);
	//! Get Peer's Data
	void* getPeerData(void* pPeer);
protected:
	NetKernel*		m_pNetKernel;
};

NS_CC_ZEPHYR_END

#endif // __ZEPHYR_NET_MSGPROC_H__
