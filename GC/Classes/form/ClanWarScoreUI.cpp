//
//  ClanWarScoreUI.cpp
//  GameClient
//
//  Created by qsun on 14-9-22.
//
//

#include "ClanWarScoreUI.h"
#include "../coc/ClanMgr.h"
#include "../coc/ClanWarMgr.h"
#include "../coc/SoundMgr.h"
#include "../coc/HttpClient.h"
#include "../coc/LogicInfo.h"
#include "../coc/PlayerScene.h"
#include "../GameWorldScene.h"

ClanWarScoreUI::ClanWarScoreUI()
: m_actionDelay(0)
, m_pView(NULL)
, m_pResultAnim(NULL)
{
    
}

ClanWarScoreUI::~ClanWarScoreUI()
{
    unschedule(schedule_selector(ClanWarScoreUI::actionUpdate));
}

bool ClanWarScoreUI::init()
{
    if (CUINode::init()) {
        
        if (ClanWarMgr::getSingleton().m_warWin) {
            m_pResultAnim = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_score_win.ccbi", this, &m_pAnimationManager);
            
            SoundMgr::getSingleton().playEffect("clanwar_win.mp3");
        }
        else {
            m_pResultAnim = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_score_lose.ccbi", this, &m_pAnimationManager);
            
            SoundMgr::getSingleton().playEffect("clanwar_lose.mp3");
        }
        
        m_pAnimationManager->setDelegate(this);
        this->setContentSize(m_pResultAnim->getContentSize());
        this->setTargetTouch(true);
        this->addChild(m_pResultAnim);
        
        char str[128];
        sprintf(str, "clan_icon_%d", ClanMgr::getSingleton().m_myclan.flag);
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        m_pClanIcon->setDisplayFrame(pFrame);
        
        m_pClanName->setString(ClanMgr::getSingleton().m_myclan.name.c_str());
        
        schedule(schedule_selector(ClanWarScoreUI::actionUpdate), 1.0f);
    }
    
    return true;
}

void ClanWarScoreUI::actionUpdate(float dt)
{
    m_actionDelay ++;
}

void ClanWarScoreUI::refreshInfo()
{
    char title[128];
    char message[128];
    if (ClanWarMgr::getSingleton().m_warWin) {
        
        sprintf(title, "%s", ViewString::getValue("battle_win_title").c_str());
        
        if (ClanWarMgr::getSingleton().m_warAttacker) {
            sprintf(message, ViewString::getValue("citywar_attack_win_message").c_str(), ClanWarMgr::getSingleton().m_warCityName.c_str());
        }
        else {
            sprintf(message, ViewString::getValue("citywar_defend_win_message").c_str(), ClanWarMgr::getSingleton().m_warCityName.c_str());
        }
        if(FacebookMgr::getSingleton().isShare() == false)
            m_pBtnFBShare->setVisible(false);
            
    }
    else {
        
        sprintf(title, "%s", ViewString::getValue("battle_lose_title").c_str());
        
        if (ClanWarMgr::getSingleton().m_warAttacker) {
            sprintf(message, ViewString::getValue("citywar_attack_lose_message").c_str(), ClanWarMgr::getSingleton().m_warCityName.c_str());
        }
        else {
            sprintf(message, ViewString::getValue("citywar_defend_lose_message").c_str(), ClanWarMgr::getSingleton().m_warCityName.c_str());
        }
        
        m_pBtnFBShare->setVisible(false);
    }
    
    m_pTitle->setString(title);
    m_pMessage->setString(message);

    char str[128];
    m_pWarRanking->setVisible(false);
    if (ClanWarMgr::getSingleton().m_warRanking > 0) {
        m_pWarRanking->setVisible(true);
        
        sprintf(str, "%d", ClanWarMgr::getSingleton().m_warRanking);
        ((CCLabelBMFont*)m_pWarRanking->getChildByTag(1))->setString(str);
    }
    
    m_pRewardGems->setVisible(false);
    if (ClanWarMgr::getSingleton().m_warRewardGems > 0) {
        m_pRewardGems->setVisible(true);
        
        sprintf(str, "%d", ClanWarMgr::getSingleton().m_warRewardGems);
        ((CCLabelBMFont*)m_pRewardGems->getChildByTag(1))->setString(str);
    }
}

bool ClanWarScoreUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    return true;
}

void ClanWarScoreUI::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

void ClanWarScoreUI::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    if (m_pResultAnim && m_actionDelay >= 3) {
        this->removeChild(m_pResultAnim);
        m_pResultAnim = NULL;
        
        m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("clanwar_score.ccbi", this);
        this->addChild(m_pView);

        refreshInfo();
    }
}
void ClanWarScoreUI::menuBtnFBShareCallback(CCObject* pSender, CCControlEvent event)
{
    FacebookMgr::getSingleton().m_pCurBtn = (CCControlButton *)pSender;
    FacebookMgr::getSingleton().m_shareType = FBShare_Clanwar;
    PlayerScene::getSingleton().m_pGameWorld->showFBShareUI();
}


void ClanWarScoreUI::menuOkCallback(CCObject* pSender, CCControlEvent event)
{
    if (ClanWarMgr::getSingleton().m_warRewardGems > 0) {
        LogicInfo::getSingleton().addGem(ClanWarMgr::getSingleton().m_warRewardGems);
    }
    
    HttpClient::getSingleton().getClanWarRewardGems(ClanWarMgr::getSingleton().m_warRewardId);
    
    PlayerScene::getSingleton().m_pGameWorld->hideClanWarScoreUI();
}

bool ClanWarScoreUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CityName", CCLabelBMFont*, m_pCityName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Title", CCLabelBMFont*, m_pTitle);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Message", CCLabelBMFont*, m_pMessage);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ResultTip", CCLabelBMFont*, m_pResultTip);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WarRanking", CCSprite*, m_pWarRanking);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RewardGems", CCSprite*, m_pRewardGems);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnFBShare", CCControlButton*, m_pBtnFBShare);
    
    return false;
}

SEL_CCControlHandler ClanWarScoreUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuOkCallback", ClanWarScoreUI::menuOkCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBtnFBShareCallback", ClanWarScoreUI::menuBtnFBShareCallback);
    
    return NULL;
}

void ClanWarScoreUI::completedAnimationSequenceNamed(const char *name)
{
   
}