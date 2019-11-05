/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYR_NET_MSG_H__
#define __ZEPHYR_NET_MSG_H__

#include "../ZephyrBaseDef.h"

NS_CC_ZEPHYR_BEGIN

#pragma pack (push)
#pragma pack (1)

typedef unsigned short	NetMsgSizeType;

//! 最大的消息包长度
const int	MAX_PKG_SIZE	=	1440;


////////////////////////////////////////////////////////////////////////////
//! 统一消息头

class MsgHead{
public:
	static const int ENCODE_LEN		=	5;
public:
	NetMsgSizeType getSize() const { return m_size; }

	uint8 getMainID() const { return m_ui8MainID; }

	uint8 getAssID() const { return m_ui8AssistantID; }

	void setHead(NetMsgSizeType size, uint8 ui8MainID, uint8 ui8AssistantID) {
		m_size = size;
		m_ui8MainID = ui8MainID;
		m_ui8AssistantID = ui8AssistantID;
	}
public:
	uint8			m_ui8MsgID;			//! 当前消息标识
	NetMsgSizeType	m_size;				//! 消息大小
	uint8			m_ui8MainID;		//! 主消息ID
	uint8			m_ui8AssistantID;	//! 辅助消息ID
};

////////////////////////////////////////////////////////////////////////////
//! 普通消息 - 带数据区

template<typename T>
class NetMsg{
public:
	//! 设置消息头
	void setHead(MsgHead& head) { m_head = head; }

	//! 设置消息头
	void setHead(NetMsgSizeType size, uint8 mainID, uint8 assID) {
		m_head.m_size = size;
		m_head.m_ui8MainID = mainID;
		m_head.m_ui8AssistantID = assID;
	}

	//! 获得消息头
	const MsgHead& getSafeHead() const { return m_head; }
public:
	//! 设置消息数据
	void setData(T& data) { m_data = data; }
	//! 获取消息数据
	T* getData() { return &m_data; }
	//! 获取消息数据
	const T& getSafeData() const { return m_data; }

	//! 获得消息长度
	NetMsgSizeType getSize() const { return m_head.m_size; }
public:
	//! 消息长度是否合法
	//! 主要用于验证变长消息
	static bool isValid(int size) {
		return size >= sizeof(MsgHead) && T::isValid(size - sizeof(MsgHead));
	}
public:
	// 保证消息正确性
	void fix() {
		m_data.fix(sizeof(NetMsg<T>), getSize());
	}
protected:
	MsgHead		m_head;
	T			m_data;
};

////////////////////////////////////////////////////////////////////////////
//! 普通消息 - 不带数据区 (特例化)

template<>
class NetMsg<null_type>{
public:
	//! 设置消息头
	void setHead(MsgHead& head) { m_head = head; }

	//! 设置消息头
	void setHead(NetMsgSizeType size, uint8 mainID, uint8 assID) {
		m_head.m_size = size;
		m_head.m_ui8MainID = mainID;
		m_head.m_ui8AssistantID = assID;
	}

	//! 获得消息头
	const MsgHead& getSafeHead() const { return m_head; }

	//! 获得消息长度
	NetMsgSizeType getSize() const { return m_head.m_size; }
private:
	MsgHead		m_head;
};

#pragma pack (pop)

NS_CC_ZEPHYR_END

#endif // __ZEPHYR_NET_MSG_H__
