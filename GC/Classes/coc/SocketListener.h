//
//  SocketListener.h
//  GameClient
//
//  Created by qsun on 14-8-27.
//
//

#ifndef __GameClient__SocketListener__
#define __GameClient__SocketListener__

#include <iostream>
#include "BaseDef.h"

NS_COC_BEGIN

class SocketListener
{
public:
	SocketListener() : m_isRelease(false) {}
	virtual ~SocketListener() {}
public:
    
	virtual bool onStateChange(int state) = 0;
    
    bool m_isRelease;
};

NS_COC_END

#endif /* defined(__GameClient__SocketListener__) */
