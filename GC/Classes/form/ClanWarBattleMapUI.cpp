//
//  ClanWarBattleMapUI.cpp
//  GameClient
//
//  Created by qsun on 14-9-11.
//
//

#include "ClanWarBattleMapUI.h"
#include "CDataStream.h"
#include "CFileStream.h"
#include "../coc/ClanWarMgr.h"
#include "../coc/ImgMgr.h"
#include "../coc/PlayerScene.h"
#include "../coc/SoundMgr.h"
#include "../GameWorldScene.h"

int BOTTOM_EDGE_HEIGHT = 0;

ClanWarBattleMapUI::ClanWarBattleMapUI()
: m_pAttackTarget(NULL)
, m_mapNodeNums(0)
{
    
}

ClanWarBattleMapUI::~ClanWarBattleMapUI()
{
    for (int i = 0; i < m_mapNodeVec.size(); i ++) {
        m_mapNodeVec[i]->release();
    }
    
    m_mapNodeVec.clear();
}

bool ClanWarBattleMapUI::init()
{
    if (CUINode::init()) {
        
        this->setTouchEnabled(true);
        
//        saveMap();
        loadMap();
    }
    
    return true;
}

void ClanWarBattleMapUI::refreshMap(int warCityType)
{
    m_warCityType = warCityType;
    
    for (int i = 0; i < m_mapNodeVec.size(); i ++) {
        m_mapNodeVec[i]->release();
    }
    m_mapNodeVec.clear();
    
    this->removeAllChildren();
    
    m_pView = (CCScrollView*)getParent();
    
    if (m_warCityType == 1) {
        m_warDefenseUserList = ClanWarMgr::getSingleton().m_warDefenseUserList;
        m_selectIndex = ClanWarMgr::getSingleton().m_selectDefendIndex;
        
        BOTTOM_EDGE_HEIGHT = PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->m_pClanWarBattleUI->m_pLootInfoNode->getContentSize().height;
    }
    else if (m_warCityType == 2) {
        m_warDefenseUserList = GroupWarMgr::getSingleton().m_warDefenseUserList;
        m_selectIndex = GroupWarMgr::getSingleton().m_selectDefendIndex;
        
        BOTTOM_EDGE_HEIGHT = PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->m_pGroupWarBattleUI->m_pLootInfoNode->getContentSize().height;
    }
    
    m_mapNodeSize = CCSizeMake(100*getResolutionScale(), 160*getResolutionScale());
    
    m_mapNodeNums = m_warDefenseUserList.size();
    
    this->setContentSize(CCSizeMake(m_pView->getViewSize().width, m_mapNodePosVec[m_mapNodeNums-1].y + BOTTOM_EDGE_HEIGHT + m_mapNodeSize.height/2));
    
    if (this->getContentSize().height < m_pView->getViewSize().height) {
        this->setContentSize(m_pView->getViewSize());
    }
    
    m_pView->setContentSize(getContentSize());
    m_pView->setContentOffset(ccp(0, m_pView->getViewSize().height - getContentSize().height));
    
    int mapPageNodeNums = 10;
    int mapPageNums = (m_mapNodeNums + mapPageNodeNums - 1) / mapPageNodeNums;
    
    int index = 0;
    for (index = 0; index < mapPageNums + 1; index++) {
        
        CCNode* pMapBgItem = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_battle_mapbg_item.ccbi", this);
        pMapBgItem->setZOrder(-1);
        pMapBgItem->setPositionY(index * pMapBgItem->getContentSize().height);
        this->addChild(pMapBgItem);
    }
    
    m_pAttackTarget = ImgMgr::getSingleton().loadSprite("clanwar_attack_0");
    m_pAttackTarget->setZOrder(-1);
    m_pAttackTarget->setVisible(false);
    CCAction* pAction = actionWithAnimate("clanwar_attack_", 6, 0.2, true);
    m_pAttackTarget->runAction(pAction);
    this->addChild(m_pAttackTarget);
    
    for (int i = 0; i < m_mapNodeNums; i++) {
        
        ClanWarUserInfo userInfo = m_warDefenseUserList[i];
        
        ClanWarBattleMapNode* pMapNode = ClanWarBattleMapNode::create(i, m_warCityType, userInfo);
        pMapNode->retain();
        
        CCPoint pos = m_mapNodePosVec[i];
        pos.x = pos.x - pMapNode->getContentSize().width/2;
        pos.y = getContentSize().height - pos.y - pMapNode->getContentSize().height/2;
        
        pMapNode->setPosition(pos);
        m_mapNodeVec.push_back(pMapNode);
        
        m_mapNodeSize = pMapNode->getContentSize();
    }
    
    onSelectStage(m_selectIndex);
}

void ClanWarBattleMapUI::onEnter()
{
    CUINode::onEnter();
    
    m_pView = (CCScrollView*)getParent();
    m_viewPos = m_pView->convertToWorldSpace(CCPointZero);
    m_viewRect = getViewRect();
}

void ClanWarBattleMapUI::onExit()
{
    CUINode::onExit();
}

void ClanWarBattleMapUI::saveMap()
{
    CCNode* pMapView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_battle_map.ccbi", this);
    this->addChild(pMapView);
    
    string fileName = CCFileUtils::sharedFileUtils()->getWritablePath() + "map2.bin";
    
    vector<CCPoint> lstMapNodes;
    
    int nodeCount = 50;
    for (int j = 0; j < nodeCount; j++) {
        
        CCNode* pNode = (CCNode*)pMapView->getChildByTag(j);
        CCPoint pos = pNode->getPosition();
        
        pos.y = pMapView->getContentSize().height - pos.y;
        
        lstMapNodes.push_back(pos);
    }
    
    fstream pFile;
    pFile.open(fileName.c_str(), ios::out|ios::binary);
    
    if(pFile.is_open()){
        
        CFileOutputStream fos(pFile);
        
        int size = lstMapNodes.size();
        fos.writeInt(size);
        for (int i = 0; i < lstMapNodes.size(); i++) {
            CCPoint mapNode = lstMapNodes[i];
            
            fos.writeInt(mapNode.x);
            fos.writeInt(mapNode.y);
        }
        
        pFile.flush();
        pFile.close();
    }
}

void ClanWarBattleMapUI::loadMap()
{
    unsigned long size;
//    string fileName = CCFileUtils::sharedFileUtils()->getWritablePath() + "map2.bin";
//    char* pBuff = (char*)CCFileUtils::sharedFileUtils()->getFileData(fileName.c_str(), "rb", &size);
    
    char* pBuff = (char*)CCFileUtils::sharedFileUtils()->getFileData("map2.bin", "rb", &size);
    
    CDataStream ds(pBuff);
    int count = ds.readInt();
    for (int i = 0; i < count; i++) {
        
        CCPoint pos = ccp(ds.readInt(), ds.readInt());
        m_mapNodePosVec.push_back(pos);
    }
    
    delete[] pBuff;
}

bool ClanWarBattleMapUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    m_dragOffset = m_pView->getContentOffset();
    return true;
}

void ClanWarBattleMapUI::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{

}

void ClanWarBattleMapUI::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    CCPoint touchLocation = pTouch->getLocation(); // Get the touch position
    touchLocation = m_pView->getParent()->convertToNodeSpace(touchLocation);
    
    if (m_pView->getContentOffset().x != m_dragOffset.x || m_pView->getContentOffset().y != m_dragOffset.y) {
        return;
    }
    
    if (touchLocation.y <= BOTTOM_EDGE_HEIGHT){
        return;
    }
    
    CCPoint offset = m_pView->getContentOffset();
    int count = m_mapNodeVec.size();
    int selectIndex = m_selectIndex;
    
    for (int i = 0; i < count; i ++) {
        
        CCPoint pos = m_mapNodePosVec[i];
        pos.y = getContentSize().height - pos.y;
        
        CCRect rect = CCRectMake(pos.x, pos.y, m_mapNodeSize.width, m_mapNodeSize.height);
        rect.origin.x += (offset.x - rect.size.width/2);
        rect.origin.y += (offset.y - rect.size.height/2);
        
        if (isTouchInside(pTouch, rect)) {
            selectIndex = i;
            break;
        }
    }
    
    if (selectIndex != m_selectIndex) {
        
        m_selectIndex = selectIndex;
        
        onSelectStage(selectIndex);
        SoundMgr::getSingleton().playEffect("btn.m4a", 0);
    }
}

void ClanWarBattleMapUI::onSelectStage(int index)
{
    m_selectIndex = index;
    
    ClanWarMgr::getSingleton().m_selectDefendIndex = index;
    GroupWarMgr::getSingleton().m_selectDefendIndex = index;
    
    if (m_warCityType == 1) {
        PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->m_pClanWarBattleUI->selectStage(index);
    }
    else if (m_warCityType == 2) {
        PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->m_pGroupWarBattleUI->selectStage(index);
    }
    
    if (m_mapNodeVec.size() > 0) {
        
        CCPoint pos = m_mapNodeVec[index]->getPosition();
        m_pAttackTarget->setPosition(ccp(pos.x+m_mapNodeSize.width/2, pos.y+m_mapNodeSize.height/2-20));
        m_pAttackTarget->setVisible(true);
        
        float posy1 = m_mapNodeVec[0]->getPositionY() - (m_pView->getContentSize().height - m_pView->getViewSize().height);
        float posy2 = m_mapNodeVec[m_mapNodeNums-1]->getPositionY();
        
        if (m_pView->getContentOffset().y + m_mapNodeVec[index]->getPositionY() > posy1) {
            
            float offy = posy1 - (m_pView->getContentOffset().y + m_mapNodeVec[index]->getPositionY());
            
            m_pView->setContentOffset(ccp(m_pView->getContentOffset().x, m_pView->getContentOffset().y + offy));
        }
        else if (m_pView->getContentOffset().y + m_mapNodeVec[index]->getPositionY() < posy2) {
            
            float offy = posy2 - (m_pView->getContentOffset().y + m_mapNodeVec[index]->getPositionY());
            
            m_pView->setContentOffset(ccp(m_pView->getContentOffset().x, m_pView->getContentOffset().y + offy));
        }
    }
}

CCRect ClanWarBattleMapUI::getViewRect()
{
    CCPoint screenPos = m_pView->convertToWorldSpace(CCPointZero);
    CCSize viewSize = m_pView->getViewSize();
    
    float scaleX = this->getScaleX();
    float scaleY = this->getScaleY();
    
    for (CCNode *p = m_pParent; p != NULL; p = p->getParent()) {
        scaleX *= p->getScaleX();
        scaleY *= p->getScaleY();
    }
    
    // Support negative scaling. Not doing so causes intersectsRect calls
    // (eg: to check if the touch was within the bounds) to return false.
    // Note, CCNode::getScale will assert if X and Y scales are different.
    if(scaleX<0.f) {
        screenPos.x += viewSize.width*scaleX;
        scaleX = -scaleX;
    }
    if(scaleY<0.f) {
        screenPos.y += viewSize.height*scaleY;
        scaleY = -scaleY;
    }
    
    return CCRectMake(screenPos.x, screenPos.y, viewSize.width*scaleX, viewSize.height*scaleY);
}

bool ClanWarBattleMapUI::isInViewRect(CCNode* pNode)
{
    CCPoint offset = m_pView->getContentOffset();
    
    CCRect rect = pNode->boundingBox();
    rect.origin.x += (offset.x + m_viewPos.x);
    rect.origin.y += (offset.y + m_viewPos.y);
    if (m_viewRect.intersectsRect(rect)) {
        return true;
    }
    
    return false;
}

void ClanWarBattleMapUI::draw()
{
    CCLayer::draw();
    
    int size = m_mapNodeVec.size();
    for (int i = 0; i < size; i ++) {
        
        if (isInViewRect(m_mapNodeVec[i])) {
            m_mapNodeVec[i]->visit();
        }
    }
}

bool ClanWarBattleMapUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    return false;
}

SEL_CCControlHandler ClanWarBattleMapUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    
    return NULL;
}


ClanWarBattleMapNode::ClanWarBattleMapNode(int index, int type, ClanWarUserInfo userInfo)
: m_index (index)
, m_type(type)
, m_userInfo (userInfo)
{
    
}

ClanWarBattleMapNode::~ClanWarBattleMapNode()
{
    this->removeAllChildren();
}

ClanWarBattleMapNode* ClanWarBattleMapNode::create(int index, int type, ClanWarUserInfo userInfo)
{
    ClanWarBattleMapNode *pRet = new ClanWarBattleMapNode(index, type, userInfo);
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool ClanWarBattleMapNode::init()
{
    if (CUINode::init()) {
        
        CCNode* pMapNode = NULL;
        if (m_type == 1) {
            pMapNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_battle_map_item.ccbi", this);
        }
        else if (m_type == 2) {
            pMapNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("groupwar_battle_map_item.ccbi", this);
        }
        
        this->setContentSize(pMapNode->getContentSize());
        this->addChild(pMapNode);
        
        refreshInfo();
    }
    
    return true;
}

void ClanWarBattleMapNode::refreshInfo()
{
    char str[128];
    sprintf(str, "%d", (m_index+1));
    m_pRankingId->setString(str);
    
    ClanWarUserInfo warUserInfo = m_userInfo;
    
    m_pPlayerName->setString(warUserInfo.uname.c_str());
    
    if (m_type == 1) {
        
        if (warUserInfo.destroyStar == 0) {
            sprintf(str, "townhall_destroy_%02d1", warUserInfo.baselevel-1);
        }
        else if (warUserInfo.destroyStar < 3) {
            sprintf(str, "townhall_destroy_%02d2", warUserInfo.baselevel-1);
        }
        else {
            sprintf(str, "townhall_destroy_%02d3", warUserInfo.baselevel-1);
        }
        
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pTownHall->setDisplayFrame(pFrame);
        
        for (int i = 1; i <= 3; i ++) {
            
            CCSpriteFrame* spriteFrame = NULL;
            if (warUserInfo.destroyStar < i) {
                spriteFrame = ImgMgr::getSingleton().getFrame("star_b_0");
            }
            else {
                spriteFrame = ImgMgr::getSingleton().getFrame("star_b_1");
            }
            
            ((CCSprite*)m_pDestroyStar->getChildByTag(i))->setDisplayFrame(spriteFrame);
        }
    }
    else if (m_type == 2) {
        
        sprintf(str, "townhall_destroy_%02d1", warUserInfo.baselevel-1);
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pTownHall->setDisplayFrame(pFrame);
        
        float percent = (float)warUserInfo.destroyStar/warUserInfo.levelupStar;
        changeProgress(m_pDestroyStarProgress, percent);
        
        sprintf(str, "%d/%d", warUserInfo.destroyStar, warUserInfo.levelupStar);
        m_pDestroyStarNums->setString(str);
        
        sprintf(str, "Level %d", warUserInfo.defenseLevel);
        m_pDefenseLevel->setString(str);
    }
}

bool ClanWarBattleMapNode::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankingId", CCLabelBMFont*, m_pRankingId);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerName", CCLabelTTF*, m_pPlayerName);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TownHall", CCSprite*, m_pTownHall);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DestroyStar", CCSprite*, m_pDestroyStar);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DestroyStarProgress", CCSprite*, m_pDestroyStarProgress);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DestroyStarNums", CCLabelBMFont*, m_pDestroyStarNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DefenseLevel", CCLabelBMFont*, m_pDefenseLevel);
    
    return false;
}

SEL_CCControlHandler ClanWarBattleMapNode::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    return NULL;
}

