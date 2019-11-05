/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "../Zephyr.h"

NS_CC_ZEPHYR_BEGIN

void buildPCMsg_PeerConnect(PCMsg_PeerConnect& msg, const IPAddr& addr, bool bOK /* = true */)
{
	msg.getData()->addr = addr;
	msg.getData()->bOK = bOK ? 1 : 0;

	msg.setHead(sizeof(msg), PHYSICALCONNECT_MSG, PCMSG_PEERCONNECT);
}

void buildPCMsg_PeerDisconnect(PCMsg_PeerDisconnect& msg, const IPAddr& addr)
{
	msg.getData()->addr = addr;

	msg.setHead(sizeof(msg), PHYSICALCONNECT_MSG, PCMSG_PEERDISCONNECT);
}

NS_CC_ZEPHYR_END