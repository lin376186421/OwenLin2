#ifndef __COC_GOLDEMITTER_H__
#define __COC_GOLDEMITTER_H__

#include "BaseDef.h"
#include <map>
#include "EmitterWorker.h"
#include "ParticleSystem.h"

using namespace cocos2d;

NS_COC_BEGIN

/////////////////////////////////////////////////////////////////////////////////////////////////////

class GoldEmitter : public EmitterWorker
{
public:
	GoldEmitter() {}
	virtual ~GoldEmitter() {}
public:
	virtual Particle* onEmitter();
};

NS_COC_END

#endif  // __COC_GOLDEMITTER_H__