#include <stdlib.h>
#include "PlayerScene.h"
#include "BuildingMgr.h"
#include "Person.h"
#include "PersonMgr.h"
#include "ImgMgr.h"
#include "LogicInfo.h"
#include "HttpClient.h"
#include "ParticleSystem.h"
#include "BuildingMgr.h"
#include "Projectile.h"
#include "SoundMgr.h"
#include "AniObjMgr.h"
#include "GameLayer.h"
#include "HeroMgr.h"
#include "SkillMgr.h"
#include "SoundMgr.h"
#include "GuideMgr.h"
#include "ConfigValMgr.h"
#include "EnvMapMgr.h"
#include "ClanMgr.h"
#include "ClanWarMgr.h"
#include "ResConfig.h"
#include "AniObjMgr.h"
#include "AniObj_Normal.h"
#include "AniObj_FlyingFont.h"
#include "../form/EditBuilding.h"
#include "../form/BuildUI.h"
#include "../Framework/CAppPlatform.h"
#include "../GameWorldScene.h"

NS_COC_BEGIN

PlayerScene::PlayerScene()
: m_pRoot(NULL)
, m_pCurBuilding(NULL)
, m_pTownHall(NULL)
, m_curSelPersonID(0)
, m_bSelClanSoldier(false)
, m_pBuildOK(NULL)
, m_pBuildCancel(NULL)
, m_pBuildUI(NULL)
, m_isInit(false)
, m_isProcListener(false)
, m_isReplayMode(false)
, m_isEditMode(false)
, m_isAlreadyInit(false)
, m_speedReplay(1)
, m_pHero(NULL)
, m_pOtherHero(NULL)
, m_pEarthquakeAct(NULL)
, m_isTouched(false)
, m_timeTouched(0)
, m_isHoldDropSoldier(false)
, m_isCanDropSoldier(false)
, m_isCanHoldDropSoldier(false)
, m_isScaleMode(false)
, m_pTouchMoving(NULL)
, m_curFrameCanSearchNums(0)
, m_pBuildingHeroAltar(NULL)
, m_pBuildingClan(NULL)
, m_pBuildingClanAllies(NULL)
, m_backHomeView(BACK_HOME_TOWN_HALL)
, m_trashBuildingNums(0)
, m_scale(1.0)
, m_isEraseMode(false)
{
}

PlayerScene::~PlayerScene()
{
}

PlayerScene& PlayerScene::getSingleton()
{
	static PlayerScene s_mgr;
    
	return s_mgr;
}

void PlayerScene::init(int w, int h)
{    
    if(m_isAlreadyInit)
    {
        m_bgr.ResumeBgAnimation();
        return;
    }
    
	if(!CAppPlatform::sharedAppPlatform()->m_pushTokenID.empty())
		HttpClient::getSingleton().updPushDeviceID(CAppPlatform::sharedAppPlatform()->m_pushTokenID.c_str());
    
	m_pTileBatchNode = ImgMgr::getSingleton().addBatchNode("building001");
	m_pTileBatchNode->setPosition(0.0f, 0.0f);
    
	m_pParticleBatchNode = ImgMgr::getSingleton().addBatchNode("scenemov001");
	m_pParticleBatchNode->setPosition(0.0f, 0.0f);
    
	BuildingMgr::getSingleton();
	ImgMgr::getSingleton();
    HeroMgr::getSingleton();
    SkillMgr::getSingleton().init();
    ConfigValMgr::getSingleton();
    EnvMapMgr::getSingleton();
    
    m_mapData.init(LOGIC_W, LOGIC_W);
	m_mapDataOcc.init(LOGIC_W, LOGIC_W);
    m_mapAttackPos.init(LOGIC_W, LOGIC_W);
    
    m_astar.m_mapState.init(LOGIC_W, LOGIC_W);
	m_astar.m_mapArea.init(LOGIC_W, LOGIC_W);
	m_astar.m_map.init(LOGIC_W, LOGIC_W);
	m_astar.m_mapSoldier.init(LOGIC_W, LOGIC_W);
    
	m_screenW = w;
	m_screenH = h;
    
	m_tilemask.initWithImageFile("tilemask.png");
    
	m_pRoot = WorldSceneNode::create((2048 + 512) * 2, 2048 * 2);
	m_pRoot->retain();
    
	m_bgr.init(*this);
    
	m_pRoot->addChild(m_pTileBatchNode, SCENE_Z_TILE);
	m_pRoot->addChild(m_pParticleBatchNode, SCENE_Z_PARTICLE);
    
	for(int i = 0; i < 4; ++i)
	{
		char strname[128];
		sprintf(strname, "arrow_%d", i + 1);
        
		m_pArrow[i] = ImgMgr::getSingleton().loadSprite(strname);
		m_pArrow[i]->setAnchorPoint(ccp(0.5, 0.5));
		m_pArrow[i]->setVisible(false);
		m_pArrow[i]->retain();
        
		m_pRoot->addChild(m_pArrow[i], SCENE_Z_ARROW);
	}
    
	for(int i = 0; i < 5; ++i)
	{
		char strname[128];
		sprintf(strname, "red_tile_%d", i + 1);
        
		CCSprite* pSpr = ImgMgr::getSingleton().loadSprite(strname);
        
		pSpr->setAnchorPoint(ccp(0.5, 0));
		pSpr->setVisible(false);
		pSpr->retain();
        
		m_pRoot->addChild(pSpr, SCENE_Z_TILEAREA);
        
		m_lstRedTile.push_back(pSpr);
	}
    
	for(int y = 0; y < LOGIC_W; ++y)
	{
		for(int x = 0; x < LOGIC_W; ++x)
		{
			m_mapArea[y][x] = NULL;
#ifdef COC_SHOWTILE
			CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("tilemask");
			//pFrame->getRect();
            
			CCSprite* pSpr = CCSprite::createWithTexture(m_pTileBatchNode->getTexture(), pFrame->getRect());
            
			pSpr->setAnchorPoint(ccp(0, 0));
			pSpr->setPosition(countScenePos(x, y));
			pSpr->setOpacity(128);
            
			m_pTileBatchNode->addChild(pSpr, SCENE_Z_PARTICLE);
            
			m_mapTile[y][x] = pSpr;
#endif // COC_SHOWTILE
		}
	}
    
	hideArrow();
	hideBuildUI();
    
    setScale(SCENE_NORMAL_SCALE, ccp(0, 0));
    
	initScene();
    
    m_isAlreadyInit = true;
}

void PlayerScene::initScene()
{
    m_isInit = false;
    
    onRefurbishLogicMap();
    
    onBuildingSelect(false, NULL);
    
	setReplaySpeed(1);
    
	setPosition(-LOGIC_W * TILE_W / 4, -LOGIC_W * TILE_H / 4);
    
    m_trashBuildingNums = 0;
    
    for(std::list<UserBuildingInfo*>::iterator it = LogicInfo::getSingleton().m_lstBuilding.begin(); it != LogicInfo::getSingleton().m_lstBuilding.end(); ++it)
	{
		addBuildingEx((*it)->buildingid, (*it)->x, (*it)->y, *it);
		if(m_pCurBuilding != NULL)
		{
            if ((*it)->hp < (*it)->maxhp) {
                m_pCurBuilding->setHP((*it)->hp);
                m_pCurBuilding->showDestroy();
            }
            
            int fulltime = ((m_pCurBuilding->m_info.buildtime[0] * 24 + m_pCurBuilding->m_info.buildtime[1]) * 60 + m_pCurBuilding->m_info.buildtime[2]) * 60;
            m_pCurBuilding->setTime(fulltime, (*it)->lasttime);
		}
        
        int buildingid = (*it)->buildingid;
        int bid = buildingid / 100;
        
        if (bid == BUILDING_TOWN_HALL) {
            m_pTownHall = m_pCurBuilding;
        }
        else if (bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_GOLD_MINE)
        {
            m_lstResBuilding.push_back(m_pCurBuilding);
        }
        else if (bid == BUILDING_WALL) {
            m_lstWallBuilding.push_back(m_pCurBuilding);
        }
        else if (bid == BUILDING_HERO_ALTAR) {
            m_pBuildingHeroAltar = m_pCurBuilding;
        }
        else if (bid == BUILDING_CLAN_CASTLE) {
            m_pBuildingClan = m_pCurBuilding;
            ClanMgr::getSingleton().m_myMaxSpace = m_pBuildingClan->m_info.housingspace;
            ClanMgr::getSingleton().m_castleLevel = buildingid % 100;
        }
        
        if (buildingid / 10000 == 9) {
            m_trashBuildingNums ++;
        }
        
        if((*it)->currestime >= 0 && (*it)->timeres > 0)
        {
            m_pCurBuilding->m_curResTime = (*it)->currestime;
            
            if(m_pCurBuilding->hasNewRes())
                m_pCurBuilding->showResTitle();
        }
        
		buildCurBuilding();
        
		if(m_pCurBuilding != NULL)
			cancelCurBuilding();
	}
    
    m_isInit = true;
    
    if ((getCurrentTime() - LogicInfo::getSingleton().m_trashLastGrowTime) >= TRASH_GROW_TIME
        && m_trashBuildingNums < TRASH_MAX_NUMS) {
        buildTrashBuilding();
    }
    
    m_astar.rebuildMapArea();

    initOtherPerson();
    
	centerOfTownhall();
    
	LogicInfo::getSingleton().fixStorageReserve(true);
    
    LogicInfo::getSingleton().onInitTroophousingSoldier();
}

void PlayerScene::release()
{
	for(int i = 0; i < 4; ++i)
	{
		if(m_pArrow[i] != NULL)
			m_pArrow[i]->release();
        
		m_pArrow[i] = NULL;
	}
    
	if(m_pRoot != NULL)
	{
		for(_SpriteList::iterator it = m_lstRedTile.begin(); it != m_lstRedTile.end(); ++it)
		{
			m_pRoot->removeChild((*it), true);
            
			(*it)->release();
		}
        
		m_pRoot->release();
        
		m_pRoot = NULL;
	}
}

CCPoint PlayerScene::countScenePos(int lx, int ly)
{
    CCPoint pos = CCPointMake(TILE_W /2 * (LOGIC_W + ly - lx - 1), TILE_H /2 * ((LOGIC_W * 2 - ly - lx) - 2));
    
    pos = ccpAdd(pos, ccp(_BEGIN_X, _BEGIN_Y));
    
    return pos;
}

CCPoint PlayerScene::countTilePos(int sx, int sy)
{
    CCPoint pos = ccpSub(ccp(sx, sy), ccp(_BEGIN_X, _BEGIN_Y));
    float mapWidth = LOGIC_W;
    float mapHeight = LOGIC_W;
    
    float halfMapWidth =  mapWidth * 0.5f;
    
    float tileWidth = TILE_W;
    float tileHeight = TILE_H;
    CCPoint tilePosDiv = CCPointMake(pos.x / tileWidth, pos.y / tileHeight);
    float inverseTileY = mapHeight - tilePosDiv.y;
    
    // 将得到的计算结果转换成 int,以确保得到的是整数
    CCPoint pt;
    pt.x = (int)(inverseTileY - tilePosDiv.x + halfMapWidth);
    pt.y = (int)(inverseTileY + tilePosDiv.x - halfMapWidth);
    
    //边界触摸范围扩大8格
    if (LogicInfo::getSingleton().isRealAttackMode()) {
        if (pt.x < 0 && pt.x >= -8) {
            pt.x = 0;
        }
        else if (pt.x > LOGIC_W - 1 && pt.x <= LOGIC_W + 8) {
            pt.x = LOGIC_W - 1;
        }
        
        if (pt.y < 0 && pt.y >= -8) {
            pt.y = 0;
        }
        else if (pt.y > LOGIC_W - 1 && pt.y <= LOGIC_W + 8) {
            pt.y = LOGIC_W - 1;
        }
    }
    
    return pt;
}

bool PlayerScene::isInTile(int ox, int oy)
{
	if(ox >= 0 && ox < m_tilemask.getWidth() && oy >= 0 && oy < m_tilemask.getHeight())
	{
		unsigned char* pBuff = m_tilemask.getData();
        
		if(pBuff[(oy * m_tilemask.getWidth() + ox) * 4 + 3] == 0xff)
			return true;
	}
    
	return false;
}

void PlayerScene::addBuilding(int buildingid, int lx, int ly)
{
	Building* pBuilding = BuildingMgr::getSingleton().newBuilding(buildingid);
	pBuilding->init(buildingid, lx, ly);
    
	pBuilding->m_indexBuilding = (++LogicInfo::getSingleton().m_curBuildingIndex);
    
	m_lstBuilding.push_back(pBuilding);
}

void PlayerScene::setBuildUI(CCNode* ui)
{
	m_pBuildUI = ui;
    
    m_pBuildOK = ((BuildUI*)m_pBuildUI)->m_pBtnOK;
    m_pBuildCancel = ((BuildUI*)m_pBuildUI)->m_pBtnCancel;
    
	if(m_pBuildUI != NULL)
	{
		m_pBuildUI->setScale(m_scale);
        
		float x = m_pRoot->getPositionX();
		float y = m_pRoot->getPositionY();
        
		m_pBuildUI->setPosition(ccp(x, y));
	}
}

void PlayerScene::setScale(float scale, CCPoint ptOff)
{
    if (m_pRoot->numberOfRunningActions() > 0) {
        return;
    }
    
    float sw = CCDirector::sharedDirector()->getWinSize().width;// / m_pRoot->getScale();
	float sh = CCDirector::sharedDirector()->getWinSize().height;// / m_pRoot->getScale();
    
    CCPoint pct = m_pRoot->getCenterPos();
    pct.x += ptOff.x - sw / 2;
    pct.y += ptOff.y - sh / 2;
	m_pRoot->chgScaleEx(scale, pct, ptOff);
    
	m_scale = m_pRoot->getScale();
    
	if(m_pBuildUI != NULL)
	{
		m_pBuildUI->setScale(m_scale);
		m_pBuildUI->setPosition(m_pRoot->getPosition());
	}
}

void PlayerScene::setPosition(int x, int y)
{
    if (m_pRoot->numberOfRunningActions() > 0) {
        return;
    }
    
	m_pRoot->chgPos(x, y);
    
	x = m_pRoot->getPosition().x;
	y = m_pRoot->getPosition().y;
    
	if(m_pBuildUI != NULL)
	{
		m_pBuildUI->setPositionX(x);
		m_pBuildUI->setPositionY(y);
	}
    
	m_centerX = x + (m_screenW / 2) * m_scale;
	m_centerY = y + (m_screenH / 2) * m_scale;
}

bool PlayerScene::canBuilding(int buildingid, int lx, int ly, int lw)
{
    if (!isValidBuildingPos(buildingid, lx, ly, lw)) {
        return false;
    }
    
	for(_BuildingList::const_iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if((*it)->isOverlap(lx, ly, lw) && (m_pCurBuilding == NULL || m_pCurBuilding != *it))
			return false;
	}
    
	return true;
}

bool PlayerScene::canBuildingEx(int buildingid, int lx, int ly, int lw)
{
    if (!isValidBuildingPos(buildingid, lx, ly, lw)) {
        return false;
    }
    
	for(_BuildingList::const_iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if((*it)->isOverlap(lx, ly, lw))
			return false;
	}
        
	return true;
}

//! 判断一排中的墙是否可以放下
bool PlayerScene::canBuildingLine(int buildingid, int lx, int ly, int lw)
{
    if (!isValidBuildingPos(buildingid, lx, ly, lw)) {
        return false;
    }
    
	for(_BuildingList::const_iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if(!isSelectWallLine(*it) && (*it)->isOverlap(lx, ly, lw))
			return false;
	}
    
	return true;
}

Building* PlayerScene::addBuildingEx(int buildingid, UserBuildingInfo* pUBI)
{    
	unselectCurBuilding();
    
	if(m_pCurBuilding != NULL)
	{
		if(m_pCurBuilding->m_state == BUILDING_CTRL_BUILD)
			deleteBuilding(m_pCurBuilding);
        
		m_pCurBuilding = NULL;
	}
    
    float sw = CCDirector::sharedDirector()->getWinSize().width;
	float sh = CCDirector::sharedDirector()->getWinSize().height;
	int x = (sw / 2 - m_pRoot->getPosition().x) / getScale();// + m_screenW / 2;
	int y = (sh / 2 - m_pRoot->getPosition().y) / getScale();// + m_screenH / 2;
    
	const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(buildingid);
	if(pInfo == NULL)
		return NULL;
    
    CCPoint pt = countTilePos(x, y);
    
    pt = findCanBuildPos(buildingid, pt.x, pt.y, pInfo->width);
    if(!canBuilding(buildingid, pt.x, pt.y, pInfo->width))
    {
        pt = countTilePos(x, y);
    }

    m_pCurBuilding = BuildingMgr::getSingleton().newBuilding(buildingid);
    m_pCurBuilding->m_pBuildingInfo = pUBI;
    //		pUBI->pBuilding = m_pCurBuilding;
    m_pCurBuilding->init(buildingid, pt.x, pt.y);
    m_pCurBuilding->chgState(BUILDING_CTRL_BUILD);
    m_pCurBuilding->buildTest();
    
    m_pCurBuilding->m_indexBuilding = (++LogicInfo::getSingleton().m_curBuildingIndex);
    m_lstBuilding.push_back(m_pCurBuilding);
    
    centerOfBuilding(*m_pCurBuilding);
    
	return m_pCurBuilding;
}

Building* PlayerScene::addBuildingEx(int buildingid, int lx, int ly, UserBuildingInfo* pUBI)
{
	if(lx >= 0 && lx < LOGIC_W && ly >= 0 && ly < LOGIC_W)
	{
		//CCLOG("addBuildingEx");
        
		if(m_pCurBuilding != NULL && m_pCurBuilding->m_state == BUILDING_CTRL_BUILD)
		{
			deleteBuilding(m_pCurBuilding);
            
			m_pCurBuilding = NULL;
		}
        
		m_pCurBuilding = BuildingMgr::getSingleton().newBuilding(buildingid);
		if(m_pCurBuilding == NULL)
			return NULL;
        
		m_pCurBuilding->m_pBuildingInfo = pUBI;
		m_pCurBuilding->init(buildingid, lx, ly);
		m_pCurBuilding->chgState(BUILDING_CTRL_BUILD);
		m_pCurBuilding->buildTest();
        
		m_pCurBuilding->m_indexBuilding = (++LogicInfo::getSingleton().m_curBuildingIndex);
        
		m_lstBuilding.push_back(m_pCurBuilding);
	}
    
	return m_pCurBuilding;
}

void PlayerScene::setBuildUIPos(int x, int y, bool bNoOK)
{
	if(m_pBuildOK != NULL)
	{
		m_pBuildOK->setVisible(true);
        
		m_pBuildOK->setPositionX(x + m_pBuildOK->getContentSize().width);
		m_pBuildOK->setPositionY(y + 70);
        
        if(bNoOK)
        {
            m_pBuildOK->setEnabled(false);
        }
        else
        {
            m_pBuildOK->setEnabled(true);
        }
	}
    
	if(m_pBuildCancel != NULL)
	{
		m_pBuildCancel->setVisible(true);
        
		m_pBuildCancel->setPositionX(x - m_pBuildCancel->getContentSize().width);
        m_pBuildCancel->setPositionY(y + 70);
	}
}

//生成杂物
void PlayerScene::buildTrashBuilding()
{
    int building[4] = {90101, 90104, 90203, 90204};
    
    int buildingid = building[rand()%4];
    
    int lx = rand() % LOGIC_W;
    int ly = rand() % LOGIC_W;
    
    const BuildingInfo* info = BuildingMgr::getSingleton().getBuildingInfo(buildingid);
    
    int count = 0;
    CCPoint pt = findCanBuildPos(buildingid, lx, ly, info->width);
    
    m_pCurBuilding = NULL;
    while (!canBuilding(buildingid, pt.x, pt.y, info->width)) {
        
        lx = rand() % LOGIC_W;
        ly = rand() % LOGIC_W;
        
        pt = findCanBuildPos(buildingid, lx, ly, info->width);
        
        count ++;
        if (count > 5) {
            break;
        }
    }
    
    if (canBuilding(buildingid, pt.x, pt.y, info->width)) {
        
        Building* pBuilding = LogicInfo::getSingleton().addBuilding(buildingid, pt.x, pt.y);
        pBuilding->m_pBuildingInfo->state = BUILDING_STATE_NORMAL;

        pBuilding->m_state = BUILDING_CTRL_NORMAL;
        
        unselectCurBuilding();
        hideBuildUI();
        
        HttpClient::getSingleton().build(pBuilding->m_pBuildingInfo->id, buildingid, pt.x, pt.y, pBuilding->m_pBuildingInfo->state);
    }
}

CCPoint PlayerScene::findCanBuildPos(int buildingid, int lx, int ly, int w)
{
	float wnums = m_screenH / (float)(TILE_H * m_scale);
	CCPoint pt(lx, ly);
    
	if(isValidPos(lx, ly))
	{
		int bx = lx - wnums / 2;
		int by = ly - wnums / 2;
        
		bx = bx / 2 * 2 - 1;
		by = by / 2 * 2 - 1;
        
		int distance = 99999999.0f;
        
		for(int y = by; y < by + wnums; y += 2)
		{
            if(y >= w - 1 && y < LOGIC_W - 2)
            {
                for(int x = bx; x < bx + wnums; x += 2)
                {
                    if(x >= w - 1 && x < LOGIC_W - 2)
                    {
                        if(isValidBuildingPos(buildingid, x, y, w) && canBuilding(buildingid, x, y, w))
                        {
                            int dis = (x - w / 2 - lx) * (x - w / 2 - lx) + (y - w / 2 - ly) * (y - w / 2 - ly);
                            if(dis <= distance)
                            {
                                distance = dis;
                                
                                pt.x = x;
                                pt.y = y;
                            }
                        }
                    }
                }
            }
		}
	}
    
	return pt;
}

void PlayerScene::moveBuilding2EditList(Building& building)
{
    if (building.canMove()) {
        m_pGameWorld->m_pMainEditUI->addBuilding2EditList(&building);
        
        onRemoveBuilding(building.m_pBuildingInfo);
    }
}

bool SortWall(const Building_Wall* v1, const Building_Wall* v2)
{
    return v1->m_buildingid > v2->m_buildingid;
}

int PlayerScene::getUpgradeWallsMoney()
{
    if (m_lstSelectWall.empty()) {
        return 0;
    }
    
    m_lstUpgradeWall.clear();
    for (std::vector<Building_Wall*>::iterator it = m_lstSelectWall.begin(); it != m_lstSelectWall.end(); ++it) {
        BuildingInfo info = (*it)->m_info;
        const BuildingInfo* tmp = BuildingMgr::getSingleton().getBuildingInfo(info.buildingid + 1);
        if(tmp && tmp->townhalllevel <= LogicInfo::getSingleton().m_myBaseLevel)
        {
            m_lstUpgradeWall.push_back(*it);
        }
    }
    
    int cost = 0;
    for (std::vector<Building_Wall*>::iterator it = m_lstUpgradeWall.begin(); it != m_lstUpgradeWall.end(); ++it) {
        BuildingInfo info = (*it)->m_info;
        const BuildingInfo* tmp = BuildingMgr::getSingleton().getBuildingInfo(info.buildingid + 1);
        cost += tmp->buildcost;
    }
    
    if (cost > LogicInfo::getSingleton().m_myInfo.maxgoldex) {
        
        cost = 0;
        m_lstUpgradeWall.clear();
    }
    
    return cost;
}

void PlayerScene::setBuildingTouchMode(Building& building, bool isTouch)
{
	if(isTouch)
	{
        CCPoint pos = countScenePos(building.getLogicX(), building.getLogicY());
		building.m_xBeginTouch = pos.x;
		building.m_yBeginTouch = pos.y;
        
		building.m_isTouch = true;
		building.m_isTouchMoved = false;
	}
	else
	{
		building.m_isTouch = false;
		building.m_isTouchMoved = false;
	}
    
    if (!building.canMove()) {
        building.m_isTouch = false;
    }
}

void PlayerScene::hideBuildUI()
{
	if(m_pBuildOK != NULL)
		m_pBuildOK->setVisible(false);
    
	if(m_pBuildCancel != NULL)
		m_pBuildCancel->setVisible(false);
}

CCSprite* PlayerScene::getTileColor(bool canBuild, int lw)
{
	lw /= 2;
    
	if(lw >= 1 && lw <= 5)
	{
		if(canBuild)
			return NULL;
		else
		{
			return m_lstRedTile[lw - 1];
		}
	}
    
	return NULL;
}

bool PlayerScene::collectRes(int x, int y, int lx, int ly)
{
    if (m_isEditMode) {
        return false;
    }
    
    for(_BuildingList::iterator it = m_lstResBuilding.begin(); it != m_lstResBuilding.end(); ++it)
    {
        if(m_pCurBuilding != *it && ((*it)->isInIcon(x, y) || (*it)->isIn(lx, ly)))
        {
            Building* pBuilding = (*it);
            if(pBuilding->hasNewRes() && (*it)->m_collectResTimes <= 0 && pBuilding->m_pBuildingInfo->timeres > 0)
            {
                const BuildingInfo* pInfo = BuildingMgr::getSingleton().getBuildingInfo(pBuilding->m_pBuildingInfo->buildingid);
                if(pInfo != NULL)
                {
                    int res = pBuilding->countRes_Sub();
                    
                    if(res > 0)
                    {
                        if(pInfo->producesres == MONEY_GOLD)
                        {
                            int pn = 30.0f * res / pBuilding->m_info.maxstored[pBuilding->m_info.producesres - 1];
                            if(pn < 3)
                                pn = 3;
                            
                            shootGold(*pBuilding, pn);
                            addGetResAni(pBuilding->getScenePt().x, pBuilding->getScenePt().y+300, res,ccYELLOW);
                            SoundMgr::getSingleton().playEffect("get_gold.mp3");
                        }
                        else if(pInfo->producesres == MONEY_ELIXIR)
                        {
                            int pn = 30.0f * res / pBuilding->m_info.maxstored[pBuilding->m_info.producesres - 1];
                            if(pn < 3)
                                pn = 3;
                            
                            shootOil(*pBuilding, pn);
                            addGetResAni(pBuilding->getScenePt().x, pBuilding->getScenePt().y+300, res,ResConfig::getSingleton().getColorValue("ADD_RES_FOR_OIL_FONT_COLOR"));
                            SoundMgr::getSingleton().playEffect("get_oil.mp3");
                        }
                        
                        LogicInfo::getSingleton().harvest(*pBuilding);
                        
                        if(pBuilding->m_curResTime <= 0)
                            pBuilding->hideResTitle();
                        
                        return true;
                    }
                    
                    pBuilding->hideResTitle();
                    return true;
                }
            }
        }
    }
    
    return false;
}

void PlayerScene::onTouchBegan(int sx, int sy)
{
	clearTouchMoving();
	m_timeTouch = 0.0f;
    
	m_isHoldDropSoldier = false;
	m_isTouchBkMove = false;
    
	m_isTouched = true;
	m_timeTouched = 450;
	m_ptTouchOff.x = 0;
	m_ptTouchOff.y = 0;
    
	m_ptTouchBegin.x = sx;
	m_ptTouchBegin.y = sy;
    
	m_ptRootBegin.x = m_pRoot->getPosition().x;
	m_ptRootBegin.y = m_pRoot->getPosition().y;
    
	int x = (sx - m_pRoot->getPosition().x) / getScale();
	int y = (sy - m_pRoot->getPosition().y) / getScale();
    
	CCPoint pt = countTilePos(x, y);
    if(LogicInfo::getSingleton().isNormalMode())
    {
        if(m_pCurBuilding != NULL)
        {
            if(m_pCurBuilding->isIn(pt.x, pt.y))
            {
                setBuildingTouchMode(*m_pCurBuilding, true);
            }
            else if(isInSelectWallLine(pt.x, pt.y))	//! 在选择一排墙的情况下
            {
                setBuildingTouchMode(*m_pCurBuilding, true);
            }
            else if (m_pCurBuilding->m_state == BUILDING_CTRL_SELECTED)
            {
                if (m_isEditMode) {
                    
                    Building* pWall = m_pGameWorld->m_pMainEditUI->buildWall();
                    
                    if (pWall != NULL) {
                        return;
                    }
                }
                
                unselectCurBuilding();
            }
            
            if (m_pCurBuilding != NULL) {
                return;
            }
        }
        
        //collect res
        if (collectRes(x, y, pt.x, pt.y))
        {
            return;
        }
        
        for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
        {
            if(m_pCurBuilding != *it && (*it)->isIn(pt.x, pt.y))
            {
                m_pCurBuilding = *it;
                return;
            }
        }
    }
    else if (LogicInfo::getSingleton().isVisitMode())
    {
        if (m_pCurBuilding == NULL) {
            for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
            {
                if((*it)->isVisible() && (*it)->isIn(pt.x, pt.y))
                {
                    m_pCurBuilding = *it;
                    break;
                }
            }
            
        }else{
            
            unselectCurBuilding();
        }
    }
}

void PlayerScene::onTouchMoved(int ox, int oy)
{
	m_ptTouchOff.x = ox;
	m_ptTouchOff.y = oy;
    
	if(LogicInfo::getSingleton().isNormalMode() && (m_pCurBuilding != NULL && m_pCurBuilding->m_isTouch))
	{
		int x = m_pCurBuilding->m_xBeginTouch + ox / getScale();
		int y = m_pCurBuilding->m_yBeginTouch + oy / getScale();
		
		if(m_pCurBuilding->m_state == BUILDING_CTRL_NORMAL)
		{
			return;
		}
        
        CCPoint pt = countTilePos(x, y);
        if(((int)pt.x) % TILE_CELL == 0)
            pt.x = ((int)pt.x) - 1;
        if(((int)pt.y) % TILE_CELL == 0)
            pt.y = ((int)pt.y) - 1;
        
        if(pt.x != m_pCurBuilding->getLogicX() || pt.y != m_pCurBuilding->getLogicY())
        {
            if (!m_pCurBuilding->m_isTouchMoved)
            {
                if (m_isEditMode) {
                    for (int i = 0; i < m_pGameWorld->m_pMainEditUI->m_lstBuildingBtn.size(); i ++)
                    {
                        m_pGameWorld->m_pMainEditUI->m_lstBuildingBtn[i]->setVisible(false);
                    }
                    
                    m_pGameWorld->m_pMainEditUI->m_isEdited = true;
                    
                    m_pGameWorld->m_pMainEditUI->buildWall();
                    
                    if (m_pCurBuilding->m_type == BUILDING_TYPE_WALL && ((Building_Wall*)m_pCurBuilding)->m_isEditMove)
                    {
                        return;
                    }
                }
                else {
                    for (int i = 0; i < m_pGameWorld->m_pMainUI->m_lstBuildingBtn.size(); i ++)
                    {
                        m_pGameWorld->m_pMainUI->m_lstBuildingBtn[i]->setVisible(false);
                    }
                }
                
                if (m_pCurBuilding->m_state == BUILDING_CTRL_SELECTED && m_pCurBuilding->m_pTileColor == NULL) {
                    onSetLogicMap(m_pCurBuilding->getLogicX(), m_pCurBuilding->getLogicY(), m_pCurBuilding->getLogicWidth(), m_pCurBuilding->m_buildingid, 0);
                }
                
                m_pCurBuilding->m_isTouchMoved = true;
                if (m_pCurBuilding->m_state != BUILDING_CTRL_BUILD) {
                    m_pCurBuilding->chgState(BUILDING_CTRL_MOVE);
                }
            }
            
            if(!m_lstSelectWall.empty())
            {
                moveSelectWallLine(pt);
            }
            else
            {
                m_pCurBuilding->setLogicPos(pt.x, pt.y);
                SoundMgr::getSingleton().playEffect("build_move.m4a");
            }
            
            if (m_isEditMode) {
                PlayerScene::getSingleton().m_pGameWorld->m_pMainEditUI->checkEditSaveEnable();
            }
        }
	}
	else if(!m_isHoldDropSoldier)
	{
		if(abs(ox) > 10 || abs(oy) > 10)
        {
			m_isTouchBkMove = true;
            setPosition(m_ptRootBegin.x + ox, m_ptRootBegin.y + oy);
        }
	}
}

void PlayerScene::onTouchEnded(int ox, int oy)
{
	m_ptTouchOff.x = ox;
	m_ptTouchOff.y = oy;
    
    int x = (m_ptTouchBegin.x + ox - m_pRoot->getPosition().x) / getScale();
    int y = (m_ptTouchBegin.y + oy - m_pRoot->getPosition().y) / getScale();
    
    CCPoint pt = countTilePos(x, y);
    
	if(LogicInfo::getSingleton().isRealAttackMode())
	{
        if (m_isCanDropSoldier) {
            
            if (m_pGameWorld->m_pGameLayer->m_lstTouch.size() > 0) {
                std::map<int, _TouchNode>::iterator iter = m_pGameWorld->m_pGameLayer->m_lstTouch.begin();
                for (; iter != m_pGameWorld->m_pGameLayer->m_lstTouch.end(); ++iter)
                {
                    onDropSoldier2Attack(iter->second.x, iter->second.y);
                }
            }else{
                onDropSoldier2Attack(m_ptTouchBegin.x, m_ptTouchBegin.y);
            }
        }
        
        if (m_isHoldDropSoldier) {
            m_isHoldDropSoldier = false;
            return;
        }
	}
    
    if(m_pCurBuilding != NULL)
    {
        if (m_pCurBuilding->m_state == BUILDING_CTRL_BUILD) {
            setBuildingTouchMode(*m_pCurBuilding, false);
            return;
        }
        else if (m_pCurBuilding->m_state == BUILDING_CTRL_SELECTED) {
            unselectCurBuilding();
            return;
        }
        
        if (LogicInfo::getSingleton().isNormalMode()) {
            
            if (m_pCurBuilding->m_state == BUILDING_CTRL_MOVE) {
                
                if(m_pCurBuilding->m_pTileColor != NULL && abs(ox) <= 10 && abs(oy) <= 10)
                {
                    if (m_isEditMode) {
                        
                        if (m_pCurBuilding->m_editStatus != EDITBUILD_STATUS_SUCCESS) {
                            
                            moveBuilding2EditList(*m_pCurBuilding);
                            return;
                        }
                    }
                    
                    if(!m_lstSelectWall.empty())
                    {
                        resetSelectWallLine();
                    }
                    else
                    {
                        m_pCurBuilding->setLogicPos(m_pCurBuilding->m_lbx, m_pCurBuilding->m_lby);
                        buildCurBuilding();
                    }
                    
                    return;
                }
            }
            
            if(m_pCurBuilding->m_isTouch)
            {
                setBuildingTouchMode(*m_pCurBuilding, false);
                
                int x = m_pCurBuilding->m_xBeginTouch + ox / getScale();
                int y = m_pCurBuilding->m_yBeginTouch + oy / getScale();
                
                CCPoint pt = countTilePos(x, y);
                if(isValidPos(pt.x, pt.y))
                {
                    if(((int)pt.x) % TILE_CELL == 0)
                        pt.x = ((int)pt.x) - 1;
                    if(((int)pt.y) % TILE_CELL == 0)
                        pt.y = ((int)pt.y) - 1;
                    
                    if(!m_lstSelectWall.empty())
                    {
                        downSelectWallLine();
                    }
                    else if(m_pCurBuilding->m_state == BUILDING_CTRL_MOVE)
                    {
                        buildCurBuilding();
                    }
                    
                    if(m_pCurBuilding != NULL)
                    {
                        setBuildingTouchMode(*m_pCurBuilding, false);
                    }
                }
                
                return;
            }
        }
    }
    
    if (LogicInfo::getSingleton().isNormalMode()) {
        
        if(m_pHero != NULL && m_pHero->isVisible())
        {
            m_pGameWorld->m_pMainUI->onSelectHero(false);
            
            if (fabs(pt.x-m_pHero->getLogicX()) <= m_pHero->getLogicWidth() && fabs(pt.y-m_pHero->getLogicY()) <= m_pHero->getLogicWidth()) {
                m_pGameWorld->m_pMainUI->onSelectHero(true);
                return;
            }
        }
    }
    
    if(m_pCurBuilding != NULL && m_pCurBuilding->m_state == BUILDING_CTRL_NORMAL)
    {
        if(isValidPos(pt.x, pt.y))
        {
            if(m_pCurBuilding->isIn(pt.x, pt.y) && !m_isTouchBkMove)
            {
                if (m_isEraseMode) {
                    moveBuilding2EditList(*m_pCurBuilding);
                }
                else {
                    selectBuilding(*m_pCurBuilding);
                }
                return;
            }
        }
    }
    
    if(m_pCurBuilding != NULL && m_pCurBuilding->m_pTileColor == NULL)
    {
        unselectCurBuilding();
    }
    
    setPosition(m_ptRootBegin.x + ox, m_ptRootBegin.y + oy);
    
    addTouchMoving(ox, oy, m_timeTouch);
}

void PlayerScene::onHeroSelect()
{
    if(m_pGameWorld && m_pGameWorld->m_pMainUI)
    {
        m_pGameWorld->m_pMainUI->onSelectHero(true);
    }
}

void PlayerScene::setIsEditMode(bool isEditMode)
{
    if (m_isEditMode == isEditMode) {
        return;
    }
    
    m_isEditMode = isEditMode;
    if (!isEditMode) {
        setIsEraseMode(false);
    }
    
    for (_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it) {
        if (m_isEditMode) {
            if ((*it)->m_pIconEx) {
                (*it)->m_pIconEx->setVisible(false);
            }
        }
    }
    
    for (int i = 0; i <= CAMP_WORKER; i++) {
        for (_PersonList::iterator it = m_lstPerson[i].begin(); it != m_lstPerson[i].end(); ++it) {
            
            if (m_isEditMode) {
                (*it)->setVisible(false);
            }
            else {
                (*it)->setVisible(true);
            }
        }
    }
}

void PlayerScene::setIsEraseMode(bool isEraseMode)
{
    m_isEraseMode = isEraseMode;
}

void PlayerScene::selectBuilding(Building& building)
{
	clearSelectWallLine();
    
	if(building.m_state == BUILDING_CTRL_MOVE || building.m_state == BUILDING_CTRL_SELECTED)
		return;
    
	m_pCurBuilding = &building;
    
	if(m_pCurBuilding->canMove())
	{
		m_pCurBuilding->chgState(BUILDING_CTRL_SELECTED);
        
		m_pCurBuilding->buildTest();
		m_pCurBuilding->chgSelMode(true);
        
        SoundMgr::getSingleton().playEffect("build_move.m4a");
	}
	else
	{
		m_pCurBuilding->chgState(BUILDING_CTRL_SELECTED);
		m_pCurBuilding->chgSelMode(true);
	}
    
	onBuildingSelect(true, m_pCurBuilding);
}

bool PlayerScene::canSelectWallLine()
{
    Building& building = *m_pCurBuilding;
    
    if (building.m_buildingid/100 != BUILDING_WALL) {
        return false;
    }

    Building_Wall* pWall = (Building_Wall*)&building;
    
    if (pWall->m_pWallLeft) {
        return true;
    }
    if (pWall->m_pWallRight) {
        return true;
    }
    if (pWall->m_pWallUp) {
        return true;
    }
    if (pWall->m_pWallDown) {
        return true;
    }
    return false;
}

bool PlayerScene::canBuildWallLine()
{
    if (m_lstSelectWall.empty()) {
        return false;
    }
    
    for(std::vector<Building_Wall*>::iterator it = m_lstSelectWall.begin(); it != m_lstSelectWall.end(); ++it)
    {
        if(!canBuildingLine(10301, (*it)->getLogicX(), (*it)->getLogicY(), (*it)->getLogicWidth())) {
            return false;
        }
    }
    
    return true;
}

//! 选择一排墙
bool PlayerScene::selectWallLine()
{
	Building& building = *m_pCurBuilding;
    
    if (building.m_buildingid/100 != BUILDING_WALL) {
        return false;
    }
    
    clearSelectWallLine();
    
    Building_Wall* pWallLeft = (Building_Wall*)&building;
    Building_Wall* pWallRight = (Building_Wall*)&building;
    Building_Wall* pWallUp = (Building_Wall*)&building;
    Building_Wall* pWallDown = (Building_Wall*)&building;
    
    int rownums = 1;
    {
        while (pWallLeft->m_pWallLeft) {
            rownums++;
            pWallLeft = pWallLeft->m_pWallLeft;
        }
        
        while (pWallRight->m_pWallRight) {
            rownums++;
            pWallRight = pWallRight->m_pWallRight;
        }
     
    }
    int colnums = 1;
    {
        while (pWallUp->m_pWallUp) {
            colnums++;
            pWallUp = pWallUp->m_pWallUp;
        }
        
        while (pWallDown->m_pWallDown) {
            colnums++;
            pWallDown = pWallDown->m_pWallDown;
        }
    }

    int dir = 0;
    if (rownums >= colnums) {
        dir = 1;
    }
	else if (colnums >= rownums) {
        dir = 2;
    }
    
    if (dir == 1) {
        Building_Wall* pWall = pWallRight;
        while (pWall) {
            pWall->chgSelMode(false);
            m_lstSelectWall.push_back(pWall);
            pWall = pWall->m_pWallLeft;
        }
    }
    else if (dir == 2) {
        Building_Wall* pWall = pWallUp;
        while (pWall) {
            pWall->chgSelMode(false);
            m_lstSelectWall.push_back(pWall);
            pWall = pWall->m_pWallDown;
        }
    }
    
	if(m_lstSelectWall.size() <= 0)
		return false;
    
	Building* bwall = m_lstSelectWall[0];
	Building* ewall = m_lstSelectWall[m_lstSelectWall.size() - 1];
    
    setBuildingTouchMode(*m_pCurBuilding, true);
	showWallLineArrow(bwall->getLogicX(), bwall->getLogicY(), ewall->getLogicX(), ewall->getLogicY());
    
    if (m_isEditMode) {
        PlayerScene::getSingleton().m_pGameWorld->m_pMainEditUI->checkEditSaveEnable();
    }
    
    return true;
}

//! 显示箭头
void PlayerScene::showWallLineArrow(int bx, int by, int ex, int ey)
{
    float lw = 2;
    
    CCPoint bpos = countScenePos(bx, by);
	CCPoint epos = countScenePos(ex, ey);
    
	if(by == ey)
	{
		CCPoint mpos = countScenePos((ex - bx) / 2.0f + bx, ey);
        
		if(m_pArrow[1] != NULL)
		{
			m_pArrow[1]->setPosition(ccp(bpos.x + lw * 3 * TILE_W / 4 - (lw - 1) * TILE_W / 2, bpos.y + lw * TILE_H * 3 / 4));
			m_pArrow[1]->setVisible(true);
		}
        
		if(m_pArrow[3] != NULL)
		{
			m_pArrow[3]->setPosition(ccp(epos.x + lw * TILE_W / 4 - (lw - 1) * TILE_W / 2, epos.y + lw * TILE_H / 4));
			m_pArrow[3]->setVisible(true);
		}
        
		if(m_pArrow[0] != NULL)
		{
			m_pArrow[0]->setPosition(ccp(mpos.x + lw * TILE_W / 4 - (lw - 1) * TILE_W / 2, mpos.y + lw * TILE_H * 3 / 4));
			m_pArrow[0]->setVisible(true);
		}
        
		if(m_pArrow[2] != NULL)
		{
			m_pArrow[2]->setPosition(ccp(mpos.x + lw * 3 * TILE_W / 4 - (lw - 1) * TILE_W / 2, mpos.y + lw * TILE_H / 4));
			m_pArrow[2]->setVisible(true);
		}
	}
	else if(bx == ex)
	{
		CCPoint mpos = countScenePos(ex, (ey - by) / 2.0f + by);
        
		if(m_pArrow[0] != NULL)
		{
			m_pArrow[0]->setPosition(ccp(bpos.x + lw * TILE_W / 4 - (lw - 1) * TILE_W / 2, bpos.y + lw * TILE_H * 3 / 4));
			m_pArrow[0]->setVisible(true);
		}
        
		if(m_pArrow[2] != NULL)
		{
			m_pArrow[2]->setPosition(ccp(epos.x + lw * 3 * TILE_W / 4 - (lw - 1) * TILE_W / 2, epos.y + lw * TILE_H / 4));
			m_pArrow[2]->setVisible(true);
		}
        
		if(m_pArrow[1] != NULL)
		{
			m_pArrow[1]->setPosition(ccp(mpos.x + lw * 3 * TILE_W / 4 - (lw - 1) * TILE_W / 2, mpos.y + lw * TILE_H * 3 / 4));
			m_pArrow[1]->setVisible(true);
		}
        
		if(m_pArrow[3] != NULL)
		{
			m_pArrow[3]->setPosition(ccp(mpos.x + lw * TILE_W / 4 - (lw - 1) * TILE_W / 2, mpos.y + lw * TILE_H / 4));
			m_pArrow[3]->setVisible(true);
		}
	}
}

//! 清空墙选择
void PlayerScene::clearSelectWallLine()
{
	for(std::vector<Building_Wall*>::iterator it = m_lstSelectWall.begin(); it != m_lstSelectWall.end(); )
	{
		Building_Wall* pBuilding = *it;
        pBuilding->chgSelModeLine(false);
        
		it = m_lstSelectWall.erase(it);
	}
}

//! 墙升级，更新选择列表
void PlayerScene::chgSelectWallLine(Building* ob, Building* nb)
{
	if(ob == NULL)
		return;
    
	Building_Wall* ow = (Building_Wall*)ob;
	Building_Wall* nw = (Building_Wall*)nb;
    
	for(std::vector<Building_Wall*>::iterator it = m_lstSelectWall.begin(); it != m_lstSelectWall.end(); ++it)
	{
		if((*it) == ow)
		{
            it = m_lstSelectWall.erase(it);
            
            if(nw != NULL)
            {
                m_lstSelectWall.insert(it, nw);
            }
			return;
		}
	}
}

//! 判断一个墙是否在选择的墙中
bool PlayerScene::isSelectWallLine(Building* building)
{
	for(std::vector<Building_Wall*>::iterator it = m_lstSelectWall.begin(); it != m_lstSelectWall.end(); ++it)
	{
		if((*it) == building)
			return true;
	}
    
	return false;
}

//！判断是否点中了一排墙中的一个
bool PlayerScene::isInSelectWallLine(int lx, int ly)
{
	for(std::vector<Building_Wall*>::iterator it = m_lstSelectWall.begin(); it != m_lstSelectWall.end(); ++it)
	{
		if((*it)->isIn(lx, ly))
			return true;
	}
    
	return false;
}

//! 移动一排墙
void PlayerScene::moveSelectWallLine(CCPoint& pt)
{
	if(m_lstSelectWall.empty())
		return;
    
    int mx = m_pCurBuilding->getLogicX() - pt.x;
    int my = m_pCurBuilding->getLogicY() - pt.y;
    
    for(vector<Building_Wall*>::iterator it = m_lstSelectWall.begin(); it != m_lstSelectWall.end(); ++it)
    {
        (*it)->m_state = BUILDING_CTRL_MOVE;
        
        int cx = (*it)->getLogicX();
        int cy = (*it)->getLogicY();
        
        (*it)->setLogicPos(cx - mx, cy - my);
    }
    
    vector<Building_Wall*>::iterator it = m_lstSelectWall.begin();
    Building_Wall* pWall = (*it);
    pWall->rebuildWall();
    it ++;
    
    for(; it != m_lstSelectWall.end(); ++it)
    {
        (*it)->rebuildWall(pWall);
        pWall = (*it);
    }
    
    Building_Wall* bwall = m_lstSelectWall[0];
    Building_Wall* ewall = m_lstSelectWall[m_lstSelectWall.size() - 1];
    
    showWallLineArrow(bwall->getLogicX(), bwall->getLogicY(), ewall->getLogicX(), ewall->getLogicY());
    
    //! 判断是否有不可以放下去的块
    bool canbuild = canBuildWallLine();
    
    for(int i = 0; i < m_lstSelectWall.size(); ++i)
    {
        m_lstSelectWall[i]->setTileLine(!canbuild, i);
    }
    
    SoundMgr::getSingleton().playEffect("build_move.m4a");
}

//! 旋转一排墙
void PlayerScene::rotationSelectWallLine()
{
	if(m_pCurBuilding == NULL || m_lstSelectWall.empty())
		return;
    
	if(m_lstSelectWall.size() <= 0)
		return;
    
	//! 旋转点坐标
	int cx = m_pCurBuilding->getLogicX();
	int cy = m_pCurBuilding->getLogicY();
    
	int bx = m_lstSelectWall[0]->getLogicX();
	int by = m_lstSelectWall[0]->getLogicY();
    
	int ex = m_lstSelectWall[m_lstSelectWall.size() - 1]->getLogicX();
	int ey = m_lstSelectWall[m_lstSelectWall.size() - 1]->getLogicY();
    
	//! 横向
	if(by == ey)
	{
		int ny = cy + (bx - cx);
        
		for(int i = 0; i < m_lstSelectWall.size(); ++i)
		{
            m_lstSelectWall[i]->m_state = BUILDING_CTRL_MOVE;
            
            if (bx < ex) {
                m_lstSelectWall[i]->setLogicPos(cx, ny + i * 2);
            }
            else {
                m_lstSelectWall[i]->setLogicPos(cx, ny - i * 2);
            }
		}
	}
	else
	{
		int nx = cx - (by - cy);
        
		for(int i = 0; i < m_lstSelectWall.size(); ++i)
		{
            m_lstSelectWall[i]->m_state = BUILDING_CTRL_MOVE;
            
            if (by < ey) {
                m_lstSelectWall[i]->setLogicPos(nx - i * 2, cy);
            }
            else {
                m_lstSelectWall[i]->setLogicPos(nx + i * 2, cy);
            }
		}
	}
    
    vector<Building_Wall*>::iterator it = m_lstSelectWall.begin();
    Building_Wall* pWall = (*it);
    pWall->rebuildWall();
    it ++;
    
    for(; it != m_lstSelectWall.end(); ++it)
    {
        (*it)->rebuildWall(pWall);
        pWall = (*it);
    }
    
	Building_Wall* bwall = m_lstSelectWall[0];
    Building_Wall* ewall = m_lstSelectWall[m_lstSelectWall.size() - 1];
    
	showWallLineArrow(bwall->getLogicX(), bwall->getLogicY(), ewall->getLogicX(), ewall->getLogicY());
    
	//! 判断是否有不可以放下去的块
	bool canbuild = canBuildWallLine();
    
	for(int i = 0; i < m_lstSelectWall.size(); ++i)
	{
		m_lstSelectWall[i]->setTileLine(!canbuild, i);
	}
    
    SoundMgr::getSingleton().playEffect("build_move.m4a");
}

//! 放下一排墙
void PlayerScene::downSelectWallLine()
{
    if(m_pCurBuilding->m_pTileColor == NULL)
    {
        //! 判断是否有不可以放下去的块
        bool canbuild = canBuildWallLine();
        
        for(int i = 0; i < m_lstSelectWall.size(); ++i)
        {
            m_lstSelectWall[i]->setTileLine(!canbuild, i);
        }
        
        if(!canbuild)
        {
            return;
        }
        
        Building_Wall* bwall = m_lstSelectWall[0];
        Building_Wall* ewall = m_lstSelectWall[m_lstSelectWall.size() - 1];
        
        int index = 0;
        
         //! 重建墙关系
        if (ewall->getLogicX() < bwall->getLogicX() || ewall->getLogicY() < bwall->getLogicY()) {
           
            //顺序操作
            for(index = 0; index < m_lstSelectWall.size(); ++index)
            {
                Building_Wall* pWall = m_lstSelectWall[index];
                
                HttpClient::getSingleton().move(pWall->m_pBuildingInfo->id, pWall->m_lbx, pWall->m_lby, pWall->getLogicX(), pWall->getLogicY());
                pWall->chgState(BUILDING_CTRL_NORMAL);
                pWall->rebuildWall();
            }
        }
        else {
            
            //逆序操作
            for(index = m_lstSelectWall.size() - 1; index >= 0; --index)
            {
                Building_Wall* pWall = m_lstSelectWall[index];
                
                HttpClient::getSingleton().move(pWall->m_pBuildingInfo->id, pWall->m_lbx, pWall->m_lby, pWall->getLogicX(), pWall->getLogicY());
                pWall->chgState(BUILDING_CTRL_NORMAL);
                pWall->rebuildWall();
            }
        }
        
        clearSelectWallLine();
        unselectCurBuilding();
        
        onRefurbishLogicMap();
        rebuildMapArea(false);
        
        if (m_isEditMode) {
            m_pGameWorld->m_pMainEditUI->m_isEdited = true;
        }
        
        SoundMgr::getSingleton().playEffect("build_ok.m4a");
    }
}

//! 初始化一排红色地格
void PlayerScene::initRedTileLine(int num)
{
	if(m_lstRedTileLine.size() < num)
	{
		int anum = num - m_lstRedTileLine.size();
        
		for(int i = 0; i < anum; ++i)
		{
			CCSprite* pSpr = ImgMgr::getSingleton().loadSprite("red_tile_1");
            
			pSpr->setAnchorPoint(ccp(0.5, 0));
			pSpr->setVisible(false);
			pSpr->retain();
            
			m_pRoot->addChild(pSpr, SCENE_Z_TILEAREA);
            
			m_lstRedTileLine.push_back(pSpr);
		}
	}
}

//! 取一排红色地格中的一个
CCSprite* PlayerScene::getRedTileLine(int index)
{
	if(index >= m_lstRedTileLine.size())
		initRedTileLine(index + 1);
    
	return m_lstRedTileLine[index];
}

//! 一排墙回到原来的位置
void PlayerScene::resetSelectWallLine()
{
    for(std::vector<Building_Wall*>::iterator it = m_lstSelectWall.begin(); it != m_lstSelectWall.end(); ++it)
    {
        (*it)->resetWallLine();
    }
    
	//! 重建墙关系
	for(std::vector<Building_Wall*>::iterator it = m_lstSelectWall.begin(); it != m_lstSelectWall.end(); ++it)
	{
		(*it)->rebuildWall();
	}
    
	clearSelectWallLine();
    
    buildCurBuilding();
    
    onRefurbishLogicMap();
    rebuildMapArea(false);
    
    hideArrow();
    
    SoundMgr::getSingleton().playEffect("build_ok.m4a");
}

//! 资源建筑升级，更新资源建筑列表
void PlayerScene::chgResBuilding(Building* ob, Building* nb)
{
	if(ob == NULL)
		return ;
    
	for(_BuildingList::iterator it = m_lstResBuilding.begin(); it != m_lstResBuilding.end(); ++it)
	{
		if((*it) == ob)
		{
            it = m_lstResBuilding.erase(it);
            
            if(nb != NULL)
            {
                m_lstResBuilding.insert(it, nb);
            }
			return;
		}
	}
}

void PlayerScene::cancelCurBuilding()
{
	if(m_pCurBuilding != NULL)
	{
		for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
		{
			if(*it == m_pCurBuilding)
			{
				chgSelectWallLine(*it, NULL);
				chgResBuilding(*it, NULL);
                
				m_lstBuilding.erase(it);
				break;
			}
		}
        
		m_pCurBuilding->releaseFromScene();
        
		m_pCurBuilding = NULL;
	}
    
	hideArrow();
    
	hideBuildUI();
}

void PlayerScene::buildCurBuilding()
{    
	bool ischg = false;
	bool isBuilding = false;
	bool isMove = false;
    
	if(m_pCurBuilding != NULL)
	{
		if(BUILDING_CTRL_SELECTED == m_pCurBuilding->m_state)
		{
			m_pCurBuilding->chgSelMode(false);
			setBuildingTouchMode(*m_pCurBuilding, false);
            
			onBuildingSelect(false, NULL);
            
			m_pCurBuilding->m_state = BUILDING_CTRL_NORMAL;
			Building* pBuilding = m_pCurBuilding;
			m_pCurBuilding = NULL;
            
			pBuilding->setLogicPos(pBuilding->getLogicX(), pBuilding->getLogicY());
            
            hideArrow();
            
            if(pBuilding->m_buildingid/100 == BUILDING_WALL)
            {
                ((Building_Wall*)pBuilding)->rebuildWall();
            }
            
			return;
		}
        
		if(m_pCurBuilding->m_pTileColor == NULL && (m_pCurBuilding->m_lbx != m_pCurBuilding->getLogicX() || m_pCurBuilding->m_lby != m_pCurBuilding->getLogicY())) {
			ischg = true;
        }
	}
    
	if(m_pCurBuilding != NULL && m_pCurBuilding->m_state == BUILDING_CTRL_BUILD)
		isBuilding = true;
	else if(m_pCurBuilding != NULL && m_pCurBuilding->m_state == BUILDING_CTRL_MOVE)
		isMove = true;
    
	Building* pBuilding = NULL;
    
	if(m_pCurBuilding != NULL && m_pCurBuilding->buildOK())
	{
        onBuildingSelect(false, NULL);
        
        m_pCurBuilding->chgSelMode(false);
		setBuildingTouchMode(*m_pCurBuilding, false);
        
		pBuilding = m_pCurBuilding;
		m_pCurBuilding = NULL;
        
        pBuilding->m_editStatus = EDITBUILD_STATUS_SUCCESS;
		pBuilding->setLogicPos(pBuilding->getLogicX(), pBuilding->getLogicY());
        
        if (pBuilding->m_pTileColor == NULL) {
            onSetLogicMap(pBuilding->getLogicX(), pBuilding->getLogicY(), pBuilding->getLogicWidth(), pBuilding->m_buildingid, pBuilding->m_buildingid);
        }
        
		if(m_isInit)
			SoundMgr::getSingleton().playEffect("build_ok.m4a");
        
		hideArrow();
	}
	else if(!m_isInit && m_pCurBuilding != NULL)
	{
		m_pCurBuilding->m_state = BUILDING_CTRL_MOVE;
        
		setBuildingTouchMode(*m_pCurBuilding, false);
        
		pBuilding = m_pCurBuilding;
		m_pCurBuilding = NULL;
        
		pBuilding->setLogicPos(pBuilding->getLogicX(), pBuilding->getLogicY());
        
        if (pBuilding->m_pTileColor == NULL) {
            onSetLogicMap(pBuilding->getLogicX(), pBuilding->getLogicY(), pBuilding->getLogicWidth(), pBuilding->m_buildingid, pBuilding->m_buildingid);
        }
        
		hideArrow();
	}
    
	hideBuildUI();
    
    if (m_isEditMode) {
        PlayerScene::getSingleton().m_pGameWorld->m_pMainEditUI->checkEditSaveEnable();
    }
    
	if(!m_isInit)
		return;
    
    if(!m_isEditMode && isBuilding && pBuilding != NULL && pBuilding->m_type == BUILDING_TYPE_WALL)
	{
		int nextdir = DIR_LEFT;
		int nums = 0;
        
        m_walldirx = 0;
        m_walldiry = 0;
        
    __rechg:
		if(m_walldirx == 0 && m_walldiry == 0)
		{
            nextdir = countNextDirWithWall(pBuilding->getLogicX(), pBuilding->getLogicY());
            countOffPosWithWall(nextdir, m_walldirx, m_walldiry);
		}
		else
		{
            nextdir = countDirWithWall(m_walldirx, m_walldiry);
		}
        
		++nums;
        
		int lx = pBuilding->getLogicX() + m_walldirx * 2;
		int ly = pBuilding->getLogicY() + m_walldiry * 2;
		if(nums <= 4 && !canBuilding(pBuilding->m_buildingid, lx, ly, 2))
		{
			nextdir += 2;
			if(nextdir >= 8)
				nextdir = 0;
            
            countOffPosWithWall(nextdir, m_walldirx, m_walldiry);
            
			goto __rechg;
		}
        
		if(isValidBuildingPos(pBuilding->m_buildingid, lx, ly, 2))
		{
            if (LogicInfo::getSingleton().canBuildingWall()) {
                LogicInfo::getSingleton().addBuilding(pBuilding->m_buildingid, pBuilding->getLogicX() + m_walldirx * 2, pBuilding->getLogicY() + m_walldiry * 2);
            }
		}
	}
    
    if(ischg) {
		rebuildMapArea(false);
    }
}

bool PlayerScene::isValidPos(int lx, int ly)
{
    if (lx >= 0 && lx < LOGIC_W && ly >= 0 && ly < LOGIC_W) {
        return true;
    }
    
    return false;
}

bool PlayerScene::isValidBuildingPos(int buildingid, int lx, int ly, int lw)
{
    if ((buildingid / 10000) == 9) {
        return lx >= (lw - 1) && ly >= (lw - 1) && lx < LOGIC_W && ly < LOGIC_W;
    }
    else {
        return lx >= (lw + 2 - 1) && ly >= (lw + 2 - 1) && lx <= LOGIC_W - 2 - 1 && ly <= LOGIC_W - 2 - 1;
    }
}

bool PlayerScene::isValidBuildingPosEx(int buildingid, int lx, int ly, int lw)
{
    if (!isValidBuildingPos(buildingid, lx, ly, lw)) {
        return false;
    }
    
    for (int x = lx - lw + 1; x < lx; x++) {
        for (int y = ly - lw + 1; y < ly; y++) {
            if (m_mapData.get(x, y) != 0) {
                return false;
            }
        }
    }
    
    return true;
}

//! 英雄释放技能
void PlayerScene::useHeroSkill(int id)
{
	if(m_pHero != NULL)
		m_pHero->useSkill(id);
}

bool PlayerScene::isHeroDie()
{
	if(m_pHero != NULL)
	{
		if(m_pHero->isEmptyHP() || m_pHero->isStatus(PERSON_STATUS_RELEASE))
			return true;
        
		return false;
	}
    
	return false;
}

Building_Wall* PlayerScene::isWall(int lx, int ly)
{
	for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if(!(*it)->isEmptyHP() && (*it)->m_info.type == BUILDING_TYPE_WALL && (*it)->isIn(lx, ly))
		{            
			return (Building_Wall*)*it;
		}
	}
    
	return NULL;
}

void PlayerScene::onIdle(int ot)
{
    if (!m_pRoot->isVisible()) {
        return;
    }
    
    CCPoint pos = m_pRoot->getPosition();
    onFixPos(pos.x, pos.y);
    
    if(m_pTouchMoving != NULL && m_pTouchMoving->isDone())
    {
        m_pTouchMoving->release();
        m_pTouchMoving = NULL;
    }
    
    if(m_isTouched)
        m_timeTouch += ot / 1000.0f;
    
    if(m_pRoot->onProcScaleAni(ot / 1000.0f))
    {
        m_scale = m_pRoot->getScale();
        
        if(m_pBuildUI != NULL)
        {
            m_pBuildUI->setScale(m_scale);
            m_pBuildUI->setPosition(m_pRoot->getPosition());
        }
    }
        
    while (m_vecPathCache.size() > MAXFRAME_SEARCHNUMS*5) {
        m_vecPathCache.erase(m_vecPathCache.begin());
    }
    
    PlayerScene::getSingleton().m_curFrameCanSearchNums = MAXFRAME_SEARCHNUMS;
    
    LogicInfo::getSingleton().onProcAsyncView();
    
    if(LogicInfo::getSingleton().isRealAttackMode())
    {
        onFightProc(ot);
        
        if(LogicInfo::getSingleton().m_isFinishAttack) {
            return;
        }
    }
    else if (LogicInfo::getSingleton().isReplayMode())
    {
        onReplayProc(ot);
        
        if(LogicInfo::getSingleton().m_isFinishAttack) {
            return;
        }
    }
    else
    {
        onNormalProc(ot);
    }
    
    onPersonProc(ot);
    
    onBuildingProc(ot);
    
    SkillMgr::getSingleton().onIdle(ot);
    
    AniObjMgr::getSingleton().onIdle(ot / 1000.0f);
    
    for(std::list<Projectile2*>::iterator it = m_lstProjectile2.begin(); it != m_lstProjectile2.end(); )
    {
        Projectile2* pProj = *it;
        if(pProj != NULL && pProj->isFinished())
        {
            pProj->m_isRelease = true;
            
            if(!pProj->m_strBoom.empty())
            {
                addBoomEx(pProj->m_strBoom.c_str(), 0, pProj->getPositionX(), pProj->getPositionY());
                
                if(pProj->m_bEarthquake)
                    setUpDownAction();
            }
            
            removeParticle(pProj);
            pProj->release();
            
            it = m_lstProjectile2.erase(it);
        }
        else {
            ++it;
        }
    }
    
    for(std::list<ParticleSystem*>::iterator it = m_lstParticle.begin(); it != m_lstParticle.end(); )
    {
        if((*it)->isEmpty())
        {
            m_pRoot->removeChild(*it, true);
            it = m_lstParticle.erase(it);
        }
        else {
            ++it;
        }
    }
    
    sortAllChildren();
}

void PlayerScene::onNormalProc(int ot)
{
    if((!m_isScaleMode && m_isTouched) && m_pCurBuilding != NULL && BUILDING_CTRL_MOVE != m_pCurBuilding->m_state && BUILDING_CTRL_SELECTED != m_pCurBuilding->m_state && BUILDING_CTRL_BUILD != m_pCurBuilding->m_state)
    {
        if (GuideMgr::getSingleton().m_pActiveBuilding == m_pCurBuilding) {
            return;
        }
        
        m_timeTouched -= ot;
        
        if(m_timeTouched <= 0)
        {
            int x = (m_ptTouchBegin.x + m_ptTouchOff.x - m_pRoot->getPosition().x) / getScale();
            int y = (m_ptTouchBegin.y + m_ptTouchOff.y - m_pRoot->getPosition().y) / getScale();
            
            CCPoint pt = countTilePos(x, y);
            
            if(fabs(m_ptTouchOff.x) <= 20 && fabs(m_ptTouchOff.y) <= 20 && isValidPos(pt.x, pt.y) && !m_isTouchBkMove)
            {
                if(m_pCurBuilding->isIn(pt.x, pt.y))
                {
                    if (m_isEraseMode) {
                        moveBuilding2EditList(*m_pCurBuilding);
                    }
                    else {
                        selectBuilding(*m_pCurBuilding);
                        setBuildingTouchMode(*m_pCurBuilding, true);
                    }
                }
            }
        }
    }
}

void PlayerScene::onPersonProc(int ot)
{
    for(int i = 0; i <= CAMP_WORKER; ++i)
    {
        for(_PersonList::iterator it = m_lstPerson[i].begin(); it != m_lstPerson[i].end(); ++it)
        {
            BaseObj* od = NULL;
            BaseObj* nd = NULL;
            
            bool needctrl = true;
            
            //! 记录行动前的目标
            if(!(*it)->isStatus(PERSON_STATUS_RELEASE) && !(*it)->isEmptyHP() && (*it)->isVisible())
            {
                od = (*it)->m_pDestTarget;
            }
            else
            {
                needctrl = false;
            }
            
            (*it)->onProc(ot);
            
            //! 记录行动后的目标
            if(!(*it)->isStatus(PERSON_STATUS_RELEASE) && !(*it)->isEmptyHP() && (*it)->isVisible())
            {
                nd = (*it)->m_pDestTarget;
            }
            else
            {
                needctrl = false;
            }
            
            if(needctrl && (*it)->m_personType != PERSON_TYPE_TOWER)
            {
                if(od != nd)
                {
                    if((*it)->m_curCamp == CAMP_ATTACK)
                    {
                        if(nd == NULL)
                            LogicInfo::getSingleton().onAddCtrl_ChgAtkDest((*it)->m_indexPerson, -1, 0);
                        else
                        {
                            if(nd->getObjType() == OBJTYPE_PERSON)
                                LogicInfo::getSingleton().onAddCtrl_ChgAtkDest((*it)->m_indexPerson, OBJTYPE_PERSON, ((Person*)nd)->m_indexPerson);
                            else
                                LogicInfo::getSingleton().onAddCtrl_ChgAtkDest((*it)->m_indexPerson, OBJTYPE_BUILDING, ((Building*)nd)->m_indexBuilding);
                        }
                    }
                    else
                    {
                        if(nd == NULL)
                            LogicInfo::getSingleton().onAddCtrl_ChgDefDest((*it)->m_indexPerson, -1, 0);
                        else
                        {
                            if(nd->getObjType() == OBJTYPE_PERSON)
                                LogicInfo::getSingleton().onAddCtrl_ChgDefDest((*it)->m_indexPerson, OBJTYPE_PERSON, ((Person*)nd)->m_indexPerson);
                            else
                                LogicInfo::getSingleton().onAddCtrl_ChgDefDest((*it)->m_indexPerson, OBJTYPE_BUILDING, ((Building*)nd)->m_indexBuilding);
                        }
                    }
                }
            }
        }
    }
    
    if(LogicInfo::getSingleton().isNormalMode()) {
        HeroMgr::getSingleton().onProcHero(ot);
    }
}

void PlayerScene::onBuildingProc(int ot)
{
    for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); )
    {
        (*it)->onProc(ot);
        
        if((*it)->m_isRemove)
        {
            chgSelectWallLine(*it, NULL);
            chgResBuilding(*it, NULL);
            
            (*it)->releaseFromScene();
            
            delete *it;
            
            it = m_lstBuilding.erase(it);
        }
        else
            ++it;
    }
    if (m_pBuildingClanAllies) {
        m_pBuildingClanAllies->onProc(ot);
    }
}

void PlayerScene::onFightProc(int ot)
{
    if (LogicInfo::getSingleton().m_fightMode == FIGHT_MODE_BEGIN) {
        LogicInfo::getSingleton().m_fightBeginTimeMS += ot;
    }
    
    if(!LogicInfo::getSingleton().m_isFinishAttack && LogicInfo::getSingleton().isFinishAttack())
    {
        onWarEnd();
        
        chg2Win();
        
        LogicInfo::getSingleton().onNotify(NOTIFY_FIGHT_END);
        
        if (GuideMgr::getSingleton().isEquVal(GUIDE_DEFEND_TOWNHALL)) {
            LogicInfo::getSingleton().m_curState = LOGICSTATE_NORMAL;
            GuideMgr::getSingleton().next();
        }
    }
    
    if(m_isTouched)
    {
        m_timeTouched -= ot;
        
        if(m_timeTouched <= 0)
        {
            m_timeTouched = 80 * 2;
            
            if(m_isCanHoldDropSoldier && (PlayerScene::getSingleton().m_bSelClanSoldier || m_curSelPersonID > 0))
            {
                m_isCanDropSoldier = true;
                m_isHoldDropSoldier = true;
                
                std::map<int, _TouchNode>::iterator iter = m_pGameWorld->m_pGameLayer->m_lstTouch.begin();
                for (; iter != m_pGameWorld->m_pGameLayer->m_lstTouch.end(); ++iter)
                {
                    onDropSoldier2Attack(iter->second.x, iter->second.y);
                }
            }
        }
    }
}

void PlayerScene::onReplayProc(int ot)
{
    m_replayReadyTime -= ot;
    m_replayTime -= ot;
    
    if (m_replayReadyTime > 0) {
        return;
    }
    
    if (m_replayTime < 0) {
        m_replayTime = 0;
    }
    
    if(m_curCtrlIndex < LogicInfo::getSingleton().m_logFight.ctrlIndex + LogicInfo::getSingleton().m_logFight.numsCtrl)
    {
        while(m_beginReplayTime + ot >= LogicInfo::getSingleton().m_logFight.node[m_curCtrlIndex].ctrl)
        {
            FightCtrlNode ctrl = LogicInfo::getSingleton().m_logFight.node[m_curCtrlIndex];
            
            int type = ctrl.type;
            int cid = ctrl.cid;
            
            if(type == CTRL_TYPE_DROP_SOLDIER)
            {
                dropPerson2Attack(cid, ctrl.x, ctrl.y);
                
                LogicInfo::getSingleton().addUsedFightSoldier(cid);
            }
            else if(type == CTRL_TYPE_DESTROY_BUILDING)
            {
                Building* pBuilding = findBuildingWithIndex(cid);
                if(pBuilding != NULL)
                {
                    pBuilding->chgHP(-pBuilding->getHP(), false, true);
                }
            }
            else if(type == CTRL_TYPE_KILL_DEFEND_SOLDIER)
            {
                Person* pPerson = findPersonWithIndex(CAMP_DEFENCE, cid);
                if(pPerson != NULL)
                {
                    pPerson->chgHP(-pPerson->getHP(), false, true);
                }
            }
            else if(type == CTRL_TYPE_KILL_ATTACK_SOLDIER)
            {
                Person* pPerson = findPersonWithIndex(CAMP_ATTACK, cid);
                if(pPerson != NULL)
                {
                    pPerson->chgHP(-pPerson->getHP(), false, true);
                }
            }
            else if (type == CTRL_TYPE_USE_CLAN_SOLDIER)
            {
                int buildid = BUILDING_CLAN_ALLIES*100 + 1;
                
                m_pBuildingClanAllies = BuildingMgr::getSingleton().newBuilding(buildid);
                m_pBuildingClanAllies->init(buildid, ctrl.x, ctrl.y);
                m_pBuildingClanAllies->m_indexBuilding = ctrl.state;
                
                LogicInfo::getSingleton().addUsedFightSoldier(CLAN_SOLDIER_ID);
            }
            else if (type == CTRL_TYPE_CLAN_SOLDIER_ATTACK)
            {
                dropPerson2Attack(cid, ctrl.x, ctrl.y);
            }
            else if(type == CTRL_TYPE_CLAN_SOLDIER_DEFEND)
            {
                Building* pBuilding = findBuildingWithIndex(cid);
                
                if(pBuilding != NULL)
                {
                    addDefenceSoldier(pBuilding, ctrl.x, ctrl.y);
                }
            }
            else if(type == CTRL_TYPE_USESKILL)
            {
                Person* pPerson = findPersonWithIndex(CAMP_ATTACK, cid);
                if(pPerson != NULL)
                {
                    pPerson->useSkill(ctrl.state);
                }
            }
            else if(type == CTRL_TYPE_CHANGE_ATTACK_TARGET)
            {
                int bpindex = cid / 1000;
                int epindex = cid % 1000;
                
                for(int i = bpindex; i <= epindex; ++i)
                {
                    Person* pPerson = findPersonWithIndex(CAMP_ATTACK, i);
                    if(pPerson != NULL)
                    {
                        int destype = ctrl.x;
                        int destindex = ctrl.y;
                        
                        if(destype == -1)
                        {
                            pPerson->chgDest_Building(NULL);
                        }
                        else if(destype == OBJTYPE_PERSON)
                        {
                            //! 医生特殊处理
                            Person* dperson = NULL;
                            
                            if(pPerson->m_personid / 100 == PERSON_HEALER)
                                dperson = findPersonWithIndex(CAMP_ATTACK, destindex);
                            else
                                dperson = findPersonWithIndex(CAMP_DEFENCE, destindex);
                            
                            if(dperson != NULL)
                                pPerson->chgDest_Person(dperson);
                        }
                        else
                        {
                            Building* building = findBuildingWithIndex(destindex);
                            
                            if(building != NULL)
                                pPerson->chgDest_Building(building);
                        }
                    }
                }
            }
            else if(type == CTRL_TYPE_CHANGE_DEFEND_TARGET)
            {
                int bpindex = cid / 1000;
                int epindex = cid % 1000;
                
                for(int i = bpindex; i <= epindex; ++i)
                {
                    Person* pPerson = findPersonWithIndex(CAMP_DEFENCE, i);
                    if(pPerson != NULL)
                    {
                        int destype = ctrl.x;
                        int destindex = ctrl.y;
                        
                        if(destype == -1)
                        {
                            pPerson->chgDest_Building(NULL);
                        }
                        else if(destype == OBJTYPE_PERSON)
                        {
                            //! 医生特殊处理
                            Person* dperson = NULL;
                            
                            if(pPerson->m_personid / 100 == PERSON_HEALER)
                                dperson = findPersonWithIndex(CAMP_DEFENCE, destindex);
                            else
                                dperson = findPersonWithIndex(CAMP_ATTACK, destindex);
                            
                            if(dperson != NULL)
                                pPerson->chgDest_Person(dperson);
                        }
                        else
                        {
                            Building* building = findBuildingWithIndex(destindex);
                            
                            if(building != NULL)
                                pPerson->chgDest_Building(building);
                        }
                    }
                }
            }
            
            ++m_curCtrlIndex;
            if(m_curCtrlIndex >= LogicInfo::getSingleton().m_logFight.ctrlIndex + LogicInfo::getSingleton().m_logFight.numsCtrl)
            {
                break;
            }
        }
    }
    else
    {
        if(!LogicInfo::getSingleton().m_isFinishAttack)
        {
            LogicInfo::getSingleton().m_isFinishAttack = true;
            LogicInfo::getSingleton().m_infoFightScore = LogicInfo::getSingleton().m_logFightMsg.fsi;
            
            onWarEnd();
            chg2Win();
        }
        
        if(m_beginReplayTime >= LogicInfo::getSingleton().m_logFight.node[LogicInfo::getSingleton().m_logFight.ctrlIndex + LogicInfo::getSingleton().m_logFight.numsCtrl - 1].ctrl + 1000)
        {
            LogicInfo::getSingleton().onNotify(NOTIFY_REPLAY_OK);
        }
    }
    
    m_beginReplayTime += ot;
}

Person* PlayerScene::addPerson(int personid, int lx, int ly, int act, int camp)
{
	Person* pPerson = PersonMgr::getSingleton().newPerson(personid, camp);
	if(pPerson != NULL)
	{
		int skill0;
		
		if(personid / 10000 == 3)
			skill0 = HeroMgr::getSingleton().m_myHero.skill3;
		else
			skill0 = 0;
        
		pPerson->init(lx, ly, act, skill0);
        
		m_lstPerson[camp].push_back(pPerson);
        
		if(personid / 10000 == 3)
        {
            if (m_pHero){
                m_pHero->releaseFromScene();
            }
            
            if(m_pBuildingHeroAltar != NULL)
            {
                pPerson->m_defendRange = m_pBuildingHeroAltar->m_info.attackrange;
            }
            if(LogicInfo::getSingleton().isNormalMode())
                m_pHero = pPerson;
            else
                m_pOtherHero = pPerson;
        }
        
		return pPerson;
	}
    
	return NULL;
}

Person* PlayerScene::addTowerPerson(int personid, int lx, int ly)
{
	Person* pPerson = PersonMgr::getSingleton().newPerson(personid, CAMP_DEFENCE);
	if(pPerson != NULL)
	{
		int skill0;
		
		if(personid / 10000 == 3)
			skill0 = HeroMgr::getSingleton().m_myHero.skill3;
		else
			skill0 = 0;
        
        pPerson->m_personType = PERSON_TYPE_TOWER;
		pPerson->init(lx, ly, PERSON_ACT_NORMAL, skill0);
        
		pPerson->m_indexPerson = (++LogicInfo::getSingleton().m_curPersonDef);
        
		m_lstPerson[CAMP_DEFENCE].push_back(pPerson);
        
		return pPerson;
	}
    
	return NULL;
}

void PlayerScene::sortAllChildren()
{
    if (m_pRoot->isReorderChildDirty()) {
     
        int i,j,length = m_pRoot->getChildren()->data->num;
        CCNode ** x = (CCNode**)m_pRoot->getChildren()->data->arr;
        CCNode *tempItem;
        
        // insertion sort
        for(i=1; i<length; i++)
        {
            tempItem = x[i];
            j = i-1;
            
            //continue moving element downwards while zOrder is smaller or when zOrder is the same but mutatedIndex is smaller
            while(j>=0 && ( tempItem->getZOrder() < x[j]->getZOrder()) )
            {
                x[j+1] = x[j];
                j = j-1;
            }
            x[j+1] = tempItem;
        }
        
        m_pRoot->setReorderChildDirty(false);
    }
}

int PlayerScene::getAreaIndex(int x, int y)
{
	if(x < 0 || x >= LOGIC_W || y < 0 || y >= LOGIC_W)
		return -1;
    
	return m_astar.m_mapArea.get(x, y);
}

void PlayerScene::rebuildMapArea(bool toAttack)
{
//    if(!toAttack)
//        m_astar.rebuildMapArea();
    
    for(int y = 0; y < m_mapDataOcc.m_height; ++y)
    {
        for(int x = 0; x < m_mapDataOcc.m_width; ++x)
        {
            int occ = m_mapDataOcc.get(x, y);
            if(occ != 0)
            {
                int tocc[4] = {0, 0, 0, 0};
                int t = 0;
                
                if(y <= 0 || m_mapDataOcc.get(x, y - 1) == 0)
                {
                    tocc[0] = 1;
                    ++t;
                }
                
                if(y >= m_mapDataOcc.m_height - 1 || m_mapDataOcc.get(x, y + 1) == 0)
                {
                    tocc[2] = 1;
                    ++t;
                }
                
                if(x <= 0 || m_mapDataOcc.get(x - 1, y) == 0)
                {
                    tocc[3] = 1;
                    ++t;
                }
                
                if(x >= m_mapDataOcc.m_width - 1 || m_mapDataOcc.get(x + 1, y) == 0)
                {
                    tocc[1] = 1;
                    ++t;
                }
                
                if(t > 2)
                    continue;
                
                if(tocc[0] == 0 && tocc[1] == 0 && tocc[2] == 0 && tocc[3] == 0)
                {
                    t = 0;
                    
                    if(y <= 0 || x >= m_mapDataOcc.m_width - 1 || m_mapDataOcc.get(x + 1, y - 1) == 0)
                    {
                        tocc[0] = 1;
                        ++t;
                    }
                    else
                        tocc[0] = 0;
                    
                    if(y >= m_mapDataOcc.m_height - 1 || x >= m_mapDataOcc.m_width - 1 || m_mapDataOcc.get(x + 1, y + 1) == 0)
                    {
                        tocc[1] = 1;
                        ++t;
                    }
                    else
                        tocc[1] = 0;
                    
                    if(y >= m_mapDataOcc.m_height - 1 || x <= 0 || m_mapDataOcc.get(x - 1, y + 1) == 0)
                    {
                        tocc[2] = 1;
                        ++t;
                    }
                    else
                        tocc[2] = 0;
                    
                    if(y <= 0 || x <= 0 || m_mapDataOcc.get(x - 1, y - 1) == 0)
                    {
                        tocc[3] = 1;
                        ++t;
                    }
                    else
                        tocc[3] = 0;
                    
                    if(t != 1)
                        continue;
                    
                    char str[32];
                    
                    if(toAttack)
                    {
                        sprintf(str, "rtt%d%d%d%d", tocc[0], tocc[1], tocc[2], tocc[3]);
                        
                        setAreaSpr(x, y, str, toAttack);
                    }
                    else
                    {
                        sprintf(str, "wtt%d%d%d%d", tocc[0], tocc[1], tocc[2], tocc[3]);
                        
                        setAreaSpr(x, y, str, toAttack);
                    }
                    
                    continue;
                }
                
                char str[32];
                
                if(toAttack)
                {
                    sprintf(str, "rt%d%d%d%d", tocc[0], tocc[1], tocc[2], tocc[3]);
                    
                    setAreaSpr(x, y, str, toAttack);
                }
                else
                {
                    sprintf(str, "wt%d%d%d%d", tocc[0], tocc[1], tocc[2], tocc[3]);
                    
                    setAreaSpr(x, y, str, toAttack);
                }
            }
        }
    }
}

void PlayerScene::setAreaSpr(int x, int y, const char* name, bool toAttack)
{
	if(m_mapArea[y][x] != NULL)
	{
		m_pTileBatchNode->removeChild(m_mapArea[y][x], true);
		m_mapArea[y][x]->release();
	}
    
	CCSprite* pSpr = ImgMgr::getSingleton().loadSprite(name);
    
	pSpr->setAnchorPoint(ccp(0, 0));
	pSpr->setPosition(countScenePos(x, y));
	pSpr->retain();
    
	m_pTileBatchNode->addChild(pSpr, SCENE_Z_TILEAREA);
    
	m_mapArea[y][x] = pSpr;
    
	if(!toAttack)
	{
		CCFadeOut* pFo = CCFadeOut::create(2.0f);
		pSpr->runAction(pFo);
	}
}

void PlayerScene::showArrow(int lx, int ly, int lw)
{
	if(!m_isInit)
		return;
    
    bool arrowdir[4] = {true, true, true, true};
    bool showAction = false;
    
    if (m_isEditMode && m_pCurBuilding != NULL && m_pCurBuilding->m_buildingid/100 == BUILDING_WALL) {
        
        Building_Wall* pWall = (Building_Wall*)m_pCurBuilding;
        
        for (int i = 0; i < 4; i ++) {
            if (pWall->m_pWallEdit[i*2+1] == NULL) {
                arrowdir[i] = false;
            }
        }
        
        lw = 10;
        lx += lw/2-1;
        ly += lw/2-1;
        
        showAction = true;
    }
    
    CCPoint pos = countScenePos(lx, ly);
    
    m_pArrow[0]->stopAllActions();
    m_pArrow[0]->setVisible(false);
    if(m_pArrow[0] != NULL && arrowdir[0])
    {
        m_pArrow[0]->setPosition(ccp(pos.x + lw * TILE_W / 4 - (lw - 1) * TILE_W / 2, pos.y + lw * TILE_H * 3 / 4));
        m_pArrow[0]->setVisible(true);
    }
    
    m_pArrow[1]->stopAllActions();
    m_pArrow[1]->setVisible(false);
    if(m_pArrow[1] != NULL && arrowdir[1])
    {
        m_pArrow[1]->setPosition(ccp(pos.x + lw * 3 * TILE_W / 4 - (lw - 1) * TILE_W / 2, pos.y + lw * TILE_H * 3 / 4));
        m_pArrow[1]->setVisible(true);
    }
    
    m_pArrow[2]->stopAllActions();
    m_pArrow[2]->setVisible(false);
    if(m_pArrow[2] != NULL && arrowdir[2])
    {
        m_pArrow[2]->setPosition(ccp(pos.x + lw * 3 * TILE_W / 4 - (lw - 1) * TILE_W / 2, pos.y + lw * TILE_H / 4));
        m_pArrow[2]->setVisible(true);
    }
    
    m_pArrow[3]->stopAllActions();
    m_pArrow[3]->setVisible(false);
    if(m_pArrow[3] != NULL && arrowdir[3])
    {
        m_pArrow[3]->setPosition(ccp(pos.x + lw * TILE_W / 4 - (lw - 1) * TILE_W / 2, pos.y + lw * TILE_H / 4));
        m_pArrow[3]->setVisible(true);
    }
 
    if (showAction) {
        CCPoint movePos[4] = {ccp(-10, 10), ccp(10, 10), ccp(10, -10), ccp(-10, -10)};
        
        for (int i = 0; i < 4; i ++) {
            if (m_pArrow[i] != NULL && m_pArrow[i]->isVisible()) {
                
                CCMoveTo* pMoveTo1 = CCMoveTo::create(0.5, ccp(m_pArrow[i]->getPositionX() + movePos[i].x, m_pArrow[i]->getPositionY() + movePos[i].y));
                CCMoveTo* pMoveTo2 = CCMoveTo::create(0.5, m_pArrow[i]->getPosition());
                CCSequence* pAction = CCSequence::create(pMoveTo1, pMoveTo2, CCDelayTime::create(0), NULL);
                m_pArrow[i]->runAction(CCRepeatForever::create(pAction));
            }
        }
    }
}

void PlayerScene::hideArrow()
{
	for(int i = 0; i < 4; ++i)
	{
        if(m_pArrow[i] != NULL) {
			m_pArrow[i]->setVisible(false);
            m_pArrow[i]->stopAllActions();
        }
	}
}

void PlayerScene::addSelectBuildingListener(SelectBuildingListener* pListener)
{
	m_lstSelectBuildingListener.push_back(pListener);
}

void PlayerScene::removeSelectBuildingListener(SelectBuildingListener* pListener)
{
	if(m_isProcListener)
		pListener->m_isRelease = true;
	else
	{
		for(std::vector<SelectBuildingListener*>::iterator it = m_lstSelectBuildingListener.begin(); it != m_lstSelectBuildingListener.end(); )
		{
			if(*it == pListener)
			{
				it = m_lstSelectBuildingListener.erase(it);
			}
			else
				++it;
		}
	}
}

void PlayerScene::onBuildingSelect(bool isSelected, Building* pBuilding)
{
	m_isProcListener = true;
    
	for(std::vector<SelectBuildingListener*>::iterator it = m_lstSelectBuildingListener.begin(); it != m_lstSelectBuildingListener.end(); )
	{
		(*it)->onSelect(isSelected, pBuilding);
		if((*it)->m_isRelease)
			it = m_lstSelectBuildingListener.erase(it);
		else
			++it;
	}
    
	m_isProcListener = false;
}

void PlayerScene::shootGold(Building& building, int nums)
{
	building.shootGold(nums);
}

void PlayerScene::shootOil(Building& building, int nums)
{
	building.shootOil(nums);
}

void PlayerScene::onRemoveBuilding(UserBuildingInfo* pUBI)
{
	if(m_pCurBuilding != NULL && m_pCurBuilding->getLogicX() == pUBI->x && m_pCurBuilding->getLogicY() == pUBI->y)
	{
		hideArrow();
		m_pCurBuilding = NULL;
        
		onBuildingSelect(false, NULL);
	}
    
    for(_BuildingList::iterator it = m_lstResBuilding.begin(); it != m_lstResBuilding.end(); ++it)
    {
        if ((*it)->m_pBuildingInfo == pUBI && (*it)->m_buildingid == pUBI->buildingid)
        {
            m_lstResBuilding.erase(it);
            break;
        }
    }
    
    for(_BuildingList::iterator it = m_lstWallBuilding.begin(); it != m_lstWallBuilding.end(); ++it)
    {
        if ((*it)->m_pBuildingInfo == pUBI && (*it)->m_buildingid == pUBI->buildingid)
        {
            m_lstWallBuilding.erase(it);
            break;
        }
    }
    
	for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
        if ((*it)->m_pBuildingInfo == pUBI && (*it)->m_buildingid == pUBI->buildingid)
		{
			if((*it)->m_isRemove)
				return;
            
			chgSelectWallLine(*it, NULL);
			chgResBuilding(*it, NULL);
            
            if ((*it)->m_pTileColor == NULL) {
                onSetLogicMap((*it)->getLogicX(), (*it)->getLogicY(), (*it)->getLogicWidth(), (*it)->m_buildingid, 0);
            }
            
			(*it)->releaseFromScene();
                        
			delete *it;
            
			m_lstBuilding.erase(it);
            
			return;
		}
	}
}

Building* PlayerScene::onUpdateBuilding(UserBuildingInfo* pUBI)
{
    for(_BuildingList::iterator it = m_lstResBuilding.begin(); it != m_lstResBuilding.end(); ++it)
    {
        if ((*it)->m_pBuildingInfo == pUBI)
        {
            m_lstResBuilding.erase(it);
            break;
        }
    }
    
    for(_BuildingList::iterator it = m_lstWallBuilding.begin(); it != m_lstWallBuilding.end(); ++it)
    {
        if ((*it)->m_pBuildingInfo == pUBI)
        {
            m_lstWallBuilding.erase(it);
            break;
        }
    }
    
	for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
        if ((*it)->m_pBuildingInfo == pUBI)
		{
			if((*it)->m_buildingid != pUBI->buildingid)
			{
				Building* ob = NULL;
				Building* nb = NULL;
                
				{
					ob = *it;
					Building* pBuilding = *it;
                    
					m_lstBuilding.erase(it);
                    
					pBuilding->releaseFromScene();
					delete pBuilding;
				}
                
                int bid = pUBI->buildingid/100;
                
				Building* pBuilding = BuildingMgr::getSingleton().newBuilding(pUBI->buildingid);
				pBuilding->init(pUBI->buildingid, pUBI->x, pUBI->y);
                
				pBuilding->m_pBuildingInfo = pUBI;
				int fulltime = ((pBuilding->m_info.buildtime[0] * 24 + pBuilding->m_info.buildtime[1]) * 60 + pBuilding->m_info.buildtime[2]) * 60;
				pBuilding->setTime(fulltime, pUBI->lasttime);
                
				if(pUBI->currestime > 0 && pUBI->timeres > 0)
				{
					pBuilding->m_curResTime = pUBI->currestime;
                    
					if(pBuilding->hasNewRes())
						pBuilding->showResTitle();
				}
                
				m_pCurBuilding = pBuilding;
				m_pCurBuilding->chgState(BUILDING_CTRL_MOVE);
				m_pCurBuilding->setLogicPos(pUBI->x, pUBI->y);
                
				m_pCurBuilding->m_indexBuilding = (++LogicInfo::getSingleton().m_curBuildingIndex);
				m_lstBuilding.push_back(pBuilding);
                
                if (bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_GOLD_MINE) {
                    m_lstResBuilding.push_back(pBuilding);
                }
                else if (bid == BUILDING_WALL) {
                    m_lstWallBuilding.push_back(pBuilding);
                    ((Building_Wall*)pBuilding)->rebuildWall();
                }
                else if (bid == BUILDING_TOWN_HALL) {
                    m_pTownHall = m_pCurBuilding;
                }
                else if (bid == BUILDING_CLAN_CASTLE) {
                    m_pBuildingClan = m_pCurBuilding;
                    ClanMgr::getSingleton().m_myMaxSpace = pBuilding->m_info.housingspace;
                    ClanMgr::getSingleton().m_castleLevel = pBuilding->m_buildingid % 100;
                }
                
				nb = pBuilding;
                
				chgSelectWallLine(ob, nb);
				chgResBuilding(ob, nb);
                
				return m_pCurBuilding;
			}
            
			(*it)->m_pBuildingInfo = pUBI;
            
			int fulltime = (((*it)->m_info.buildtime[0] * 24 + (*it)->m_info.buildtime[1]) * 60 + (*it)->m_info.buildtime[2]) * 60;
			(*it)->setTime(fulltime, pUBI->lasttime);
            
			if(pUBI->state == 2)//pUBI->currestime > 0 && pUBI->timeres > 0)
			{
				(*it)->m_curResTime = pUBI->currestime;
                
				if((*it)->hasNewRes())
					(*it)->showResTitle();
			}
            
			return (*it);
		}
	}
    
	return NULL;
}

void PlayerScene::clearNormalSoldier()
{
	for(int i = 0; i < 2; ++i)
	{
		for(_PersonList::iterator it = m_lstPerson[i].begin(); it != m_lstPerson[i].end(); )
		{
			Person* pPerson = *it;
            
			if(pPerson->m_personType == PERSON_TYPE_SOLDIER)
			{
				pPerson->releaseFromScene();
                
				delete pPerson;
                
				it = m_lstPerson[i].erase(it);
			}
			else
				++it;
            
			//it = m_lstBuilding.erase(it);
		}
	}
}

void PlayerScene::clearScene()
{
	clearSelectWallLine();
    
    m_pBuildingClan = NULL;
	m_pBuildingHeroAltar = NULL;
	m_bDefenceHeroFight = false;
    
    if (m_pBuildingClanAllies) {
        m_pBuildingClanAllies->releaseFromScene();
        m_pBuildingClanAllies = NULL;
    }
    
	AniObjMgr::getSingleton().cleanup();
	//releaseScreenMask();
    
    GuideMgr::getSingleton().hideArrow();
    
	m_pHero = NULL;
	m_pCurBuilding = NULL;
    
    for(std::list<Projectile2*>::iterator it = m_lstProjectile2.begin(); it != m_lstProjectile2.end(); ++it)
	{
		Projectile2* pProj = *it;
        
        removeParticle(pProj);
        //m_pRoot->removeChild(pProj, true);
        pProj->release();
    }
    
    m_lstProjectile2.clear();
    
	for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		Building* pBuilding = *it;
        
		pBuilding->releaseFromScene();
        
		delete pBuilding;
	}
    
	m_lstBuilding.clear();
    m_lstResBuilding.clear();
    m_lstWallBuilding.clear();
    
	for(int i = 0; i < 3; ++i)
	{
		for(_PersonList::iterator it = m_lstPerson[i].begin(); it != m_lstPerson[i].end(); ++it)
		{
			Person* pPerson = *it;
            
			pPerson->releaseFromScene();
            
			delete pPerson;
            
			//it = m_lstBuilding.erase(it);
		}
        
		m_lstPerson[i].clear();
	}
    
	for(std::list<ParticleSystem*>::iterator it = m_lstParticle.begin(); it != m_lstParticle.end(); ++it)
	{
		(*it)->clear();
        
		m_pRoot->removeChild(*it, true);
	}
    
	m_lstParticle.clear();
    
	for(std::list<_TombInfo>::iterator it = m_lstTomb.begin(); it != m_lstTomb.end(); ++it)
	{
		m_pRoot->removeChild(it->pTomb, true);
	}
    
	m_lstTomb.clear();
    
	for(int y = 0; y < LOGIC_W; ++y)
	{
		for(int x = 0; x < LOGIC_W; ++x)
		{
			if(m_mapArea[y][x] != NULL)
			{
				m_pTileBatchNode->removeChild(m_mapArea[y][x], true);
				m_mapArea[y][x]->release();
                
				m_mapArea[y][x] = NULL;
			}
		}
	}
    
	if(m_pTileBatchNode != NULL)
		m_pTileBatchNode->removeAllChildrenWithCleanup(true);
    
	if(m_pParticleBatchNode != NULL)
		m_pParticleBatchNode->removeAllChildrenWithCleanup(true);
    
    AniObjMgr::getSingleton().releaseAllAni();
}

void PlayerScene::deleteBuilding(Building* pBuilding)
{
	if(pBuilding == m_pCurBuilding)
		m_pCurBuilding = NULL;
    
	for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if(*it == pBuilding)
		{
			chgSelectWallLine(*it, NULL);
			chgResBuilding(*it, NULL);
            
			pBuilding->releaseFromScene();
            
			delete pBuilding;
            
			pBuilding = NULL;
            
			m_lstBuilding.erase(it);
            
			return ;
		}
	}
}

void PlayerScene::onInitOther()
{
	setReplaySpeed(1);
    
	m_isInit = false;
    onRefurbishLogicMap();
    
	for(std::list<UserBuildingInfo*>::iterator it = LogicInfo::getSingleton().m_otherBuilding.begin(); it != LogicInfo::getSingleton().m_otherBuilding.end(); ++it)
	{
		addBuildingEx((*it)->buildingid, (*it)->x, (*it)->y, *it);
		if(m_pCurBuilding == NULL)
			continue;
        
		if((*it) != NULL && (*it)->state == 1)
			m_pCurBuilding->showFence(true);
        
		if((*it)->currestime > 0 && (*it)->timeres > 0)
		{
			m_pCurBuilding->m_curResTime = (*it)->currestime;
            
			if(m_pCurBuilding->hasNewRes())
				m_pCurBuilding->showResTitle();
		}
        
        int bid = (*it)->buildingid / 100;
        
        if (bid == BUILDING_TOWN_HALL) {
            m_pTownHall = m_pCurBuilding;
        }
        else if (bid == BUILDING_GOLD_MINE || bid == BUILDING_ELIXIR_COLLECTOR) {
            m_lstResBuilding.push_back(m_pCurBuilding);
        }
        else if (bid == BUILDING_WALL) {
            m_lstWallBuilding.push_back(m_pCurBuilding);
        }
        
        if ((*it)->buildingid / 100 == BUILDING_HERO_ALTAR) {
            m_pBuildingHeroAltar = m_pCurBuilding;
            m_bDefenceHeroFight = false;
        }
        
		Building* pBuilding = m_pCurBuilding;
        
		buildCurBuilding();
        
        if(m_pCurBuilding != NULL) {
			cancelCurBuilding();
        }
		else if(pBuilding->m_state == BUILDING_CTRL_MOVE)
		{
			m_pCurBuilding = pBuilding;
			cancelCurBuilding();
		}
	}
    
	m_isInit = true;
    
	LogicInfo::getSingleton().onInitOtherTroophousingSoldier();
    
	centerOfTownhall();
    
	m_astar.rebuildMapArea();
    rebuildMapArea(true);
    
    if (LogicInfo::getSingleton().isVisitMode()) {
        initHero(HeroMgr::getSingleton().m_otherHero);
    }
    
    if(LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_PVE)
    {
        EnvStageInfo* pInfo = EnvMapMgr::getSingleton().getEnvStageInfo(LogicInfo::getSingleton().m_pveInfo.curstage);
        if(pInfo != NULL)
        {
            LogicInfo::getSingleton().m_otherInfo.maxgoldex = pInfo->gold;
            LogicInfo::getSingleton().m_otherInfo.maxoilex = pInfo->oil;
            
            LogicInfo::getSingleton().m_otherInfo.goldex = LogicInfo::getSingleton().m_otherInfo.maxgoldex;
            LogicInfo::getSingleton().m_otherInfo.oilex = LogicInfo::getSingleton().m_otherInfo.maxoilex;
        }
    }
    else if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_GROUPWAR)
    {
        for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
        {
            Building* pBuilding = *it;
            
            float ratio = GroupWarMgr::getSingleton().m_warUserInfo.defenseRatio;
            
            pBuilding->m_info.hp *= ratio;
            pBuilding->m_info.damage *= ratio;
            pBuilding->setHP(pBuilding->getMaxHP());
        }
    }
    
    LogicInfo::getSingleton().fixStorageReserve(false);
}

void PlayerScene::onUpdateBarrack(BarrackInfo& barrack)
{
	for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if((*it)->m_pBuildingInfo != NULL && (*it)->m_pBuildingInfo->barrackid == barrack.barrackid)
		{
			if(!barrack.lstSoldier.empty())
			{
				(*it)->setTime(barrack.lstSoldier.front().needtimeex, barrack.lasttime);
			}
		}
	}
}

void PlayerScene::onInitTroophousing(TroophousingInfo& troophousing, bool isOther, bool isOnlyHero)
{
	for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if((*it)->m_pBuildingInfo != NULL && (*it)->m_pBuildingInfo->troophousingid == troophousing.troophousingid)
		{
			int curindex = 0;
            
			for(std::list<TroophousingSoldierInfo>::iterator it1 = troophousing.lstSoldier.begin(); it1 != troophousing.lstSoldier.end(); ++it1)
			{
				for(int i = 0; i < it1->numsex; ++i)
				{
					if(LogicInfo::getSingleton().canRecruitSoldier(it1->pid, isOther))
					{
						CCPoint pt;
						countCurPosTroophousing(pt, curindex);
						Person* pPerson = addPerson(it1->pid, (*it)->getLogicX() - pt.x, (*it)->getLogicY() - pt.y, PERSON_ACT_NORMAL, CAMP_DEFENCE);
						pPerson->m_personType = PERSON_TYPE_SOLDIER;
                        
						pPerson->m_ptSleep.x = (*it)->getLogicX() - pt.x;
						pPerson->m_ptSleep.y = (*it)->getLogicY() - pt.y;
                        
						pPerson->m_pTroophousing = (*it)->m_pBuildingInfo;
						pPerson->m_tlx = pt.x;
						pPerson->m_tly = pt.y;
                        
						++curindex;
					}
				}
			}
		}
	}
}

void PlayerScene::onSetLogicMap(int lx, int ly, int lw, int buildingid, int value, bool isUpdOccData)
{
    m_mapData.set(lx - lw + 1, ly - lw + 1, lw, lw, value);
    
    if(isUpdOccData && buildingid < 50000)
    {
        int x = lx - lw - 1;
        int y = ly - lw - 1;
            
        int addval = 1;
        if (value == 0) {
            addval = -1;
        }
        
        for(int ty = y; ty < y + lw + 4; ++ty)
        {
            for(int tx = x; tx < x + lw + 4; ++tx)
            {
                if ((addval == 1) || (addval == -1 && m_mapDataOcc.get(tx, ty) > 0)) {
                    m_mapDataOcc.add(tx, ty, addval);
                }
            }
        }
    }
    
    if(buildingid/100 == BUILDING_WALL || (buildingid >= 90000 && lw == 2))
        m_astar.m_map.set(lx - lw + 1, ly - lw + 1, lw, lw, value);
    else if(buildingid/100 == BUILDING_ARMY_CAMP)
        m_astar.m_map.set(lx - lw + 5, ly - lw + 5, lw - 8, lw - 8, value);
    else
        m_astar.m_map.set(lx - lw + 2, ly - lw + 2, lw - 2, lw - 2, value);
}

void PlayerScene::onRefurbishLogicMap()
{
	m_astar.m_map.set(0, 0, LOGIC_W, LOGIC_W, 0);
	m_astar.m_mapSoldier.set(0, 0, LOGIC_W, LOGIC_W, 0);
    m_mapData.set(0, 0, LOGIC_W, LOGIC_W, 0);
    m_mapDataOcc.set(0, 0, LOGIC_W, LOGIC_W, 0);
    
	for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if((*it)->isEmptyHP())
			continue;
        
        onSetLogicMap((*it)->getLogicX(), (*it)->getLogicY(), (*it)->getLogicWidth(), (*it)->m_buildingid, (*it)->m_buildingid);
	}
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	m_astar.saveMap();
#endif
}

Building* PlayerScene::findTroophousing(int troophousingid)
{
	for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if((*it)->m_pBuildingInfo != NULL && (*it)->m_pBuildingInfo->troophousingid == troophousingid)
		{
			return *it;
		}
	}
    
	return NULL;
}

void PlayerScene::countCurPosTroophousing(CCPoint& pt, int curindex)
{
	curindex = rand() % 49;
    
	pt.x = 0;
	pt.y = 0;
    
	pt.y = curindex % 7;
	pt.x = curindex / 7;
    
	pt.x = 7 - pt.x;
	pt.y = 7 - pt.y;
}

Person* PlayerScene::dropPerson2Attack(int personid, int lx, int ly)
{
	Person* pPerson = PersonMgr::getSingleton().newPerson(personid, CAMP_ATTACK);
	if(pPerson != NULL)
	{
		int skill0;
		
		if(personid / 10000 == 3)
			skill0 = HeroMgr::getSingleton().m_myHero.skill3;
		else
			skill0 = 0;
        
		pPerson->init(lx, ly, PERSON_ACT_DROPDOWN, skill0);//PERSON_ACT_NORMAL);//PERSON_ACT_DROPDOWN);
        pPerson->m_personState = PERSON_STATE_ATTACK;
        
        pPerson->m_indexPerson = (++LogicInfo::getSingleton().m_curPersonAtk);
        
        if (personid >= PERSON_HERO *100) {
            SoundMgr::getSingleton().playEffect(CCString::createWithFormat("player_deploy_30%d.m4a", personid%10000/100)->getCString());
        }
        else {
            SoundMgr::getSingleton().playEffect(CCString::createWithFormat("player_deploy_%d.m4a", personid%10000/100)->getCString(), SOUND_DELAY);
        }
        
        if (GuideMgr::getSingleton().isEquVal(GUIDE_DEFEND_TOWNHALL)) { //防御城堡教程攻击目标优先防御建筑
            pPerson->m_attackTarget = ATTACK_TARGET_DEFENSE;
        }
        
        int pid = personid / 100;
        
        if (pid == PERSON_HEALER) {
            findNearSoldierWithAddHP(pPerson);
        }
        else {
            BaseObj *pNearPerson = findNextAttackPerson(pPerson, pPerson->isFlying());
            
            if (pNearPerson == NULL) {
                findNextAttackBuilding(pPerson, pPerson->isFlying());
            }
        }
        
        m_lstPerson[CAMP_ATTACK].push_back(pPerson);
        
        if(pid == PERSON_HERO)
        {
            m_pHero = pPerson;
        }
        
        if (m_lstPerson[CAMP_WORKER].size() > 0) {
            for(_PersonList::iterator it = m_lstPerson[CAMP_WORKER].begin(); it != m_lstPerson[CAMP_WORKER].end(); ++it)
            {
                if (!(*it)->isStatus(PERSON_STATUS_MOVE_OUT)) {
                    
                    (*it)->m_speed *= 5;
                    
                    (*it)->chgDestTarget(m_pTownHall);
                    (*it)->move2LogicPos(m_pTownHall->getLogicX()-4, m_pTownHall->getLogicY());
                    (*it)->addPath(m_pTownHall->getLogicX()-4, m_pTownHall->getLogicY());
                    
                    (*it)->addStatus(PERSON_STATUS_MOVE_OUT);
                }
            }
        }
        
        return pPerson;
    }
    
	return NULL;
}

// type == 0墙以外的建筑，最近
// type == 1攻击建筑
// type == 2炸弹人的找墙
// type == 3资源
// type == 4远程的策略
// type == 5普通士兵的找墙策略
Building* PlayerScene::findNearAttackBuilding(Person* pPerson, int lx, int ly, int type)
{
	Building* pBuilding = NULL;
	int distance = 99999999.0f;
    
	//! 首先找最近的
	for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if(!LogicInfo::getSingleton().isCanDestroyBuidling((*it)->m_buildingid) || (*it)->isEmptyHP())
			continue;
        
		if((type == 0 || type == 4) && (*it)->m_type == BUILDING_TYPE_WALL)
			continue;
		if(type == 1 && !(*it)->isAttackBuilding())
			continue;
		if(type == 2 && ((*it)->m_type != BUILDING_TYPE_WALL))
			continue;
		if(type == 5 && (*it)->m_type != BUILDING_TYPE_WALL)
			continue;
		if(type == 3 && !(*it)->isResourceBuilding())
			continue;
        
		int dis = (*it)->countDistance(lx, ly);
        if (type == 2) {
            Building_Wall* pWall = (Building_Wall*)(*it);

            if (checkAttackWall(pPerson, pWall, NULL)) {
                
                if (pWall->m_beAttackTargetNums >= 1)
                {
                    dis *= 0.5;
                }
                else if (pWall->getWallDirNums() >= 2)
                {
                    dis *= 0.6;
                }
                else if (pBuilding != NULL && pWall->getHP() < pBuilding->getHP())
                {
                    dis *= 0.8;
                }
                
                if (dis < distance) {
                    distance = dis;
                    pBuilding = *it;
                }
            }
        }
        else
        {
            if(dis < distance)
            {
                distance = dis;
                pBuilding = *it;
            }
        }
	}
    
	return pBuilding;
}

// 查找最近的加血士兵
Person* PlayerScene::findNearSoldierWithAddHP(Person* pPerson)
{
    int camp = pPerson->m_curCamp;
    int lx = pPerson->getLogicX();
    int ly = pPerson->getLogicY();
    
    bool needAddHp = false;
    for(_PersonList::iterator it = m_lstPerson[camp].begin(); it != m_lstPerson[camp].end(); ++it)
	{
        if(!(*it)->isFlying() && !(*it)->isEmptyHP() && !(*it)->isFullHp()) {
            needAddHp = true;
            break;
        }
    }
    
	Person* pSoldier = NULL;
	float distance = 99999999.0f;
    
	for(_PersonList::iterator it = m_lstPerson[camp].begin(); it != m_lstPerson[camp].end(); ++it)
	{
		if((*it)->m_personType == PERSON_TYPE_TOWER || (*it)->isStatus(PERSON_STATUS_RELEASE))
			continue;
        
		if((*it)->isEmptyHP() || (needAddHp && (*it)->isFullHp()))
			continue;
        
		if((*it)->m_personid / 100 == PERSON_HEALER)
			continue;
        
		if((*it)->isFlying())
			continue;
        
		if((*it)->m_curCamp == camp)
		{
			float ox = fabs(lx - (*it)->getCenterPt().x);
			float oy = fabs(ly - (*it)->getCenterPt().y);
            
			float dis = ox * ox + oy * oy;
            
            if(dis < distance)
            {
                distance = dis;
                pSoldier = *it;
            }
		}
	}
    
    pPerson->chgDestTarget(pSoldier);
    
    if(pSoldier != NULL)
    {
        pPerson->setPathFlying(pSoldier->getLogicX(), pSoldier->getLogicY());
    }
    
	return pSoldier;
}

BaseObj* PlayerScene::findNextAttackBuilding(Person* pPerson, bool isFlying, bool canJump)
{
    BaseObj* pBuilding = NULL;
    
	//! 由Buff决定查找方式
	if(pPerson->m_Buffs[BUFF_ATTACKTYPE].value != 0)
	{
		pBuilding = findNearAttackBuilding(pPerson, pPerson->getLogicX(), pPerson->getLogicY(), pPerson->m_Buffs[BUFF_ATTACKTYPE].value - 1);
	}
	else if(pPerson->m_Talents[BUFF_ATTACKTYPE].value != 0)
	{
		pBuilding = findNearAttackBuilding(pPerson, pPerson->getLogicX(), pPerson->getLogicY(), pPerson->m_Talents[BUFF_ATTACKTYPE].value - 1);
	}
	else
	{
        pBuilding = findNearAttackBuilding(pPerson, pPerson->getLogicX(), pPerson->getLogicY(), pPerson->m_attackTarget);
	}
    
    if (pBuilding == NULL) {
        pBuilding = findNearAttackBuilding(pPerson, pPerson->getLogicX(), pPerson->getLogicY(), 0);
    }
    
    BaseObj* pNearBuilding = pBuilding;
    
    BaseObj* pDestTarget = NULL;
    if (searchCachePath(pPerson, pBuilding)) {
        pDestTarget = pBuilding;
    }
    else {
        pDestTarget = checkAttackObject(pPerson, pBuilding);
        
        if (pDestTarget && pPerson->m_path.size() > 1) {
            _PathCacheNode pathCacheNode;
            pathCacheNode.pPerson = pPerson;
            pathCacheNode.pDestTarget = pBuilding;
            pathCacheNode.pWall = NULL;
            pathCacheNode.path.insert(pathCacheNode.path.begin(), pPerson->m_path.begin(), pPerson->m_path.end());
            
            if (pDestTarget != pBuilding) {
                pathCacheNode.pWall = (Building*)pDestTarget;
            }
            
            m_vecPathCache.push_back(pathCacheNode);
        }
    }
    
    pPerson->chgDestTarget(pDestTarget);
    
    if (pDestTarget) {
        
        if (pDestTarget->getObjType() == OBJTYPE_BUILDING) {
            if (((Building*)pDestTarget)->m_type == BUILDING_TYPE_WALL) {
                pPerson->m_pWallTarget = pNearBuilding;
            }
        }
        
        ((Building*)pDestTarget)->setAttackTargetAni();
    }
    
    return pDestTarget;
}

BaseObj* PlayerScene::findNextAttackPerson(Person* pPerson, bool isFlying, bool canJump)
{
    if (pPerson->m_curCamp == CAMP_ATTACK && pPerson->m_attackTarget != ATTACK_TARGET_NORMAL) {
        return NULL;
    }
    
    int eindex = (pPerson->m_curCamp == 0 ? 1 : 0);
    
	float distance = 99999999.0;
	BaseObj* pEnemy = NULL;
    
    float range = 20;
    
	for(_PersonList::iterator it = m_lstPerson[eindex].begin(); it != m_lstPerson[eindex].end(); ++it)
	{
		if((*it)->isEmptyHP())
			continue;
        
		if((*it)->isStatus(PERSON_STATUS_RELEASE))
			continue;
        
		if((*it)->isHidden())
			continue;
        
		if((*it)->m_personType == PERSON_TYPE_TOWER)
			continue;
        
		if((!isFlying && !pPerson->isShootAttack()) && (*it)->isFlying())
			continue;
        
		int cx = pPerson->getLogicX();
		int cy = pPerson->getLogicY();
		int dx = (*it)->getLogicX();
		int dy = (*it)->getLogicY();
        
		float dis = (cx - dx) * (cx - dx) + (cy - dy) * (cy - dy);
        
		if(dis <= range*range || pPerson->isStatus(PERSON_STATUS_DEFENSE_ACTIVE))
		{
			if(distance > dis)
			{
				distance = dis;
				pEnemy = (*it);
			}
		}
	}
    
	if(pEnemy == NULL)
		return NULL;
    
    BaseObj* pDestTarget = NULL;
    if (searchCachePath(pPerson, pEnemy)) {
        pDestTarget = pEnemy;
    }
    else {
        pDestTarget = checkAttackObject(pPerson, pEnemy);
        
        _PathCacheNode pathCacheNode;
        pathCacheNode.pPerson = pPerson;
        pathCacheNode.pDestTarget = pEnemy;
        pathCacheNode.pWall = NULL;
        pathCacheNode.path.insert(pathCacheNode.path.begin(), pPerson->m_path.begin(), pPerson->m_path.end());
        
        if (pDestTarget != pEnemy) {
            pathCacheNode.pWall = (Building*)pDestTarget;
        }
        
        m_vecPathCache.push_back(pathCacheNode);
    }
    
    pPerson->chgDestTarget(pDestTarget);
    
    if (pDestTarget) {
        
        if (pPerson->m_curCamp == CAMP_DEFENCE) {
            pPerson->addStatus(PERSON_STATUS_DEFENSE_ACTIVE);
        }
    }
	
    return pDestTarget;
}

CCPoint PlayerScene::findAttackPos(Person* pPerson, BaseObj* pDestObj, CCPoint destPt)
{
    int destw = pDestObj->getLogicWidth() / 2;
    
    if (destPt.x == -1 && destPt.y == -1)
    {
        destPt = ccp(pPerson->getLogicX(), pPerson->getLogicY());
    }
    
    if (destw < 2) {
        destw = 2;
    }
    
    int dx = pDestObj->getCenterPt().x;
    int dy = pDestObj->getCenterPt().y;
    
    int offx = destPt.x - dx;
    int offy = destPt.y - dy;
    
    if (offx > destw) {
        offx = destw;
    }
    else if(offx <= -destw) {
        offx = -destw + 1;
    }
    
    if (offy > destw) {
        offy = destw;
    }
    else if (offy <= -destw) {
        offy = -destw + 1;
    }
    
    CCPoint attackPt = ccp(dx + offx, dy + offy);
    
     //当查找攻击占位无效时，重新查找最近有效占位
    int area = m_astar.m_mapArea.get(pPerson->getLogicX(), pPerson->getLogicY());
    if (m_astar.m_map.get(attackPt.x, attackPt.y) > 0 || (!pPerson->isCanJumpWall() && m_astar.m_mapArea.get(attackPt.x, attackPt.y) != area)) {
        
        float distance = INT_MAX;

        int x = dx - destw + 1;
        for (; x <= dx + destw; x ++) {
            
            int y = dy - destw + 1;
            for (; y <= dy + destw; y ++) {
                
                float dis = fabs(x-destPt.x)+ fabs(y-destPt.y);
                if (m_astar.m_map.get(x, y) == 0 && (pPerson->isCanJumpWall() || m_astar.m_mapArea.get(x, y) == area))
                {
                    if (dis < distance) {
                        
                        attackPt.x = x;
                        attackPt.y = y;
                        distance = dis;
                    }
                }
            }
        }
    }

    return attackPt;
}

void PlayerScene::fixAttackPos(Person* pPerson)
{
    //左右旋转30‘,分散士兵占位
    int rnd = (pPerson->m_indexPerson % 2);
    if (rnd == 0) rnd = -1;
    float degress = 60 * rnd;
    if (pPerson->isShootAttack()) {
        degress = 30 * rnd;
    }
    
    if (degress != 0 && !pPerson->m_path.empty()) {
    
        PathNode pathNode = pPerson->m_path.back();
        
        BaseObj* pDestTarget = pPerson->m_pDestTarget;
        
        CCPoint destPt = ccp(pathNode.x, pathNode.y);
        
        float radian = CC_DEGREES_TO_RADIANS(degress);
        destPt = ccpRotateByAngle(destPt, pDestTarget->getCenterPt(), radian);
        
        destPt = findAttackPos(pPerson, pDestTarget, destPt);
    }
}

BaseObj* PlayerScene::checkAttackObject(Person* pPerson, BaseObj *pDestObj)
{
    if(pDestObj != NULL)
	{
        int refindnums = 0;
		int maxfar = 30;
        
    __farwaay:
        
        if (refindnums != 0 && searchCachePath(pPerson, pDestObj)) {
            return pDestObj;
        }
        
        pPerson->chgDestTarget(pDestObj);
        
        CCPoint destPt = findAttackPos(pPerson, pDestObj, ccp(-1, -1));
        
        int ex = destPt.x;
        int ey = destPt.y;
        
		if(pPerson->isFlying())
		{
			pPerson->setPathFlying(pDestObj->getCenterPt().x, pDestObj->getCenterPt().y);
			return pDestObj;
		}
		else
		{
            bool isDestWall = false;
            if (pDestObj->getObjType() == OBJTYPE_BUILDING) {
                isDestWall = ((Building*)pDestObj)->m_buildingid / 100 == BUILDING_WALL;
            }
            
            pPerson->clearPath();
			float length = pPerson->getSearchLength();
            
            if ((length > 0 || pPerson->isCanJumpWall()) || canAttackTarget(pPerson, pDestObj)) {
                
                m_curFrameCanSearchNums --;
                if(m_astar.search(pPerson->getLogicX(), pPerson->getLogicY(), ex, ey, length, pPerson->isCanJumpWall(), isDestWall, maxfar, pPerson->getLogicWidth()))
                {
                    for(AStarNodeList::iterator it = m_astar.m_lstResult.begin(); it != m_astar.m_lstResult.end(); ++it)
                    {
                        if(it->x != pPerson->getLogicX() || it->y != pPerson->getLogicY())
                        {
                            pPerson->addPath(it->x, it->y);
                        }
                    }
                    
                    return pDestObj;
                }
            }
            
            pDestObj = findAttackWall(pPerson, pDestObj);
            
            if (refindnums >= 3) {
                return pDestObj;
            }
            
            maxfar = 15;
            if(pDestObj != NULL)
            {
                ++refindnums;
                goto __farwaay;
            }
                        
            return NULL;
		}
	}
    
	return NULL;
}

//! 判断一个Person是否还存在
bool PlayerScene::hasPerson(Person* pPerson)
{
	for(int i = 0; i < 2; ++i)
	{
		for(_PersonList::iterator it = m_lstPerson[i].begin(); it != m_lstPerson[i].end(); ++it)
		{
			if((*it)->isEmptyHP())
				continue;
            
			if((*it)->isStatus(PERSON_STATUS_RELEASE))
				continue;
            
			if((*it)->isHidden())
				continue;
            
			if(pPerson == (*it))
				return true;
		}
	}
    
	return false;
}

//! 添加防守方的英雄
void PlayerScene::addDefenceHero()
{
	if(!LogicInfo::getSingleton().isAttackMode())
		return;
    
    if(HeroMgr::getSingleton().m_otherHero.heroid == 0 || HeroMgr::getSingleton().m_otherHero.hp < HeroMgr::getSingleton().m_otherHero.maxhp || HeroMgr::getSingleton().m_otherHeroState == HERO_STATE_UPGRADING)
		return;
    
    m_bDefenceHeroFight = true;
    
	Person* pPerson = addDefenceSoldier(m_pBuildingHeroAltar, HeroMgr::getSingleton().m_otherHero.heroid, HeroMgr::getSingleton().m_otherHero.skill3);
    
    if (pPerson) {
        
        if (LogicInfo::getSingleton().isReplayMode()) {
            
            int lx = HeroMgr::getSingleton().m_otherHeroCtrl.x;
            int ly = HeroMgr::getSingleton().m_otherHeroCtrl.y;
            int nextWalkStep = HeroMgr::getSingleton().m_otherHeroCtrl.ctrl;
            int sleepTime = HeroMgr::getSingleton().m_otherHeroCtrl.state;
            
            pPerson->setLogicPos(lx, ly);
            pPerson->m_sleepTime = sleepTime;
            pPerson->m_nextWalkStep = nextWalkStep;
            pPerson->chgState(PERSON_ACT_NORMAL);
        }
        
        pPerson->m_defendRange = m_pBuildingHeroAltar->m_info.attackrange;
        m_pOtherHero = pPerson;
    }
}

//! 修正落地的坐标（尝试分散下落的士兵）
int personDropIndex = 0;
void PlayerScene::fixDropPosition(int& lx, int& ly)
{
    if (lx < 0) {
        lx = 0;
    }
    if (lx > LOGIC_W - 1) {
        lx = LOGIC_W - 1;
    }
    if (ly < 0) {
        ly = 0;
    }
    if (ly > LOGIC_W - 1) {
        ly = LOGIC_W - 1;
    }
    
	std::vector<CCPoint> lstpt;
    
	for(int i = -1; i <= 1; ++i)
	{
		for(int j = -1; j <= 1; ++j)
		{
			CCPoint pt;
            
			pt.x = lx + i;
			pt.y = ly + j;
            
			if(!m_mapDataOcc.isValidPos(pt.x, pt.y))
				continue;
            
			if(m_mapDataOcc.get(pt.x, pt.y) == 0 || (pt.x == 0 || pt.y == 0 || pt.x == LOGIC_W-1 || pt.y == LOGIC_W-1))
			{
				lstpt.push_back(pt);
			}
		}
	}
    
	if(lstpt.size() <= 0)
		return;
    
	int index = (personDropIndex++) % lstpt.size();
    
	lx = lstpt[index].x;
	ly = lstpt[index].y;
}

void PlayerScene::revengeAttack(Person* pPerson)
{
    int camp = pPerson->m_curCamp;
    CCPoint ptPerson = ccp(pPerson->getLogicX(), pPerson->getLogicY());
    for(_PersonList::iterator it = m_lstPerson[camp].begin(); it != m_lstPerson[camp].end(); ++it)
    {
        Person* pPerson_ = *it;
        if (!pPerson_->isEmptyHP() && (pPerson_->m_attackTarget == ATTACK_TARGET_NORMAL)
            && ccpDistance(ptPerson, ccp(pPerson_->getLogicX(), pPerson_->getLogicY())) < 10) {
            if (pPerson_->m_pDestTarget == NULL || pPerson_->m_pDestTarget->getObjType() != OBJTYPE_PERSON) {
                pPerson_->m_lastResearch = 0;
            }
        }
    }
}

Person* PlayerScene::addDefenceSoldier(Building* pBuilding, int soldierid, int skillid)
{
	Person* pPerson = PersonMgr::getSingleton().newPerson(soldierid, CAMP_DEFENCE);
	if(pPerson != NULL)
	{
        int lx = pBuilding->getLogicX();
        int ly = pBuilding->getLogicY();
        
		pPerson->init(lx, ly, PERSON_ACT_NORMAL, skillid);
        
		pPerson->m_indexPerson = (++LogicInfo::getSingleton().m_curPersonDef);
        
		m_lstPerson[CAMP_DEFENCE].push_back(pPerson);
	}
    
	return pPerson;
}

// mode 0 - 地面, 1 - 天空, 2 - 天空&地面
Person* PlayerScene::findNearPerson(int lx, int ly, float min, float max, int mode, int camp)
{
	float distance = 99999999.0f;
	Person* pPerson = NULL;
    
	for(_PersonList::iterator it = m_lstPerson[camp].begin(); it != m_lstPerson[camp].end(); ++it)
	{
		if((*it)->m_personType == PERSON_TYPE_TOWER || (*it)->isStatus(PERSON_STATUS_RELEASE))
			continue;
        
		if((*it)->isEmptyHP())
			continue;
        
		if((*it)->isHidden())
			continue;
        
		if(mode == 0 && (*it)->getSoldierInfo().isFlying)
			continue;
		else if(mode == 1 && !(*it)->getSoldierInfo().isFlying)
			continue;
        
		int tx = (*it)->getLogicX();
		int ty = (*it)->getLogicY();
        
		float td = (lx - tx) * (lx - tx) + (ly - ty) * (ly - ty);
		if(td >= min*min && td <= max*max && td < distance)
		{
			distance = td;
			pPerson = *it;
		}
	}
    
	return pPerson;
}

void PlayerScene::killPerson(Person* pPerson)
{    
    if (pPerson->m_curCamp == CAMP_ATTACK) {
        LogicInfo::getSingleton().onAddCtrl_KillAtkSoldier(pPerson->m_indexPerson);
    }
    else if (pPerson->m_curCamp == CAMP_DEFENCE) {
        LogicInfo::getSingleton().onAddCtrl_KillDefSoldier(pPerson->m_indexPerson);
    }
}

void PlayerScene::addTomb(int lx, int ly)
{
	if(!canBuilding(0, lx, ly, 2))
		return;
    
	CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("tombstore");
	if(pFrame != NULL)
	{
		for(std::list<_TombInfo>::iterator it = m_lstTomb.begin(); it != m_lstTomb.end(); ++it)
		{
			if(it->lx == lx && it->ly == ly)
				return ;
		}
        
		_TombInfo ti;
        
		ti.pTomb = CCSprite::create();
		ti.pTomb->initWithSpriteFrame(pFrame);
		m_pRoot->addChild(ti.pTomb, (lx + ly) * SCENE_Z_BUILDING_JUMP);
        
		CCPoint pos = countScenePos(lx, ly);
        
		ti.pTomb->setPosition(pos);
        
		ti.lx = lx;
		ti.ly = ly;
        
		m_lstTomb.push_back(ti);
	}
}

void PlayerScene::addBuildingBoom(int sx, int sy)
{
	AniObj* pObj = AniObjMgr::getSingleton().newAniObj(m_pParticleBatchNode, "building_bomb", 0, true, false, -1, 0, false, NULL);
	pObj->setPosition(ccp(sx, sy));
	pObj->start();
}

void PlayerScene::addCannonSmoke(int sx, int sy)
{
	AniObj* pObj = AniObjMgr::getSingleton().newAniObj(m_pParticleBatchNode, "boom003", 0, true, false, -1, 0, false, NULL);
	pObj->setPosition(ccp(sx, sy));
	pObj->setScale(2.0f);
	pObj->start();
}

void PlayerScene::addGetResAni(int sx, int sy, int nums, const ccColor3B &color)
{
	AniObj_FlyingFont* pAniObj = (AniObj_FlyingFont*)AniObjMgr::getSingleton().newAniObj(m_pRoot, "addresfont", 0, true, false, -1, SCENE_Z_BUILDINGUI, false, NULL);
	if(pAniObj != NULL)
	{
		char str[128];
		sprintf(str, "%d", nums);
        
		pAniObj->setString(str);
        pAniObj->setColor(color);
		//pAniObj->setParent(m_pRoot, SCENE_Z_BUILDINGUI);
        
		CCPoint pt;
        
		pt.x = sx;
		pt.y = sy;
        
		pAniObj->setPosition(pt);
		pAniObj->start();
	}
}

void PlayerScene::onGoHome()
{
	onBuildingSelect(false, NULL);
    
	setReplaySpeed(1);
    
	m_isInit = false;
    
	initScene();
    
	setPosition(-LOGIC_W * TILE_W / 4, -LOGIC_W * TILE_H / 4);
    
	m_isInit = true;
    
	LogicInfo::getSingleton().onInitTroophousingSoldier();
    
	initOtherPerson();
    
	centerOfTownhall();
    
	LogicInfo::getSingleton().fixStorageReserve(true);
}

int PlayerScene::countBuildingNums()
{
	int nums = 0;
    
	for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if(!(*it)->isEmptyHP() && LogicInfo::getSingleton().isNeedDestroyBuidling((*it)->m_buildingid))
			++nums;
	}
    
	return nums;
}

int PlayerScene::countSoldierNums(int camp)
{
	int nums = 0;
    
    for(_PersonList::iterator it = m_lstPerson[camp].begin(); it != m_lstPerson[camp].end(); ++it)
    {
        if((*it)->m_personType == PERSON_TYPE_TOWER || (*it)->isStatus(PERSON_STATUS_RELEASE))
            continue;
        
        //! 天使不记入战斗士兵
        if((*it)->m_personid / 100 == PERSON_HEALER)
            continue;
        
        if(((*it)->m_personid < 20000 || (*it)->m_personid > 30000))
            ++nums;
    }
    
	return nums;
}

bool PlayerScene::hasAttackBuilding()
{
	for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if((*it)->isEmptyHP())
			continue;
        
		if((*it)->isAttackBuilding())
			return true;
	}
    
	return false;
}

bool PlayerScene::hasResourceBuilding()
{
	for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if((*it)->isEmptyHP())
			continue;
        
		int bid = (*it)->m_buildingid / 100;
		if(bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_ELIXIR_STORAGE || bid == BUILDING_GOLD_MINE || bid == BUILDING_GOLD_STORAGE)
			return true;
	}
    
	return false;
}

void PlayerScene::setReplaySpeed(int speed)
{
	m_speedReplay = speed;
    
	CCDirector::sharedDirector()->getScheduler()->setTimeScale(m_speedReplay);
}

void PlayerScene::onReplay()
{
	setReplaySpeed(1);
    
    m_replayReadyTime = 0;
    m_beginReplayTime = 0;
    
    FightCtrlNode fightNode = LogicInfo::getSingleton().m_logFight.node[LogicInfo::getSingleton().m_logFight.ctrlIndex + LogicInfo::getSingleton().m_logFight.numsCtrl - 1];
        
    m_replayTime = fightNode.ctrl;
    if (m_replayTime <= FIGHT_TIME * 1000 - 3000) {
        
        m_replayReadyTime = 3000;
        m_replayTime += m_replayReadyTime;
    }
    
	m_isReplayMode = true;
    
	m_curCtrlIndex = LogicInfo::getSingleton().m_logFight.ctrlIndex;
}

void PlayerScene::onStopReplay()
{
	//CCDirector::sharedDirector()->m_limitTime = false;
	//while(!CCDirector::sharedDirector()->m_queueTimeOffMs.empty())
	//	CCDirector::sharedDirector()->m_queueTimeOffMs.pop();
    
	setReplaySpeed(1);
    
	m_beginReplayTime = 0;
	m_isReplayMode = false;
    
	m_curCtrlIndex = 0;
}

void PlayerScene::onFixPos(float& x, float& y)
{
	if(m_pRoot != NULL) {
		m_pRoot->fixPos(x, y);
        m_pRoot->chgPos(x, y);
    }
}

void PlayerScene::onStopAttack()
{
	for(int i = 0; i < 2; ++i)
	{
		for(_PersonList::iterator it = m_lstPerson[i].begin(); it != m_lstPerson[i].end(); ++it)
		{
            (*it)->chgState(PERSON_ACT_NORMAL);
		}
	}
}

void PlayerScene::onTroophousingMov(Building& building)
{
    for(_PersonList::iterator it = m_lstPerson[CAMP_DEFENCE].begin(); it != m_lstPerson[CAMP_DEFENCE].end(); ++it)
    {
        (*it)->onTroophousingMov(building);
    }
}

void PlayerScene::addTile(CCSprite* pSpr)
{
	m_pTileBatchNode->addChild(pSpr, SCENE_Z_TILE);
}

void PlayerScene::removeTile(CCSprite* pSpr)
{
	m_pTileBatchNode->removeChild(pSpr, true);
}

void PlayerScene::addShadow(CCSprite* pSpr)
{
	m_pTileBatchNode->addChild(pSpr, SCENE_Z_SHADOW);
}

void PlayerScene::addFlyingShadow(CCSprite* pSpr)
{
	m_pRoot->addChild(pSpr, SCENE_Z_FLYINGSHADOW);
}

void PlayerScene::removeShadow(CCSprite* pSpr, bool isFlying)
{
	if(isFlying)
		m_pRoot->removeChild(pSpr, true);
	else
		m_pTileBatchNode->removeChild(pSpr, true);
}

void PlayerScene::addParticle(CCSprite* pSpr)
{
	m_pParticleBatchNode->addChild(pSpr, SCENE_Z_PARTICLE);
}

void PlayerScene::removeParticle(CCSprite* pSpr)
{
	m_pParticleBatchNode->removeChild(pSpr, true);
}

void PlayerScene::randXY(int& x, int& y)
{
	x = rand() % (LOGIC_W - EDGE_W*2) + EDGE_W;
	y = rand() % (LOGIC_W - EDGE_W*2) + EDGE_W;
}

void PlayerScene::randRangeXY(int &x, int &y, int range)
{
    int rangex = (rand() % range) - range/2;
	int rangey = (rand() % range) - range/2;
    
    x = x + rangex;
    y = y + rangey;
    
    if (x < EDGE_W) {
        x += range/2;
    }
    else if (x > LOGIC_W - EDGE_W*2) {
        x -= range/2;
    }
    if (y < EDGE_W) {
        y += range/2;
    }
    else if (y > LOGIC_W - EDGE_W*2) {
        y -= range/2;
    }
}

void PlayerScene::initHero(HeroInfo2 hero)
{
	if(hero.heroid != 0)
    {
        int x = 0;
        int y = 0;
        
        randXY(x, y);
        if (m_pBuildingHeroAltar) {
            x = m_pBuildingHeroAltar->getLogicX();
            y = m_pBuildingHeroAltar->getLogicY();
        }
        
        Person* pPerson = addPerson(hero.heroid, x, y, PERSON_ACT_NORMAL, CAMP_DEFENCE);
        if(pPerson != NULL)
        {
            pPerson->m_personType = PERSON_TYPE_FREE;
            pPerson->m_sleepTime = 1000 + rand() % 3000;
            
            pPerson->setHP(hero.hp);
            HeroMgr::getSingleton().refreshHero();
        }
    }
}

void PlayerScene::initOtherPerson()
{
	initHero(HeroMgr::getSingleton().m_myHero);
    
    int x;
	int y;
        
	for(int i = 0; i < LogicInfo::getSingleton().m_curTownhall.level + 5; ++i)
	{
        randXY(x, y);
        while (m_astar.m_map.get(x, y) != 0) {
            randXY(x, y);
        }
        
		Person* pPerson = addPerson(20101, x, y, PERSON_ACT_NORMAL, CAMP_WORKER);
		pPerson->m_personType = PERSON_TYPE_FREE;
		pPerson->m_sleepTime = 1000 + rand() % 3000;
	}
}

Projectile2* PlayerScene::addProjectile2(const char* strFrame, int bx, int by, BaseObj* pObj, float bendVal,
        const char* strBomb, bool isDrop, bool earthquake, bool bRotate, float speed, const char* strEndFrame,
        float endTime, bool isTrack, bool isAOE, float damage, int camp, float rAOE, bool flyfont)
{
	Projectile2* pProj = Projectile2::create();
    if (isTrack)
        pProj->shoot2Obj(strFrame, bx, by, pObj, speed, bendVal, strBomb, isDrop, earthquake, bRotate, strEndFrame,
                         endTime, isTrack, isAOE, damage, camp, rAOE, flyfont);
    else
        pProj->shoot2Point(strFrame, bx, by, pObj->getAttackPos(), pObj->getCenterPt(), speed, bendVal, strBomb, isDrop, earthquake, bRotate, strEndFrame, endTime, isTrack, isAOE, damage, camp, rAOE, flyfont);
    
    if (bendVal < 1 && !isDrop) {
        pProj->setAnchorPoint(ccp(0.5, 0));
    }
    else {
        pProj->setAnchorPoint(ccp(0.5, 1));
    }
    
    pProj->setScale(1.2);
    
	m_lstProjectile2.push_back(pProj);
	pProj->retain();
    
	addParticle(pProj);
    
    return pProj;
}

void PlayerScene::addBoomEx(const char* strBomb, int dir, int sx, int sy, bool isFlip)
{
	AniObj_Normal* pObj = (AniObj_Normal*)(AniObjMgr::getSingleton().newAniObj(m_pParticleBatchNode, strBomb, dir, true, false, -1, 0, false, NULL));
	pObj->setPosition(ccp(sx, sy));
	if(isFlip)
	{
		pObj->setScaleX(-1.2f);
		pObj->setScaleY(1.2f);
	}
	else
	{
		pObj->setScale(1.2f);
	}
	pObj->start();
}

void PlayerScene::addBoomEx2(const char* strBomb, int dir, int sx, int sy, int zorder)
{
	AniObj* pObj = AniObjMgr::getSingleton().newAniObj(PlayerScene::getSingleton().m_pRoot, strBomb, dir, true, false, -1, zorder, false, NULL);
	pObj->setPosition(ccp(sx, sy));
	pObj->setScale(1.2f);
	pObj->start();
}

void PlayerScene::setUpDownAction()
{
	return;
    
	if(m_isScaleMode)
		return;
    
	if(m_pEarthquakeAct != NULL && !m_pEarthquakeAct->isDone())
		return ;
    
	if(m_pEarthquakeAct != NULL)
	{
		m_pEarthquakeAct->release();
        
		m_pEarthquakeAct = NULL;
	}
    
	CCPoint cpt = m_pRoot->getPosition();
    
	//CCMoveBy
	CCPoint pt1;
	CCPoint pt2;
    
	pt1.x = rand() % 5;
	pt1.y = 10 + rand() % 8;
    
	pt2.y = -pt1.x * 2;//rand() % 20;
	pt2.y = -pt1.y * 2;//rand() % 20;
    
	CCMoveBy* mv1 = CCMoveBy::create(0.1f, pt1);
	CCMoveBy* mv2 = CCMoveBy::create(0.1f, pt2);
    
	CCPoint pt3;
	CCPoint pt4;
    
	pt3.x = rand() % 5;
	pt3.y = 5 + rand() % 8;
    
	pt4.y = -pt3.x * 2;//rand() % 10;
	pt4.y = -pt3.y * 2;//rand() % 10;
    
	CCMoveBy* mv3 = CCMoveBy::create(0.1f, pt3);
	CCMoveBy* mv4 = CCMoveBy::create(0.1f, pt4);
    
	CCPoint pt5;
	CCPoint pt6;
    
	pt5.x = rand() % 5;
	pt5.y = 3 + rand() % 8;
    
	pt6.y = -pt5.x * 2;//rand() % 10;
	pt6.y = -pt5.y * 2;//rand() % 10;
    
	CCMoveBy* mv5 = CCMoveBy::create(0.1f, pt5);
	CCMoveBy* mv6 = CCMoveBy::create(0.1f, pt6);
    
	CCMoveTo* mv7 = CCMoveTo::create(0.1f, cpt);
    
	CCFiniteTimeAction* action1 = CCSequence::create(mv1, mv2, mv3, mv4, mv5, mv6, mv7, NULL);
    
	m_pEarthquakeAct = m_pRoot->runAction(action1);
	m_pEarthquakeAct->retain();
}

void PlayerScene::onUpdateHero()
{
	if(m_pHero != NULL)
	{
		int chp = m_pHero->getHP();
		int off = chp - HeroMgr::getSingleton().m_myHero.hp;
        
		m_pHero->chgHP(-off);
	}
}

Building* PlayerScene::findBuilding(int buildingid)
{
    for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
    {
        Building* pBuilding = *it;
        if(pBuilding != NULL && pBuilding->m_pBuildingInfo != NULL && pBuilding->m_pBuildingInfo->buildingid == buildingid)// && pBuilding->m_pBuildingInfo->state == 2)
        {
            return pBuilding;
        }
    }
    
    return NULL;
}

void PlayerScene::centerOfBuilding(Building& building)
{
	setCenterPos(building.getAttackPos().x * m_pRoot->getScale(), building.getAttackPos().y * m_pRoot->getScale());
}

void PlayerScene::centerOfPoint(CCPoint point)
{
    CCPoint pos = countScenePos(point.x, point.y);
    setCenterPos(pos.x * m_pRoot->getScale(), pos.y * m_pRoot->getScale());
}

void PlayerScene::setCenterPos(float x, float y)
{
	m_pRoot->chgCenterPos(x, y);
        
	if(m_pBuildUI != NULL)
	{
		m_pBuildUI->setPosition(m_pRoot->getPosition());
	}
    
    return ;
    
	float cx = x - (m_screenW / 2) / m_scale;
	float cy = y - (m_screenH / 2) / m_scale;
    
	setPosition(-cx, -cy);
}

void PlayerScene::centerOfTownhall()
{
	for(_BuildingList::const_iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		int bid = (*it)->m_buildingid / 100;
		if(bid == BUILDING_TOWN_HALL)
		{
			centerOfBuilding(**it);
            
			return ;
		}
	}
}

void PlayerScene::unselectCurBuilding()
{
	if(m_pCurBuilding == NULL)
		return;
    
    if(m_pCurBuilding->m_state == BUILDING_CTRL_BUILD)
    {
        return;
    }
	else if(m_pCurBuilding->m_state == BUILDING_CTRL_SELECTED)
	{
        if (!m_lstSelectWall.empty()) {
            clearSelectWallLine();
        }
	}
    else if(m_pCurBuilding->m_state == BUILDING_CTRL_MOVE)
	{
        if (!m_lstSelectWall.empty()) {
            
            downSelectWallLine();
            
            if(m_pCurBuilding != NULL && m_pCurBuilding->m_pTileColor != NULL)
            {
                resetSelectWallLine();
            }
        }
        else {
            
            buildCurBuilding();
            
            if(m_pCurBuilding != NULL && m_pCurBuilding->m_pTileColor != NULL)
            {
                m_pCurBuilding->setLogicPos(m_pCurBuilding->m_lbx, m_pCurBuilding->m_lby);
                buildCurBuilding();
            }
        }
	}
    
    if (m_pCurBuilding) {
        
        m_pCurBuilding->m_state = BUILDING_CTRL_NORMAL;
        m_pCurBuilding->chgSelMode(false);
        
        setBuildingTouchMode(*m_pCurBuilding, false);
        onBuildingSelect(false, NULL);
        
        m_pCurBuilding = NULL;
    }
    
    hideArrow();
    
    if (m_isEditMode) {
        PlayerScene::getSingleton().m_pGameWorld->m_pMainEditUI->checkEditSaveEnable();
    }
}

void PlayerScene::onDestroyBuilding(Building& building)
{
    int bid = building.m_buildingid/100;
    
    int len = 30;
    for(int i = 0; i < 2; ++i)
    {
        for(_PersonList::iterator it = m_lstPerson[i].begin(); it != m_lstPerson[i].end(); ++it)
        {
            Person* pPerson = *it;
            
            if (bid == BUILDING_WALL) {
                
                int ox = building.getLogicX() - pPerson->getLogicX();
                int oy = building.getLogicY() - pPerson->getLogicY();
                
                int dis = ox * ox + oy * oy;
                
                if(dis < len * len)
                {
                    if(pPerson->m_pDestTarget != NULL && pPerson->m_pDestTarget->getObjType() == OBJTYPE_BUILDING)
                    {
                        pPerson->m_personState = PERSON_STATE_WAITATTACK_TARGET;
                    }
                }
            }
            else if (pPerson->m_pWallTarget == &building)
            {
                pPerson->m_personState = PERSON_STATE_WAITATTACK_TARGET;
            }
        }
    }
    
    {
        BaseObj* pWallTarget = NULL;
        for (vector<_PathCacheNode>::iterator it = m_vecPathCache.begin(); it != m_vecPathCache.end(); it ++)
        {
            if (it->pWall == &building) {
                
                pWallTarget = it->pDestTarget;
                break;
            }
        }
        
        for (vector<_PathCacheNode>::iterator it = m_vecPathCache.begin(); it != m_vecPathCache.end(); )
        {
            if ((it->pWall && it->pDestTarget == pWallTarget) || (it->pDestTarget == &building)) {
                it = m_vecPathCache.erase(it);
                continue;
            }
            
            it ++;
        }
    }
    
    onSetLogicMap(building.getLogicX(), building.getLogicY(), building.getLogicWidth(), building.m_buildingid, 0, false);
    m_astar.rebuildMapArea();
}

// mode 0 - 地面, 1 - 天空, 2 - 天空&地面
void PlayerScene::procAOE(int x, int y, int damage, float range, int camp, int mode, bool isWall, int damageEx, bool flyfont)
{
	if(CAMP_ATTACK == camp)
	{
		for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
		{
			if((*it)->m_buildingid >= 50000 || (*it)->isEmptyHP())
				continue;
            
			float ox = fabs((*it)->getCenterPt().x - x);
			float oy = fabs((*it)->getCenterPt().y - y);
			float len = ox * ox + oy * oy;
            
			if(len > range*range)
				continue;
            
            if(isWall && (*it)->m_type == BUILDING_TYPE_WALL) {
				(*it)->chgHP(-damageEx, flyfont);
            }
            else {
				(*it)->chgHP(-damage, flyfont);
            }
		}
	}
    
    int damageCamp = camp == CAMP_ATTACK ? CAMP_DEFENCE : CAMP_ATTACK;
	for(_PersonList::iterator it = m_lstPerson[damageCamp].begin(); it != m_lstPerson[damageCamp].end(); ++it)
	{
		if((*it)->m_personType == PERSON_TYPE_TOWER || (*it)->isStatus(PERSON_STATUS_RELEASE))
			continue;
        
		if((*it)->isEmptyHP())
			continue;
        
        if(mode == 0 && (*it)->getSoldierInfo().isFlying)
            continue;
        else if(mode == 1 && !(*it)->getSoldierInfo().isFlying)
            continue;
        
		float ox = fabs((*it)->getCenterPt().x - x);
		float oy = fabs((*it)->getCenterPt().y - y);
		float len = ox * ox + oy * oy;
        
		if(len > range*range)
			continue;
        
		(*it)->chgHP(-damage, flyfont);
	}
}

void PlayerScene::procStab(int x, int y, float range, int camp, float rx, float ry)
{
	for(_PersonList::iterator it = m_lstPerson[camp].begin(); it != m_lstPerson[camp].end(); ++it)
	{
		if((*it)->m_personType == PERSON_TYPE_TOWER || (*it)->isStatus(PERSON_STATUS_RELEASE))
			continue;
        
		if((*it)->isEmptyHP())
			continue;
        
		if((*it)->isFlying())
			continue;
        
		if((*it)->isHero() || ((*it)->m_personid/100 == PERSON_PEKKA))
			continue;
        
		float ox = fabs((*it)->getCenterPt().x - x);
		float oy = fabs((*it)->getCenterPt().y - y);
		float len = ox * ox + oy * oy;
        
		if(len > range*range)
			continue;
        
		(*it)->chgHP(-(*it)->getHP());
	}
}

void PlayerScene::procAOE_Medical(int x, int y, int damage, float range, int camp, bool isWall, int damageEx)
{
	for(_PersonList::iterator it = m_lstPerson[camp].begin(); it != m_lstPerson[camp].end(); ++it)
	{
		if((*it)->m_personType == PERSON_TYPE_TOWER || (*it)->isStatus(PERSON_STATUS_RELEASE))
			continue;
        
		if((*it)->isEmptyHP())
			continue;
        
		if((*it)->isFlying())
			continue;
        
		float ox = fabs((*it)->getCenterPt().x - x);
		float oy = fabs((*it)->getCenterPt().y - y);
		float len = ox * ox + oy * oy;
        
		if(len > range*range)
			continue;
        
		float cl = 1.0f;
		float cr = len / range;
		if(cr > 0.5f && cr <= 0.75f)
			cl = 0.5f;
		else if(cr > 0.75f)
			cl = 0.25f;
        
		(*it)->chgHP(damage * cl);
        
		AniObj* ani = AniObjMgr::getSingleton().newAniObj((*it)->m_pSkillEffect[1], "person108_hit", 0, true, false, -1, 0, false, NULL);
        
		ani->start();
	}
}

void PlayerScene::onWarEnd()
{
	for(std::list<Projectile2*>::iterator it = m_lstProjectile2.begin(); it != m_lstProjectile2.end(); )
	{
		Projectile2* pProj = *it;
		if(pProj != NULL && !pProj->isEndState())
		{
			pProj->m_isRelease = true;
            
			removeParticle(pProj);
            
			pProj->release();
            
			it = m_lstProjectile2.erase(it);
		}
        else {
			++it;
        }
	}
}

void PlayerScene::chg2Win(int camp)
{
	for(_PersonList::iterator it = m_lstPerson[camp].begin(); it != m_lstPerson[camp].end(); ++it)
	{
		if((*it)->isStatus(PERSON_STATUS_RELEASE))
			continue ;
        
		if((*it)->isEmptyHP())
			continue ;
        
		(*it)->chg2Win();
	}
}

int PlayerScene::countDirWithWall(int ox, int oy)
{
	if(ox < 0 && oy == 0)
		return DIR_RIGHT;
	else if(ox > 0 && oy == 0)
		return DIR_LEFT;
	else if(ox == 0 && oy < 0)
		return DIR_UP;
	else if(ox == 0 && oy > 0)
		return DIR_DOWN;
    else if (ox == 0 && oy == 0)
        return DIR_LEFT;
    
	return -1;
}

// countDirWithWall的反函数
void PlayerScene::countOffPosWithWall(int dir, int& ox, int& oy)
{
	if(dir == DIR_RIGHT)
		ox = -1, oy = 0;
	else if(dir == DIR_LEFT)
		ox = 1, oy = 0;
	else if(dir == DIR_UP)
		ox = 0, oy = -1;
	else if(dir == DIR_DOWN)
		ox = 0, oy = 1;
	else
		ox = 0, oy = 0;
}

// 判断下一个墙出现的方向
int PlayerScene::countNextDirWithWall(int clx, int cly)
{
	if(PlayerScene::getSingleton().isWall(clx - 2, cly) && canBuilding(10301, clx + 2, cly, 2))
		return DIR_LEFT;
	else if(PlayerScene::getSingleton().isWall(clx + 2, cly) && canBuilding(10301, clx - 2, cly, 2))
		return DIR_RIGHT;
	else if(PlayerScene::getSingleton().isWall(clx, cly + 2) && canBuilding(10301, clx, cly - 2, 2))
		return DIR_UP;
	else if(PlayerScene::getSingleton().isWall(clx, cly - 2) && canBuilding(10301, clx, cly + 2, 2))
		return DIR_DOWN;
    
	return DIR_LEFT;
}

//! 士兵升级
void PlayerScene::onSoldierUpgrade(int soldierid)
{
	for(int i = 0; i < 2; ++i)
	{
		for(_PersonList::iterator it = m_lstPerson[i].begin(); it != m_lstPerson[i].end(); ++it)
		{
			if((*it)->m_personType == PERSON_TYPE_TOWER || (*it)->isStatus(PERSON_STATUS_RELEASE))
				continue;
            
			if((*it)->isEmptyHP())
				continue;
            
			(*it)->upgrade(soldierid);
		}
	}
}

// 快速升级士兵
void PlayerScene::onFastUpgradeSoldier(int ot)
{
	for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
    {
        Building* pBuilding = *it;
		if(pBuilding != NULL && pBuilding->m_buildingid / 100 == 303)
        {
			LogicInfo::getSingleton().onRefurbishLabMS(ot, *pBuilding);
            
			return;
        }
    }
}

// 判断点在有向直线的左侧还是右侧.
// 返回值:-1: 点在线段左侧; 0: 点在线段上; 1: 点在线段右侧
int PointAtLineLeftRight(CCPoint ptStart, CCPoint ptEnd, CCPoint ptTest)
{
    ptStart.x -= ptTest.x;
    ptStart.y -= ptTest.y;
    ptEnd.x -= ptTest.x;
    ptEnd.y -= ptTest.y;
    
    int nRet = ptStart.x * ptEnd.y - ptStart.y * ptEnd.x;
    if (nRet == 0)
        return 0;
    else if (nRet > 0)
        return 1;
    else if (nRet < 0)
        return -1;
    
    return 0;
}

// 判断两条线段是否相交
bool IsTwoLineIntersect(CCPoint ptLine1Start, CCPoint ptLine1End, CCPoint ptLine2Start, CCPoint ptLine2End)
{
    int nLine1Start = PointAtLineLeftRight(ptLine2Start, ptLine2End, ptLine1Start);
    int nLine1End = PointAtLineLeftRight(ptLine2Start, ptLine2End, ptLine1End);
    if (nLine1Start * nLine1End >= 0)
        return false;
    
    int nLine2Start = PointAtLineLeftRight(ptLine1Start, ptLine1End, ptLine2Start);
    int nLine2End = PointAtLineLeftRight(ptLine1Start, ptLine1End, ptLine2End);
    
    if (nLine2Start * nLine2End >= 0)
        return false;
    
    return true;
}

Building_Wall* PlayerScene::findAttackWall(Person* pPerson, BaseObj* pDestObj, int findnums)
{
    findnums ++;
    
	Building_Wall* pAttackWall = NULL;
	float distance = 9999999.0f;
    
	for(_BuildingList::iterator it = m_lstWallBuilding.begin(); it != m_lstWallBuilding.end(); ++it)
    {
		if((*it)->isEmptyHP())
			continue;
        
        Building_Wall* pWall = (Building_Wall*)*it;
        
        if (pWall == pDestObj) {
            continue;
        }
        
        if (checkAttackWall(pPerson, pWall, pDestObj))
        {
            float dis1 = (pPerson->getLogicX() - pWall->getCenterPt().x) * (pPerson->getLogicX() - pWall->getCenterPt().x)
                + (pPerson->getLogicY() - pWall->getCenterPt().y) * (pPerson->getLogicY() - pWall->getCenterPt().y);
            float dis2 = (pWall->getCenterPt().x - pDestObj->getCenterPt().x) * (pWall->getCenterPt().x - pDestObj->getCenterPt().x) + (pWall->getCenterPt().y - pDestObj->getCenterPt().y) * (pWall->getCenterPt().y - pDestObj->getCenterPt().y);
            
            float dis = dis1 + dis2;
            
            if (pWall->m_beAttackTargetNums >= 1)
            {
                dis *= 0.5;
            }
            else if (pWall->getWallDirNums() >= 2)
            {
                dis *= 0.6;
            }
            else if (pAttackWall != NULL && pWall->getHP() < pAttackWall->getHP())
            {
                dis *= 0.8;
            }
            
            if (dis < distance) {
                
                pAttackWall = pWall;
                distance = dis;
            }
        }
    }
    
	return pAttackWall;
}

bool PlayerScene::checkAttackWall(Person *pPerson, Building_Wall* pWall, BaseObj* pDestObj)
{
    if (!canAttackTarget(pPerson, pWall)) {
        return false;
    }
    
    if (pWall->getWallDirNums() == 0) {
        return false;
    }
    
    int walllinelen = 3;
    int walllinedir = 0;
    
    int leftnums = 0;
    Building_Wall* pWallLeft = pWall->m_pWallLeft;
    while (pWallLeft && leftnums < walllinelen && walllinedir < 2) {
        
        pWallLeft = pWallLeft->m_pWallLeft;
        leftnums ++;
    }
    
    if (leftnums >= walllinelen) {
        walllinedir ++;
    }
    
    int rightnums = 0;
    Building_Wall* pWallRight = pWall->m_pWallRight;
    while (pWallRight && rightnums < walllinelen && walllinedir < 2) {
        
        pWallRight = pWallRight->m_pWallRight;
        rightnums ++;
    }
    
    if (rightnums >= walllinelen) {
        walllinedir ++;
    }
    
    int upnums = 0;
    Building_Wall* pWallUp = pWall->m_pWallUp;
    while (pWallUp && upnums < walllinelen && walllinedir < 2) {
        
        pWallUp = pWallUp->m_pWallUp;
        upnums ++;
    }
    
    if (upnums >= walllinelen) {
        walllinedir ++;
    }
    
    int downnums = 0;
    Building_Wall* pWallDown = pWall->m_pWallDown;
    while (pWallDown && downnums < walllinelen && walllinedir < 2) {
        
        pWallDown = pWallDown->m_pWallDown;
        downnums ++;
    }
    
    if (downnums >= walllinelen) {
        walllinedir ++;
    }
    
    if (walllinedir < 2) {
        return false;
    }
    
    int lx = pWall->getLogicX();
    int ly = pWall->getLogicY();
    int lw = pWall->getLogicWidth();
    
    int parea = m_astar.m_mapArea.get(pPerson->getLogicX(), pPerson->getLogicY());
    if (parea == -1) {
        for (int x = pPerson->getLogicX() - 1; x <= pPerson->getLogicX() + 1; x ++) {
            for (int y = pPerson->getLogicY() - 1; y <= pPerson->getLogicY() + 1; y ++) {
                
                parea = m_astar.m_mapArea.get(x, y);
                if (parea != -1) {
                    break;
                }
            }
            if (parea != -1) {
                break;
            }
        }
    }
    
    int dirx = lw;
    if (pPerson->getLogicX() - lx < 0) {
        dirx = -lw;
    }
    
    int diry = lw;
    if (pPerson->getLogicY() - ly < 0) {
        diry = -lw;
    }
    
    if (m_astar.m_mapArea.get(lx+dirx, ly) == parea ||
        m_astar.m_mapArea.get(lx+dirx, ly+dirx) == parea)
    {
        return true;
    }
    
    if (m_astar.m_mapArea.get(lx, ly+diry) == parea ||
        m_astar.m_mapArea.get(lx+dirx, ly+diry) == parea)
    {
        return true;
    }

    return false;
}

bool PlayerScene::canAttackTarget(Person* pPerson, BaseObj* pDestObj)
{
    int lx = pPerson->getLogicX();
    int ly = pPerson->getLogicY();
    
	int parea = m_astar.m_mapArea.get(lx, ly);
    if (parea == -1) {
        for (int x = lx - 1; x <= lx + 1; x ++) {
            for (int y = ly - 1; y <= ly + 1; y ++) {
                
                parea = m_astar.m_mapArea.get(x, y);
                if (parea != -1) {
                    break;
                }
            }
            if (parea != -1) {
                break;
            }
        }
    }
    
	int bx = pDestObj->getLogicX() - pDestObj->getLogicWidth();
	int by = pDestObj->getLogicY() - pDestObj->getLogicWidth();
    
	int ex = pDestObj->getLogicX() + 1;
	int ey = pDestObj->getLogicY() + 1;
    
	for(int x = bx; x <= ex; ++x)
	{
		for(int y = by; y <= ey; ++y)
		{
			if(x == pDestObj->getLogicX() && y == pDestObj->getLogicY())
				continue;
            
			if(!m_astar.m_mapArea.isValidPos(x, y))
				continue;
            
			if(m_astar.m_mapArea.get(x, y) == parea)
				return true;
		}
	}
    
	return false;
}

bool PlayerScene::searchCachePath(Person *pPerson, BaseObj* &pDestTarget)
{
    if (pDestTarget == NULL || pDestTarget->getObjType() == OBJTYPE_PERSON) {
        return false;
    }
    
    int range = 2;
    
    Path::iterator pathNode;
    
    vector<_PathCacheNode>::iterator it = m_vecPathCache.begin();
    for (; it != m_vecPathCache.end(); it ++) {
        
        if (pDestTarget == it->pDestTarget && !it->path.empty()) {
            
            if (it->pWall != NULL && it->pWall->isEmptyHP()) {
                continue;
            }
            
            Path::iterator node = it->path.begin();
            PathNode endNode = it->path.back();
            
            int ox = abs(pPerson->getLogicX() - node->x);
            int oy = abs(pPerson->getLogicY() - node->y);
            
            int _range = range;
            if (pPerson->m_indexPerson % 2 != it->pPerson->m_indexPerson % 2) {  //士兵编队,使行走路径分散
                _range = 1;
            }
            
            if (ox <= _range && oy <= _range
                && it->pPerson->isCanJumpWall() == pPerson->isCanJumpWall()
                && it->pPerson->isFlying() == pPerson->isFlying()
                && it->pPerson->getSearchLength() <= pPerson->getSearchLength()) {
                
                int mindis = INT_MAX;
                int idx = 0;
                for (; node != it->path.end(); node++) {
                    
                    if (idx < range) {
                    
                        int ox = abs(node->x - endNode.x);
                        int oy = abs(node->y - endNode.y);
                        
                        int dis = ox + oy;
                        if (dis <= mindis) {
                            
                            pathNode = node;
                            mindis = dis;
                        }
                    }
                }
                
                break;
            }
        }
    }
    
    bool isfind = false;
    
    if (it != m_vecPathCache.end()) {
        
        int ex = pathNode->x;
        int ey = pathNode->y;
        
        int maxfar = 5;
        if(m_astar.search(pPerson->getLogicX(), pPerson->getLogicY(), ex, ey, 0, pPerson->isCanJumpWall(), false, maxfar, pPerson->getLogicWidth()/2))
        {
            pPerson->clearPath();
            for(AStarNodeList::iterator it = m_astar.m_lstResult.begin(); it != m_astar.m_lstResult.end(); ++it)
            {
                if(it->x != pPerson->getLogicX() || it->y != pPerson->getLogicY())
                {
                    pPerson->addPath(it->x, it->y);
                }
            }
            
            for (; pathNode != it->path.end(); pathNode ++) {
                pPerson->addPath(pathNode->x, pathNode->y);
            }
            
            if (it->pWall != NULL && !it->pWall->isEmptyHP()) {
                pDestTarget = it->pWall;
            }
            
            isfind = true;
        }
    }
    
    return isfind;
}

void PlayerScene::addNote(const char* str)
{
	if(m_pGameWorld != NULL && m_pGameWorld->m_pNotifyUI != NULL)
		m_pGameWorld->m_pNotifyUI->addNote(str);
}

void PlayerScene::onDropSoldier2Attack(int sx, int sy)
{
    if(LogicInfo::getSingleton().m_isFinishAttack)
        return;
    
	int x = (sx - m_pRoot->getPosition().x) / getScale();
	int y = (sy - m_pRoot->getPosition().y) / getScale();
    
    CCPoint pt = countTilePos(x, y);
    
	if(isValidPos(pt.x, pt.y))
	{
		if((PlayerScene::getSingleton().m_bSelClanSoldier || m_curSelPersonID > 0) && ((pt.x == 0 || pt.y == 0 || pt.x == LOGIC_W -1 || pt.y == LOGIC_W - 1) || m_mapDataOcc.get(pt.x, pt.y) == 0))
		{
			int lx = pt.x;
			int ly = pt.y;
            
            if (LogicInfo::getSingleton().m_fightBeginTimeMS == 0) {
                LogicInfo::getSingleton().beginFight();
            }
            
            if (PlayerScene::getSingleton().m_bSelClanSoldier) {
                if (ClanMgr::getSingleton().m_fightAttackSoldier.size() > 0) {
                    
                    if (m_pBuildingClanAllies == NULL) {
                        
                        int buildid = BUILDING_CLAN_ALLIES*100 + 1;
                        m_pBuildingClanAllies = BuildingMgr::getSingleton().newBuilding(buildid);
                        m_pBuildingClanAllies->init(buildid, lx, ly);
                        
                        LogicInfo::getSingleton().m_clanSoldierUsed = 1;
                        LogicInfo::getSingleton().m_isAttackDropPerson = true;
                                                
                        LogicInfo::getSingleton().onAddCtrl_UseClanSoldier(lx, ly);
                        LogicInfo::getSingleton().addUsedFightSoldier(CLAN_SOLDIER_ID);
                        
                        LogicInfo::getSingleton().onNotify(NOTIFY_DROPSOLDIER);
                    }
                }
            }
            else {
                
                fixDropPosition(lx, ly);
                LogicInfo::getSingleton().dropSoldier2Fight(m_curSelPersonID, lx, ly);
            }
		}
	}
}

// 处理 push
void PlayerScene::onProcPushMsg()
{
    CAppPlatform::sharedAppPlatform()->cancelAllNotifications();
    
    if (LogicInfo::getSingleton().isNormalMode()) {
        
        for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
        {
            if((*it)->isEmptyHP())
                continue ;
            
            UserBuildingInfo* pInfo = (*it)->m_pBuildingInfo;
            if(pInfo != NULL && (pInfo->state == 0 || pInfo->state == 1))
            {
                int level = pInfo->buildingid % 100;
                char str[128];
                if (level == 1) {
                    sprintf(str, ViewString::getValue("building_build_complete").c_str(), (*it)->m_info.name.c_str());
                }
                else{
                    sprintf(str, ViewString::getValue("building_upgrade_complete").c_str(), (*it)->m_info.name.c_str(), level);
                }
                
                CAppPlatform::sharedAppPlatform()->fireNotification((*it)->m_lasttime, str);
            }
        }
        
        //calc soldier troops total train time
        MapBarrackInfo mapBarrack;
        for(MapBarrackInfo::iterator it = LogicInfo::getSingleton().m_mapBarrack.begin(); it != LogicInfo::getSingleton().m_mapBarrack.end(); ++it)
        {
            PairBarrackInfo pbi;
            pbi.first = it->first;
            pbi.second.lasttime = it->second.lasttime;
            
            for(std::list<BarrackSoldierInfo>::iterator it1 = it->second.lstSoldier.begin(); it1 != it->second.lstSoldier.end(); ++it1){
                BarrackSoldierInfo soldier;
                soldier.pid = it1->pid;
                soldier.needtimeex = it1->needtimeex;
                soldier.numsex = it1->numsex;
                pbi.second.lstSoldier.push_back(soldier);
            }
            mapBarrack.insert(pbi);
        }
        
        int needtime = 0;
        int lasttime = 0;
        int curSpace, maxSpace;
        LogicInfo::getSingleton().countTroopSpace(LogicInfo::getSingleton().m_mapTroophousing, curSpace, maxSpace);
        
        while (curSpace < maxSpace) {
            
            lasttime = 99999;
            
            int nums = 0;
            for(MapBarrackInfo::iterator it = mapBarrack.begin(); it != mapBarrack.end(); ++it)
            {
                if (!it->second.isUpgrade && (it->second.lstSoldier.size() > 0 && it->second.lasttime < lasttime)) {
                    lasttime = it->second.lasttime;
                }
            }
            
            for(MapBarrackInfo::iterator it = mapBarrack.begin(); it != mapBarrack.end(); ++it)
            {
                if (it->second.lstSoldier.size() > 0) {
                    
                    std::list<BarrackSoldierInfo>::iterator soldier = it->second.lstSoldier.begin();
                    const SoldierInfo* pInfo = PersonMgr::getSingleton().getSoldierInfo(soldier->pid);
                    it->second.lasttime -= lasttime;
                    
                    if (it->second.lasttime <= 0) {
                        curSpace+= pInfo->housingSpaceex;
                        
                        it->second.lasttime = soldier->needtimeex;
                        soldier->numsex -= 1;
                        if (soldier->numsex <= 0)
                            it->second.lstSoldier.erase(soldier);
                    }
                    nums += soldier->numsex;
                }
            }
            
            needtime += lasttime;
            
            if (nums == 0 || curSpace >= maxSpace) {
                break;
            }
        }
        
        if (needtime > 0 && curSpace >= maxSpace) {
            CAppPlatform::sharedAppPlatform()->fireNotification(needtime, ViewString::getValue("soldier_troops_full").c_str());
        }
    }
}

void PlayerScene::chgScaleMode(bool isScaleMode)
{
	m_pRoot->m_bProcScaleAni = !isScaleMode;
    
	m_isScaleMode = isScaleMode;
    
	if(isScaleMode)
	{
		if(m_pEarthquakeAct != NULL)
		{
			m_pRoot->stopAction(m_pEarthquakeAct);
            
			m_pEarthquakeAct->release();
            
			m_pEarthquakeAct = NULL;
		}
	}
}

void PlayerScene::clearTouchMoving()
{
	if(m_pTouchMoving != NULL)
	{
		m_pRoot->stopAction(m_pTouchMoving);
        
		m_pTouchMoving->release();
        
		m_pTouchMoving = NULL;
	}
}

void PlayerScene::addTouchMoving(int ox, int oy, float offTime)
{
	if(m_pTouchMoving != NULL)
		return;
    
	if(offTime == 0 || offTime > 2)
		return;
    
	CCPoint pt;
	pt.x = ox * 2 / (offTime*10.0);
	pt.y = oy * 2 / (offTime*10.0);
    
	CCMoveBy* moveAct = CCMoveBy::create(0.7f, pt);
	CCEaseIn* easeAct = CCEaseIn::create(moveAct, 0.5f);
    
	m_pTouchMoving = m_pRoot->runAction(easeAct);
	m_pTouchMoving->retain();
}

void PlayerScene::clearHero()
{
	for(int i = 0; i < 2; ++i)
	{
		for(_PersonList::iterator it = m_lstPerson[i].begin(); it != m_lstPerson[i].end(); )
		{
			Person* pPerson = *it;
            
			if(pPerson->m_personid / 10000 == 3)
			{
				pPerson->releaseFromScene();
                
				delete pPerson;
                
				it = m_lstPerson[i].erase(it);
			}
            else {
				++it;
            }
		}
	}
}

void PlayerScene::completedAnimationSequenceNamed(const char *name)
{
    
}

Building* PlayerScene::findBuildingWithIndex(int index)
{
	for(_BuildingList::iterator it = m_lstBuilding.begin(); it != m_lstBuilding.end(); ++it)
	{
		if((*it)->m_indexBuilding == index)
		{
			return *it;
		}
	}
    
	return NULL;
}

Person* PlayerScene::findPersonWithIndex(int camp, int index)
{
	if(camp == 0 || camp == 1)
	{
		for(_PersonList::iterator it = m_lstPerson[camp].begin(); it != m_lstPerson[camp].end(); ++it)
		{
			Person* pPerson = *it;
            
			if(pPerson->m_indexPerson == index)
				return pPerson;
		}
	}
    
	return NULL;
}

void PlayerScene::addTestLog(const char *s)
{
    if (LogicInfo::getSingleton().m_curState == LOGICSTATE_LOADING) {

    }
}

NS_COC_END
