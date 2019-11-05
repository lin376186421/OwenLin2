//
//  Building_Clan.cpp
//  GameClient
//
//  Created by qsun on 13-9-16.
//
//

#include "Building_Clan.h"
#include "Person.h"
#include "BuildingMgr.h"
#include "ImgMgr.h"
#include "PlayerScene.h"
#include "LogicInfo.h"
#include "ClanMgr.h"
#include "Action_LightFlash.h"
#include "Action_GrayFlash.h"

NS_COC_BEGIN

Building_Clan::Building_Clan(BuildingInfo info)
: Building(info)
, m_pBulding(NULL)
, m_pShadow(NULL)
, m_pSelAction(NULL)
, m_pClanIcon(NULL)
, m_pClanName(NULL)
, m_isDefenseActive(false)
, m_outSoldierTime(0)
{
	m_type = BUILDING_TYPE_CLAN;
}

void Building_Clan::init(int buildingid, int lx, int ly)
{
	{
		char strname[128];
		sprintf(strname, "%s_2", m_info.res.c_str());
		m_pBulding = ImgMgr::getSingleton().loadSprite(strname);
		m_pBulding->setAnchorPoint(ccp(0.5f, 0));
		m_pBulding->retain();
		PlayerScene::getSingleton().m_pRoot->addChild(m_pBulding, SCENE_Z_BUILDING_BEGIN);
	}
    
    int cid = 0;
    int flag = 0;
    
    std::string name;
    
    if(LogicInfo::getSingleton().isNormalMode())
    {
        cid = ClanMgr::getSingleton().m_myclan.id;
        flag = ClanMgr::getSingleton().m_myclan.flag;
        name = ClanMgr::getSingleton().m_myclan.name;
    }
    else
    {
        cid = ClanMgr::getSingleton().m_otherclan.id;
        flag = ClanMgr::getSingleton().m_otherclan.flag;
        name = ClanMgr::getSingleton().m_otherclan.name;
    }
    
    if(cid != 0)
    {
        char strname[128];
        sprintf(strname, "clan_icon_%d", flag);
        m_pClanIcon = ImgMgr::getSingleton().loadSprite(strname);
        m_pClanIcon->setAnchorPoint(ccp(0.5f, 0));
        m_pClanIcon->setScale(1.0f);
        m_pClanIcon->retain();
        PlayerScene::getSingleton().m_pRoot->addChild(m_pClanIcon, SCENE_Z_BUILDING_BEGIN);
        
        m_pClanName = CCLabelTTF::create(name.c_str(), "Font/Candal.ttf", 32);
        m_pClanName->setColor(ccc3(255, 255, 198));
        m_pClanName->setScale(0.7);
        m_pClanName->setAnchorPoint(ccp(0.5, 0));
        m_pClanName->setVisible(true);
        m_pClanName->retain();
        PlayerScene::getSingleton().m_pRoot->addChild(m_pClanName, SCENE_Z_BUILDINGUI);
    }
		
	{
		char strname[128];
		sprintf(strname, "area_white");
		m_pLightArea = ImgMgr::getSingleton().loadSprite(strname);
		//m_pLightArea->setAnchorPoint(ccp(0, 0));
		m_pLightArea->retain();
		PlayerScene::getSingleton().m_pRoot->addChild(m_pLightArea, SCENE_Z_HUAN);

		Action_LightFlash* pLightFlash = Action_LightFlash::create(3, 156, 256);
		CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
		m_pLightArea->runAction(pRF);

		m_pLightArea->setScale(m_info.attackrange * 2 / 500.0f);

		m_pLightArea->setVisible(false);
	}
    
	Building::init(buildingid, lx, ly);
}

void Building_Clan::setLogicPos(int lx, int ly)
{
    Building::setLogicPos(lx, ly);
    
	if(m_isPreBuilding)
	{
		if(m_pBulding != NULL)
			m_pBulding->setVisible(false);
		
		if(m_pClanIcon != NULL)
			m_pClanIcon->setVisible(false);
        
        if (m_pClanName != NULL) {
            m_pClanName->setVisible(false);
        }
	}
	else if(isEmptyHP())
	{
		if(m_pBulding != NULL)
			m_pBulding->setVisible(false);

		if(m_pClanIcon != NULL)
			m_pClanIcon->setVisible(false);

        if (m_pClanName != NULL) {
            m_pClanName->setVisible(false);
        }
        
		if(m_pLightArea != NULL)
			m_pLightArea->setVisible(false);
	}
	else
	{
		if(m_pBulding != NULL)
			m_pBulding->setVisible(true);

		if(m_pClanIcon != NULL)
			m_pClanIcon->setVisible(true);
        
        if (m_pClanName != NULL) {
            m_pClanName->setVisible(true);
        }
	}
    
	//else
	{
		CCPoint pt = countSceneTilePt(lx, ly);
        
		if(m_pBulding != NULL)
		{
            CCPoint cpt = pt;            
			m_pBulding->setPosition(cpt);

			if(m_pClanIcon != NULL)
			{
				cpt.y += 160;
				m_pClanIcon->setPosition(cpt);
			}

			if(m_pClanName != NULL)
			{
				m_pClanName->setPosition(cpt);
			}
		}

		if(m_pLightArea != NULL)
		{
			CCPoint pt = countSceneTilePt(lx, ly);
			
            pt.x += m_pTile->getContentSize().width / 2;
            pt.y += m_pTile->getContentSize().height / 2;

            m_pLightArea->setPosition(pt);
		}
	}
}

void Building_Clan::releaseFromScene()
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

	if(m_pClanIcon != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pClanIcon, false);
		m_pClanIcon->release();
		m_pClanIcon = NULL;
	}

	if(m_pLightArea != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pLightArea, true);
		m_pLightArea->release();
		m_pLightArea = NULL;
	}

	if(m_pClanName != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pClanName, true);
		m_pClanName->release();
		m_pClanName = NULL;
	}

	Building::releaseFromScene();
}

void Building_Clan::reorder(int zorder)
{
    Building::reorder(zorder);
    
	if(m_pBulding != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->reorderChild(m_pBulding, zorder);
	}

	if(m_pClanIcon != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->reorderChild(m_pClanIcon, zorder);
	}
    
    if(m_pClanName != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->reorderChild(m_pClanName, zorder);
	}
}

void Building_Clan::chgSelMode(bool isSel)
{
	Building::chgSelMode(isSel);

	if(isSel)
	{
		Action_GrayFlash* pLightFlash = Action_GrayFlash::create(3, 128, 256);
		CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
		m_pSelAction = m_pBulding->runAction(pRF);

		setUpDownAction(m_pBulding, true);
	}
	else
	{
		m_pBulding->stopAction(m_pSelAction);
        m_pSelAction = NULL;
        
		ccColor3B c;
		c.r = c.g = c.b = 255;

		m_pBulding->setColor(c);
	}

#if !COC_CLAN
    if (m_pLabelLevel) {
        m_pLabelLevel->setString("coming soon...");
    }
#endif
}

void Building_Clan::onProc(int ot)
{
    Building::onProc(ot);
    
	if(LogicInfo::getSingleton().isAttackMode())
	{
		if (m_outSoldierTime <= 0) {

            int lx = getLogicX();
            int ly = getLogicY();
            
            if (!m_isDefenseActive) {
                Person* pPerson = PlayerScene::getSingleton().findNearPerson(lx, ly, 0, m_info.attackrange/LOGIC_METER, 2, CAMP_ATTACK);
                
                if (pPerson) {
                    m_isDefenseActive = true;
                }
            }
            else if (ClanMgr::getSingleton().m_otherListSoldier.size() > 0) {
                
                vector<int>::iterator it = ClanMgr::getSingleton().m_otherListSoldier.begin();
                
                if (it != ClanMgr::getSingleton().m_otherListSoldier.end()) {
                    
                    int sid = *it;
                    
                    Person* pPerson = PlayerScene::getSingleton().addDefenceSoldier(this, sid, 0);
                    pPerson->addStatus(PERSON_STATUS_DEFENSE_ACTIVE);
                    
                    LogicInfo::getSingleton().onAddCtrl_ClanSoldierDefend(m_indexBuilding, sid, 0);
                    
                    ClanMgr::getSingleton().m_otherListSoldier.erase(it);
                }
            }
            
            m_outSoldierTime = 500;
        }
        
        m_outSoldierTime -= ot;
	}
}

void Building_Clan::onBuildingEnd()
{
    
}

void Building_Clan::setBeAttackMode()
{
    
}

void Building_Clan::onBuilding(bool isBuilding)
{
    
}

void Building_Clan::updateClan(int cid, int flag, const char* name)
{
	if(cid != 0)
	{
		char strname[128];
		sprintf(strname, "clan_icon_%d", flag);
        
        if (m_pClanIcon == NULL) {
            m_pClanIcon = ImgMgr::getSingleton().loadSprite(strname);
            m_pClanIcon->setAnchorPoint(ccp(0.5f, 0));
            m_pClanIcon->setScale(1.0f);
            m_pClanIcon->retain();
            PlayerScene::getSingleton().m_pRoot->addChild(m_pClanIcon, SCENE_Z_BUILDING_BEGIN);
        }
		m_pClanIcon->setDisplayFrame(ImgMgr::getSingleton().getFrame(strname));
        
        if(m_pClanName == NULL)
        {
            m_pClanName = CCLabelTTF::create("", "Font/Candal.ttf", 32);
            m_pClanName->setColor(ccc3(255, 255, 198));
            m_pClanName->setScale(0.7);
            m_pClanName->setAnchorPoint(ccp(0.5, 0));
            m_pClanName->setVisible(true);
            m_pClanName->retain();
            PlayerScene::getSingleton().m_pRoot->addChild(m_pClanName, SCENE_Z_BUILDINGUI);
        }
        
        m_pClanName->setString(name);
	}
	else
	{
		if(m_pClanIcon != NULL)
		{
			PlayerScene::getSingleton().m_pRoot->removeChild(m_pClanIcon, false);
			m_pClanIcon->release();
			m_pClanIcon = NULL;
		}
        
        if(m_pClanName != NULL)
        {
            PlayerScene::getSingleton().m_pRoot->removeChild(m_pClanName, true);
            m_pClanName->release();
            m_pClanName = NULL;
        }
	}

	setLogicPos(m_lx, m_ly);
}

void Building_Clan::setClanCD(int fulltime, int lasttime)
{
	if(m_pBuildingInfo->state != 2)
		return ;

	setTime(fulltime, lasttime);
	setLogicPos(getLogicX(), getLogicY());
}

Building_Clan::~Building_Clan()
{
}

NS_COC_END