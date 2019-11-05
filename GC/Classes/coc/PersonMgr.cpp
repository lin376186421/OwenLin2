#include "PersonMgr.h"
#include "PlayerScene.h"

NS_COC_BEGIN

PersonMgr::PersonMgr()
{
	m_isRemote = true;

	load("player.cfg");
}

PersonMgr::~PersonMgr()
{
}

PersonMgr& PersonMgr::getSingleton()
{
	static PersonMgr s_mgr;

	return s_mgr;
}

Person* PersonMgr::newPerson(int personid, int camp)
{
	_MapSoldierInfo::iterator it = m_mapSoldier.find(personid);
	if(it != m_mapSoldier.end())
	{
		Person* pPerson = new Person(it->second, camp);
		if(pPerson != NULL)
			pPerson->onLoadRes();

		return pPerson;
	}

	return NULL;
}

void PersonMgr::onRelease()
{
	m_mapSoldier.clear();
}

void PersonMgr::onLoadEnd()
{
	for(int i = 1; i < getHeight(); ++i)
	{
		_PairSoldierInfo p;

		p.first = atoi(get("ID", i));

		p.second.soldierid = p.first;

		p.second.name = get("Name", i);
		p.second.resname = get("ResID", i);

		p.second.frames[0] = atoi(get("frames1", i));
		p.second.frames[1] = atoi(get("frames2", i));
		p.second.frames[2] = atoi(get("frames3", i));
		p.second.frames[3] = atoi(get("frames4", i));
		p.second.frames[4] = atoi(get("frames5", i));

		p.second.framesTime[0] = atoi(get("frames1Time", i));
		p.second.framesTime[1] = atoi(get("frames2Time", i));
		p.second.framesTime[2] = atoi(get("frames3Time", i));
		p.second.framesTime[3] = atoi(get("frames4Time", i));
		p.second.framesTime[4] = atoi(get("frames5Time", i));

		p.second.timeShoot = atoi(get("shootTime", i));

		p.second.dirmode = atoi(get("dir", i));

		p.second.housingSpaceex = atoi(get("HousingSpace", i));
		p.second.barrackLevelex = atoi(get("BarrackLevel", i));
		p.second.laboratoryLevelex = atoi(get("LaboratoryLevel", i));
		p.second.speedex = atoi(get("Speed", i));
		p.second.hpex = atoi(get("Hitpoints", i));

		p.second.trainingTime = atoi(get("TrainingTime", i));

		if(strcmp(get("TrainingResource", i), "Elixir") == 0)
			p.second.trainingRes = MONEY_ELIXIR;
		else
			p.second.trainingRes = MONEY_GEM;

		p.second.trainingCost = atoi(get("TrainingCost", i));

		p.second.upgradeTimeHour = atoi(get("UpgradeTimeH", i));
		p.second.upgradeRes = atoi(get("UpgradeResource", i));
		p.second.upgradeCost = atoi(get("UpgradeCost", i));

		p.second.attackRangeex = atoi(get("AttackRange", i));
		p.second.attackSpeedex = atoi(get("AttackSpeed", i));
		p.second.damageex = atoi(get("Damage", i));
		p.second.preferedTargetDamageMod = atoi(get("PreferedTargetDamageMod", i));
		p.second.damageRadius = atoi(get("DamageRadius", i));
		p.second.preferedTargetBuilding = atoi(get("PreferedTargetBuilding", i));
		p.second.isFlying = atoi(get("IsFlying", i));
		p.second.airTargets = atoi(get("AirTargets", i));
		p.second.groundTargets = atoi(get("GroundTargets", i));
        p.second.attackTarget = atoi(get("AttackTarget", i));
		p.second.needexp = atoi(get("needexp", i));

		p.second.needtownhall = atoi(get("needtownhall", i));
		p.second.baseexp = atoi(get("baseexp", i));
		p.second.recoverhp = atoi(get("recoverhp", i));

		m_mapSoldier.insert(p);
	}
}

const SoldierInfo* PersonMgr::getSoldierInfo(int soldierid) const
{
	_MapSoldierInfo::const_iterator it = m_mapSoldier.find(soldierid);
	if(it != m_mapSoldier.end())
		return &(it->second);

	return NULL;
}

const SoldierInfo* PersonMgr::getSoldierMaxInfo(int soldierid) const
{
	const SoldierInfo* info = PersonMgr::getSingleton().getSoldierInfo(soldierid);
    
    while(info != NULL)
    {
        soldierid++;
        info = PersonMgr::getSingleton().getSoldierInfo(soldierid);
    }
    info = PersonMgr::getSingleton().getSoldierInfo(soldierid-1);
    return info;
}
NS_COC_END