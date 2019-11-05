/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYR_NET_MSGDECODER_H__
#define __ZEPHYR_NET_MSGDECODER_H__

#include "../ZephyrBaseDef.h"

NS_CC_ZEPHYR_BEGIN

class MsgDecoder{
public:
	MsgDecoder() {}
	virtual ~MsgDecoder() {}
public:
	//! 解码
	virtual void decode(void* pMsg, int msgSize, const MsgDecoderKey& key) = 0;
	//! 编码
	virtual void encode(void* pMsg, int msgSize, const MsgDecoderKey& key) = 0;
};

NS_CC_ZEPHYR_END

#endif // __ZEPHYR_NET_MSGDECODER_H__
