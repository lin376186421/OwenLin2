//
//  Building_ClanAllies.cpp
//  GameClient
//
//  Created by qsun on 14-4-22.
//
//

#include "Building_ClanAllies.h"
#include "BuildingMgr.h"
#include "PersonMgr.h"
#include "ImgMgr.h"
#include "PlayerScene.h"
#include "LogicInfo.h"
#include "ClanMgr.h"
#include "Action_LightFlash.h"
#include "Action_GrayFlash.h"

NS_COC_BEGIN

Building_ClanAllies::Building_ClanAllies(BuildingInfo info)
: Building(info)
, m_pBulding(NULL)
, m_pFlag(NULL)
{
	m_type = BUILDING_TYPE_CLAN;
}

void Building_ClanAllies::init(int buildingid, int lx, int ly)
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
    if (LogicInfo::getSingleton().isRealAttackMode()) {
        cid = ClanMgr::getSingleton().m_myclan.id;
        flag = ClanMgr::getSingleton().m_myclan.flag;
    }
    else if (LogicInfo::getSingleton().isReplayMode()) {
        cid = LogicInfo::getSingleton().m_logFightMsg.fsi.user[0].clanid;
        flag =  LogicInfo::getSingleton().m_logFightMsg.fsi.user[0].clanflag;
    }

    if(cid != 0)
    {
        char strname[128];
        sprintf(strname, "clan_icon_%d", flag);
        m_pFlag = ImgMgr::getSingleton().loadSprite(strname);
        m_pFlag->setAnchorPoint(ccp(0.5f, 0));
        m_pFlag->setScale(1.0f);
        m_pFlag->retain();
        PlayerScene::getSingleton().m_pRoot->addChild(m_pFlag, SCENE_Z_BUILDING_BEGIN);
    }
    
    for (vector<int>::iterator it = ClanMgr::getSingleton().m_fightAttackSoldier.begin(); it != ClanMgr::getSingleton().m_fightAttackSoldier.end(); it++) {
        m_lstSoldier.push_back(*it);
    }
    
    m_outSoldierTime = 500;
    
	Building::init(buildingid, lx, ly);
    
    m_pTile->setVisible(false);
}

void Building_ClanAllies::setLogicPos(int lx, int ly)
{
    Building::setLogicPos(lx, ly);
    
	if(m_isPreBuilding)
	{
		if(m_pBulding != NULL)
			m_pBulding->setVisible(false);
		
		if(m_pFlag != NULL)
			m_pFlag->setVisible(false);
	}
	else
	{
		if(m_pBulding != NULL)
			m_pBulding->setVisible(true);
        
		if(m_pFlag != NULL)
			m_pFlag->setVisible(true);
	}
    
	{
		CCPoint pt = countSceneTilePt(lx, ly);
        
		if(m_pBulding != NULL)
		{
			CCPoint cpt = pt;            
			m_pBulding->setPosition(cpt);
            
			if(m_pFlag != NULL)
			{
				cpt.y += 100;
				m_pFlag->setPosition(cpt);
			}
		}
	}
}

void Building_ClanAllies::releaseFromScene()
{
	if(m_pBulding != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pBulding, false);
		m_pBulding->release();
		m_pBulding = NULL;
	}
    
	if(m_pFlag != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pFlag, false);
		m_pFlag->release();
		m_pFlag = NULL;
	}
    
	Building::releaseFromScene();
}

void Building_ClanAllies::reorder(int zorder)
{
    Building::reorder(zorder);
    
	if(m_pBulding != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->reorderChild(m_pBulding, zorder);
	}
    
	if(m_pFlag != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->reorderChild(m_pFlag, zorder);
	}
}

void Building_ClanAllies::chgSelMode(bool isSel)
{
	Building::chgSelMode(isSel);
}

void Building_ClanAllies::onProc(int ot)
{
    Building::onProc(ot);
    
    if (m_lstSoldier.size() > 0)
    {
        if (m_outSoldierTime <= 0) {
            vector<int>::iterator it = m_lstSoldier.begin();
            
            if (it != m_lstSoldier.end()) {

                int sid = *it;
                
                int lx = getCenterPt().x;
                int ly = getCenterPt().y;
                
                PlayerScene::getSingleton().fixDropPosition(lx, ly);
                LogicInfo::getSingleton().dropSoldier2Fight(sid, lx, ly, m_indexBuilding);
                
                m_lstSoldier.erase(it);
            }
            
            m_outSoldierTime = 300;
        }
        
        m_outSoldierTime -= ot;
    }
}

void Building_ClanAllies::onBuildingEnd()
{
    
}

void Building_ClanAllies::setBeAttackMode()
{
    
}

void Building_ClanAllies::onBuilding(bool isBuilding)
{
    
}

Building_ClanAllies::~Building_ClanAllies()
{
}

NS_COC_END