//
//  Building_HeroAltar.cpp
//  GameClient
//
//  Created by qsun on 14-3-7.
//
//

#include "Building_HeroAltar.h"
#include "BuildingMgr.h"
#include "ImgMgr.h"
#include "PlayerScene.h"
#include "LogicInfo.h"
#include "Action_LightFlash.h"
#include "Action_GrayFlash.h"
#include "HeroMgr.h"
#include "Person.h"

NS_COC_BEGIN

Building_HeroAltar::Building_HeroAltar(BuildingInfo info)
: Building(info)
, m_pBulding(NULL)
, m_pShadow(NULL)
, m_pFlag(NULL)
, m_pSelAction(NULL)
{
	m_type = BUILDING_TYPE_HERO_ALTAR;
}

void Building_HeroAltar::init(int buildingid, int lx, int ly)
{
	{
		char strname[128];
		sprintf(strname, "%s_2", m_info.res.c_str());
		m_pBulding = ImgMgr::getSingleton().loadSprite(strname);
		m_pBulding->setAnchorPoint(ccp(0.5f, 0));
		m_pBulding->retain();
		PlayerScene::getSingleton().m_pRoot->addChild(m_pBulding, SCENE_Z_BUILDING_BEGIN);
	}
    
	{
		char strname[128];
		sprintf(strname, "area_white");
		m_pLightArea = ImgMgr::getSingleton().loadSprite(strname);
		//m_pLightArea->setAnchorPoint(ccp(0, 0));
		m_pLightArea->retain();
		PlayerScene::getSingleton().m_pRoot->addChild(m_pLightArea, SCENE_Z_HUAN);
        
		Action_LightFlash* pLightFlash = Action_LightFlash::create(3, 156, 256);
		CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
		m_pLightArea->runAction(pRF);
        
		m_pLightArea->setScale(m_info.attackrange * 2 / 500.0f);
        
		m_pLightArea->setVisible(false);
	}
    
	Building::init(buildingid, lx, ly);
}

void Building_HeroAltar::setLogicPos(int lx, int ly)
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
    
    if (HeroMgr::getSingleton().m_heroState == HERO_STATE_UPGRADING) {
        if (PlayerScene::getSingleton().m_pHero)
        {
            Person* pHero = PlayerScene::getSingleton().m_pHero;
            
            pHero->setLogicPos(getCenterPt().x, getCenterPt().y);
            pHero->reorder(m_pBulding->getZOrder() + 1);
        }
    }
}

void Building_HeroAltar::releaseFromScene()
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
    
	if(m_pFlag != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pFlag, false);
		m_pFlag->release();
		m_pFlag = NULL;
	}
    
	if(m_pLightArea != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pLightArea, true);
		m_pLightArea->release();
		m_pLightArea = NULL;
	}
    
	Building::releaseFromScene();
}

void Building_HeroAltar::reorder(int zorder)
{
    Building::reorder(zorder);
    
	if(m_pBulding != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->reorderChild(m_pBulding, zorder);
	}
}

void Building_HeroAltar::chgSelMode(bool isSel)
{
	Building::chgSelMode(isSel);
    
	if(isSel)
	{
		Action_GrayFlash* pLightFlash = Action_GrayFlash::create(3, 128, 256);
		CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
		m_pSelAction = m_pBulding->runAction(pRF);
        
		setUpDownAction(m_pBulding, false);
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

void Building_HeroAltar::onProc(int ot)
{
    Building::onProc(ot);
    
    //! 防守英雄出战
    if(!PlayerScene::getSingleton().m_bDefenceHeroFight)
    {
        PlayerScene::getSingleton().addDefenceHero();
    }    
}

void Building_HeroAltar::onBuildingEnd()
{
    
}

void Building_HeroAltar::setBeAttackMode()
{
    
}

void Building_HeroAltar::onBuilding(bool isBuilding)
{
    
}

Building_HeroAltar::~Building_HeroAltar()
{
}

NS_COC_END