//
//  ClanWarMapUI.cpp
//  GameClient
//
//  Created by qsun on 14-9-4.
//
//

#include "ClanWarMapUI.h"
#include "../coc/ClanMgr.h"
#include "../coc/ClanWarMgr.h"
#include "../coc/PlayerScene.h"
#include "../GameWorldScene.h"

ClanWarMapUI::ClanWarMapUI()
: m_bProcScaleAni(false)
, m_pCityMenuUI(NULL)
, m_pClanWarBattleUI(NULL)
, m_pClanWarSignupUI(NULL)
, m_pGroupWarBattleUI(NULL)
, m_pClanWarHelpUI(NULL)
, m_pClanWarTipUI(NULL)
, m_lastClanWarState(-1)
, m_lastGroupWarState(-1)
{
    
}

ClanWarMapUI::~ClanWarMapUI()
{
    unscheduleUpdate();
    
    LogicInfo::getSingleton().removeListener(this);
}

bool ClanWarMapUI::init()
{
    if (CUINode::init()) {
        
        LogicInfo::getSingleton().addListener(this);
        
        CCNode* pClanWar = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar.ccbi", this);
        this->setContentSize(pClanWar->getContentSize());
        this->setTargetTouch(true);
        this->addChild(pClanWar);
        
        CCNode* pClanWarMap = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_map.ccbi", this);
        m_pMapView->setContentSize(pClanWarMap->getContentSize());
        m_pMapView->addChild(pClanWarMap);
        
        m_pMapView->setMinScale(MAX(0.3f*getResolutionScale(), this->getContentSize().width / pClanWarMap->getContentSize().width));
        m_pMapView->setMaxScale(1.2f);
        m_pMapView->setZoomScale(m_pMapView->getMinScale());
        
        float offsetx = -(m_pMapView->getContentSize().width*m_pMapView->getZoomScale()-m_pMapView->getViewSize().width)/2;
        float offsety = -(m_pMapView->getContentSize().height*m_pMapView->getZoomScale()-m_pMapView->getViewSize().height)/2;
        
        m_pMapView->setContentOffset(ccp(offsetx, offsety));
        
        m_pCityMenuUI = ClanCityMenuUI::create();
        m_pCityMenuUI->setVisible(false);
        
        this->addChild(m_pCityMenuUI);
        
        if (PlayerScene::getSingleton().m_backHomeView != BACK_HOME_CLANWAR_BATTLE) {
            
            ClanWarMgr::getSingleton().m_pSelectCityInfo = NULL;
            
            HttpClient::getSingleton().requestClanWarInfo();
            
            m_pLoadingUI = CUINode::createWithFile("loading3.ccbi", this);
            m_pLoadingUI->setTargetTouch(true);
            this->addChild(m_pLoadingUI);
        }
        else {
            onNotify(NOTIFY_CLANWAR_CITYINFO);
        }
        
        scheduleUpdate();
    }
    
    return true;
}

bool ClanWarMapUI::onNotify(int notifyid, const void* pVal)
{
    if (notifyid == NOTIFY_CLANWAR_INFO) {
        refreshInfo();
        
        if(CCUserDefault::sharedUserDefault()->getBoolForKey("CLANWAR_MAP_TIP") == false)
        {
            ImgMgr::getSingleton().addSpriteFrames("game_npc");
            
            m_pClanWarTipUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("groupwar_help_1.ccbi", this);
            GuideMgr::getSingleton().addGuideUI(m_pClanWarTipUI);
            
            CCUserDefault::sharedUserDefault()->setBoolForKey("CLANWAR_MAP_TIP", true);
            CCUserDefault::sharedUserDefault()->flush();
        }
    }
    else if (notifyid == NOTIFY_CLANWAR_CITYINFO) {
        
        for (int i = 0; i < ClanWarMgr::getSingleton().m_warCityList.size(); i++) {
            
            ClanCityInfo* pCityInfo = &ClanWarMgr::getSingleton().m_warCityList[i];
            
            ClanCityUI* pCityNode = ClanCityUI::create(pCityInfo);
            pCityNode->setPosition(ccp(pCityInfo->posx-pCityNode->getContentSize().width/2, pCityInfo->posy-pCityNode->getContentSize().height/2));
            pCityNode->setTouchControlView(m_pMapView);
            
            m_pMapView->addChild(pCityNode);
        }
        
        if (m_pLoadingUI) {
            this->removeChild(m_pLoadingUI);
            m_pLoadingUI = NULL;
        }
        
        refreshInfo();
        
        onFocusCity();
    }
    
    return true;
}

void ClanWarMapUI::onFocusCity()
{
    CCPoint focusPt = ccp(-1, -1);
    
    for (int i = 0; i < ClanWarMgr::getSingleton().m_warCityList.size(); i++) {
        
        ClanCityInfo* pCityInfo = &ClanWarMgr::getSingleton().m_warCityList[i];
        
        ClanCityUI* pCityNode = ClanCityUI::create(pCityInfo);
        pCityNode->setPosition(ccp(pCityInfo->posx-pCityNode->getContentSize().width/2, pCityInfo->posy-pCityNode->getContentSize().height/2));
        pCityNode->setTouchControlView(m_pMapView);
        
        m_pMapView->addChild(pCityNode);
        
        if (ClanWarMgr::getSingleton().isWarStateOpen()) {
            if (ClanWarMgr::getSingleton().m_warSignupCityId == pCityInfo->cityid) {
                focusPt = pCityNode->getPosition();
            }
        }
        else if (GroupWarMgr::getSingleton().isWarStateOpen()) {
            if (pCityNode->m_pCityInfo->citytype == GROUPWAR_CITY) {
                focusPt = pCityNode->getPosition();
            }
        }
    }
    
    if (focusPt.x != -1 && focusPt.y != -1) {
        
        CCSize viewSize = m_pMapView->getViewSize();

        float scale = 0.8;
        m_pMapView->setZoomScale(scale);
        
        CCSize mapSize = CCSizeMake(m_pMapView->getContentSize().width*scale, m_pMapView->getContentSize().height*scale);
        
        float posx = focusPt.x*scale - viewSize.width/2;
        float posy = focusPt.y*scale - viewSize.height/2;
        
        if (posx < 0) {
            posx = 0;
        }
        else if (posx + viewSize.width/2 >= mapSize.width) {
            posx = mapSize.width - viewSize.width/2;
        }
        if (posy < 0) {
            posy = 0;
        }
        else if (posy + viewSize.height/2 >= mapSize.height) {
            posy = mapSize.height - viewSize.height/2;
        }
        
        m_pMapView->setContentOffset(ccp(-posx, -posy));
    }
}

void ClanWarMapUI::update(float dt)
{
    if (m_bProcScaleAni) {
        
        float scale = m_pMapView->getZoomScale();
        float timeScaleAni = 0.5f;
        
        if(scale > 1.0)
        {
            scale -= fabs(m_pMapView->getMaxScale() - 1.0) * dt / timeScaleAni;
            
            if(scale < 1.0) {
                scale = 1.0;
                m_bProcScaleAni = false;
            }
            
            m_pMapView->setZoomScale(scale);
        }
    }
    
    int time = getCurrentTime();
    if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_CLOSE) {
        m_pClanWarStateTime->setVisible(false);
    }
    else if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_SIGNUP) {
        m_pClanWarStateTime->setString(formatTime(ClanWarMgr::getSingleton().m_warPrepareTime-time, 3, true).c_str());
    }
    else if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_BATTLE_PREPARE) {
        m_pClanWarStateTime->setString(formatTime(ClanWarMgr::getSingleton().m_warStartTime-time, 3, true).c_str());
    }
    else if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_BATTLE_START) {
        m_pClanWarStateTime->setString(formatTime(ClanWarMgr::getSingleton().m_warEndTime-time, 3, true).c_str());
    }
    else if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_BATTLE_SCORE) {
        m_pClanWarStateTime->setVisible(false);
    }
    else {
        if (ClanWarMgr::getSingleton().m_warNextTime > time) {
            m_pClanWarStateTime->setString(formatTime(ClanWarMgr::getSingleton().m_warNextTime-time, 3, true).c_str());
        }
    }
    
    if (m_lastClanWarState != ClanWarMgr::getSingleton().getWarState()) {
        
        refreshInfo();
        m_lastClanWarState = ClanWarMgr::getSingleton().getWarState();
    }
    
    
    if (GroupWarMgr::getSingleton().getWarState() == GROUPWAR_STATE_CLOSE) {
        m_pGroupWarStateTime->setVisible(false);
    }
    else if (GroupWarMgr::getSingleton().getWarState() == GROUPWAR_STATE_SIGNUP) {
        m_pGroupWarStateTime->setString(formatTime(GroupWarMgr::getSingleton().m_warPrepareTime-time, 3, true).c_str());
    }
    else if (GroupWarMgr::getSingleton().getWarState() == GROUPWAR_STATE_BATTLE_PREPARE) {
        m_pGroupWarStateTime->setString(formatTime(GroupWarMgr::getSingleton().m_warStartTime-time, 3, true).c_str());
    }
    else if (GroupWarMgr::getSingleton().getWarState() == GROUPWAR_STATE_BATTLE_START) {
        m_pGroupWarStateTime->setString(formatTime(GroupWarMgr::getSingleton().m_warEndTime-time, 3, true).c_str());
    }
    else if (GroupWarMgr::getSingleton().getWarState() == GROUPWAR_STATE_BATTLE_SCORE) {
        m_pGroupWarStateTime->setVisible(false);
    }
    else {
        if (GroupWarMgr::getSingleton().m_warNextTime > time) {
            m_pGroupWarStateTime->setString(formatTime(GroupWarMgr::getSingleton().m_warNextTime-time, 3, true).c_str());
        }
    }
    
    if (m_lastGroupWarState != GroupWarMgr::getSingleton().getWarState()) {
        
        refreshInfo();
        m_lastGroupWarState = GroupWarMgr::getSingleton().getWarState();
    }
}

void ClanWarMapUI::refreshInfo()
{
    int time = getCurrentTime();
    
    //clan war state
    m_pClanWarStateTime->setVisible(true);
    if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_CLOSE) {
        m_pClanWarStateTip->setString(ViewString::getValue("citywar_state_close_tip").c_str());
        m_pClanWarStateTime->setVisible(false);
    }
    else if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_SIGNUP) {
        
        m_pClanWarStateTip->setString(ViewString::getValue("citywar_state_signup_end_tip").c_str());
    }
    else if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_BATTLE_PREPARE) {
        m_pClanWarStateTip->setString(ViewString::getValue("citywar_state_start_time_tip").c_str());
    }
    else if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_BATTLE_START) {
        m_pClanWarStateTip->setString(ViewString::getValue("citywar_state_end_time_tip").c_str());
    }
    else if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_BATTLE_SCORE) {
        m_pClanWarStateTip->setString(ViewString::getValue("citywar_state_score_tip").c_str());
        m_pClanWarStateTime->setVisible(false);
    }
    else {
        
        if (ClanWarMgr::getSingleton().m_warNextTime > time) {
            m_pClanWarStateTip->setString(ViewString::getValue("citywar_state_countdown_tip").c_str());
        }
        else {
            m_pClanWarStateTip->setString(ViewString::getValue("citywar_state_close_tip").c_str());
            m_pClanWarStateTime->setVisible(false);
        }
    }
    
    if (ClanWarMgr::getSingleton().isWarStateOpen()) {
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("clanwar_events_0");
        m_pBtnClanWar->getChildByTag(1)->setVisible(true);
        ((CCSprite*)m_pBtnClanWar->getChildByTag(2))->setDisplayFrame(pFrame);
    }
    else {
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("clanwar_events_1");
        m_pBtnClanWar->getChildByTag(1)->setVisible(false);
        ((CCSprite*)m_pBtnClanWar->getChildByTag(2))->setDisplayFrame(pFrame);
    }
    
    //group
    m_pGroupWarStateTime->setVisible(true);
    if (GroupWarMgr::getSingleton().getWarState() == GROUPWAR_STATE_CLOSE) {
        m_pGroupWarStateTip->setString(ViewString::getValue("citywar_state_close_tip").c_str());
        m_pGroupWarStateTime->setVisible(false);
    }
    else if (GroupWarMgr::getSingleton().getWarState() == GROUPWAR_STATE_SIGNUP) {
        
        m_pGroupWarStateTip->setString(ViewString::getValue("citywar_state_signup_end_tip").c_str());
    }
    else if (GroupWarMgr::getSingleton().getWarState() == GROUPWAR_STATE_BATTLE_PREPARE) {
        m_pGroupWarStateTip->setString(ViewString::getValue("citywar_state_start_time_tip").c_str());
    }
    else if (GroupWarMgr::getSingleton().getWarState() == GROUPWAR_STATE_BATTLE_START) {
        m_pGroupWarStateTip->setString(ViewString::getValue("citywar_state_end_time_tip").c_str());
    }
    else if (GroupWarMgr::getSingleton().getWarState() == GROUPWAR_STATE_BATTLE_SCORE) {
        m_pGroupWarStateTip->setString(ViewString::getValue("citywar_state_score_tip").c_str());
        m_pGroupWarStateTime->setVisible(false);
    }
    else {
        
        if (GroupWarMgr::getSingleton().m_warNextTime > time) {
            m_pGroupWarStateTip->setString(ViewString::getValue("citywar_state_countdown_tip").c_str());
        }
        else {
            m_pGroupWarStateTip->setString(ViewString::getValue("citywar_state_close_tip").c_str());
            m_pGroupWarStateTime->setVisible(false);
        }
    }
    
    if (GroupWarMgr::getSingleton().isWarStateOpen()) {
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("clanwar_events_2");
        m_pBtnGroupWar->getChildByTag(1)->setVisible(true);
        ((CCSprite*)m_pBtnGroupWar->getChildByTag(2))->setDisplayFrame(pFrame);
    }
    else {
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("clanwar_events_3");
        m_pBtnGroupWar->getChildByTag(1)->setVisible(false);
        ((CCSprite*)m_pBtnGroupWar->getChildByTag(2))->setDisplayFrame(pFrame);
    }
    
    m_pBtnCityWar->setVisible(false);
    if ((ClanWarMgr::getSingleton().isWarStateBattle() && ClanWarMgr::getSingleton().m_warSignupCityId != 0) || GroupWarMgr::getSingleton().isWarStateBattle()) {
        m_pBtnCityWar->setVisible(true);
    }
}

bool ClanWarMapUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    m_bProcScaleAni = false;
    m_pCityMenuUI->setVisible(false);
    return true;
}

void ClanWarMapUI::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

void ClanWarMapUI::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    m_bProcScaleAni = true;
    
    if (m_pClanWarTipUI) {
        GuideMgr::getSingleton().removeGuideUI();
        ImgMgr::getSingleton().removeRes("game_npc");
    }
}

void ClanWarMapUI::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
    m_bProcScaleAni = true;
}

void ClanWarMapUI::menuHelpCallback(CCObject* pSender, CCControlEvent event)
{
    int tag = ((CCNode*)pSender)->getTag();
    showClanWarHelpUI(tag);
}

void ClanWarMapUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    if (m_pClanWarTipUI) {
        GuideMgr::getSingleton().removeGuideUI();
        ImgMgr::getSingleton().removeRes("game_npc");
    }
    
    PlayerScene::getSingleton().m_pGameWorld->hideClanWarMapUI();
}

void ClanWarMapUI::menuCityCallback(CCObject* pSender, ClanCityUI* pCityNode)
{
    if ((m_pCityMenuUI->isVisible() && isTouchInside(((CCControlButton*)pSender)->getTouchPosition(), m_pCityMenuUI->m_pView)) || m_pMapView->isDragging()) {
        return;
    }
    
    if (ClanMgr::getSingleton().m_myclan.id == 0) {
        MessageUI::show(MSG_NORMAL, NULL, ViewString::getValue("citywar_no_clan_tip").c_str());
        return;
    }
    
    ClanWarMgr::getSingleton().m_pSelectCityInfo = pCityNode->m_pCityInfo;
    GroupWarMgr::getSingleton().m_pSelectCityInfo = pCityNode->m_pCityInfo;
    
    CCSprite* pArrow = (CCSprite*)m_pCityMenuUI->m_pView->getChildByTag(1);
    
    CCPoint cityPos = pCityNode->getPosition();
    cityPos.y += pCityNode->getContentSize().height/2;
    
    CCPoint viewPos = ccpAdd(m_pMapView->getContentOffset(), ccpMult(cityPos, m_pMapView->getZoomScale()));
    CCSize viewSize = this->getContentSize();
    
    CCSize menuSize = m_pCityMenuUI->getContentSize();
    
    CCSize arrowSize = pArrow->getContentSize();
    
    cityPos = viewPos;
    
    float offx = arrowSize.width + 40;
    
    pArrow->setFlipX(false);
    pArrow->setPositionX(0);
    
    if (viewPos.x + offx + menuSize.width > viewSize.width) {
        viewPos.x -= menuSize.width;
        
        pArrow->setFlipX(true);
        pArrow->setPositionX(menuSize.width+arrowSize.width);
        
        offx = -offx + arrowSize.width;
    }
    else if (viewPos.x < 0) {
        viewPos.x = 0;
    }
    
    viewPos.x += offx;
    
    if (viewPos.y + menuSize.height/2 > viewSize.height) {
        viewPos.y = viewSize.height - menuSize.height/2;
    }
    else if (viewPos.y < menuSize.height/2) {
        viewPos.y = menuSize.height/2;
    }
    
    float arrowY = menuSize.height/2 + (cityPos.y - viewPos.y);
    if (arrowY > menuSize.height - arrowSize.height) {
        arrowY = menuSize.height - arrowSize.height;
    }
    else if (arrowY < 0){
        arrowY = arrowSize.height;
    }
    
    pArrow->setPositionY(arrowY);
    
    m_pCityMenuUI->setPosition(viewPos);
    m_pCityMenuUI->setVisible(true);
    
    m_pCityMenuUI->setCityInfo(ClanWarMgr::getSingleton().m_pSelectCityInfo);
}

void ClanWarMapUI::menuCityWarCallback(CCObject* pSender, ClanCityUI* pCityNode)
{
    if (ClanMgr::getSingleton().m_myclan.id <= 0) {
        MessageUI::show(MSG_NORMAL, NULL, ViewString::getValue("citywar_no_clan_tip").c_str());
        return;
    }
    
    if (ClanWarMgr::getSingleton().isWarStateBattle() && ClanWarMgr::getSingleton().m_warSignupCityId != 0) {
        
        for (int i = 0; i < ClanWarMgr::getSingleton().m_warCityList.size(); i++) {
            
            ClanCityInfo* pCityInfo = &ClanWarMgr::getSingleton().m_warCityList[i];
         
            if (pCityInfo->cityid == ClanWarMgr::getSingleton().m_warSignupCityId) {
                
                ClanWarMgr::getSingleton().m_pSelectCityInfo = pCityInfo;
                break;
            }
        }
        
        PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->showClanWarBattleUI();
    }
    else if (GroupWarMgr::getSingleton().isWarStateBattle()) {
        
        for (int i = 0; i < ClanWarMgr::getSingleton().m_warCityList.size(); i++) {
            
            ClanCityInfo* pCityInfo = &ClanWarMgr::getSingleton().m_warCityList[i];
            
            if (pCityInfo->citytype == GROUPWAR_CITY) {
                
                GroupWarMgr::getSingleton().m_pSelectCityInfo = pCityInfo;
                break;
            }
        }
        
        PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->showGroupWarBattleUI();
    }
}

void ClanWarMapUI::showClanWarBattleUI()
{
    if (m_pClanWarBattleUI == NULL) {
        m_pClanWarBattleUI = ClanWarBattleUI::create(ClanWarMgr::getSingleton().m_pSelectCityInfo);
        this->addChild(m_pClanWarBattleUI);
    }
    
    m_pMapView->setVisible(false);
    m_pCityMenuUI->setVisible(false);
}

void ClanWarMapUI::hideClanWarBattleUI()
{
    this->removeChild(m_pClanWarBattleUI);
    m_pClanWarBattleUI = NULL;
    
    m_pMapView->setVisible(true);
}

void ClanWarMapUI::showClanWarSignupUI()
{
    ClanWarMgr::getSingleton().m_warAttackClanList.clear();
    
    if (m_pClanWarSignupUI == NULL) {
        m_pClanWarSignupUI = ClanWarSignupUI::create(ClanWarMgr::getSingleton().m_pSelectCityInfo);
        this->addChild(m_pClanWarSignupUI);
    }
    
    m_pCityMenuUI->setVisible(false);
}

void ClanWarMapUI::hideClanWarSignupUI()
{
    this->removeChild(m_pClanWarSignupUI);
    m_pClanWarSignupUI = NULL;
}

void ClanWarMapUI::showGroupWarBattleUI()
{
    if (m_pGroupWarBattleUI == NULL) {
        m_pGroupWarBattleUI = GroupWarBattleUI::create(GroupWarMgr::getSingleton().m_pSelectCityInfo);
        this->addChild(m_pGroupWarBattleUI);
    }
    
    m_pMapView->setVisible(false);
    m_pCityMenuUI->setVisible(false);
}

void ClanWarMapUI::hideGroupWarBattleUI()
{
    this->removeChild(m_pGroupWarBattleUI);
    m_pGroupWarBattleUI = NULL;
    
    m_pMapView->setVisible(true);
}

void ClanWarMapUI::showClanWarHelpUI(int type)
{
    if (m_pClanWarHelpUI == NULL) {
        m_pClanWarHelpUI = ClanWarHelpUI::create(type);
        this->addChild(m_pClanWarHelpUI);
    }
}

void ClanWarMapUI::hideClanWarHelpUI()
{
    this->removeChild(m_pClanWarHelpUI);
    m_pClanWarHelpUI = NULL;
}

bool ClanWarMapUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnCityBattle", CCControlButton*, m_pBtnClanWar);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CityBattleStateTip", CCLabelBMFont*, m_pClanWarStateTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CityBattleStateTime", CCLabelBMFont*, m_pClanWarStateTime);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnCityInvasion", CCControlButton*, m_pBtnGroupWar);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CityInvasionStateTip", CCLabelBMFont*, m_pGroupWarStateTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CityInvasionStateTime", CCLabelBMFont*, m_pGroupWarStateTime);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "MapView", CCScrollView*, m_pMapView);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnCityWar", CCControlButton*, m_pBtnCityWar);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnClose", CCControlButton*, m_pBtnClose);
    
    return false;
}

SEL_CCControlHandler ClanWarMapUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuHelpCallback", ClanWarMapUI::menuHelpCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", ClanWarMapUI::menuCloseCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCityWarCallback", ClanWarMapUI::menuCityWarCallback);
    
    return NULL;
}

ClanCityUI::ClanCityUI(ClanCityInfo* pCityInfo)
: m_pCityInfo(pCityInfo)
{
    
}

ClanCityUI::~ClanCityUI()
{
    
}

ClanCityUI* ClanCityUI::create(ClanCityInfo* info)
{
    ClanCityUI *pRet = new ClanCityUI(info);
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

bool ClanCityUI::init()
{
    if (CUINode::init()) {
        CCNode* pCityNode = NULL;
        if (m_pCityInfo->citytype == 1) {
            pCityNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_city.ccbi", this);
        }
        else if (m_pCityInfo->citytype == 2) {
            pCityNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("groupwar_city.ccbi", this);
        }
        
        this->setContentSize(pCityNode->getContentSize());
        this->addChild(pCityNode);
        
        m_pCityName->setString(m_pCityInfo->cityname.c_str());
        
        char str[128];
        sprintf(str, "nationalflag_%02d", m_pCityInfo->clanInfo.national);
        CCAction* pFlagAction = actionWithAnimate(str, 5, 0.12f, true);
        m_pCityFlag->runAction(pFlagAction);
        
        sprintf(str, "clan_icon_%d", m_pCityInfo->clanInfo.flag);
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pClanIcon->setDisplayFrame(pFrame);
        
        m_pClanName->setString(m_pCityInfo->clanInfo.name.c_str());
        
        m_pClanWarState->setVisible(false);
        if (m_pCityInfo->citytype == CLANWAR_CITY) {
            
            if (ClanWarMgr::getSingleton().m_warSignupCityId == m_pCityInfo->cityid) {
                
                if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_SIGNUP) {
                    sprintf(str, "clanbattle_mode01");
                    m_pClanWarState->setVisible(true);
                }
                else if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_BATTLE_PREPARE) {
                    sprintf(str, "clanbattle_mode05");
                    m_pClanWarState->setVisible(true);
                }
                else if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_BATTLE_START) {
                    sprintf(str, "clanbattle_mode02");
                    m_pClanWarState->setVisible(true);
                }
            }
            else if (m_pCityInfo->clanInfo.id == ClanMgr::getSingleton().m_myclan.id) {
                
                if (m_pCityInfo->dayrewardid > 0) {
                    sprintf(str, "clanbattle_mode04");
                    m_pClanWarState->setVisible(true);
                }
                else if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_BATTLE_PREPARE) {
                    sprintf(str, "clanbattle_mode05");
                    m_pClanWarState->setVisible(true);
                }
                else if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_BATTLE_START) {
                    sprintf(str, "clanbattle_mode03");
                    m_pClanWarState->setVisible(true);
                }
            }
        }
        else if (m_pCityInfo->citytype == GROUPWAR_CITY) {
            
            if (GroupWarMgr::getSingleton().isWarStateOpen()) {
                sprintf(str, "clanbattle_mode02");
                m_pClanWarState->setVisible(true);
            }
        }
        
        if (m_pClanWarState->isVisible()) {
            
            CCAction* pAnimate = actionWithAnimate(str, 6, 0.12f, true);
            m_pClanWarState->runAction(pAnimate);
        }
    }
    return true;
}

bool ClanCityUI::checkTouchInside(CCObject* pSender)
{
    ClanWarMapUI* pWarMapUI = PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI;
    
    if (isTouchInside(((CCControlButton*)pSender)->getTouchPosition(), pWarMapUI->m_pBtnClose)) {
        pWarMapUI->menuCloseCallback(pSender, CCControlEventTouchDown);
        return false;
    }
    
    return true;
}

void ClanCityUI::menuCityCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->menuCityCallback(pSender, this);
}

bool ClanCityUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CityName", CCLabelBMFont*, m_pCityName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CityFlag", CCSprite*, m_pCityFlag);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanWarState", CCSprite*, m_pClanWarState);
    
    return false;
}

SEL_CCControlHandler ClanCityUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCityCallback", ClanCityUI::menuCityCallback);

    return NULL;
}

ClanCityMenuUI::ClanCityMenuUI()
: m_pRewardInfoUI(NULL)
{
    
}

ClanCityMenuUI::~ClanCityMenuUI()
{
    
}

bool ClanCityMenuUI::init()
{
    if (CUINode::init()) {
        
        m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_cityinfo.ccbi", this);
        this->setContentSize(m_pView->getContentSize());
        this->addChild(m_pView);
    }
    
    return true;
}

void ClanCityMenuUI::setCityInfo(ClanCityInfo* pCityInfo)
{
    m_pCityInfo = pCityInfo;
    
    if (m_pCityInfo->citytype == CLANWAR_CITY) {
        m_pTitle->setString(ViewString::getValue("citybattle_title").c_str());
    }
    else if (m_pCityInfo->citytype == GROUPWAR_CITY) {
        m_pTitle->setString(ViewString::getValue("cityinvasion_title").c_str());
    }
    
    char str[128];
    sprintf(str, "clan_national_%d", pCityInfo->clanInfo.national);
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pClanNational->setDisplayFrame(pFrame);
    
    m_pNationalName->setString(national[pCityInfo->clanInfo.national].c_str());
    
    sprintf(str, "clan_icon_%d", pCityInfo->clanInfo.flag);
    pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pClanIcon->setDisplayFrame(pFrame);
    
    m_pClanName->setString(pCityInfo->clanInfo.name.c_str());
    
    sprintf(str, "%d", pCityInfo->wingems);
    m_pWinGems->setString(str);
    
    sprintf(str, "%d", pCityInfo->productgems);
    m_pProductGems->setString(str);
    
    m_pBtnSignup->setVisible(false);
    m_pBtnCheckBattle->setVisible(false);
    m_pBtnEnterBattle->setVisible(false);
    m_pBtnDailyGems->setVisible(false);
    
    if (m_pCityInfo->dayrewardid > 0) {
        m_pBtnDailyGems->setVisible(true);
    }
    
    if (m_pCityInfo->citytype == CLANWAR_CITY) {
        
        if (ClanWarMgr::getSingleton().getWarState() == CLANWAR_STATE_SIGNUP) {
            m_pBtnSignup->setVisible(true);
        }
        else if (ClanWarMgr::getSingleton().isWarStateBattle()){
            
            if (ClanWarMgr::getSingleton().m_warSignupCityId == pCityInfo->cityid) {
                m_pBtnEnterBattle->setVisible(true);
            }
            else {
                m_pBtnCheckBattle->setVisible(true);
            }
        }
        else {
            m_pBtnCheckBattle->setVisible(true);
        }
    }
    else {
        if (GroupWarMgr::getSingleton().isWarStateOpen()) {
            m_pBtnEnterBattle->setVisible(true);
        }
        else {
            m_pBtnCheckBattle->setVisible(true);
        }
    }
}

void ClanCityMenuUI::menuClanInfoCallback(CCObject* pSender, CCControlEvent event)
{
    ClanMgr::getSingleton().m_otherclan = m_pCityInfo->clanInfo;
    PlayerScene::getSingleton().m_pGameWorld->showClanInfoUI();
}

void ClanCityMenuUI::menuSignupCallback(CCObject *pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->showClanWarSignupUI();
}

void ClanCityMenuUI::menuBattleCallback(CCObject *pSender, CCControlEvent event)
{
    if (m_pCityInfo->citytype == 1) {
        PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->showClanWarBattleUI();
    }
    else if (m_pCityInfo->citytype == 2) {
        PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->showGroupWarBattleUI();
    }
}

void ClanCityMenuUI::menuDailyGemsCallback(CCObject *pSender, CCControlEvent event)
{
    LogicInfo::getSingleton().chgGem(m_pCityInfo->productgems);
    
    if (m_pCityInfo->citytype == CLANWAR_CITY) {
        HttpClient::getSingleton().getClanWarRewardGems(m_pCityInfo->dayrewardid);
    }
    else if (m_pCityInfo->citytype == GROUPWAR_CITY) {
        HttpClient::getSingleton().getGroupWarRewardGems(m_pCityInfo->dayrewardid, 0);
    }
    
    m_pCityInfo->dayrewardid = 0;
    
    this->setVisible(false);
}

void ClanCityMenuUI::menuRewardInfoCallback(CCObject *pSender, CCControlEvent event)
{
    if (m_pRewardInfoUI == NULL) {
        
        if (m_pCityInfo->citytype == CLANWAR_CITY) {
            m_pRewardInfoUI = CUINode::createWithFile("clanwar_reward.ccbi", this);
            m_pRewardInfoUI->setTargetTouch(true);
            PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->addChild(m_pRewardInfoUI);
        }
        else if (m_pCityInfo->citytype == GROUPWAR_CITY) {
            m_pRewardInfoUI = CUINode::createWithFile("groupwar_reward.ccbi", this);
            m_pRewardInfoUI->setTargetTouch(true);
            PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->addChild(m_pRewardInfoUI);
        }
    }
}

void ClanCityMenuUI::menuCloseRewardInfoCallback(CCObject *pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->removeChild(m_pRewardInfoUI);
    m_pRewardInfoUI = NULL;
}

bool ClanCityMenuUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Title", CCLabelBMFont*, m_pTitle);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanNational", CCSprite*, m_pClanNational);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "NationalName", CCLabelBMFont*, m_pNationalName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WinGems", CCLabelBMFont*, m_pWinGems);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ProductGems", CCLabelBMFont*, m_pProductGems);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnSignup", CCControlButton*, m_pBtnSignup);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnEnterBattle", CCControlButton*, m_pBtnEnterBattle);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnCheckBattle", CCControlButton*, m_pBtnCheckBattle);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnDailyGems", CCControlButton*, m_pBtnDailyGems);
    
    return false;
}

SEL_CCControlHandler ClanCityMenuUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuClanInfoCallback", ClanCityMenuUI::menuClanInfoCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSignupCallback", ClanCityMenuUI::menuSignupCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBattleCallback", ClanCityMenuUI::menuBattleCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuDailyGemsCallback", ClanCityMenuUI::menuDailyGemsCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuRewardInfoCallback", ClanCityMenuUI::menuRewardInfoCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseRewardInfoCallback", ClanCityMenuUI::menuCloseRewardInfoCallback);
    
    return NULL;
}