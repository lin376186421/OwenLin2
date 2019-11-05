#ifndef __COC_EMITTERWORKER_H__
#define __COC_EMITTERWORKER_H__

#include "BaseDef.h"
#include <map>

using namespace cocos2d;

NS_COC_BEGIN

/////////////////////////////////////////////////////////////////////////////////////////////////////

class EmitterWorker
{
public:
	EmitterWorker() {}
	virtual ~EmitterWorker() {}
public:
	virtual Particle* onEmitter() = 0;
};

NS_COC_END

#endif  // __COC_EMITTERWORKER_H__