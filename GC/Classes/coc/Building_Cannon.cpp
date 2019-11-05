#include "Building_Cannon.h"
#include "BuildingMgr.h"
#include "ImgMgr.h"
#include "PlayerScene.h"
#include "Action_LightFlash.h"
#include "Action_GrayFlash.h"
#include "Person.h"
#include "SimpleAudioEngine.h"
#include "SoundMgr.h"
#include "LogicInfo.h"
#include "ResConfig.h"

NS_COC_BEGIN

Building_Cannon::Building_Cannon(BuildingInfo info)
	: Building_Attack(info)
	, m_iDirEx(-1)
	, m_lastTurnTime(-1)
	, m_pCannon(NULL)
	, m_pCannonSelAction(NULL)
    , m_pShootAction(NULL)
{
	m_type = BUILDING_TYPE_CANNON;

	for(int i = 0; i < 7; ++i)
	{
		m_pFirstFrame[i] = NULL;
		m_pAnimate[i] = NULL;
	}
}

Building_Cannon::~Building_Cannon()
{
}

void Building_Cannon::init(int buildingid, int lx, int ly)
{
	char strname[128];

	for(int i = 0; i < 7; ++i)
	{
		CCAnimation* pAni = CCAnimation::create();

		int ct = i * m_info.connonFrameNums;
		sprintf(strname, "%s_2_%d", m_info.res.c_str(), ct);
		m_pFirstFrame[i] = ImgMgr::getSingleton().getFrame(strname);
		pAni->addSpriteFrame(m_pFirstFrame[i]);
		//m_pFirstFrame[i]->retain();

		pAni->addSpriteFrame(m_pFirstFrame[i]);

		//if(m_info.connonMainFrame == 0)
		//	m_pFirstFrame[i]->retain();

		for(int j = 0; j < m_info.connonFrameNums - 1; ++j)
		{
			ct = i * m_info.connonFrameNums + j + 1;
			sprintf(strname, "%s_2_%d", m_info.res.c_str(), ct);
			CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(strname);
			pAni->addSpriteFrame(pFrame);

			if(m_info.connonMainFrame - 1 == j)
				m_pFirstFrame[i] = pFrame;
		}

		m_pFirstFrame[i]->retain();

		pAni->setDelayPerUnit(0.08f);

		m_pAnimate[i] = CCAnimate::create(pAni);
		m_pAnimate[i]->retain();
	}

	m_pCannon = CCSprite::createWithSpriteFrame(m_pFirstFrame[0]);
	m_pCannon->setAnchorPoint(ccp(0.5, 0));
	m_pCannon->retain();
	PlayerScene::getSingleton().m_pRoot->addChild(m_pCannon, SCENE_Z_BUILDING_BEGIN);

    chgDirEx(rand() % 8);
    
	Building_Attack::init(buildingid, lx, ly);
}

void Building_Cannon::setLogicPos(int lx, int ly)
{
	Building_Attack::setLogicPos(lx, ly);

	CCPoint pt = countSceneTilePt(lx, ly);

	if(m_isPreBuilding)
	{
		if(m_pCannon != NULL)
			m_pCannon->setVisible(false);
	}
	else if(isEmptyHP())
	{
		if(m_pCannon != NULL)
			m_pCannon->setVisible(false);
	}
	else if(m_pCannon != NULL)
	{
		m_pCannon->setVisible(true);
	}

	if(m_pCannon != NULL)
	{
		m_pCannon->setPosition(pt);
	}
}

int Building_Cannon::getBuildingWidth()
{
    return m_pBulding->getTextureRect().size.width;
}

int Building_Cannon::getBuildingHeight()
{
    int off = 0;
    
	return m_pCannon->getOffsetPosition().y + m_pCannon->getTextureRect().size.height + off;
}

void Building_Cannon::releaseFromScene()
{
	for(int i = 0; i < 7; ++i)
	{
		if(m_pFirstFrame[i] != NULL)
		{
			m_pFirstFrame[i]->release();
			m_pFirstFrame[i] = NULL;
		}

		if(m_pAnimate[i] != NULL)
		{
			m_pAnimate[i]->release();
			m_pAnimate[i] = NULL;
		}
	}

	if(m_pCannon != NULL)
	{
		m_pCannon->release();

		PlayerScene::getSingleton().m_pRoot->removeChild(m_pCannon, true);

		m_pCannon = NULL;
	}

	Building_Attack::releaseFromScene();
}

void Building_Cannon::onBuildingEnd()
{
	if(m_isPreBuilding)
	{
		m_pCannon->setVisible(true);
	}

	Building_Attack::onBuildingEnd();
}

void Building_Cannon::reorder(int zorder)
{
	Building_Attack::reorder(zorder);

	if(m_pCannon != NULL)
	{
		PlayerScene::getSingleton().m_pRoot->reorderChild(m_pCannon, zorder + 1);
	}
}

CCPoint ptCannonOff[8] = {ccp(0, 85), ccp(38, 74), ccp(62, 58), ccp(76, 28), ccp(64, 0), ccp(42, -26), ccp(0, -32)};

void Building_Cannon::onStartAttack()
{
	int bid = m_buildingid / 100;

	CCPoint bPos = PlayerScene::getSingleton().countScenePos(getCenterPt().x, getCenterPt().y);
	CCPoint ePos = PlayerScene::getSingleton().countScenePos(m_pDestPerson->getLogicX(), m_pDestPerson->getLogicY());
	int dir = countDirExWithScreenPos(bPos.x, bPos.y, ePos.x, ePos.y);
	chgDirEx(dir);
	shoot();

    float bx = m_pCannon->getPositionX();
    float by = m_pCannon->getPositionY() + m_pCannon->getContentSize().height/2;

    dir = countDirExIndex(m_iDirEx);
    
    bool isFlip = false;
    if(m_iDirEx == DIREX_DOWN_LEFT || m_iDirEx == DIREX_LEFT_DOWN || m_iDirEx == DIREX_LEFT || m_iDirEx == DIREX_LEFT_UP || m_iDirEx == DIREX_UP_LEFT) {
        isFlip = true;
    }
    
    int damage = fixDamage(m_info.damage) * fixAttspeed(m_info.attackspeed) / 1000;
    
    if(BUILDING_CANNON == bid)
    {
        PlayerScene::getSingleton().addBoomEx("building101_atk", dir, bx, by, isFlip);
        
        if (!isFlip) {
            bx += ptCannonOff[dir].x;
        }
        else {
            bx -= ptCannonOff[dir].x;
        }
        by += ptCannonOff[dir].y;
        
        PlayerScene::getSingleton().addProjectile2("building101_bullet", bx, by, m_pDestPerson, 0, "building101_hit", false, false, true, 2000, "", 0.0f, true, false, damage, CAMP_DEFENCE, 0.0f, false);
    }
    else if(BUILDING_WIZARD_TOWER == bid)
    {
        PlayerScene::getSingleton().addBoomEx("building108_atk", dir, bx, by, isFlip);
        
        PlayerScene::getSingleton().addProjectile2("building108_bullet", bx, by, m_pDestPerson, 0, "building108_hit", false, false, true, 2000, "", 0.0f, true, true, damage, CAMP_DEFENCE, 1.0f, false);
    }

    SoundMgr::getSingleton().playEffect("building_attack_10101_0.mp3");
}

void Building_Cannon::onProc(int ot)
{
	Building_Attack::onProc(ot);

	if(!LogicInfo::getSingleton().isAttackMode())
	{
		m_lastTurnTime -= ot;

		if(m_lastTurnTime <= 0)
		{
			int dir = m_iDirEx;
			if(rand() % 2 == 0)
				++dir;
			else
				--dir;

			if(dir < 0)
				dir = 11;
			else if(dir > 11)
				dir = 0;

			chgDirEx(dir);

			m_lastTurnTime = rand() % 5000 + 1000;
		}
	}
}

int Building_Cannon::countDirExIndex(int direx)
{
	if(direx == DIREX_UP)
	{
		return 0;
	}
	else if(direx == DIREX_UP_RIGHT)
	{
		return 1;
	}
	else if(direx == DIREX_RIGHT_UP)
	{
		return 2;
	}
	else if(direx == DIREX_RIGHT)
	{
		return 3;
	}
	else if(direx == DIREX_RIGHT_DOWN)
	{
		return 4;
	}
	else if(direx == DIREX_DOWN_RIGHT)
	{
		return 5;
	}
	else if(direx == DIREX_DOWN)
	{
		return 6;
	}
	else if(direx == DIREX_DOWN_LEFT)
	{
		return 5;
	}
	else if(direx == DIREX_LEFT_DOWN)
	{
		return 4;
	}
	else if(direx == DIREX_LEFT)
	{
		return 3;
	}
	else if(direx == DIREX_LEFT_UP)
	{
		return 2;
	}
	else if(direx == DIREX_UP_LEFT)
	{
		return 1;
	}

	return 0;
}

void Building_Cannon::chgDirEx(int direx)
{
	if(direx == m_iDirEx)
		return ;

	m_iDirEx = direx;

	if(direx == DIREX_UP)
	{
		m_pCannon->setFlipX(false);
		m_pCannon->setDisplayFrame(m_pFirstFrame[0]);
	}
	else if(direx == DIREX_UP_RIGHT)
	{
		m_pCannon->setFlipX(false);
		m_pCannon->setDisplayFrame(m_pFirstFrame[1]);
	}
	else if(direx == DIREX_RIGHT_UP)
	{
		m_pCannon->setFlipX(false);
		m_pCannon->setDisplayFrame(m_pFirstFrame[2]);
	}
	else if(direx == DIREX_RIGHT)
	{
		m_pCannon->setFlipX(false);
		m_pCannon->setDisplayFrame(m_pFirstFrame[3]);
	}
	else if(direx == DIREX_RIGHT_DOWN)
	{
		m_pCannon->setFlipX(false);
		m_pCannon->setDisplayFrame(m_pFirstFrame[4]);
	}
	else if(direx == DIREX_DOWN_RIGHT)
	{
		m_pCannon->setFlipX(false);
		m_pCannon->setDisplayFrame(m_pFirstFrame[5]);
	}
	else if(direx == DIREX_DOWN)
	{
		m_pCannon->setFlipX(false);
		m_pCannon->setDisplayFrame(m_pFirstFrame[6]);
	}
	else if(direx == DIREX_DOWN_LEFT)
	{
		m_pCannon->setFlipX(true);
		m_pCannon->setDisplayFrame(m_pFirstFrame[5]);
	}
	else if(direx == DIREX_LEFT_DOWN)
	{
		m_pCannon->setFlipX(true);
		m_pCannon->setDisplayFrame(m_pFirstFrame[4]);
	}
	else if(direx == DIREX_LEFT)
	{
		m_pCannon->setFlipX(true);
		m_pCannon->setDisplayFrame(m_pFirstFrame[3]);
	}
	else if(direx == DIREX_LEFT_UP)
	{
		m_pCannon->setFlipX(true);
		m_pCannon->setDisplayFrame(m_pFirstFrame[2]);
	}
	else if(direx == DIREX_UP_LEFT)
	{
		m_pCannon->setFlipX(true);
		m_pCannon->setDisplayFrame(m_pFirstFrame[1]);
	}
}

int Building_Cannon::countDirExWithLogicPos(int bx, int by, int ex, int ey)
{
	CCPoint bPos = PlayerScene::getSingleton().countScenePos(bx, by);
	CCPoint ePos = PlayerScene::getSingleton().countScenePos(ex, ey);

	if(bPos.x < ePos.x)
	{
		if(bPos.y > ePos.y)
		{
			if(fabs(ePos.x - bPos.x) > fabs(ePos.y - bPos.y))
				return DIREX_RIGHT_DOWN;
			else
				return DIREX_DOWN_RIGHT;
		}
		else if(bPos.y == ePos.y)
			return DIREX_RIGHT;
		else
		{
			if(fabs(ePos.x - bPos.x) > fabs(ePos.y - bPos.y))
				return DIREX_RIGHT_UP;
			else
				return DIREX_UP_RIGHT;
		}
	}
	else if(bPos.x == ePos.x)
	{
		if(bPos.y > ePos.y)
			return DIREX_DOWN;
		else if(bPos.y < ePos.y)
			return DIREX_UP;
	}
	else
	{
		if(bPos.y > ePos.y)
		{
			if(fabs(ePos.x - bPos.x) > fabs(ePos.y - bPos.y))
				return DIREX_LEFT_DOWN;
			else
				return DIREX_DOWN_LEFT;
		}
		else if(bPos.y == ePos.y)
			return DIREX_LEFT;
		else
		{
			if(fabs(ePos.x - bPos.x) > fabs(ePos.y - bPos.y))
				return DIREX_LEFT_UP;
			else
				return DIREX_UP_LEFT;
		}
	}

	return m_iDirEx;
}

int Building_Cannon::countDirExWithScreenPos(int bx, int by, int ex, int ey)
{
	int sx = ex - bx;
	int sy = ey - by;
	float degree = 0;

	if(sx == 0 && sy == 0)
		return m_iDirEx;

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

	if(degree >= 0 && degree <= 15)
		return DIREX_UP;
	else if(degree > 15 && degree <= 45)
		return DIREX_UP_RIGHT;
	else if(degree > 45 && degree <= 75)
		return DIREX_RIGHT_UP;
	else if(degree > 75 && degree <= 105)
		return DIREX_RIGHT;
	else if(degree > 105 && degree <= 135)
		return DIREX_RIGHT_DOWN;
	else if(degree > 135 && degree <= 165)
		return DIREX_DOWN_RIGHT;
	else if(degree > 165 && degree <= 180)
		return DIREX_DOWN;

	degree = -degree;
	if(degree >= 0 && degree <= 15)
		return DIREX_UP;
	else if(degree > 15 && degree <= 45)
		return DIREX_UP_LEFT;
	else if(degree > 45 && degree <= 75)
		return DIREX_LEFT_UP;
	else if(degree > 75 && degree <= 105)
		return DIREX_LEFT;
	else if(degree > 105 && degree <= 135)
		return DIREX_LEFT_DOWN;
	else if(degree > 135 && degree <= 165)
		return DIREX_DOWN_LEFT;
	else if(degree > 165 && degree <= 180)
		return DIREX_DOWN;

	return m_iDirEx;
}

void Building_Cannon::shoot()
{
    if(m_pShootAction != NULL && !m_pShootAction->isDone())
        return;
    
    if(m_pShootAction != NULL && m_pShootAction->isDone())
    {
        m_pShootAction->release();
        
        m_pShootAction = NULL;
    }
        
	int index = 0;

	if(m_iDirEx == DIREX_UP)
	{
		index = 0;
	}
	else if(m_iDirEx == DIREX_UP_RIGHT || m_iDirEx == DIREX_UP_LEFT)
	{
		index = 1;
	}
	else if(m_iDirEx == DIREX_RIGHT_UP || m_iDirEx == DIREX_LEFT_UP)
	{
		index = 2;
	}
	else if(m_iDirEx == DIREX_RIGHT || m_iDirEx == DIREX_LEFT)
	{
		index = 3;
	}
	else if(m_iDirEx == DIREX_RIGHT_DOWN || m_iDirEx == DIREX_LEFT_DOWN)
	{
		index = 4;
	}
	else if(m_iDirEx == DIREX_DOWN_RIGHT || m_iDirEx == DIREX_DOWN_LEFT)
	{
		index = 5;
	}
	else if(m_iDirEx == DIREX_DOWN)
	{
		index = 6;
	}

	m_pShootAction = m_pCannon->runAction(m_pAnimate[index]);
    m_pShootAction->retain();
}

void Building_Cannon::setBeAttackMode()
{
	if(m_pActBeAttack != NULL)
		return ;

	if(m_pBulding != NULL)
		setBeAttackAni(m_pBulding, true, true);

	if(m_pShadow != NULL)
		setBeAttackAni(m_pShadow, false);
}

void Building_Cannon::chgSelMode(bool isSel)
{
	Building_Attack::chgSelMode(isSel);

	if(isSel)
	{
		Action_GrayFlash* pLightFlash = Action_GrayFlash::create(3, 128, 256);
		CCRepeatForever* pRF = CCRepeatForever::create(pLightFlash);
		//m_pSelAction = m_pBulding->runAction(pRF);

		//setUpDownAction(m_pBulding, true);

		//pLightFlash = Action_GrayFlash::create(3, 128, 256);
		//pRF = CCRepeatForever::create(pLightFlash);
		m_pCannonSelAction = m_pCannon->runAction(pRF);

		setUpDownAction(m_pCannon, true);
	}
	else
	{
		//m_pBulding->stopAction(m_pSelAction);
//        m_pSelAction = NULL;

		ccColor3B c;
		//c.r = c.g = c.b = 255;

		//m_pBulding->setColor(c);

		m_pCannon->stopAction(m_pCannonSelAction);
        m_pCannonSelAction = NULL;
        
		c.r = c.g = c.b = 255;

		m_pCannon->setColor(c);
	}
}

NS_COC_END