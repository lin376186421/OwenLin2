#include "Building_Resource.h"
#include "BuildingMgr.h"
#include "ImgMgr.h"
#include "PlayerScene.h"
#include "Action_GrayFlash.h"
#include "LogicInfo.h"

NS_COC_BEGIN

Building_Resource::Building_Resource(BuildingInfo info)
	: Building(info)
	, m_pShadow(NULL)
	, m_pPersonAni(NULL)
	, m_pCurPersonAction(NULL)
{
	m_lastPersonAniTime = rand() % 5000 + 5000;

	m_type = BUILDING_TYPE_COLLECT;

	m_pSelAction[0] = m_pSelAction[1] = m_pSelAction[2] = m_pSelAction[3] = NULL;

	m_pLevel[0] = m_pLevel[1] = NULL;
}

Building_Resource::~Building_Resource()
{
}

void Building_Resource::init(int buildingid, int lx, int ly)
{
	{
		char strname[128];
		sprintf(strname, "%s_1", m_info.res.c_str());
		m_pShadow = ImgMgr::getSingleton().loadSprite(strname);
		if(m_pShadow != NULL)
		{
			m_pShadow->setAnchorPoint(ccp(0.5f, 0));
			m_pShadow->setScale(2.0f);
			PlayerScene::getSingleton().addShadow(m_pShadow);
			m_pShadow->retain();
		}
	}

	{
		char strname[128];

		//for(int i = 0; i < 3; ++i)
		{
			sprintf(strname, "%s_2", m_info.res.c_str());

			m_pBulding = ImgMgr::getSingleton().loadSprite(strname);
			m_pBulding->setAnchorPoint(ccp(0.5f, 0));
			PlayerScene::getSingleton().m_pRoot->addChild(m_pBulding, SCENE_Z_BUILDING_BEGIN);
			m_pBulding->retain();
		}
	}

	{
		char strname[128];

		CCAnimation* pAni = CCAnimation::create();

		for(int i = 0; i < m_info.frameNums; ++i)
		{
			sprintf(strname, "%s_4_%d", m_info.res.c_str(), i);
			CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(strname);
			pAni->addSpriteFrame(pFrame);
		}

		pAni->setDelayPerUnit(0.2f);

		m_pPersonAni = CCAnimate::create(pAni);
		m_pPersonAni->retain();

		sprintf(strname, "%s_4_0", m_info.res.c_str());
		m_pSprAni = ImgMgr::getSingleton().loadSprite(strname);
		m_pSprAni->setAnchorPoint(ccp(0.5f, 0));
		PlayerScene::getSingleton().m_pRoot->addChild(m_pSprAni, SCENE_Z_BUILDING_BEGIN);
		m_pSprAni->retain();
	}

	m_pSprAni->setVisible(false);

	if(m_info.isAniLoop)
		m_lastPersonAniTime = 0;
	else
		m_lastPersonAniTime = rand() % 5000 + 3000;

	Building::init(buildingid, lx, ly);
}

void Building_Resource::setLogicPos(int lx, int ly)
{
	Building::setLogicPos(lx, ly);

	if(m_isPreBuilding)
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(false);

		if(m_pBulding != NULL)
			m_pBulding->setVisible(false);

		for(int i = 0; i < 2; ++i)
		{
			if(m_pLevel[i] != NULL)
				m_pLevel[i]->setVisible(false);
		}

		if(m_pSprAni != NULL)
			m_pSprAni->setVisible(false);
	}
	else if(isEmptyHP())
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(false);

		if(m_pBulding != NULL)
			m_pBulding->setVisible(false);

		for(int i = 0; i < 2; ++i)
		{
			if(m_pLevel[i] != NULL)
				m_pLevel[i]->setVisible(false);
		}

		if(m_pSprAni != NULL)
			m_pSprAni->setVisible(false);
	}
	else
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(true);

		if(m_pBulding != NULL)
			m_pBulding->setVisible(true);

		for(int i = 0; i < 2; ++i)
		{
			if(m_pLevel[i] != NULL)
				m_pLevel[i]->setVisible(true);
		}

		//if(m_pSprAni != NULL)
		//	m_pSprAni->setVisible(true);
	}

	//else
	{
		CCPoint pt = countSceneTilePt(lx, ly);

		if(m_pShadow != NULL)
			m_pShadow->setPosition(pt);

		if(m_pBulding != NULL)
			m_pBulding->setPosition(pt);

		for(int i = 0; i < 2; ++i)
		{
			if(m_pLevel[i] != NULL)
				m_pLevel[i]->setPosition(pt);
		}

		if(m_pSprAni != NULL)
			m_pSprAni->setPosition(pt);
	}
}

void Building_Resource::releaseFromScene()
{
	if(m_pPersonAni != NULL)
	{
		m_pPersonAni->release();

		m_pPersonAni = NULL;
	}

	if(m_pCurPersonAction != NULL)
	{
		m_pCurPersonAction->release();

		m_pCurPersonAction = NULL;
	}

	if(m_pSprAni != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pSprAni, false);

		m_pSprAni->release();
		//m_pBulding->release();
		m_pSprAni = NULL;
	}

	if(m_pShadow != NULL)
	{
		PlayerScene::getSingleton().removeShadow(m_pShadow, false);
		//m_scene.m_pRoot->removeChild(m_pShadow, false);
		//m_pBulding->release();
		m_pShadow = NULL;
	}

	if(m_pBulding != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pBulding, false);

		m_pBulding->release();
		//m_pBulding->release();
		m_pBulding = NULL;
	}

	for(int i = 0; i < 2; ++i)
	{
		if(m_pLevel[i] != NULL)
		{
			PlayerScene::getSingleton().m_pRoot->removeChild(m_pLevel[i], false);
			//m_pBulding->release();

			m_pLevel[i]->release();

			m_pLevel[i] = NULL;
		}
	}

	Building::releaseFromScene();
}

void Building_Resource::reorder(int zorder)
{
	Building::reorder(zorder);

	if(m_pBulding != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->reorderChild(m_pBulding, zorder);
	}

	for(int i = 0; i < 2; ++i)
	{
		if(m_pLevel[i] != NULL)
		{
			PlayerScene::getSingleton().m_pRoot->reorderChild(m_pLevel[i], zorder + 1);
		}
	}

	if(m_pSprAni != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->reorderChild(m_pSprAni, zorder + 2);
	}
}

void Building_Resource::chgSelMode(bool isSel)
{
	Building::chgSelMode(isSel);

	if(isSel)
	{
		{
			Action_GrayFlash* pLightFlash = Action_GrayFlash::create(3, 128, 256);
			CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
			m_pSelAction[0] = m_pBulding->runAction(pRF);
			setUpDownAction(m_pBulding, true);
		}

		for(int i = 0; i < 2; ++i)
		{
			if(m_pLevel[i] != NULL)
			{
				Action_GrayFlash* pLightFlash = Action_GrayFlash::create(3, 128, 256);
				CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
				m_pSelAction[i + 1] = m_pLevel[i]->runAction(pRF);

				setUpDownAction(m_pLevel[i], true);
			}
		}

		{
			Action_GrayFlash* pLightFlash = Action_GrayFlash::create(3, 128, 256);
			CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
			m_pSelAction[3] = m_pSprAni->runAction(pRF);

			setUpDownAction(m_pSprAni, true);
		}
	}
	else
	{
		ccColor3B c;
		c.r = c.g = c.b = 255;

		m_pBulding->stopAction(m_pSelAction[0]);
		m_pBulding->setColor(c);

		for(int i = 0; i < 2; ++i)
		{
			if(m_pLevel[i] != NULL)
			{
				m_pLevel[i]->stopAction(m_pSelAction[i + 1]);

				m_pLevel[i]->setColor(c);
			}
		}

		m_pSprAni->stopAction(m_pSelAction[3]);
		m_pSprAni->setColor(c);
	}
}

void Building_Resource::onBuildingEnd()
{
	if(m_isPreBuilding)
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(true);

		if(m_pBulding != NULL)
			m_pBulding->setVisible(true);

		for(int i = 0; i < 2; ++i)
		{
			if(m_pLevel[i] != NULL)
				m_pLevel[i]->setVisible(true);
		}

		if(m_pSprAni != NULL)
			m_pSprAni->setVisible(true);
	}

	Building::onBuildingEnd();
}

void Building_Resource::setBeAttackMode()
{
	if(m_pActBeAttack != NULL)
		return ;

	if(m_pBulding != NULL)
		setBeAttackAni(m_pBulding, true);

	for(int i = 0; i < 2; ++i)
	{
		if(m_pLevel[i] != NULL)
			setBeAttackAni(m_pLevel[i], false);
	}

	if(m_pSprAni != NULL)
		setBeAttackAni(m_pSprAni, false);

	if(m_pShadow != NULL)
		setBeAttackAni(m_pShadow, false);
}

void Building_Resource::onProc(int ot)
{
	Building::onProc(ot);

	if(!LogicInfo::getSingleton().isAttackMode())
	{
		if(m_pBuildingInfo != NULL && m_pBuildingInfo->state != 2)
			return ;

		if(m_pCurPersonAction != NULL && m_pCurPersonAction->isDone())
		{
			m_pCurPersonAction->release();

			if(m_info.isAniLoop)
			{
				m_pCurPersonAction = m_pSprAni->runAction(m_pPersonAni);
				m_pCurPersonAction->retain();
			}
			else
			{
				m_pSprAni->setVisible(false);
				m_pCurPersonAction = NULL;
			}
		}
		else if(m_pCurPersonAction == NULL)
		{
			m_lastPersonAniTime -= ot;
			if(m_lastPersonAniTime <= 0)
			{
				if(m_info.isAniLoop)
					m_lastPersonAniTime = 0;
				else
					m_lastPersonAniTime = rand() % 5000 + 3000;

				m_pSprAni->setVisible(true);
				m_pCurPersonAction = m_pSprAni->runAction(m_pPersonAni);
				m_pCurPersonAction->retain();
			}
		}
	}
}

// 建筑建造状态
void Building_Resource::onBuilding(bool isBuilding)
{
	if(m_pCurPersonAction != NULL && !m_pCurPersonAction->isDone())
	{
		m_pSprAni->stopAction(m_pCurPersonAction);
		m_pCurPersonAction->release();
		m_pCurPersonAction = NULL;
	}
}

NS_COC_END