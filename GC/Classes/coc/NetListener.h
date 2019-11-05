#ifndef __COC_NETLISTENER_H__
#define __COC_NETLISTENER_H__

#include "BaseDef.h"
#include "../Utils/jsoncpp/json.h"

NS_COC_BEGIN

class NetListener
{
public:
	NetListener() : m_isRelease(false), m_pDelegate(NULL), m_funcRelease(NULL) {}
	virtual ~NetListener() {}
public:
	//! 如果返回false，表示拦截消息
	virtual bool onProc(const Json::Value& val) = 0;
public:
	//void release() { m_isRelease = true; }
public:
	bool			m_isRelease;

	CCObject*		m_pDelegate;
	SEL_CallFunc	m_funcRelease;
};

NS_COC_END

#endif  // __COC_NETLISTENER_H__