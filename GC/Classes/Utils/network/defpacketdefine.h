#ifndef _DEFPACKETDEFINE_H_
#define _DEFPACKETDEFINE_H_

struct NetPacket
{
	short opcode;
};

#define NET_PACKET(packet) \
struct packet; \
	CInputStream& operator>>(CInputStream& is, packet& pt); \
	COutputStream& operator<<(COutputStream& os, const packet& pt); \
struct packet : public NetPacket	

#define PACKET_GET(packet) \
	CInputStream& operator>>(CInputStream& is, packet& pt)

#define PACKET_PUT(packet) \
	COutputStream& operator<<(COutputStream& os, const packet& pt)

#endif