#ifndef __COC_SELECTBUILDINGLISTENER_H__
#define __COC_SELECTBUILDINGLISTENER_H__

#include "BaseDef.h"
#include "Building.h"

NS_COC_BEGIN

class SelectBuildingListener
{
public:
	SelectBuildingListener() : m_isRelease(false) {}
	virtual ~SelectBuildingListener() {}
public:
	//! 如果返回false，表示拦截消息
	virtual bool onSelect(bool isSelect, Building* pBuilding) = 0;
public:
	bool		m_isRelease;
};

NS_COC_END

#endif  // __COC_SELECTBUILDINGLISTENER_H__