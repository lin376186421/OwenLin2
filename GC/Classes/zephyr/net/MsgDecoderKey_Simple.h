/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYR_NET_MSGDECODERKEY_SIMPLE_H__
#define __ZEPHYR_NET_MSGDECODERKEY_SIMPLE_H__

#include "../ZephyrBaseDef.h"
#include "MsgDecoderKey.h"

NS_CC_ZEPHYR_BEGIN

	class MsgDecoderKey_Simple : public MsgDecoderKey{
	public:
		MsgDecoderKey_Simple() : m_ui8Key(0x7b) {}
		virtual ~MsgDecoderKey_Simple() {}
	public:
		virtual void* getKeyPtr() { return &m_ui8Key; }

		virtual const void* getKeyPtr() const { return &m_ui8Key; }

		virtual int getKeySize() const { return 1; }
	public:
		void setKey(uint8 ui8Key) { m_ui8Key = ui8Key; }
	protected:
		uint8		m_ui8Key;
	};

NS_CC_ZEPHYR_END

#endif // __ZEPHYR_NET_MSGDECODERKEY_SIMPLE_H__
