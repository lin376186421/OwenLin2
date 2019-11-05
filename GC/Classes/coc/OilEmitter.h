#ifndef __COC_OILEMITTER_H__
#define __COC_OILEMITTER_H__

#include "BaseDef.h"
#include <map>
#include "EmitterWorker.h"
#include "ParticleSystem.h"

using namespace cocos2d;

NS_COC_BEGIN

/////////////////////////////////////////////////////////////////////////////////////////////////////

class OilEmitter : public EmitterWorker
{
public:
	OilEmitter() {}
	virtual ~OilEmitter() {}
public:
	virtual Particle* onEmitter();
};

NS_COC_END

#endif  // __COC_OILEMITTER_H__