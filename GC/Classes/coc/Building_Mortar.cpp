#include "Building_Mortar.h"
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

Building_Mortar::Building_Mortar(BuildingInfo info)
	: Building_Attack(info)
{
	m_type = BUILDING_TYPE_MORTAR;
}

Building_Mortar::~Building_Mortar()
{
}

void Building_Mortar::init(int buildingid, int lx, int ly)
{

	Building_Attack::init(buildingid, lx, ly);
}

void Building_Mortar::setLogicPos(int lx, int ly)
{
	Building_Attack::setLogicPos(lx, ly);
}

void Building_Mortar::releaseFromScene()
{
	
	Building_Attack::releaseFromScene();
}

void Building_Mortar::reorder(int zorder)
{
	Building_Attack::reorder(zorder);
}

void Building_Mortar::chgSelMode(bool isSel)
{
	Building_Attack::chgSelMode(isSel);
}

void Building_Mortar::onBuildingEnd()
{
	Building_Attack::onBuildingEnd();
}

void Building_Mortar::setBeAttackMode()
{
	if(m_pActBeAttack != NULL)
		return;

	Building_Attack::setBeAttackMode();
}

void Building_Mortar::onStartAttack()
{
	int bid = m_buildingid / 100;

    float bx = m_pBulding->getPositionX();
    float by = m_pBulding->getPositionY() + m_pBulding->getContentSize().height/2;
    
	if(bid == BUILDING_BIG_MORTAR)
	{
		//int damage = m_info.damage * m_info.attackspeed / 1000;
		int damage = fixDamage(m_info.damage) * fixAttspeed(m_info.attackspeed) / 1000;

        PlayerScene::getSingleton().addBoomEx("building104_atk", 0, bx, by);
        
        by += 120;
        
		PlayerScene::getSingleton().addProjectile2("building104_bullet", bx, by, m_pDestPerson, 1.0f, "building104_hit", false,
			true, true, 500, "", 0.0f, false, true, damage, CAMP_DEFENCE, 1.5f, false);
	}

    SoundMgr::getSingleton().playEffect("building_attack_10201_0.mp3");
}

NS_COC_END