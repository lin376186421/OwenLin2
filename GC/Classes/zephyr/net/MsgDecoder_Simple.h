/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYR_NET_MSGDECODER_SIMPLE_H__
#define __ZEPHYR_NET_MSGDECODER_SIMPLE_H__

#include "../ZephyrBaseDef.h"
#include "MsgDecoder.h"

NS_CC_ZEPHYR_BEGIN

class MsgDecoder_Simple : public MsgDecoder{
public:
	MsgDecoder_Simple() {}
	virtual ~MsgDecoder_Simple() {}
public:
	//! 解码
	virtual void decode(void* pMsg, int msgSize, const MsgDecoderKey& key);
	//! 编码
	virtual void encode(void* pMsg, int msgSize, const MsgDecoderKey& key);
};

NS_CC_ZEPHYR_END

#endif // __ZEPHYR_NET_MSGDECODER_SIMPLE_H__
