//
//  GuideMgr.cpp
//  GameClient
//
//  Created by chowray on 13-6-6.
//
//

#include "GuideMgr.h"
#include "ImgMgr.h"
#include "PersonMgr.h"
#include "LogicInfo.h"
#include "NGNKit.h"
#include "HeroMgr.h"
#include "PlayerScene.h"
#include "GameWorldScene.h"
#include "Building.h"

NS_COC_BEGIN

GuideMgr::GuideMgr()
: m_pArrow(NULL)
, m_pFinger(NULL)
, m_pActiveBtn(NULL)
, m_pGuideUI(NULL)
, val2(0)
, m_isInitOk(false)
{
    load("guide.csv");
}

GuideMgr::~GuideMgr()
{

}

GuideMgr& GuideMgr::getSingleton()
{
    static GuideMgr s_mgr;
    return s_mgr;
}

bool GuideMgr::init()
{
    val1 = LogicInfo::getSingleton().m_zyval;
    
    m_isGuideEnd = false;
    m_pActiveBtn = NULL;
    m_pActiveBuilding = NULL;
    m_pMessageUI = NULL;
    
    if(val1 == 999)
    {
        m_isGuideEnd = true;
    }
    
    if (!isGuideEnd()) {
        
        this->setTouchEnabled(true);
        
        ImgMgr::getSingleton().addSpriteFrames("game_npc");
        ImgMgr::getSingleton().loadImg("ui_attacked_npc");
        
        ImgMgr::getSingleton().addSpriteFrames("person10701");
        
        m_pArrow = CUINodeCache::sharedUINodeCache()->UINodeFromFile("arrow.ccbi", this, &m_pArrowAnimation);
        m_pArrow->retain();
        
        scaleY = m_pArrow->getScaleY();
        
        setMainUIButton(PlayerScene::getSingleton().m_pGameWorld->m_pMainUI, false);
        
        setVal(val1);
    }
    else {
        PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->showBtnTip();
    }
    
    m_isInitOk = true;
    
    return true;
}

void GuideMgr::setVal(int val)
{
    val1 = val;
    
    if(val1 == 999)
    {
        m_isGuideEnd = true;
    }
    
    if (!isGuideEnd()) {
        
        check(val1, true);
        refresh(val1, val2);
    }
}

void GuideMgr::onLoadEnd()
{
	for(int i = 1; i < getHeight(); ++i)
	{
        pair<string, GuideItemInfo> _pair;
        
		_pair.first = get("id", i);
        _pair.second.file = get("filename", i);
		_pair.second.animIn = get("animIn", i);
        _pair.second.animOut = get("animOut", i);
        _pair.second.tip = get("tip", i);
        
		m_map.insert(_pair);
	}
}

bool GuideMgr::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    if (PlayerScene::getSingleton().m_pGameWorld == NULL) {
        return false;
    }
    
    if (m_btnLst.size() == 0 || PlayerScene::getSingleton().m_pGameWorld->m_pChgScene != NULL) {
        return false;
    }
    
    MainUI* pMainUI = PlayerScene::getSingleton().m_pGameWorld->m_pMainUI;
    BuildUI* pBuildUI = PlayerScene::getSingleton().m_pGameWorld->m_pBuildUI;
    
    CCPoint touchLocation = pTouch->getLocation(); // Get the touch position
    CCPoint touchPos;
    CCRect bBox;
    
    if (m_pMessageUI) {
        
        touchPos = m_pMessageUI->getParent()->convertToNodeSpace(touchLocation);
        bBox = m_pMessageUI->boundingBox();
        if (bBox.containsPoint(touchPos)) {
            return false;
        }
    }
    
    bool bSwallowsTouches = true;
    bool touchEnable = true;
    
    if (LogicInfo::getSingleton().isNormalMode()) {
        if (m_pActiveBtn != NULL || m_pActiveBuilding != NULL) {
            
            for (int index = 0; index < m_btnLst.size(); index++) { //main ui button
                
                touchPos = m_btnLst[index]->getParent()->convertToNodeSpace(touchLocation);
                bBox = m_btnLst[index]->boundingBox();
                
                if (m_pActiveBtn == m_btnLst[index]) {
                    bSwallowsTouches = false;
                }
                else if (bBox.containsPoint(touchPos))
                {
                    touchEnable = false;
                }
            }
            
            for (int index = 0; index < pMainUI->m_lstBuildingBtn.size(); index++) { //build menu button
                touchPos = pMainUI->m_lstBuildingBtn[index]->getParent()->convertToNodeSpace(touchLocation);
                CCRect bBox = pMainUI->m_lstBuildingBtn[index]->boundingBox();
                
                if (m_pActiveBtn == pMainUI->m_lstBuildingBtn[index]) {
                    //                bSwallowsTouches = false;
                }
                else if (bBox.containsPoint(touchPos))
                {
                    touchEnable = false;
                }
            }
            
            touchPos = pBuildUI->m_pBtnOK->getParent()->convertToNodeSpace(touchLocation);
            CCRect bBox = pBuildUI->m_pBtnOK->boundingBox();
            
            if (m_pActiveBtn == pBuildUI->m_pBtnOK){
                bSwallowsTouches = false;
            }
            else if (bBox.containsPoint(touchPos))
            {
                touchEnable = false;
            }
            
            touchPos = pBuildUI->m_pBtnCancel->getParent()->convertToNodeSpace(touchLocation);
            bBox = pBuildUI->m_pBtnCancel->boundingBox();
            if (m_pActiveBtn == pBuildUI->m_pBtnCancel){
                bSwallowsTouches = false;
            }
            else if (bBox.containsPoint(touchPos))
            {
                touchEnable = false;
            }
        }
    }else{
        if (m_pActiveBtn != NULL) {
            touchEnable = false;
        }
    }
    
    if (m_pActiveBtn != NULL) {
        
        if (!bSwallowsTouches && touchEnable) {
            return false;
        }
        
        touchPos = m_pActiveBtn->getParent()->convertToNodeSpace(touchLocation);
        bBox = m_pActiveBtn->boundingBox();
        
        if (bBox.containsPoint(touchPos)) {
            return false;
        }
    }
    else {
        if (touchEnable) {
            return false;
        }
    }
    
    return true;
}

void GuideMgr::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
}

void GuideMgr::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
}

void GuideMgr::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
}

void GuideMgr::registerWithTouchDispatcher()
{
//    int touchPriority = PlayerScene::getSingleton().m_pGameWorld->getTouchPriority();
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -999999, true);
}

void GuideMgr::check(int i1, bool isInit)
{
    if((isInit || val1 == GUIDE_BUILD_CANNON))  //BUILDING_CANNON
    {
        if (PlayerScene::getSingleton().findBuilding(10101) != NULL) {
            
            if (val2 == 0) {
                Building* build = PlayerScene::getSingleton().findBuilding(10101);
                if(build->m_pBuildingInfo->state == BUILDING_STATE_NORMAL)
                {
                    val1 = GUIDE_DEFEND_TOWNHALL;
                    val2 = 0;
                }
                else
                {
                    val1 = GUIDE_BUILD_CANNON;
                    val2 = 60;
                }
            }
        }
        else if (val1 > GUIDE_BUILD_CANNON) {
            val1 = GUIDE_BUILD_CANNON;
            val2 = 0;
        }
    }
    
    if ((isInit || val1 == GUIDE_ADD_BUILDER)) { //BUILDING_BUILDER_HOME
        if (LogicInfo::getSingleton().m_myInfo.totalworkernumsex >= 2 && val2 == 0) {
            val1 = GUIDE_BUILD_ELIXIR_COLLECT;
            val2 = 0;
        }
    }
    
    if((isInit || val1 == GUIDE_BUILD_ELIXIR_COLLECT)) //BUILDING_ELIXIR_COLLECTOR
    {
        if (PlayerScene::getSingleton().findBuilding(20301) != NULL) {
            
            if (val2 == 0) {
                Building* build = PlayerScene::getSingleton().findBuilding(20301);
                if(build->m_pBuildingInfo->state == BUILDING_STATE_NORMAL)
                {
                    val1 = GUIDE_BUILD_ELIXIR_STORAGE;
                    val2 = 0;
                }
                else
                {
                    val1 = GUIDE_BUILD_ELIXIR_COLLECT;
                    val2 = 60;
                }
            }
        }
        else if (val1 > GUIDE_BUILD_ELIXIR_COLLECT) {
            val1 = GUIDE_BUILD_ELIXIR_COLLECT;
            val2 = 0;
        }
    }
    
    if((isInit || val1 == GUIDE_BUILD_ELIXIR_STORAGE)) //GUIDE_BUILD_ELIXIR_STORAGE
    {
        if (PlayerScene::getSingleton().findBuilding(20401) != NULL) {
            if (val2 == 0) {
                Building* build = PlayerScene::getSingleton().findBuilding(20401);
                if(build->m_pBuildingInfo->state == BUILDING_STATE_NORMAL)
                {
                    val1 = GUIDE_BUILD_GOLD_STORAGE;
                    val2 = 0;
                }
                else
                {
                    val1 = GUIDE_BUILD_ELIXIR_STORAGE;
                    val2 = 60;
                }
            }
        }
        else if (val1 > GUIDE_BUILD_ELIXIR_STORAGE) {
            val1 = GUIDE_BUILD_ELIXIR_STORAGE;
            val2 = 0;
        }
    }
    
    if((isInit || val1 == GUIDE_BUILD_GOLD_STORAGE)) //BUILDING_GOLD_STORAGE
    {
        if (PlayerScene::getSingleton().findBuilding(20601) != NULL) {
            if (val2 == 0) {
                Building* build = PlayerScene::getSingleton().findBuilding(20601);
                if(build->m_pBuildingInfo->state == BUILDING_STATE_NORMAL)
                {
                    val1 = GUIDE_BUILD_BARRACK;
                    val2 = 0;
                }
                else
                {
                    val1 = GUIDE_BUILD_GOLD_STORAGE;
                    val2 = 50;
                }
            }
        }
        else if (val1 > GUIDE_BUILD_GOLD_STORAGE) {
            val1 = GUIDE_BUILD_GOLD_STORAGE;
            val2 = 0;
        }
    }
    
    if ((isInit || val1 == GUIDE_BUILD_BARRACK)) { //BUILDING_BARRACKS
        if (PlayerScene::getSingleton().findBuilding(30201) != NULL) {
            if (val2 == 0) {
                Building* build = PlayerScene::getSingleton().findBuilding(30201);
                if(build->m_pBuildingInfo->state == BUILDING_STATE_NORMAL){
                    val1 = GUIDE_TRAIN_SOLDIER;
                    val2 = 0;
                }else if(build->m_pBuildingInfo->state == BUILDING_STATE_BUILD || build->m_pBuildingInfo->state == BUILDING_STATE_UPGRADING){
                    val1 = GUIDE_BUILD_BARRACK;
                    val2 = 50;
                }
            }
        }
        else if (val1 > GUIDE_BUILD_BARRACK) {
            val1 = GUIDE_BUILD_BARRACK;
            val2 = 0;
        }
    }

    if(val1 == GUIDE_TRAIN_SOLDIER)
    {
        if (val2 == 0 && LogicInfo::getSingleton().countSoldierNums() >= 20) {
            val1 = GUIDE_RAVEN_GOBLIN;
            val2 = 0;
        }
        else if (val2 == 10 && (PlayerScene::getSingleton().m_pCurBuilding != NULL &&
            PlayerScene::getSingleton().m_pCurBuilding->m_pBuildingInfo->buildingid/100 == BUILDING_BARRACKS))
        {
            val2 = 20;
        }
        else if (val2 == 30 && LogicInfo::getSingleton().m_curSoldier >= 20)
        {
            val2 = 40;
        }
    }

    if (val1 == GUIDE_RAVEN_GOBLIN)
    {
        if (val2 == 0 && LogicInfo::getSingleton().countSoldierNums() < 20) {
            val1 = GUIDE_MAKE_NAME;
            val2 = 0;
        }
    }
    
    if ((isInit && i1 == GUIDE_MAKE_NAME))
    {
        char str[128];
        sprintf(str, "uid_%d", LogicInfo::getSingleton().m_myInfo.uid);
        if(LogicInfo::getSingleton().m_myInfo.uname == str) {
            val1 = GUIDE_MAKE_NAME;
            val2 = 0;
        }
        else{
            val1 = GUIDE_UPGRADE_TOWNHALL;
            val2 = 0;
        }
    }
    
    if ((isInit || val1 == GUIDE_UPGRADE_TOWNHALL)) {  //BUILDING_TOWN_HALL
        
        if (PlayerScene::getSingleton().findBuilding(20102) != NULL) {
            if (val2 == 0) {
                Building* build = PlayerScene::getSingleton().findBuilding(20102);
                
                if (build->m_pBuildingInfo->state == BUILDING_STATE_UPGRADING) {
                    val1 = GUIDE_UPGRADE_TOWNHALL;
                    val2 = 40;
                }
                else if (build->m_pBuildingInfo->state == BUILDING_STATE_NORMAL) {
                    val1 = GUIDE_ACHIEVEMENT;
                    val2 = 0;
                }
            }
        }
        else if (val1 > GUIDE_UPGRADE_TOWNHALL) {
            val1 = GUIDE_UPGRADE_TOWNHALL;
            val2 = 0;
        }
    }
    
    if (val1 != i1 || val2 == 0) {
        LogicInfo::getSingleton().setGuide(val1);
    }
}

void GuideMgr::refresh(int i1, int i2)
{
    CCControlButton* pItem = NULL;
    
    m_pActiveBtn = NULL;
    m_pActiveBuilding = NULL;
    
    this->hideArrow();
    
    if (PlayerScene::getSingleton().m_pGameWorld == NULL)
        return;
    
    MainUI* pMainUI = PlayerScene::getSingleton().m_pGameWorld->m_pMainUI;
    
    if(val1 != i1)
    {
        val1 = i1;
        LogicInfo::getSingleton().setGuide(val1);
    }
    val2 = i2;
    
    check(i1, false);
    
    if(val1 == GUIDE_NEW_WELCOME)
    {
        if(val2 == 0){
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if (val2 == 10){
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if (val2 == 20){
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
    }
    else if(val1 == GUIDE_BUILD_CANNON)
    {
        if(val2 == 0){
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if(val2 == 10){
            pItem = pMainUI->m_pBtnShop;
            
            m_pActiveBtn = pItem;
        }
        else if(val2 == 20)
        {
            pItem = (CCControlButton*)(PlayerScene::getSingleton().m_pGameWorld)->m_pTypeShop->m_pShopMenu->getChildByTag(2);
            
            m_pActiveBtn = pItem;
        }
        else if(val2 == 30)
        {
            pItem = (CCControlButton*)(PlayerScene::getSingleton().m_pGameWorld)->m_pShop->m_pShopView->getChildByTag(1);
            
            m_pActiveBtn = (CCControlButton*)(PlayerScene::getSingleton().m_pGameWorld)->m_pShop->m_pShopView->getChildByTag(0);
            
        }
        else if(val2 == 40)
        {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if(val2 == 50)
        {
            pItem = (PlayerScene::getSingleton().m_pGameWorld)->m_pBuildUI->m_pBtnOK;
            m_pActiveBtn = pItem;
            
        }
        else if(val2 == 60)
        {
            Building* build = PlayerScene::getSingleton().findBuilding(10101);
            if(build != NULL)
            {
                m_pActiveBuilding = build;
                PlayerScene::getSingleton().centerOfBuilding(*build);
            }
        }
        else if(val2 == 70)
        {
            pItem = pMainUI->m_pBtnFast;
            m_pActiveBtn = pItem;
        }
    }
    else if (val1 == GUIDE_DEFEND_TOWNHALL)
    {
        if (val2 == 0) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if (val2 == 10) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if (val2 == 20) {
            
            LogicInfo::getSingleton().m_curState = LOGICSTATE_ATTACK;
            LogicInfo::getSingleton().m_isAttackDropPerson = true;
            LogicInfo::getSingleton().m_isFinishAttack = false;
            LogicInfo::getSingleton().m_lastFightSoldierNums = 0;
            
            int personid = PERSON_BARBARIAN * 100 + 1;
            for (int i = 0; i < 3; i++) {

                Person* pPerson = PlayerScene::getSingleton().dropPerson2Attack(personid, 5, 38+i*4);
                pPerson->m_speed *= 1.5;
            }
            
            float scale = 0.5;
            CCSize size = PlayerScene::getSingleton().m_pRoot->m_size;
            CCSize winSize = CCDirector::sharedDirector()->getWinSize();
            
            PlayerScene::getSingleton().setScale(scale, ccp(0, 0));
            CCPoint pos(-(size.width*scale*0.8-winSize.width), -(size.height*scale)/2);
            PlayerScene::getSingleton().m_pRoot->fixPos(pos.x, pos.y);
            
//            CCScaleTo* pScaleTo = CCScaleTo::create(0.5, scale);
            CCMoveTo* pMoveTo = CCMoveTo::create(1.0, pos);
            
            PlayerScene::getSingleton().m_pRoot->runAction(pMoveTo);
        }
        else if (val2 == 30) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if (val2 == 40) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
    }
    else if(val1 == GUIDE_ATTACK_GOBLIN)
    {
        if (val2 == 0) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if(val2 == 10){
            FightSoldierInfoEx info;
            info.pid = PERSON_WIZARD*100 + 1;
            info.numsex = 5;
            LogicInfo::getSingleton().m_lstFightSoldier.clear();
            LogicInfo::getSingleton().m_lstFightSoldier.push_back(info);
            
            LogicInfo::getSingleton().m_lastFightSoldierNums = info.numsex;
            
            LogicInfo::getSingleton().attackEnv(0);
            PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_FINDMATCH);
        }
        else if (val2 == 20) {
            
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if (val2 == 30) {
            //wait fight
        }
        else if (val2 == 40) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
    }
    else if(val1 == GUIDE_ADD_BUILDER)
    {
        if(val2 == 0){
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if(val2 == 10){
            pItem = pMainUI->m_pBtnShop;
            m_pActiveBtn = pItem;
        }
        else if(val2 == 20){
            pItem = (CCControlButton*)PlayerScene::getSingleton().m_pGameWorld->m_pTypeShop->m_pShopMenu->getChildByTag(1);
            m_pActiveBtn = pItem;
        }
        else if(val2 == 30){
            pItem = (CCControlButton*)PlayerScene::getSingleton().m_pGameWorld->m_pShop->m_pShopView->getChildByTag(4);
            m_pActiveBtn = pItem;
        }
        else if(val2 == 40)
        {
            pItem = PlayerScene::getSingleton().m_pGameWorld->m_pBuildUI->m_pBtnOK;
            m_pActiveBtn = pItem;
        }
    }
    else if(val1 == GUIDE_BUILD_ELIXIR_COLLECT)
    {
        if(val2 == 0) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if (val2 == 10) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if(val2 == 20) {
            pItem = pMainUI->m_pBtnShop;
            m_pActiveBtn = pItem;
        }
        else if(val2 == 30) {
            pItem = (CCControlButton*)(PlayerScene::getSingleton().m_pGameWorld)->m_pTypeShop->m_pShopMenu->getChildByTag(1);
            m_pActiveBtn = pItem;
        }
        else if(val2 == 40) {
            pItem = (CCControlButton*)(PlayerScene::getSingleton().m_pGameWorld)->m_pShop->m_pShopView->getChildByTag(0);
            m_pActiveBtn = pItem;
        }
        else if(val2 == 50) {
            pItem = (PlayerScene::getSingleton().m_pGameWorld)->m_pBuildUI->m_pBtnOK;
            m_pActiveBtn = pItem;
        }
        else if(val2 == 60) {
            Building* build = PlayerScene::getSingleton().findBuilding(20301);
            if(build != NULL){
                m_pActiveBuilding = build;
                PlayerScene::getSingleton().centerOfBuilding(*build);
            }
        }
        else if(val2 == 70) {
            pItem = pMainUI->m_pBtnFast;
            m_pActiveBtn = pItem;
        }
    }
    else if (val1 == GUIDE_BUILD_ELIXIR_STORAGE)
    {
        if (val2 == 0) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if (val2 == 10) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if (val2 == 20) {
            pItem = pMainUI->m_pBtnShop;
            m_pActiveBtn = pItem;
        }
        else if (val2 == 30) {
            pItem = (CCControlButton*)(PlayerScene::getSingleton().m_pGameWorld)->m_pTypeShop->m_pShopMenu->getChildByTag(1);
            m_pActiveBtn = pItem;
        }
        else if (val2 == 40) {
            pItem = (CCControlButton*)(PlayerScene::getSingleton().m_pGameWorld)->m_pShop->m_pShopView->getChildByTag(1);
            m_pActiveBtn = pItem;
        }
        else if(val2 == 50) {
            pItem = PlayerScene::getSingleton().m_pGameWorld->m_pBuildUI->m_pBtnOK;
            m_pActiveBtn = pItem;
        }
        else if(val2 == 60) {
            Building* build = PlayerScene::getSingleton().findBuilding(20401);
            if(build != NULL) {
                m_pActiveBuilding = build;
                PlayerScene::getSingleton().centerOfBuilding(*build);
            }
        }
        else if(val2 == 70) {
            pItem = pMainUI->m_pBtnFast;
            m_pActiveBtn = pItem;
        }
    }
    else if(val1 == GUIDE_BUILD_GOLD_STORAGE)
    {
        if(val2 == 0) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if(val2 == 10) {
            pItem = pMainUI->m_pBtnShop;
            m_pActiveBtn = pItem;
        }
        else if(val2 == 20) {
            pItem = (CCControlButton*)(PlayerScene::getSingleton().m_pGameWorld)->m_pTypeShop->m_pShopMenu->getChildByTag(1);
            m_pActiveBtn = pItem;
        }
        else if(val2 == 30) {
            pItem = (CCControlButton*)(PlayerScene::getSingleton().m_pGameWorld)->m_pShop->m_pShopView->getChildByTag(3);
            m_pActiveBtn = pItem;
        }
        else if(val2 == 40) {
            pItem = PlayerScene::getSingleton().m_pGameWorld->m_pBuildUI->m_pBtnOK;
            m_pActiveBtn = pItem;
        }
        else if(val2 == 50) {
            Building* build = PlayerScene::getSingleton().findBuilding(20601);
            if(build != NULL) {
                m_pActiveBuilding = build;
                PlayerScene::getSingleton().centerOfBuilding(*build);
            }
        }
        else if(val2 == 60) {
            pItem = pMainUI->m_pBtnFast;
            m_pActiveBtn = pItem;
        }
        else if (val2 == 70) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
    }
    else if(val1 == GUIDE_BUILD_BARRACK)
    {        
        if(val2 == 0) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if (val2 == 10) {
            pItem = pMainUI->m_pBtnShop;
            m_pActiveBtn = pItem;
        }
        else if(val2 == 20) {
            pItem = (CCControlButton*)PlayerScene::getSingleton().m_pGameWorld->m_pTypeShop->m_pShopMenu->getChildByTag(3);
            m_pActiveBtn = pItem;
        }
        else if(val2 == 30) {
            pItem = (CCControlButton*)PlayerScene::getSingleton().m_pGameWorld->m_pShop->m_pShopView->getChildByTag(1);
            m_pActiveBtn = pItem;
        }
        else if(val2 == 40) {
            pItem = PlayerScene::getSingleton().m_pGameWorld->m_pBuildUI->m_pBtnOK;
            m_pActiveBtn = pItem;
        }
        else if(val2 == 50) {
            Building* build = PlayerScene::getSingleton().findBuilding(30201);
            if(build != NULL) {
                m_pActiveBuilding = build;
                PlayerScene::getSingleton().centerOfBuilding(*build);
            }
        }
        else if(val2 == 60) {
            pItem = pMainUI->m_pBtnFast;
            m_pActiveBtn = pItem;
        }
    }
    else if(val1 == GUIDE_TRAIN_SOLDIER)
    {
        if(val2 == 0) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if(val2 == 10) {
            Building* build = PlayerScene::getSingleton().findBuilding(30201);
            if(build != NULL)
            {
                m_pActiveBuilding = build;
                PlayerScene::getSingleton().centerOfBuilding(*build);
            }
        }
        else if(val2 == 20) {
            pItem = pMainUI->m_pBtnTroops;
            m_pActiveBtn = pItem;
        }
        else if(val2 == 30) {
            SoldierItem* pSoldierItem = (SoldierItem*)PlayerScene::getSingleton().m_pGameWorld->m_pBarrackUI->m_pSoldierView->getChildren()->objectAtIndex(0);
            pItem = pSoldierItem->m_pBtnTrain;
            
            m_pActiveBtn = pItem;
        }
        else if(val2 == 40) {
            pItem = PlayerScene::getSingleton().m_pGameWorld->m_pBarrackUI->m_pBtnFinishTrain;
            m_pActiveBtn = pItem;
        }
    }
    else if(val1 == GUIDE_RAVEN_GOBLIN)
    {
        if(val2 == 0) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        if(val2 == 10) {
            pMainUI->m_pBtnAttack->setVisible(true);
            pItem = pMainUI->m_pBtnAttack;
            m_pActiveBtn = pItem;
        }
        else if(val2 == 20) {

            FindMatchUI* pFindMatchUI = PlayerScene::getSingleton().m_pGameWorld->m_pFindMatchUI;
            
            EnvMapNode stageNode = pFindMatchUI->m_pEnvMapUI->m_stageNodeVec[1];
            pFindMatchUI->m_pEnvMapUI->onSelectStage(stageNode);
            
            pItem = pFindMatchUI->m_pEnvMapUI->m_pBtnAttack;
            m_pActiveBtn = pItem;
        }
        if(val2 == 30) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if(val2 == 40) {
            //wait fight
        }
    }
    else if (val1 == GUIDE_MAKE_NAME)
    {
        if (val2 == 0) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if (val2 == 10) {
            PlayerScene::getSingleton().m_pGameWorld->showCreateUI();
        }
    }
    else if(val1 == GUIDE_UPGRADE_TOWNHALL)
    {
        if(val2 == 0) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if(val2 == 10) {
            Building* build = PlayerScene::getSingleton().findBuilding(20101);
            if(build != NULL) {
                m_pActiveBuilding = build;
                PlayerScene::getSingleton().centerOfBuilding(*build);
            }
        }
        else if(val2 == 20) {
            pItem = pMainUI->m_pBtnUpgrade;
            m_pActiveBtn = pItem;
        }
        else if(val2 == 30) {
            pItem = PlayerScene::getSingleton().m_pGameWorld->m_pUpLevelUI->m_pBtnUpgrade;
            m_pActiveBtn = pItem;
        }
        else if(val2 == 40) {
            Building* build = PlayerScene::getSingleton().findBuilding(20102);
            if(build != NULL) {
                m_pActiveBuilding = build;
                PlayerScene::getSingleton().centerOfBuilding(*build);
            }
        }
        else if(val2 == 50) {
            pItem = pMainUI->m_pBtnFast;
            m_pActiveBtn = pItem;
        }
        else if (val2 == 60) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
    }
    else if (val1 == GUIDE_ACHIEVEMENT)
    {
        if (val2 == 0) {
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
        else if (val2 == 10) {
            pMainUI->m_pInfoNode->setVisible(true);
            pMainUI->m_pBtnAchievement->setVisible(true);
            
            pItem = pMainUI->m_pBtnAchievement;
            m_pActiveBtn = pItem;
        }
    }
    else if(val1 == GUIDE_FINAL_HINT)
    {
        if(val2 == 0){
            PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
        }
    }

    if (m_pArrow) {
        refreshArrowPos();
        
        if (m_pActiveBtn != NULL) {
            
            if (m_pActiveBtn == PlayerScene::getSingleton().m_pBuildOK) {
                PlayerScene::getSingleton().m_pBuildOK->addChild(m_pArrow, SCENE_Z_BUILDINGUI);
                m_pArrowAnimation->runAnimationsForSequenceNamed("arrow_am");
            }
            else
            {
                PlayerScene::getSingleton().m_pGameWorld->addChild(m_pArrow, SCENE_Z_BUILDINGUI);
                m_pArrowAnimation->runAnimationsForSequenceNamed("arrow_am");
            }
            
            if (!m_pActiveBtn->isVisible()) {
                m_pArrow->setVisible(false);
            }
        }
        else if (m_pActiveBuilding != NULL)
        {
            PlayerScene::getSingleton().m_pRoot->addChild(m_pArrow, SCENE_Z_BUILDINGUI);
            m_pArrowAnimation->runAnimationsForSequenceNamed("arrow_am");
        }
    }
}

void GuideMgr::next()
{
    if (val1 == GUIDE_NEW_WELCOME)
    {
        if (val2 == 0) {
            refresh(GUIDE_NEW_WELCOME, 10);
        }
        else if (val2 == 10) {
            refresh(GUIDE_NEW_WELCOME, 20);
        }
        else if (val2 == 20) {
            refresh(GUIDE_BUILD_CANNON, 0);
        }
    }
    else if (val1 == GUIDE_BUILD_CANNON)
    {
        if (val2 == 0) {
            refresh(GUIDE_BUILD_CANNON, 10);
        }
        else if (val2 == 10) {
            refresh(GUIDE_BUILD_CANNON, 20);
        }
        else if (val2 == 20) {
            refresh(GUIDE_BUILD_CANNON, 30);
        }
        else if (val2 == 30) {
            refresh(GUIDE_BUILD_CANNON, 40);
        }
        else if (val2 == 40) {
            refresh(GUIDE_BUILD_CANNON, 50);
        }
        else if (val2 == 50) {
            refresh(GUIDE_BUILD_CANNON, 70);
        }
        else if (val2 == 60) {
            refresh(GUIDE_BUILD_CANNON, 70);
        }
        else if (val2 == 70) {
            
            PlayerScene::getSingleton().unselectCurBuilding();
            
            refresh(GUIDE_DEFEND_TOWNHALL, 0);
        }
    }
    else if (val1 == GUIDE_DEFEND_TOWNHALL)
    {
        if (val2 == 0) {
            
            PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->setVisible(false);
            refresh(GUIDE_DEFEND_TOWNHALL, 10);
        }
        else if (val2 == 10) {
            refresh(GUIDE_DEFEND_TOWNHALL, 20);
        }
        else if (val2 == 20) {
            refresh(GUIDE_DEFEND_TOWNHALL, 30);
        }
        else if (val2 == 30) {
            refresh(GUIDE_DEFEND_TOWNHALL, 40);
        }
        else if (val2 == 40) {
            refresh(GUIDE_ATTACK_GOBLIN, 0);
        }
    }
    else if (val1 == GUIDE_ATTACK_GOBLIN)
    {
        if (val2 == 0) {
            PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->setVisible(false);
            refresh(GUIDE_ATTACK_GOBLIN, 10);
        }
        else if (val2 == 10) {
            refresh(GUIDE_ATTACK_GOBLIN, 20);
        }
        else if (val2 == 20) {
            refresh(GUIDE_ATTACK_GOBLIN, 30);
        }
        else if (val2 == 30) {
            refresh(GUIDE_ATTACK_GOBLIN, 40);
        }
        else if (val2 == 40) {
            refresh(GUIDE_ADD_BUILDER, 0);
        }
    }
    else if (val1 == GUIDE_ADD_BUILDER)
    {
        if (val2 == 0) {
            refresh(GUIDE_ADD_BUILDER, 10);
        }
        else if (val2 == 10) {
            refresh(GUIDE_ADD_BUILDER, 20);
        }
        else if (val2 == 20) {
            refresh(GUIDE_ADD_BUILDER, 30);
        }
        else if (val2 == 30) {
            refresh(GUIDE_ADD_BUILDER, 40);
        }
        else if (val2 == 40) {
            
            PlayerScene::getSingleton().unselectCurBuilding();
            
            refresh(GUIDE_BUILD_ELIXIR_COLLECT, 0);
        }
    }
    else if (val1 == GUIDE_BUILD_ELIXIR_COLLECT)
    {
        if (val2 == 0) {
            refresh(GUIDE_BUILD_ELIXIR_COLLECT, 10);
        }
        else if (val2 == 10) {
            refresh(GUIDE_BUILD_ELIXIR_COLLECT, 20);
        }
        else if (val2 == 20) {
            refresh(GUIDE_BUILD_ELIXIR_COLLECT, 30);
        }
        else if (val2 == 30) {
            refresh(GUIDE_BUILD_ELIXIR_COLLECT, 40);
        }
        else if (val2 == 40) {
            refresh(GUIDE_BUILD_ELIXIR_COLLECT, 50);
        }
        else if (val2 == 50) {
            refresh(GUIDE_BUILD_ELIXIR_COLLECT, 70);
        }
        else if (val2 == 60) {
            refresh(GUIDE_BUILD_ELIXIR_COLLECT, 70);
        }
        else if (val2 == 70) {
            
            PlayerScene::getSingleton().unselectCurBuilding();
            
            refresh(GUIDE_BUILD_ELIXIR_STORAGE, 0);
        }
    }
    else if (val1 == GUIDE_BUILD_ELIXIR_STORAGE)
    {
        if (val2 == 0) {
            refresh(GUIDE_BUILD_ELIXIR_STORAGE, 10);
        }
        else if (val2 == 10) {
            refresh(GUIDE_BUILD_ELIXIR_STORAGE, 20);
        }
        else if (val2 == 20) {
            refresh(GUIDE_BUILD_ELIXIR_STORAGE, 30);
        }
        else if (val2 == 30) {
            refresh(GUIDE_BUILD_ELIXIR_STORAGE, 40);
        }
        else if (val2 == 40) {
            refresh(GUIDE_BUILD_ELIXIR_STORAGE, 50);
        }
        else if (val2 == 50) {
            refresh(GUIDE_BUILD_ELIXIR_STORAGE, 70);
        }
        else if (val2 == 60) {
            refresh(GUIDE_BUILD_ELIXIR_STORAGE, 70);
        }
        else if (val2 == 70) {
            
            PlayerScene::getSingleton().unselectCurBuilding();
            
            refresh(GUIDE_BUILD_GOLD_STORAGE, 0);
        }
    }
    else if (val1 == GUIDE_BUILD_GOLD_STORAGE)
    {
        if (val2 == 0) {
            refresh(GUIDE_BUILD_GOLD_STORAGE, 10);
        }
        else if (val2 == 10) {
            refresh(GUIDE_BUILD_GOLD_STORAGE, 20);
        }
        else if (val2 == 20) {
            refresh(GUIDE_BUILD_GOLD_STORAGE, 30);
        }
        else if (val2 == 30) {
            refresh(GUIDE_BUILD_GOLD_STORAGE, 40);
        }
        else if (val2 == 40) {
            refresh(GUIDE_BUILD_GOLD_STORAGE, 60);
        }
        else if (val2 == 50) {
            refresh(GUIDE_BUILD_GOLD_STORAGE, 60);
        }
        else if (val2 == 60) {
            
            PlayerScene::getSingleton().unselectCurBuilding();
            
            refresh(GUIDE_BUILD_GOLD_STORAGE, 70);
        }
        else if (val2 == 70) {
            refresh(GUIDE_BUILD_BARRACK, 0);
        }
    }
    else if (val1 == GUIDE_BUILD_BARRACK)
    {
        if (val2 == 0) {
            refresh(GUIDE_BUILD_BARRACK, 10);
        }
        else if (val2 == 10) {
            refresh(GUIDE_BUILD_BARRACK, 20);
        }
        else if (val2 == 20) {
            refresh(GUIDE_BUILD_BARRACK, 30);
        }
        else if (val2 == 30) {
            refresh(GUIDE_BUILD_BARRACK, 40);
        }
        else if (val2 == 40) {
            refresh(GUIDE_BUILD_BARRACK, 60);
        }
        else if (val2 == 50) {
            refresh(GUIDE_BUILD_BARRACK, 60);
        }
        else if (val2 == 60) {
            refresh(GUIDE_TRAIN_SOLDIER, 0);
        }
    }
    else if (val1 == GUIDE_TRAIN_SOLDIER)
    {
        if (val2 == 0) {
            refresh(GUIDE_TRAIN_SOLDIER, 10);
        }
        else if (val2 == 10) {
            refresh(GUIDE_TRAIN_SOLDIER, 20);
        }
        else if (val2 == 20) {
            refresh(GUIDE_TRAIN_SOLDIER, 30);
        }
        else if (val2 == 30) {
            refresh(GUIDE_TRAIN_SOLDIER, 40);
        }
        else if (val2 == 40) {
            
            PlayerScene::getSingleton().unselectCurBuilding();
            
            refresh(GUIDE_RAVEN_GOBLIN, 0);
        }
    }
    else if (val1 == GUIDE_RAVEN_GOBLIN)
    {
        if (val2 == 0) {
            refresh(GUIDE_RAVEN_GOBLIN, 10);
        }
        else if (val2 == 10) {
            refresh(GUIDE_RAVEN_GOBLIN, 20);
        }
        else if (val2 == 20) {
            refresh(GUIDE_RAVEN_GOBLIN, 30);
        }
        else if (val2 == 30) {
            refresh(GUIDE_RAVEN_GOBLIN, 40);
        }
        else if (val2 == 40) {
            refresh(GUIDE_MAKE_NAME, 0);
        }
    }
    else if (val1 == GUIDE_MAKE_NAME)
    {
        if (val2 == 0) {
            refresh(GUIDE_MAKE_NAME, 10);
        }
        else if (val2 == 10) {
            refresh(GUIDE_UPGRADE_TOWNHALL, 0);
        }
    }
    else if (val1 == GUIDE_UPGRADE_TOWNHALL)
    {
        if (val2 == 0) {
            refresh(GUIDE_UPGRADE_TOWNHALL, 10);
        }
        else if (val2 == 10) {
            refresh(GUIDE_UPGRADE_TOWNHALL, 20);
        }
        else if (val2 == 20) {
            refresh(GUIDE_UPGRADE_TOWNHALL, 30);
        }
        else if (val2 == 30) {
            refresh(GUIDE_UPGRADE_TOWNHALL, 50);
        }
        else if (val2 == 40) {
            refresh(GUIDE_UPGRADE_TOWNHALL, 50);
        }
        else if (val2 == 50) {
            
            PlayerScene::getSingleton().unselectCurBuilding();
            
            refresh(GUIDE_UPGRADE_TOWNHALL, 60);
        }
        else if (val2 == 60) {            
            refresh(GUIDE_ACHIEVEMENT, 0);
        }
    }
    else if (val1 == GUIDE_ACHIEVEMENT) {
        if (val2 == 0) {
            refresh(GUIDE_ACHIEVEMENT, 10);
        }
        else if (val2 == 10) {
            refresh(GUIDE_ACHIEVEMENT, 20);
        }
        else if (val2 == 20) {
            refresh(GUIDE_FINAL_HINT, 0);
        }
    }
    else if (val1 == GUIDE_FINAL_HINT) {
        refresh(GUIDE_END, 0);
        setMainUIButton(PlayerScene::getSingleton().m_pGameWorld->m_pMainUI, true);
        LogicInfo::getSingleton().countMilitaryRank();
        PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->showBtnTip();
    }
}

void GuideMgr::refreshArrowPos()
{
    if (m_pArrow != NULL) {
        
        if (m_pActiveBtn != NULL) {
            
            CCPoint pos = m_pActiveBtn->getParent()->convertToWorldSpace(m_pActiveBtn->getPosition());
            float offX, offY;
            if (m_pArrow->getScaleY() < 0) {
                
                offX = (0.5-m_pActiveBtn->getAnchorPoint().x)*m_pActiveBtn->getContentSize().width;
                offY = (0.2-m_pActiveBtn->getAnchorPoint().y)*m_pActiveBtn->getContentSize().height;
                
                m_pArrow->setPosition(ccp(pos.x + offX, pos.y + offY));
            }
            else
            {
                offX = (0.5-m_pActiveBtn->getAnchorPoint().x)*m_pActiveBtn->getContentSize().width;
                offY = (0.8-m_pActiveBtn->getAnchorPoint().y)*m_pActiveBtn->getContentSize().height;
                m_pArrow->setPosition(ccp(pos.x + offX, pos.y + offY));
            }
            
            if (m_pActiveBtn == PlayerScene::getSingleton().m_pBuildOK) {
                m_pArrow->setPosition(offX+m_pActiveBtn->getContentSize().width/2, offY+m_pActiveBtn->getContentSize().height/2);
            }
            if (PlayerScene::getSingleton().m_pGameWorld->m_pMainUI != NULL) {
                
                for (int index = 0; index < PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->m_lstBuildingBtn.size(); index++) {
                    if (m_pActiveBtn == PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->m_lstBuildingBtn[index]) {
                        
                        float offY = (0.8-m_pActiveBtn->getAnchorPoint().y)*m_pActiveBtn->getContentSize().height;
                        m_pArrow->setPosition(ccp(m_pActiveBtn->getPositionX(), 110 + offY));
                        break;
                    }
                }
            }
        }
        else if (m_pActiveBuilding != NULL) {
            
            CCPoint pos = PlayerScene::getSingleton().countScenePos(m_pActiveBuilding->getCenterPt().x, m_pActiveBuilding->getCenterPt().y);
            
            m_pArrow->setPosition(pos.x, pos.y + TILE_H);
        }
        
        m_pArrow->setVisible(true);
    }
}

void GuideMgr::setMainUIButton(MainUI* pMainUI, bool isVisible)
{
    //main ui button
    m_btnLst.clear();
    m_btnLst.push_back(pMainUI->m_pBtnAttack);
    m_btnLst.push_back(pMainUI->m_pBtnShop);
    m_btnLst.push_back(pMainUI->m_pBtnAchievement);
    m_btnLst.push_back(pMainUI->m_pBtnRanking);
    m_btnLst.push_back(pMainUI->m_pBtnMail);
    m_btnLst.push_back(pMainUI->m_pBtnAddWorker);
    m_btnLst.push_back(pMainUI->m_pBtnAddShield);
    m_btnLst.push_back(pMainUI->m_pBtnAddGold);
    m_btnLst.push_back(pMainUI->m_pBtnAddElixir);
    m_btnLst.push_back(pMainUI->m_pBtnAddGem);
    m_btnLst.push_back(pMainUI->m_pBtnSetting);
    m_btnLst.push_back(pMainUI->m_pBtnSpin);
    m_btnLst.push_back(pMainUI->m_pBtnFreeBonus);    
    m_btnLst.push_back(pMainUI->m_pBtnFreeBonus);
    m_btnLst.push_back(pMainUI->m_pBtnMilitaryRank);
    m_btnLst.push_back(pMainUI->m_pBtnLeague);
    m_btnLst.push_back(pMainUI->m_pBtnFBShare);
    m_btnLst.push_back(pMainUI->m_pBtnAnnouncement);
    m_btnLst.push_back(pMainUI->m_pBtnClanWar);
    m_btnLst.push_back(pMainUI->m_pBtnChat);
    m_btnLst.push_back(pMainUI->m_pBtnEdit);

    for (int i = 0; i < m_btnLst.size(); i++) {
        if (m_btnLst[i] != pMainUI->m_pBtnShop && m_btnLst[i] != pMainUI->m_pBtnAddGem
            ) {
            m_btnLst[i]->setVisible(isVisible);
        }
    }
    pMainUI->m_pInfoNode->setVisible(isVisible);
    
    if (PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pChatUI) {
         PlayerScene::getSingleton().m_pGameWorld->m_pChatUI->setVisible(isVisible);
    }
}

void GuideMgr::addGuideUI(CCNode *pGuideUI)
{
    PlayerScene::getSingleton().m_pGameWorld->showGuideUI();
    m_pGuideUI = PlayerScene::getSingleton().m_pGameWorld->m_pGuideUI;
    
    m_pGuideUI->addChild(pGuideUI);
}

void GuideMgr::removeGuideUI(CCNode *pGuideUI)
{
    if (m_pGuideUI == pGuideUI) {
        PlayerScene::getSingleton().m_pGameWorld->hideGuideUI();
        m_pGuideUI = NULL;
    }
}

void GuideMgr::removeGuideUI()
{
    PlayerScene::getSingleton().m_pGameWorld->hideGuideUI();
    m_pGuideUI = NULL;
}

bool GuideMgr::isEquVal(int i1)
{
    if (i1 == val1) {
        return true;
    }
    return false;
}

bool GuideMgr::isEquVal(int i1, int i2)
{
    if(i1 == val1 && i2 == val2)
    {
        return true;
    }
    
    return false;
}

void GuideMgr::reset()
{
    val1 = 0;
    val2 = 0;
    
    LogicInfo::getSingleton().m_zyval = 0;
}

void GuideMgr::hideArrow()
{
    if(m_pArrow)
    {
        if(m_pArrow->getParent() != NULL)
        {
            m_pArrow->removeFromParentAndCleanup(false);
        }
        m_pArrow->setScaleY(scaleY);
    }
    
    if (m_pFinger) {
        m_pFinger->removeFromParentAndCleanup(true);
        m_pFinger = NULL;
    }
    
}

GuideItemInfo* GuideMgr::getGuideItemInfo(string key)
{
    map<string, GuideItemInfo>::iterator it = m_map.find(key);
    if (it != m_map.end()) {
        return &(it->second);
    }
    return NULL;
}

NS_COC_END
