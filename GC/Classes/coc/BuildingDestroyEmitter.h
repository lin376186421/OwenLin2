#ifndef __COC_BUILDINGDESTROYEMITTER_H__
#define __COC_BUILDINGDESTROYEMITTER_H__

#include "BaseDef.h"
#include <map>
#include "EmitterWorker.h"
#include "ParticleSystem.h"

using namespace cocos2d;

NS_COC_BEGIN

/////////////////////////////////////////////////////////////////////////////////////////////////////

class BuildingDestroyEmitter : public EmitterWorker
{
public:
	BuildingDestroyEmitter() {}
	virtual ~BuildingDestroyEmitter() {}
public:
	virtual Particle* onEmitter();
};

NS_COC_END

#endif  // __COC_BUILDINGDESTROYEMITTER_H__