/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "../Zephyr.h"

NS_CC_ZEPHYR_BEGIN

//! SendMsg
int MsgProc::sendMsg(void* pPeer, void* pMsg, int msgSize)
{
	CCAssert(m_pNetKernel != NULL, "MsgProc::SendMsg() m_pNetKernel fail!");

	return m_pNetKernel->sendMsg(pPeer, pMsg, msgSize);
}

//! SendMsgEx
int MsgProc::sendMsgEx(void* pPeer, void* pMsg, int msgSize, bool bEncode)
{
	CCAssert(m_pNetKernel != NULL, "MsgProc::SendMsgEx() m_pNetKernel fail!");

	return m_pNetKernel->sendMsgEx(pPeer, pMsg, msgSize, bEncode);
}

//! Cur Peer
void MsgProc::cutPeer(void* pPeer)
{
	CCAssert(m_pNetKernel != NULL, "MsgProc::SendMsgEx() m_pNetKernel fail!");

	m_pNetKernel->cutPeer(pPeer);
}

void MsgProc::deletePeer(void* pPeer) 
{ 
	m_pNetKernel->deletePeer(pPeer);
}

//! Set Peer's Data
void MsgProc::setPeerData(void* pPeer, void* pData) 
{
	if(m_pNetKernel != NULL)
		m_pNetKernel->setPeerData(pPeer, pData);
}
//! Get Peer's Data
void* MsgProc::getPeerData(void* pPeer) 
{
	if(m_pNetKernel != NULL)
		return m_pNetKernel->getPeerData(pPeer);

	return NULL;
}

NS_CC_ZEPHYR_END
