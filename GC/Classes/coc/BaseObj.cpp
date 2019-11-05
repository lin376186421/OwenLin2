#include "BaseObj.h"
#include "PlayerScene.h"
#include "RandMgr.h"

#include "LogicInfo.h"
#include "AniObjMgr.h"
#include "AniObj_Normal.h"
#include "AniObj_FlyingFont2.h"

NS_COC_BEGIN

BaseObj::BaseObj(int objType)
	: m_lx(0)
	, m_ly(0)
	, m_objType(objType)
	, m_isVisible(true)
	, m_iSecondTime(0)
    , m_beAttackTargetNums(0)
	, m_pLabelName(NULL)
{
	memset(m_Buffs, 0, sizeof(BuffNode) * BUFF_MAXNUM);
	memset(m_Talents, 0, sizeof(TalentNode) * BUFF_MAXNUM);

	m_yTop = TILE_H * 2;
}

BaseObj::~BaseObj()
{
}

void BaseObj::setLogicPos(int lx, int ly)
{
	m_lx = lx;
	m_ly = ly;

	if(m_lw >= 2)
	{
		m_ptCenter.x = lx - m_lw / 2;
		m_ptCenter.y = ly - m_lw / 2;
	}
	else
	{
		m_ptCenter.x = lx;
		m_ptCenter.y = ly;
	}
}

void BaseObj::chgHP(int off, bool flyfont, bool isReal)
{
	int roff = fixHP(off);
	m_hp += roff;
    
    if (m_hp < 0) {
        m_hp = 0;
    }

	if(flyfont)
	{
		char tmp[128];

		if(roff > 0)
		{
			sprintf(tmp, "+%d", roff);
			addFlyingFont2(tmp, 50, ccc3(0, 255, 0));
		}
		else if(roff < 0)
		{
			sprintf(tmp, "%d", roff);
			addFlyingFont2(tmp, 50, ccc3(255, 0, 0));
		}
	}
}

CCPoint BaseObj::getAttackPos() const
{
	CCPoint pt = PlayerScene::getSingleton().countScenePos(getCenterPt().x, getCenterPt().y);

	pt.y += TILE_H;

	if(getLogicWidth() % 2 == 0)
		pt.x += TILE_W;
	else
		pt.x += TILE_W / 2;

	return pt;
}

void BaseObj::addProjectile2(Projectile2& proj)
{
	proj.retain();

	m_lstProj.push_back(&proj);
}

void BaseObj::releaseProjectile2()
{
	for(std::list<Projectile2*>::iterator it = m_lstProj.begin(); it != m_lstProj.end(); ++it)
	{
		Projectile2* pProj = *it;

		pProj->m_isRelease = true;
		pProj->release();
	}

	m_lstProj.clear();
}

void BaseObj::onProcProjectile2(int ot)
{
	for(std::list<Projectile2*>::iterator it = m_lstProj.begin(); it != m_lstProj.end();)
	{
		Projectile2* pProj = *it;

		if(pProj->isFinished())
		{
			pProj->m_isRelease = true;
			pProj->release();

			it = m_lstProj.erase(it);
		}
		else
			++it;
	}

	m_lstProj.clear();
}

void BaseObj::setVisible(bool bVisible)
{
	for(int i = 0; i < 3; ++i)
	{
		if(m_pSkillEffect[i] != NULL)
			m_pSkillEffect[i]->setVisible(bVisible);
	}
    
    m_isVisible = bVisible;
}

void BaseObj::onProc(int ot)
{
    if (LogicInfo::getSingleton().isAttackMode()) {
        
        for(int i = 0; i < BUFF_MAXNUM; ++i)
        {
            if(m_Buffs[i].time > 0)
            {
                m_Buffs[i].time -= ot;
                
                if(m_Buffs[i].time <= 0)
                {
                    m_Buffs[i].time = 0;
                    m_Buffs[i].value = 0;
                    
                    if(i == BUFF_MAXLIFE)
                        onChgMaxHP();
                }
            }
        }
        
        m_iSecondTime += ot;
        
        while(m_iSecondTime >= 1000)
        {
            m_iSecondTime -= 1000;
            
            if(m_Buffs[BUFF_LIFE].value != 0 || m_Talents[BUFF_LIFE].value)
            {
                chgHP(m_Buffs[BUFF_LIFE].value + m_Talents[BUFF_LIFE].value, true);
            }
        }
    }
}

//! Buff相关
//! 调整伤害
int BaseObj::fixDamage(int damage)
{
    if(damage < 0)
		return damage;
    
	damage += m_Buffs[BUFF_DAMAGE].value + m_Talents[BUFF_DAMAGE].value;
	damage *= 1.0f + (m_Buffs[BUFF_DAMAGE_P].value + m_Talents[BUFF_DAMAGE_P].value) / 10000.0f;

	//! 暴击判断
	{
		if(m_Buffs[BUFF_CRIT].value != 0)
			return damage * (m_Buffs[BUFF_CRIT].value / 10000.0f);

		if(m_Talents[BUFF_CRIT].prob != 0)
		{
			if(RandMgr::getSingleton().randWar(10000) < m_Talents[BUFF_CRIT].prob)
				return damage * (m_Talents[BUFF_CRIT].value / 10000.0f);
		}
	}

	//damage = damage * 0.1f;

	return damage;
}

//! 调整速度
int BaseObj::fixSpeed(int speed)
{
	float ospeed = speed / SCALE_PERSONSPEED;

	ospeed += m_Buffs[BUFF_SPEED].value + m_Talents[BUFF_SPEED].value;
	ospeed *= 1.0f + (m_Buffs[BUFF_SPEED_P].value + m_Talents[BUFF_SPEED_P].value) / 10000.0f;

	speed = SCALE_PERSONSPEED * ospeed;

	if(speed < 1)
		speed = 1;

	return speed;
}

//! 调整攻速
int BaseObj::fixAttspeed(int attspeed)
{
	attspeed += m_Buffs[BUFF_ATTSPEED].value + m_Talents[BUFF_ATTSPEED].value;
	attspeed *= 1.0f + (m_Buffs[BUFF_ATTSPEED_P].value + m_Talents[BUFF_ATTSPEED_P].value) / 10000.0f;

	if(attspeed < 10)
		attspeed = 10;

	return attspeed;
}

//! 调整防御（减伤）
int BaseObj::fixHP(int off)
{
	if(off >= 0)
		return off;

	//! 无敌判断
	{
		if(m_Buffs[BUFF_SUPER].value != 0)
		{
			addFlyingFont2(ViewString::getSingleton().getValue("buff_super").c_str(), 50, ccc3(0, 0, 255));
			return 0;
		}

		if(m_Talents[BUFF_SUPER].prob != 0)
		{
			if(RandMgr::getSingleton().randWar(10000) < m_Talents[BUFF_SUPER].prob)
			{
				addFlyingFont2(ViewString::getSingleton().getValue("buff_super").c_str(), 50, ccc3(0, 0, 255));
				return 0;
			}
		}
	}

	off = -off;

	off -= m_Buffs[BUFF_DEFENCE].value + m_Talents[BUFF_DEFENCE].value;
	off *= 1.0f - (m_Buffs[BUFF_DEFENCE_P].value + m_Talents[BUFF_DEFENCE_P].value) / 10000.0f;

	if(off < 0)
		off = 0;

	int avalue = 0;

	if(m_Buffs[BUFF_AGAINST].value > 0)
	{
		if(off >= m_Buffs[BUFF_AGAINST].value)
		{
			avalue += m_Buffs[BUFF_AGAINST].value;

			off -= m_Buffs[BUFF_AGAINST].value;
			m_Buffs[BUFF_AGAINST].value = 0;
		}
		else
		{
			avalue += off;

			m_Buffs[BUFF_AGAINST].value -= off;
			off = 0;
		}
	}

	if(m_Talents[BUFF_AGAINST].value > 0)
	{
		if(off >= m_Talents[BUFF_AGAINST].value)
		{
			avalue += m_Talents[BUFF_AGAINST].value;

			off -= m_Talents[BUFF_AGAINST].value;
			m_Talents[BUFF_AGAINST].value = 0;
		}
		else
		{
			avalue += off;

			m_Talents[BUFF_AGAINST].value -= off;
			off = 0;
		}
	}

	if(off == 0 && avalue != 0)
	{
		char tmp[128];
		sprintf(tmp, "%s %d", ViewString::getSingleton().getValue("buff_against").c_str(), avalue);
		addFlyingFont2(tmp, 50, ccc3(0, 0, 255));
	}

	return -off;
}

//! 改变生命上限
void BaseObj::onChgMaxHP()
{
	int cmax = getMaxHP() + m_Buffs[BUFF_MAXLIFE].value + m_Talents[BUFF_MAXLIFE].value;
	cmax *= 1.0f + (m_Buffs[BUFF_MAXLIFE_P].value + m_Talents[BUFF_MAXLIFE_P].value) / 10000.0f;

	if(m_hp > cmax)
		m_hp = cmax;
}

//! 调整生命上限
int BaseObj::fixMaxHP()
{
	int cmax = getMaxHP() + m_Buffs[BUFF_MAXLIFE].value + m_Talents[BUFF_MAXLIFE].value;
	cmax *= 1.0f + (m_Buffs[BUFF_MAXLIFE_P].value + m_Talents[BUFF_MAXLIFE_P].value) / 10000.0f;

	return cmax;
}

void BaseObj::addBuff(int type, int value, int time)
{
	//if(m_Buffs[type].value > value)
	//	return ;

	if(m_Buffs[type].value == value)
	{
		if(m_Buffs[type].time >= time)
			return ;
	}

	m_Buffs[type].value = value;
	m_Buffs[type].time = time;

	if(type == BUFF_MAXLIFE || type == BUFF_MAXLIFE_P)
		onChgMaxHP();
}

void BaseObj::clearAllBuff()
{
	for(int i = 0; i < BUFF_MAXNUM; ++i)
	{
		m_Buffs[i].time = 0;
		m_Buffs[i].value = 0;
	}
}

//! 添加天赋
void BaseObj::addTalent(int type, int value, int prob)
{
	m_Talents[type].value = value;
	m_Talents[type].prob = prob;

	if(type == BUFF_MAXLIFE || type == BUFF_MAXLIFE_P)
	{
		onChgMaxHP();
		setHP(fixMaxHP());
	}
}

//! 减少天赋
void BaseObj::removeTalent(int type)
{
	m_Talents[type].value = 0;

	if(type == BUFF_MAXLIFE || type == BUFF_MAXLIFE_P)
	{
		onChgMaxHP();
		setHP(fixMaxHP());
	}
}

//! 增加受击动画
void BaseObj::addEffAni(const char* name)
{
	AniObj_Normal* ani = (AniObj_Normal*)AniObjMgr::getSingleton().newAniObj(m_pSkillEffect[1], name, 0, true, false, -1, 0, false, NULL);
	ani->start();

	//! 根据大小随机
	int w = getWidth() / 4;
	int h = getHeight() / 4;

	int bx = 0;
	int by = 0;

	if(w > 0)
	{
		bx = rand() % w - w / 2;
	}

	if(h > 0)
	{
		by = rand() % h - h / 2;
	}

	ani->setPosition(ccp(bx, by));
}

void BaseObj::addFlyingFont2(const char* str, int size, ccColor3B c)
{
	AniObj_FlyingFont2* pAni = (AniObj_FlyingFont2*)AniObjMgr::getSingleton().newAniObj(m_pSkillEffect[2], "warfont", 0, true, false, 1.0f, 0, false, NULL);
	if(pAni != NULL)
	{
		pAni->setPosition(ccp(0, m_yTop));

		pAni->setString(str);
		pAni->setFontSize(size);
		pAni->setFontColor(c);

		pAni->start();
	}
}

void BaseObj::releaseFromScene()
{
	onFreeRes();
}

NS_COC_END