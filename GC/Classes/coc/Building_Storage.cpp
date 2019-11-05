#include "Building_Storage.h"
#include "BuildingMgr.h"
#include "ImgMgr.h"
#include "PlayerScene.h"
#include "Action_GrayFlash.h"

NS_COC_BEGIN

Building_Storage::Building_Storage(BuildingInfo info)
	: Building(info)
	, m_pBulding(NULL)
	, m_pShadow(NULL)
	, m_pLevel(NULL)
	, m_pSelAction(NULL)
	, m_pSelAction_Level(NULL)
	, m_iLevel(0)
{
	m_type = BUILDING_TYPE_STORAGE;

	for(int i = 0; i < 4; ++i)
		m_pLevelFrame[i] = NULL;
}

Building_Storage::~Building_Storage()
{
}

void Building_Storage::init(int buildingid, int lx, int ly)
{
	char strname[128];
	sprintf(strname, "%s_1", m_info.res.c_str());
	m_pShadow = ImgMgr::getSingleton().loadSprite(strname);
	if(m_pShadow != NULL)
	{
		m_pShadow->setAnchorPoint(ccp(0.5f, 0));
		m_pShadow->setScale(2.0f);
		m_pShadow->retain();
		PlayerScene::getSingleton().addShadow(m_pShadow);
		//m_scene.m_pRoot->addChild(m_pShadow, SCENE_Z_SHADOW);
	}

	sprintf(strname, "%s_2", m_info.res.c_str());
	m_pBulding = ImgMgr::getSingleton().loadSprite(strname);
	m_pBulding->setAnchorPoint(ccp(0.5f, 0));
	m_pBulding->retain();
	PlayerScene::getSingleton().m_pRoot->addChild(m_pBulding, SCENE_Z_BUILDING_BEGIN);

	for(int i = 0; i < 4; ++i)
	{
		sprintf(strname, "%s_2_%d", m_info.res.c_str(), i);
		m_pLevelFrame[i] = ImgMgr::getSingleton().getFrame(strname);
		m_pLevelFrame[i]->retain();
	}

	m_pLevel = CCSprite::createWithSpriteFrame(m_pLevelFrame[0]);
	m_pLevel->setAnchorPoint(ccp(0.5f, 0));
	m_pLevel->retain();
	PlayerScene::getSingleton().m_pRoot->addChild(m_pLevel, SCENE_Z_BUILDING_BEGIN + 1);

	Building::init(buildingid, lx, ly);

	onChgStoreRes();
}

void Building_Storage::setLogicPos(int lx, int ly)
{
	Building::setLogicPos(lx, ly);

	if(m_isPreBuilding)
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(false);

		if(m_pBulding != NULL)
			m_pBulding->setVisible(false);

		if(m_pLevel != NULL)
			m_pLevel->setVisible(false);
	}
	else if(isEmptyHP())
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(false);

		if(m_pBulding != NULL)
			m_pBulding->setVisible(false);

		if(m_pLevel != NULL)
			m_pLevel->setVisible(false);
	}
	else
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(true);

		if(m_pBulding != NULL)
			m_pBulding->setVisible(true);

		if(m_pLevel != NULL)
			m_pLevel->setVisible(true);
	}

	//else
	{
		CCPoint pt = countSceneTilePt(lx, ly);
		CCPoint cpt = pt;

		if(m_pShadow != NULL)
			m_pShadow->setPosition(cpt);

		if(m_pBulding != NULL)
			m_pBulding->setPosition(cpt);

		if(m_pLevel != NULL)
		{
			cpt.y += m_info.offHeight;
			m_pLevel->setPosition(cpt);
		}
	}
}

void Building_Storage::releaseFromScene()
{
	if(m_pShadow != NULL)
	{
		PlayerScene::getSingleton().removeShadow(m_pShadow, false);
		//m_scene.m_pRoot->removeChild(m_pShadow, false);
		m_pShadow->release();
		m_pShadow = NULL;
	}

	if(m_pBulding != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pBulding, false);
		m_pBulding->release();
		m_pBulding = NULL;
	}

	if(m_pLevel != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pLevel, false);
		m_pLevel->release();
		m_pLevel = NULL;
	}

	for(int i = 0; i < 4; ++i)
	{
		if(m_pLevelFrame[i] != NULL)
		{
			m_pLevelFrame[i]->release();
			m_pLevelFrame[i] = NULL;
		}
	}

	Building::releaseFromScene();
}

void Building_Storage::reorder(int zorder)
{
	Building::reorder(zorder);

	if(m_pBulding != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->reorderChild(m_pBulding, zorder);
	}

	if(m_pLevel != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->reorderChild(m_pLevel, zorder + 1);
	}
}

void Building_Storage::chgSelMode(bool isSel)
{
	Building::chgSelMode(isSel);

	if(isSel)
	{
		Action_GrayFlash* pLightFlash = Action_GrayFlash::create(3, 128, 256);
		CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
		m_pSelAction = m_pBulding->runAction(pRF);

		setUpDownAction(m_pBulding, true);

		pLightFlash = Action_GrayFlash::create(3, 128, 256);
		pRF = CCRepeatForever::create(pLightFlash);
		m_pSelAction_Level = m_pLevel->runAction(pRF);

		setUpDownAction(m_pLevel, true);
	}
	else
	{
		m_pBulding->stopAction(m_pSelAction);
        m_pSelAction = NULL;
        
		ccColor3B c;
		c.r = c.g = c.b = 255;

		m_pBulding->setColor(c);

		m_pLevel->stopAction(m_pSelAction_Level);
        m_pSelAction_Level = NULL;

		m_pLevel->setColor(c);
	}
}

void Building_Storage::onBuildingEnd()
{
	if(m_isPreBuilding)
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(true);

		if(m_pBulding != NULL)
			m_pBulding->setVisible(true);

		if(m_pLevel != NULL)
			m_pLevel->setVisible(true);
	}

	Building::onBuildingEnd();
}

void Building_Storage::setBeAttackMode()
{
	if(m_pActBeAttack != NULL)
		return ;

	if(m_pShadow != NULL)
		setBeAttackAni(m_pShadow, true);

	if(m_pBulding != NULL)
		setBeAttackAni(m_pBulding, false);

	if(m_pLevel != NULL)
		setBeAttackAni(m_pLevel, false);
}

void Building_Storage::onChgStoreRes()
{
	if(m_pBuildingInfo != NULL)
	{
		if(m_pBuildingInfo->maxstore[0] == 0 && m_pBuildingInfo->maxstore[1] == 0)
			return;

		int clevel = 0;
		float cs = m_pBuildingInfo->store[0] > 0 ? m_pBuildingInfo->store[0] : m_pBuildingInfo->store[1];
		float ms = m_pBuildingInfo->maxstore[0] > 0 ? m_pBuildingInfo->maxstore[0] : m_pBuildingInfo->maxstore[1];
		float cl = cs / ms;

		if(cl < 0.25f)
			clevel = 0;
		else if(cl < 0.5f)
			clevel = 1;
		else if(cl < 0.75f)
			clevel = 2;
		else
			clevel = 3;

		if(clevel != m_iLevel)
		{
			m_iLevel = clevel;

			m_pLevel->setDisplayFrame(m_pLevelFrame[clevel]);
		}
	}
}

void Building_Storage::onRefurbish()
{
	onChgStoreRes();
}

NS_COC_END