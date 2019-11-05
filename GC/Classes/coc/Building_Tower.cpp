#include "Building_Tower.h"
#include "BuildingMgr.h"
#include "ImgMgr.h"
#include "SoundMgr.h"
#include "PlayerScene.h"
#include "Action_LightFlash.h"
#include "Action_GrayFlash.h"
#include "Person.h"
#include "LogicInfo.h"

NS_COC_BEGIN

Building_Tower::Building_Tower(BuildingInfo info)
: Building_Attack(info)
, m_iDirEx(-1)
{
	m_type = BUILDING_TYPE_TOWER;
}

Building_Tower::~Building_Tower()
{
}

void Building_Tower::init(int buildingid, int lx, int ly)
{
	for(int i = 0; i < m_info.towerSoldierNums; ++i)
	{
		Person* pSoldier = PlayerScene::getSingleton().addTowerPerson(m_info.towerSoldierID, lx, ly);
		pSoldier->m_damageOnTower = m_info.damage / m_info.towerSoldierNums;
		//pSoldier->m_statePerson = PERSONSTATE_TOWER;

		pSoldier->m_ptTower.x = m_info.towerSoldierX[i];
		pSoldier->m_ptTower.y = m_info.towerSoldierY[i];

		m_lstSoldier.push_back(pSoldier);
	}
    
	Building_Attack::init(buildingid, lx, ly);
}

void Building_Tower::setLogicPos(int lx, int ly)
{
	Building_Attack::setLogicPos(lx, ly);

	if(m_isPreBuilding)
	{
		//for(std::vector<Person*>::iterator it = m_lstSoldier.begin(); it != m_lstSoldier.end(); ++it)
		//{
		//	Person* pSoldier = *it;
		//	if(pSoldier != NULL)
		//		pSoldier->setVisible(false);
		//}
	}
	else if(isEmptyHP())
	{
		for(std::vector<Person*>::iterator it = m_lstSoldier.begin(); it != m_lstSoldier.end(); ++it)
		{
			Person* pSoldier = *it;
			if(pSoldier != NULL)
				PlayerScene::getSingleton().killPerson(pSoldier);
		}

		m_lstSoldier.clear();
	}
	else
	{
	}

	{
		for(std::vector<Person*>::iterator it = m_lstSoldier.begin(); it != m_lstSoldier.end(); ++it)
		{
			Person* pSoldier = *it;
			if(pSoldier != NULL)
			{
				pSoldier->setLogicPos(lx, ly);
			}
		}
	}
}

int Building_Tower::getBuildingWidth()
{
    return m_pBulding->getTextureRect().size.width;
}

int Building_Tower::getBuildingHeight()
{
    int height = m_pBulding->getOffsetPosition().y + m_pBulding->getTextureRect().size.height;
    if (m_lstSoldier.size() > 0) {
		height = m_info.towerSoldierY[0] + m_lstSoldier[0]->m_sprPerson->getTextureRect().size.height/2;
    }

    return height;
}

void Building_Tower::releaseFromScene()
{
	for(std::vector<Person*>::iterator it = m_lstSoldier.begin(); it != m_lstSoldier.end(); ++it)
	{
		Person* pSoldier = *it;
		if(pSoldier != NULL)
			PlayerScene::getSingleton().killPerson(pSoldier);
	}

	m_lstSoldier.clear();

	Building_Attack::releaseFromScene();
}

void Building_Tower::reorder(int zorder)
{
	Building_Attack::reorder(zorder);
}

void Building_Tower::chgSelMode(bool isSel)
{
	Building_Attack::chgSelMode(isSel);
}

void Building_Tower::onProc(int ot)
{
	Building_Attack::onProc(ot);

	if(LogicInfo::getSingleton().isAttackMode())
	{
		for(std::vector<Person*>::iterator it = m_lstSoldier.begin(); it != m_lstSoldier.end(); ++it)
		{
			Person* pSoldier = *it;
            if(pSoldier != NULL) {
                pSoldier->chgDestTarget(m_pDestPerson);
            }
		}
	}

	//Building_Attack::onProc(ot);
}

void Building_Tower::onBuildingEnd()
{
	Building_Attack::onBuildingEnd();
}

void Building_Tower::setBeAttackMode()
{
	Building_Attack::setBeAttackMode();
}

CCPoint ptArcherTowerOff[4] = {ccp(-62, 26), ccp(64, 26), ccp(64, 144), ccp(-70, 144)};
void Building_Tower::onStartAttack()
{
    int bid = m_buildingid / 100;
    
    float bx;
    float by;
    
	if(BUILDING_ARCHER_TOWER == bid)
	{
        if(m_pDestPerson != NULL)
        {
            CCPoint pos = PlayerScene::getSingleton().countScenePos(getCenterPt().x, getCenterPt().y);
            
            bx = pos.x;
            by = pos.y;
            
            int dx = m_pDestPerson->m_sprPerson->getPositionX();
            int dy = m_pDestPerson->m_sprPerson->getPositionY();
            
            int dir = 0;
            if(bx < dx)
            {
                if(by < dy)
                    dir = 2;
                else
                    dir = 1;
            }
            else
            {
                if(by < dy)
                    dir = 3;
                else
                    dir = 0;
            }
            
            bx = m_pBulding->getPositionX();
            by = m_pBulding->getPositionY() + m_pBulding->getContentSize().height/2;
            
            PlayerScene::getSingleton().addBoomEx("building102_atk", dir, bx, by, false);
            
            int damage = fixDamage(m_info.damage) * fixAttspeed(m_info.attackspeed) / 1000;
            
            bx += ptArcherTowerOff[dir].x;
            by += ptArcherTowerOff[dir].y;
            
            PlayerScene::getSingleton().addProjectile2("building102_bullet", bx, by, m_pDestPerson, 0, "building102_hit", false,
                    true, true, 2000, "", 0.0f, true, false, damage, CAMP_DEFENCE, 0.0f, false);
            SoundMgr::getSingleton().playEffect("building_attack_10101_0.mp3");
        }
	}
}


// 建筑建造状态
void Building_Tower::onBuilding(bool isBuilding)
{
	//if(isBuilding)
	{
		for(std::vector<Person*>::iterator it = m_lstSoldier.begin(); it != m_lstSoldier.end(); ++it)
		{
			Person* pSoldier = *it;
			if(pSoldier != NULL)
				pSoldier->setVisible(isBuilding ? false : true);
		}
	}
}

NS_COC_END