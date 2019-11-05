/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "../Zephyr.h"

NS_CC_ZEPHYR_BEGIN

//! 消息处理接口
int NetKernel::onHandleMessage(void* pPeer, void* pMsg, int msgSize)
{
	CCAssert(m_pProc != NULL, "NetKernel::HandleMessage");
	CCAssert(msgSize >= sizeof(MsgHead), "NetKernel::HandleMessage");

	int hRet = m_pProc->onHandleMessage(pPeer, pMsg, msgSize);

	return hRet;
}

NS_CC_ZEPHYR_END
