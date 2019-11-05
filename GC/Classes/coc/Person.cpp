#include "PErson.h"
#include "PlayerScene.h"
#include "ImgMgr.h"
#include "Building.h"
#include "SimpleAudioEngine.h"
#include "SoundMgr.h"
#include "LogicInfo.h"
#include "PersonMgr.h"
#include "SkillMgr.h"
#include "CUIExt.h"
#include "SceneResMgr.h"
#include "HeroMgr.h"
#include "ResConfig.h"
#include "AniObjMgr.h"
#include "AniObj_Normal.h"

NS_COC_BEGIN

const int PERSON_ATTACK_DEST_NUMS = 2;

const int PERSON_WALK_STEP = 5;
const CCPoint PERSON_WALK_PATH[PERSON_WALK_STEP] = {ccp(1, -1), ccp(-1, 1), ccp(1, 1), ccp(-1, -1), ccp(0, 0)};

int personWalkIndex = 0;

Person::Person(SoldierInfo info, int camp)
	: BaseObj(OBJTYPE_PERSON)
	, m_curDir(-1)
    , m_curDirIndex(-1)
    , m_curAct(-1)
	, m_pCurAction(NULL)
    , m_iAct(-1)
	, m_info(info)
	, m_bFlip(false)
	, m_lastPathTime(0)
	, m_pProHP(NULL)
	, m_pDestTarget(NULL)
    , m_pWallTarget(NULL)
	, m_pTroophousing(NULL)
	, m_pShadow(NULL)
	, m_pIcon(NULL)
	, m_pMFTime(NULL)
    , m_pProTime(NULL)
	, m_sleepTime(-1)
    , m_walkTime(0)
    , m_walkStep(0)
    , m_nextWalkStep(0)
	, m_beginAttackTime(0)
	, m_damageOnTower(0)
	, m_bAlreadyShoot(true)
    , m_personType(PERSON_TYPE_FREE)
    , m_personState(0)
    , m_defendRange(0)
    , m_status(0)
	, m_lastResearch(0)
	, m_curCamp(camp)
	, m_lastJumpTime(-1)
	, m_iBeforeState(-1)
	, m_isShadowFlying(false)
	, m_indexPerson(0)
    , m_findAttackPosNums(0)
{
    m_personid = info.soldierid;
    
	m_ptOff.x = 0;
	m_ptOff.y = 0;

	m_curPtOff.x = -1;
	m_curPtOff.y = -1;
	m_curPtEnd.x = -1;
	m_curPtEnd.y = -1;
    
    m_ptDestAttack = ccp(-1, -1);
    
	setHP((int)getSoldierInfo().hpex);

	m_speed = SCALE_PERSONSPEED * getSoldierInfo().speedex;
	m_attackSpeed = getSoldierInfo().attackSpeedex;
    
    m_lastResearch = PERSON_RESEARCHTIME[m_curCamp];
    
    m_ptSleep.x = -1;
    m_ptSleep.y = -1;

	for(int i = 0; i < 5; ++i)
	{
		for(int j = 0; j < 5; ++j)
		{
			m_pAnimate[i][j] = NULL;
			m_pFirstFrame[i][j] = NULL;
		}
	}
    
    m_attackTarget = info.attackTarget;
}

Person::~Person()
{
//	release();
}

bool Person::isShootAttack() const
{
	int pid = m_personid / 100;
	if(pid == PERSON_ARCHER || pid == PERSON_BALLOON || pid == PERSON_WIZARD || pid == PERSON_DRAGON)
		return true;
    
	return false;
}

void Person::init(int lx, int ly, int act, int skill0)
{
	if(isShootAttack())
		m_isShootAttack = true;
	else
		m_isShootAttack = false;
    
	char strname[128];

    for(int dir = 0; dir < 5; ++dir)
    {
        if(dir >= getSoldierInfo().dirmode)
            break;

        for(int act = 0; act < 5; ++act)
        {
            if(getSoldierInfo().frames[act] > 0)
            {
                CCAnimation* pAni = CCAnimation::create();

                for(int frame = 0; frame < getSoldierInfo().frames[act]; ++frame)
                {
                    sprintf(strname, "%s01_%02d_%d%02d", getSoldierInfo().resname.c_str(), (act+1), dir, frame+1);
                    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(strname);
                    CCAssert(pFrame != NULL, "person no frame");
                    pAni->addSpriteFrame(pFrame);

                    if(act == PERSON_ACTION_WALK)
                    {
                        if(frame == 0)
                        {
                            m_pFirstFrame[dir][3] = pFrame;
                            m_pFirstFrame[dir][3]->retain();
                        }
                    }

                    if(frame == 0)
                    {
                        m_pFirstFrame[dir][act] = pFrame;
                        m_pFirstFrame[dir][act]->retain();
                    }
                }

                if(getSoldierInfo().frames[act] > 1)
                {
                    pAni->setDelayPerUnit(getSoldierInfo().framesTime[act] / 1000.0f);
                    m_pAnimate[dir][act] = CCAnimate::create(pAni);
                    m_pAnimate[dir][act]->retain();
                }
                else
                    pAni->release();
            }
        }
    }

	sprintf(strname, "%s01_01_001", getSoldierInfo().resname.c_str());
	m_sprPerson = ImgMgr::getSingleton().loadSprite(strname);
	m_sprPerson->retain();
	m_sprPerson->setAnchorPoint(ccp(0.5f, 0.5f));
 
	PlayerScene::getSingleton().m_pRoot->addChild(m_sprPerson, 10);

	m_nodPersonBask = CCNode::create();
	m_nodPersonBask->retain();
	m_nodPersonBask->setAnchorPoint(ccp(0.5f, 0.5f));
	PlayerScene::getSingleton().m_pRoot->addChild(m_nodPersonBask, 9);

	{
		m_pSkillEffect[0] = CCNode::create();
		m_pSkillEffect[0]->retain();
		m_pSkillEffect[0]->setAnchorPoint(ccp(0.5f, 0.5f));
		PlayerScene::getSingleton().m_pRoot->addChild(m_pSkillEffect[0], SCENE_Z_SKILLEFF_GROUND);

		m_pSkillEffect[1] = CCNode::create();
		m_pSkillEffect[1]->retain();
		m_pSkillEffect[1]->setAnchorPoint(ccp(0.5f, 0.5f));
		PlayerScene::getSingleton().m_pRoot->addChild(m_pSkillEffect[1], 11);

		m_pSkillEffect[2] = CCNode::create();
		m_pSkillEffect[2]->retain();
		m_pSkillEffect[2]->setAnchorPoint(ccp(0.5f, 0.5f));
		PlayerScene::getSingleton().m_pRoot->addChild(m_pSkillEffect[2], SCENE_Z_SKILLEFF_TOP);
	}

    if (LogicInfo::getSingleton().isAttackMode() || LogicInfo::getSingleton().isHero(m_personid)) {
        CCSprite* pFrameBg = ImgMgr::getSingleton().loadSprite("ui_img_pro_bk_0");
        if (m_personid  < 30000) {
            CCSprite* pFrame = ImgMgr::getSingleton().loadSprite("ui_img_pro_org_0");
            m_pProHP = CProgress::create(pFrame, pFrameBg);
            m_pProHP->setAnchorPoint(ccp(0.5, 0));
        }
        else
        {
            CCSprite* pFrame ;
            if(LogicInfo::getSingleton().m_curState == LOGICSTATE_NORMAL || m_curCamp == CAMP_ATTACK)//自己英雄
                pFrame = ImgMgr::getSingleton().loadSprite("ui_img_pro_org_6");
            else
                pFrame = ImgMgr::getSingleton().loadSprite("ui_img_pro_org_2");
            m_pProHP = CProgress::create(pFrame, pFrameBg);
            m_pProHP->setAnchorPoint(ccp(0.5, 0));
        }
        
        m_pProHP->retain();
        PlayerScene::getSingleton().m_pRoot->addChild(m_pProHP, SCENE_Z_BUILDINGUI);
        
        if(isHero() || (LogicInfo::getSingleton().isAttackMode() && m_personType != PERSON_TYPE_TOWER && getHP() < fixMaxHP()))
        {
            m_pProHP->setVisible(true);
            m_pProHP->changeProgress((float)getHP() / fixMaxHP());
        }
        else
            m_pProHP->setVisible(false);
    }
	
	{
		m_pShadow = ImgMgr::getSingleton().loadSprite("person_shadow");
		m_pShadow->setAnchorPoint(ccp(0.5f, 0.5f));
		m_pShadow->setScale(2.0f);
		m_pShadow->retain();

		if(getSoldierInfo().isFlying || m_personType == PERSON_TYPE_TOWER)
		{
			m_isShadowFlying = true;
			PlayerScene::getSingleton().addFlyingShadow(m_pShadow);
		}
		else
			PlayerScene::getSingleton().addShadow(m_pShadow);
	}

	if(m_personid/100 == PERSON_HERO)
	{        
        if(LogicInfo::getSingleton().m_curState == LOGICSTATE_NORMAL || m_curCamp == CAMP_ATTACK)//自己英雄
            m_pIcon = ImgMgr::getSingleton().loadSprite("hero_icon_0");
        else
            m_pIcon = ImgMgr::getSingleton().loadSprite("hero_icon_1");
		m_pIcon->setAnchorPoint(ccp(0.5f, 0.0f));
		m_pIcon->retain();
		PlayerScene::getSingleton().m_pRoot->addChild(m_pIcon, SCENE_Z_BUILDINGUI);
        
        char str[128];
        sprintf(str, "%d", m_personid % 100);

        CCLabelBMFont* pLevel = CCLabelBMFont::create(str, "Font/tbFont_b.fnt", -1, kCCTextAlignmentCenter);
        pLevel->setScale(0.5);
        pLevel->setPosition(m_pIcon->getContentSize().width/2, m_pIcon->getContentSize().height/2);
        m_pIcon->addChild(pLevel);
        
	}
    
    m_scale = 1.0;
    setLogicWidth(2);
    
    m_sprPerson->setScale(m_scale);

	setLogicPos(lx, ly);
    m_curPathNode.x = getLogicX();
    m_curPathNode.y = getLogicY();
    
	chgState(act);
    
    int dir = countDirWithLogicPos(getLogicX(), getLogicY(), LOGIC_W/2, LOGIC_W/2);
    chgDir(dir);
    
	//! 攻方不使用天赋
	if(skill0 != 0 && m_curCamp == CAMP_DEFENCE)
	{
		SkillMgr::getSingleton().addTalent(this, skill0);

		//! 特效
		int tindex = (skill0 / 100) % 10;
		char tmp[16];
		sprintf(tmp, "talent%02d", tindex);

		AniObj* ani = AniObjMgr::getSingleton().newAniObj(m_pSkillEffect[0], tmp, 0, true, true, 99999999, 0, false, NULL);
		ani->start();
	}
    
}

void Person::setLogicPos(int lx, int ly)
{
    if(m_iAct == PERSON_ACT_DROPDOWN) {
        return;
	}
    
    if (lx == getLogicX() && ly == getLogicY()) {
        return;
    }
    
    bool oldlx = getLogicX();
    bool oldly = getLogicY();
    
    if (LogicInfo::getSingleton().isAttackMode()) {
        updateDestPosition(lx, ly);
    }

    CCPoint pt = countLogicTilePt(m_sprPerson->getPositionX(), m_sprPerson->getPositionY());
    if (pt.x != lx || pt.y != ly) {
        
        pt = countSceneTilePt(lx, ly);
        chgScenePos(pt.x, pt.y);
    }
	
	BaseObj::setLogicPos(lx, ly);
    
    //walk 状态, 减少 zreorder 次数
    if (m_iAct == PERSON_ACT_WALK) {
        if ((oldlx != m_lx && m_lx % 2) == 0 || (oldly != m_ly && m_ly % 2) == 0) {
            reorderMe();
        }
    }
    else {
        reorderMe();
    }
}

void Person::reorderMe()
{
    int zorder = 0;
    
    if(getSoldierInfo().isFlying || m_personType == PERSON_TYPE_TOWER)
    {
        zorder = SCENE_Z_FLYING + (getLogicX() + getLogicY()) * SCENE_Z_BUILDING_JUMP;
    }
    else
    {
        zorder = SCENE_Z_BUILDING_BEGIN + (getLogicX() + getLogicY()) * SCENE_Z_BUILDING_JUMP;
    }
    
    reorder(zorder);
}

void Person::reorder(int zorder)
{
    PlayerScene::getSingleton().m_pRoot->reorderChild(m_sprPerson, zorder - 1);
    PlayerScene::getSingleton().m_pRoot->reorderChild(m_nodPersonBask, zorder - 2);
    
    PlayerScene::getSingleton().m_pRoot->reorderChild(m_pSkillEffect[1], zorder);
}

void Person::chgScenePos(float x, float y)
{
    CCPoint pt;

    pt.x = x;
    pt.y = y;

    m_sprPerson->setPosition(pt);
    m_nodPersonBask->setPosition(pt);

    for(int i = 0; i < 3; ++i)
    {
        if(m_pSkillEffect[i] != NULL)
        {
            m_pSkillEffect[i]->setPosition(pt);
        }
    }

    if(m_pIcon != NULL)
    {
        CCPoint cpt = pt;
        cpt.x -= (m_pProHP->getContentSize().width + m_pIcon->getContentSize().width)/2 - 5;
        cpt.y += getHeight();

        m_pIcon->setPosition(cpt);
    }

    if(m_pProHP != NULL)
    {
        CCPoint cpt = pt;
        cpt.y += getHeight() + 5;

        m_pProHP->setPosition(cpt);
    }

    if(m_pLabelName != NULL)
    {
        CCPoint cpt = pt;
        cpt.y += TILE_H * 2 + 24;

        m_pLabelName->setPosition(cpt);
    }

    if(m_pShadow != NULL)
    {
        CCPoint cpt = pt;

        if(m_info.isFlying)
            pt.y -= FLYING_HEIGHT;

        m_pShadow->setPosition(pt);
    }

    if(m_pMFTime != NULL)
    {
        CCPoint cpt = pt;
        cpt.y += TILE_H * 2 + 80;

        m_pMFTime->setPosition(cpt);
    }
    
	if(m_pProTime != NULL)
	{
        
        CCPoint cpt = pt;
        cpt.y += getHeight() + 5;
        m_pProTime->setPosition(cpt);
	}
}

int Person::getWidth()
{
    return m_pFirstFrame[0][0]->getRect().size.width;
}

int Person::getHeight()
{
    return m_pFirstFrame[0][0]->getRect().size.height;
}

CCPoint Person::countSceneTilePt(int lx, int ly)
{
	CCPoint pt = PlayerScene::getSingleton().countScenePos(lx, ly);

	pt.x += TILE_W / 2;
	pt.y += TILE_H / 2;

	pt.x += m_ptOff.x;
	pt.y += m_ptOff.y;

	return pt;
}

CCPoint Person::countLogicTilePt(int sx, int sy)
{
    CCPoint pt = PlayerScene::getSingleton().countTilePos(sx, sy);
    
    return pt;
}

void Person::onProc_DropDown(int ot)
{
	if(m_lastDropTime > 0)
	{
		m_lastDropTime -= ot;

		if(m_lastDropTime <= 0)
		{
			m_lastDropTime = -1;

			m_sprPerson->stopAllActions();
			m_sprPerson->setScale(m_scale);
            
            if (m_pCurAction) {
                m_pCurAction->release();
                m_pCurAction = NULL;
            }
            
			setLogicPos(getLogicX(), getLogicY());

			chgState(PERSON_ACT_WALK);
                
            if (m_personid >= PERSON_HERO *100)
            {
                SoundMgr::getSingleton().playEffect(CCString::createWithFormat("player_deploy_30%d.m4a",m_personid%10000/100)->getCString());

            } else
            {
                SoundMgr::getSingleton().playEffect(CCString::createWithFormat("player_deploy_%d.m4a",m_personid%10000/100)->getCString(), SOUND_DELAY);
            }

			int pid = m_personid / 100;
			if(m_personid > 30000 || pid == PERSON_GIANT)
				PlayerScene::getSingleton().setUpDownAction();
		}
	}
}

void Person::onProc_Jump(int ot)
{
	if(m_lastJumpTime >= 0)
	{
		m_lastJumpTime -= ot;

		if(m_lastJumpTime <= 0)
		{
			m_lastJumpTime = -1;

			m_sprPerson->stopAllActions();
			m_sprPerson->setScale(m_scale);
            
            if (m_pCurAction) {
                m_pCurAction->release();
                m_pCurAction = NULL;
            }

			m_lastPathTime = -1;

            setLogicPos(m_ptJumpEnd.x, m_ptJumpEnd.y);
            
            nextPath();
            
            chgState(PERSON_ACT_WALK);
            
			int pid = m_personid / 100;
			if(m_personid > 30000 || pid == PERSON_GIANT)
				PlayerScene::getSingleton().setUpDownAction();
		}
	}
}

void Person::onProc_Walk(int ot)
{
	if(m_lastPathTime > 0)
	{
		if(m_lastPathTime < ot) {
            ot = m_lastPathTime;
        }
        float pathTime = getWalkStepTime();
        
        CCPoint pt = m_sprPerson->getPosition();
        pt.x += m_curPtOff.x * ((float)ot / pathTime);
        pt.y += m_curPtOff.y * ((float)ot / pathTime);

        m_lastPathTime -= ot;
        m_walkTime += ot;
        
        chgScenePos(pt.x, pt.y);
        
        if(m_lastPathTime <= 0) {
            
			setLogicPos(m_curPathNode.x, m_curPathNode.y);
			nextPath();
		}
	}
	else {
        
        setLogicPos(m_curPathNode.x, m_curPathNode.y);
		nextPath();
    }
    
    if(LogicInfo::getSingleton().isAttackMode())
    {
        if (m_pDestTarget != NULL) {
            
            if (m_pDestTarget->isEmptyHP()) {
                clearPath();
                onMoveEnd();
            }
            else {
                
                if (canAttack() && canStopAttack()) {
                    clearPath();
                    onMoveEnd();
                }
                else if (canFindAttackStop()) {
                    findDestAttackStop();
                }
            }
        }
    }
}

// 自爆攻击
void Person::onPreAttack_Boom()
{
    PlayerScene::getSingleton().addBoomEx("person105_hit", 0, getAttackPos().x, getAttackPos().y);

	if(m_pDestTarget != NULL)
	{
		PlayerScene::getSingleton().procAOE(m_pDestTarget->getCenterPt().x, m_pDestTarget->getCenterPt().y, 
			fixDamage(getSoldierInfo().damageex), 2, m_curCamp, 2, true, fixDamage(getSoldierInfo().damageex) * getSoldierInfo().preferedTargetDamageMod, isHero());
	}

	chgHP(-getHP());

	//PlayerScene::getSingleton().killPerson(this);
}

void Person::onPreMedical()
{
	if(!canMedical())
	{
        chgDestTarget(NULL);
        
		chgState(PERSON_ACT_WALK);
        
		nextAttack();
        
		return;
	}

	if(m_pDestTarget != NULL && !m_pDestTarget->isEmptyHP())
	{
		//int damage = getSoldierInfo().damage * getSoldierInfo().attackSpeed / 1000;
		int damage = fixDamage(getSoldierInfo().damageex) * fixAttspeed(getSoldierInfo().attackSpeedex) / 1000;

		int sid = getSoldierInfo().soldierid / 100;
		if(sid == PERSON_HEALER)
		{
			PlayerScene::getSingleton().procAOE_Medical(m_pDestTarget->getCenterPt().x, m_pDestTarget->getCenterPt().y,
				-damage, 2 * 2, m_curCamp, true, 0);
		}
	}
	else
	{
		nextAttack();
	}
}

// 每次攻击，普通攻击
void Person::onPreAttack()
{
	if(m_pDestTarget != NULL && !m_pDestTarget->isEmptyHP())
	{
		//int damage = getSoldierInfo().damage * getSoldierInfo().attackSpeed / 1000;
		int damage = fixDamage(getSoldierInfo().damageex) * fixAttspeed(getSoldierInfo().attackSpeedex) / 1000;

		int sid = getSoldierInfo().soldierid / 100;
		if(sid == PERSON_GOBLIN)
		{
			if(m_pDestTarget->getObjType() == OBJTYPE_BUILDING)
			{
				int bid = ((Building*)m_pDestTarget)->m_info.buildingid / 100;
				if(bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_ELIXIR_STORAGE || bid == BUILDING_GOLD_MINE || bid == BUILDING_GOLD_STORAGE) {
					damage *= getSoldierInfo().preferedTargetDamageMod;
                }
			}
		}

		if(!isShootAttack())
			m_pDestTarget->chgHP(-damage, isHero());
        
        if (m_pDestTarget->getObjType() == OBJTYPE_PERSON) {
            ((Person*)m_pDestTarget)->onRevengeAttack();
        }
	}
	else
	{
		if(!nextAttack())
			return;
	}
}

void Person::onProc_Attack_OnTower(int ot)
{
	if(m_pCurAction == NULL)
	{
		if(m_pDestTarget == NULL || m_pDestTarget->isEmptyHP())
		{
			chgState(PERSON_ACT_NORMAL);
		}
		else if(!m_bAlreadyShoot && m_beginAttackTime >= getSoldierInfo().timeShoot)
		{
			beginShootAni();
		}
		else if(m_beginAttackTime >= getSoldierInfo().attackSpeedex)
		{
			m_beginAttackTime -= getSoldierInfo().attackSpeedex;

			beginAttackAni();
		}
	}
	else if(m_pCurAction != NULL && m_pCurAction->isDone())
	{
		m_pCurAction->release();
                        
		m_pCurAction = NULL;
		m_curAct = -1;

		if(m_pDestTarget == NULL || m_pDestTarget->isEmptyHP())
		{
			chgState(PERSON_ACT_NORMAL);
		}
	}
}

void Person::onProc_Medical()
{
	if(m_pCurAction == NULL)	// 等待下一次攻击开始
	{
		if(m_beginAttackTime >= getSoldierInfo().attackSpeedex)
		{
			m_beginAttackTime -= getSoldierInfo().attackSpeedex;

			beginAttackAni();
		}
	}
	else if(m_pCurAction->isDone())		// 动画放完，处理攻击后效果
	{
        m_pCurAction->release();
        
        m_pCurAction = NULL;
        m_curAct = -1;
        
        if(m_personState == PERSON_STATE_WAITNEXTATTACK)
        {
            if(m_beginAttackTime >= getSoldierInfo().attackSpeedex)
            {
                m_beginAttackTime -= getSoldierInfo().attackSpeedex;
                
                beginAttackAni();
            }
        }
        else {
            
            chg2WaitNextAttack();
            
            if(m_personid / 100 == PERSON_HEALER)
            {
                onPreMedical();
            }
        }
	}
}

void Person::onProc_Attack(int ot)
{
	if(LogicInfo::getSingleton().isAttackMode())
	{
		m_beginAttackTime += ot;
        
		if(m_personType == PERSON_TYPE_TOWER)
		{
			onProc_Attack_OnTower(ot);
		}
		else if(m_personid / 100 == PERSON_HEALER)
		{
			onProc_Medical();
		}
		else if (m_pDestTarget != NULL)
		{
            if(!canAttack())
			{
                if (m_pDestTarget->isEmptyHP()) {
                    chgState(PERSON_ACT_NORMAL);
                    m_sleepTime = getSoldierInfo().attackSpeedex - m_beginAttackTime;
                }
                else{
                    nextAttack();
                }
			}
			else
			{
				if(!m_bAlreadyShoot && m_beginAttackTime >= getSoldierInfo().timeShoot)
				{
					beginShootAni();
				}

				if(m_pCurAction == NULL)	// 等待下一次攻击开始
				{
					if(m_beginAttackTime >= getSoldierInfo().attackSpeedex)
					{
						m_beginAttackTime -= getSoldierInfo().attackSpeedex;

						beginAttackAni();
					}
				}
				else if(m_pCurAction->isDone())		// 动画放完，处理攻击后效果
				{
					m_pCurAction->release();
                    
					m_pCurAction = NULL;
					m_curAct = -1;

					if(m_personState == PERSON_STATE_WAITNEXTATTACK)
					{
						if(m_beginAttackTime >= getSoldierInfo().attackSpeedex)
						{
							m_beginAttackTime -= getSoldierInfo().attackSpeedex;

							beginAttackAni();
						}
					}
					else
					{
						chg2WaitNextAttack();

						if(m_personid / 100 == PERSON_WALL_BREAKER)
							onPreAttack_Boom();
						else
						{
							onPreAttack();
						}
					}
				}
			}
		}
	}
}

void Person::researchAttack()
{
    if(LogicInfo::getSingleton().isRealAttackMode() && m_curCamp != CAMP_WORKER)
	{
		if(m_personType != PERSON_TYPE_TOWER && (m_iAct != PERSON_ACT_DROPDOWN && m_iAct != PERSON_ACT_DEAD))
		{
            if (m_personid/100 == PERSON_HEALER)
            {
                if(m_pDestTarget == NULL || m_pDestTarget->getObjType() != OBJTYPE_PERSON
                   || m_pDestTarget->getHP() < m_pDestTarget->getMaxHP())
                {
                    findNextAddHpSoldier();
                    
                    m_lastResearch = PERSON_RESEARCHTIME[m_curCamp];
                }
            }
            else
            {
                if(m_pDestTarget == NULL || m_pDestTarget->getObjType() != OBJTYPE_PERSON)
                {
                    findNextAttackEnemy();
                    
                    m_lastResearch = PERSON_RESEARCHTIME[m_curCamp];
                }
            }
            
            if(m_iAct == PERSON_ACT_WALK)
            {
                if (m_lastResearch <= 0 && (m_pDestTarget != NULL && m_pDestTarget->getObjType() == OBJTYPE_PERSON)){
                    
                    if (m_personid/100 == PERSON_HEALER) {
                        findNextAddHpSoldier();
                    }
                    else{
                        findNextAttackEnemy();
                    }
                }
            }
            else if(m_iAct != PERSON_ACT_ATTACK && m_pDestTarget != NULL)
            {
                m_personState = PERSON_STATE_WAITATTACK_TARGET;
                
                chgState(PERSON_ACT_WALK);
                
                m_lastPathTime = 0;
            }
            else {
                m_lastPathTime = 0;
            }
		}
	}
}

bool Person::nextAttack()
{
	chgDestTarget(NULL);

	chgState(PERSON_ACT_WALK);

	if(PlayerScene::getSingleton().m_curFrameCanSearchNums <= 0)
	{
        m_personState = PERSON_STATE_WAITATTACK_TARGET;

		chgState(PERSON_ACT_WALK);

		return false;
	}

    m_personState = PERSON_STATE_ATTACK;

	if(m_curCamp == CAMP_ATTACK)
	{
        if(m_personid / 100 == PERSON_HEALER)
        {
            if(findNextAddHpSoldier())
            {
                return true;
            }
            
            return false;
        }
        else if(findNextAttackEnemy())
        {
            chgState(PERSON_ACT_WALK);
            return true;
        }
        else if (findNextAttackBuilding()){
            
            chgState(PERSON_ACT_WALK);
            return true;
        }
        else {
            
            chgState(PERSON_ACT_NORMAL);
            
            return false;
        }
	}
	else
	{
        if(m_personid / 100 == PERSON_HEALER)
        {
            if(findNextAddHpSoldier())
            {
                return true;
            }
            
            return false;
        }
        else if(findNextAttackEnemy())
        {
            chgState(PERSON_ACT_WALK);
            return true;
        }
	}
    
    chgState(PERSON_ACT_WALK);
    
	return true;
}

void Person::beginAttackAni()
{
	m_bAlreadyShoot = false;

    m_personState = PERSON_ACT_ATTACK;
    
    chgState(PERSON_ACT_ATTACK);
	
	onAttackBegin();
}

void Person::onProc_Normal(int ot)
{
	if(LogicInfo::getSingleton().isAttackMode())
	{
        m_sleepTime -= ot;
        
        if(m_sleepTime <= 0)
        {
            if(m_personType == PERSON_TYPE_TOWER)
            {
                if(m_pDestTarget != NULL && !m_pDestTarget->isEmptyHP())
                {
                    beginAttackAni();
                }
            }
            else if(m_pDestTarget == NULL || m_pDestTarget->isEmptyHP())
            {
                nextAttack();
                
                if (m_pDestTarget == NULL) {
                    
                    if (m_personid / 100 == PERSON_HERO) {
                        
                        m_walkStep = m_nextWalkStep;
                        
                        int lx = PlayerScene::getSingleton().m_pBuildingHeroAltar->getCenterPt().x;
                        int ly = PlayerScene::getSingleton().m_pBuildingHeroAltar->getCenterPt().y;
                        
                        int range = (PlayerScene::getSingleton().m_pBuildingHeroAltar->m_info.attackrange/50)/2;
                        
                        lx = lx + PERSON_WALK_PATH[m_walkStep].x * range;
                        ly = ly + PERSON_WALK_PATH[m_walkStep].y * range;
                        
                        PlayerScene::getSingleton().m_astar.procDestPt(getLogicX(), getLogicY(), lx, ly);
                        
                        setPersonPath(lx, ly);
                        
                        m_nextWalkStep ++;
                        if (m_nextWalkStep >= PERSON_WALK_STEP) {
                            m_nextWalkStep = 0;
                        }
                    }
                }
            }
        }
	}
	else
	{        
		if(m_personType == PERSON_TYPE_SOLDIER)
		{
			if(m_ptSleep.x < 0 || m_ptSleep.y < 0)
				return;
            
			m_sleepTime -= ot;
			if(m_sleepTime <= 0)
			{
				chgState(PERSON_ACT_WALK);

				if(PlayerScene::getSingleton().m_curFrameCanSearchNums > 0)
				{
                    int tx = m_ptSleep.x + rand() % 3 - 2;
                    int ty = m_ptSleep.y + rand() % 3 - 2;
                        
                    PlayerScene::getSingleton().m_astar.procDestPt(m_ptSleep.x, m_ptSleep.y, tx, ty);
                    
                    setPersonPath(tx, ty);
				}
				else
				{
                    m_personState = PERSON_STATE_WAIT_TARGET;
				}
			}
		}
		else if(m_personType == PERSON_TYPE_FREE)
		{
			m_sleepTime -= ot;
			if(m_sleepTime <= 0)
			{
				chgState(PERSON_ACT_WALK);

				if(PlayerScene::getSingleton().m_curFrameCanSearchNums > 0)
				{
					int lx;
					int ly;
                    
                    if (m_personid / 100 == PERSON_HERO && PlayerScene::getSingleton().m_pBuildingHeroAltar) {
                        
                        lx = PlayerScene::getSingleton().m_pBuildingHeroAltar->getCenterPt().x;
                        ly = PlayerScene::getSingleton().m_pBuildingHeroAltar->getCenterPt().y;
                        
                        int range = PlayerScene::getSingleton().m_pBuildingHeroAltar->m_info.attackrange/50;
                        PlayerScene::getSingleton().randRangeXY(lx, ly, range);
                        
                        PlayerScene::getSingleton().m_astar.procDestPt(getLogicX(), getLogicY(), lx, ly);
                        
                        setPersonPath(lx, ly);
                    }
                    else
                    {
                        PlayerScene::getSingleton().randXY(lx, ly);
                        
                        PlayerScene::getSingleton().m_astar.procDestPt(getLogicX(), getLogicY(), lx, ly);
                        
                        setPersonPath(lx, ly);
                    }
				}
				else
				{
                    m_personState = PERSON_STATE_WAITATTACK_TARGET;
				}
			}
		}
		else if(m_personType == PERSON_TYPE_TOWER)
		{
			m_sleepTime -= ot;
			if(m_sleepTime <= 0)
			{
				int dir = rand() % 8;
				chgDir(dir);

				m_sleepTime = 5000 + rand() % 3000;
			}
		}
		else if(m_personState == PERSON_STATE_WAITFREE_TARGET)
		{
			if(PlayerScene::getSingleton().m_curFrameCanSearchNums > 0)
			{
				chgState(PERSON_ACT_WALK);

				int lx;
				int ly;

				PlayerScene::getSingleton().randXY(lx, ly);
				setPersonPath(lx, ly);

                m_personType = PERSON_TYPE_FREE;
			}
			else
			{
                m_personState = PERSON_STATE_WAITATTACK_TARGET;
			}
		}
		else if(m_personState == PERSON_STATE_WAIT_TARGET)
		{
			if(PlayerScene::getSingleton().m_curFrameCanSearchNums > 0)
			{
				chgState(PERSON_ACT_WALK);

				int ttx = m_ptSleep.x + rand() % 3 - 2;
				int tty = m_ptSleep.y + rand() % 3 - 2;

				PlayerScene::getSingleton().m_astar.procDestPt(m_ptSleep.x, m_ptSleep.y, ttx, tty);

				setPersonPath(ttx, tty);

                m_personType = PERSON_TYPE_SOLDIER;
			}
			else
			{
                m_personState = PERSON_STATE_WAITATTACK_TARGET;
			}
		}
	}
}

void Person::onProc(int ot)
{
    if(isStatus(PERSON_STATUS_UPGRADE) || isStatus(PERSON_STATUS_RELEASE) || !isVisible())
        return;
    
    if (LogicInfo::getSingleton().isReplayMode() && PlayerScene::getSingleton().m_replayReadyTime > 0) {
        return;
    }
    
	BaseObj::onProc(ot);
    
    if (LogicInfo::getSingleton().isRealAttackMode()) {
        
        if (m_iAct != PERSON_ACT_DEAD) {
            m_lastResearch -= ot / 1000.0f;
            
            if (m_lastResearch < 0) {
                researchAttack();
                m_lastResearch = PERSON_RESEARCHTIME[m_curCamp];
            }
            
            if(m_personState == PERSON_STATE_WAITATTACK_TARGET)
            {
                nextAttack();
                return;
            }
        }
    }

	if(m_iAct == PERSON_ACT_DROPDOWN)
	{
		onProc_DropDown(ot);
	}
	else if(m_iAct == PERSON_ACT_JUMP)
	{
		onProc_Jump(ot);
	}
	else if(m_iAct == PERSON_ACT_WALK)
	{
		onProc_Walk(ot);
	}
	else if(m_iAct == PERSON_ACT_ATTACK)
	{
		onProc_Attack(ot);
	}
	else if(m_iAct == PERSON_ACT_NORMAL)
	{
		onProc_Normal(ot);
	}
	else if(m_iAct == PERSON_ACT_USESKILL)
	{
		onProc_Skill(ot);
	}
    else if (m_iAct == PERSON_ACT_DEAD)
    {
        if (m_pCurAction != NULL && m_pCurAction->isDone()) {
            this->releaseFromScene();
        }
    }

	if(m_pCurAction != NULL && m_pCurAction->isDone())
	{
		m_pCurAction->release();

		m_pCurAction = NULL;
		m_curAct = -1;
	}
}

bool Person::isFlying()
{
    return m_info.isFlying > 0;
}

bool Person::isCanJumpWall()
{
	if(m_curCamp == CAMP_DEFENCE || m_curCamp == CAMP_WORKER)
		return true;

	if(m_Buffs[BUFF_JUMP].value != 0)
		return true;

	return false;
}

void Person::jumpTo(int ex, int ey)
{
    chgState(PERSON_ACT_JUMP);
	m_curAct = -1;
    
	m_ptJumpEnd.x = ex;
	m_ptJumpEnd.y = ey;

	CCPoint bPos = m_sprPerson->getPosition();
	CCPoint ePos = PlayerScene::getSingleton().countScenePos(ex, ey);

	CCPoint pt1 = ccp(bPos.x + (ePos.x - bPos.x) / 2, bPos.y + 150);
	CCPoint pt2 = ccp(ePos.x, ePos.y);

	CCMoveTo* mv1 = CCMoveTo::create(0.1f, pt1);
	CCMoveTo* mv2 = CCMoveTo::create(0.1f, pt2);
    
	CCFiniteTimeAction* jumpAction = CCSequence::create(mv1, mv2, NULL);
	m_sprPerson->runAction(jumpAction);
    
    CCMoveTo* mv = CCMoveTo::create(0.2, pt2);
    m_pShadow->runAction(mv);

	m_lastJumpTime = 250;
}

void Person::onMoveEnd()
{
    if (isStatus(PERSON_STATUS_MOVE_OUT)) {
        this->releaseFromScene();
        return;
    }
    
	if(LogicInfo::getSingleton().isAttackMode())
	{        
		if(m_pDestTarget == NULL || m_pDestTarget->isEmptyHP())
		{
			nextAttack();

            if (m_pDestTarget == NULL) {
                chgState(PERSON_ACT_NORMAL);
                
                m_sleepTime = 2000;
            }
            
			return;
		}
        else if(!canAttack()){
            
            nextAttack();
            return;
        }
        
        m_beginAttackTime = 0;
        beginAttackAni();
	}
	else
	{
		chgState(PERSON_ACT_NORMAL);

		m_sleepTime = rand() % 5000 + 2000;
	}
}

bool Person::canStopAttack()
{
    if (m_path.empty()) {
        return true;
    }
    
    int lx = getLogicX();
    int ly = getLogicY();
    
    int nums = PlayerScene::getSingleton().m_astar.m_mapSoldier.get(lx, ly);
    if (nums > PERSON_ATTACK_DEST_NUMS) {
        
        if (canAttack() && m_findAttackPosNums < 2) {
            findDestAttackStop();
        }
        
        return false;
    }
    
    return true;
}

bool Person::canFindAttackStop()
{
    if (m_findAttackPosNums >= 1) {
        return false;
    }
    
    if (m_path.size() > 0) {
        PathNode path = m_path.back();
        
        int lx = path.x;
        int ly = path.y;
        
        int dw = m_pDestTarget->getLogicWidth()/2;
        if (m_pDestTarget->getLogicWidth() <= TILE_CELL) {
            dw = TILE_CELL;
        }
        
        int range = getSearchLength() + m_Buffs[BUFF_RANGE].value * 2 + m_Talents[BUFF_RANGE].value * 2 + dw;
        
        if (abs(getLogicX() - lx) < range + 5 && abs(getLogicY() - ly) < range + 5) {
            return true;
        }
    }
    
    return false;
}

void Person::findDestAttackStop()
{
    if (m_path.empty() || m_findAttackPosNums >= 1) {
        return;
    }
    
    m_findAttackPosNums ++;
    
    PathNode pathNode = m_path.back();
    
    int nums = PlayerScene::getSingleton().m_mapAttackPos.get(pathNode.x, pathNode.y);
    if (nums < PERSON_ATTACK_DEST_NUMS) {
        updateDestAttackPos();
        return;
    }
    
    int ex = pathNode.x;
    int ey = pathNode.y;
    
    int degress[12] = {10, -10, 20, -20, 30, -30, 40, -40, 50, -50, 60, -60};
    
    int count = 12;
    
    //左右旋转60‘,分散士兵占位
    for (int i = 0; i < count; i++) {
        
        CCPoint destPt = ccp(pathNode.x, pathNode.y);
        
        float radian = CC_DEGREES_TO_RADIANS(degress[i]);
        destPt = ccpRotateByAngle(destPt, m_pDestTarget->getCenterPt(), radian);
        
        destPt = PlayerScene::getSingleton().findAttackPos(this, m_pDestTarget, destPt);
        
        int nums = PlayerScene::getSingleton().m_mapAttackPos.get(destPt.x, destPt.y);
        if (nums >= PERSON_ATTACK_DEST_NUMS) {
            continue;
        }
        
        ex = destPt.x;
        ey = destPt.y;
        
        break;
    }
    
    if (ex == pathNode.x && ey == pathNode.y) {
        updateDestAttackPos();
        return;
    }

    int maxfar = 10;
    if(PlayerScene::getSingleton().m_astar.search(getLogicX(), getLogicY(), ex, ey, 0, isCanJumpWall(), false, maxfar, getLogicWidth()))
    {
        clearPath();
        
        for(AStarNodeList::iterator it = PlayerScene::getSingleton().m_astar.m_lstResult.begin(); it != PlayerScene::getSingleton().m_astar.m_lstResult.end(); ++it)
        {
            if(it->x != getLogicX() || it->y != getLogicY())
            {
                addPath(it->x, it->y);
            }
        }
    }
    
    updateDestAttackPos();
}

void Person::updateDestAttackPos()
{
    int ex = getLogicX();
    int ey = getLogicY();
    
    if (!m_path.empty()) {
        
        PathNode pathNode = m_path.back();
        
        ex = pathNode.x;
        ey = pathNode.y;
    }

    PlayerScene::getSingleton().m_mapAttackPos.sub(m_ptDestAttack.x, m_ptDestAttack.y, 1);

    PlayerScene::getSingleton().m_mapAttackPos.add(ex, ey, 1);
    m_ptDestAttack = ccp(ex, ey);
}

//! 释放技能
void Person::useSkill(int id)
{
	if(isEmptyHP() || isStatus(PERSON_STATUS_RELEASE))
		return ;

	SkillMgr::getSingleton().useSkill(this, id);
}

void Person::onProc_Skill(int ot)
{
	if(m_pCurAction == NULL || m_pCurAction->isDone())
	{
		chgState(m_iBeforeState);
	}
}

int Person::getWalkStepTime()
{
    int pathTime = ((TILE_W + TILE_H)/2*LOGIC_METER*2) / fixSpeed(m_speed);
    
    return pathTime;
}

void Person::nextPath()
{
	if(m_path.empty())
	{
		onMoveEnd();
		return;
	}
	else if(!isFlying() && isCanJumpWall()) // 跳墙
	{
		PathNode pn = m_path.front();
		if(PlayerScene::getSingleton().m_astar.isWall(pn.x, pn.y))
		{
			m_path.erase(m_path.begin());

			while(!m_path.empty() && PlayerScene::getSingleton().m_astar.isWall(m_path.front().x, m_path.front().y))
			{
				m_path.erase(m_path.begin());
			}

			if(m_path.empty())
			{
                if (m_pDestTarget) {
                    jumpTo(m_pDestTarget->getLogicX(), m_pDestTarget->getLogicY());
                }
                else {
                    onMoveEnd();
                }
			}
            else {
                
                pn = m_path.front();
                jumpTo(pn.x, pn.y);
            }
            
            return;
		}
	}

	m_curPathNode = m_path.front();
	if(m_curPathNode.x == getLogicX() && m_curPathNode.y == getLogicY())
	{
		m_path.erase(m_path.begin());

		nextPath();
		return;
	}

	int dir = countDirWithLogicPos(getLogicX(), getLogicY(), m_curPathNode.x, m_curPathNode.y);
	chgDir(dir);
   
	m_curPtEnd = PlayerScene::getSingleton().countScenePos(m_curPathNode.x, m_curPathNode.y);

	CCPoint pos = PlayerScene::getSingleton().countScenePos(getLogicX(), getLogicY());

	setLogicPos(getLogicX(), getLogicY());

	m_curPtOff.x = m_curPtEnd.x - pos.x;
	m_curPtOff.y = m_curPtEnd.y - pos.y;

	m_path.erase(m_path.begin());
    
    m_lastPathTime = getWalkStepTime();

}

void Person::addPath(int lx, int ly)
{
    if (isStatus(PERSON_STATUS_MOVE_OUT)) {
        return;
    }
    
	PathNode node(lx, ly);

	m_path.push_back(node);
}

void Person::chgDir(int dir)
{
	if(isStatus(PERSON_STATUS_RELEASE))
		return;

    if(m_curDir != dir && (dir >= 0 && dir < 8))
    {
        int rot = 0;
        if (dir == DIR_LEFT || dir == DIR_LEFT_UP || dir == DIR_DOWN_LEFT) {
            m_bFlip = true;
        }
        else {
            m_bFlip = false;
        }
        
        if(m_pSkillEffect[1] != NULL)
            m_pSkillEffect[1]->setScaleX(m_bFlip ? -1.0f : 1.0f);
        
        int dirindex = countDirIndex(dir);
        
        if(m_iAct == PERSON_ACT_DROPDOWN || m_iAct == PERSON_ACT_NORMAL)
        {
            if(dirindex >= 0 && dirindex < getSoldierInfo().dirmode)
            {
                int aniindex = PERSON_ACTION_STAND;
                if(m_personState == PERSON_STATE_WIN)
                {
                    aniindex = PERSON_ACTION_WIN;
                    
                    if(m_pFirstFrame[dirindex][aniindex] == NULL)
                        aniindex = PERSON_ACTION_STAND;
                }
                
                m_sprPerson->setDisplayFrame(m_pFirstFrame[dirindex][aniindex]);
                m_sprPerson->setAnchorPoint(ccp(0.5, 0.5));
                
                if(m_curAct != PERSON_ACT_NORMAL || m_curDirIndex != dirindex)
                {
                    if(m_pCurAction != NULL)
                    {
                        m_sprPerson->stopAction(m_pCurAction);
                        m_pCurAction->release();
                        m_pCurAction = NULL;
                    }
                    
                    if(m_pAnimate[dirindex][aniindex] != NULL)
                    {
                        CCRepeatForever* pRF = CCRepeatForever::create(m_pAnimate[dirindex][aniindex]);
                        m_pCurAction = m_sprPerson->runAction(pRF);
                        m_pCurAction->retain();
                    }
                    
                    m_curAct = PERSON_ACT_NORMAL;
                }
                
                m_sprPerson->setFlipX(m_bFlip);
                m_sprPerson->setRotation(rot);
            }
        }
        else if(m_iAct == PERSON_ACT_WALK)
        {
            if(dirindex >= 0 && dirindex < getSoldierInfo().dirmode)
            {
                m_sprPerson->setDisplayFrame(m_pFirstFrame[dirindex][PERSON_ACTION_WALK]);
                m_sprPerson->setAnchorPoint(ccp(0.5, 0.5));

                if(m_curAct != PERSON_ACT_WALK || m_curDirIndex != dirindex)
                {
                    if(m_pCurAction != NULL)
                    {
                        m_sprPerson->stopAction(m_pCurAction);
                        m_pCurAction->release();

                        m_pCurAction = NULL;
                    }

                    if(m_pAnimate[dirindex][PERSON_ACTION_WALK] != NULL)
                    {
                        CCRepeatForever* pRF = CCRepeatForever::create(m_pAnimate[dirindex][PERSON_ACTION_WALK]);
                        m_pCurAction = m_sprPerson->runAction(pRF);
                        m_pCurAction->retain();
                    }

                    m_curAct = PERSON_ACT_WALK;
                }

                m_sprPerson->setFlipX(m_bFlip);
                m_sprPerson->setRotation(rot);
            }
        }
        else if(m_iAct == PERSON_ACT_ATTACK)
        {
            if(dirindex >= 0 && dirindex < getSoldierInfo().dirmode)
            {
                int aniindex = PERSON_ACTION_ATTACK;
                if(m_personState == PERSON_STATE_WAITNEXTATTACK)
                    aniindex = PERSON_ACTION_STAND;

                m_sprPerson->setDisplayFrame(m_pFirstFrame[dirindex][aniindex]);
                m_sprPerson->setAnchorPoint(ccp(0.5, 0.5));

                if(m_pCurAction != NULL)
                {
                    m_sprPerson->stopAction(m_pCurAction);
                    m_pCurAction->release();

                    m_pCurAction = NULL;
                }

                if(m_pAnimate[dirindex][aniindex] != NULL)
                {
                    m_pCurAction = m_sprPerson->runAction(m_pAnimate[dirindex][aniindex]);
                    m_pCurAction->retain();
                }

                m_curAct = PERSON_ACT_ATTACK;

                m_sprPerson->setFlipX(m_bFlip);
                m_sprPerson->setRotation(rot);
            }
        }
        else if(m_iAct == PERSON_ACT_USESKILL)
        {
            if(dirindex >= 0 && dirindex < getSoldierInfo().dirmode)
            {
                int aniindex = PERSON_ACTION_WIN;
                if(m_personState == PERSON_STATE_WAITNEXTATTACK)
                    aniindex = PERSON_ACTION_STAND;
                else if(m_pFirstFrame[dirindex][aniindex] == NULL)
                    aniindex = PERSON_ACTION_STAND;

                m_sprPerson->setDisplayFrame(m_pFirstFrame[dirindex][aniindex]);
                m_sprPerson->setAnchorPoint(ccp(0.5, 0.5));

                if(m_pCurAction != NULL)
                {
                    m_sprPerson->stopAction(m_pCurAction);
                    m_pCurAction->release();

                    m_pCurAction = NULL;
                }

                if(m_pAnimate[dirindex][aniindex] != NULL)
                {
                    m_pCurAction = m_sprPerson->runAction(m_pAnimate[dirindex][aniindex]);
                    m_pCurAction->retain();
                }

                m_curAct = PERSON_ACT_USESKILL;

                m_sprPerson->setFlipX(m_bFlip);
                m_sprPerson->setRotation(rot);
            }
        }
        else if (m_iAct == PERSON_ACT_DEAD)
        {
            if(m_pCurAction != NULL)
            {
                m_sprPerson->stopAction(m_pCurAction);
                m_pCurAction->release();
                
                m_pCurAction = NULL;
            }
            
            if(m_pAnimate[dirindex][PERSON_ACTION_DEAD] != NULL)
            {
                m_pCurAction = m_sprPerson->runAction(m_pAnimate[dirindex][PERSON_ACTION_DEAD]);
                m_pCurAction->retain();
            }
            
            if (m_pShadow) {
                m_pShadow->setVisible(false);
            }
            if (m_pProHP) {
                m_pProHP->setVisible(false);
            }
        }
        
        m_curDirIndex = dirindex;
        m_curDir = dir;
    }
}

int Person::countDirWithLogicPos(int bx, int by, int ex, int ey)
{
	if(bx < ex)
	{
		if(by > ey)
			return DIR_LEFT;
		else if(by == ey)
			return DIR_DOWN_LEFT;
		else
			return DIR_DOWN;
	}
	else if(bx == ex)
	{
		if(by > ey)
			return DIR_LEFT_UP;
		else if(by < ey)
			return DIR_RIGHT_DOWN;
	}
	else
	{
		if(by > ey)
			return DIR_UP;
		else if(by == ey)
			return DIR_UP_RIGHT;
		else
			return DIR_RIGHT;
	}
    
	return m_curDir;
}

int Person::countDirWithScreenPos(int bx, int by, int ex, int ey)
{
	int sx = ex - bx;
	int sy = ey - by;
	float degree = 0;

	if(sx == 0 && sy == 0) {
		return m_curDir;
    }

	if(sy == 0 && sx > 0)
		degree = 90;
	else if(sy == 0 && sx < 0)
		degree = -90;
	else if(sx == 0 && sy > 0)
		degree = 0;
	else if(sx == 0 && sy < 0)
		degree = 180;
	else
	{
		float radians = atanf(sx / (float)sy);
		degree = CC_RADIANS_TO_DEGREES(radians);

		if(sx < 0 && sy < 0)
			degree = 180 + degree;
		else if(sx > 0 && sy < 0)
			degree = 180 + degree;
	}

	if(degree > 180)
		degree = -(360 - degree);

    if(degree >= 0 && degree <= 60)
        return DIR_UP_RIGHT;
    else if(degree > 60 && degree <= 120)
        return DIR_RIGHT;
    else if(degree > 120 && degree <= 180)
        return DIR_RIGHT_DOWN;

    degree = -degree;
    if(degree >= 0 && degree <= 60)
        return DIR_LEFT_UP;
    else if(degree > 60 && degree <= 120)
        return DIR_LEFT;
    else if(degree > 120 && degree <= 180)
        return DIR_DOWN_LEFT;

	return m_curDir;
}

void Person::releaseFromScene()
{
    addStatus(PERSON_STATUS_RELEASE);

	if(m_pMFTime != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pMFTime, true);
		m_pMFTime->release();

		m_pMFTime = NULL;
	}
    
    if(m_pProTime != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pProTime, true);
		m_pProTime->release();
        
		m_pProTime = NULL;
	}

	if(m_pShadow != NULL)
	{
		PlayerScene::getSingleton().removeShadow(m_pShadow, m_isShadowFlying);
		//m_scene.m_pRoot->removeChild(m_pShadow, false);
		m_pShadow->release();
		m_pShadow = NULL;
	}

	clearDestPosition();
    
	if(m_sprPerson != NULL)
	{
		m_sprPerson->stopAllActions();
        
        if (m_pCurAction) {
            m_pCurAction->release();
            m_pCurAction = NULL;
        }

		PlayerScene::getSingleton().m_pRoot->removeChild(m_sprPerson, true);
		m_sprPerson->release();

		m_sprPerson = NULL;
	}

	if(m_nodPersonBask != NULL)
	{
		m_nodPersonBask->stopAllActions();

		PlayerScene::getSingleton().m_pRoot->removeChild(m_nodPersonBask, true);
		m_nodPersonBask->release();

		m_nodPersonBask = NULL;
	}

	{
		for(int i = 0; i < 3; ++i)
		{
			if(m_pSkillEffect[i] != NULL)
			{
				AniObjMgr::getSingleton().releaseAllAni(m_pSkillEffect[i]);
				PlayerScene::getSingleton().m_pRoot->removeChild(m_pSkillEffect[i], true);
				m_pSkillEffect[i]->release();
				m_pSkillEffect[i] = NULL;
			}
		}
	}

	for(int i = 0; i < 5; ++i)
	{
		for(int j = 0; j < 5; ++j)
		{
			if(m_pAnimate[i][j] != NULL)
			{
				m_pAnimate[i][j]->release();
				m_pAnimate[i][j] = NULL;
			}

			if(m_pFirstFrame[i][j] != NULL)
			{
				m_pFirstFrame[i][j]->release();

				m_pFirstFrame[i][j] = NULL;
			}
		}
	}

	if(m_pIcon != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pIcon, true);
		m_pIcon->release();
		m_pIcon = NULL;
	}

	if(m_pProHP != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pProHP, true);
		m_pProHP->release();
		m_pProHP = NULL;
	}

	if(m_pLabelName != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pLabelName, true);
		m_pLabelName->release();
		m_pLabelName = NULL;
	}
    
    if(HeroMgr::getSingleton().m_pUpgradeAction[0] && isHero())
    {
        HeroMgr::getSingleton().m_pUpgradeAction[0]->release();
        HeroMgr::getSingleton().m_pUpgradeAction[0] = NULL;
    }
    if(HeroMgr::getSingleton().m_pUpgradeAction[1] && isHero())
    {
        HeroMgr::getSingleton().m_pUpgradeAction[1]->release();
        HeroMgr::getSingleton().m_pUpgradeAction[1] = NULL;
    }

	BaseObj::releaseFromScene();
}

void Person::chgState(int act)
{
	if(m_iBeforeState != m_iAct)
		m_iBeforeState = m_iAct;

    int curDir = m_curDir;
    
    if (m_iAct != act || m_pCurAction == NULL) {
        m_curDir = -1;
    }
    
	m_iAct = act;
    
    if (!m_isVisible) {
        setVisible(true);
    }

	if(m_iAct == PERSON_ACT_NORMAL)
	{
        m_bAlreadyShoot = true;
		chgDir(curDir);
	}
	else if(m_iAct == PERSON_ACT_WALK)
	{
        m_walkTime = -(LogicInfo::getSingleton().m_fightBeginTimeMS % getWalkStepTime());
		chgDir(curDir);
	}
	else if(m_iAct == PERSON_ACT_ATTACK)
	{
        int dir = curDir;
        
        if (m_personid / 100 != PERSON_BALLOON && m_pDestTarget != NULL) {

            CCPoint pos1 = PlayerScene::getSingleton().countScenePos(getCenterPt().x, getCenterPt().y);
            CCPoint pos2 = PlayerScene::getSingleton().countScenePos(m_pDestTarget->getCenterPt().x, m_pDestTarget->getCenterPt().y);
            
            dir = countDirWithScreenPos(pos1.x, pos1.y, pos2.x, pos2.y);
            
            if (dir == -1) {
                dir = curDir;
            }
        }
		
		chgDir(dir);
	}
	else if(m_iAct == PERSON_ACT_DROPDOWN)
	{
		CCPoint pt = countSceneTilePt(getLogicX(), getLogicY());

        CCPoint pt1 = pt;//pSpr->getPosition();
        m_sprPerson->setScale(m_scale+0.5f);

        CCMoveTo* mv = CCMoveTo::create(0.1f, pt1);

        CCScaleTo* sc = CCScaleTo::create(0.1f, m_scale);

        m_sprPerson->runAction(mv);
        m_sprPerson->runAction(sc);

        int dir = countDirWithLogicPos(getLogicX(), getLogicY(), LOGIC_W/2, LOGIC_W/2);
        if (dir == -1) {
            dir = curDir;
        }
        chgDir(dir);
        
		m_lastDropTime = 200;
	}
	else if(m_iAct == PERSON_ACT_JUMP)
	{
        chgDir(curDir);
	}
	else if(m_iAct == PERSON_ACT_USESKILL)
	{
		chgDir(curDir);
	}
    else if (m_iAct == PERSON_ACT_HIDE)
    {
        this->setVisible(false);
    }
    else if (m_iAct == PERSON_ACT_DEAD)
    {
        m_personState = PERSON_STATE_DEAD;
        chgDir(curDir);
    }

}

void Person::chgHP(int off, bool flyfont, bool isReal)
{
	if(off == 0)
		return;

	if(isStatus(PERSON_STATUS_RELEASE) || isEmptyHP() || !isVisible())
		return;

#ifdef COC_NOBUILDING_ATTACK
	if(m_curCamp == CAMP_ATTACK)
		return;
#endif // COC_NOBUILDING_ATTACK
    
	BaseObj::chgHP(off);

	if(getHP() > fixMaxHP())
		BaseObj::chgHP(-(getHP() - fixMaxHP()));

	int nhp = getHP();
	if(!LogicInfo::getSingleton().isRealAttackMode())
	{
		if(!isReal)
		{
			if(nhp <= 0)
				nhp = 1;
		}
		else
		{
			if(nhp < 0)
				nhp = 0;
		}

		setHP(nhp);
	}
    
    if(LogicInfo::getSingleton().isHero(m_personid) && m_curCamp == CAMP_ATTACK)
    {
        HeroMgr::getSingleton().m_curAttackHPOff = fixMaxHP() - getHP();
        if (HeroMgr::getSingleton().m_curAttackHPOff < 0){
            HeroMgr::getSingleton().m_curAttackHPOff = 0;
        }
        else if (HeroMgr::getSingleton().m_curAttackHPOff > fixMaxHP()) {
            HeroMgr::getSingleton().m_curAttackHPOff = fixMaxHP();
        }
    }

	if(isEmptyHP())
	{
        chgState(PERSON_ACT_DEAD);
        
		PlayerScene::getSingleton().killPerson(this);

		PlayerScene::getSingleton().addTomb(getLogicX() / 2 * 2, getLogicY() / 2 * 2);
                
        if (m_personid >= PERSON_HERO *100)
        {
            SoundMgr::getSingleton().playEffect(CCString::createWithFormat("player_death_30%d.m4a",m_personid%10000/100)->getCString());
            
        } else
        {
            SoundMgr::getSingleton().playEffect(CCString::createWithFormat("player_death_%d.m4a",m_personid%10000/100)->getCString(), SOUND_DELAY);
        }

		//! 记录防御方死亡信息
		if(LogicInfo::getSingleton().isRealAttackMode() && m_curCamp == CAMP_DEFENCE && !isHero())
		{
			LogicInfo::getSingleton().deadDefenceSoldier(m_personid);
		}
        
		return;
	}
	else if(getHP() < fixMaxHP() && m_pProHP)
	{
        m_pProHP->setVisible(true);
        if(HeroMgr::getSingleton().m_heroState == HERO_STATE_UPGRADING && LogicInfo::getSingleton().isNormalMode())
        {
            m_pProHP->setVisible(false);
        }
        if (LogicInfo::getSingleton().isAttackMode() && m_personid/100 != PERSON_HERO) {
            m_pProHP->addFadeOut(5.0);
        }
	}

	if(m_pProHP != NULL)
	{
		float chp = getHP();
		chp /= fixMaxHP();
        m_pProHP->changeProgress(chp);
	}
}

bool Person::canAttack()
{    
	if(m_personid / 100 == PERSON_HEALER)
		return canMedical();

	if(m_pDestTarget != NULL && !m_pDestTarget->isEmptyHP())
	{
		int cx = getLogicX();
		int cy = getLogicY();
		int dx = m_pDestTarget->getLogicX();
		int dy = m_pDestTarget->getLogicY();
        
        int dw = m_pDestTarget->getLogicWidth()/2;

        if (m_pDestTarget->getLogicWidth() <= TILE_CELL) {
            dw = TILE_CELL;
        }
        
        int range = getSearchLength() + m_Buffs[BUFF_RANGE].value * 2 + m_Talents[BUFF_RANGE].value * 2 + dw;
        
        if (m_pDestTarget->getObjType() == OBJTYPE_BUILDING) {
            int lw = m_pDestTarget->getLogicWidth()/2;
            if (cx <= (dx - lw) + range && cx >= (dx - lw + 1) - range && cy <= (dy - lw) + range && cy >= (dy - lw + 1) - range) {
                return true;
            }
        }
        else {
            
            range += 2;
            
            float ox = dx - cx;
            float oy = dy - cy;
            
            float dis = ox * ox + oy * oy;
            
            if (dis < range * range) {
                return true;
            }
        }
    }

	return false;
}

bool Person::canAttackBuildingEx(CCPoint& pt, Building& building)
{
	int cx = pt.x;
	int cy = pt.y;
	int dx = building.getCenterPt().x;
	int dy = building.getCenterPt().y;

	float dis = sqrt((float)(cx - dx) * (cx - dx) + (cy - dy) * (cy - dy));

	if(dis <= (getSoldierInfo().attackRangeex / LOGIC_METER) / 2 + m_Buffs[BUFF_RANGE].value * 2 + m_Talents[BUFF_RANGE].value * 2 + m_pDestTarget->getLogicWidth()/2 + getLogicWidth()/2)
	{
		return true;
	}

	return false;
}

bool Person::canMedical()
{
	if(m_pDestTarget != NULL && !m_pDestTarget->isEmptyHP())
	{
		int cx = getCenterPt().x;
		int cy = getCenterPt().y;
		int dx = m_pDestTarget->getCenterPt().x;
		int dy = m_pDestTarget->getCenterPt().y;

		float dis = sqrt((float)(cx - dx) * (cx - dx) + (cy - dy) * (cy - dy));

		if(dis <= getSoldierInfo().attackRangeex / LOGIC_METER / 2 + m_pDestTarget->getLogicWidth())
		{
			return true;
		}
	}

	return false;
}

void Person::onAttackBegin()
{
	if(m_pDestTarget == NULL)
		return;
    
    if (m_personid/100 == PERSON_HERO)
    {
        SoundMgr::getSingleton().playEffect(CCString::createWithFormat("player_attack_30%d.m4a",m_personid%10000/100)->getCString());
        
    } else
    {
        SoundMgr::getSingleton().playEffect(CCString::createWithFormat("player_attack_%d.m4a",m_personid%10000/100)->getCString(), SOUND_DELAY);
    }

	//! 添加受击特效
	if(m_pDestTarget != NULL && !isShootAttack())
	{
        char name[64];
        sprintf(name, "person%d_hit", m_personid/100);
		m_pDestTarget->addEffAni(name);
	}
}

void Person::onRevengeAttack()
{
    PlayerScene::getSingleton().revengeAttack(this);
}

void Person::onTroophousingMov(Building& building)
{
	if(m_pTroophousing != NULL && building.m_pBuildingInfo != NULL && building.m_pBuildingInfo->troophousingid == m_pTroophousing->troophousingid && !this->isStatus(PERSON_STATUS_MOVE_OUT))
	{
        int ex = building.getLogicX() - m_tlx;
        int ey = building.getLogicY() - m_tly;
        PlayerScene::getSingleton().m_astar.procDestPt(getLogicX(), getLogicY(), ex, ey);
        
		setPersonPath(ex, ey);
        chgState(PERSON_ACT_WALK);
        
        m_ptSleep.x = ex;
        m_ptSleep.y = ey;
	}
}

void Person::clearPath()
{
	m_lastPathTime = 0;
    
    if (m_path.size() > 0) {
     	m_path.clear();
    }

	m_curPathNode.x = getLogicX();
	m_curPathNode.y = getLogicY();

    updateDestAttackPos();
    
	chgState(PERSON_ACT_WALK);
}

//void Person::setVisible(bool isVisible)
//{
//	if(m_sprPerson != NULL)
//	{
//		m_sprPerson->setVisible(isVisible);
//	}
//}

int Person::countDirIndex(int dir)
{
	if(getSoldierInfo().dirmode == 3)
	{
		if(dir == DIR_UP || dir == DIR_LEFT_UP || dir == DIR_UP_RIGHT)
			return 0;
		else if(dir == DIR_DOWN || dir == DIR_RIGHT_DOWN || dir == DIR_DOWN_LEFT)
			return 2;
		else
			return 1;
	}
	else if(getSoldierInfo().dirmode == 5)
	{
		if(dir == DIR_UP)
			return 0;
		else if(dir == DIR_LEFT_UP || dir == DIR_UP_RIGHT)
			return 1;
		else if(dir == DIR_LEFT || dir == DIR_RIGHT)
			return 2;
		else if(dir == DIR_RIGHT_DOWN || dir == DIR_DOWN_LEFT)
			return 3;
		else
			return 4;
	}

	return 0;
}

CCPoint Person::getAttackPos() const
{
    CCPoint pt = m_sprPerson->getPosition();
    
	return pt;
}

int Person::getSearchLength()
{
	int sid = getSoldierInfo().soldierid / 100;

    if (isShootAttack()) {
        return getSoldierInfo().attackRangeex / LOGIC_METER;
    }
    
    if(sid == PERSON_WALL_BREAKER)
		return 1;

	return 0;
}

void Person::chg2Win()
{
    m_personState = PERSON_STATE_WIN;

	chgState(PERSON_ACT_NORMAL);
}

void Person::chg2WaitNextAttack()
{
    m_personState = PERSON_STATE_WAITNEXTATTACK;

    chgState(PERSON_ACT_ATTACK);
}

void Person::beginShootAni()
{
	if(m_pDestTarget == NULL)
		return;

	m_bAlreadyShoot = true;
	int bid = m_personid / 100;

	if(bid == PERSON_ARCHER)
	{
		CCPoint pt = countSceneTilePt(getLogicX(), getLogicY());

		int damage;

		if(m_personType == PERSON_TYPE_TOWER)
		{
			pt.x += m_ptTower.x;
			pt.y += m_ptTower.y;

			//damage = m_damageOnTower * getSoldierInfo().attackSpeed / 1000;
			damage = fixDamage(m_damageOnTower) * fixAttspeed(getSoldierInfo().attackSpeedex) / 1000;
		}
		else
		{
			//damage = getSoldierInfo().damage * getSoldierInfo().attackSpeed / 1000;
			damage = fixDamage(getSoldierInfo().damageex) * fixAttspeed(getSoldierInfo().attackSpeedex) / 1000;
		}

        switch(m_curDir)
        {
            case DIR_LEFT:
            {
                pt.x -= 40;
                pt.y += 25;
            }
                break;
            case DIR_LEFT_UP:
            {
                pt.x -= 20;
                pt.y += 50;
            }
                break;
            case DIR_UP:
            {
                pt.y += 50;
            }
                break;
            case DIR_UP_RIGHT:
            {
                pt.x += 20;
                pt.y += 50;
            }
                break;
            case DIR_RIGHT:
            {
                pt.x += 40;
                pt.y += 25;
            }
                break;
            case DIR_RIGHT_DOWN:
            {
                pt.x += 30;
                pt.y += 10;
            }
                break;
            case DIR_DOWN:
            {
            }
                break;
            case DIR_DOWN_LEFT:
            {
                pt.x -= 30;
                pt.y += 10;
            }
                break;
        }
        PlayerScene::getSingleton().addProjectile2("person102_bullet", pt.x, pt.y, m_pDestTarget, 0.1f, "person102_hit", false, false, false, 700, "", 0.0f, true, false, damage, m_curCamp, 0.0f, isHero());
		
	}
	else if(bid == PERSON_BALLOON)
	{
		CCPoint pt = countSceneTilePt(getLogicX(), getLogicY());
		pt.y += FLYING_HEIGHT;

		int damage = fixDamage(getSoldierInfo().damageex) * fixAttspeed(getSoldierInfo().attackSpeedex) / 1000;

        PlayerScene::getSingleton().addProjectile2("person106_bullet", pt.x, pt.y, m_pDestTarget, 0.1f, "person106_hit", true, false, true, 450, "", 0.0f, true, true, damage, m_curCamp, 2.0f, isHero());
		
    }
	else if(bid == PERSON_WIZARD)
	{
		CCPoint pt = countSceneTilePt(getLogicX(), getLogicY());

		int damage;
		bool isAOE;

		if(m_personType == PERSON_TYPE_TOWER)
		{
			pt.x += m_ptTower.x;
			pt.y += m_ptTower.y;

			//damage = m_damageOnTower * getSoldierInfo().attackSpeed / 1000;
			damage = fixDamage(m_damageOnTower) * fixAttspeed(getSoldierInfo().attackSpeedex) / 1000;
			isAOE = true;
		}
		else
		{
			//damage = getSoldierInfo().damage * getSoldierInfo().attackSpeed / 1000;
			damage = fixDamage(getSoldierInfo().damageex) * fixAttspeed(getSoldierInfo().attackSpeedex) / 1000;
			isAOE = true;
		}
		
        PlayerScene::getSingleton().addProjectile2("person107_bullet", pt.x, pt.y, m_pDestTarget, 0, "person107_hit", false, false, true, 1000, "", 0.0f, true, isAOE, damage, m_curCamp, 1.0f, isHero());
    }
	else if(bid == PERSON_DRAGON)
	{
		CCPoint pt = countSceneTilePt(getLogicX(), getLogicY());
		pt.y += FLYING_HEIGHT;
		
        //damage = getSoldierInfo().damage * getSoldierInfo().attackSpeed / 1000;
        int damage = fixDamage(getSoldierInfo().damageex) * fixAttspeed(getSoldierInfo().attackSpeedex) / 1000;
        bool isAOE = true;

        PlayerScene::getSingleton().addProjectile2("person109_bullet", pt.x, pt.y, m_pDestTarget, 0, "person109_hit", false, false, true, 1000, "", 0.0f, true, isAOE, damage, m_curCamp, 1.0f, isHero());
    }
}

void Person::setVisible(bool bVisible)
{
	BaseObj::setVisible(bVisible);

	if(m_pMFTime != NULL && !bVisible)
		m_pMFTime->setVisible(bVisible);
    
	if(m_pShadow != NULL)
		m_pShadow->setVisible(bVisible);

	if(m_sprPerson != NULL)
		m_sprPerson->setVisible(bVisible);

	if(m_nodPersonBask != NULL)
		m_nodPersonBask->setVisible(bVisible);

	if(m_pIcon != NULL)
		m_pIcon->setVisible(bVisible);

	if(m_pProHP != NULL)
	{
		if(!bVisible)
			m_pProHP->setVisible(bVisible);
		else if(!isEmptyHP())
			m_pProHP->setVisible(bVisible);
	}
    
    if(m_pProTime != NULL)
	{
		if(!bVisible)
			m_pProTime->setVisible(bVisible);
	}
}

void Person::upgrade(int soldierid)
{
	if(getSoldierInfo().soldierid / 100 != soldierid / 100)
		return;

	const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(soldierid);
	if(pInfo == NULL)
		return;
    
	m_info = *pInfo;

	m_personid = soldierid;
}

//! 虚函数，载入资源用
void Person::onLoadRes()
{
    char res[64];
    sprintf(res, "person%d01", m_personid/100);
    
    SceneResMgr::getSingleton().loadRes(res);
}

//! 虚函数，释放资源用
void Person::onFreeRes()
{
    char res[64];
    sprintf(res, "person%d01", m_personid/100);
    
    SceneResMgr::getSingleton().freeRes(res);
}

bool Person::findNextAddHpSoldier()
{
    if(PlayerScene::getSingleton().findNearSoldierWithAddHP(this))
    {
        return true;
    }
    
    return false;
}

bool Person::findNextAttackEnemy()
{
	if(m_pDestTarget != NULL && m_pDestTarget->getObjType() == OBJTYPE_PERSON)
		return false;

	if(m_personid / 100 == PERSON_HEALER)
		return false;

	return PlayerScene::getSingleton().findNextAttackPerson(this, isFlying(), isCanJumpWall());
}

bool Person::findNextAttackBuilding()
{
    return PlayerScene::getSingleton().findNextAttackBuilding(this, isFlying(), isCanJumpWall());
}

//! 更新寻路占地
void Person::updateDestPosition(int lx, int ly)
{
    if (isStatus(PERSON_STATUS_RELEASE)) {
        return;
    }

    PlayerScene::getSingleton().m_astar.m_mapSoldier.sub(getLogicX(), getLogicY(), 1);

	if(lx >= 0 && ly >= 0 && lx < LOGIC_W && ly < LOGIC_W)
    {
        PlayerScene::getSingleton().m_astar.m_mapSoldier.add(lx, ly, 1);
    }
}

//! 清除寻路占地
void Person::clearDestPosition()
{
    PlayerScene::getSingleton().m_astar.m_mapSoldier.sub(getLogicX(), getLogicY(), 1);
    
    PlayerScene::getSingleton().m_mapAttackPos.sub(m_ptDestAttack.x, m_ptDestAttack.y, 1);
    m_ptDestAttack = ccp(-1, -1);
}

//! 更换目标（录像用）
void Person::chgDest_Person(Person* pPerson)
{
	if(pPerson != m_pDestTarget)
	{
        chgDestTarget(pPerson);

        m_personState = PERSON_STATE_ATTACK;

		if(m_pDestTarget == NULL)
		{
            m_personState = PERSON_STATE_WAITATTACK_TARGET;
			chgState(PERSON_ACT_WALK);
		}
		else {
            BaseObj* pDestTarget = PlayerScene::getSingleton().checkAttackObject(this, pPerson);
            chgDestTarget(pDestTarget);
        }
	}
}

void Person::chgDest_Building(Building* pBuilding)
{
	if(pBuilding != m_pDestTarget)
	{
        chgDestTarget(pBuilding);

        m_personState = PERSON_STATE_ATTACK;

		if(m_pDestTarget == NULL)
		{
            m_personState = PERSON_STATE_WAITATTACK_TARGET;
			chgState(PERSON_ACT_WALK);
		}
		else
		{
            BaseObj* pDestTarget = PlayerScene::getSingleton().checkAttackObject(this, pBuilding);
            
            chgDestTarget(pDestTarget);
            
            if (pDestTarget) {
                PlayerScene::getSingleton().fixAttackPos(this);
                ((Building*)pDestTarget)->setAttackTargetAni();
            }
		}
	}
}

void Person::chgDestTarget(BaseObj *pDestObj)
{
    if (m_pDestTarget && !m_pDestTarget->isEmptyHP()) {
        m_pDestTarget->m_beAttackTargetNums --;
    }
 
    m_pWallTarget = NULL;
    m_pDestTarget = pDestObj;
    
    if (m_pDestTarget) {
        m_pDestTarget->m_beAttackTargetNums ++;
    }
    
    m_findAttackPosNums = 0;
}

void Person::addPersonPath(int lx, int ly, int dx, int dy)
{    
    if(getSoldierInfo().isFlying > 0)
    {
        setPathFlying(dx, dy);
        
        return;
    }
    
    int ex = dx;
    int ey = dy;
    
    int maxfar = 30;
    
    PlayerScene::getSingleton().m_curFrameCanSearchNums --;
    if(PlayerScene::getSingleton().m_astar.search(lx, ly, ex, ey, 0, false, false, maxfar, getLogicWidth()))
    {
        for(AStarNodeList::iterator it = PlayerScene::getSingleton().m_astar.m_lstResult.begin(); it != PlayerScene::getSingleton().m_astar.m_lstResult.end(); ++it)
        {
            if(it->x != getLogicX() || it->y != getLogicY())
            {
                addPath(it->x, it->y);
            }
        }
    }
    else if(isCanJumpWall()){
        maxfar = 60;
        
        ex = dx;
        ey = dy;
        
        PlayerScene::getSingleton().m_curFrameCanSearchNums --;
        if(PlayerScene::getSingleton().m_astar.search(lx, ly, ex, ey, 0, isCanJumpWall(), false, maxfar, getLogicWidth()))
        {
            for(AStarNodeList::iterator it = PlayerScene::getSingleton().m_astar.m_lstResult.begin(); it != PlayerScene::getSingleton().m_astar.m_lstResult.end(); ++it)
            {
                if(it->x != getLogicX() || it->y != getLogicY())
                {
                    addPath(it->x, it->y);
                }
            }
        }
    }
}

void Person::setPersonPath(int dx, int dy)
{
    clearPath();
    
    addPersonPath(getLogicX(), getLogicY(), dx, dy);
}

void Person::setPathFlying(int dx, int dy)
{
    clearPath();
    
    int ox = dx - getLogicX();
    int oy = dy - getLogicY();
    
    int m = abs(ox) > abs(oy) ? abs(ox) : abs(oy);
    
    if(ox != 0)
        ox = ox / abs(ox);
    if(oy != 0)
        oy = oy / abs(oy);
    
    int cx = getLogicX();
    int cy = getLogicY();
    
    for(int i = 0; i < m; ++i)
    {
        if(cx != dx)
            cx += ox;
        
        if(cy != dy)
            cy += oy;
        
        addPath(cx, cy);
    }
}

void Person::move2Obj(BaseObj& obj)
{
    this->move2LogicPos(obj.getLogicX(), obj.getLogicY());
}

void Person::move2LogicPos(int lx, int ly)
{
    setPersonPath(lx, ly);
}

void Person::addStatus(int status)
{
    if ((m_status & status) == 0) {
        m_status |= status;
    }
}

void Person::removeStatus(int status)
{
    if ((m_status & status) != 0) {
        m_status &= ~status;
    }
}

bool Person::isStatus(int status)
{
    return (m_status & status) != 0;
}

NS_COC_END