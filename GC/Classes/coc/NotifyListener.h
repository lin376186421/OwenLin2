#ifndef __COC_NOTIFYLISTENER_H__
#define __COC_NOTIFYLISTENER_H__

#include "BaseDef.h"

NS_COC_BEGIN

class NotifyListener
{
public:
	NotifyListener() : m_isRelease(false) {}
	virtual ~NotifyListener() {}
public:
	//! 如果返回false，表示拦截消息
	virtual bool onNotify(int notifyid, const void* pVal) = 0;
public:
	//void release() { m_isRelease = true; }
public:
	bool		m_isRelease;
};

NS_COC_END

#endif  // __COC_NOTIFYLISTENER_H__