//
//  HeroMgr.cpp
//  GameClient
//
//  Created by chowray on 13-9-23.
//
//

#include "HeroMgr.h"
#include "LogicInfo.h"
#include "PlayerScene.h"
#include "HttpClient.h"
#include "Building.h"
#include "Building_HeroAltar.h"
#include "PersonMgr.h"
#include "SkillMgr.h"
#include "SoundMgr.h"
#include "GameWorldScene.h"

NS_COC_BEGIN

HeroMgr::HeroMgr()
: m_curAttackExp(0)
, m_lastHeroHPTime(0)
, m_heroLastTime(0)
, m_heroFullTime(0)
, m_heroLastTimems(0)
, m_heroCurTime(0)
, m_showLightAction(false)
{
    init();
}

HeroMgr::~HeroMgr()
{
    
}

HeroMgr& HeroMgr::getSingleton()
{
	static HeroMgr s_mgr;
    
	return s_mgr;
}

void HeroMgr::init()
{
    memset(&m_myHero, 0, sizeof(m_myHero));
    m_isSelect = false;
    m_heroState = HERO_STATE_NORMAL;
    m_otherHeroState = HERO_STATE_NORMAL;
    m_isFreeTrain = 0;
    m_pUpgradeAction[0] = NULL;
    m_pUpgradeAction[1] = NULL;
}

void HeroMgr::createHero(int heroid)
{
    m_myHero.heroid = heroid;
    m_myHero.hp = PersonMgr::getSingleton().getSoldierInfo(heroid)->hpex;
    m_myHero.maxhp = PersonMgr::getSingleton().getSoldierInfo(heroid)->hpex;
    m_myHero.maxexp = PersonMgr::getSingleton().getSoldierInfo(heroid)->needexp;
    m_myHero.skill0 = 1000101;
    m_myHero.skill3 = 2000101;
    for(int i = 0; i < 12; i++)
        m_myHero.bskill[i] = 1;
    
    LogicInfo::getSingleton().selectHero(heroid);
        
    PlayerScene::getSingleton().initHero(m_myHero);
}

bool HeroMgr::isFullHP()
{
    if (m_myHero.heroid == 0)
        return false;
    
    return m_myHero.hp >= m_myHero.maxhp;
}

void HeroMgr::onProcHero(int ot)
{
    time_t ct = getCurrentTime();
    if(ct != m_lastHeroHPTime)
    {
        int lasttime = LogicInfo::getSingleton().getHeroLastTime();
        if(lasttime > 0)
        {
            m_lastHeroHPTime = getCurrentTime();
            
            std::string str;
            LogicInfo::getSingleton().countLastTimeString(str, lasttime);
        }
    }
    if(m_heroLastTime > 0)
    {
        m_heroLastTimems += ot;
        if(m_heroLastTimems >= 1000)
        {
            m_heroLastTime -= m_heroLastTimems / 1000;
            m_heroLastTimems = m_heroLastTimems % 1000;
            
            onUpdateHeroTime();
            
            if(m_heroLastTime <= 0)
            {
                m_heroLastTime = 0;
                finishUpgrade(false);
            }
        }
    }
}

int HeroMgr::onProcHeroHp()
{
    if(m_myHero.hp < m_myHero.maxhp)
    {
        time_t ct = getCurrentTime();
        int off = ct - m_myHero.curtime;
        if(off <= 0)
            return 0;
        
        int pointSecond = 60/getRecoverHP();
        
        if(off >= pointSecond)
        {
            m_myHero.hp ++;
            m_myHero.curtime = ct;
            
            if(m_myHero.hp > m_myHero.maxhp)
            {
                m_myHero.hp = m_myHero.maxhp;
                m_myHero.curtime = 0;
            }
            
            LogicInfo::getSingleton().onNotify(NOTIFY_HEROHPCHG);
            
            PlayerScene::getSingleton().onUpdateHero();
        }
        
        return off;
    }
    
    return 0;
}

int HeroMgr::getHeroLastTime()
{
	int off = onProcHeroHp();

	if(m_myHero.hp < m_myHero.maxhp)
	{
		int offhp = m_myHero.maxhp - m_myHero.hp;
        
        int pointSecond = 60/getRecoverHP();
        
        int time = offhp * pointSecond - off;
        if (time <= 0) time = 0;
		return time;
	}

	return 0;
}

void HeroMgr::onBeginAttack()
{
	m_curAttackExp = 0;
	m_curAttackHPOff = 0;
	m_isHeroInWar = false;
}

int HeroMgr::getFillHeroHPGems()
{
    int lasttime = getHeroLastTime();
	if(lasttime <= 0)
		return 0;

	//return ceil(lasttime / (60 * 4));
    
    return lasttime / (60 * 4) + (lasttime % (60 * 4) > 0 ? 1 : 0);
}

bool HeroMgr::fastHeroHP()
{
	int lasttime = getHeroLastTime();
	if(lasttime <= 0)
		return true;

	int gem = getFillHeroHPGems();
	if(LogicInfo::getSingleton().m_myInfo.gemex < gem)
		return false;

	LogicInfo::getSingleton().chgGem(-gem);

	m_myHero.hp = m_myHero.maxhp;
	m_myHero.curtime = 0;

	LogicInfo::getSingleton().onNotify(NOTIFY_HEROHPCHG);

	PlayerScene::getSingleton().onUpdateHero();
	HttpClient::getSingleton().fastHeroHP(lasttime);

	return true;
}

int HeroMgr::getRecoverHP()
{
	const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(m_myHero.heroid);
	if(pInfo != NULL)
		return pInfo->recoverhp;

	return 0;
}

void HeroMgr::setTime(int fulltime, int lasttime)
{
    m_heroFullTime = fulltime;
    m_heroLastTime = lasttime;
    
    onUpdateHeroTime();
}

void HeroMgr::onUpdateHeroTime()
{
    if(!LogicInfo::getSingleton().isNormalMode() || PlayerScene::getSingleton().m_isEditMode)
        return;
    
    Person *pHero = PlayerScene::getSingleton().m_pHero;
    if(pHero == NULL)
        return;
    
    if(m_heroLastTime <= 0)
    {
        if(pHero->m_pMFTime != NULL)
            pHero->m_pMFTime->setVisible(false);
        
        if(pHero->m_pProTime != NULL)
            pHero->m_pProTime->setVisible(false);
        
        if(pHero->m_pProHP)
            pHero->m_pProHP->setVisible(true);
        
        if(pHero->m_pIcon)
            pHero->m_pIcon->setVisible(true);
        
        return;
    }
    
    if(LogicInfo::getSingleton().isNormalMode())
    {
        if(pHero->m_pProTime == NULL)
        {
            CCSprite* pFrameBg = ImgMgr::getSingleton().loadSprite("ui_img_pro_bk_1");
            CCSprite* pFrame = ImgMgr::getSingleton().loadSprite("ui_img_pro_org_3");
            
            pHero->m_pProTime = CProgress::create(pFrame, pFrameBg);
            pHero->m_pProTime->setAnchorPoint(ccp(0.5, 0));
            pHero->m_pProTime->retain();
            PlayerScene::getSingleton().m_pRoot->addChild(pHero->m_pProTime, SCENE_Z_BUILDINGUI);
        }
        
        if(pHero->m_pMFTime == NULL)
        {
            pHero->m_pMFTime = CCLabelBMFont::create("", "Font/tbFont_b.fnt", -1, kCCTextAlignmentCenter);
            pHero->m_pMFTime->setScale(0.5);
            pHero->m_pMFTime->retain();
            PlayerScene::getSingleton().m_pRoot->addChild(pHero->m_pMFTime, SCENE_Z_BUILDINGUI);
        }
    }
    
    CCPoint pt = pHero->countSceneTilePt(pHero->getLogicX(), pHero->getLogicY());
    
    if(pHero->m_pMFTime != NULL)
    {
        pHero->m_pMFTime->setVisible(true);
        std::string str;
        
        LogicInfo::getSingleton().countLastTimeString(str, m_heroLastTime);
        
        pHero->m_pMFTime->setString(str.c_str());
        
        CCPoint cpt = pt;
        cpt.y += TILE_H * 2 + 80;
        pHero->m_pMFTime->setPosition(cpt);
    }
    
    if(pHero->m_pProTime != NULL)
    {
        CCPoint cpt = pt;
        cpt.y += pHero->getHeight() + 5;
        pHero->m_pProTime->changeProgress(1 - (float)m_heroLastTime / m_heroFullTime);
        pHero->m_pProTime->setPosition(cpt);
        
        pHero->m_pProTime->setVisible(true);
    }
}

int HeroMgr::countWarExp(int star, int atkbaselevel, int defbaselevel)
{
	if(!m_isHeroInWar)
		return 0;

	const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(m_myHero.heroid);
	if(pInfo == NULL)
		return 0;

    if (!canLevelUp()) {
        return 0;
    }
    
	float jc = 1 - ((atkbaselevel - defbaselevel) / 3.0f);

	if(defbaselevel <= atkbaselevel - 3)
		jc = 0.2;
	else if(defbaselevel >= atkbaselevel + 3)
		jc = 2;

	int exp = pInfo->baseexp * jc * star / 3;

	return exp;
}

void HeroMgr::refreshHero()
{
    Person *pHero = NULL;
    int state;
    if(LogicInfo::getSingleton().isNormalMode())
    {
        pHero = PlayerScene::getSingleton().m_pHero;
        state = m_heroState;
    }
    else
    {
        pHero = PlayerScene::getSingleton().m_pOtherHero;
        state = m_otherHeroState;
    }
    
    if(pHero == NULL)
        return;
    
    Building_HeroAltar* pHeroAltar = (Building_HeroAltar*)PlayerScene::getSingleton().m_pBuildingHeroAltar;

    if(state == HERO_STATE_NORMAL)
    {
        if(m_pUpgradeAction[0] != NULL)
        {
            pHero->m_sprPerson->stopAction(m_pUpgradeAction[0]);
            m_pUpgradeAction[0]->release();
            m_pUpgradeAction[0] = NULL;
        }
        
        if(m_pUpgradeAction[1] != NULL)
        {
            pHero->m_sprPerson->stopAction(m_pUpgradeAction[1]);
            m_pUpgradeAction[1]->release();
            m_pUpgradeAction[1] = NULL;
        }
        
        if(pHero->m_pIcon)
        {
            pHero->m_pIcon->setVisible(true);
            char str[128];
            sprintf(str, "%d", pHero->m_personid % 100);
            CCLabelBMFont *pLevel = (CCLabelBMFont *)pHero->m_pIcon->getChildren()->objectAtIndex(0);
            if(pLevel)
                pLevel->setString(str);
        }
        
        if(pHero->m_pProHP)
        {
            pHero->m_pProHP->setVisible(true);
        }
        
        if(pHero->m_pProTime)
            pHero->m_pProTime->setVisible(false);
        
        if(pHero->m_pMFTime)
            pHero->m_pMFTime->setVisible(false);
        
        if(pHeroAltar != NULL)
        {
            pHero->m_sleepTime = 0;
            pHero->setLogicPos(pHeroAltar->getLogicX(), pHeroAltar->getLogicY());
        }
        
        pHero->removeStatus(PERSON_STATUS_UPGRADE);
    }
    else if(state == HERO_STATE_UPGRADING)
    {
        if(pHero->m_pIcon)
            pHero->m_pIcon->setVisible(false);
        
        if(pHero->m_pProHP)
            pHero->m_pProHP->setVisible(false);
        
        if(LogicInfo::getSingleton().isNormalMode())
        {
            if(pHero->m_pProTime)
                pHero->m_pProTime->setVisible(true);
            
            if(pHero->m_pMFTime)
                pHero->m_pMFTime->setVisible(true);
        }
        else
        {
            if(pHero->m_pProTime)
                pHero->m_pProTime->setVisible(false);
            
            if(pHero->m_pMFTime)
                pHero->m_pMFTime->setVisible(false);
        }
        
        if(pHeroAltar != NULL)
        {
            pHero->clearPath();
            pHero->onMoveEnd();
            pHero->chgDir(5);
            
            if(pHero->m_pCurAction != NULL)
            {
                pHero->m_sprPerson->stopAction(pHero->m_pCurAction);
                pHero->m_pCurAction->release();
                pHero->m_pCurAction = NULL;
            }
            
            if(LogicInfo::getSingleton().isNormalMode() && m_showLightAction == true)
            {
                m_showLightAction = false;
                if(m_pUpgradeAction[0] == NULL )
                {
                    CCAnimate* pAction = (CCAnimate*)actionWithAnimate("person30101_06_0", 11, 0.1, false);
                    CCCallFunc *pCall = CCCallFunc::create(this, callfunc_selector(HeroMgr::upgradeActionCall));
                    m_pUpgradeAction[0] = CCSequence::create(pAction, pCall, NULL);
                    m_pUpgradeAction[0]->retain();
                    
                    pHero->m_sprPerson->setDisplayFrame(ImgMgr::getSingleton().getFrame("person30101_06_00"));
                    pHero->m_sprPerson->setAnchorPoint(ccp(0.5, 0.5));
                    pHero->m_sprPerson->runAction(m_pUpgradeAction[0]);
                }
            }
            else
            {
                if(m_pUpgradeAction[1] == NULL)
                {
                    CCAction* pUplevelAct = actionWithAnimate("person30101_07_0", 21, 0.1, true);
                    m_pUpgradeAction[1] = pUplevelAct;
                    m_pUpgradeAction[1]->retain();
                    pHero->m_sprPerson->setDisplayFrame(ImgMgr::getSingleton().getFrame("person30101_07_00"));
                    pHero->m_sprPerson->setAnchorPoint(ccp(0.5, 0.5));
                    pHero->m_sprPerson->runAction(m_pUpgradeAction[1]);
                }
            }
            
            pHero->setLogicPos(pHeroAltar->getCenterPt().x, pHeroAltar->getCenterPt().y);
            pHero->reorder(pHeroAltar->m_pBulding->getZOrder() + 1);
        }
        
        pHero->addStatus(PERSON_STATUS_UPGRADE);
    }
}

void HeroMgr::upgradeActionCall()
{
    Person *pHero = PlayerScene::getSingleton().m_pHero;
    if(pHero == NULL)
        return;
    
    if(m_pUpgradeAction[0] != NULL)
    {
        pHero->m_sprPerson->stopAction(m_pUpgradeAction[0]);
        m_pUpgradeAction[0]->release();
        m_pUpgradeAction[0] = NULL;
    }
    
    if(m_pUpgradeAction[1] == NULL)
    {
        CCAction* pUplevelAct = actionWithAnimate("person30101_07_0", 21, 0.1, true);
        m_pUpgradeAction[1] = pUplevelAct;
        m_pUpgradeAction[1]->retain();
        pHero->m_sprPerson->setDisplayFrame(ImgMgr::getSingleton().getFrame("person30101_07_00"));
        pHero->m_sprPerson->setAnchorPoint(ccp(0.5, 0.5));
        pHero->m_sprPerson->runAction(m_pUpgradeAction[1]);
    }
}

bool HeroMgr::finishUpgrade(bool fast)
{
    //加协议
    const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(m_myHero.heroid);
    
    if(pInfo == NULL)
        return false;
    
    m_heroState = HERO_STATE_NORMAL;
    
    
    int gem = 0;
    gem = LogicInfo::getSingleton().countBuildingHeroGem();
    if(fast)
    {
        PlayerScene::getSingleton().onHeroSelect();
        LogicInfo::getSingleton().chgGem(-gem);
    }
    else
    {
        SoundMgr::getSingleton().playEffect(CCString::createWithFormat("player_deploy_%d.m4a",m_myHero.heroid/100)->getCString());
        if(m_isSelect)
        {
            for (int i = 0; i < PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->m_lstBuildingBtn.size(); i ++)
            {
                PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->m_lstBuildingBtn[i]->setVisible(false);
            }
        }
    }
    
    setTime(0, 0);
    m_myHero.maxhp = pInfo->hpex;
    m_myHero.hp = m_myHero.maxhp;
    m_myHero.curtime = 0;
    
    
    
    
    refreshHero();
    
    HttpClient::getSingleton().finishUpLevelHero(gem);
    return true;
}

void HeroMgr::cancelUpgrade()
{
    Person *pHero = PlayerScene::getSingleton().m_pHero;
    if(pHero == NULL)
        return;
    
	if(m_heroState == HERO_STATE_NORMAL)
        return;
    
    int heroid = m_myHero.heroid - 1;
    
    const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(heroid);
    if(pInfo == NULL)
    {
        pInfo = PersonMgr::getSingleton().getSoldierInfo(m_myHero.heroid);
        if(pInfo == NULL)
            return;
    }
    
    const SoldierInfo* pInfo2 = PersonMgr::getSingleton().getSoldierInfo(heroid+1);
    if(pInfo2 != NULL)
        LogicInfo::getSingleton().chgMoney(pInfo2->upgradeRes, pInfo2->upgradeCost / 2);
    
    pHero->m_personid = heroid;
    m_myHero.heroid = heroid;
    m_myHero.maxhp = pInfo->hpex;
    if(m_myHero.hp > m_myHero.maxhp)
        m_myHero.hp = m_myHero.maxhp;
    m_heroLastTime = 0;
    m_heroCurTime = getCurrentTime();
    
    m_heroState = HERO_STATE_NORMAL;
    refreshHero();
    
    PlayerScene::getSingleton().onHeroSelect();
    HttpClient::getSingleton().cancelUpLevelHero();
}

int HeroMgr::upLevelHero(int &need)
{
    if (!canLevelUp()) {
        return MSG_ERROR;
    }
	need = 0;
    
	if(!LogicInfo::getSingleton().hasFreeWorker())
    {
        return MSG_NOWORKER;
    }
    
    const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(m_myHero.heroid + 1);
    if(pInfo == NULL)
        return MSG_ERROR;
    
    if(pInfo->upgradeCost > LogicInfo::getSingleton().getMoney(pInfo->upgradeRes))
    {
        need = pInfo->upgradeCost - LogicInfo::getSingleton().getMoney(pInfo->upgradeRes);
        
        return MSG_NOGEM - pInfo->upgradeRes;
    }
    
    LogicInfo::getSingleton().chgMoney(pInfo->upgradeRes, -pInfo->upgradeCost);
    
    m_heroLastTime = pInfo->upgradeTimeHour*60*60;
    m_heroCurTime = getCurrentTime();
    
    Person *pHero = PlayerScene::getSingleton().m_pHero;
    
    int lasttime = m_heroLastTime;
    int fulltime = m_heroLastTime;
    setTime(fulltime, lasttime);
    if(m_heroLastTime == 0)
    {
        m_heroState = HERO_STATE_NORMAL;
    }
    else
    {
        m_heroState = HERO_STATE_UPGRADING;
    }
    
    int newHeroId = m_myHero.heroid +1;
    m_myHero.heroid = newHeroId;
    pHero->m_personid = newHeroId;
    m_showLightAction = true;
    refreshHero();
    PlayerScene::getSingleton().onHeroSelect();
    HttpClient::getSingleton().upLevelHero();
    
    return 1;
}

bool HeroMgr::canLevelUp()
{
	int townLevel = LogicInfo::getSingleton().m_myBaseLevel;

	const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(m_myHero.heroid + 1);
	if(pInfo == NULL)
		return false;

    if(townLevel >= pInfo->needtownhall)
		return true;

	return false;
}

int HeroMgr::getHeroLevel()
{
	return m_myHero.heroid % 100;
}

int HeroMgr::getEquSkill(int index)
{
    int skillId = 0;
    if (index == 0) {
        if (m_myHero.skill0 != 0) {
            skillId = m_myHero.skill0 + m_myHero.bskill[(m_myHero.skill0/100)%10000-1];
        }
    }else if (index == 1) {
        if (m_myHero.skill1 != 0) {
            skillId = m_myHero.skill1 + m_myHero.bskill[(m_myHero.skill1/100)%10000-1];
        }
    }else if (index == 2) {
        if (m_myHero.skill2 != 0) {
            skillId = m_myHero.skill2 + m_myHero.bskill[(m_myHero.skill2/100)%10000-1];
        }
    }
    return skillId;
}

NS_COC_END