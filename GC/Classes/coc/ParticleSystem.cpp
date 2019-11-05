#include "ParticleSystem.h"
#include "ImgMgr.h"
#include "PlayerScene.h"

NS_COC_BEGIN

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool Particle::init()
{
	bool bRet = false;
    do 
    {
		bRet = true;
	}while(0);

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

ParticleSystem::ParticleSystem(EmitterWorker& emitter)
	: m_emitter(emitter)//m_lastTime(0)
	, m_nowEmitter(false)
	//, m_emitter(emitter)
{
}

ParticleSystem::~ParticleSystem()
{
}

ParticleSystem* ParticleSystem::create(EmitterWorker& emitter)
{
	ParticleSystem *pRet = new ParticleSystem(emitter);
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool ParticleSystem::init()
{
	bool bRet = false;
    do 
    {
		CCNode::init();

		schedule(schedule_selector(ParticleSystem::onIdle));  

		bRet = true;
	}while(0);

	return bRet;
}

Particle* ParticleSystem::onEmitter()
{
	//Particle* pNode = Particle::create();
	//m_lstParticle.push_back(pNode);

	Particle* pNode = m_emitter.onEmitter();
	m_lstParticle.push_back(pNode);
	pNode->setPosition(getPosition());

	PlayerScene::getSingleton().addParticle(pNode);

	//addChild(pNode);

	return pNode;
}

bool ParticleSystem::isEmpty()
{
	return m_lastNums <= 0 && m_lstParticle.empty();
}

void ParticleSystem::onIdle(float fDelta)
{
	if(m_lastNums > 0)
	{
		//for(int i = 0; i < nums; ++i)
		onEmitter();

		m_lastNums--;
	}

	float fot = fDelta;//(curTime - m_lastTime) / 1000.0f;

	for(std::list<Particle*>::iterator it = m_lstParticle.begin(); it != m_lstParticle.end(); )
	{
		Particle* pNode = *it;
		CCPoint pt = pNode->getPosition();

		pt.x += pNode->m_ptSpeed.x * fot;
		pt.y += pNode->m_ptSpeed.y * fot;

		pNode->setPosition(pt);

		pNode->m_ptSpeed.x += pNode->m_ptAccSpeed.x * fot;
		pNode->m_ptSpeed.y += pNode->m_ptAccSpeed.y * fot;

		pNode->m_lastTime -= fDelta * 1000;//curTime - m_lastTime;

		if(pNode->m_lastTime <= 0)
		{
			PlayerScene::getSingleton().removeParticle(pNode);
			//removeChild(pNode, true);
			it = m_lstParticle.erase(it);
		}
		else
			++it;
	}

	//m_lastTime = curTime;

	//if(m_lstParticle.empty())
	//	getParent()->removeChild(this, true);
}

void ParticleSystem::emitter(int nums)
{
	if(m_nowEmitter)
	{
		for(int i = 0; i < nums; ++i)
			onEmitter();

		m_lastNums = 0;
	}
	else
		m_lastNums = nums;
}

void ParticleSystem::clear()
{
	for(std::list<Particle*>::iterator it = m_lstParticle.begin(); it != m_lstParticle.end(); ++it)
	{
		Particle* pNode = *it;

		PlayerScene::getSingleton().removeParticle(pNode);
		//removeChild(pNode, true);
	}

	m_lstParticle.clear();
}

NS_COC_END