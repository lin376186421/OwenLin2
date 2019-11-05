#include "Building_Barrack.h"
#include "BuildingMgr.h"
#include "ImgMgr.h"
#include "PlayerScene.h"
#include "Action_LightFlash.h"
#include "Action_GrayFlash.h"
#include "LogicInfo.h"

NS_COC_BEGIN

Building_Barrack::Building_Barrack(BuildingInfo info)
	: Building(info)
	, m_pBulding(NULL)
	, m_pShadow(NULL)
//	, m_pLighting(NULL)
	, m_pSelAction(NULL)
	, m_iCurSoldierID(-1)
{
	m_type = BUILDING_TYPE_BARRACK;
}

Building_Barrack::~Building_Barrack()
{
}

void Building_Barrack::init(int buildingid, int lx, int ly)
{
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
		}
	}

	{
		char strname[128];
		sprintf(strname, "%s_2", m_info.res.c_str());
		m_pBulding = ImgMgr::getSingleton().loadSprite(strname);
		m_pBulding->setAnchorPoint(ccp(0.5f, 0));
		m_pBulding->retain();
		PlayerScene::getSingleton().m_pRoot->addChild(m_pBulding, SCENE_Z_BUILDING_BEGIN);
	}

	//{
	//	char strname[128];
	//	sprintf(strname, "%s_2_0", m_info.res);
	//	m_pLighting = ImgMgr::getSingleton().loadImg(strname);
	//	m_pLighting->setAnchorPoint(ccp(0, 0));
	//	m_pLighting->retain();
	//	m_scene.m_pRoot->addChild(m_pLighting, SCENE_Z_BUILDING_BEGIN);

	//	Action_LightFlash* pLightFlash = Action_LightFlash::create(3, 128, 196);
	//	CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
	//	m_pLighting->runAction(pRF);
	//}

	Building::init(buildingid, lx, ly);
}

void Building_Barrack::setLogicPos(int lx, int ly)
{
	Building::setLogicPos(lx, ly);

	if(m_isPreBuilding)
	{
		if(m_pBulding != NULL)
			m_pBulding->setVisible(false);

		if(m_pShadow != NULL)
			m_pShadow->setVisible(false);
	}
	else if(isEmptyHP())
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(false);

		if(m_pBulding != NULL)
			m_pBulding->setVisible(false);
//		m_pLighting->setVisible(false);
	}
	else
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(true);

		if(m_pBulding != NULL)
			m_pBulding->setVisible(true);
	}

	//else
	{
		CCPoint pt = countSceneTilePt(lx, ly);

		CCPoint cpt = pt;

		if(m_pShadow != NULL)
			m_pShadow->setPosition(cpt);

		if(m_pBulding != NULL)
			m_pBulding->setPosition(cpt);
	}
}

void Building_Barrack::releaseFromScene()
{
	if(m_pShadow != NULL)
	{
		PlayerScene::getSingleton().removeShadow(m_pShadow, false);
		//m_scene.m_pRoot->removeChild(m_pShadow, true);
		m_pShadow->release();
		m_pShadow = NULL;
	}

	if(m_pBulding != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pBulding, true);
		m_pBulding->release();
		m_pBulding = NULL;
	}

	//if(m_pLighting != NULL)
	//{
	//	m_scene.m_pRoot->removeChild(m_pLighting, true);
	//	m_pLighting->release();
	//	m_pLighting = NULL;
	//}

	Building::releaseFromScene();
}

void Building_Barrack::reorder(int zorder)
{
	Building::reorder(zorder);

	if(m_pBulding != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->reorderChild(m_pBulding, zorder);
	}
}

void Building_Barrack::chgSelMode(bool isSel)
{
	Building::chgSelMode(isSel);

	if(isSel)
	{
		Action_GrayFlash* pLightFlash = Action_GrayFlash::create(3, 128, 256);
		CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
		m_pSelAction = m_pBulding->runAction(pRF);

		setUpDownAction(m_pBulding, true);
//		setUpDownAction(m_pLighting);
	}
	else
	{
		m_pBulding->stopAction(m_pSelAction);
        m_pSelAction = NULL;
        
		ccColor3B c;
		c.r = c.g = c.b = 255;

		m_pBulding->setColor(c);
	}
}

void Building_Barrack::onBuildingEnd()
{
	if(m_isPreBuilding)
	{
		m_pBulding->setVisible(true);
	}

	Building::onBuildingEnd();
}

void Building_Barrack::setBeAttackMode()
{
	if(m_pActBeAttack != NULL)
		return ;

	if(m_pBulding != NULL)
		setBeAttackAni(m_pBulding, true);

	if(m_pShadow != NULL)
		setBeAttackAni(m_pShadow, false);
}

void Building_Barrack::onUpdateTime()
{
	if(m_lasttime <= 0)
	{
		m_iCurSoldierID = -1;

		setIcon(NULL);
	}
	else if(m_pBuildingInfo != NULL && m_pBuildingInfo->state == BUILDING_STATE_NORMAL)
	{
		int bid = m_info.buildingid / 100;
		if(bid == BUILDING_LABORATORY)
		{
			int sid = LogicInfo::getSingleton().getCurUpgradeSoldier();
			if(sid > 0)
			{
				if(m_iCurSoldierID != sid)
				{
					m_iCurSoldierID = sid;

					char str[128];
					sprintf(str, "player2_%d", sid/100);

					setIcon(str);
				}
			}
		}
		else if(m_pBuildingInfo->barrackid >= 0)
		{
			BarrackInfo* pBarrack = LogicInfo::getSingleton().getBarrackInfo(m_pBuildingInfo->barrackid);
			if(pBarrack != NULL && !pBarrack->lstSoldier.empty())
			{
				if(m_iCurSoldierID != pBarrack->lstSoldier.front().pid)
				{
					m_iCurSoldierID = pBarrack->lstSoldier.front().pid;

					char str[128];
					sprintf(str, "player2_%d", m_iCurSoldierID/100);

					setIcon(str);
				}
			}
		}
	}

	Building::onUpdateTime();
}

NS_COC_END