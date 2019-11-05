//
//  EnvMapUI.cpp
//  GameClient
//
//  Created by qsun on 14-3-20.
//
//

#include "EnvMapUI.h"
#include "FindMatchUI.h"
#include "CDataStream.h"
#include "CFileStream.h"
#include "../coc/ImgMgr.h"
#include "../coc/EnvMapMgr.h"
#include "../coc/LogicInfo.h"
#include "../coc/PlayerScene.h"
#include "../coc/SoundMgr.h"
#include "../GameWorldScene.h"

USING_NS_COC;

EnvMapUI::EnvMapUI()
{
    
}

EnvMapUI::~EnvMapUI()
{
    m_pStage->removeAllChildren();
    m_pStage->release();
    m_pPoint->release();
    m_pMapBgItem->release();
    
    m_pCurLevel->release();
}

bool EnvMapUI::init()
{
    if (CUINode::init()) {
        CCNode* pMapNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("envmap_bg.ccbi", this);
        this->setContentSize(pMapNode->getContentSize());
        this->setAnchorPoint(ccp(0, 0));
        this->addChild(pMapNode, -1);
        
        m_pMapBgItem = CUINodeCache::sharedUINodeCache()->UINodeFromFile("envmap_item.ccbi", this);
        m_pMapBgItem->retain();
        
        for (int i = 0; i < 6; i++) {
            m_mapBgItemPos[i] = ccp(((int)m_pMapBgItem->getContentSize().width)*i, 0);
        }
        
        {
            m_pStage = ImgMgr::getSingleton().loadSprite("map_level_0");
            m_pStage->retain();
            
            m_pStageStar = CCNode::create();
            for (int i = 0; i < 3; i++) {
                CCSprite* pStar = ImgMgr::getSingleton().loadSprite("star_s_0");
                pStar->setPosition(ccp(pStar->getContentSize().width*(i-1), 0));
                pStar->setRotation(30*(1-i));
                pStar->setTag(i);
                m_pStageStar->addChild(pStar);
            }
            m_pStageStar->setPosition(ccp(m_pStage->getContentSize().width/2, m_pStage->getContentSize().height+10));
            m_pStageStar->setScale(1.0*getResolutionScale());
            m_pStageStar->setTag(1);
            m_pStage->addChild(m_pStageStar);
            
            m_pStageName = CCLabelBMFont::create("Test", "Font/tbFont_b.fnt", -1, kCCTextAlignmentCenter);
            m_pStageName->setPosition(ccp(m_pStage->getContentSize().width/2, m_pStage->getContentSize().height+30));
            m_pStageName->setScale(0.3*getResolutionScale());
            m_pStageName->setTag(2);
            m_pStage->addChild(m_pStageName);
        
            m_pPoint = ImgMgr::getSingleton().loadSprite("map_route_1");
            m_pPoint->retain();
            m_pPoint->setScale(1.0*getResolutionScale());
            
            pMapNode->removeChild(m_pBtnAttack, false);
            this->addChild(m_pBtnAttack);
            
            pMapNode->removeChild(m_pBtnVideo, false);
            this->addChild(m_pBtnVideo);
            
            m_pCurLevel->retain();
            pMapNode->removeChild(m_pCurLevel);
        }
        
        m_pBtnAttack->setVisible(false);
        m_pBtnVideo->setVisible(false);
        m_pCurLevel->setVisible(false);
      
        this->setTouchEnabled(true);
        
        m_curStage.stage = -1;
                
        loadMap();
     }
    
    return true;
}

void EnvMapUI::saveMap()
{
    string fileName = CCFileUtils::sharedFileUtils()->getWritablePath() + "map1.bin";
    
    vector<EnvMapNode> lstMapNodes;
    
    CCNode* pMapView = this->getChildByTag(999);
    CCNode* pPointNode = pMapView->getChildByTag(999);
    
    int count = pPointNode->getChildrenCount();
    for (int j = 0; j < count; j++) {
        
        CCNode* pNode = (CCNode*)pPointNode->getChildren()->objectAtIndex(j);
        CCPoint pos = pNode->getPosition();
        
        EnvMapNode mapNode;
        mapNode.type = 1;
        mapNode.stage = -1;
        mapNode.pos = pos;
        mapNode.flag = 0;
        lstMapNodes.push_back(mapNode);
    }
    
    count = EnvMapMgr::getSingleton().getEnvStageNums();
    for (int index = 0; index < count; index++) {
        
        int stage = EnvMapMgr::getSingleton().getEnvStageId(index);
        CCControlButton* pStage = (CCControlButton*)pMapView->getChildByTag(stage);
        
        EnvMapNode mapNode;
        
        mapNode.type = 0;
        mapNode.stage = stage;
        mapNode.pos = pStage->getPosition();
        mapNode.flag = 0;
        if (stage >= HARD_STAGE) {
            mapNode.flag = 1;
        }
        
        lstMapNodes.push_back(mapNode);
    }
    
    fstream pFile;
    pFile.open(fileName.c_str(), ios::out|ios::binary);
    
    if(pFile.is_open()){
        
        CFileOutputStream fos(pFile);
        
        int size = lstMapNodes.size();
        fos.writeInt(size);
        for (int i = 0; i < lstMapNodes.size(); i++) {
            EnvMapNode mapNode = lstMapNodes[i];
            
            fos.writeInt(mapNode.type);
            fos.writeInt(mapNode.stage);
            fos.writeInt(mapNode.pos.x);
            fos.writeInt(mapNode.pos.y);
            fos.writeInt(mapNode.flag);
            //            char data[sizeof(mapNode)];
            //            memcpy(&mapNode, data, sizeof(mapNode));
            //            pFile.write(data, sizeof(mapNode));
        }
        
        pFile.flush();
        pFile.close();
    }
}

void EnvMapUI::loadMap()
{
    unsigned long size;
//    string fileName = CCFileUtils::sharedFileUtils()->getWritablePath() + "map.bin";
//    char* pBuff = (char*)CCFileUtils::sharedFileUtils()->getFileData(fileName.c_str(), "rb", &size);
    
    char* pBuff = (char*)CCFileUtils::sharedFileUtils()->getFileData("map1.bin", "rb", &size);
    
    CDataStream ds(pBuff);
    int count = ds.readInt();
    for (int i = 0; i < count; i++) {
        EnvMapNode mapNode;
        mapNode.type = ds.readInt();
        mapNode.stage = ds.readInt();
        mapNode.pos = ccp(ds.readInt()*getResolutionScale(), ds.readInt()*getResolutionScale());
        mapNode.flag = ds.readInt();
        
        if (mapNode.stage >= 0) {
            m_stageNodeVec.push_back(mapNode);
        }
        
        m_mapNodeVec.push_back(mapNode);
    }
    
    delete[] pBuff;
}

void EnvMapUI::onEnter()
{
    CUINode::onEnter();
    
    m_pView = (CCScrollView*)getParent();
    m_viewPos = m_pView->convertToWorldSpace(CCPointZero);
    m_viewRect = getViewRect();
}

void EnvMapUI::onExit()
{
    CUINode::onExit();
}

bool EnvMapUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    return true;
}

void EnvMapUI::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

void EnvMapUI::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    m_curStage.stage = -1;
    
    CCPoint offset = m_pView->getContentOffset();
    int count = m_stageNodeVec.size();
    for (int stage = 0; stage < count; stage++) {
        EnvMapNode mapNode = m_stageNodeVec[stage];
        
        CCRect rect = CCRectMake(mapNode.pos.x, mapNode.pos.y, 80, 80);
        rect.origin.x += (offset.x - rect.size.width/2);
        rect.origin.y += (offset.y - rect.size.height/2);
        
        if (isTouchInside(pTouch, rect)) {
            m_curStage = mapNode;
            break;
        }
    }
    
    onSelectStage(m_curStage);
}

CCRect EnvMapUI::getViewRect()
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

bool EnvMapUI::isInViewRect(CCNode* pNode)
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

void EnvMapUI::update(float dt)
{

}

void EnvMapUI::draw()
{
    CCLayer::draw();
    
    for (int i = 0; i < 6; i++) {
        m_pMapBgItem->setPosition(m_mapBgItemPos[i]);
        if (isInViewRect(m_pMapBgItem)) {
            m_pMapBgItem->visit();
        }
    }
    
    CCSprite* pCurLevel = (CCSprite*)m_pCurLevel->getChildByTag(1);
    pCurLevel->setRotation(pCurLevel->getRotation()+3);
    m_pCurLevel->visit();
    
    int size = m_mapNodeVec.size();
    for (int i = 0; i < size; i++) {
        
        EnvMapNode mapNode = m_mapNodeVec[i];
        
        if (mapNode.type == 0) {
            m_pStage->setPosition(mapNode.pos);
            if (isInViewRect(m_pStage)) {
                
                drawStage(mapNode);
            }
        }
        else if (mapNode.type == 1) {
            m_pPoint->setPosition(mapNode.pos);
            if (isInViewRect(m_pPoint)) {
                m_pPoint->visit();
            }
        }
    }
}

void EnvMapUI::drawStage(EnvMapNode mapNode)
{
    EnvStageInfo* stageInfo = EnvMapMgr::getSingleton().getEnvStageInfo(mapNode.stage);
    
    m_pStageName->setString(stageInfo->name.c_str());
    
    if (mapNode.stage <= m_openStage || (mapNode.stage >= HARD_STAGE && m_openStage / SMALL_STAGE_NUMS >= mapNode.stage - HARD_STAGE)) //open stage
    {
        if (mapNode.stage < HARD_STAGE) {
            m_pStage->setDisplayFrame(ImgMgr::getSingleton().getFrame("map_level_1"));
        }
        else{
            m_pStage->setDisplayFrame(ImgMgr::getSingleton().getFrame("map_level_3"));
        }
    }
    else {
        if (mapNode.stage < HARD_STAGE) {
            m_pStage->setDisplayFrame(ImgMgr::getSingleton().getFrame("map_level_0"));
        }
        else{
            m_pStage->setDisplayFrame(ImgMgr::getSingleton().getFrame("map_level_2"));
        }
    }
    
    for (int i = 0; i < 3; i++) {
        if (stageInfo->star <= i) {
            ((CCSprite*)m_pStageStar->getChildByTag(i))->setDisplayFrame(ImgMgr::getSingleton().getFrame("star_s_0"));
        }
        else{
            ((CCSprite*)m_pStageStar->getChildByTag(i))->setDisplayFrame(ImgMgr::getSingleton().getFrame("star_s_1"));
        }
    }
    
    m_pStage->visit();
}

void EnvMapUI::viewDidScroll()
{
    
}

void EnvMapUI::onSelectStage(EnvMapNode stageNode)
{
    m_curStage = stageNode;
    
    if (stageNode.stage >= 0) {
        if (stageNode.stage <= m_openStage || (stageNode.stage >= HARD_STAGE && m_openStage / SMALL_STAGE_NUMS >= stageNode.stage - HARD_STAGE)) { //open stage
            m_pBtnAttack->setVisible(true);
            m_pBtnVideo->setVisible(true);
            
            m_pBtnAttack->setEnabled(true);
        }
        else
        {
            m_pBtnAttack->setVisible(false);
            m_pBtnVideo->setVisible(false);
        }
        m_pCurLevel->setVisible(true);

        SoundMgr::getSingleton().playEffect("btn.m4a", 0);
    }
    else {
        m_pBtnAttack->setVisible(false);
        m_pBtnVideo->setVisible(false);
        
        m_pBtnAttack->setEnabled(false);
    }
    
    m_pBtnVideo->setVisible(false);
    
    int offY = -70;
    m_pBtnAttack->setPosition(m_curStage.pos.x, m_curStage.pos.y + offY);
//    m_pBtnAttack->setPosition(m_curStage.pos.x - m_pBtnAttack->getContentSize().width/2 - 5, m_curStage.pos.y + offY);
//    m_pBtnVideo->setPosition(m_curStage.pos.x + m_pBtnAttack->getContentSize().width/2 + 5, m_curStage.pos.y + offY);
    m_pCurLevel->setPosition(m_curStage.pos);
    
    LogicInfo::getSingleton().m_pveInfo.curstage = stageNode.stage;
    PlayerScene::getSingleton().m_pGameWorld->m_pFindMatchUI->onSelectStage(stageNode.stage);
}

void EnvMapUI::menuAttackCallback(CCObject* pSender, CCControlEvent event)
{
    if (LogicInfo::getSingleton().m_pveInfo.curlife <= 0) {
        PlayerScene::getSingleton().m_pGameWorld->m_pFindMatchUI->addFullLife();
        return;
    }
    
    LogicInfo::getSingleton().refurbishFightSoldierInfo();
    if(LogicInfo::getSingleton().m_lstFightSoldier.size() == 0)
    {
        MessageUI::show(MSG_NO_SOLDIER, "", ViewString::getSingleton().getValue("no_soldier").c_str());
        return;
    }
    
    LogicInfo::getSingleton().attackEnv(m_curStage.stage);
    
    PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_FINDMATCH);
    PlayerScene::getSingleton().m_pGameWorld->hideFindMatchUI();
    
    if (GuideMgr::getSingleton().isGuideEnd()) {
        PlayerScene::getSingleton().m_backHomeView = BACK_HOME_PVE_MAP;
    }
}

void EnvMapUI::menuVideoCallback(CCObject* pSender, CCControlEvent event)
{
    EnvStageInfo* info = EnvMapMgr::getSingleton().getEnvStageInfo(m_curStage.stage);
    
    strcpy(LogicInfo::getSingleton().m_infoFightScore.user[0].name, LogicInfo::getSingleton().m_myInfo.uname.c_str());
    strcpy(LogicInfo::getSingleton().m_infoFightScore.user[1].name, info->name.c_str());
    
    LogicInfo::getSingleton().m_infoFightScore.user[1].gold = info->basegold;
    LogicInfo::getSingleton().m_infoFightScore.user[1].oil = info->baseoil;
    LogicInfo::getSingleton().m_otherInfo.uname = info->name;
    
    HttpClient::getSingleton().requestAttackEnvVideo(m_curStage.stage);
    
    PlayerScene::getSingleton().m_pGameWorld->hideFindMatchUI();
    PlayerScene::getSingleton().m_pGameWorld->hideMainUI();
    
    LogicInfo::getSingleton().chg2Wait();
    
    PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_NORMAL);
    
    PlayerScene::getSingleton().m_backHomeView = BACK_HOME_PVE_MAP;
}

bool EnvMapUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CurLevel", CCNode*, m_pCurLevel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnAttack", CCControlButton*, m_pBtnAttack);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnVideo", CCControlButton*, m_pBtnVideo);

    return false;
}

SEL_CCControlHandler EnvMapUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuAttackCallback", EnvMapUI::menuAttackCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuVideoCallback", EnvMapUI::menuVideoCallback);
    
    return NULL;
}