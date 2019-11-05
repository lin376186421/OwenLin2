//
//  Building_AirDefense.cpp
//  GameClient
//
//  Created by qsun on 14-5-4.
//
//

#include "Building_AirDefense.h"
#include "BuildingMgr.h"
#include "ImgMgr.h"
#include "PlayerScene.h"
#include "Action_LightFlash.h"
#include "Action_GrayFlash.h"
#include "Person.h"
#include "SimpleAudioEngine.h"
#include "SoundMgr.h"
#include "LogicInfo.h"

NS_COC_BEGIN

Building_AirDefense::Building_AirDefense(BuildingInfo info)
: Building_Attack(info)
{
	m_type = BUILDING_TYPE_AIR_DEFENSE;
}

Building_AirDefense::~Building_AirDefense()
{
}

void Building_AirDefense::init(int buildingid, int lx, int ly)
{
	Building_Attack::init(buildingid, lx, ly);
}

void Building_AirDefense::setLogicPos(int lx, int ly)
{
	Building_Attack::setLogicPos(lx, ly);
}

void Building_AirDefense::releaseFromScene()
{
	
	Building_Attack::releaseFromScene();
}

void Building_AirDefense::reorder(int zorder)
{
	Building_Attack::reorder(zorder);
}

void Building_AirDefense::chgSelMode(bool isSel)
{
	Building_Attack::chgSelMode(isSel);
}

void Building_AirDefense::onBuildingEnd()
{
	Building_Attack::onBuildingEnd();
}

void Building_AirDefense::setBeAttackMode()
{
	if(m_pActBeAttack != NULL)
		return ;
    
	Building_Attack::setBeAttackMode();
}

void Building_AirDefense::onStartAttack()
{
	int bid = m_buildingid / 100;
        
    float bx = m_pBulding->getPositionX();
    float by = m_pBulding->getPositionY() + m_pBulding->getContentSize().height/2;
    
	if(bid == BUILDING_AIR_DEFENSE)
	{
        PlayerScene::getSingleton().addBoomEx("building106_atk", 0, bx, by, false);
        
		int damage = fixDamage(m_info.damage) * fixAttspeed(m_info.attackspeed) / 1000;
        
        bx += 10;
        by += 50;
        
        PlayerScene::getSingleton().addProjectile2("building106_bullet", bx, by, m_pDestPerson, 0.02f, "building106_hit", false, false, false, 1500, "", 0.0f, true, false, damage, CAMP_DEFENCE, 0.0f, false);
        
        SoundMgr::getSingleton().playEffect("building_attack_10601_0.mp3");
    }
}

NS_COC_END