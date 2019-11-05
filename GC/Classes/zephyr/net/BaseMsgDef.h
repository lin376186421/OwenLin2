/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYR_NET_BASEMSGDEF_H__
#define __ZEPHYR_NET_BASEMSGDEF_H__

#include "../ZephyrBaseDef.h"
#include "MsgHead.h"

NS_CC_ZEPHYR_BEGIN

	//! 在 ZephyrNet 3.0 中，我们把连接分为2个级别
	//! 分别是 物理连接 和 逻辑连接
	//! 网络内核来协调 2个级别 的连接关系
	//! 上层逻辑仅关心逻辑连接即可

	///////////////////////////////////////////////////////////////////////////
	//! 物理连接消息

	//--------------------------------------------------------------
	//! 物理连接消息 MainID
	const uint8		PHYSICALCONNECT_MSG			=	0xff;


	//--------------------------------------------------------------
	//! 物理连接消息
	const uint8		PCMSG_PEERCONNECT			=	0x01;
	//! 物理连接断开消息
	const uint8		PCMSG_PEERDISCONNECT		=	0x02;
	//! 物理连接消息
	const uint8		PCMSG_PEERSAFECONNECT		=	0x03;
	//! 物理连接断开消息
	const uint8		PCMSG_PEERSAFEDISCONNECT	=	0x04;
	////! 物理连接消息
	//const uint8		PCMSG_PEERSAFECONNECTEX		=	0x05;
	////! 物理连接断开消息
	//const uint8		PCMSG_PEERSAFEDISCONNECTEX	=	0x06;

	//--------------------------------------------------------------
#pragma pack (push)
#pragma pack (1)

	//! 物理连接消息
	//! 本消息并非由客户端发送，而是NetKernel模拟出来的
	struct __PCMsg_PeerConnect{
		IPAddr		addr;
		uint8		bOK;		//! 是否连接成功
	};

	typedef NetMsg<__PCMsg_PeerConnect>		PCMsg_PeerConnect;

	//! 物理连接断开消息
	//! 本消息并非由客户端发送，而是NetKernel模拟出来的
	struct __PCMsg_PeerDisconnect{
		IPAddr		addr;
	};

	typedef NetMsg<__PCMsg_PeerDisconnect>		PCMsg_PeerDisconnect;

	//! 安全连接消息
	//! 本消息由客户端发送
	struct __PCMsg_PeerSafeConnect{
		int32		key;
		int32		connectnums;
	};

	typedef NetMsg<__PCMsg_PeerSafeConnect>		PCMsg_PeerSafeConnect;

	//! 安全断开消息
	//! 本消息由客户端发送
	struct __PCMsg_PeerSafeDisconnect{
		int32		key;
	};

	typedef NetMsg<__PCMsg_PeerSafeDisconnect>	PCMsg_PeerSafeDisconnect;

	////! 安全连接消息
	////! 本消息由客户端发送
	//struct __PCMsg_PeerSafeConnectEx{
	//	Int32		key;
	//	UInt8		bOK;		//! 是否连接成功
	//};

	//typedef NetMsg<__PCMsg_PeerSafeConnectEx>		PCMsg_PeerSafeConnectEx;

	////! 安全断开消息
	////! 本消息由客户端发送
	//struct __PCMsg_PeerSafeDisconnectEx{
	//	Int32		key;
	//};

	//typedef NetMsg<__PCMsg_PeerSafeDisconnectEx>	PCMsg_PeerSafeDisconnectEx;

#pragma pack (pop)

	//--------------------------------------------------------------

	void buildPCMsg_PeerConnect(PCMsg_PeerConnect& msg, const IPAddr& addr, bool bOK = true);
	void buildPCMsg_PeerDisconnect(PCMsg_PeerDisconnect& msg, const IPAddr& addr);

	//void BuildPCMsg_PeerSafeConnectEx(PCMsg_PeerConnect& msg, int key, bool bOK = true);
	//void BuildPCMsg_PeerSafeDisconnectEx(PCMsg_PeerDisconnect& msg, int key);

NS_CC_ZEPHYR_END

#endif // __ZEPHYR_NET_BASEMSGDEF_H__
