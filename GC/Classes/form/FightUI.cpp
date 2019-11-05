//
//  FightUI.cpp
//  coc4
//
//  Created by chowray on 13-4-3.
//
//

#include "FightUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/HttpClient.h"
#include "../coc/LogicInfo.h"
#include "../coc/PlayerScene.h"
#include "../coc/GuideMgr.h"
#include "../coc/SoundMgr.h"
#include "../coc/HeroMgr.h"
#include "../coc/ClanMgr.h"
#include "../coc/SkillMgr.h"
#include "NotifyUI.h"


using namespace cocos2d;
using namespace coc;

FightUI::FightUI()
: m_bNeedRefurbish(false)
, m_i64RefurbishTime(0)
, m_pActGold(NULL)
, m_pActOil(NULL)
, m_isSkillOn(false)
{
    
    m_time1 = 0;
    m_time2 = 0;
    m_time3 = 0;
    
    m_pPT1 = NULL;
    m_pPT2 = NULL;
    m_pPT3 = NULL;
}

FightUI::~FightUI()
{
    LogicInfo::getSingleton().removeListener(this);
}

bool FightUI::init()
{
    bool bRet = false;
    
    do
    {
        CC_BREAK_IF(!CUINode::init());
        {
            
            LogicInfo::getSingleton().addListener(this);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
            
            CCNode* FightUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("fight.ccbi", this, &m_pAnimationManager);
            this->addChild(FightUI);
            
            if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_LEAGUE)
            {
                m_pCupNode->setVisible(false);
                m_pLootNode->setVisible(false);
            }
            else if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_CLANWAR ||
                     LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_GROUPWAR) {
                m_pCupNode->setVisible(false);
            }
            
            int findGold;
            if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_LEAGUE) {
                findGold = LeagueMgr::getSingleton().getSearchCost();
            }
            else {
                findGold = LogicInfo::getSingleton().getFindGold();
            }
            
            char str[128];
            sprintf(str, "%d", findGold);
            ((CCLabelBMFont*)m_pBtnFindMatch->getChildByTag(1))->setString(str);
            
            //init soldier
            CCScrollView* pScrollView = CCScrollView::create(m_pTroopsView->getContentSize());
            pScrollView->setDirection(kCCScrollViewDirectionHorizontal);
        
            m_pSoldierView = (CCLayer*)pScrollView->getContainer();
            
            CCSize oriSize = ImgMgr::getSingleton().getFrame("player1_101")->getRect().size;
            float scale = m_pTroopsView->getContentSize().height/oriSize.height;
            CCSize soldierSize(oriSize.width*scale, oriSize.height*scale);
            
            pScrollView->setContentSize(CCSizeMake(soldierSize.width*LogicInfo::getSingleton().m_lstFightSoldier.size(), m_pTroopsView->getContentSize().height));
                        
            int index = 0;
            list<FightSoldierInfoEx>::iterator it;
            for(it = LogicInfo::getSingleton().m_lstFightSoldier.begin(); it != LogicInfo::getSingleton().m_lstFightSoldier.end(); ++it)
            {
                sprintf(str, "player1_%d", (int)(it->pid / 100));
                CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
                
                CCControlButton* pSoldier = CCControlButton::create(CCScale9Sprite::createWithSpriteFrame(pFrame));
                pSoldier->setZoomOnTouchDown(false);
                pSoldier->setAnchorPoint(ccp(0, 0));
                pSoldier->setPreferredSize(pFrame->getRect().size);
                pSoldier->setScale(scale);
                pSoldier->setPosition(ccp(soldierSize.width * index, (m_pTroopsView->getContentSize().height-soldierSize.height)/2));
                
                pSoldier->setTag(it->pid);
                pSoldier->addTargetWithActionForControlEvents(this, cccontrol_selector(FightUI::menuSoldierCallback), CCControlEventTouchUpInside);
                
                CCSprite* pSelect = ImgMgr::getSingleton().loadSprite("player_select");
                pSelect->setAnchorPoint(ccp(0, 0));
                pSelect->setTag(0);
                pSelect->setVisible(false);
                pSelect->setZOrder(1);
                pSoldier->addChild(pSelect);
                
                int nums = it->numsex;
                sprintf(str, "x%d", nums);
                
                CCLabelBMFont* pNums = CCLabelBMFont::create(str, "Font/tbFont_b.fnt");
                pNums->setTag(1);
                pNums->setAnchorPoint(ccp(0, 0.5));
                pNums->setPosition(ccp(pSoldier->getPreferredSize().width*0.268, pSoldier->getPreferredSize().height*0.208));
                pNums->setScale(0.8);
                pNums->setZOrder(1);
                pSoldier->addChild(pNums);
                
                m_pSoldierView->addChild(pSoldier);
                
                if (it->pid < 30000) {
                    pFrame = ImgMgr::getSingleton().getFrame("player_star");
                    
                    int level = 1;
                    if(LogicInfo::getSingleton().getMySoldierLevel(it->pid) != 0){
                        level = (LogicInfo::getSingleton().getMySoldierLevel(it->pid)) % 10;
                    }
                    for(int i = 0; i < level; ++i)
                    {
                        CCSprite* pStar = CCSprite::createWithSpriteFrame(pFrame);
                        pStar->setAnchorPoint(ccp(0, 0.5));
                        pStar->setPosition(ccp(8 + 17 * i, pSoldier->getPreferredSize().height*0.396));
                        pSoldier->addChild(pStar, 10);
                    }
                }
                else{
                    m_pBtnHero->setTag(it->pid);
                }
                
                if (index == 0) {
                    LogicInfo::getSingleton().setSoldier2Fight(it->pid);
                    pSoldier->getChildByTag(0)->setVisible(true);
                    pSoldierSelected = pSoldier;
                }
                
                index++;
            }
            
            if(ClanMgr::getSingleton().m_myListSoldier.size() > 0)
            {
                m_pClanSoldierNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("fight_clan_item.ccbi", this);
                
                sprintf(str, "clan_icon_%d", ClanMgr::getSingleton().m_myclan.flag);
                CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
                
                CCSprite* pSprite = (CCSprite*)m_pBtnClanSoldier->getChildByTag(1);
                pSprite->setDisplayFrame(pFrame);
                
                m_pBtnClanSoldier->setScale(scale);
                m_pBtnClanSoldier->setTag(-1);
                
                m_pBtnClanSoldier->setPosition(ccp(soldierSize.width * index, (m_pTroopsView->getContentSize().height-soldierSize.height)/2));
                
                CCSprite* pSelect = ImgMgr::getSingleton().loadSprite("player_select");
                pSelect->setAnchorPoint(ccp(0, 0));
                pSelect->setTag(0);
                pSelect->setVisible(false);
                pSelect->setZOrder(1);
                m_pBtnClanSoldier->addChild(pSelect);
                
                m_pSoldierView->addChild(m_pClanSoldierNode);
            }
            
            CUINode* soldierView = CUINode::create(m_pTroopsView->getContentSize());
            soldierView->addChild(pScrollView);
            soldierView->setTargetTouch(true);
            m_pTroopsView->addChild(soldierView);
            
            if(GuideMgr::getSingleton().isEquVal(GUIDE_ATTACK_GOBLIN) || GuideMgr::getSingleton().isEquVal(GUIDE_RAVEN_GOBLIN))
            {
                m_pBtnEndBattle->setVisible(false);
                m_pBtnFindMatch->setVisible(false);
                
//                HeroMgr::getSingleton().myHero.skill0 = 1000100;
//                HeroMgr::getSingleton().myHero.skill1 = 1000200;
//                HeroMgr::getSingleton().myHero.skill2 = 1000300;
//                
//                HeroMgr::getSingleton().myHero.bskill[0] = 1;
//                HeroMgr::getSingleton().myHero.bskill[1] = 1;
//                HeroMgr::getSingleton().myHero.bskill[2] = 1;
                
                if (GuideMgr::getSingleton().isEquVal(GUIDE_ATTACK_GOBLIN)) {
                    m_pDropSoldierTip->setString(ViewString::getValue("guide_10").c_str());
                }
                else if (GuideMgr::getSingleton().isEquVal(GUIDE_RAVEN_GOBLIN)) {
                    m_pDropSoldierTip->setString(ViewString::getValue("guide_20").c_str());
                }
                
            }
            
            if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_PVE) {
                m_pBtnFindMatch->setVisible(false);
                
                m_pBattleTime->setVisible(false);
                m_pBattleTimeTip->setVisible(false);
                
                m_pCupNode->setVisible(false);
            }
            else if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_CLANWAR ||
                     LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_GROUPWAR) {
                m_pBtnFindMatch->setVisible(false);
            }
            
            {
                CCSpriteFrame* pFrame;
                char str[128];
                
                pFrame = ImgMgr::getSingleton().getFrame("ui_skill_back");
                
                m_pSkill1->setVisible(false);
                m_pSkill2->setVisible(false);
                m_pSkill3->setVisible(false);
                
                m_pSkill1->setBackgroundSpriteFrameForState(pFrame, CCControlStateNormal);
                m_pSkill2->setBackgroundSpriteFrameForState(pFrame, CCControlStateNormal);
                m_pSkill3->setBackgroundSpriteFrameForState(pFrame, CCControlStateNormal);
                
                m_pSkill1->setEnabled(false);
                m_pSkill2->setEnabled(false);
                m_pSkill3->setEnabled(false);
                if(HeroMgr::getSingleton().m_myHero.skill0 != 0)
                {
                    sprintf(str, "ui_skill_%d", HeroMgr::getSingleton().m_myHero.skill0 / 100);
                    pFrame = ImgMgr::getSingleton().getFrame(str);
                    setControlButtonFrameForAllState(m_pSkill1, pFrame);
                    
                    sprintf(str, "ui_skill_%d_0", HeroMgr::getSingleton().m_myHero.skill0 / 100);
                    CCSprite* pSprite = ImgMgr::getSingleton().loadSprite(str);
                    m_pPT1 = CCProgressTimer::create(pSprite);
                    m_pPT1->setPercentage(0);
                    m_pPT1->setType(kCCProgressTimerTypeRadial);
                    m_pPT1->setPosition(ccp(m_pSkill1->getContentSize().width / 2, m_pSkill1->getContentSize().height / 2));
                    m_pSkill1->addChild(m_pPT1, 1);
                }
                if(HeroMgr::getSingleton().m_myHero.skill1 != 0)
                {
                    sprintf(str, "ui_skill_%d", HeroMgr::getSingleton().m_myHero.skill1 / 100);
                    pFrame = ImgMgr::getSingleton().getFrame(str);
                    setControlButtonFrameForAllState(m_pSkill2, pFrame);
                    
                    sprintf(str, "ui_skill_%d_0", HeroMgr::getSingleton().m_myHero.skill1 / 100);
                    CCSprite* pSprite = ImgMgr::getSingleton().loadSprite(str);
                    m_pPT2 = CCProgressTimer::create(pSprite);
                    m_pPT2->setPercentage(0);
                    m_pPT2->setType(kCCProgressTimerTypeRadial);
                    m_pPT2->setPosition(ccp(m_pSkill2->getContentSize().width / 2, m_pSkill2->getContentSize().height / 2));
                    m_pSkill2->addChild(m_pPT2, 1);
                }
                if(HeroMgr::getSingleton().m_myHero.skill2 != 0)
                {
                    sprintf(str, "ui_skill_%d", HeroMgr::getSingleton().m_myHero.skill2 / 100);
                    pFrame = ImgMgr::getSingleton().getFrame(str);
                    setControlButtonFrameForAllState(m_pSkill3, pFrame);
                    
                    sprintf(str, "ui_skill_%d_0", HeroMgr::getSingleton().m_myHero.skill2 / 100);
                    CCSprite* pSprite = ImgMgr::getSingleton().loadSprite(str);
                    m_pPT3 = CCProgressTimer::create(pSprite);
                    m_pPT3->setPercentage(0);
                    m_pPT3->setType(kCCProgressTimerTypeRadial);
                    m_pPT3->setPosition(ccp(m_pSkill3->getContentSize().width / 2, m_pSkill3->getContentSize().height / 2));
                    m_pSkill3->addChild(m_pPT3, 1);
                }
            }
            
            if (GuideMgr::getSingleton().isGuiding()) {
                
                m_pSkill1->setVisible(false);
                m_pSkill2->setVisible(false);
                m_pSkill3->setVisible(false);
                
                m_pBtnHero->setVisible(false);
            }
            
            m_pBtnHero->setVisible(false);
            if (HeroMgr::getSingleton().m_myHero.heroid != 0 && HeroMgr::getSingleton().m_heroState !=HERO_STATE_UPGRADING){
                sprintf(str, "hero_icon_%d_0", HeroMgr::getSingleton().m_myHero.heroid/100);
                CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
                sprintf(str, "hero_icon_%d_1", HeroMgr::getSingleton().m_myHero.heroid/100);
                CCSpriteFrame* pFrameDisabled = ImgMgr::getSingleton().getFrame(str);
                
                m_pBtnHero->setBackgroundSpriteFrameForState(pFrame, CCControlStateNormal);
                m_pBtnHero->setBackgroundSpriteFrameForState(pFrame, CCControlStateHighlighted);
                m_pBtnHero->setBackgroundSpriteFrameForState(pFrameDisabled, CCControlStateDisabled);
                m_pBtnHero->setVisible(true);
            }
            
            m_pClanName->setVisible(false);
            m_pClanIcon->setVisible(false);
            if (ClanMgr::getSingleton().m_otherclan.id > 0) {
                m_pClanName->setString(ClanMgr::getSingleton().m_otherclan.name.c_str());
                
                sprintf(str, "clan_icon_%d", ClanMgr::getSingleton().m_otherclan.flag);
                CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
                m_pClanIcon->setDisplayFrame(pFrame);
                
                m_pClanName->setVisible(true);
                m_pClanIcon->setVisible(true);
            }
            
            if (pScrollView->getContainer()->getChildrenCount() * soldierSize.width < m_pTroopsView->getContentSize().width) {
                pScrollView->setTouchEnabled(false);
            }
            else {
                m_pSoldierView->setContentSize(CCSizeMake(pScrollView->getContainer()->getChildrenCount() * soldierSize.width, m_pSoldierView->getContentSize().height));
            }
            
        }
        
        LogicInfo::getSingleton().m_fightUseSkillGems = 0;
        
        m_half = false;
        m_starnum = 0;
        
        m_isHeroDroped = false;
        
        if (LogicInfo::getSingleton().m_revengeBattleId > 0) {
            m_pBtnFindMatch->setVisible(false);
        }
        
        if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_PVE) {
            chgMode(FIGHT_MODE_BEGIN);
        }else{
            chgMode(FIGHT_MODE_WAIT);
        }
        
        showStars();
        refreshInfo();
        
        bRet = true;
    }while (0);
    
    this->schedule(schedule_selector(FightUI::MutUpdate));
    
    this->schedule(schedule_selector(FightUI::onTimer), 0.1);
    
    return bRet;
}

// a selector callback
void FightUI::menuEndFightCallback(CCObject* pSender, CCControlEvent event)
{
    if(!LogicInfo::getSingleton().m_isAttackDropPerson)
    {
        GameWorld* pGameWorld = PlayerScene::getSingleton().m_pGameWorld;
        pGameWorld->hideFightUI();
        
        pGameWorld->showChgScene(CHANGE_TYPE_BACKHOME);
        
        LogicInfo::getSingleton().goHome();
    } else
    {
        //end();
        MessageUI::show(MSG_END_BATTLE, NULL, NULL);
    }

}

// a selector callback
void FightUI::menuFindCallback(CCObject* pSender, CCControlEvent event)
{
    int cost;
    if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_PVP) {
        cost = LogicInfo::getSingleton().getFindGold();
    }
    else if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_LEAGUE) {
        cost = LeagueMgr::getSingleton().getSearchCost();
    }
    
    if(LogicInfo::getSingleton().m_myInfo.goldex < cost)
    {        
        char message[512];
        int money = cost - LogicInfo::getSingleton().m_myInfo.goldex;
        sprintf(message, ViewString::getSingleton().getValue("msg_nogold_message").c_str(), money);
        
        if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_PVP) {
            MessageUI::show(MSG_NOGOLD, ViewString::getSingleton().getValue("msg_nogold_title").c_str(), message, money, GETGEM_FINDMATCH_PVP);
        }
        else if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_LEAGUE) {
            MessageUI::show(MSG_NOGOLD, ViewString::getSingleton().getValue("msg_nogold_title").c_str(), message, money, GETGEM_FINDMATCH_LEAGUE);
        }
        
        return;
    }

    (PlayerScene::getSingleton().m_pGameWorld)->showChgScene(CHANGE_TYPE_FINDMATCH);
    if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_LEAGUE) {
        LogicInfo::getSingleton().findAttackUser(ATTACK_TYPE_LEAGUE);
    }
    else {
        LogicInfo::getSingleton().findAttackUser(ATTACK_TYPE_PVP);
    }
	(PlayerScene::getSingleton().m_pGameWorld)->hideFightUI();
}

void FightUI::menuSoldierCallback(CCObject *pSender, CCControlEvent event)
{
    int tag = ((CCNode*)pSender)->getTag();
    std::list<FightSoldierInfoEx>::iterator it;
    for(it = LogicInfo::getSingleton().m_lstFightSoldier.begin(); it != LogicInfo::getSingleton().m_lstFightSoldier.end(); ++it)
    {
        if (it->pid == tag) {
            
            if (it->pid/100 == PERSON_HERO && !HeroMgr::getSingleton().isFullHP()) {
                
                int gems = HeroMgr::getSingleton().getFillHeroHPGems();
                if (gems > 0) {
                    
                    char str[256];
                    sprintf(str, ViewString::getSingleton().getValue("recover_hero_hp_message").c_str(), gems);
                    MessageUI::show(MSG_FILL_HERO_HP, ViewString::getSingleton().getValue("recover_hero_hp_title").c_str(), str, gems, -1);
                }
                
                return;
            }
            
            LogicInfo::getSingleton().setSoldier2Fight(it -> pid);
            
            if (pSoldierSelected != NULL) {
                pSoldierSelected->getChildByTag(0)->setVisible(false);
            }
            
            ((CCControlButton*)pSender)->getChildByTag(0)->setVisible(true);
            pSoldierSelected = (CCControlButton*)pSender;            
            break;
        }
    }
}

void FightUI::menuClanSoldierCallback(CCObject *pSender, CCControlEvent event)
{
    if (pSoldierSelected != NULL) {
        pSoldierSelected->getChildByTag(0)->setVisible(false);
    }
    
    ((CCControlButton*)pSender)->getChildByTag(0)->setVisible(true);
    pSoldierSelected = (CCControlButton*)pSender;
    
    LogicInfo::getSingleton().setClanSoldier2Fight();
}

bool FightUI::onNotify(int notifyid, const void* pVal)
{
	if(NOTIFY_OTHERBASEINFO == notifyid)
	{
		m_bNeedRefurbish = true;
		refreshInfo();
	}
    else if(NOTIFY_DROPSOLDIER == notifyid)
    {
        if(m_mode == FIGHT_MODE_WAIT)
        {
            chgMode(FIGHT_MODE_BEGIN);
        }
        
        if (PlayerScene::getSingleton().m_curSelPersonID > 30000) {
            m_isHeroDroped = true;
            
            if(HeroMgr::getSingleton().m_myHero.skill0 != 0)
            {
                m_pSkill1->setEnabled(true);
            }
            if(HeroMgr::getSingleton().m_myHero.skill1 != 0)
            {
                m_pSkill2->setEnabled(true);
            }
            if(HeroMgr::getSingleton().m_myHero.skill2 != 0)
            {
                m_pSkill3->setEnabled(true);
            }
            
            if (!m_isSkillOn) {
                m_pAnimationManager->runAnimationsForSequenceNamed("skill_in");
                m_isSkillOn = true;
                
                m_pSkill1->setVisible(true);
                m_pSkill2->setVisible(true);
                m_pSkill3->setVisible(true);
            }
        }
        else if (PlayerScene::getSingleton().m_bSelClanSoldier) {
            
            ((CCSprite*)m_pBtnClanSoldier->getChildByTag(1))->setColor(ccc3(60, 60, 60));
            m_pBtnClanSoldier->setColor(ccc3(120, 120, 120));
        }
        
        char str[128];
        for(std::list<FightSoldierInfoEx>::iterator it = LogicInfo::getSingleton().m_lstFightSoldier.begin(); it != LogicInfo::getSingleton().m_lstFightSoldier.end(); ++it)
        {
            CCControlButton* pSoldier = (CCControlButton*)m_pSoldierView->getChildByTag(it->pid);
            
            sprintf(str, "x%d", (int)it->numsex);
            CCLabelBMFont* pNums = (CCLabelBMFont*)pSoldier->getChildByTag(1);
            pNums->setString(str);
            
            if(it->numsex == 0)
            {
                sprintf(str, "player1_%d_0", (int)(it->pid / 100));

                CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
                pSoldier->setBackgroundSpriteFrameForState(pFrame, CCControlStateNormal);
            }
        }
                
        m_pDropSoldierTip->setVisible(false);
    }
    else if(NOTIFY_FIGHT_END == notifyid)
    {
        chgMode(FIGHT_MODE_END);
        
        LogicInfo::getSingleton().onStopAttack(false);
        
        schedule(schedule_selector(FightUI::exit), 2.0f, 0, 2.0f);
    }
    else if(NOTIFY_NEWFIND == notifyid)
    {
        refreshInfo();
    }
    else if(NOTIFY_DESTROY_HOME == notifyid)
    {
        m_starnum ++;
        showStars();
    }
    else if (NOTIFY_HERO_FULL_HP == notifyid)
    {
        LogicInfo::getSingleton().m_lastFightSoldierNums++;
        
        LogicInfo::getSingleton().setSoldier2Fight(HeroMgr::getSingleton().m_myHero.heroid);

        CCControlButton* pSoldierButton = (CCControlButton*)m_pSoldierView->getChildByTag(HeroMgr::getSingleton().m_myHero.heroid);
        if (pSoldierButton) {
            if (pSoldierSelected != NULL) {
                pSoldierSelected->getChildByTag(0)->setVisible(false);
            }
            pSoldierButton->getChildByTag(0)->setVisible(true);
            pSoldierSelected = (CCControlButton*)pSoldierButton;
        }
    }
    
	return true;
}

void FightUI::chgMode(int mode)
{
    if (m_mode == FIGHT_MODE_WAIT && mode == FIGHT_MODE_BEGIN) {
        m_pBattleTimeTip->setString("Battle ends in:");
        m_pBtnFindMatch->setVisible(false);
        m_pDropSoldierTip->setVisible(false);
    }
    
    m_mode = mode;
    
	if(m_mode == FIGHT_MODE_WAIT)
	{
		m_lastWaitTimeMs = FIGHT_READY_TIME * 1000;
        SoundMgr::getSingleton().playBackgroundMusic("fight_start.mp3");
	}
	else if(m_mode == FIGHT_MODE_BEGIN)
	{
		m_lastFightTimeMs = FIGHT_TIME * 1000;
        SoundMgr::getSingleton().playBackgroundMusic("fight_back.mp3");
	}
}

void FightUI::MutUpdate(float fDelta)
{
    char str[128];
	if(m_bNeedRefurbish)
	{
		long long ctime = getCurrentTimeMs();
        
		if(ctime - m_i64RefurbishTime >= 1000)
			refreshInfo();
	}
    
    if (LogicInfo::getSingleton().m_attackType != ATTACK_TYPE_PVE)
    {
        if(m_mode == FIGHT_MODE_WAIT)
        {
            if(m_lastWaitTimeMs > FIGHT_READY_TIME * 1000)
                m_lastWaitTimeMs = FIGHT_READY_TIME * 1000;
            
            int t = m_lastWaitTimeMs / 1000;
            
            m_lastWaitTimeMs -= fDelta * 1000;
            if(m_lastWaitTimeMs >= 0 && (t == FIGHT_READY_TIME || t != m_lastWaitTimeMs / 1000))
            {
                t = m_lastWaitTimeMs / 1000;
                if(t <= 3 && m_pAnimationManager->getRunningSequenceName() == NULL)
                {
                    m_pAnimationManager->runAnimationsForSequenceNamed("attack_countdown");
                }
                
                if(t < 0)
                    t = 0;
                
                m_pBattleTime->setString(formatTime(t).c_str());
            }
            
            if(m_lastWaitTimeMs <= 0)
            {
                LogicInfo::getSingleton().beginFight();
                chgMode(FIGHT_MODE_BEGIN);
            }
        }
        else if(m_mode == FIGHT_MODE_BEGIN)
        {
            m_lastFightTimeMs = (FIGHT_TIME * 1000 - LogicInfo::getSingleton().m_fightBeginTimeMS);
            
            if (m_lastFightTimeMs < 0) {
                m_lastFightTimeMs = 0;
                
                chgMode(FIGHT_MODE_END);
                
                LogicInfo::getSingleton().onStopAttack(false);
                LogicInfo::getSingleton().onNotify(NOTIFY_FIGHT_END);
            }
            
            m_pBattleTime->setString(formatTimeMs(m_lastFightTimeMs).c_str());
        }
    }
    
    if(m_pActGold != NULL && m_pActGold->isDone())
	{
		m_pActGold->release();
		m_pActGold = NULL;
	}
    
	if(m_pActOil != NULL && m_pActOil->isDone())
	{
		m_pActOil->release();
		m_pActOil = NULL;
	}
    
    if (PlayerScene::getSingleton().isHeroDie()) {
        
        sprintf(str, "hero_icon_%d_2", HeroMgr::getSingleton().m_myHero.heroid/100);
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        setControlButtonFrameForAllState(m_pBtnHero, pFrame);
        
        m_time1 = 0;
        m_time2 = 0;
        m_time3 = 0;
        
        if (m_pPT1) {
            m_pPT1->setPercentage(0);
        }
        if (m_pPT2) {
            m_pPT2->setPercentage(0);
        }
        if (m_pPT3) {
            m_pPT3->setPercentage(0);
        }

        m_pBtnHero->setEnabled(false);
        
        if (m_isSkillOn) {
            m_pAnimationManager->runAnimationsForSequenceNamed("skill_out");
            m_isSkillOn = false;
        }
    }
}

void FightUI::refreshInfo()
{
    long long ctime = getCurrentTimeMs();
    if(m_i64RefurbishTime == 0)
        m_i64RefurbishTime = ctime;
    else if(ctime - m_i64RefurbishTime < 1000)
        return;
    
    m_bNeedRefurbish = false;
    
    char str[128];
    sprintf(str, "%s", LogicInfo::getSingleton().m_otherInfo.uname.c_str());
    m_pRivalName->setString(str);
    
    sprintf(str, "player_mr_%d",LogicInfo::getSingleton().m_otherInfo.military);
    CCSpriteFrame *frame = ImgMgr::getSingleton().getFrame(str);
    m_pIconMilitary->setDisplayFrame(frame);
    
    sprintf(str, "%d", LogicInfo::getSingleton().countWarCup(3, LogicInfo::getSingleton().m_otherBaseLevel - LogicInfo::getSingleton().m_myBaseLevel, LogicInfo::getSingleton().m_otherInfo.cupex - LogicInfo::getSingleton().m_myInfo.cupex));
    
    m_pWinCup->setString(str);
    
    sprintf(str, "%d", LogicInfo::getSingleton().countWarCup(0, LogicInfo::getSingleton().m_otherBaseLevel - LogicInfo::getSingleton().m_myBaseLevel, LogicInfo::getSingleton().m_otherInfo.cupex - LogicInfo::getSingleton().m_myInfo.cupex));
    m_pLoseCup->setString(str);
    
    EncryptInt curgold, curoil;
        
    curgold = LogicInfo::getSingleton().m_myInfo.goldex + LogicInfo::getSingleton().m_infoFightScore.goldex;
    if(curgold > LogicInfo::getSingleton().m_myInfo.maxgoldex)
        curgold = LogicInfo::getSingleton().m_myInfo.maxgoldex;
    
    curoil = LogicInfo::getSingleton().m_myInfo.oilex + LogicInfo::getSingleton().m_infoFightScore.oilex;
    if(curoil > LogicInfo::getSingleton().m_myInfo.maxoilex)
        curoil = LogicInfo::getSingleton().m_myInfo.maxoilex;
    
    if (LogicInfo::getSingleton().m_attackType != ATTACK_TYPE_LEAGUE || m_mode == FIGHT_MODE_WAIT)
    {
        if (LogicInfo::getSingleton().m_infoFightScore.goldex > 0) {
            if(curgold != atoi(m_pPlayerGold->getString()))
            {
                if(m_pActGold == NULL)
                {
                    m_pActGold = setChgAni(m_pIconGold);
                    m_pActGold->retain();
                }
            }
        }
        
        if (LogicInfo::getSingleton().m_infoFightScore.oilex > 0) {
            if(curoil != atoi(m_pPlayerElixir->getString()))
            {
                if(m_pActOil == NULL)
                {
                    m_pActOil = setChgAni(m_pIconElixir);
                    m_pActOil->retain();
                }
            }
        }
        
        sprintf(str, "%d", (int)curgold);
        m_pPlayerGold->setString(str);
        sprintf(str, "%d", (int)LogicInfo::getSingleton().m_myInfo.maxgoldex);
        m_pPlayerMaxGold->setString(str);
        
        sprintf(str, "%d", (int)curoil);
        m_pPlayerElixir->setString(str);
        sprintf(str, "%d", (int)LogicInfo::getSingleton().m_myInfo.maxoilex);
        m_pPlayerMaxElixir->setString(str);
        
        changeProgress(m_pProgressGold, ((float)curgold / LogicInfo::getSingleton().m_myInfo.maxgoldex));
        changeProgress(m_pProgressElixir, ((float)curoil / LogicInfo::getSingleton().m_myInfo.maxoilex));
        
        sprintf(str, "%d", (int)LogicInfo::getSingleton().m_otherInfo.goldex);
        m_pRivalGold->setString(str);
        sprintf(str, "%d", (int)LogicInfo::getSingleton().m_otherInfo.oilex);
        m_pRivalElixir->setString(str);
    }
    
    if (LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_CLANWAR
        || LogicInfo::getSingleton().m_attackType == ATTACK_TYPE_GROUPWAR) {
        m_pWinCup->setVisible(false);
        m_pLoseCup->setVisible(false);
    }
    
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

void FightUI::showStars()
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

void FightUI::exit(float fDelta)
{
    LogicInfo::getSingleton().onNotify(MSG_END_BATTLE, &NOTIFY_MESSAGE_CLOSE);
    
    PlayerScene::getSingleton().m_pGameWorld->hideFightUI();
    PlayerScene::getSingleton().m_pGameWorld->showScoreUI();
}

void FightUI::end()
{
    LogicInfo::getSingleton().onNotify(MSG_END_BATTLE, &NOTIFY_MESSAGE_CLOSE);
    
    LogicInfo::getSingleton().onStopAttack(false);
    
    PlayerScene::getSingleton().m_pGameWorld->hideFightUI();
    PlayerScene::getSingleton().m_pGameWorld->showScoreUI();
}

CCAction* FightUI::setChgAni(CCSprite* spr)
{
	CCPoint pt1 = spr->getPosition();
	CCPoint pt2 = spr->getPosition();
    
	pt1.x -= spr->getContentSize().width * 0.1f / 2;
	pt1.y -= spr->getContentSize().height * 0.1f / 2;
    
	CCMoveTo* mv1 = CCMoveTo::create(0.08f, pt1);
	CCMoveTo* mv2 = CCMoveTo::create(0.08f, pt2);
    
    CCScaleTo* sc1 = CCScaleTo::create(0.08f, spr->getScale() + 0.1);
	CCScaleTo* sc2 = CCScaleTo::create(0.08f, spr->getScale());
    
	CCFiniteTimeAction* action1 = CCSequence::create(sc1, sc2, sc1, sc2, sc1, sc2, NULL);
	CCFiniteTimeAction* action2 = CCSequence::create(mv1, mv2, mv1, mv2, mv1, mv2, NULL);
    
	CCAction* pAct = spr->runAction(action1);
	spr->runAction(action2);
    
	return pAct;
}

void FightUI::menuHeroCallback(CCObject* pSender, CCControlEvent event)
{
    int pid = ((CCNode*)pSender)->getTag();
    
    if (!HeroMgr::getSingleton().isFullHP()) {
        
        int gems = HeroMgr::getSingleton().getFillHeroHPGems();
        if (gems > 0) {
            
            char str[256];
            sprintf(str, ViewString::getSingleton().getValue("recover_hero_hp_message").c_str(), gems);
            MessageUI::show(MSG_FILL_HERO_HP, ViewString::getSingleton().getValue("recover_hero_hp_title").c_str(), str, gems, -1);
        }
        
        return;
    }
    
    LogicInfo::getSingleton().setSoldier2Fight(pid);
    
    if (pSoldierSelected != NULL) {
        pSoldierSelected->getChildByTag(0)->setVisible(false);
    }
    
    if (m_isHeroDroped && !PlayerScene::getSingleton().isHeroDie())
    {
        if (m_isSkillOn) {
            m_pAnimationManager->runAnimationsForSequenceNamed("skill_out");
            m_isSkillOn = false;
        }
        else
        {
            m_pAnimationManager->runAnimationsForSequenceNamed("skill_in");
            m_isSkillOn = true;
            
            m_pSkill1->setVisible(true);
            m_pSkill2->setVisible(true);
            m_pSkill3->setVisible(true);
        }
    }
    
}

void FightUI::menuSkill1Callback(CCObject* pSender, CCControlEvent event)
{
    if(!m_isHeroDroped || HeroMgr::getSingleton().m_myHero.skill0 == 0 || PlayerScene::getSingleton().isHeroDie())
        return;
    
    if(m_time1 > 0)
        return;
    
    int skillid = HeroMgr::getSingleton().getEquSkill(0);
    m_time1 = SkillMgr::getSingleton().getNode(skillid)->cd / 100;
    
    PlayerScene::getSingleton().useHeroSkill(skillid);
}

void FightUI::menuSkill2Callback(CCObject* pSender, CCControlEvent event)
{
    if(!m_isHeroDroped || HeroMgr::getSingleton().m_myHero.skill1 == 0 || PlayerScene::getSingleton().isHeroDie())
        return;
    
    if(m_time2 > 0)
        return;
    
    int skillid = HeroMgr::getSingleton().getEquSkill(1);
    m_time2 = SkillMgr::getSingleton().getNode(skillid)->cd / 100;
    PlayerScene::getSingleton().useHeroSkill(skillid);
}

void FightUI::menuSkill3Callback(CCObject* pSender, CCControlEvent event)
{
    if(!m_isHeroDroped || HeroMgr::getSingleton().m_myHero.skill2 == 0 || PlayerScene::getSingleton().isHeroDie())
        return;
    
    if(m_time3 > 0)
        return;
    
    int skillid = HeroMgr::getSingleton().getEquSkill(2);
    m_time3 = SkillMgr::getSingleton().getNode(skillid)->cd / 100;
    PlayerScene::getSingleton().useHeroSkill(skillid);
}

void FightUI::onTimer(float time)
{
    if(m_time1 > 0)
    {
        m_time1 --;
        
		int skillid = HeroMgr::getSingleton().getEquSkill(0);
        
        m_pPT1->setPercentage((float)m_time1 / (SkillMgr::getSingleton().getNode(skillid)->cd / 100) * 100);
        m_pSkill1->setEnabled(false);
        
        if (m_time1 <= 0) {
            m_pSkill1->setEnabled(true);
        }
    }
    
    if(m_time2 > 0)
    {
        m_time2 --;
        
		int skillid = HeroMgr::getSingleton().getEquSkill(1);
        
        m_pPT2->setPercentage((float)m_time2 / (SkillMgr::getSingleton().getNode(skillid)->cd / 100) * 100);
        m_pSkill2->setEnabled(false);
        
        if (m_time2 <= 0) {
            m_pSkill2->setEnabled(true);
        }
    }
    
    if(m_time3 > 0)
    {
        m_time3 --;
        
		int skillid = HeroMgr::getSingleton().getEquSkill(2);
        
        m_pPT3->setPercentage((float)m_time3 / (SkillMgr::getSingleton().getNode(skillid)->cd / 100) * 100);
        m_pSkill3->setEnabled(false);
        
        if (m_time3 <= 0) {
            m_pSkill3->setEnabled(true);
        }
    }
}

bool FightUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RivalGold", CCLabelBMFont*, m_pRivalGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RivalElixir", CCLabelBMFont*, m_pRivalElixir);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WinCup", CCLabelBMFont*, m_pWinCup);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LoseCup", CCLabelBMFont*, m_pLoseCup);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RivalName", CCLabelTTF*, m_pRivalName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerGold", CCLabelBMFont*, m_pPlayerGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerMaxGold", CCLabelBMFont*, m_pPlayerMaxGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerElixir", CCLabelBMFont*, m_pPlayerElixir);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerMaxElixir", CCLabelBMFont*, m_pPlayerMaxElixir);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleTime", CCLabelBMFont*, m_pBattleTime);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleTimeTip", CCLabelBMFont*, m_pBattleTimeTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "DropSoldierTip", CCLabelBMFont*, m_pDropSoldierTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleProgress", CCLabelBMFont*, m_pBattleProgress);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleStars", CCSprite*, m_pBattleStars);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "IconGold", CCSprite*, m_pIconGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "IconElixir", CCSprite*, m_pIconElixir);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ProgressGold", CCScale9Sprite*, m_pProgressGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ProgressElixir", CCScale9Sprite*, m_pProgressElixir);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CupNode", CCNode*, m_pCupNode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LootNode", CCNode*, m_pLootNode);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TroopsView", CCLayer*, m_pTroopsView);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnEndBattle", CCControlButton*, m_pBtnEndBattle);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnFindMatch", CCControlButton*, m_pBtnFindMatch);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnHero", CCControlButton*, m_pBtnHero);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Skill1", CCControlButton*, m_pSkill1);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Skill2", CCControlButton*, m_pSkill2);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Skill3", CCControlButton*, m_pSkill3);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnClanSoldier", CCControlButton*, m_pBtnClanSoldier);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "IconMilitary", CCSprite*, m_pIconMilitary);
    
    return NULL;
}

SEL_CCControlHandler FightUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuEndFightCallback", FightUI::menuEndFightCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuFindCallback", FightUI::menuFindCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuHeroCallback", FightUI::menuHeroCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSkill1Callback", FightUI::menuSkill1Callback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSkill2Callback", FightUI::menuSkill2Callback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSkill3Callback", FightUI::menuSkill3Callback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuClanSoldierCallback", FightUI::menuClanSoldierCallback);
    
    return NULL;
}