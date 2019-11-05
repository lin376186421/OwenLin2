#ifndef __COC_PARTICLESYSTEM_H__
#define __COC_PARTICLESYSTEM_H__

#include "BaseDef.h"
#include "EmitterWorker.h"
#include <map>

using namespace cocos2d;

NS_COC_BEGIN

/////////////////////////////////////////////////////////////////////////////////////////////////////

class Particle : public CCSprite
{
public:
	Particle() : m_ptSpeed(0, 0), m_ptAccSpeed(0, 0), m_lastTime(0) {}
	virtual ~Particle() {}
public:
	CREATE_FUNC(Particle);
public:
	bool init();
public:
	CCPoint		m_ptSpeed;
	CCPoint		m_ptAccSpeed;

	int			m_lastTime;

	//CCSprite*	m_pSpr;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

class ParticleSystem : public CCNode
{
public:
	ParticleSystem(EmitterWorker& emitter);
	virtual ~ParticleSystem();
public:
	static ParticleSystem* create(EmitterWorker& emitter);
public:
	bool init();

	//virtual void draw();
	void onIdle(float fDelta);

	void clear();

	bool isEmpty();
public:
	void emitter(int nums);

	virtual Particle* onEmitter();
public:
	std::list<Particle*>		m_lstParticle;

	//int							m_lastTime;

	EmitterWorker&				m_emitter;

	int							m_lastNums;
	bool						m_nowEmitter;
};

NS_COC_END

#endif  // __COC_PARTICLESYSTEM_H__