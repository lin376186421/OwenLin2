#ifndef __COC_NETMODBASE_H__
#define __COC_NETMODBASE_H__

#include "BaseDef.h"

NS_COC_BEGIN

class NetModBase{
protected:
	NetModBase() {}
	virtual ~NetModBase() {}
public:
	virtual bool onMessage(const Json::Value& val) = 0;
};

NS_COC_END

#endif // __COC_NETMODBASE_H__