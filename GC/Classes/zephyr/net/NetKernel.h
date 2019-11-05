/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYR_NET_NETKERNEL_H__
#define __ZEPHYR_NET_NETKERNEL_H__

#include "MsgDecoder.h"
#include "../ZephyrBaseDef.h"

NS_CC_ZEPHYR_BEGIN

//! 在ZephyrNet中，我们把连接分为2个级别
//! 分别是 物理连接 和 逻辑连接
//! 网络内核来协调 2个级别 的连接关系
//! 上层逻辑仅关心逻辑连接即可

class NetKernel{
public:
	NetKernel(MsgProc* pProc, MsgDecoder* pDecoder) : m_pProc(pProc), m_pDecoder(pDecoder) {}
	virtual ~NetKernel() {}
public:
	//! 消息处理接口
	virtual int onHandleMessage(void* pPeer, void* pMsg, int msgSize);

	//! 端连接接口
	//! 客户端如果连接到服务器，也会调用该接口
	virtual void onPeerConnect(void* pPeer) = 0;
	//! 端断开连接接口
	//! 客户端如果和服务器断开连接，也会调用该接口
	virtual void onPeerDisconnect(void* pPeer) = 0;
	//! New Peer
	virtual void* newPeer() = 0;
	//! Delete Peer
	virtual void deletePeer(void* pPeer) = 0;
	//! 断开连接
	virtual void cutPeer(void* pPeer) = 0;

	//! Set Peer's Data
	virtual void setPeerData(void* pPeer, void* pData) = 0;
	//! Get Peer's Data
	virtual void* getPeerData(void* pPeer) = 0;

	//! SendMsg
	virtual int sendMsg(void* pPeer, void* pMsg, int msgSize) = 0;
	//! SendMsgEx
	virtual int sendMsgEx(void* pPeer, void* pMsg, int msgSize, bool bEncode) = 0;
protected:
	//! 解码
	void decode(void* pMsg, int msgSize, const MsgDecoderKey& key) {
		CCAssert(m_pDecoder != NULL, "NetKernel::Decode");

		m_pDecoder->decode(pMsg, msgSize, key);
	}
	//! 编码
	void encode(void* pMsg, int msgSize, const MsgDecoderKey& key) {
		CCAssert(m_pDecoder != NULL, "NetKernel::Encode");

		m_pDecoder->encode(pMsg, msgSize, key);
	}
protected:
	MsgProc*		m_pProc;
	MsgDecoder*		m_pDecoder;
};

NS_CC_ZEPHYR_END

#endif // __ZEPHYR_NET_NETKERNEL_H__
