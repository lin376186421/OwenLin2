#include "Building_Normal.h"
#include "BuildingMgr.h"
#include "ImgMgr.h"
#include "PlayerScene.h"
#include "Action_GrayFlash.h"
#include "LogicInfo.h"

NS_COC_BEGIN

Building_Normal::Building_Normal(BuildingInfo info)
	: Building(info)
	, m_pBulding(NULL)
	, m_pShadow(NULL)
	, m_pSelAction(NULL)
    , m_pSprAni(NULL)
    , m_pSprAction(NULL)
{
	m_type = BUILDING_TYPE_NORMAL;
}

Building_Normal::~Building_Normal()
{
}

void Building_Normal::init(int buildingid, int lx, int ly)
{
	{
		char strname[128];
		sprintf(strname, "%s_1", m_info.res.c_str());
		m_pShadow = ImgMgr::getSingleton().loadSprite(strname);
		if(m_pShadow != NULL)
		{
			m_pShadow->setAnchorPoint(ccp(0.5f, 0));
			m_pShadow->setScale(2.0f);
			m_pShadow->retain();
			PlayerScene::getSingleton().addShadow(m_pShadow);
		}

		//m_pShadow->setScale(0.5f);
	}

	{
		char strname[128];
		sprintf(strname, "%s_2", m_info.res.c_str());
		m_pBulding = ImgMgr::getSingleton().loadSprite(strname);
		m_pBulding->setAnchorPoint(ccp(0.5f, 0));
		m_pBulding->retain();
		PlayerScene::getSingleton().m_pRoot->addChild(m_pBulding, SCENE_Z_BUILDING_BEGIN);

		//m_pBulding->setScale(0.5f);
	}
    
    if(buildingid / 100 == BUILDING_TOWN_HALL && buildingid % 100 >= 11)//只有老家11级以上才有动画
    {
        char strname[128];
        sprintf(strname, "%s_2_", m_info.res.c_str());

        CCAction* pAction = actionWithAnimate(strname, 20, 0.15f, false);
        CCDelayTime* pDelay = CCDelayTime::create(1.0f);
        
        m_pSprAction = CCRepeatForever::create(CCSequence::create((CCAnimate*)pAction, pDelay, NULL));
        m_pSprAction->retain();
        
        sprintf(strname, "%s_2_0", m_info.res.c_str());
        m_pSprAni = ImgMgr::getSingleton().loadSprite(strname);
        m_pSprAni->retain();
        m_pSprAni->setAnchorPoint(ccp(0.5f, 0));
        m_pSprAni->runAction(m_pSprAction);
        PlayerScene::getSingleton().m_pRoot->addChild(m_pSprAni, SCENE_Z_BUILDING_BEGIN);
    }

	Building::init(buildingid, lx, ly);
}

void Building_Normal::setLogicPos(int lx, int ly)
{
	Building::setLogicPos(lx, ly);

	if(m_isPreBuilding)
	{
		if(m_pBulding != NULL)
			m_pBulding->setVisible(false);

		if(m_pShadow != NULL)
			m_pShadow->setVisible(false);
        
	}
	else if(isEmptyHP())
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(false);

		if(m_pBulding != NULL)
			m_pBulding->setVisible(false);
        
        if(m_pSprAni != NULL)
            m_pSprAni->setVisible(false);
	}
	else
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(true);
        
		if(m_pBulding != NULL)
			m_pBulding->setVisible(true);
        
        if(m_pSprAni != NULL)
            m_pSprAni->setVisible(true);
	}

	//else
	{
		CCPoint pt = countSceneTilePt(lx, ly);

		if(m_pShadow != NULL)
			m_pShadow->setPosition(pt);

		if(m_pBulding != NULL)
			m_pBulding->setPosition(pt);
        
        if(m_pSprAni != NULL)
            m_pSprAni->setPosition(pt);
	}
}
void Building_Normal::onProc(int ot)
{
	Building::onProc(ot);
}

// 建筑建造状态
void Building_Normal::onBuilding(bool isBuilding)
{
    if (isBuilding) {
        if (m_pSprAni != NULL) {
            
            m_pSprAni->stopAction(m_pSprAction);
            m_pSprAni->setVisible(false);
        }
    }
    else {
        if (m_pSprAni != NULL) {
            m_pSprAni->setVisible(true);
        }
    }
}

void Building_Normal::releaseFromScene()
{
	if(m_pShadow != NULL)
	{
		PlayerScene::getSingleton().removeShadow(m_pShadow, false);
		//m_scene.m_pRoot->removeChild(m_pShadow, false);
		m_pShadow->release();
		m_pShadow = NULL;
	}

	if(m_pBulding != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pBulding, false);
		m_pBulding->release();
		m_pBulding = NULL;
	}
        
    if(m_pSprAni != NULL) {

        PlayerScene::getSingleton().m_pRoot->removeChild(m_pSprAni, false);
        m_pSprAni->release();
        m_pSprAni = NULL;
    }
    
    if (m_pSprAction != NULL) {
        m_pSprAction->release();
        m_pSprAction = NULL;
    }
    
	Building::releaseFromScene();
}

void Building_Normal::reorder(int zorder)
{
	Building::reorder(zorder);

	if(m_pBulding != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->reorderChild(m_pBulding, zorder);
	}
    
    if(m_pSprAni != NULL) {
        PlayerScene::getSingleton().m_pRoot->reorderChild(m_pSprAni, zorder + 2);
    }
}

void Building_Normal::chgSelMode(bool isSel)
{
	Building::chgSelMode(isSel);

	if(isSel)
	{
		Action_GrayFlash* pLightFlash = Action_GrayFlash::create(3, 128, 256);
		CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
		m_pSelAction = m_pBulding->runAction(pRF);

		setUpDownAction(m_pBulding, true);
        
        if(m_pSprAni != NULL) {
            Action_GrayFlash* pLightFlash = Action_GrayFlash::create(3, 128, 256);
            CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
            pRF->setTag(1);
            m_pSprAni->runAction(pRF);
            
            setUpDownAction(m_pSprAni, true);
        }
	}
	else
	{
		m_pBulding->stopAction(m_pSelAction);        
        m_pSelAction = NULL;

		ccColor3B c;
		c.r = c.g = c.b = 255;

		m_pBulding->setColor(c);
        
        if(m_pSprAni != NULL) {
            m_pSprAni->stopActionByTag(1);
            
            ccColor3B c;
            c.r = c.g = c.b = 255;
            
            m_pSprAni->setColor(c);
        }
	}
}

int Building_Normal::getHeight()
{
	if(m_pBulding != NULL)
		return m_pBulding->getContentSize().height;

	return Building::getHeight();
}

void Building_Normal::onBuildingEnd()
{
	if(m_isPreBuilding)
	{
		if(m_pShadow != NULL)
			m_pShadow->setVisible(true);

		if(m_pBulding != NULL)
			m_pBulding->setVisible(true);
	}
    
    if(m_pSprAni != NULL) {
        
        m_pSprAni->runAction(m_pSprAction);
        m_pSprAni->setVisible(true);
    }

	Building::onBuildingEnd();
}

void Building_Normal::setBeAttackMode()
{
	if(m_pActBeAttack != NULL)
		return;

	if(m_pBulding != NULL)
		setBeAttackAni(m_pBulding, true);

	if(m_pShadow != NULL)
		setBeAttackAni(m_pShadow, false);
    
    if (m_pSprAni) {
        m_pSprAni->stopAction(m_pSprAction);
        setBeAttackAni(m_pSprAni, true);
    }
}

//bool Building_Normal::buildOK()
//{
//}

NS_COC_END
