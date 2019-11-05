//
//  GroupWarScoreUI.cpp
//  GameClient
//
//  Created by qsun on 14-10-30.
//
//

#include "GroupWarScoreUI.h"
#include "../coc/ClanMgr.h"
#include "../coc/GroupWarMgr.h"
#include "../coc/SoundMgr.h"
#include "../coc/HttpClient.h"
#include "../coc/LogicInfo.h"
#include "../coc/PlayerScene.h"
#include "../GameWorldScene.h"

GroupWarScoreUI::GroupWarScoreUI()
: m_pView(NULL)
{
    
}

GroupWarScoreUI::~GroupWarScoreUI()
{
}

bool GroupWarScoreUI::init()
{
    if (CUINode::init()) {
        
        CCNode* pScoreUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("groupwar_score.ccbi", this);
        
        this->setContentSize(pScoreUI->getContentSize());
        this->setTargetTouch(true);
        this->addChild(pScoreUI);
        
        refreshInfo();
    }
    
    return true;
}

void GroupWarScoreUI::refreshInfo()
{
    if(FacebookMgr::getSingleton().isShare() == false) {
        m_pBtnFBShare->setVisible(false);
    }
    
    char str[128];
    
    sprintf(str, "%d", GroupWarMgr::getSingleton().m_warRanking);
    ((CCLabelBMFont*)m_pWarRanking->getChildByTag(1))->setString(str);

    sprintf(str, "%d", GroupWarMgr::getSingleton().m_warStar);
    ((CCLabelBMFont*)m_pWarStar->getChildByTag(1))->setString(str);
    
    m_pRankingRewardGems->setVisible(false);
    if (GroupWarMgr::getSingleton().m_warRankingGems > 0) {
        m_pRankingRewardGems->setVisible(true);
        
        sprintf(str, "%d", GroupWarMgr::getSingleton().m_warRankingGems);
        ((CCLabelBMFont*)m_pRankingRewardGems->getChildByTag(1))->setString(str);
    }
    
    m_pStarRewardGems->setVisible(false);
    if (GroupWarMgr::getSingleton().m_warStarGems > 0) {
        m_pStarRewardGems->setVisible(true);
        
        sprintf(str, "%d", GroupWarMgr::getSingleton().m_warStarGems);
        ((CCLabelBMFont*)m_pStarRewardGems->getChildByTag(1))->setString(str);
    }

    m_pDailyGemTip->setVisible(false);
    if (GroupWarMgr::getSingleton().m_warRanking == 1) {
        m_pDailyGemTip->setVisible(true);
    }
    
}

bool GroupWarScoreUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    return true;
}

void GroupWarScoreUI::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

void GroupWarScoreUI::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
}

void GroupWarScoreUI::menuBtnFBShareCallback(CCObject* pSender, CCControlEvent event)
{
//    CCControlButton *pBtn = (CCControlButton *)pSender;
//    pBtn->setVisible(false);//分享时隐藏分享按钮
//    FacebookMgr::getSingleton().fbShare();
//    pBtn->setVisible(true);
}

void GroupWarScoreUI::menuOkCallback(CCObject* pSender, CCControlEvent event)
{
    if (GroupWarMgr::getSingleton().m_warRankingGems > 0) {
        LogicInfo::getSingleton().addGem(GroupWarMgr::getSingleton().m_warRankingGems);
    }
    if (GroupWarMgr::getSingleton().m_warStarGems > 0) {
        LogicInfo::getSingleton().addGem(GroupWarMgr::getSingleton().m_warStarGems);
    }
    
    HttpClient::getSingleton().getGroupWarRewardGems(GroupWarMgr::getSingleton().m_warRewardId, GroupWarMgr::getSingleton().m_warStarRewardId);
    
    PlayerScene::getSingleton().m_pGameWorld->hideGroupWarScoreUI();
}

bool GroupWarScoreUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CityName", CCLabelBMFont*, m_pCityName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Title", CCLabelBMFont*, m_pTitle);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Message", CCLabelBMFont*, m_pMessage);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ResultTip", CCLabelBMFont*, m_pResultTip);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DailyGemTip", CCLabelBMFont*, m_pDailyGemTip);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WarRanking", CCSprite*, m_pWarRanking);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WarStar", CCSprite*, m_pWarStar);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankingRewardGems", CCSprite*, m_pRankingRewardGems);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "StarRewardGems", CCSprite*, m_pStarRewardGems);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnFBShare", CCControlButton*, m_pBtnFBShare);
    
    return false;
}

SEL_CCControlHandler GroupWarScoreUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuOkCallback", GroupWarScoreUI::menuOkCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBtnFBShareCallback", GroupWarScoreUI::menuBtnFBShareCallback);
    
    return NULL;
}

void GroupWarScoreUI::completedAnimationSequenceNamed(const char *name)
{
    
}