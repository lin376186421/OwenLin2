#include "Building_Wall.h"
#include "PlayerScene.h"
#include "ImgMgr.h"
#include "Action_GrayFlash.h"
#include "HttpClient.h"
#include "BuildingMgr.h"
#include "../GameWorldScene.h"
#include "../form/MainEditUI.h"

NS_COC_BEGIN

Building_Wall::Building_Wall(BuildingInfo info)
	: Building(info)
	, m_pUp(NULL)
	, m_pRight(NULL)
	, m_pBulding(NULL)
	, m_pSelAction(NULL)
	, m_pWallRight(NULL)
	, m_pWallUp(NULL)
	, m_pWallLeft(NULL)
	, m_pWallDown(NULL)
	, m_pCmb(NULL)
	, m_pBatchNode(NULL)
    , m_isEditWall(false)
    , m_isEditMove(false)
{
	m_type = BUILDING_TYPE_WALL;
    
    for (int i = 0; i < 8; i ++) {
        m_pWallEdit[i] = NULL;
    }
}

Building_Wall::~Building_Wall()
{
}

void Building_Wall::init(int buildingid, int lx, int ly)
{
	m_pBatchNode = ImgMgr::getSingleton().addBatchNode("building003");//CCSpriteBatchNode::create("building001.pvr.ccz");
	m_pBatchNode->setPosition(0.5f, 0.0f);
	m_pBatchNode->retain();

	PlayerScene::getSingleton().m_pRoot->addChild(m_pBatchNode, SCENE_Z_BUILDING_BEGIN);

	{
		char strname[128];
		sprintf(strname, "%s_2_2", m_info.res.c_str());
		m_pUp = ImgMgr::getSingleton().loadSprite(strname);
		m_pUp->setAnchorPoint(ccp(0.5, 0));
		m_pUp->setVisible(false);
		m_pUp->retain();

		m_pBatchNode->addChild(m_pUp, 0);
	}

	{
		char strname[128];
		sprintf(strname, "%s_3_2", m_info.res.c_str());
		m_pRight = ImgMgr::getSingleton().loadSprite(strname);
		m_pRight->setAnchorPoint(ccp(0.5, 0));
		m_pRight->setVisible(false);
		m_pRight->retain();

		m_pBatchNode->addChild(m_pRight, 0);
	}

	{
		char strname[128];
		sprintf(strname, "%s_1_2", m_info.res.c_str());
		m_pBulding = ImgMgr::getSingleton().loadSprite(strname);
		m_pBulding->setAnchorPoint(ccp(0.5, 0));
		m_pBulding->retain();

		m_pBatchNode->addChild(m_pBulding, 1);
	}

	Building::init(buildingid, lx, ly);
}

void Building_Wall::setLogicPos(int lx, int ly)
{
    if (m_state != BUILDING_CTRL_MOVE) {
        if (lx != getLogicX() || ly != getLogicY()) {
            rebuildWall();
        }
    }
    
	Building::setLogicPos(lx, ly);

	if(isEmptyHP())
	{
		m_pBulding->setVisible(false);

		if(m_pUp != NULL)
			m_pUp->setVisible(false);

		if(m_pRight != NULL)
			m_pRight->setVisible(false);
	}
	else
	{
        if (!m_pBulding->isVisible()) {
            
            rebuildWall();
            m_pBulding->setVisible(true);
        }
        
		if(m_state == BUILDING_CTRL_MOVE)
		{
			if(lx != m_lbx || ly != m_lby)
			{
				if(m_pUp != NULL)
					m_pUp->setVisible(false);

				if(m_pRight != NULL)
					m_pRight->setVisible(false);
                
                if (PlayerScene::getSingleton().m_isEditMode) {
                    for (int i = 0; i < 8; i ++) {
                        if (m_pWallEdit[i]) {
                            m_pWallEdit[i]->releaseFromScene();
                            m_pWallEdit[i] = NULL;
                        }
                    }
                }
			}
		}
	}
    
    CCPoint pt = countSceneTilePt(lx, ly);
    
    if(m_pUp != NULL)
        m_pUp->setPosition(pt);
    
    if(m_pRight != NULL)
        m_pRight->setPosition(pt);
    
    if(m_pBulding != NULL)
        m_pBulding->setPosition(pt);
}

void Building_Wall::rebuildWall(Building_Wall *pWall)
{
    if (m_isEditWall) {
        return;
    }
    
    m_wallDirNums = 0;
    
    if (m_pWallLeft) {
        
        m_pWallLeft->m_pRight->setVisible(false);
        
        m_pWallLeft->m_pWallRight = NULL;
        m_pWallLeft->m_wallDirNums --;
        
        m_pWallLeft = NULL;
    }
    
    if (m_pWallRight) {
        
        m_pWallRight->m_pWallLeft = NULL;
        m_pWallRight->m_wallDirNums --;
        
        m_pWallRight = NULL;
    }
    
    if (m_pWallUp) {
        m_pWallUp->m_pWallDown = NULL;
        m_pWallUp->m_wallDirNums --;
        
        m_pWallUp = NULL;
    }
    
    if (m_pWallDown) {
        
        m_pWallDown->m_pUp->setVisible(false);
        
        m_pWallDown->m_pWallUp = NULL;
        m_pWallDown->m_wallDirNums --;
        
        m_pWallDown = NULL;
    }
    
    m_pUp->setVisible(false);
    m_pRight->setVisible(false);
    
    if (!isEmptyHP() && !m_isRemove) {
        
        Building_Wall* pWallLeft = NULL;
        if (pWall) {
            if (pWall->getLogicX() == getLogicX() + 2) {
                pWallLeft = pWall;
            }
        }
        else {
             pWallLeft = PlayerScene::getSingleton().isWall(getLogicX() + 2, getLogicY());
        }
        
        if (pWallLeft != NULL && pWallLeft->m_state == m_state) {
            
            m_pWallLeft = pWallLeft;
            m_wallDirNums ++;
            
            m_pWallLeft->m_pRight->setVisible(true);
            
            m_pWallLeft->m_pWallRight = this;
            m_pWallLeft->m_wallDirNums ++;
        }
        
        Building_Wall* pWallRight = NULL;
        if (pWall) {
            if (pWall->getLogicX() == getLogicX() - 2) {
                pWallRight = pWall;
            }
        }
        else {
            pWallRight = PlayerScene::getSingleton().isWall(getLogicX() - 2, getLogicY());
        }
        
        if(pWallRight != NULL && pWallRight->m_state == m_state)
        {
            m_pWallRight = pWallRight;
            m_wallDirNums ++;
            
            m_pRight->setVisible(true);
            
            m_pWallRight->m_pWallLeft = this;
            m_pWallRight->m_wallDirNums ++;
        }
        
        Building_Wall* pWallUp = NULL;
        if (pWall) {
            if (pWall->getLogicY() == getLogicY() - 2) {
                pWallUp = pWall;
            }
        }
        else {
            pWallUp = PlayerScene::getSingleton().isWall(getLogicX(), getLogicY() - 2);
        }
        
        if(pWallUp != NULL && pWallUp->m_state == m_state)
        {
            m_pWallUp = pWallUp;
            m_wallDirNums ++;
            
            m_pUp->setVisible(true);
            
            m_pWallUp->m_pWallDown = this;
            m_pWallUp->m_wallDirNums ++;
        }
        
        Building_Wall* pWallDown = NULL;
        if (pWall) {
            if (pWall->getLogicY() == getLogicY() + 2) {
                pWallDown = pWall;
            }
        }
        else {
            pWallDown = PlayerScene::getSingleton().isWall(getLogicX(), getLogicY() + 2);
        }
        
        if (pWallDown != NULL && pWallDown->m_state == m_state) {
            
            m_pWallDown = pWallDown;
            m_wallDirNums ++;
            
            m_pWallDown->m_pUp->setVisible(true);
            
            m_pWallDown->m_pWallUp = this;
            m_pWallDown->m_wallDirNums ++;
        }
    }
}

void Building_Wall::chgHP(int off, bool flyfont, bool isReal)
{
    Building::chgHP(off, flyfont, isReal);
    
    if (isEmptyHP()) {
        rebuildWall();
    }
}

bool Building_Wall::buildOK()
{
	if(Building::buildOK())
	{
		rebuildWall();
        
		return true;
	}

	return false;
}

//! 整排修墙
bool Building_Wall::buildOKLine()
{
	chgSelMode(false);

	if(PlayerScene::getSingleton().canBuildingLine(10301, getLogicX(), getLogicY(), m_info.width))
	{
		HttpClient::getSingleton().move(m_pBuildingInfo->id, m_lbx, m_lby, getLogicX(), getLogicY());

		if(m_pTileColor != NULL)
		{
			m_pTileColor->setVisible(false);

			m_pTileColor = NULL;
		}

		if(m_pBuildingInfo != NULL)
		{
			m_pBuildingInfo->x = getLogicX();
			m_pBuildingInfo->y = getLogicY();
		}

		chgState(BUILDING_CTRL_NORMAL);

		return true;
	}

	return false;
}

//! 整排墙的设置Tile
void Building_Wall::setTileLine(bool show, int index)
{
    if (show) {
        m_pTileColor = PlayerScene::getSingleton().getRedTileLine(index);
        
        if(m_pTileColor != NULL)
        {
            CCPoint pt = countSceneTilePt(getLogicX(), getLogicY());
            
            m_pTileColor->setPosition(pt);
            m_pTileColor->setVisible(show);
            
            m_pTileColor->setVisible(true);
        }
    }
    else {
        
        if (m_pTileColor) {
            
            m_pTileColor->setVisible(false);
            m_pTileColor = NULL;
        }
    }
}

//! 整排墙复位
void Building_Wall::resetWallLine()
{
	chgSelMode(false);
    setLogicPos(m_lbx, m_lby);
    
    m_pWallLeft = NULL;
    m_pWallRight = NULL;
    m_pWallUp = NULL;
    m_pWallDown = NULL;

	if(m_pTileColor != NULL)
	{
		m_pTileColor->setVisible(false);
		m_pTileColor = NULL;
	}

	if(m_pBuildingInfo != NULL)
	{
		m_pBuildingInfo->x = getLogicX();
		m_pBuildingInfo->y = getLogicY();
	}
    
    chgState(BUILDING_CTRL_NORMAL);
}

void Building_Wall::reorder(int zorder)
{
	Building::reorder(zorder);

	if(m_pBatchNode != NULL)
		PlayerScene::getSingleton().m_pRoot->reorderChild(m_pBatchNode, zorder);
}

void Building_Wall::releaseFromScene()
{
    m_isRemove = true;

    rebuildWall();
    
	if(m_pUp != NULL)
	{
		m_pBatchNode->removeChild(m_pUp, false);
		m_pUp = NULL;
	}

	if(m_pRight != NULL)
	{
		m_pBatchNode->removeChild(m_pRight, false);
		m_pRight = NULL;
	}
    
	if(m_pBulding != NULL)
	{
        m_pBulding->stopAllActions();
        
		m_pBatchNode->removeChild(m_pBulding, false);
		m_pBulding = NULL;
	}

	if(m_pBatchNode != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->removeChild(m_pBatchNode, false);
		m_pBatchNode->release();
		m_pBatchNode = NULL;
	}
    
    for (int i = 0; i < 8; i ++) {
        
        if (m_pWallEdit[i]) {
            m_pWallEdit[i]->releaseFromScene();
            m_pWallEdit[i] = NULL;
        }
    }

	Building::releaseFromScene();
}

void Building_Wall::chgSelMode(bool isSel)
{
	Building::chgSelMode(isSel);

	if(m_pSelAction != NULL)
	{
		m_pBulding->stopAction(m_pSelAction);
		m_pSelAction = NULL;
	}

	if(isSel)
	{
		Action_GrayFlash* pLightFlash = Action_GrayFlash::create(3, 128, 256);
		CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
		m_pSelAction = m_pBulding->runAction(pRF);

		setUpDownAction(m_pBulding, false);
        
        if (PlayerScene::getSingleton().m_isEditMode) {
            
            m_isEditMove = false;
            buildEditWall();
        }
    }
	else
	{
		m_pBulding->stopAction(m_pSelAction);
        m_pSelAction = NULL;
        
		ccColor3B c;
		c.r = c.g = c.b = 255;

		m_pBulding->setColor(c);
        
        m_isEditMove = false;
        for (int i = 0; i < 8; i ++) {

            if (m_pWallEdit[i]) {
                m_pWallEdit[i]->releaseFromScene();
                m_pWallEdit[i] = NULL;
            }
        }
	}
}

void Building_Wall::chgSelModeLine(bool isSel)
{
	if(m_pSelAction != NULL)
	{
		m_pBulding->stopAction(m_pSelAction);
		m_pSelAction = NULL;
	}

	{
		if(m_pLabelName != NULL)
		{
			m_pLabelName->setVisible(false);
			m_pLabelName->release();
			PlayerScene::getSingleton().m_pRoot->removeChild(m_pLabelName);
            m_pLabelName = NULL;
		}
        if (m_pLabelLevel != NULL)
		{
            m_pLabelLevel->setVisible(false);
			m_pLabelLevel->release();
			PlayerScene::getSingleton().m_pRoot->removeChild(m_pLabelLevel);
            m_pLabelLevel = NULL;
		}
	}

	if(isSel)
	{
		Action_GrayFlash* pLightFlash = Action_GrayFlash::create(3, 128, 256);
		CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
		m_pSelAction = m_pBulding->runAction(pRF);

		setUpDownAction(m_pBulding, false);
	}
	else
	{
		m_pBulding->stopAction(m_pSelAction);
        m_pSelAction = NULL;
        
		ccColor3B c;
		c.r = c.g = c.b = 255;

		m_pBulding->setColor(c);
	}

	setLogicPos(getLogicX(), getLogicY());
}

void Building_Wall::buildEditWall()
{
    if (PlayerScene::getSingleton().m_pGameWorld->m_pMainEditUI->getBuildingCount(m_buildingid) == 0) {
        return;
    }
    
    CCPoint pos[8] = {ccp(0, -2), ccp(0, -4), ccp(-2, 0), ccp(-4, 0), ccp(0, 2), ccp(0, 4), ccp(2, 0), ccp(4, 0)};
    
    for (int i = 0; i < 8; i++) {
        
        bool canBuild = PlayerScene::getSingleton().isValidBuildingPosEx(m_buildingid, getLogicX() + pos[i].x, getLogicY() + pos[i].y, m_info.width);
        
        if ((i % 2 != 0) && canBuild) {
            canBuild = PlayerScene::getSingleton().isValidBuildingPosEx(m_buildingid, getLogicX() + pos[(i/2)*2].x, getLogicY() + pos[(i/2)*2].y, m_info.width);
        }
        
        if (canBuild && m_pWallEdit[i] == NULL) {
            
            m_pWallEdit[i] = (Building_Wall*)BuildingMgr::getSingleton().newBuilding(m_buildingid);
            m_pWallEdit[i]->init(m_buildingid, getLogicX() + pos[i].x, getLogicY() + pos[i].y);
            m_pWallEdit[i]->m_isEditWall = true;
            CCSprite* pSprite[3] = {NULL, NULL, m_pWallEdit[i]->m_pBulding};
            
            if (i == 1 && m_pWallEdit[0] != NULL) {
                pSprite[0] = m_pWallEdit[0]->m_pUp;
            }
            else if (i == 3 && m_pWallEdit[2] != NULL) {
                pSprite[1] = m_pWallEdit[2]->m_pRight;
            }
            else if (i == 5 && m_pWallEdit[4] != NULL) {
                pSprite[0] = m_pWallEdit[5]->m_pUp;
            }
            else if (i == 7 && m_pWallEdit[6] != NULL) {
                pSprite[1] = m_pWallEdit[7]->m_pRight;
            }
            
            for (int i = 0; i < 3; i ++) {
                
                if (pSprite[i] != NULL) {
                    
                    pSprite[i]->setOpacity(120);
                    pSprite[i]->setVisible(true);
                    
                    float scale = pSprite[i]->getScale();
                    CCScaleTo* pScaleIn = CCScaleTo::create(0.5, scale + 0.05);
                    CCScaleTo* pScaleOut = CCScaleTo::create(0.5, scale - 0.05);
                    CCRepeatForever* pRepeatAction1 = CCRepeatForever::create(CCSequence::create(pScaleIn, pScaleOut, CCDelayTime::create(0), NULL));

                    CCFadeTo* pFadeIn = CCFadeTo::create(0.5, 60);
                    CCFadeTo* pFadeOut = CCFadeTo::create(0.5, 120);
                    CCRepeatForever* pRepeatAction2 = CCRepeatForever::create(CCSequence::create(pFadeIn, pFadeOut, CCDelayTime::create(0), NULL));

                    pSprite[i]->runAction(pRepeatAction1);
                    pSprite[i]->runAction(pRepeatAction2);
                }
            }
        }
    }
    
    PlayerScene::getSingleton().showArrow(getLogicX(), getLogicY(), getLogicWidth());
}

void Building_Wall::setBeAttackMode()
{
	if(m_pActBeAttack != NULL)
		return;

	if(m_pBulding != NULL)
		setBeAttackAni(m_pBulding, true);
}

int Building_Wall::getWallDirNums()
{
    if (m_wallDirNums == 0) {
        return 0;
    }
    
    if (m_wallDirNums >= 3) {
        return 3;
    }
    
    if ((m_pWallLeft || m_pWallRight) && (m_pWallUp || m_pWallDown)) {
        return 2;
    }
    
    return 1;
}

NS_COC_END
