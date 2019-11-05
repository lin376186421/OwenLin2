#include "BuildingDestroyEmitter.h"
#include "ImgMgr.h"

using namespace cocos2d;

NS_COC_BEGIN

/////////////////////////////////////////////////////////////////////////////////////////////////////

Particle* BuildingDestroyEmitter::onEmitter()
{
	char strname[128];

	Particle* pSpr = Particle::create();
	//CCAnimation* pAni = CCAnimation::create();

	int curIndex = rand() % 23 + 1;
	sprintf(strname, "buildingbomb_%03d", curIndex);
	CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(strname);
	pSpr->initWithSpriteFrame(pFrame);

	//for(int i = 0; i < 23; ++i)
	//{
	//	sprintf(strname, "buildingbomb_%03d", (i + 1));
	//	CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(strname);
	//	pAni->addSpriteFrame(pFrame);

	//	if(i == 0)
	//		pSpr->initWithSpriteFrame(pFrame);
	//}

	//pNode->addChild(pSpr);
	pSpr->setRotation(90 - rand() % 180);

	//pAni->setDelayPerUnit(0.04f);

	//CCAnimate* pAnimate = CCAnimate::create(pAni);

	//CCRepeatForever* pRF = CCRepeatForever::create(pAnimate);
	//pSpr->runAction(pRF);

	pSpr->runAction(CCRotateBy::create(2, 180 + rand() % 180));
	pSpr->runAction(CCFadeOut::create(2));

	float r = 2 + rand() % 130 / 100.0f + 0.2f;
	pSpr->runAction(CCScaleTo::create(2, r, r));

	pSpr->m_ptSpeed.x = 400 - rand() % 800;
	pSpr->m_ptSpeed.y = rand() % 300 + 600;

	pSpr->m_ptAccSpeed.y = -1200;

	pSpr->m_lastTime = 1500;
	//pNode->m_pSpr = pSpr;

	return pSpr;
}

NS_COC_END
