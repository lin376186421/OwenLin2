//
//  ReplayUI.cpp
//  GameClient
//
//  Created by chowray on 13-4-27.
//
//

#include "ReplayUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/PlayerScene.h"
#include "../coc/SoundMgr.h"

using namespace cocos2d;
using namespace coc;

ReplayUI::ReplayUI()
{
    
}

ReplayUI::~ReplayUI()
{
    LogicInfo::getSingleton().removeListener(this);
}

bool ReplayUI::init()
{
    bool bRet = false;
    
    do{
        
        CC_BREAK_IF(!CUINode::init());
        
        LogicInfo::getSingleton().addListener(this);
        
        CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
        SoundMgr::getSingleton().playBackgroundMusic("fight_back.mp3");
        
        CCNode* replayNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("replay.ccbi", this);
        this->addChild(replayNode);
        
        m_half = false;
        m_starnum = 0;
        
        char str[128];
        FightScoreInfoEx3 info = LogicInfo::getSingleton().m_infoFightScore;
        
        {
            sprintf(str, "player_mr_%d",info.user[1].military);
            CCSpriteFrame *frame = ImgMgr::getSingleton().getFrame(str);
            m_pDefendMilitaryIcon->setDisplayFrame(frame);
        }
        
        {
            sprintf(str, "player_mr_%d",info.user[0].military);
            CCSpriteFrame *frame = ImgMgr::getSingleton().getFrame(str);
            m_pAttackMilitaryIcon->setDisplayFrame(frame);
        }
        
        m_pRivalName->setString(info.user[1].name);
        
        sprintf(str, "%d", info.user[1].gold);
        m_pRivalGold->setString(str);
        sprintf(str, "%d", info.user[1].oil);
        m_pRivalElixir->setString(str);
        
        m_pPlayerName->setString(info.user[0].name);
        
        int replayTime = PlayerScene::getSingleton().m_replayTime;
        m_pBattleTime->setString(formatTimeMs(replayTime).c_str());
        
        m_pClanName->setVisible(false);
        m_pClanIcon->setVisible(false);
        if (info.user[0].clanid > 0) {
            m_pClanName->setString(info.user[0].clanname);
            
            sprintf(str, "clan_icon_%d", info.user[0].clanflag);
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
            m_pClanIcon->setDisplayFrame(pFrame);
            
            m_pClanName->setVisible(true);
            m_pClanIcon->setVisible(true);
        }
        
        m_pRivalClanName->setVisible(false);
        m_pRivalClanIcon->setVisible(false);
        if (info.user[1].clanid > 0) {
            m_pRivalClanName->setString(info.user[1].clanname);
            
            sprintf(str, "clan_icon_%d", info.user[1].clanflag);
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
            m_pRivalClanIcon->setDisplayFrame(pFrame);
            
            m_pRivalClanName->setVisible(true);
            m_pRivalClanIcon->setVisible(true);
        }
        
        refreshInfo();
        
        schedule(schedule_selector(ReplayUI::updateBattleTime), 1.0f);
        
        bRet = true;
    }
    while (0);
    
    return bRet;
}

void ReplayUI::menuBackCallback(CCObject* pSender, CCControlEvent event)
{
    LogicInfo::getSingleton().stopReplay();
    
	PlayerScene::getSingleton().m_pGameWorld->hideReplayUI();
        
    PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_NORMAL);
    
	LogicInfo::getSingleton().goHome();
}

void ReplayUI::menuSpeedCallback(CCObject *pSender, CCControlEvent event)
{
    if(PlayerScene::getSingleton().m_speedReplay == 1)
        PlayerScene::getSingleton().setReplaySpeed(2);
    else if(PlayerScene::getSingleton().m_speedReplay == 2)
        PlayerScene::getSingleton().setReplaySpeed(4);
    else if(PlayerScene::getSingleton().m_speedReplay == 4)
        PlayerScene::getSingleton().setReplaySpeed(1);
    char str[128];
    sprintf(str, "X %d", PlayerScene::getSingleton().m_speedReplay);
    ((CCLabelBMFont*)m_pBtnReplaySpeed->getChildByTag(1))->setString(str);
}

void ReplayUI::refreshInfo()
{
    char str[128];
    FightScoreInfoEx3 info = LogicInfo::getSingleton().m_infoFightScore;
    
    sprintf(str, "%d", info.user[1].gold - LogicInfo::getSingleton().m_infoFightScore.goldex);
    m_pRivalGold->setString(str);
    sprintf(str, "%d", info.user[1].oil - LogicInfo::getSingleton().m_infoFightScore.oilex);
    m_pRivalElixir->setString(str);
    
    sprintf(str, "%d", (int)LogicInfo::getSingleton().m_infoFightScore.goldex);
    m_pGainGold->setString(str);
    
    sprintf(str, "%d", (int)LogicInfo::getSingleton().m_infoFightScore.oilex);
    m_pGainElixir->setString(str);
    
    sprintf(str, "%d%%", (int)(LogicInfo::getSingleton().m_infoFightScore.progress * 100));
    m_pBattleProgress->setString(str);
    
    if(!m_half && LogicInfo::getSingleton().m_infoFightScore.progress >= 0.5)
    {
        m_half = true;
        m_starnum ++;
        showStars();
    }
    else if(LogicInfo::getSingleton().m_infoFightScore.progress == 1)
    {
        m_starnum = 3;
        showStars();
    }
}

void ReplayUI::updateBattleTime(float dt)
{
    int replayTime = PlayerScene::getSingleton().m_replayTime;
    m_pBattleTime->setString(formatTimeMs(replayTime).c_str());
}

bool ReplayUI::onNotify(int notifyid, const void* pVal)
{
	if(NOTIFY_OTHERBASEINFO == notifyid)
	{
		refreshInfo();
	}
    else if(NOTIFY_DESTROY_HOME == notifyid)
    {
        m_starnum ++;
        showStars();
    }
    else if(NOTIFY_REPLAY_OK == notifyid)
    {
        PlayerScene::getSingleton().m_pGameWorld->hideReplayUI();
        PlayerScene::getSingleton().m_pGameWorld->showScoreUI();
    }
    
	return true;
}

void ReplayUI::showStars()
{
    if(m_starnum > 3)
        m_starnum = 3;
    
    for (int star = 1; star <= 3; star ++) {
        if (m_starnum >= star) {
            CCSpriteFrame* spriteFrame = ImgMgr::getSingleton().getFrame("star_b_1");
            CCSprite* pStar = (CCSprite*)m_pBattleStars->getChildByTag(star);
            pStar->setDisplayFrame(spriteFrame);
            
            if (m_starnum == star) {
                CCScaleTo* mv1 = CCScaleTo::create(0.3f, 2);
                CCScaleTo* mv2 = CCScaleTo::create(0.3f, 1);
                
                CCActionInterval* action1 = CCSequence::create(mv1, mv2, NULL);
                pStar->runAction(action1);
            }
        }
    }
}

bool ReplayUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RivalName", CCLabelTTF*, m_pRivalName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RivalGold", CCLabelBMFont*, m_pRivalGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RivalElixir", CCLabelBMFont*, m_pRivalElixir);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerName", CCLabelTTF*, m_pPlayerName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GainGold", CCLabelBMFont*, m_pGainGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GainElixir", CCLabelBMFont*, m_pGainElixir);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RivalClanName", CCLabelTTF*, m_pRivalClanName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RivalClanIcon", CCSprite*, m_pRivalClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "AttackMilitaryIcon", CCSprite*, m_pAttackMilitaryIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DefendMilitaryIcon", CCSprite*, m_pDefendMilitaryIcon);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleStars", CCSprite*, m_pBattleStars);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleProgress", CCLabelBMFont*, m_pBattleProgress);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnReplaySpeed", CCControlButton*, m_pBtnReplaySpeed);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleTime", CCLabelBMFont*, m_pBattleTime);
    
    return NULL;
}

SEL_CCControlHandler ReplayUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{

    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", ReplayUI::menuBackCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSpeedCallback", ReplayUI::menuSpeedCallback);
    
    return NULL;
}