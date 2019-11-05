 //
//  SkillMgr.cpp
//  GameClient
//
//  Created by chowray on 13-9-23.
//
//

#include "SkillMgr.h"
#include "PlayerScene.h"
#include "GameWorldScene.h"
#include "HttpClient.h"
#include "LogicInfo.h"
#include "RandMgr.h"
#include "HeroMgr.h"
#include "Person.h"
#include "Building.h"

NS_COC_BEGIN

SkillMgr::SkillMgr()
{
	m_isRemote = true;
	load("heroskill.cfg");
}

SkillMgr::~SkillMgr()
{
	release();
}

SkillMgr& SkillMgr::getSingleton()
{
	static SkillMgr s_mgr;
    
	return s_mgr;
}

void SkillMgr::onRelease()
{
	m_mapInfo.clear();
}

void SkillMgr::onLoadEnd()
{
	for(int i = 1; i < getHeight(); ++i)
	{
		_Pair p;
        
		p.first = atoi(get("skillid", i));
        
		p.second.skillid = p.first;
		p.second.name = get("name", i);
		p.second.info = get("info", i);
		p.second.needlevel = atoi(get("needlevel", i));

		string::size_type pos = 0;
		do{
			pos = p.second.info.find('^');
			if(pos != string::npos)
				p.second.info.replace(pos, 1, ",");
		}while(pos != string::npos);

		p.second.per = atoi(get("per", i));
		p.second.per_begin = atoi(get("per_begin", i));

		p.second.att_name = get("att_name", i);
		p.second.att_layer = atoi(get("att_layer", i));
		p.second.att_type = atoi(get("att_type", i));
		p.second.eff_name = get("eff_name", i);
		p.second.eff_layer = atoi(get("eff_layer", i));
		p.second.eff_type = atoi(get("eff_type", i));

		p.second.cd = atoi(get("cd", i));

		p.second.bvalue1 = atoi(get("bvalue1", i));
		p.second.bvalue2 = atoi(get("bvalue2", i));
		p.second.btime1 = atoi(get("btime1", i));
		p.second.delay = atoi(get("delay", i));

		p.second.maxlevel = atoi(get("maxlevel", i));

		p.second.trainExp = atoi(get("upexp", i));
		p.second.maxExp = atoi(get("needexp", i));
		p.second.trainGem = atoi(get("costgem", i));
        
		m_mapInfo.insert(p);
	}
}

void SkillMgr::release()
{
	m_mapInfo.clear();
    
	BaseCSV::release();
}

void SkillMgr::init()
{
    m_skillNums = 8;
    
    //    for(int i = 0; i < 23; ++ i)
    //    {
    //        m_lstTask.push_back(new TaskInfo());
    //    }
    //
    //    //    curTaskID = 10001;
    //
    //    for(int i = 0; i < curTaskID % 10000 - 1; ++i)
    //    {
    //        m_lstTask[i]->state = 2;
    //    }
    //
    //    if(curTaskID != 0)
    //    {
    //        m_lstTask[curTaskID % 10000 - 1]->state = 1;
    //    }
    //
    //    for(int i = curTaskID % 10000; i < 23; ++i)
    //    {
    //        m_lstTask[i]->state = 0;
    //    }
}

const SkillInfo* SkillMgr::getNode(int id) const
{
	_Map::const_iterator it = m_mapInfo.find(id);
	if(it != m_mapInfo.end())
		return &(it->second);
    
	return NULL;
}

int SkillMgr::chgSkill3(int skillid)
{
	if(HeroMgr::getSingleton().m_myHero.skill3 == skillid)
		return 1;
    
	HeroMgr::getSingleton().m_myHero.skill3 = skillid;
	HttpClient::getSingleton().chgSkill3(skillid);

	return 1;
}

int SkillMgr::upgradeSkill(int& need, int skillId, int type)
{
    int skillIndex = (skillId/100 % 10000) - 1;
	if(skillIndex < 0 || skillIndex > SkillMgr::getSingleton().m_skillNums)
		return 0;

	if(HeroMgr::getSingleton().m_myHero.bskill[skillIndex] >= 10)
		return 0;
    
    const SkillInfo* sinfo = SkillMgr::getSingleton().getNode(skillId);
    if(sinfo == NULL)
        return 0;
    
	int gem = SkillMgr::getSingleton().getSkillTrainGem(skillId);
    int exp = SkillMgr::getSingleton().getSkillTrainExp(skillId);
    
	if(type == 2)
	{
		gem = SkillMgr::getSingleton().getSkillTotalTrainGem(skillId);
        int curexp = HeroMgr::getSingleton().m_myHero.bskillexp[skillIndex];
        exp = sinfo->maxExp - curexp;
        if(exp <= 0) exp = 0;
	}
    
//    if(HeroMgr::getSingleton().m_lbnums == 0)
    if(type == 2 || HeroMgr::getSingleton().m_isFreeTrain == 0)
    {
        int cost = gem;
        if(cost > LogicInfo::getSingleton().getMoney(MONEY_GEM))
        {
            need = cost - LogicInfo::getSingleton().getMoney(MONEY_GEM);
            
            MessageUI::show(MSG_NOGEM, NULL, NULL);
            
            return MSG_NOGEM;
        }
        
        LogicInfo::getSingleton().chgGem(-cost);
    }
    else
    {
        HeroMgr::getSingleton().m_isFreeTrain = 0;
    }

	HeroMgr::getSingleton().m_myHero.bskillexp[skillIndex] += exp;
	if(HeroMgr::getSingleton().m_myHero.bskillexp[skillIndex] >= sinfo->maxExp)
	{
		HeroMgr::getSingleton().m_myHero.bskillexp[skillIndex] -= sinfo->maxExp;
		HeroMgr::getSingleton().m_myHero.bskill[skillIndex]++;
	}

	HttpClient::getSingleton().upgradeSkill(skillIndex, type);

	return exp;
}

void SkillMgr::onIdle(int ot)
{
	for(_SkillList::iterator it = m_lstSkill.begin(); it != m_lstSkill.end(); )
	{
		it->time -= ot;

		if(it->time <= 0)
		{
			switch(it->id)
			{
			case 10001:
				useSkill_10001(it->hero, it->sinfo);
				break;
			case 10002:
				useSkill_10002(it->hero, it->sinfo);
				break;
			case 10003:
				useSkill_10003(it->hero, it->sinfo);
				break;
			case 10004:
				useSkill_10004(it->hero, it->sinfo);
				break;
			case 10005:
				useSkill_10005(it->hero, it->sinfo);
				break;
			case 10006:
				useSkill_10006(it->hero, it->sinfo);
				break;
			case 10007:
				useSkill_10007(it->hero, it->sinfo);
				break;
			case 10008:
				useSkill_10008(it->hero, it->sinfo);
				break;
			case 10009:
				useSkill_10009(it->hero, it->sinfo);
				break;
			case 10010:
				useSkill_10010(it->hero, it->sinfo);
				break;
			case 10011:
				useSkill_10011(it->hero, it->sinfo);
				break;
			case 10012:
				useSkill_10012(it->hero, it->sinfo);
				break;
			}

			it = m_lstSkill.erase(it);
		}
		else
		{
			++it;
		}
	}
}

//! 释放技能
void SkillMgr::useSkill(Person* hero, int id)
{
	if(hero == NULL)
		return ;

	const SkillInfo* sinfo = SkillMgr::getSingleton().getNode(id);

	if(sinfo == NULL)
		return;

	LogicInfo::getSingleton().onAddCtrl_UseSkill(hero->m_indexPerson, id);

	if(hero->m_iAct != PERSON_ACT_USESKILL)
		hero->chgState(PERSON_ACT_USESKILL);

	if(sinfo->att_name.size() != 0)
	{
		int layer = sinfo->att_layer;

        char aniname[64];
        
        sprintf(aniname, "skill%02d_attack", (id/100)%10);
		if(sinfo->att_type == 2)
		{
			int dir = hero->countDirIndex(hero->m_curDir);
			AniObj* ani = AniObjMgr::getSingleton().newAniObj(hero->m_pSkillEffect[layer], aniname, dir, true, false, -1, 0, false, NULL);
			ani->start();
		}
		else if(sinfo->att_type == 0)
		{
			AniObj* ani = AniObjMgr::getSingleton().newAniObj(hero->m_pSkillEffect[layer], aniname, 0, true, false, -1, 0, false, NULL);
			ani->start();
		}
	}

	id /= 100;

	_skillNode node;

	node.hero = hero;
	node.id = id;
	node.sinfo = sinfo;
	node.time = sinfo->delay;

	m_lstSkill.push_back(node);

	//switch(id)
	//{
	//case 10001:
	//	useSkill_10001(hero, sinfo, level);
	//	break;
	//case 10002:
	//	useSkill_10002(hero, sinfo, level);
	//	break;
	//case 10003:
	//	useSkill_10003(hero, sinfo, level);
	//	break;
	//case 10004:
	//	useSkill_10004(hero, sinfo, level);
	//	break;
	//case 10005:
	//	useSkill_10005(hero, sinfo, level);
	//	break;
	//case 1000006:
	//	useSkill_10006(hero, sinfo, level);
	//	break;
	//}
}

//! 释放重击
void SkillMgr::useSkill_10006(Person* hero, const SkillInfo* sinfo)
{
	if(hero->canAttack())
	{
		BaseObj* dest = hero->m_pDestTarget;

		if(sinfo->eff_name.size() != 0)
		{
			int layer = sinfo->eff_layer;

			if(sinfo->eff_type == 0)
			{
				AniObj* ani = AniObjMgr::getSingleton().newAniObj(dest->m_pSkillEffect[layer], "skill06_hit", 0, true, false, -1, 0, false, NULL);
				ani->start();
			}
		}

		dest->chgHP(-sinfo->bvalue1, true);
	}
}

//! 释放激昂
void SkillMgr::useSkill_10002(Person* hero, const SkillInfo* sinfo)
{
	int value = sinfo->bvalue1;
	int time = sinfo->btime1;

	PlayerScene::_PersonList& lstperson = PlayerScene::getSingleton().m_lstPerson[hero->m_curCamp];

	for(PlayerScene::_PersonList::iterator it = lstperson.begin(); it != lstperson.end(); ++it)
	{
		if((*it)->m_curCamp != hero->m_curCamp)
			continue ;

		 if((*it)->isEmptyHP() || (*it)->isStatus(PERSON_STATUS_RELEASE))
			 continue ;

		float cdis = ((*it)->getLogicX() - hero->getLogicX()) * ((*it)->getLogicX() - hero->getLogicX()) +
			((*it)->getLogicY() - hero->getLogicY()) * ((*it)->getLogicY() - hero->getLogicY());

		cdis = sqrt(cdis);

		if(cdis <= 10.0f)
		{
			Person* person = (*it);

			if(sinfo->eff_name.size() != 0)
			{
				int layer = sinfo->eff_layer;

				if(sinfo->eff_type == 0)
				{
					AniObj* ani = AniObjMgr::getSingleton().newAniObj(person->m_pSkillEffect[layer], "skill02_hit", 0, true, false, -1, 0, false, NULL);
					ani->start();
				}
				else if(sinfo->eff_type == 1)
				{
					AniObj* ani = AniObjMgr::getSingleton().newAniObj(person->m_pSkillEffect[layer], "skill02_hit", 0, true, true, time / 1000.0f , 0, false, NULL);
					ani->start();
				}
			}

			person->addBuff(BUFF_ATTSPEED_P, value, time);
			person->addFlyingFont2(sinfo->name.c_str(), 50, ccc3(0, 0, 255));
		}
	}
}

//! 释放守护
void SkillMgr::useSkill_10003(Person* hero, const SkillInfo* sinfo)
{
	int value = sinfo->bvalue1;
	int time = sinfo->btime1;

	if(sinfo->eff_name.size() != 0)
	{
		int layer = sinfo->eff_layer;

		if(sinfo->eff_type == 0)
		{
			AniObj* ani = AniObjMgr::getSingleton().newAniObj(hero->m_pSkillEffect[layer], "skill03_hit", 0, true, false, -1, 0, false, NULL);
			ani->start();
		}
		else if(sinfo->eff_type == 1)
		{
			AniObj* ani = AniObjMgr::getSingleton().newAniObj(hero->m_pSkillEffect[layer], "skill03_hit", 0, true, true, time / 1000.0f , 0, false, NULL);
			ani->start();
		}
	}

	hero->addBuff(BUFF_DEFENCE, value, time);
}

//! 释放斗志
void SkillMgr::useSkill_10004(Person* hero, const SkillInfo* sinfo)
{
	int value = sinfo->bvalue1;
	int time = sinfo->btime1;

	PlayerScene::_PersonList& lstperson = PlayerScene::getSingleton().m_lstPerson[hero->m_curCamp];

	for(PlayerScene::_PersonList::iterator it = lstperson.begin(); it != lstperson.end(); ++it)
	{
		if((*it)->m_curCamp != hero->m_curCamp)
			continue ;

		if((*it)->isEmptyHP() || (*it)->isStatus(PERSON_STATUS_RELEASE))
			 continue ;

		float cdis = ((*it)->getLogicX() - hero->getLogicX()) * ((*it)->getLogicX() - hero->getLogicX()) +
			((*it)->getLogicY() - hero->getLogicY()) * ((*it)->getLogicY() - hero->getLogicY());

		cdis = sqrt(cdis);

		if(cdis <= 10.0f)
		{
			Person* person = (*it);

			if(sinfo->eff_name.size() != 0)
			{
				int layer = sinfo->eff_layer;

				if(sinfo->eff_type == 0)
				{
					AniObj* ani = AniObjMgr::getSingleton().newAniObj(person->m_pSkillEffect[layer], "skill04_hit", 0, true, false, -1, 0, false, NULL);
					ani->start();
				}
				else if(sinfo->eff_type == 1)
				{
					AniObj* ani = AniObjMgr::getSingleton().newAniObj(person->m_pSkillEffect[layer], "skill04_hit", 0, true, true, time / 1000.0f , 0, false, NULL);
					ani->start();
				}
			}

			person->addBuff(BUFF_DAMAGE, value, time);
			person->addFlyingFont2(sinfo->name.c_str(), 50, ccc3(0, 0, 255));
		}
	}
}

//! 释放强击
void SkillMgr::useSkill_10005(Person* hero, const SkillInfo* sinfo)
{
	int value = sinfo->bvalue1;
	int time = sinfo->btime1;

	PlayerScene::_PersonList& lstperson = PlayerScene::getSingleton().m_lstPerson[hero->m_curCamp];

	for(PlayerScene::_PersonList::iterator it = lstperson.begin(); it != lstperson.end(); ++it)
	{
		if((*it)->m_curCamp != hero->m_curCamp)
			continue ;

		if((*it)->isEmptyHP() || (*it)->isStatus(PERSON_STATUS_RELEASE))
			 continue ;

		float cdis = ((*it)->getLogicX() - hero->getLogicX()) * ((*it)->getLogicX() - hero->getLogicX()) +
			((*it)->getLogicY() - hero->getLogicY()) * ((*it)->getLogicY() - hero->getLogicY());

		cdis = sqrt(cdis);

		if(cdis <= 10.0f)
		{
			Person* person = (*it);

			if(sinfo->eff_name.size() != 0)
			{
				int layer = sinfo->eff_layer;

				if(sinfo->eff_type == 0)
				{
					AniObj* ani = AniObjMgr::getSingleton().newAniObj(person->m_pSkillEffect[layer], "skill05_hit", 0, true, false, -1, 0, false, NULL);
					ani->start();
				}
				else if(sinfo->eff_type == 1)
				{
					AniObj* ani = AniObjMgr::getSingleton().newAniObj(person->m_pSkillEffect[layer], "skill05_hit", 0, true, true, time / 1000.0f , 0, false, NULL);
					ani->start();
				}
			}

			person->addBuff(BUFF_SPEED, value, time);
			person->addFlyingFont2(sinfo->name.c_str(), 50, ccc3(0, 0, 255));
			//person->addBuff(BUFF_JUMP, 1, time);
		}
	}
}

//! 释放治疗
void SkillMgr::useSkill_10001(Person* hero, const SkillInfo* sinfo)
{
	int value = sinfo->bvalue1;
	int time = sinfo->btime1;

	PlayerScene::_PersonList& lstperson = PlayerScene::getSingleton().m_lstPerson[hero->m_curCamp];

	for(PlayerScene::_PersonList::iterator it = lstperson.begin(); it != lstperson.end(); ++it)
	{
		if((*it)->m_curCamp != hero->m_curCamp)
			continue ;

		if((*it)->isEmptyHP() || (*it)->isStatus(PERSON_STATUS_RELEASE))
			 continue ;

		float cdis = ((*it)->getLogicX() - hero->getLogicX()) * ((*it)->getLogicX() - hero->getLogicX()) +
			((*it)->getLogicY() - hero->getLogicY()) * ((*it)->getLogicY() - hero->getLogicY());

		cdis = sqrt(cdis);

		if(cdis <= 10.0f)
		{
			Person* person = (*it);

			if(sinfo->eff_name.size() != 0)
			{
				int layer = sinfo->eff_layer;
                
				if(sinfo->eff_type == 0)
				{
					AniObj* ani = AniObjMgr::getSingleton().newAniObj(person->m_pSkillEffect[layer], "skill01_hit", 0, true, false, -1, 0, false, NULL);
					ani->start();
				}
				else if(sinfo->eff_type == 1)
				{
					AniObj* ani = AniObjMgr::getSingleton().newAniObj(person->m_pSkillEffect[layer], "skill01_hit", 0, true, true, time / 1000.0f , 0, false, NULL);
					ani->start();
				}
			}

			person->addBuff(BUFF_LIFE, value, time);
			person->addFlyingFont2(sinfo->name.c_str(), 50, ccc3(0, 0, 255));
		}
	}
}

//! 释放拆除
void SkillMgr::useSkill_10007(Person* hero, const SkillInfo* sinfo)
{
	float value = sinfo->bvalue1;

	PlayerScene::_BuildingList& lstbuilding = PlayerScene::getSingleton().m_lstBuilding;

	for(PlayerScene::_BuildingList::iterator it = lstbuilding.begin(); it != lstbuilding.end(); ++it)
	{
		float cdis = ((*it)->getLogicX() - hero->getLogicX()) * ((*it)->getLogicX() - hero->getLogicX()) +
			((*it)->getLogicY() - hero->getLogicY()) * ((*it)->getLogicY() - hero->getLogicY());

		cdis = sqrt(cdis);

		if(cdis <= 10.0f)
		{
			Building* building = (*it);

			if(sinfo->eff_name.size() != 0)
			{
				int layer = sinfo->eff_layer;

				if(sinfo->eff_type == 0)
				{
					AniObj* ani = AniObjMgr::getSingleton().newAniObj(building->m_pSkillEffect[layer], "skill07_hit", 0, true, false, -1, 0, false, NULL);
					
					if(ani != NULL)
						ani->start();
				}
			}

			building->chgHP(-value, true);
		}
	}
}

//! 释放挑衅
void SkillMgr::useSkill_10008(Person* hero, const SkillInfo* sinfo)
{
	float value = sinfo->bvalue1;
	int time = sinfo->btime1;

	if(sinfo->eff_name.size() != 0)
	{
		int layer = sinfo->eff_layer;

		if(sinfo->eff_type == 0)
		{
			AniObj* ani = AniObjMgr::getSingleton().newAniObj(hero->m_pSkillEffect[layer], "skill08_hit", 0, true, false, -1, 0, false, NULL);
			ani->start();
		}
		else if(sinfo->eff_type == 1)
		{
			AniObj* ani = AniObjMgr::getSingleton().newAniObj(hero->m_pSkillEffect[layer], "skill08_hit", 0, true, true, time / 1000.0f , 0, false, NULL);
			ani->start();
		}
	}

	hero->addBuff(BUFF_DAMAGE, value, time);
	hero->addBuff(BUFF_ATTACKTYPE, 2, time);
	hero->addFlyingFont2(sinfo->name.c_str(), 50, ccc3(0, 0, 255));
}

//! 释放爆发
void SkillMgr::useSkill_10009(Person* hero, const SkillInfo* sinfo)
{
	int value1 = sinfo->bvalue1;
	int time = sinfo->btime1;

	if(sinfo->eff_name.size() != 0)
	{
		int layer = sinfo->eff_layer;

		if(sinfo->eff_type == 0)
		{
			AniObj* ani = AniObjMgr::getSingleton().newAniObj(hero->m_pSkillEffect[layer], "skill09_hit", 0, true, false, -1, 0, false, NULL);
			
			if(ani != NULL)
				ani->start();
		}
		else if(sinfo->eff_type == 1)
		{
			AniObj* ani = AniObjMgr::getSingleton().newAniObj(hero->m_pSkillEffect[layer], "skill09_hit", 0, true, true, time / 1000.0f , 0, false, NULL);
			
			if(ani != NULL)
				ani->start();
		}
	}

	hero->addBuff(BUFF_MAXLIFE, value1, time);
	hero->chgHP(value1, true);
}

//! 释放远视
void SkillMgr::useSkill_10010(Person* hero, const SkillInfo* sinfo)
{
	int value = sinfo->bvalue1;
	int time = sinfo->btime1;

	PlayerScene::_PersonList& lstperson = PlayerScene::getSingleton().m_lstPerson[hero->m_curCamp];

	for(PlayerScene::_PersonList::iterator it = lstperson.begin(); it != lstperson.end(); ++it)
	{
		if((*it)->m_curCamp != hero->m_curCamp)
			continue ;

		if((*it)->isEmptyHP() || (*it)->isStatus(PERSON_STATUS_RELEASE))
			 continue ;

		float cdis = ((*it)->getLogicX() - hero->getLogicX()) * ((*it)->getLogicX() - hero->getLogicX()) +
			((*it)->getLogicY() - hero->getLogicY()) * ((*it)->getLogicY() - hero->getLogicY());

		cdis = sqrt(cdis);

		if(cdis <= 14.0f)
		{
			Person* person = (*it);

			if(sinfo->eff_name.size() != 0)
			{
				int layer = sinfo->eff_layer;

				if(sinfo->eff_type == 0)
				{
					AniObj* ani = AniObjMgr::getSingleton().newAniObj(person->m_pSkillEffect[layer], "skill10_hit", 0, true, false, -1, 0, false, NULL);
					ani->start();
				}
				else if(sinfo->eff_type == 1)
				{
					AniObj* ani = AniObjMgr::getSingleton().newAniObj(person->m_pSkillEffect[layer], "skill10_hit", 0, true, true, time / 1000.0f , 0, false, NULL);
					ani->start();
				}
			}

			if(person->m_isShootAttack)
			{
				person->addBuff(BUFF_RANGE, value, time);
				person->addFlyingFont2(sinfo->name.c_str(), 50, ccc3(0, 0, 255));
			}
		}
	}
}

//! 释放闪电
void SkillMgr::useSkill_10011(Person* hero, const SkillInfo* sinfo)
{
	int value = -sinfo->bvalue1;
	int time = sinfo->btime1;

	if(hero->m_pDestTarget != NULL && !hero->m_pDestTarget->isEmptyHP())
	{
		BaseObj* dest = hero->m_pDestTarget;

		if(sinfo->eff_name.size() != 0)
		{
			int layer = sinfo->eff_layer;

			AniObj* ani = AniObjMgr::getSingleton().newAniObj(dest->m_pSkillEffect[layer], "skill11_hit", 0, true, true, time / 1000.0f, 0, false, NULL);
			
			if(ani != NULL)
				ani->start();
		}

		dest->addBuff(BUFF_LIFE, value, time);
	}
}

//! 释放隐身
void SkillMgr::useSkill_10012(Person* hero, const SkillInfo* sinfo)
{
	int time = sinfo->btime1;

	PlayerScene::_PersonList& lstperson = PlayerScene::getSingleton().m_lstPerson[hero->m_curCamp];

	for(PlayerScene::_PersonList::iterator it = lstperson.begin(); it != lstperson.end(); ++it)
	{
		if((*it)->m_curCamp != hero->m_curCamp)
			continue ;

		if((*it)->isEmptyHP() || (*it)->isStatus(PERSON_STATUS_RELEASE))
			 continue ;

		float cdis = ((*it)->getLogicX() - hero->getLogicX()) * ((*it)->getLogicX() - hero->getLogicX()) +
			((*it)->getLogicY() - hero->getLogicY()) * ((*it)->getLogicY() - hero->getLogicY());

		cdis = sqrt(cdis);

		if(cdis <= 10.0f)
		{
			Person* person = (*it);

			if(sinfo->eff_name.size() != 0)
			{
				int layer = sinfo->eff_layer;

				if(sinfo->eff_type == 0)
				{
					AniObj* ani = AniObjMgr::getSingleton().newAniObj(person->m_pSkillEffect[layer], "skill12_hit", 0, true, false, -1, 0, false, NULL);
					
					if(ani != NULL)
						ani->start();
				}
				else if(sinfo->eff_type == 1)
				{
					AniObj* ani = AniObjMgr::getSingleton().newAniObj(person->m_pSkillEffect[layer],"skill12_hit", 0, true, true, time / 1000.0f, 0, false, NULL);
					
					if(ani != NULL)
						ani->start();
				}
			}

			person->addBuff(BUFF_HIDDEN, 1, time);
			person->addFlyingFont2(sinfo->name.c_str(), 50, ccc3(0, 0, 255));
		}
	}
}

//! 添加天赋
void SkillMgr::addTalent(Person* hero, int id)
{
	const SkillInfo* sinfo = SkillMgr::getSingleton().getNode(id);

	if(sinfo == NULL)
		return ;

	hero->addTalent(sinfo->bvalue1, sinfo->bvalue2, sinfo->btime1);
}

bool SkillMgr::canLevelUp(int skillid)
{
	const SkillInfo* sinfo = SkillMgr::getSingleton().getNode(skillid);
	if(sinfo == NULL)
		return false;

	if(sinfo->needlevel >= HeroMgr::getSingleton().getHeroLevel())
		return true;

	return false;
}

//! 获取每次训练的经验
int SkillMgr::getSkillTrainExp(int skillid)
{
	const SkillInfo* sinfo = SkillMgr::getSingleton().getNode(skillid);
	if(sinfo == NULL)
		return 0;
    
	return sinfo->trainExp;
}

//! 获取每次训练的消耗
int SkillMgr::getSkillTrainGem(int skillid)
{
	const SkillInfo* sinfo = SkillMgr::getSingleton().getNode(skillid);
	if(sinfo == NULL)
		return 0;
    
	return sinfo->trainGem;
}

//! 获取训练到升级的消耗
int SkillMgr::getSkillTotalTrainGem(int skillid)
{
	const SkillInfo* sinfo = SkillMgr::getSingleton().getNode(skillid);
	if(sinfo == NULL)
		return 0;

	int index = ((skillid / 100) % 10000) - 1;
	int curexp = HeroMgr::getSingleton().m_myHero.bskillexp[index];
	int lastexp = sinfo->maxExp - curexp;
	if(lastexp <= 0)
		return 0;

	return lastexp * sinfo->trainGem / sinfo->trainExp;
}

NS_COC_END
