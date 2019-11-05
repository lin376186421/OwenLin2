/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYR_NET_MSGDECODERKEY_H__
#define __ZEPHYR_NET_MSGDECODERKEY_H__

#include "../ZephyrBaseDef.h"

NS_CC_ZEPHYR_BEGIN

class MsgDecoderKey{
public:
	MsgDecoderKey() {}
	virtual ~MsgDecoderKey() {}
public:
	virtual void* getKeyPtr() = 0;

	virtual const void* getKeyPtr() const = 0;

	virtual int getKeySize() const = 0;
};

NS_CC_ZEPHYR_END

#endif // __ZEPHYR_NET_MSGDECODERKEY_H__
