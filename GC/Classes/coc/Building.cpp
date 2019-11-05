#include "Building.h"
#include "PlayerScene.h"
#include "ImgMgr.h"
#include "BuildingMgr.h"
#include "GuideMgr.h"
#include "HttpClient.h"
#include "LogicInfo.h"
#include "ParticleSystem.h"
#include "SoundMgr.h"
#include "../form/MainEditUI.h"

#include "AniObjMgr.h"
#include "HeroMgr.h"
#include "../form/MainEditUI.h"
#include "../GameWorldScene.h"

NS_COC_BEGIN

Building::Building(BuildingInfo info)
: BaseObj(OBJTYPE_BUILDING)
, m_info(info)
, m_pTile(NULL)
//	, m_pBulding(NULL)
//	, m_pShadow(NULL)
, m_pTileColor(NULL)
, m_isTouch(false)
, m_type(BUILDING_TYPE_NORMAL)
//	, m_pSelAction(NULL)
, m_state(BUILDING_CTRL_NORMAL)
, m_lasttime(0)
, m_lasttimems(0)
, m_fulltime(0)
, m_pBuildingInfo(NULL)
//, m_pResTitle(NULL)
, m_curResTime(-1)
, m_curResTimems(0)
, m_collectResTimes(0)
, m_pFence(NULL)
//, m_pHPBack(NULL)
, m_pProHP(NULL)
, m_pDestroy(NULL)
, m_pPreBuilding(NULL)
, m_isPreBuilding(false)
, m_pActBeAttack(NULL)
, m_pLabelLevel(NULL)
, m_pMFTime(NULL)
, m_pProTime(NULL)
, m_psGold(NULL)
, m_psOil(NULL)
, m_pIcon(NULL)
//, m_pAttackTrgetAnimate(NULL)
//, m_pAttackTrgetAction(NULL)
, m_pAttackTarget(NULL)
, m_isRemove(false)
, m_isResFull(false)
, m_pIconEx(NULL)
, m_indexBuilding(0)
, m_editStatus(EDITBUILD_STATUS_INVALID)
{
    //m_hp = m_info.hp;
    
    setHP(m_info.hp);
    setLogicWidth(m_info.width);
    
    for(int i = 0; i < 3; ++i)
        m_pSkillEffect[i] = NULL;
}

Building::~Building()
{
}

void Building::init(int buildingid, int lx, int ly)
{
    m_lbx = lx;
    m_lby = ly;
    m_buildingid = buildingid;
    
    if(m_type != BUILDING_TYPE_WALL && (!LogicInfo::getSingleton().isAttackMode()
                                        || m_type != BUILDING_TYPE_TRAP))
    {
        char strname[128];
        sprintf(strname, "tile%d", m_info.width / 2);
        m_pTile = ImgMgr::getSingleton().loadSprite(strname);
        m_pTile->setAnchorPoint(ccp(0.5, 0));
        m_pTile->retain();
        
        PlayerScene::getSingleton().addTile(m_pTile);
    }
    
    if(!LogicInfo::getSingleton().isAttackMode())
    {
        if (GuideMgr::getSingleton().isGuiding()) {
            CCSprite* pFrameBg = ImgMgr::getSingleton().loadSprite("ui_img_pro_bk_1");
            CCSprite* pFrame = ImgMgr::getSingleton().loadSprite("ui_img_pro_org_3");
            
            m_pProHP = CProgress::create(pFrame, pFrameBg);
            m_pProHP->setAnchorPoint(ccp(0.5, 0));
            
            m_pProHP->retain();
            m_pProHP->setVisible(false);
            PlayerScene::getSingleton().m_pRoot->addChild(m_pProHP, SCENE_Z_BUILDINGUI);
        }
    }
    else
    {
        CCSprite* pFrameBg = ImgMgr::getSingleton().loadSprite("ui_img_pro_bk_1");
        CCSprite* pFrame = ImgMgr::getSingleton().loadSprite("ui_img_pro_org_3");
        m_pProHP = CProgress::create(pFrame, pFrameBg);
        m_pProHP->setAnchorPoint(ccp(0.5, 0));
        
        m_pProHP->retain();
        PlayerScene::getSingleton().m_pRoot->addChild(m_pProHP, SCENE_Z_BUILDINGUI);
        
        if(m_buildingid < 50000 && getHP() < fixMaxHP())
        {
            m_pProHP->setVisible(true);
            m_pProHP->changeProgress((float)getHP() / fixMaxHP());
        }
        else
            m_pProHP->setVisible(false);
    }
    
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
    
    onUpdateTime();
    
    setLogicPos(lx, ly);
}

void Building::setLogicPos(int lx, int ly)
{
    CCPoint pt = countSceneTilePt(lx, ly);
    
    setScenePt(pt);
    
    if(m_pTile != NULL)
        m_pTile->setPosition(pt);
    
    if(m_pTileColor != NULL)
        m_pTileColor->setPosition(pt);
    
    if(m_pMFTime != NULL)
    {
        CCPoint cpt = pt;
        
        if(m_pIcon != NULL)
        {
            cpt.y += getHeight() + 40;
        }
        else
        {
            cpt.y += getHeight() + 40;
        }
        
        m_pMFTime->setPosition(cpt);
    }
    
    if(m_pProTime != NULL)
    {
        CCPoint cpt = pt;
        cpt.y += getHeight() + 20;
        
        if(m_pIcon != NULL)
        {
            m_pProTime->setPosition(cpt);
            
            m_pIcon->setScale(0.9f);
            cpt.x -= (m_pProTime->getContentSize().width+m_pIcon->getContentSize().width)/2;
            cpt.y -= m_pIcon->getContentSize().height/3;
            
            m_pIcon->setPosition(cpt);
        }
        else
            m_pProTime->setPosition(cpt);
    }
    
    if(m_pIconEx != NULL)
    {
        CCPoint cpt = pt;
        cpt.y += getHeight();
        
        m_pIconEx->setPosition(cpt);
    }
    
    if(m_pFence != NULL)
    {
        CCPoint cpt = pt;
        m_pFence->setPosition(cpt);
    }
    
    if(m_pProHP != NULL)
    {
        CCPoint cpt = pt;
        cpt.y += getHeight();
        
        m_pProHP->setPosition(cpt);
    }
    
    if(m_pLabelName != NULL)
    {
        CCPoint cpt = pt;
        if (m_pMFTime == NULL || !m_pMFTime->isVisible()) {
            cpt.y += getHeight();
        }
        else {
            cpt.y += getHeight() + 90;
        }
        
        m_pLabelName->setPosition(cpt);
        m_pLabelLevel->setPosition(cpt);
    }
    
    if(m_pDestroy != NULL)
        m_pDestroy->setPosition(pt);
    
    if(m_pPreBuilding != NULL)
    {
        CCPoint cpt = pt;
        m_pPreBuilding->setPosition(cpt);
    }
    
    if(m_pAttackTarget != NULL)
    {
        CCPoint cpt = pt;
        cpt.y += getHeight() / 2;
        
        m_pAttackTarget->setPosition(cpt);
    }
    
    if (m_pBuildingInfo) {
        m_pBuildingInfo->x = lx;
        m_pBuildingInfo->y = ly;
    }
    
    BaseObj::setLogicPos(lx, ly);
    
    if(m_state == BUILDING_CTRL_BUILD || m_state == BUILDING_CTRL_MOVE)
    {
        buildTest();
        PlayerScene::getSingleton().showArrow(getLogicX(), getLogicY(), m_info.width);
    }
    
    {
        CCPoint cpt = getCenterPt();
        CCPoint npt = PlayerScene::getSingleton().countScenePos(cpt.x, cpt.y);
        
        for(int i = 0; i < 3; ++i)
        {
            if(m_pSkillEffect[i] != NULL)
            {
                m_pSkillEffect[i]->setPosition(npt);
            }
        }
    }
    
    reorderMe();
}

void Building::setVisible(bool isVisible)
{
    BaseObj::setVisible(isVisible);
    
    if (m_pTile) {
        m_pTile->setVisible(isVisible);
    }
    if (m_pTileColor) {
        m_pTileColor->setVisible(isVisible);
    }
    if (m_pLabelLevel) {
        m_pLabelLevel->setVisible(isVisible);
    }
    if (m_pMFTime) {
        m_pMFTime->setVisible(isVisible);
    }
    if (m_pProTime) {
        m_pProTime->setVisible(isVisible);
    }
    if (m_pFence) {
        m_pFence->setVisible(isVisible);
    }
    if (m_pProHP) {
        m_pProHP->setVisible(isVisible);
    }
    if (m_pIcon) {
        m_pIcon->setVisible(isVisible);
    }
    if (m_pIconEx) {
        m_pIconEx->setVisible(isVisible);
    }
}

int Building::getHeight()
{
    return m_info.width * TILE_H;
}

int Building::getWidth()
{
    return m_info.width * TILE_W;
}

int Building::getBuildingWidth()
{
    return getWidth();
}

int Building::getBuildingHeight()
{
    return getHeight();
}

void Building::reorder(int zorder)
{
    if(m_pPreBuilding != NULL)
        PlayerScene::getSingleton().m_pRoot->reorderChild(m_pPreBuilding, zorder);
    
    if(m_pFence != NULL)
        PlayerScene::getSingleton().m_pRoot->reorderChild(m_pFence, zorder + SCENE_Z_BUILDING_JUMP - 1);
    
    if(m_pSkillEffect[1] != NULL)
        PlayerScene::getSingleton().m_pRoot->reorderChild(m_pSkillEffect[1], zorder + SCENE_Z_BUILDING_JUMP - 1);
}

void Building::reorderMe()
{
    int zorder = 0;
    
    if (m_lw == 2) {
        zorder = (getLogicX() + getLogicY()) * SCENE_Z_BUILDING_JUMP;
    }
    else {
        zorder = (getLogicX() + getLogicY() - getLogicWidth()/2) * SCENE_Z_BUILDING_JUMP;
    }
    
    if (zorder < SCENE_Z_TILE) {
        zorder = SCENE_Z_TILE;
    }
    
    if (m_isTouchMoved) {
        zorder = SCENE_Z_CURBUILDING;
    }
    
    reorder(zorder);
}

CCPoint Building::countSceneTilePt(int lx, int ly)
{
    CCPoint pt = PlayerScene::getSingleton().countScenePos(lx, ly);
    
    pt.x += (TILE_W/2);
    
    return pt;
}

CCPoint Building::getScenePt()
{
    return m_scenePt;
}

void Building::setScenePt(CCPoint pt)
{
    m_scenePt = pt;
}

bool Building::isIn(int lx, int ly) const
{
    if(lx <= getLogicX() && ly <= getLogicY() && lx > getLogicX() - m_info.width && ly > getLogicY() - m_info.width)
        return true;
    
    return false;
}

bool Building::isInIcon(int px, int py)
{
    if (m_pIconEx) {
        
        return m_pIconEx->boundingBox().containsPoint(ccp(px, py));
    }
    return false;
}

bool Building::isOverlap(int lx, int ly, int lw) const
{
    if(lx <= getLogicX() - m_info.width || lx - lw >= getLogicX())
        return false;
    
    if(ly <= getLogicY() - m_info.width || ly - lw >= getLogicY())
        return false;
    
    return true;
}

void Building::releaseFromScene()
{
    releaseProjectile2();
    
    if(m_psGold != NULL)
    {
        PlayerScene::getSingleton().m_pRoot->removeChild(m_psGold, true);
        m_psGold->release();
        m_psGold = NULL;
    }
    
    if(m_psOil != NULL)
    {
        PlayerScene::getSingleton().m_pRoot->removeChild(m_psOil, true);
        m_psOil->release();
        m_psOil = NULL;
    }
    
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
    
    if(m_pTile != NULL)
    {
        PlayerScene::getSingleton().removeTile(m_pTile);
        m_pTile->release();
        m_pTile = NULL;
    }
    
    if(m_pIconEx != NULL)
    {
        PlayerScene::getSingleton().m_pRoot->removeChild(m_pIconEx, true);
        m_pIconEx->release();
        m_pIconEx = NULL;
    }
    
    if(m_pFence != NULL)
    {
        PlayerScene::getSingleton().m_pRoot->removeChild(m_pFence, true);
        m_pFence->release();
        m_pFence = NULL;
    }
    
    if(m_pProHP != NULL)
    {
        PlayerScene::getSingleton().m_pRoot->removeChild(m_pProHP, true);
        m_pProHP->release();
        m_pProHP = NULL;
    }
    
    if(m_pTileColor != NULL)
    {
        m_pTileColor->setVisible(false);
        m_pTileColor = NULL;
    }
    
    if(m_pLabelName != NULL)
    {
        PlayerScene::getSingleton().m_pRoot->removeChild(m_pLabelName, true);
        m_pLabelName->release();
        m_pLabelName = NULL;
    }
    if (m_pLabelLevel != NULL) {
        PlayerScene::getSingleton().m_pRoot->removeChild(m_pLabelLevel, true);
        m_pLabelLevel->release();
        m_pLabelLevel = NULL;
    }
    
    if(m_pDestroy != NULL)
    {
        PlayerScene::getSingleton().m_pRoot->removeChild(m_pDestroy, true);
        m_pDestroy->release();
        m_pDestroy = NULL;
    }
    
    if(m_pPreBuilding != NULL)
    {
        PlayerScene::getSingleton().m_pRoot->removeChild(m_pPreBuilding, true);
        m_pPreBuilding->release();
        m_pPreBuilding = NULL;
    }
    
    if(m_pIcon != NULL)
    {
        PlayerScene::getSingleton().m_pRoot->removeChild(m_pIcon, true);
        m_pIcon->release();
        m_pIcon = NULL;
    }
    
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
    
    BaseObj::releaseFromScene();
}

void Building::buildTest()
{
    if(m_pTileColor != NULL)
        m_pTileColor->setVisible(false);
    
    if(PlayerScene::getSingleton().canBuilding(m_buildingid, getLogicX(), getLogicY(), m_info.width))
    {
        CCPoint pos = PlayerScene::getSingleton().countScenePos(getCenterPt().x, getCenterPt().y);
        
        if(m_state == BUILDING_CTRL_BUILD)
        {
            PlayerScene::getSingleton().setBuildUIPos(pos.x + TILE_W / 2, pos.y + getHeight()/2, false);
            
            m_pTileColor = PlayerScene::getSingleton().getTileColor(true, m_info.width);
        }
        else
            m_pTileColor = NULL;
    }
    else
    {
        CCPoint pos = PlayerScene::getSingleton().countScenePos(getCenterPt().x, getCenterPt().y);
        
        if(m_state == BUILDING_CTRL_BUILD)
        {
            PlayerScene::getSingleton().setBuildUIPos(pos.x + TILE_W / 2, pos.y + getHeight()/2, true);
        }
        
        //PlayerScene::getSingleton().hideBuildUI();
        
        m_pTileColor = PlayerScene::getSingleton().getTileColor(false, m_info.width);
    }
    
    if(m_pTileColor != NULL)
    {
        CCPoint pt = countSceneTilePt(getLogicX(), getLogicY());
        
        m_pTileColor->setPosition(pt);
        m_pTileColor->setVisible(true);
    }
    
    //chgSelMode(true);
}

bool Building::canBuild()
{
    return PlayerScene::getSingleton().canBuilding(m_buildingid, getLogicX(), getLogicY(), m_info.width);
}

bool Building::buildOK()
{
    chgSelMode(false);
    
    if(PlayerScene::getSingleton().canBuilding(m_buildingid, getLogicX(), getLogicY(), m_info.width))
    {
        if(m_state == BUILDING_CTRL_BUILD)
        {
            if(PlayerScene::getSingleton().m_isInit)
            {
                int fulltime = ((m_info.buildtime[0] * 24 + m_info.buildtime[1]) * 60 + m_info.buildtime[2]) * 60;
                m_pBuildingInfo->lasttime = fulltime;
                m_pBuildingInfo->curtime = getCurrentTime();
                setTime(fulltime, fulltime);
            }
        }
        else if(m_state == BUILDING_CTRL_MOVE)
        {
            if(m_lbx != getLogicX() || m_lby != getLogicY())
            {
                HttpClient::getSingleton().move(m_pBuildingInfo->id, m_lbx, m_lby, getLogicX(), getLogicY());
                
                int bid = m_buildingid / 100;
                if(bid == BUILDING_ARMY_CAMP && !PlayerScene::getSingleton().m_isEditMode)
                {
                    PlayerScene::getSingleton().onTroophousingMov(*this);
                }
            }
        }
        
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
        
        //m_scene.m_lstBuilding.push_back(this);
        
        chgState(BUILDING_CTRL_NORMAL);
        
        return true;
    }
    
    return false;
}

//void Building::reorder(int zorder)
//{
//	if(m_pBulding != NULL)
//	{
//		m_scene.m_pRoot->reorderChild(m_pBulding, zorder);
//	}
//}

int Building::countDistance(int lx, int ly)
{
    int tx = getCenterPt().x;
    int ty = getCenterPt().y;
    
    //if(m_info.width > 1)
    //{
    //	if(lx < tx)
    //		tx = m_lx - m_info.width + 1;
    
    //	if(ly < ty)
    //		ty = m_ly - m_info.width + 1;
    //}
    
    return (lx - tx) * (lx - tx) + (ly - ty) * (ly - ty);
}

void Building::setUpDownAction(CCSprite* pSpr, bool isCenter)
{
    CCPoint pt1 = pSpr->getPosition();
    CCPoint pt2 = pSpr->getPosition();
    
    pt1.y += 3;
    
    if(!isCenter)
        pt1.x -= pSpr->getContentSize().width * 0.1f / 2;
    
    CCMoveTo* mv1 = CCMoveTo::create(0.08f, pt1);
    CCMoveTo* mv2 = CCMoveTo::create(0.08f, pt2);
    
    CCFiniteTimeAction* action1 = CCSequence::create(mv1, mv2, NULL);
    
    CCScaleTo* sc1 = CCScaleTo::create(0.08f, 1.1f);
    CCScaleTo* sc2 = CCScaleTo::create(0.08f, 1.0f);
    
    CCFiniteTimeAction* action2 = CCSequence::create(sc1, sc2, NULL);
    
    pSpr->runAction(action1);
    pSpr->runAction(action2);
}

//void Building::resort()
//{
//	if(m_pBulding != NULL)
//	{
//		m_scene.m_pRoot->removeChild(m_pBulding, false);
//		m_scene.m_pRoot->addChild(m_pBulding, 10);
//	}
//}

void Building::chgState(int state)
{
    m_state = state;
    
    if(state == BUILDING_CTRL_NORMAL)
    {
        m_lbx = getLogicX();
        m_lby = getLogicY();
    }
    else if(state == BUILDING_CTRL_BUILD)
    {
        PlayerScene::getSingleton().showArrow(getLogicX(), getLogicY(), m_info.width);
    }
    else if (state == BUILDING_CTRL_SELECTED)
    {
        PlayerScene::getSingleton().showArrow(getLogicX(), getLogicY(), m_info.width);
    }
}

void Building::onProcIconEx()
{
    if(m_pBuildingInfo == NULL)
        return;
    
    if(m_pBuildingInfo->state != 2)
    {
        if(m_pIconEx != NULL)
        {
            m_pIconEx->setVisible(false);
            m_strIconEx = "";
        }
        
        return;
    }
    
    int bid = m_buildingid / 100;
    
    if(m_pIconEx != NULL && m_pIconEx->isVisible())
    {
        if(bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_GOLD_MINE)
        {
            if (LogicInfo::getSingleton().isNormalMode()) {
                
                int64_t res = (m_curResTime * m_info.producesperhour / (60 * 60));
                if(m_info.producesres == MONEY_ELIXIR)
                {
                    if(res > m_info.maxstored[1])
                        res = m_info.maxstored[1];
                    
                    bool isFull = LogicInfo::getSingleton().isOilFull(res);
                    if((isFull && !m_isResFull) || (!isFull && m_isResFull))
                    {
                        m_isResFull = isFull;
                        
                        if(isFull)
                        {
                            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("ui_img_oil_f");
                            
                            m_pIconEx->setDisplayFrame(pFrame);
                        }
                        else
                        {
                            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("ui_img_oil");
                            
                            m_pIconEx->setDisplayFrame(pFrame);
                        }
                    }
                }
                else
                {
                    if(res > m_info.maxstored[0])
                        res = m_info.maxstored[0];
                    
                    bool isFull = LogicInfo::getSingleton().isGoldFull(res);
                    if((isFull && !m_isResFull) || (!isFull && m_isResFull))
                    {
                        m_isResFull = isFull;
                        
                        if(isFull)
                        {
                            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("ui_img_gold_f");
                            
                            m_pIconEx->setDisplayFrame(pFrame);
                        }
                        else
                        {
                            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("ui_img_gold");
                            
                            m_pIconEx->setDisplayFrame(pFrame);
                        }
                    }
                }
            }
            
        } // res
        else if(bid == BUILDING_BARRACKS)
        {
            if(LogicInfo::getSingleton().isNormalMode())
            {
                if(LogicInfo::getSingleton().isEmptyBarrack(m_pBuildingInfo->barrackid) && !LogicInfo::getSingleton().isFullTroophousing())
                {
                    if(m_strIconEx != "ui_img_train")
                    {
                        chgIconEx("ui_img_train");
                    }
                }
                else if (LogicInfo::getSingleton().isFullTroophousing() && getBarrackInfo()->isPause)
                {
                    if (m_strIconEx != "ui_img_exc") {
                        chgIconEx("ui_img_exc");
                    }
                }
                else if(m_pIconEx != NULL)
                {
                    m_pIconEx->setVisible(false);
                    m_strIconEx = "";
                }
            }
            
        }
        else if(bid == BUILDING_ARMY_CAMP)
        {
            if (LogicInfo::getSingleton().isNormalMode()) {
                if(LogicInfo::getSingleton().isFullTroophousing())
                {
                    if(m_strIconEx != "ui_img_full")
                    {
                        chgIconEx("ui_img_full");
                    }
                }
                else if(m_pIconEx != NULL)
                {
                    m_pIconEx->setVisible(false);
                    m_strIconEx = "";
                }
            }
            
        }
    }
    else
    {
        if(bid == BUILDING_BARRACKS)
        {
            if (LogicInfo::getSingleton().isNormalMode()) {
                
                if (LogicInfo::getSingleton().isEmptyBarrack(m_pBuildingInfo->barrackid) && !LogicInfo::getSingleton().isFullTroophousing())
                {
                    if (m_strIconEx != "ui_img_train") {
                        chgIconEx("ui_img_train");
                    }
                }
                //else if(m_pIconEx != NULL)
                //	m_pIconEx->setVisible(false);
            }
        }
        else if(bid == BUILDING_ARMY_CAMP)
        {
            if(LogicInfo::getSingleton().isNormalMode())
            {
                if (LogicInfo::getSingleton().isFullTroophousing()) {
                    if (m_strIconEx != "ui_img_full") {
                        chgIconEx("ui_img_full");
                    }
                }
                //else if(m_pIconEx != NULL)
                //	m_pIconEx->setVisible(false);
            }
        }
    }
}

void Building::onProc(int ot)
{
    BaseObj::onProc(ot);
    
    onProcProjectile2(ot);
    
    onProcIconEx();
    
    if(m_pActBeAttack != NULL)
    {
        if(m_pActBeAttack->isDone())
        {
            m_pActBeAttack->release();
            
            m_pActBeAttack = NULL;
        }
    }
    
    if(m_pBuildingInfo != NULL && m_pBuildingInfo->state == 2 && m_curResTime >= 0)
    {
        m_curResTimems += ot;
        if(m_curResTimems >= 1000)
        {
            m_curResTime += m_curResTimems / 1000;
            m_curResTimems = m_curResTimems % 1000;
            
            m_collectResTimes --;
            
            if(hasNewRes() && m_collectResTimes <= 0){
                showResTitle();
                m_collectResTimes = 0;
            }
        }
    }
    
    if(m_pBuildingInfo != NULL && m_pBuildingInfo->state == 2)
    {
        int bid = m_buildingid / 100;
        
        if(bid == BUILDING_LABORATORY)
        {
            LogicInfo::getSingleton().onRefurbishLabMS(ot, *this);
        }
        else if(m_pBuildingInfo->barrackid >= 0)
        {
            LogicInfo::getSingleton().onRefurbishBarrackMs(m_pBuildingInfo->barrackid, ot, this);
        }
    }
    else if(m_lasttime > 0)
    {
        m_lasttimems += ot;
        if(m_lasttimems >= 1000)
        {
            m_lasttime -= m_lasttimems / 1000;
            m_lasttimems = m_lasttimems % 1000;
            
            onUpdateTime();
            
            if(m_lasttime <= 0)
            {
                m_lasttime = 0;
                
                if(m_buildingid >= 90000)
                    m_isRemove = true;
                
                LogicInfo::getSingleton().finishBuilding(*this);
            }
        }
    }
    
    if (LogicInfo::getSingleton().isNormalMode()) {
        if (m_hp < getMaxHP()) {
            
            int offhp = getMaxHP()/200;
            if (offhp < 1) {
                offhp = 1;
            }
            
            m_hp += offhp;
            
            if (m_hp >= getMaxHP()) {
                m_hp = getMaxHP();
            }
            
            m_pBuildingInfo->hp = m_hp;
            
            m_pProHP->changeProgress((float)getHP() / fixMaxHP());
            
            if (getHP() >= getMaxHP()) {
                
                if(m_pDestroy != NULL)
                {
                    PlayerScene::getSingleton().m_pRoot->removeChild(m_pDestroy, true);
                    m_pDestroy->release();
                    m_pDestroy = NULL;
                }
                
                if(m_pProHP != NULL)
                {
                    PlayerScene::getSingleton().m_pRoot->removeChild(m_pProHP, true);
                    m_pProHP->release();
                    m_pProHP = NULL;
                }
                
                setLogicPos(m_lx, m_ly);
            }
        }
    }
}

bool Building::hasNewRes()
{
    if (getHP() < getMaxHP()) {
        return false;
    }
    
    if(m_curResTime >= 30)
    {
        int64_t res = (m_curResTime * m_info.producesperhour / (60 * 60));
        if(res < 1)
            return false;
        
        return true;
    }
    
    return false;
}

void Building::onUpdateTime()
{
    if(m_lasttime <= 0)
    {
        if(m_pMFTime != NULL)
            m_pMFTime->setVisible(false);
        
        if(m_pProTime != NULL)
            m_pProTime->setVisible(false);
        
        showFence(false);
        
        return;
    }
    
    if(!LogicInfo::getSingleton().isAttackMode())
    {
        if(m_pProTime == NULL)
        {
            CCSprite* pFrameBg = ImgMgr::getSingleton().loadSprite("ui_img_pro_bk_1");
            CCSprite* pFrame = ImgMgr::getSingleton().loadSprite("ui_img_pro_org_3");
            
            m_pProTime = CProgress::create(pFrame, pFrameBg);
            m_pProTime->setAnchorPoint(ccp(0.5, 0));
            m_pProTime->retain();
            PlayerScene::getSingleton().m_pRoot->addChild(m_pProTime, SCENE_Z_BUILDINGUI);
        }
        
        if(m_pMFTime == NULL)
        {
            m_pMFTime = CCLabelBMFont::create("", "Font/tbFont_b.fnt", -1, kCCTextAlignmentCenter);
            m_pMFTime->setScale(0.5);
            m_pMFTime->retain();
            PlayerScene::getSingleton().m_pRoot->addChild(m_pMFTime, SCENE_Z_BUILDINGUI);
        }
    }
    
    if(m_pMFTime != NULL)
        m_pMFTime->setVisible(true);
    
    if(m_pProTime != NULL)
        m_pProTime->setVisible(true);
    
    CCPoint pt = countSceneTilePt(getLogicX(), getLogicY());
    
    if(m_pMFTime != NULL)
    {
        std::string str;
        
        LogicInfo::getSingleton().countLastTimeString(str, m_lasttime);
        
        m_pMFTime->setString(str.c_str());
        
        CCPoint cpt = pt;
        
        if(m_pIcon != NULL)
        {
            cpt.y += getHeight() + 40;
        }
        else
        {
            cpt.y += getHeight() + 40;
        }
        
        m_pMFTime->setPosition(cpt);
    }
    
    if(m_pProTime != NULL)
    {
        CCPoint cpt = pt;
        cpt.y += getHeight() + 20;
        
        if(m_pIcon != NULL)
        {
            m_pProTime->changeProgress(1 - (float)m_lasttime / m_fulltime);
            m_pProTime->setPosition(cpt);
            
            m_pIcon->setScale(0.9f);
            cpt.x -= (m_pProTime->getContentSize().width+m_pIcon->getContentSize().width)/2;
            cpt.y -= m_pIcon->getContentSize().height/3;
            
            m_pIcon->setPosition(cpt);
        }
        else
        {
            m_pProTime->changeProgress(1 - (float)m_lasttime / m_fulltime);
            
            m_pProTime->setPosition(cpt);
        }
    }
}

void Building::setTime(int fulltime, int lasttime)
{
    m_fulltime = fulltime;
    m_lasttime = lasttime;
    
    if(m_pBuildingInfo != NULL && (m_pBuildingInfo->state == 0 || m_pBuildingInfo->state == 1) && m_lasttime > 0)
        showFence(true);
    
    onUpdateTime();
}

void Building::showResTitle()
{
    if(LogicInfo::getSingleton().isNormalMode())
    {
        if(m_pIconEx == NULL)
        {
            int64_t res = (m_curResTime * m_info.producesperhour / (60 * 60));
            if(m_info.producesres == MONEY_ELIXIR)
            {
                if(res > m_info.maxstored[1])
                    res = m_info.maxstored[1];
                
                if(LogicInfo::getSingleton().isOilFull(res))
                {
                    m_pIconEx = ImgMgr::getSingleton().loadSprite("ui_img_oil_f");
                    m_pIconEx->setAnchorPoint(ccp(0.5, 0));
                    m_isResFull = true;
                }
                else
                {
                    m_pIconEx = ImgMgr::getSingleton().loadSprite("ui_img_oil");
                    m_pIconEx->setAnchorPoint(ccp(0.5, 0));
                    m_isResFull = false;
                }
            }
            else
            {
                if(res > m_info.maxstored[0])
                    res = m_info.maxstored[0];
                
                if(LogicInfo::getSingleton().isGoldFull(res))
                {
                    m_pIconEx = ImgMgr::getSingleton().loadSprite("ui_img_gold_f");
                    m_pIconEx->setAnchorPoint(ccp(0.5, 0));
                    m_isResFull = true;
                }
                else
                {
                    m_pIconEx = ImgMgr::getSingleton().loadSprite("ui_img_gold");
                    m_pIconEx->setAnchorPoint(ccp(0.5, 0));
                    m_isResFull = false;
                }
            }
            
            PlayerScene::getSingleton().m_pRoot->addChild(m_pIconEx, SCENE_Z_BUILDINGUI);
            m_pIconEx->setAnchorPoint(ccp(0.5, 0));
            m_pIconEx->retain();
        }
        
        if(m_pIconEx != NULL && !PlayerScene::getSingleton().m_isEditMode)
        {
            m_pIconEx->setVisible(true);
            
            onProcIconEx();
            
            CCPoint pt = countSceneTilePt(getLogicX(), getLogicY());
            pt.y += getHeight();
            
            m_pIconEx->setPosition(pt);
        }
    }
}

void Building::hideResTitle()
{
    if(m_pIconEx != NULL)
        m_pIconEx->setVisible(false);
    
    m_collectResTimes = 60;
}

bool Building::canMove()
{
    if(m_info.buildingid / 10000 == 9 || m_info.buildingid == 30501)
        return false;
    
    return true;
}

void Building::showFence(bool bVisible)
{
    if(isFenceVisible() == bVisible)
        return;
    
    if(m_buildingid >= 90000)
        return;
    
    onBuilding(bVisible);
    
    if(m_pFence != NULL)
    {
        m_pFence->setVisible(bVisible);
        
        if(!bVisible)
            onBuildingEnd();
        
        reorderMe();
    }
    else if(bVisible)
    {
        char strname[128];
        sprintf(strname, "fence%d", m_info.width / 2);
        m_pFence = ImgMgr::getSingleton().loadSprite(strname);
        m_pFence->setAnchorPoint(ccp(0.5, 0));
        m_pFence->retain();
        PlayerScene::getSingleton().m_pRoot->addChild(m_pFence, SCENE_Z_TILE);
        
        //		if(m_pBuildingInfo != NULL && m_pBuildingInfo->state == 0)
        //			showPreBuilding();
        
        reorderMe();
    }
}

bool Building::isFenceVisible() const
{
    if(m_pFence == NULL)
        return false;
    
    return m_pFence->isVisible();
}

void Building::chgHP(int off, bool flyfont, bool isReal)
{
    if(off == 0)
        return;
    
    if(isEmptyHP())
        return;
    
    //! 不掉血的建筑
    if(m_buildingid / 10000 == 9 || m_buildingid / 10000 == 6)
        return;
    
    int old = getHP();
    
    BaseObj::chgHP(off, flyfont);
    
    int nhp = getHP();
    int rhp = nhp;
    if(LogicInfo::getSingleton().isReplayMode())
    {
        if(!isReal) {
            if(nhp <= 0) {
                nhp = 1;
                rhp = 0;
            }
        }
        else {
            if(nhp < 0) {
                nhp = 0;
                rhp = 0;
            }
        }
        setHP(nhp);
    }
    
    if (m_pBuildingInfo) {
        m_pBuildingInfo->hp = m_hp;
    }
    
    int bid = m_buildingid / 100;
    
    if((bid == BUILDING_TOWN_HALL || bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_ELIXIR_STORAGE ||
        bid == BUILDING_GOLD_MINE || bid == BUILDING_GOLD_STORAGE) && rhp < old && rhp > 0)
    {
        LogicInfo::getSingleton().onDropOutRes(*this, (old - rhp) / (float)fixMaxHP());
    }
    
    setBeAttackMode();
    
    if(isEmptyHP())
    {
        LogicInfo::getSingleton().onDestroyBuilding(*this);
        PlayerScene::getSingleton().onDestroyBuilding(*this);
        
        showDestroy();
    }
    else if(getHP() < fixMaxHP()){
        if (m_pProHP) {
            m_pProHP->setVisible(true);
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

BarrackInfo* Building::getBarrackInfo()
{
    if(m_pBuildingInfo != NULL && m_pBuildingInfo->barrackid >= 0)
        return LogicInfo::getSingleton().getBarrackInfo(m_pBuildingInfo->barrackid);
    
    return NULL;
}

void Building::chgSelMode(bool isSel)
{
    if(isSel)
    {
        if(!LogicInfo::getSingleton().isAttackMode())
        {
            char str[128];
            
            //! Clan特殊处理
            if(m_info.buildingid / 100 == BUILDING_CLAN_CASTLE)
            {
                if(m_info.buildingid % 100 == 1)
                    sprintf(str, "");
                else
                    sprintf(str, "Level %d", m_info.buildingid % 100 - 1);
            }
            else
                sprintf(str, "Level %d", m_info.buildingid % 100);
            
            if(m_pLabelName == NULL)
            {
                m_pLabelName = CCLabelBMFont::create(m_info.name.c_str(), "Font/tbFont_b.fnt", -1, kCCTextAlignmentCenter);
                
                m_pLabelName->setColor(ccc3(255, 255, 198));
                m_pLabelName->setScale(0.7);
                m_pLabelName->setAnchorPoint(ccp(0.5, 0));
                m_pLabelName->setVisible(false);
                
                m_pLabelName->retain();
                PlayerScene::getSingleton().m_pRoot->addChild(m_pLabelName, SCENE_Z_BUILDINGUI);
            }
            
            if(m_pLabelLevel == NULL)
            {
                m_pLabelLevel = CCLabelBMFont::create(str, "Font/tbFont_b.fnt", -1, kCCTextAlignmentCenter);
                m_pLabelLevel->setColor(ccc3(206, 255, 90));
                m_pLabelLevel->setScale(0.6);
                m_pLabelLevel->setAnchorPoint(ccp(0.5, 1.2));
                
                //m_pLabelName->retain();
                m_pLabelLevel->retain();
                //PlayerScene::getSingleton().m_pRoot->addChild(m_pLabelName, SCENE_Z_BUILDINGUI);
                PlayerScene::getSingleton().m_pRoot->addChild(m_pLabelLevel, SCENE_Z_BUILDINGUI);
            }
        }
        
        if(m_pLabelName != NULL)
            m_pLabelName->setVisible(true);
        if (m_pLabelLevel != NULL)
            m_pLabelLevel->setVisible(true);
        
        setLogicPos(getLogicX(), getLogicY());
    }
    else
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
}

void Building::showDestroy()
{
    releaseProjectile2();
    
    //! 释放特效
    for(int i = 0; i < 3; ++i)
    {
        if(m_pSkillEffect[i] != NULL)
        {
            AniObjMgr::getSingleton().releaseAllAni(m_pSkillEffect[i]);
        }
    }
    
    clearAllBuff();
    
    if (LogicInfo::getSingleton().isAttackMode())
    {
        CCPoint pt = countSceneTilePt(getLogicX(), getLogicY());
        PlayerScene::getSingleton().addBuildingBoom(pt.x, pt.y + getHeight() / 2);
    }
    
    if(m_pDestroy == NULL)
    {
        char str[128];
        if(m_info.width == 6)
        {
            if(m_buildingid / 100 == 203 || m_buildingid / 100 == 204)
                sprintf(str, "destroy_3_0");
            else
                sprintf(str, "destroy_3_1");
        }
        else
            sprintf(str, "destroy_%d", m_info.width / 2);
        
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        if(pFrame != NULL)
        {
            m_pDestroy = CCSprite::create();
            m_pDestroy->initWithSpriteFrame(pFrame);
            m_pDestroy->setAnchorPoint(ccp(0.5, 0));
            m_pDestroy->retain();
            
            PlayerScene::getSingleton().m_pRoot->addChild(m_pDestroy, SCENE_Z_DESTROY);
        }
    }
    
    if(m_pFence != NULL)
        m_pFence->setVisible(false);
    
    if(m_pProHP != NULL)
        m_pProHP->setVisible(false);
    
    if(m_pLabelName != NULL)
        m_pLabelName->setVisible(false);
    
    if (m_pLabelLevel != NULL)
        m_pLabelLevel->setVisible(false);
    
    if (LogicInfo::getSingleton().isNormalMode()) {
        
        if (m_pProHP == NULL) {
            CCSprite* pFrameBg = ImgMgr::getSingleton().loadSprite("ui_img_pro_bk_1");
            CCSprite* pFrame = ImgMgr::getSingleton().loadSprite("ui_img_pro_org_3");
            
            m_pProHP = CProgress::create(pFrame, pFrameBg);
            m_pProHP->setAnchorPoint(ccp(0.5, 0));
            
            m_pProHP->retain();
            PlayerScene::getSingleton().m_pRoot->addChild(m_pProHP, SCENE_Z_BUILDINGUI);
        }
        
        m_pProHP->setVisible(true);
    }
    
    setLogicPos(getLogicX(), getLogicY());
}

void Building::showPreBuilding()
{
    if(m_pPreBuilding == NULL)
    {
        char str[128];
        if(m_info.width > 2)
        {
            sprintf(str, "prebuilding_%d", m_info.width / 2);
            
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
            if(pFrame != NULL)
            {
                m_pPreBuilding = CCSprite::create();
                m_pPreBuilding->initWithSpriteFrame(pFrame);
                m_pPreBuilding->setAnchorPoint(ccp(0.5, 0));
                m_pPreBuilding->retain();
                
                PlayerScene::getSingleton().m_pRoot->addChild(m_pPreBuilding, (getLogicX() + getLogicY()) * SCENE_Z_BUILDING_JUMP);
            }
            
            m_isPreBuilding = true;
            
            setLogicPos(getLogicX(), getLogicY());
        }
    }
}

void Building::onBuildingEnd()
{
    if(m_isPreBuilding)
    {
        if(m_pPreBuilding != NULL)
        {
            m_pPreBuilding->release();
            PlayerScene::getSingleton().m_pRoot->removeChild(m_pPreBuilding, true);
            m_pPreBuilding = NULL;
        }
        
        m_isPreBuilding = false;
    }
    
    if(PlayerScene::getSingleton().m_isAlreadyInit)
    {
        CCPoint pos = PlayerScene::getSingleton().countScenePos(getCenterPt().x, getCenterPt().y);
        
        PlayerScene::getSingleton().addBoomEx("building_levelup", 0, pos.x, pos.y);
    }
}

void Building::setBeAttackAni(CCSprite* spr, bool isMain, bool isCenter)
{
    float cs = spr->getScale();
    
    CCPoint pt1 = spr->getPosition();
    CCPoint pt2 = spr->getPosition();
    
    //if(!isCenter)
    //	pt1.x += spr->getContentSize().width * 0.05f / 2;
    
    pt1.y += m_info.width * TILE_H / 2 * 0.05f;
    
    CCMoveTo* mv1 = CCMoveTo::create(0.08f, pt1);
    CCMoveTo* mv2 = CCMoveTo::create(0.08f, pt2);
    
    CCScaleTo* sc1 = CCScaleTo::create(0.08f, cs * 0.95f);
    CCScaleTo* sc2 = CCScaleTo::create(0.08f, cs * 1.0f);
    
    CCFiniteTimeAction* action1 = CCSequence::create(sc1, sc2, sc1, sc2, sc1, sc2, NULL);
    CCFiniteTimeAction* action2 = CCSequence::create(mv1, mv2, mv1, mv2, mv1, mv2, NULL);
    
    CCAction* pAct = spr->runAction(action1);
    spr->runAction(action2);
    
    //if(isMain)
    {
        m_pActBeAttack = pAct;
        m_pActBeAttack->retain();
    }
}

int Building::countRes()
{
    if(m_info.producesres < 0)
        return 0;
    
    int64_t res = (int64_t)m_curResTime * m_info.producesperhour / (60 * 60);
    if(res > m_info.maxstored[m_info.producesres - 1])
        res = m_info.maxstored[m_info.producesres - 1];
    
    return res;
}

int Building::countRes_Sub()
{
    if(m_info.producesres < 0)
        return 0;
    
    int64_t res = (int64_t)m_curResTime * m_info.producesperhour / (60 * 60);
    if(res > m_info.maxstored[m_info.producesres - 1])
        res = m_info.maxstored[m_info.producesres - 1];
    
    int fullnum;
    
    if(m_info.producesres == MONEY_GOLD)
    {
        if (LogicInfo::getSingleton().m_myInfo.goldex >= LogicInfo::getSingleton().m_myInfo.maxgoldex)
        {
            fullnum = 0;
        }
        else
        {
            fullnum = LogicInfo::getSingleton().m_myInfo.maxgoldex - LogicInfo::getSingleton().m_myInfo.goldex;
        }
    }
    else if (m_info.producesres == MONEY_ELIXIR)
    {
        if (LogicInfo::getSingleton().m_myInfo.oilex >= LogicInfo::getSingleton().m_myInfo.maxoilex)
        {
            fullnum = 0;
        }
        else
        {
            fullnum = LogicInfo::getSingleton().m_myInfo.maxoilex - LogicInfo::getSingleton().m_myInfo.oilex;
        }
    }
    
    int asec;
    
    if(res > fullnum)
    {
        asec = (fullnum * (60 * 60)) / m_info.producesperhour;
        
        if((fullnum * (60 * 60)) % m_info.producesperhour != 0)
        {
            asec += 1;
        }
        
        res = asec * m_info.producesperhour / (60 * 60);
        
        if(m_info.producesres == MONEY_GOLD)
        {
            PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI->addNote(ViewString::getSingleton().getValue("gold_storages_full_tip").c_str());
        }
        else
        {
            PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI->addNote(ViewString::getSingleton().getValue("elixir_storages_full_tip").c_str());
        }
    }
    else
    {
        asec = m_curResTime;
    }
    
    return res;
}

void Building::onRefurbish()
{
}

void Building::shootGold(int gold)
{
    if(m_psGold == NULL)
    {
        m_psGold = ParticleSystem::create(PlayerScene::getSingleton().m_goldEmitter);
        PlayerScene::getSingleton().m_pRoot->addChild(m_psGold, SCENE_Z_PARTICLE);
        m_psGold->retain();
    }
    
    CCPoint pos = PlayerScene::getSingleton().countScenePos(getCenterPt().x, getCenterPt().y);
    m_psGold->setPosition(pos);
    
    m_psGold->emitter(gold);
}

void Building::shootOil(int oil)
{
    if(m_psOil == NULL)
    {
        m_psOil = ParticleSystem::create(PlayerScene::getSingleton().m_oilEmitter);
        PlayerScene::getSingleton().m_pRoot->addChild(m_psOil, SCENE_Z_PARTICLE);
        m_psOil->retain();
    }
    
    CCPoint pos = PlayerScene::getSingleton().countScenePos(getCenterPt().x, getCenterPt().y);
    m_psOil->setPosition(pos);
    
    m_psOil->emitter(oil);
}

void Building::setIcon(const char* str)
{
    if(str == NULL)
    {
        if(m_pIcon != NULL)
            m_pIcon->setVisible(false);
        
        return ;
    }
    
    if(m_pIcon == NULL)
    {
        m_pIcon = ImgMgr::getSingleton().loadSprite(str);
        m_pIcon->setAnchorPoint(ccp(0.5, 0));
        m_pIcon->retain();
        
        PlayerScene::getSingleton().m_pRoot->addChild(m_pIcon, SCENE_Z_BUILDINGUI);
        
        setLogicPos(getLogicX(), getLogicY());
    }
    else
    {
        m_pIcon->setVisible(true);
        
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pIcon->setDisplayFrame(pFrame);
    }
}

int Building::countAttackMode()
{
    int mode = 0;
    if(m_info.airtargets > 0)
    {
        if(m_info.groundtargets > 0)
            mode = 2;
        else
            mode = 1;
    }
    else
        mode = 0;
    
    return mode;
}

void Building::setAttackTargetAni()
{
    if(m_pAttackTarget == NULL)
    {
        m_pAttackTarget = AniObjMgr::getSingleton().newAniObj(PlayerScene::getSingleton().m_pParticleBatchNode, "attack_target", 0, false, false, -1, 0, false, NULL);
        
        CCPoint pt = PlayerScene::getSingleton().countScenePos(getCenterPt().x, getCenterPt().y);
        m_pAttackTarget->setPosition(ccp(pt.x + TILE_W / 2, pt.y));
    }
    
    if(m_pAttackTarget != NULL)
        m_pAttackTarget->start();
}

void Building::chgIconEx(const char* strIcon)
{
    if(LogicInfo::getSingleton().isAttackMode())
        return ;
    
    if (strIcon == NULL) {
        m_pIconEx->setVisible(false);
        return;
    }
    
    if (m_pIconEx) {
        PlayerScene::getSingleton().m_pRoot->removeChild(m_pIconEx, true);
        m_pIconEx->release();
        m_pIconEx = NULL;
    }
    
    if(m_pIconEx == NULL)
    {
        m_pIconEx = ImgMgr::getSingleton().loadSprite(strIcon);
        m_pIconEx->setAnchorPoint(ccp(0.5, 0));
        m_strIconEx = strIcon;
        
        PlayerScene::getSingleton().m_pRoot->addChild(m_pIconEx, SCENE_Z_BUILDINGUI);
        m_pIconEx->retain();
    }
    
    if(m_pIconEx != NULL)
    {
        m_pIconEx->setVisible(true);
        
        CCPoint pt = countSceneTilePt(getLogicX(), getLogicY());
        pt.y += getHeight();
        
        m_pIconEx->setPosition(pt);
    }
}

bool Building::isAttackBuilding()
{
    return m_type == BUILDING_TYPE_ATTACK || m_type == BUILDING_TYPE_TOWER || m_type == BUILDING_TYPE_CANNON
    || m_type == BUILDING_TYPE_MORTAR || m_type == BUILDING_TYPE_AIR_DEFENSE;
}

bool Building::isResourceBuilding()
{
    int bid = m_buildingid / 100;
    
    return bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_ELIXIR_STORAGE || bid == BUILDING_GOLD_MINE || bid == BUILDING_GOLD_STORAGE;
}

NS_COC_END
