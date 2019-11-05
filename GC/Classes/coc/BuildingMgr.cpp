#include "BuildingMgr.h"
#include "PlayerScene.h"

NS_COC_BEGIN

BuildingMgr::BuildingMgr()
{
	m_isRemote = true;
    
	load("build.cfg");
}

BuildingMgr::~BuildingMgr()
{
	release();
}

BuildingMgr& BuildingMgr::getSingleton()
{
	static BuildingMgr s_mgr;

	return s_mgr;
}

Building* BuildingMgr::newBuilding(int buildingid)
{
	_MapBuildingInfo::iterator it = m_mapBuildingInfo.find(buildingid);
	if(it != m_mapBuildingInfo.end())
	{
		switch(it->second.type)
		{
		case BUILDING_TYPE_COLLECT:
			return new Building_Resource(it->second);
		case BUILDING_TYPE_BARRACK:
			return new Building_Barrack(it->second);
		case BUILDING_TYPE_CANNON:
			return new Building_Cannon(it->second);
		case BUILDING_TYPE_STORAGE:
			return new Building_Storage(it->second);
		case BUILDING_TYPE_WALL:
			return new Building_Wall(it->second);
		case BUILDING_TYPE_TOWER:
			return new Building_Tower(it->second);
		case BUILDING_TYPE_ATTACK:
			return new Building_Attack(it->second);
		case BUILDING_TYPE_TRAP:
			return new Building_Trap(it->second);
		case BUILDING_TYPE_MORTAR:
			return new Building_Mortar(it->second);
        case BUILDING_TYPE_CLAN:
            return new Building_Clan(it->second);
        case BUILDING_TYPE_CLAN_ALLIES:
            return new Building_ClanAllies(it->second);
        case BUILDING_TYPE_HERO_ALTAR:
            return new Building_HeroAltar(it->second);
        case BUILDING_TYPE_AIR_DEFENSE:
            return new Building_AirDefense(it->second);
		case BUILDING_TYPE_NORMAL:
		default:
			return new Building_Normal(it->second);
		}
	}

	return NULL;
}

void BuildingMgr::onRelease()
{
	m_mapBuildingInfo.clear();
}

void BuildingMgr::onLoadEnd()
{
	for(int i = 1; i < getHeight(); ++i)
	{
		_PairBuildingInfo p;

		p.first = atoi(get("ID", i));

		p.second.buildingid = p.first;

		p.second.name = get("Name", i);
		p.second.res = get("ResName", i);

		p.second.connonFrameNums = atoi(get("cannonframenums", i));
		p.second.connonMainFrame = atoi(get("cannonmainframe", i));

		p.second.offHeight = atoi(get("heightOff", i));

		p.second.type = atoi(get("BuildingType", i));
		p.second.width = atoi(get("BuildingWidth", i)) * 2;

		p.second.buildtime[0] = atoi(get("BuildTimeD", i));
		p.second.buildtime[1] = atoi(get("BuildTimeH", i));
		p.second.buildtime[2] = atoi(get("BuildTimeM", i));

		if(strcmp(get("BuildResource", i), "Elixir") == 0)
			p.second.buildres = MONEY_ELIXIR;
		else if(strcmp(get("BuildResource", i), "Gold") == 0)
			p.second.buildres = MONEY_GOLD;
		else if(strcmp(get("BuildResource", i), "Diamonds") == 0)
			p.second.buildres = MONEY_GEM;
		else
			p.second.buildres = MONEY_GOLD;

		p.second.buildcost = atoi(get("BuildCost", i));
        p.second.removecost = atoi(get("RemoveCost", i));

		p.second.townhalllevel = atoi(get("TownHallLevel", i));

		p.second.maxstored[MONEY_GOLD - 1] = atoi(get("MaxStoredGold", i));
		p.second.maxstored[MONEY_ELIXIR - 1] = atoi(get("MaxStoredElixir", i));

		p.second.housingspace = atoi(get("HousingSpace", i));
        
		p.second.producesres = -1;
		if(strcmp(get("ProducesResource", i), "Elixir") == 0)
		{
			p.second.producesres = MONEY_ELIXIR;
			p.second.maxstored[MONEY_ELIXIR - 1] = atoi(get("ResourceMax", i));
		}
		else if(strcmp(get("ProducesResource", i), "Gold") == 0)
		{
			p.second.producesres = MONEY_GOLD;
			p.second.maxstored[MONEY_GOLD - 1] = atoi(get("ResourceMax", i));
		}

		p.second.producesperhour = atoi(get("ResourcePerHour", i));

		p.second.hp = atoi(get("Hitpoints", i));

		p.second.regentime = atoi(get("RegenTime", i));

		p.second.attackrange = atoi(get("AttackRange", i));
		p.second.attackspeed = atoi(get("AttackSpeed", i));
		p.second.damage = atoi(get("Damage", i));
		p.second.airtargets = atoi(get("AirTargets", i));
		p.second.groundtargets = atoi(get("GroundTargets", i));
		p.second.minattackrange = atoi(get("MinAttackRange", i));
		p.second.unitproduction = atoi(get("UnitProduction", i));
        
        p.second.exp = atoi(get("Exp", i));

		p.second.towerSoldierID = atoi(get("soldierTower", i));
		p.second.towerSoldierNums = atoi(get("soldierNumsTower", i));

		p.second.frameNums = atoi(get("frameNums", i));
		p.second.isAniLoop = (atoi(get("aniLoop", i)) != 0);

		for(int j = 0; j < 3; ++j)
		{
			char str[128];

			sprintf(str, "soldierX%d", (j + 1));
			p.second.towerSoldierX[j] = atoi(get(str, i));

			sprintf(str, "soldierY%d", (j + 1));
			p.second.towerSoldierY[j] = atoi(get(str, i));
		}

		p.second.attack_exp = atoi(get("attack_exp", i));

		m_mapBuildingInfo.insert(p);
	}
    
}

void BuildingMgr::release()
{
	m_mapBuildingInfo.clear();

	BaseCSV::release();
}

const BuildingInfo* BuildingMgr::getBuildingInfo(int buildingid) const
{
	_MapBuildingInfo::const_iterator it = m_mapBuildingInfo.find(buildingid);
	if(it != m_mapBuildingInfo.end())
		return &(it->second);

	return NULL;
}

const BuildingInfo* BuildingMgr::getBuildingMaxInfo(int buildingid) const
{
    const BuildingInfo* pBuilding = NULL;
    int level = 0;
    do {
        level++;
        pBuilding = getBuildingInfo((int)(buildingid * 100 + level));
    } while (pBuilding != NULL);
    
    pBuilding = getBuildingInfo((int)(buildingid * 100 + level-1));
	return pBuilding;
}

NS_COC_END