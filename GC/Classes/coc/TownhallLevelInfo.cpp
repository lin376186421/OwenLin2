#include "TownhallLevelInfo.h"
#include "LogicInfo.h"

NS_COC_BEGIN

TownhallLevelInfo::TownhallLevelInfo()
{
	load("townhall.cfg");
}

TownhallLevelInfo::~TownhallLevelInfo()
{
	release();
}

TownhallLevelInfo& TownhallLevelInfo::getSingleton()
{
	static TownhallLevelInfo s_mgr;

	return s_mgr;
}

void TownhallLevelInfo::onLoadEnd()
{
	for(int i = 1; i < getHeight(); ++i)
	{
		_PairTownhallInfo p;

		p.first = atoi(get("Name", i));

		p.second.level = p.first;

		p.second.attackCost = atoi(get("AttackCost", i));
		p.second.troopHousing = atoi(get("Troop-Housing", i));
		p.second.elixirStorage = atoi(get("Elixir-Storage", i));
		p.second.goldStorage = atoi(get("Gold-Storage", i));
		p.second.elixirPump = atoi(get("Elixir-Pump", i));
		p.second.goldMine = atoi(get("Gold-Mine", i));
		p.second.barrack = atoi(get("Barrack", i));
		p.second.cannon = atoi(get("Cannon", i));
		p.second.wall = atoi(get("Wall", i));
		p.second.archerTower = atoi(get("Archer-Tower", i));
		p.second.wizardTower = atoi(get("Wizard-Tower", i));
		p.second.airDefense = atoi(get("Air-Defense", i));
		p.second.mortar = atoi(get("Mortar", i));
		p.second.workerBuilding = atoi(get("Worker-Building", i));
		p.second.laboratory = atoi(get("Laboratory", i));
        p.second.heroAltar = atoi(get("HeroAltar", i));
        
		p.second.bomb = atoi(get("bomb", i));
		p.second.flybomb = atoi(get("flybomb", i));
		p.second.spring = atoi(get("spring", i));
		p.second.bigbomb = atoi(get("bigbomb", i));
        
		p.second.obuilding[0] = 1;
        p.second.obuilding[1] = 1;
        p.second.obuilding[2] = 1;
        p.second.obuilding[3] = 1;
        p.second.obuilding[4] = 4;
        p.second.obuilding[5] = 4;
        p.second.obuilding[6] = 4;
        
        for(int i = 0; i < 40; ++i)
        {
            p.second.letter[i] = 3;
        }

		m_mapInfo.insert(p);
	}
}

int TownhallLevelInfo::getAttackCost(int level)
{
    int cost = 1000;
    _MapTownhallInfo::const_iterator it = m_mapInfo.find(level);
	if(it != m_mapInfo.end())
    {
        cost = it->second.attackCost;
    }
    return cost;
}

void TownhallLevelInfo::onRelease()
{
	m_mapInfo.clear();
}

void TownhallLevelInfo::release()
{
	m_mapInfo.clear();

	BaseCSV::release();
}

const Townhall_BuildingNums* TownhallLevelInfo::getInfo(int level) const
{
	_MapTownhallInfo::const_iterator it = m_mapInfo.find(level);
	if(it != m_mapInfo.end())
		return &(it->second);

	return NULL;
}

vector<int> TownhallLevelInfo::getLevelBuildings()
{
    vector<int> builds;
    builds.push_back(BUILDING_CANNON);
    builds.push_back(BUILDING_ARCHER_TOWER);
    builds.push_back(BUILDING_WALL);
    builds.push_back(BUILDING_BIG_MORTAR);
    builds.push_back(BUILDING_AIR_DEFENSE);
    builds.push_back(BUILDING_WIZARD_TOWER);
    builds.push_back(BUILDING_ELIXIR_COLLECTOR);
    builds.push_back(BUILDING_ELIXIR_STORAGE);
    builds.push_back(BUILDING_GOLD_MINE);
    builds.push_back(BUILDING_GOLD_STORAGE);
    builds.push_back(BUILDING_ARMY_CAMP);
    builds.push_back(BUILDING_BARRACKS);
    builds.push_back(BUILDING_LABORATORY);
    builds.push_back(BUILDING_BOMB);
    builds.push_back(BUILDING_SPIKE_TRAP);
    builds.push_back(BUILDING_AIR_BOMB);
    builds.push_back(BUILDING_BIG_BOMB);
    builds.push_back(BUILDING_HERO_ALTAR);

    return builds;
}

int TownhallLevelInfo::getBuildingNums(const Townhall_BuildingNums* buildingNums, int buildid)
{
    if (buildid == BUILDING_CANNON) {
        return buildingNums->cannon;
    }
    else if (buildid == BUILDING_ARCHER_TOWER)
    {
        return buildingNums->archerTower;
    }
    else if (buildid == BUILDING_WALL)
    {
        return buildingNums->wall;
    }
    else if (buildid == BUILDING_BIG_MORTAR)
    {
        return buildingNums->mortar;
    }
    else if (buildid == BUILDING_HERO_ALTAR)
    {
        return buildingNums->heroAltar;
    }
    else if (buildid == BUILDING_AIR_DEFENSE)
    {
        return buildingNums->airDefense;
    }
    else if (buildid == BUILDING_WIZARD_TOWER)
    {
        return buildingNums->wizardTower;
    }
    else if (buildid == BUILDING_ELIXIR_COLLECTOR)
    {
        return buildingNums->elixirPump;
    }
    else if (buildid == BUILDING_ELIXIR_STORAGE)
    {
        return buildingNums->elixirStorage;
    }
    else if (buildid == BUILDING_GOLD_MINE)
    {
        return buildingNums->goldMine;
    }
    else if (buildid == BUILDING_GOLD_STORAGE)
    {
        return buildingNums->goldStorage;
    }
    else if (buildid == BUILDING_ARMY_CAMP)
    {
        return buildingNums->troopHousing;
    }
    else if (buildid == BUILDING_BARRACKS)
    {
        return buildingNums->barrack;
    }
    else if (buildid == BUILDING_LABORATORY)
    {
        return buildingNums->laboratory;
    }
    else if (buildid == BUILDING_BOMB)
    {
        return buildingNums->bomb;
    }
    else if (buildid == BUILDING_SPIKE_TRAP)
    {
        return buildingNums->spring;
    }
    else if (buildid == BUILDING_AIR_BOMB)
    {
        return buildingNums->flybomb;
    }
    else if (buildid == BUILDING_BIG_BOMB)
    {
        return buildingNums->bigbomb;
    }
    else if (buildid == BUILDING_MAIL_BOX)
    {
        return buildingNums->obuilding[0];
    }
    else if (buildid == BUILDING_STELE)
    {
        return buildingNums->obuilding[1];
    }
    else if (buildid == BUILDING_WEATHER_VANE)
    {
        return buildingNums->obuilding[2];
    }
    else if (buildid == BUILDING_ROAD_SIGN)
    {
        return buildingNums->obuilding[3];
    }
    else if (buildid == BUILDING_BULLETIN_BOARD)
    {
        return buildingNums->obuilding[4];
    }
    else if (buildid == BUILDING_FLOWER_BED1)
    {
        return buildingNums->obuilding[5];
    }
    else if (buildid == BUILDING_FLOWER_BED2)
    {
        return buildingNums->obuilding[6];
    }
    else if (buildid == BUILDING_BUILDER_HOME)
    {
        return buildingNums->workerBuilding;
    }
    else if(buildid >= BUILDING_LETTER && buildid <= BUILDING_LETTER + BUILDING_LETTER_NUM-1)
    {
        return buildingNums->letter[buildid - BUILDING_LETTER];
    }
    
	return 0;
}

int TownhallLevelInfo::getBuildingCurNums(int buildid)
{
    Townhall_BuildingNums curNums = LogicInfo::getSingleton().m_curTownhall;
    return getBuildingNums(&curNums, buildid);
}

int TownhallLevelInfo::getBuildingMaxNums(int buildid)
{
    Townhall_BuildingNums curNums = LogicInfo::getSingleton().m_curTownhall;
    const Townhall_BuildingNums* pMaxNums = TownhallLevelInfo::getSingleton().getInfo(curNums.level);
    if(pMaxNums == NULL)
        return 0;
    
    return getBuildingNums(pMaxNums, buildid);
}

NS_COC_END