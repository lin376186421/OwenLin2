//
//  ScoreUI.cpp
//  coc4
//
//  Created by chowray on 13-4-11.
//
//

#include "ScoreUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/SoundMgr.h"
#include "../coc/PlayerScene.h"
#include "../coc/GuideMgr.h"
#include "../coc/HeroMgr.h"
#include "../coc/ClanMgr.h"
#include "CAppPlatform.h"
#include "CUserData.h"

using namespace cocos2d;
using namespace coc;

ScoreUI* ScoreUI::create(int type)
{
    ScoreUI *pRet = new ScoreUI(type);
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

ScoreUI::ScoreUI(int type)
{
    m_type = type;
}

ScoreUI::~ScoreUI()
{
    
}

bool ScoreUI::init()
{
    bool bRet = false;
    
    do{
        
        CC_BREAK_IF(!CUINode::init());
        {
            CCNode* scoreUI = NULL;
            
            if (m_type == SCORE_TYPE_ATTACK) {
                scoreUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("fight_result.ccbi", this);
                m_pBtnFBShare->setVisible(false);
                if(LogicInfo::getSingleton().m_attackType != ATTACK_TYPE_PVE && LogicInfo::getSingleton().m_curWarStar>0)
                {
                    if(FacebookMgr::getSingleton().isShare())
                        m_pBtnFBShare->setVisible(true);
                }
                
            }else if (m_type == SCORE_TYPE_REPLAY) {
                scoreUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("replay_result.ccbi", this);
            }
            
            this->setContentSize(scoreUI->getContentSize());
            this->setTargetTouch(true);
            this->addChild(scoreUI);
            
            char str[128];
            FightScoreInfoEx3 info = LogicInfo::getSingleton().m_infoFightScore;
            
            if (m_type == SCORE_TYPE_ATTACK) {
                SoundMgr::getSingleton().stopBackgroundMusic();
                if(LogicInfo::getSingleton().m_curWarStar > 0)
                {
                    m_pBattleResult->setString("Victory");
                    SoundMgr::getSingleton().playBackgroundMusic("win.mp3");
                }
                else
                {
                    m_pBattleResult->setString("Defeat");
                    SoundMgr::getSingleton().playBackgroundMusic("lose.mp3");
                }
            }
            else if (m_type == SCORE_TYPE_REPLAY) {
                
                m_pRivalName->setString(info.user[1].name);
                
                sprintf(str, "%d", info.user[1].gold);
                m_pRivalGold->setString(str);
                
                sprintf(str, "%d", info.user[1].oil);
                m_pRivalElixir->setString(str);
                
                m_pPlayerName->setString(info.user[0].name);
                
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
                
            }
            
            //Controll by league
            if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_LEAGUE)
            {
                if (LogicInfo::getSingleton().m_curWarStar > 0)
                {
                    LeagueMgr::ResInfo resInfo = LeagueMgr::getSingleton().getCurBattleAwardInfo(LogicInfo::getSingleton().m_curWarStar);
                    for (int i = 0; i<resInfo.types; i++)
                    {
                        int type = resInfo.resType[i];
                        int num  = resInfo.num[i];
                        sprintf(str, "%d",num);
                        if (type == MONEY_GOLD)
                        {
                            m_pGainGold->setString(str);
                        }
                        else if (type == MONEY_ELIXIR)
                        {
                            m_pGainElixir->setString(str);
                        }
                    }
                }
                else
                {
                    m_pGainGold->setString("0");
                    m_pGainElixir->setString("0");
                }
                
                m_pGainCup->setVisible(false);
                m_pCupIcon->setVisible(false);
            }
            else
            {
                sprintf(str, "%d", (int)info.goldex);
                m_pGainGold->setString(str);
                
                sprintf(str, "%d", (int)info.oilex);
                m_pGainElixir->setString(str);
                
                sprintf(str, "%d", LogicInfo::getSingleton().m_winCups);
                m_pGainCup->setString(str);
            }
           
            CCScrollView* pScrollView = CCScrollView::create(m_pTroopsView->getContentSize());
            pScrollView->setDirection(kCCScrollViewDirectionHorizontal);
            
            float scale = 0.5;
            CCSize soldierSize = ImgMgr::getSingleton().getFrame("player1_101")->getRect().size/2;
            float posX = 0;
            if (soldierSize.width * LogicInfo::getSingleton().m_lstUsedFightSoldier.size() < m_pTroopsView->getContentSize().width) {
                pScrollView->setTouchEnabled(false);
                posX = (m_pTroopsView->getContentSize().width - soldierSize.width*LogicInfo::getSingleton().m_lstUsedFightSoldier.size())/2;
            }
            
            int i = 0;
            for(std::list<FightSoldierInfoEx>::iterator it = LogicInfo::getSingleton().m_lstUsedFightSoldier.begin(); it != LogicInfo::getSingleton().m_lstUsedFightSoldier.end(); ++it)
            {
                if((it->pid / 10000) == 3)
                {
                    sprintf(str, "player1_%d", (int)(it->pid / 100));
                    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
                    
                    CCControlButton* pSoldier = CCControlButton::create(CCScale9Sprite::createWithSpriteFrame(pFrame));
                    pSoldier->setZoomOnTouchDown(false);
                    pSoldier->setAnchorPoint(ccp(0, 0));
                    pSoldier->setPreferredSize(pFrame->getRect().size);
                    pSoldier->setScale(scale);
                    float y = (m_pTroopsView->getContentSize().height-soldierSize.height)/2;
                    pSoldier->setPosition(ccp(posX + soldierSize.width*i, y));
                    pScrollView->addChild(pSoldier);
                    ++i;
                    continue;
                }
                
                if (it->pid == CLAN_SOLDIER_ID) {
                    sprintf(str, "player0_0");
                }
                else {
                    sprintf(str, "player1_%d", (int)(it->pid / 100));
                }
                CCSprite* pSoldier = ImgMgr::getSingleton().loadSprite(str);
                pSoldier->setAnchorPoint(ccp(0, 0));
                pSoldier->setScale(scale);
                float y = (m_pTroopsView->getContentSize().height-soldierSize.height)/2;
                pSoldier->setPosition(ccp(posX + soldierSize.width*i, y));
                pScrollView->addChild(pSoldier);
                
                if(it->pid < 30000)
                {
                    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("player_star");
                    
                    int level = it->pid % 100;
                    for(int i = 0; i < level; ++i)
                    {
                        CCSprite* pStar = CCSprite::createWithSpriteFrame(pFrame);
                        pStar->setAnchorPoint(ccp(0, 0));
                        pStar->setPosition(ccp(8 + 17 * i, 50));
                        pSoldier->addChild(pStar, 10);
                    }
                    
                    int nums = it->numsex;
                    sprintf(str, "x%d", nums);
                    
                    CCLabelBMFont* pNums = CCLabelBMFont::create(str, "Font/tbFont_b.fnt");
                    pNums->setTag(1);
                    pNums->setPosition(ccp(30, 25));
                    pNums->setScale(0.5);
                    pSoldier->addChild(pNums);
                }
                else if (it->pid == CLAN_SOLDIER_ID) {
                    
                    sprintf(str, "clan_icon_%d", info.user[0].clanflag);
                    CCSprite* pClanIcon = ImgMgr::getSingleton().loadSprite(str);
                    pClanIcon->setPosition(ccp(pSoldier->getContentSize().width/2, pSoldier->getContentSize().height/2));
                    pSoldier->addChild(pClanIcon);
                }
                
                ++i;
            }
            
            m_pTroopsView->addChild(pScrollView);
            
            if (soldierSize.width * LogicInfo::getSingleton().m_lstUsedFightSoldier.size() < m_pTroopsView->getContentSize().width) {
                pScrollView->setTouchEnabled(false);
            }
            
            for (int star = 1; star <= 3; star ++) {
                if (LogicInfo::getSingleton().m_curWarStar >= star)
                {
                    CCSpriteFrame* spriteFrame = ImgMgr::getSingleton().getFrame("star_b_1");
                    CCSprite* pStar = (CCSprite*)m_pBattleStars->getChildByTag(star);
                    pStar->setDisplayFrame(spriteFrame);
                    
                    CCScaleTo* action1 = CCScaleTo::create(0.3f, pStar->getScale()+0.5);
                    CCScaleTo* action2 = CCScaleTo::create(0.3f, pStar->getScale());
                        
                    CCActionInterval* action = CCSequence::create(action1, action2, NULL);
                    pStar->runAction(action);
                }
            }
            
            sprintf(str, "%d%%", (int)(info.progress * 100));
            m_pBattleProgress->setString(str);
            
            if (m_type == SCORE_TYPE_ATTACK && HeroMgr::getSingleton().m_myHero.heroid != 0) {
                sprintf(str, "+%d", HeroMgr::getSingleton().m_curAttackExp);
                m_pHeroExp->setString(str);
                
                sprintf(str, "ui_main_icon_%d", HeroMgr::getSingleton().m_myHero.heroid/100);
                CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
                
                m_pHeroIcon->setDisplayFrame(pFrame);
            }
            
            if(m_type == SCORE_TYPE_ATTACK && HeroMgr::getSingleton().m_myHero.heroid == 0) {
                m_pHeroNode->setVisible(false);
            }
            
            if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_PVE ||
                LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_CLANWAR ||
                LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_GROUPWAR) {
                m_pCupIcon->setVisible(false);
                m_pGainCup->setVisible(false);
            }
            
            if (GuideMgr::getSingleton().isGuiding()) {
                m_pHeroNode->setVisible(false);
                
                m_pCupIcon->setVisible(false);
                m_pGainCup->setVisible(false);
            }
        
            if (m_type == SCORE_TYPE_REPLAY) {
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
            }
            
            GuideMgr::getSingleton().m_pActiveBtn = NULL;

            bRet = true;
        }
    }
    while (0);
    
    return bRet;
}

void ScoreUI::menuReplayCallback(CCObject *pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().onStopReplay();
    LogicInfo::getSingleton().onBeginReplay();
    
    PlayerScene::getSingleton().m_pGameWorld->hideScoreUI();
    PlayerScene::getSingleton().m_pGameWorld->showReplayUI();
}

// a selector callback
void ScoreUI::menuBackCallback(CCObject* pSender, CCControlEvent event)
{
    if (m_type == SCORE_TYPE_REPLAY) {
        LogicInfo::getSingleton().stopReplay();
    }

	PlayerScene::getSingleton().m_pGameWorld->hideScoreUI();
    
    PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_BACKHOME);
    
    LogicInfo::getSingleton().goHome();
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    if(LogicInfo::getSingleton().m_curWarStar == 3 && GuideMgr::getSingleton().isGuideEnd())
    {
        if(!CCUserDefault::sharedUserDefault()->getBoolForKey(RATE_FREE_GEMS))
        {
//5星评分
//            MessageUI::show(MSG_RATE, ViewString::getSingleton().getValue("rate_tips"),       PlayerScene::getSingleton().m_pGameWorld, -1);
//            CAppPlatform::sharedAppPlatform()->rateApp();
        }
    }
#endif
}

void ScoreUI::menuBtnFBShareCallback(CCObject* pSender, CCControlEvent event)
{
    FacebookMgr::getSingleton().m_pCurBtn = (CCControlButton *)pSender;
    FacebookMgr::getSingleton().m_shareType = FBShare_Fight;
    PlayerScene::getSingleton().m_pGameWorld->showFBShareUI();
}

bool ScoreUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleStars", CCSprite*, m_pBattleStars);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleProgress", CCLabelBMFont*, m_pBattleProgress);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RivalName", CCLabelTTF*, m_pRivalName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RivalGold", CCLabelBMFont*, m_pRivalGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RivalElixir", CCLabelBMFont*, m_pRivalElixir);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerName", CCLabelTTF*, m_pPlayerName);
    
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RivalClanName", CCLabelTTF*, m_pRivalClanName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RivalClanIcon", CCSprite*, m_pRivalClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "AttackMilitaryIcon", CCSprite*, m_pAttackMilitaryIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DefendMilitaryIcon", CCSprite*, m_pDefendMilitaryIcon);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GainGold", CCLabelBMFont*, m_pGainGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GainElixir", CCLabelBMFont*, m_pGainElixir);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GainCup", CCLabelBMFont*, m_pGainCup);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleResult", CCLabelBMFont*, m_pBattleResult);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "HeroExp", CCLabelBMFont*, m_pHeroExp);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CupIcon", CCSprite*, m_pCupIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "HeroIcon", CCSprite*, m_pHeroIcon);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "HeroNode", CCNode*, m_pHeroNode);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TroopsView", CCLayer*, m_pTroopsView);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnFBShare", CCControlButton*, m_pBtnFBShare);
    
    
    return NULL;
}

SEL_CCControlHandler ScoreUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuReplayCallback", ScoreUI::menuReplayCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", ScoreUI::menuBackCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBtnFBShareCallback", ScoreUI::menuBtnFBShareCallback);
    
    return NULL;
}