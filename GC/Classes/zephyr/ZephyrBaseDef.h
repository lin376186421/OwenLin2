/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#ifndef __ZEPHYR_BASEDEF_H__
#define __ZEPHYR_BASEDEF_H__

#include <cocos2d.h>
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

#if (CC_PLATFORM_WIN32 != CC_TARGET_PLATFORM && CC_PLATFORM_WP8 != CC_TARGET_PLATFORM)
#include <netdb.h>
#include <netinet/in.h>
#endif

#define NS_CC_ZEPHYR_BEGIN	namespace cocos2d{ namespace zephyr{
#define NS_CC_ZEPHYR_END	}}
#define USING_NS_CC_ZEPHYR  using namespace cocos2d::zephyr

#define IMGMGR_NAME_NOPNG

#define ZEPHYR_BASETYPE

NS_CC_ZEPHYR_BEGIN

class MsgProc;
class NetKernel;
class MsgDecoderKey;
class MsgDecoder;
class TCPClient;

class LockImp;
class Thread;

typedef unsigned char	uint8;
typedef char			int8;
typedef short			int16;
typedef unsigned short	uint16;
typedef int				int32;
typedef unsigned int	uint32;
typedef long long		int64;
typedef struct{}		null_type;

typedef void* (*ThreadFunc)(void*);

#pragma pack (push)
#pragma pack (1)

union IPv4{
	struct _IP{
		uint8	_1, _2, _3, _4;
	} child;

	uint32	ip;
};

//! IPv4 Addr
struct IPv4Addr{
	IPv4		ip;
	uint16		port;
};

//! 默认的 IP Addr
typedef IPv4Addr	IPAddr;

// ani obj
const int	ANIOBJ_TYPE_NORMAL			=	0;
const int	ANIOBJ_TYPE_FLYINGFONT		=	1;
const int	ANIOBJ_TYPE_WAITFADEOUT		=	2;
const int	ANIOBJ_TYPE_DOWN			=	3;
const int	ANIOBJ_TYPE_MOVE			=	4;
const int	ANIOBJ_TYPE_FLYINGFONT2		=	5;
const int	ANIOBJ_TYPE_FLASH			=	6;

#pragma pack (pop)

NS_CC_ZEPHYR_END

#endif // __ZEPHYR_BASEDEF_H__
