#include "Building_Attack.h"
#include "BuildingMgr.h"
#include "ImgMgr.h"
#include "PlayerScene.h"
#include "Action_LightFlash.h"
#include "Action_GrayFlash.h"
#include "Person.h"
#include "SimpleAudioEngine.h"
#include "SoundMgr.h"
#include "LogicInfo.h"
#include "HeroMgr.h"
#include "CUINodeCache.h"
#include "ResConfig.h"

NS_COC_BEGIN

Building_Attack::Building_Attack(BuildingInfo info)
	: Building(info)
	, m_pBulding(NULL)
	, m_pShadow(NULL)
	, m_pSelAction(NULL)
	, m_pDestPerson(NULL)
{
	m_type = BUILDING_TYPE_ATTACK;

	m_pLightArea[0] = NULL;
	m_pLightArea[1] = NULL;
}

Building_Attack::~Building_Attack()
{
}

void Building_Attack::init(int buildingid, int lx, int ly)
{
	char strname[128];

    sprintf(strname, "%s_1", m_info.res.c_str());
    m_pShadow = ImgMgr::getSingleton().loadSprite(strname);
    if(m_pShadow != NULL)
    {
        m_pShadow->setAnchorPoint(ccp(0.5f, 0));
        m_pShadow->retain();
        m_pShadow->setScale(2.0f);
        PlayerScene::getSingleton().addShadow(m_pShadow);
    }

	{
		sprintf(strname, "%s_2", m_info.res.c_str());
		m_pBulding = ImgMgr::getSingleton().loadSprite(strname);
		m_pBulding->setAnchorPoint(ccp(0.5, 0));
		m_pBulding->retain();
		PlayerScene::getSingleton().m_pRoot->addChild(m_pBulding, SCENE_Z_BUILDING_BEGIN);
	}

	{
		char strname[128];
		sprintf(strname, "area_white");
		m_pLightArea[0] = ImgMgr::getSingleton().loadSprite(strname);
		m_pLightArea[0]->setAnchorPoint(ccp(0.5, 0.5));
		m_pLightArea[0]->retain();
		PlayerScene::getSingleton().m_pRoot->addChild(m_pLightArea[0], SCENE_Z_HUAN);

		Action_LightFlash* pLightFlash = Action_LightFlash::create(3, 156, 256);
		CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
		m_pLightArea[0]->runAction(pRF);

		m_pLightArea[0]->setScale(m_info.attackrange * 2 / 500.0f);

		m_pLightArea[0]->setVisible(false);
	}

	if(m_info.minattackrange > 0)
	{
		char strname[128];
		sprintf(strname, "area_red");
		m_pLightArea[1] = ImgMgr::getSingleton().loadSprite(strname);
		m_pLightArea[1]->setAnchorPoint(ccp(0.5, 0.5));
		m_pLightArea[1]->retain();
		PlayerScene::getSingleton().m_pRoot->addChild(m_pLightArea[1], SCENE_Z_HUAN);

		Action_LightFlash* pLightFlash = Action_LightFlash::create(3, 156, 256);
		CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
		m_pLightArea[1]->runAction(pRF);

		m_pLightArea[1]->setScale(m_info.minattackrange * 2 / 500.0f);

		m_pLightArea[1]->setVisible(false);
	}

	m_lastAttackTimeMs = m_info.attackspeed;

	Building::init(buildingid, lx, ly);
}

void Building_Attack::setLogicPos(int lx, int ly)
{
	Building::setLogicPos(lx, ly);

	if(m_isPreBuilding)
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(false);

		if(m_pBulding != NULL)
			m_pBulding->setVisible(false);
	}
	else if(isEmptyHP())
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(false);

		if(m_pBulding != NULL)
			m_pBulding->setVisible(false);

		if(m_pLightArea[0] != NULL)
			m_pLightArea[0]->setVisible(false);

		if(m_pLightArea[1] != NULL)
			m_pLightArea[1]->setVisible(false);
	}
	else
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(true);

		if(m_pBulding != NULL)
			m_pBulding->setVisible(true);
	}

	//else
	{
		CCPoint pt = countSceneTilePt(lx, ly);

		if(m_pShadow != NULL)
			m_pShadow->setPosition(pt);

		if(m_pBulding != NULL)
			m_pBulding->setPosition(pt);

		if(m_pLightArea[0] != NULL)
		{
            CCPoint cpt = pt;

            if (m_pTile) {
                cpt.y += m_pTile->getContentSize().height/2;
            }
            m_pLightArea[0]->setPosition(cpt);
		}

		if(m_pLightArea[1] != NULL)
		{
            CCPoint cpt = pt;
            
            if (m_pTile) {
                cpt.y += m_pTile->getContentSize().height/2;
            }
            m_pLightArea[1]->setPosition(cpt);
			
		}
	}
}

void Building_Attack::setVisible(bool isVisible)
{
    Building::setVisible(isVisible);
    
    if(m_pBulding) {
        m_pBulding->setVisible(isVisible);
    }
    if (m_pShadow) {
        m_pShadow->setVisible(isVisible);
    }
    if (m_pLightArea[0]) {
        m_pLightArea[0]->setVisible(isVisible);
    }
    if (m_pLightArea[1]) {
        m_pLightArea[1]->setVisible(isVisible);
    }
}

int Building_Attack::getBuildingWidth()
{
    return m_pBulding->getContentSize().width;
}

int Building_Attack::getBuildingHeight()
{
    return m_pBulding->getOffsetPosition().y + m_pBulding->getTextureRect().size.height;
}

void Building_Attack::releaseFromScene()
{
	if(m_pShadow != NULL)
	{
		PlayerScene::getSingleton().removeShadow(m_pShadow, false);
		//m_scene.m_pRoot->removeChild(m_pShadow, true);
		m_pShadow->release();
		m_pShadow = NULL;
	}

	if(m_pBulding != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pBulding, true);
		m_pBulding->release();
		m_pBulding = NULL;
	}

	if(m_pLightArea[0] != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pLightArea[0], true);
		m_pLightArea[0]->release();
		m_pLightArea[0] = NULL;
	}

	if(m_pLightArea[1] != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pLightArea[1], true);
		m_pLightArea[1]->release();
		m_pLightArea[1] = NULL;
	}
    
	Building::releaseFromScene();
}

void Building_Attack::reorder(int zorder)
{
	Building::reorder(zorder);

	if(m_pBulding != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->reorderChild(m_pBulding, zorder);
	}
}

void Building_Attack::chgSelMode(bool isSel)
{
	Building::chgSelMode(isSel);

	if(isSel)
	{
		Action_GrayFlash* pLightFlash = Action_GrayFlash::create(3, 128, 256);
		CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
		m_pSelAction = m_pBulding->runAction(pRF);

		setUpDownAction(m_pBulding, true);

		if(m_pLightArea[0] != NULL)
			m_pLightArea[0]->setVisible(true);

		if(m_pLightArea[1] != NULL)
			m_pLightArea[1]->setVisible(true);
	}
	else
	{
		m_pBulding->stopAction(m_pSelAction);
        m_pSelAction = NULL;

		ccColor3B c;
		c.r = c.g = c.b = 255;

		m_pBulding->setColor(c);

		if(m_pLightArea[0] != NULL)
			m_pLightArea[0]->setVisible(false);

		if(m_pLightArea[1] != NULL)
			m_pLightArea[1]->setVisible(false);
	}
}

bool Building_Attack::canAttack(Person* pPerson)
{
	if(m_pDestPerson->isEmptyHP())
		return false;
    
    if(m_pDestPerson->isHidden())
        return false;

	if(m_pDestPerson->isStatus(PERSON_STATUS_RELEASE))
		return false;

	float dis = sqrt((getCenterPt().x - pPerson->getCenterPt().x) * (getCenterPt().x - pPerson->getCenterPt().x) + (getCenterPt().y - pPerson->getCenterPt().y) * (getCenterPt().y - pPerson->getCenterPt().y));

	float min = m_info.minattackrange == 0 ? 0 : (m_info.width / 2 + m_info.minattackrange / (LOGIC_METER*2));
	float max = m_info.width / 2 + m_info.attackrange / (LOGIC_METER*2);
	if(dis >= min && dis <= max * 2)
		return true;

	return false;
}

void Building_Attack::onStartAttack()
{
	
}

void Building_Attack::onProc(int ot)
{
	Building::onProc(ot);

	if(LogicInfo::getSingleton().isAttackMode())
	{
#ifdef COC_NOBUILDING_ATTACK
		return;
#endif // COC_NOBUILDING_ATTACK

		if(isEmptyHP() || m_pBuildingInfo->state != 2)
		{
			if(m_pDestPerson != NULL)
				m_pDestPerson = NULL;

			return;
		}

		if(m_pDestPerson != NULL)
		{
			if(!canAttack(m_pDestPerson))
				m_pDestPerson = NULL;
		}

		if(m_pDestPerson == NULL)
		{
			int mode = countAttackMode();
            float minAttackRange = m_info.minattackrange == 0 ? 0 : m_info.minattackrange / LOGIC_METER;
            float maxAttackRange = m_info.attackrange / LOGIC_METER;
            
			m_pDestPerson = PlayerScene::getSingleton().findNearPerson(getCenterPt().x, getCenterPt().y, minAttackRange, maxAttackRange, mode, CAMP_ATTACK);
		}

		if(m_pDestPerson != NULL)
		{
			if(m_lastAttackTimeMs >= m_info.attackspeed)
			{
				onStartAttack();

				m_lastAttackTimeMs = 0;
			}

			m_lastAttackTimeMs += ot;
		}
	}
}

void Building_Attack::onBuildingEnd()
{
	if(m_isPreBuilding)
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(true);

		if(m_pBulding != NULL)
			m_pBulding->setVisible(true);
	}

	Building::onBuildingEnd();
}

void Building_Attack::setBeAttackMode()
{
	if(m_pActBeAttack != NULL)
		return ;

	if(m_pBulding != NULL)
		setBeAttackAni(m_pBulding, true);

	if(m_pShadow != NULL)
		setBeAttackAni(m_pShadow, false);
}

NS_COC_END