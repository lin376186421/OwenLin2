/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "../Zephyr.h"

NS_CC_ZEPHYR_BEGIN

//! 解码
void MsgDecoder_Simple::decode(void* pMsg, int msgSize, const MsgDecoderKey& key)
{
	CCAssert(pMsg != NULL, "MsgDecoder_Simple::Decode");
	CCAssert(msgSize >= sizeof(MsgHead), "MsgDecoder_Simple::Decode");
	CCAssert(key.getKeyPtr() != NULL, "MsgDecoder_Simple::Decode");
	CCAssert(key.getKeySize() == sizeof(uint8), "MsgDecoder_Simple::Decode");

	if(msgSize > MsgHead::ENCODE_LEN)
	{
		uint8* pTmp = static_cast<uint8*>(pMsg) + MsgHead::ENCODE_LEN;
		msgSize -= MsgHead::ENCODE_LEN;

		while(msgSize-- > 0)
			*pTmp++ ^= *(static_cast<const uint8*>(key.getKeyPtr()));
	}
}

//! 编码
void MsgDecoder_Simple::encode(void* pMsg, int msgSize, const MsgDecoderKey& key)
{
	CCAssert(pMsg != NULL, "MsgDecoder_Simple::Decode");
	CCAssert(msgSize >= sizeof(MsgHead), "MsgDecoder_Simple::Decode");
	CCAssert(key.getKeyPtr() != NULL, "MsgDecoder_Simple::Decode");
	CCAssert(key.getKeySize() == sizeof(uint8), "MsgDecoder_Simple::Decode");

	if(msgSize > MsgHead::ENCODE_LEN)
	{
		uint8* pTmp = static_cast<uint8*>(pMsg) + MsgHead::ENCODE_LEN;
		msgSize -= MsgHead::ENCODE_LEN;

		while(msgSize-- > 0)
			*pTmp++ ^= *(static_cast<const uint8*>(key.getKeyPtr()));
	}
}

NS_CC_ZEPHYR_END
