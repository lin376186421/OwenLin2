//
//  LeagueUI.cpp
//  GameClient
//
//  Created by Lee on 14-6-23.
//
//

#include "LeagueUI.h"
#include "../coc/ImgMgr.h"
#include "../coc/LogicInfo.h"
#include "../coc/PlayerScene.h"
#include "../coc/GoldEmitter.h"
#include "../GameWorldScene.h"

using namespace cocos2d;
using namespace coc;


LeagueUI::LeagueUI()
{
    
}

LeagueUI::~LeagueUI()
{
    LogicInfo::getSingleton().removeListener(this);
}

bool LeagueUI::init()
{
    bool bRet = false;
    do
    {
		CC_BREAK_IF(!CUINode::init());
        {
			LogicInfo::getSingleton().addListener(this);
            
            CCNode* rankingBg = CUINodeCache::sharedUINodeCache()->UINodeFromFile("league_bg.ccbi", this);//
            this->setContentSize(rankingBg->getContentSize());
            this->setTargetTouch(true);
            this->addChild(rankingBg);
            
            m_pLoading = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading1.ccbi", this);
            m_pLoading->setPosition(ccp(getContentSize().width/2, getContentSize().height/2));
            addChild(m_pLoading,10);
            
            m_pPageNode = NULL;
            m_pLeagueInfoUI = NULL;
            m_pGetAwardUI = NULL;
            m_pBtnLeagueMatch = NULL;
            m_pClearingNode = NULL;
            m_pEndLBNode = NULL;
            m_pClearingLBNode = NULL;
            m_pNextLBNode = NULL;
            
            m_pView = NULL;
            m_endTime    = 0;
            pageIndex    = 0;
            m_helpIndex  = 0;
            m_isLeagueOk = false;
            
            addPageNode(pageIndex);
            refreshPage();
//            m_pBtnPages[2]->setVisible(false);
            
            HttpClient::getSingleton().requestLeagueInfo();
            schedule(schedule_selector(LeagueUI::update), 1.0);
            
        }
		bRet = true;
	}while(0);
    
	return bRet;
}

void LeagueUI::update(float dt)
{
    if (pageIndex == 0 && m_isLeagueOk)
    {
        
            if (LeagueMgr::getSingleton().isReadyState())
            {
                if(m_pEndLBNode)
                {
                    m_pEndLBNode->setVisible(false);
                    m_pClearingLBNode->setVisible(false);
                    m_pNextLBNode->setVisible(true);
                    CCLabelBMFont *pTimeLabel = (CCLabelBMFont *)m_pNextLBNode->getChildByTag(1);
                    CCLabelBMFont *pCountLabel = (CCLabelBMFont *)m_pNextLBNode->getChildByTag(2);
                    pTimeLabel->setString("Next LB Starts In:");
                    pCountLabel->setString(formatTime(LeagueMgr::getSingleton().m_startTime - getCurrentTime()).c_str());
                }
                else
                {
                    m_pTimeDesLabel->setString("LB Starts In:");
                    m_pTimeCount->setString(formatTime(LeagueMgr::getSingleton().m_startTime - getCurrentTime()).c_str());
                }
                
                
                if (m_pClearingNode && m_pView)
                {
                    m_pClearingNode->setVisible(false);
                    m_pView->setVisible(true);
                }
            }
            else if(LeagueMgr::getSingleton().isBattleState())
            {
                if(m_pEndLBNode)
                {
                    m_pNextLBNode->setVisible(false);
                    m_pClearingLBNode->setVisible(false);
                    m_pEndLBNode->setVisible(true);
                }
                
                m_pTimeDesLabel->setString("LB Ends In:");
                m_pTimeCount->setString(formatTime(LeagueMgr::getSingleton().m_endTime - getCurrentTime()).c_str());
                if (m_pClearingNode && m_pView)
                {
                    m_pClearingNode->setVisible(false);
                    m_pView->setVisible(true);
                }
            }
            else if (LeagueMgr::getSingleton().isClearingState())
            {
                if(m_pEndLBNode)
                {
                    m_pNextLBNode->setVisible(false);
                    m_pEndLBNode->setVisible(false);
                    m_pClearingLBNode->setVisible(true);
                    CCLabelBMFont *pTimeLabel = (CCLabelBMFont *)m_pClearingLBNode->getChildByTag(1);
                    pTimeLabel->setString("LB Ended");
                }
                else
                {
                    m_pTimeCount->setString("");
                    m_pTimeDesLabel->setString("LB Ended");
                }
                
                
                if (m_pClearingNode && m_pView)
                {
                    m_pClearingNode->setVisible(true);
                    m_pView->setVisible(false);
                }
            }
            if (m_pBtnLeagueMatch)
            {
                m_pBtnLeagueMatch->setEnabled(LeagueMgr::getSingleton().canAttack());
            }
            
        }
}

void LeagueUI::addPageNode(int nPageIndex)
{
    
    if (m_pPageNode)
    {
        this->removeChild(m_pPageNode);
        m_pPageNode = NULL;
        m_pView = NULL;
        m_pBtnLeagueMatch = NULL;
        m_pClearingNode = NULL;
        m_pNextLBNode = NULL;
        m_pClearingLBNode = NULL;
        m_pEndLBNode = NULL;
    }
    
    if (nPageIndex == 0)
    {
        if (m_isLeagueOk)
        {
            if (LeagueMgr::getSingleton().isInLeague())
            {
                m_pPageNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("ranking_league_1.ccbi", this);
                m_pTableView = CCTableView::create(this, m_pView->getContentSize());
                m_pTableView->setDirection(kCCScrollViewDirectionVertical);
                m_pTableView->setDelegate(this);
                m_pTableView->reloadData();
                m_pTableView->setAnchorPoint(ccp(0, 0));
                m_pView->addChild(m_pTableView);
            } else
            {
                m_pPageNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("ranking_league_0.ccbi", this);
                
            }
            addChild(m_pPageNode);
        }
        
    }
    else if (nPageIndex == 1)
    {
        m_pPageNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("league_help.ccbi", this);
        refreshHelpPage(m_helpIndex);
        addChild(m_pPageNode);
    }

    else if (nPageIndex == 2)
    {
        if (m_isLeagueOk && LeagueMgr::getSingleton().isInLeague())
        {
            m_pPageNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("ranking_league_info.ccbi", this);
            addChild(m_pPageNode);
        }
        
    }
    
    if (m_pView)
    {
        m_pItemMenu = ItemMenuUI::create(m_pView);
        m_pItemMenu->setVisible(false);
        m_pView->addChild(m_pItemMenu);
    }
    
}

void LeagueUI::refreshPage()
{
    for (int i = 0; i < 2; i++)
    {
        m_pBtnPages[i]->setEnabled(true);
    }
    m_pBtnPages[pageIndex]->setEnabled(false);
    
    if (m_isLeagueOk)
    {
        if (pageIndex == 0)
        {
            refreshLeagueInfo();
        }
        else if (pageIndex == 2)
        {
            refreshBattleReward();
        }
    }
    else if (pageIndex == 1)
    {
        m_pLoading->setVisible(false);
    }
    else
    {
        m_pLoading->setVisible(true);
    }
}

void LeagueUI::refreshLeagueInfo()
{
    if (m_pPageNode == NULL)
    {
        return;
    }
    if(LeagueMgr::getSingleton().isInLeague())
    {
        refreshLeaguePage1();
    }
    else
    {
        refreshLeaguePage0();
    }
    update(0.0);
}


void LeagueUI::refreshLeaguePage1()
{
    char str[128];

    m_pBtnLeagueMatch->setEnabled(LeagueMgr::getSingleton().canAttack());
    
    sprintf(str, "%d",LeagueMgr::getSingleton().getSearchCost());
    CCLabelBMFont *pCostLabel = (CCLabelBMFont *)m_pBtnLeagueMatch->getChildByTag(1);
    pCostLabel->setString(str);
    
    sprintf(str, "icon_clanlevel_%d", LeagueMgr::getSingleton().m_myGroup.levelid);
    CCSprite *pLeagueIcon = (CCSprite *)m_pBtnLeagueMatch->getChildByTag(2);
    CCSpriteFrame* pLeagueIconFrame = ImgMgr::getSingleton().getFrame(str);
    pLeagueIcon->setDisplayFrame(pLeagueIconFrame);
    
    CCSpriteFrame* pCurFrame = ImgMgr::getSingleton().getFrame(str);
    m_pCurIcon->setDisplayFrame(pCurFrame);
    
    sprintf(str, "%d/%d",LeagueMgr::getSingleton().m_myGroup.myInfo.winNums,(int)LeagueMgr::getSingleton().m_myGroup.playerLst.size()-1);
    m_pWinNumLabel->setString(str);
    m_pCurNameLabel->setString(LeagueMgr::getSingleton().m_myGroup.name.c_str());
    
    //group 瓒�澶�  ���璧�绛�绾ц��楂�
    LeagueMgr::LevelInfo nextLevelInfo;
    if (LeagueMgr::getSingleton().m_myGroup.levelid<LeagueMgr::getSingleton().m_levelInfoLst.size())
    {
        nextLevelInfo = LeagueMgr::getSingleton().m_levelInfoLst[LeagueMgr::getSingleton().m_myGroup.levelid];
    }
    else
    {
        m_pNextTittle->setString("Max \nLeague Level");
        nextLevelInfo = LeagueMgr::getSingleton().m_levelInfoLst[LeagueMgr::getSingleton().m_myGroup.levelid-1];
    }
    m_pNextNameLabel->setString(nextLevelInfo.name.c_str());
    LeagueMgr::ResInfo nextResInfo = nextLevelInfo.basLst[2];
    for (int i = 0; i<nextResInfo.types; i++)
    {
        int type = nextResInfo.resType[i];
        int num =  nextResInfo.num[i];
        sprintf(str, "%d",num);
    }
    sprintf(str, "icon_clanlevel_%d", nextLevelInfo.levelid);
    CCSpriteFrame* pNextFrame = ImgMgr::getSingleton().getFrame(str);
    m_pNextIcon->setDisplayFrame(pNextFrame);
    
    m_pGroupInfoLabel->setString(CCString::createWithFormat(ViewString::getSingleton().getValue("lb_group_info").c_str(),LeagueMgr::getSingleton().m_myGroup.groupid,LeagueMgr::getSingleton().m_myGroup.groupnums)->getCString());
    
    m_pCurGroupLabel->setString(CCString::createWithFormat("(Group %d)",LeagueMgr::getSingleton().m_myGroup.groupid)->getCString());
    
    LeagueMgr::ResInfo curResInfo = LeagueMgr::getSingleton().getCurBattleAwardInfo(3);
    for (int i = 0; i<curResInfo.types; i++)
    {
        int type = curResInfo.resType[i];
        int num  = curResInfo.num[i];
        sprintf(str, "%d",num);
        if (type == MONEY_GOLD)
        {
            m_pCurMaxGoldLabel->setString(str);
        }
        else if (type == MONEY_ELIXIR)
        {
            m_pCurMaxOilLabel->setString(str);
        }
    }
    
    CCNode *pNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("ranking_league_bonus.ccbi", this);
    int rankCount = LeagueMgr::getSingleton().m_myGroup.rankAwardLst.size()>10?10:LeagueMgr::getSingleton().m_myGroup.rankAwardLst.size();
    float itemHeight = pNode->getContentSize().height + 3;
    float scorllViewHeight = (pNode->getContentSize().height+3) * rankCount;
    m_pScrollView->setContentSize(CCSize(m_pScrollView->getViewSize().width, scorllViewHeight));
    
    for (int i = 1; i<=rankCount; i++)
    {
        pNode->setPosition((m_pScrollView->getContentSize().width - pNode->getContentSize().width)/2, scorllViewHeight - itemHeight*i);
        m_pScrollView->addChild(pNode,100);
        
        CCLabelBMFont *pRank = (CCLabelBMFont *)pNode->getChildByTag(1);
        sprintf(str, "%d",i);
        pRank->setString(str);
        
        LeagueMgr::ResInfo rankRes = LeagueMgr::getSingleton().getRankAwardInfo(i);
        for (int j = 0; j<rankRes.types; j++)
        {
            int type = rankRes.resType[j];
            int num  = rankRes.num[j];
            sprintf(str, "%d",num);
            if (type == LEAGUE_MEDAL)
            {
                CCLabelBMFont *pMedal = (CCLabelBMFont *)pNode->getChildByTag(3);
                pMedal->setString(str);
            }
            else if (type == MONEY_GEM)
            {
                CCLabelBMFont *pMedal = (CCLabelBMFont *)pNode->getChildByTag(2);
                pMedal->setString(str);
            }
        }
        pNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("ranking_league_bonus.ccbi", this);
    }
    
    int offsetY = 0;
    if (scorllViewHeight < m_pScrollView->getViewSize().height)
    {
        offsetY = m_pScrollView->getViewSize().height - scorllViewHeight;
    }
    else
    {
        offsetY = -(scorllViewHeight - m_pScrollView->getViewSize().height);
    }
    m_pScrollView->setContentOffset(ccp(0, offsetY));

}

void LeagueUI::refreshLeaguePage0()
{
    char str[128];

    for (int i = 0; i<LeagueMgr::getSingleton().m_levelInfoLst.size(); i++)
    {
        LeagueMgr::LevelInfo info = LeagueMgr::getSingleton().m_levelInfoLst.at(i);
        CCNode *pNode = m_pPageNode->getChildByTag(i+1);
        CCSprite *pIcon = (CCSprite *)pNode->getChildByTag(1);
        CCLabelBMFont *pName = (CCLabelBMFont *)pNode->getChildByTag(2);
        CCLabelBMFont *pCup = (CCLabelBMFont *)pNode->getChildByTag(3);
        sprintf(str, "icon_clanlevel_%d", info.levelid);
        CCSpriteFrame* pCurFrame = ImgMgr::getSingleton().getFrame(str);
        pIcon->setDisplayFrame(pCurFrame);
        pName->setString(info.name.c_str());
        sprintf(str, "%d+",info.mincup);
        pCup->setString(str);
    }
    m_pBtnBack->setVisible(false);
    
    if(m_pLeagueTipInfo)
    {
        string infoStr;
        if(LeagueMgr::getSingleton().isInLeague())
        {
            infoStr = ViewString::getSingleton().getValue("lb_playing_tip_info");
        }
        else
        {
            if(LeagueMgr::getSingleton().m_myGroup.state == LB_USER_STATUS_NOT_CUP)
            {
                int minCup ;
                for (int i = 0; i<LeagueMgr::getSingleton().m_levelInfoLst.size(); i++)
                {
                    LeagueMgr::LevelInfo levelInfo = LeagueMgr::getSingleton().m_levelInfoLst.at(i);
                    
					if (i == 0)
						minCup = levelInfo.mincup;
                    
                    if(LogicInfo::getSingleton().m_myInfo.cupex < levelInfo.mincup)
                    {
                        minCup = levelInfo.mincup;
                        break;
                    }
                    
                }
                
                infoStr = string(CCString::createWithFormat(ViewString::getSingleton().getValue("lb_not_cup_tip_info").c_str(),minCup)->getCString());
            }
            else
            {
                if(LeagueMgr::getSingleton().isBattleState())
                {
                    infoStr = ViewString::getSingleton().getValue("lb_start_tip_info");
                }
                else if (LeagueMgr::getSingleton().isDoneGroupState())
                {
                    infoStr = ViewString::getSingleton().getValue("lb_group_done_tip_info");
                }
                else
                {
                    infoStr = ViewString::getSingleton().getValue("lb_end_tip_info");
                }
            }
        }
        
        m_pLeagueTipInfo->setString(infoStr.c_str());
    }
}


void LeagueUI::refreshBattleReward()
{
    if (m_pPageNode == NULL)
    {
        return;
    }
    CCNode *pRoot = m_pPageNode->getChildByTag(1);
    char str[128];
    sprintf(str, "icon_clanlevel_%d", LeagueMgr::getSingleton().m_myGroup.levelid);
    
    CCSprite *pIcon = (CCSprite *)pRoot->getChildByTag(4);
    CCSpriteFrame* pCurFrame = ImgMgr::getSingleton().getFrame(str);
    pIcon->setDisplayFrame(pCurFrame);
    
    CCLabelBMFont *pName = (CCLabelBMFont *)pRoot->getChildByTag(5);
    pName->setString(LeagueMgr::getSingleton().m_myGroup.name.c_str());
    
    for (int i = 1; i<=3; i++)
    {
        CCNode *pNode = pRoot->getChildByTag(i);
        CCLabelBMFont *pGold = (CCLabelBMFont *)pNode->getChildByTag(1);
        CCLabelBMFont *pOil = (CCLabelBMFont *)pNode->getChildByTag(2);
        LeagueMgr::ResInfo curResInfo = LeagueMgr::getSingleton().getCurBattleAwardInfo(i);
        for (int j = 0; j<curResInfo.types; j++)
        {
            int type = curResInfo.resType[j];
            int num  = curResInfo.num[j];
            sprintf(str, "%d",num);
            if (type == MONEY_GOLD)
            {
                pGold->setString(str);
            }
            else if (type == MONEY_ELIXIR)
            {
                pOil->setString(str);
            }
        }
    }
}

void LeagueUI::showGetAwardUI()
{
    char str[128];
    hideGetAwardUI();
    m_pGetAwardUI = CUINode::createWithFile("league_result.ccbi", this);
    
    CCNode *pRoot = m_pGetAwardUI->getContainer()->getChildByTag(1);
    
    CCNode *pBtnShare = pRoot->getChildByTag(999);
    if(FacebookMgr::getSingleton().isShare() == false)
        pBtnShare->setVisible(false);
    
    sprintf(str, "%d",LeagueMgr::getSingleton().m_myGroup.myAward.rank);
    CCLabelBMFont *pRankLabel = (CCLabelBMFont *)pRoot->getChildByTag(1);
    pRankLabel->setString(str);
    
    CCNode *pAwardNode   = pRoot->getChildByTag(2);
    CCNode *pNoAwardNode = pRoot->getChildByTag(3);
    
    if (LeagueMgr::getSingleton().m_myGroup.myAward.hasAward)
    {
        pNoAwardNode->setVisible(false);
        pAwardNode->setVisible(true);
        CCLabelBMFont *pGemLabel = (CCLabelBMFont *)pAwardNode->getChildByTag(1);
        CCLabelBMFont *pMedalLabel = (CCLabelBMFont *)pAwardNode->getChildByTag(2);
        LeagueMgr::ResInfo awardRes = LeagueMgr::getSingleton().m_myGroup.myAward.resInfo;
        for (int j = 0; j<awardRes.types; j++)
        {
            int type = awardRes.resType[j];
            int num  = awardRes.num[j];
            sprintf(str, "%d",num);
            if (type == MONEY_GEM)
            {
                pGemLabel->setString(str);
            }
            else if (type == LEAGUE_MEDAL)
            {
                pMedalLabel->setString(str);
            }
        }
    }
    else
    {
        pNoAwardNode->setVisible(true);
        pAwardNode->setVisible(false);
    }
    
    PlayerScene::getSingleton().m_pGameWorld->addChild(m_pGetAwardUI,1);
    m_pGetAwardUI->setTargetTouch(true);
}

void LeagueUI::hideGetAwardUI()
{
    if (m_pGetAwardUI)
    {
        m_pGetAwardUI->removeFromParentAndCleanup(true);
        m_pGetAwardUI = NULL;
    }
}


void LeagueUI::menuBtnFBShareCallback(CCObject* pSender, CCControlEvent event)
{
    FacebookMgr::getSingleton().m_pCurBtn = (CCControlButton *)pSender;
    FacebookMgr::getSingleton().m_shareType = FBShare_Fight;
    PlayerScene::getSingleton().m_pGameWorld->showFBShareUI();
}
void LeagueUI::menuPageCallback(CCObject *pSender, CCControlEvent event)
{
    int tag = ((CCNode*)pSender)->getTag();
    if (pageIndex != tag)
    {
        pageIndex = tag;
        addPageNode(pageIndex);
        refreshPage();
    }
}

void LeagueUI::menuHelpCallback(CCObject *pSender, CCControlEvent event)
{
    int tag = ((CCNode*)pSender)->getTag();
    if (m_helpIndex != tag)
    {
        m_helpIndex = tag;
    }
    refreshHelpPage(m_helpIndex);
}

void LeagueUI::refreshHelpPage(int nIndex)
{
    for (int i = 0; i < 3; i++)
    {
        if (i == nIndex)
        {
            m_pBtnHelps[i]->setEnabled(false);
        }
        else
        {
            m_pBtnHelps[i]->setEnabled(true);
        }
    }
    
    for (int i = 3; i < 6; i++)
    {
        CCNode *pNode = m_pPageNode->getChildByTag(i);
        if (i == (nIndex+3))
        {
            pNode->setVisible(true);
        }
        else
        {
            pNode->setVisible(false);
        }
    }
}


void LeagueUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    (PlayerScene::getSingleton().m_pGameWorld)->hideLeagueUI();
    
    if(CCUserDefault::sharedUserDefault()->getBoolForKey("CLANWAR_TIP") == false)
    {
        PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->showBtnTip();
    }
}

void LeagueUI::menuBackCallback(cocos2d::CCObject *pSender, CCControlEvent event)
{
    addPageNode(0);
    refreshPage();
}

void LeagueUI::menuLeagueMatchCallback(cocos2d::CCObject *pSender, CCControlEvent event)
{
    LogicInfo::getSingleton().refurbishFightSoldierInfo();
    if(LogicInfo::getSingleton().m_lstFightSoldier.size() == 0)
    {
        MessageUI::show(MSG_NO_SOLDIER, "", ViewString::getSingleton().getValue("no_soldier").c_str());
        return;
    }
    
    if(LogicInfo::getSingleton().m_myInfo.goldex < LeagueMgr::getSingleton().getSearchCost())
    {
        char message[512];
        int value = LeagueMgr::getSingleton().getSearchCost() - LogicInfo::getSingleton().m_myInfo.goldex;
        sprintf(message, ViewString::getSingleton().getValue("msg_nogold_message").c_str(), value);
        
        MessageUI::show(MSG_NOGOLD, ViewString::getSingleton().getValue("msg_nogold_title").c_str(), message, value, GETGEM_FINDMATCH_LEAGUE);
        
        return;
    }
    
    LogicInfo::getSingleton().findAttackUser(ATTACK_TYPE_LEAGUE);
    PlayerScene::getSingleton().m_pGameWorld->hideLeagueUI();
    
    PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_FINDMATCH);
}

void LeagueUI::menuLeagueInfoCallback(cocos2d::CCObject *pSender, CCControlEvent event)
{
    if (m_pPageNode)
    {
        this->removeChild(m_pPageNode);
        m_pPageNode = NULL;
        m_pView = NULL;
        m_pBtnLeagueMatch = NULL;
        m_pClearingNode = NULL;
        m_pNextLBNode = NULL;
        m_pEndLBNode = NULL;
        m_pClearingLBNode = NULL;
    }
    m_pPageNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("ranking_league_0.ccbi", this);
    addChild(m_pPageNode,10);
    refreshLeaguePage0();
    update(0.0);
}

void LeagueUI::menuGetAwardCallback(cocos2d::CCObject *pSender, CCControlEvent event)
{
    HttpClient::getSingleton().getLeagueAward(LeagueMgr::getSingleton().m_myGroup.myAward.awardid);
    hideGetAwardUI();
    char str[128];
    if (LeagueMgr::getSingleton().m_myGroup.myAward.hasAward)
    {
        LeagueMgr::ResInfo awardRes = LeagueMgr::getSingleton().m_myGroup.myAward.resInfo;
        for (int j = 0; j<awardRes.types; j++)
        {
            int type = awardRes.resType[j];
            int num  = awardRes.num[j];
            sprintf(str, "%d",num);
            if (type == MONEY_GEM)
            {
                LogicInfo::getSingleton().chgMoney(MONEY_GEM, num);
            }
            else if (type == LEAGUE_MEDAL)
            {
                LogicInfo::getSingleton().m_myInfo.medal+=num;
            }
        }
    }
    
    LogicInfo::getSingleton().countMilitaryRank();
}

bool LeagueUI::onNotify(int notifyid, const void* pVal)
{
    if (NOTIFY_LG_LEVEL_INFO_OK == notifyid && !m_isLeagueOk)
    {
        m_pLoading->setVisible(false);
        m_isLeagueOk = true;
        addPageNode(pageIndex);
        refreshPage();
        
        if (LeagueMgr::getSingleton().isInLeague())
        {
//            m_pBtnPages[2]->setVisible(true);
        }
    }
    
    return true;
}

bool LeagueUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankingView", CCLayer*, m_pView);
    
    char name[128];
    for (int i = 0; i < 2; i++)
    {
        sprintf(name, "BtnPage%d", i);
        CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, name, CCControlButton*, m_pBtnPages[i]);
    }
    
    for (int i = 0; i < 3; i++)
    {
        sprintf(name, "BtnHelp%d", i);
        CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, name, CCControlButton*, m_pBtnHelps[i]);
    }
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnLeagueMatch", CCControlButton*, m_pBtnLeagueMatch);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnBack", CCControlButton*, m_pBtnBack);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TimeCount", CCLabelBMFont*, m_pTimeCount);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TimeDes", CCLabelBMFont*, m_pTimeDesLabel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WinNumLabel", CCLabelBMFont*, m_pWinNumLabel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CurMaxGoldLabel", CCLabelBMFont*, m_pCurMaxGoldLabel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CurMaxOilLabel", CCLabelBMFont*, m_pCurMaxOilLabel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "NextTittle", CCLabelBMFont*, m_pNextTittle);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CurNameLabel", CCLabelBMFont*, m_pCurNameLabel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "NextNameLabel", CCLabelBMFont*, m_pNextNameLabel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CurIcon", CCSprite*, m_pCurIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "NextIcon", CCSprite*, m_pNextIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ScrollView", CCScrollView*, m_pScrollView);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClearingNode", CCNode*, m_pClearingNode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GroupInfoLabel", CCLabelBMFont*, m_pGroupInfoLabel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CurGroup", CCLabelBMFont*, m_pCurGroupLabel);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LeagueTipInfo", CCLabelBMFont*, m_pLeagueTipInfo);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "NextLBNode", CCNode*, m_pNextLBNode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "EndLBNode", CCNode*, m_pEndLBNode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClearingLBNode", CCNode*, m_pClearingLBNode);
        
    return false;
}

SEL_CCControlHandler LeagueUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuPageCallback", LeagueUI::menuPageCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuHelpCallback", LeagueUI::menuHelpCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", LeagueUI::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", LeagueUI::menuBackCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuLeagueMatchCallback", LeagueUI::menuLeagueMatchCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuLeagueInfoCallback", LeagueUI::menuLeagueInfoCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuGetAwardCallback", LeagueUI::menuGetAwardCallback);
    
    return NULL;
}

CCSize LeagueUI::cellSizeForTable(CCTableView *table)
{
    return CCSizeMake(960, 70 + 8);
}

CCTableViewCell* LeagueUI::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
    CCTableViewCell *cell = table->dequeueCell();
    if (cell == NULL) {
        cell = new CCTableViewCell();
        cell->autorelease();
    }
    
    cell->setIdx(idx);
    cell->setAnchorPoint(ccp(0, 0));
    
    int rankIdx = numberOfCellsInTableView(table) - idx;
    
    int emptyCell[2] = {10+1, 100+2};
    
    if (rankIdx == emptyCell[0] || rankIdx == emptyCell[1]) {
        //empty cell
    }
    else{
        
        rankIdx = rankIdx - 1;
        if (rankIdx >= emptyCell[1]) {
            rankIdx -= 2;
        }else if (rankIdx >= emptyCell[0]){
            rankIdx -= 1;
        }
        
        if (pageIndex == 0)
        {
            LeagueMgr::PlayerInfo info = LeagueMgr::getSingleton().m_myGroup.playerLst[rankIdx];
            
            LeaguePlayerItem *item = LeaguePlayerItem::create(info, m_pView->getContentSize());
            item->setTag(1);
            item->setTouchControlView(table);
            cell->addChild(item);
            
        }
    }
    
    return cell;
}

void LeagueUI::tableCellTouched(CCTableView *table, CCTableViewCell *cell)
{
    
    if (pageIndex == 0)
    {
        CCSize cellSize = cellSizeForTable(table);
        LeaguePlayerItem* item = (LeaguePlayerItem*)cell->getChildByTag(1);
        if(item) {
            m_pItemMenu->resetMenu(item->m_info.uid, item->m_info.uname.c_str(), -1);
            m_pItemMenu->setPositionX(0.4*cellSize.width);
            m_pItemMenu->setPositionY(cell->getIdx() * cellSize.height + table->getContentOffset().y + cellSize.height/2);
            m_pItemMenu->adjuestPosition();
        }
    }
}

unsigned int LeagueUI::numberOfCellsInTableView(CCTableView *table)
{
    int size = 0;
    
    if (pageIndex == 0) {
        
        size = LeagueMgr::getSingleton().m_myGroup.playerLst.size();
    }
    else if (pageIndex == 1) {
        
        //        size = ClanMgr::getSingleton().m_pClanRankingLst.size();
    }
    else if (pageIndex == 2) {
        
        size = LogicInfo::getSingleton().m_lstCupRank.size();
    }
    
    if (size > 100) { // top 100
        size += 2;
    }
    else if (size > 10) { //top 10
        size += 1;
    }
    
    return size;
}



#pragma mask - LeaguePlayerItem

LeaguePlayerItem::LeaguePlayerItem(LeagueMgr::PlayerInfo info,CCSize size)
{
    m_size = size;
    m_info  = info;
}

LeaguePlayerItem::~LeaguePlayerItem()
{
    
}

LeaguePlayerItem* LeaguePlayerItem::create(LeagueMgr::PlayerInfo info, CCSize size)
{
    LeaguePlayerItem *pRet = new LeaguePlayerItem(info, size);
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

bool LeaguePlayerItem::init()
{
    if (CUINode::init()) {
        
        CCNode* itemNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("ranking_leagueitem.ccbi", this, m_size);
        this->setContentSize(itemNode->getContentSize());
        this->addChild(itemNode);
        
        refresh();
        return true;
    }
    
    return false;
}

void LeaguePlayerItem::refresh()
{
    char str[128];
    
    sprintf(str, "%d", m_info.rank);
    m_pRankingId->setString(str);
    
    sprintf(str, "%d", m_info.lv);
    m_pPlayerLevel->setString(str);
    
    m_pPlayerName->setString(m_info.uname.c_str());
    
    sprintf(str, "%d", m_info.star);
    m_pStars->setString(str);
    
    
    if((m_info.winNums+m_info.loseNums) <= 0)
    {
        m_pRankingId->setVisible(false);
    }
    else
    {
        m_pRankingId->setVisible(true);
    }
    
    sprintf(str, "%d/%d",m_info.winNums+m_info.loseNums,(int)LeagueMgr::getSingleton().m_myGroup.playerLst.size()-1);
    m_pWinNum->setString(str);
    
    sprintf(str, "player_mr_%d",m_info.mrank);
    CCSpriteFrame *pRankFrame = ImgMgr::getSingleton().getFrame(str);
    m_pRankFlag->setDisplayFrame(pRankFrame);
    
    
    sprintf(str, "icon_clanlevel_%d", LeagueMgr::getSingleton().getLevel(m_info.cup));
    CCSpriteFrame* pLevelFrame = ImgMgr::getSingleton().getFrame(str);
    m_pLeagueLevelIcon->setDisplayFrame(pLevelFrame);
    
    m_pClanIcon->setVisible(false);
    m_pClanName->setVisible(false);
    if (m_info.clanid > 0)
    {
        sprintf(str, "clan_icon_%d",m_info.clanFlag);
        m_pClanIcon->setDisplayFrame(ImgMgr::getSingleton().getFrame(str));
        m_pClanIcon->setVisible(true);
        
        m_pClanName->setString(m_info.clanName.c_str());
        m_pClanName->setVisible(true);
    }
    
    CCSize contentSize = m_pItemBg->getContentSize();
    if (m_info.uid == LogicInfo::getSingleton().m_myInfo.uid) { //self
        m_pItemBg->setSpriteFrame(ImgMgr::getSingleton().getFrame("mayClan_list_2"));
    }else {
        if (m_info.rank > 0) {
            LeagueMgr::PlayerInfo preRank = LeagueMgr::getSingleton().m_myGroup.playerLst[m_info.rank-1];
            if (preRank.uid == LogicInfo::getSingleton().m_myInfo.uid) {
                m_pItemBg->setSpriteFrame(ImgMgr::getSingleton().getFrame("mayClan_list_1"));
            }
        }else if (m_info.rank < LogicInfo::getSingleton().m_lstCupRank.size() - 1) {
            LeagueMgr::PlayerInfo nextRank = LeagueMgr::getSingleton().m_myGroup.playerLst[m_info.rank+1];
            if (nextRank.uid == LogicInfo::getSingleton().m_myInfo.uid) {
                m_pItemBg->setSpriteFrame(ImgMgr::getSingleton().getFrame("mayClan_list_0"));
            }
        }
    }
    
    m_pItemBg->setPreferredSize(contentSize);
}

bool LeaguePlayerItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankingId", CCLabelBMFont*, m_pRankingId);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerLevel", CCLabelBMFont*, m_pPlayerLevel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerName", CCLabelTTF*, m_pPlayerName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RankFlag", CCSprite*, m_pRankFlag);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LeagueLevelIcon", CCSprite*, m_pLeagueLevelIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "WinNum", CCLabelBMFont*, m_pWinNum);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Stars", CCLabelBMFont*, m_pStars);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ItemBg", CCScale9Sprite*, m_pItemBg);
    
    return false;
}

SEL_CCControlHandler LeaguePlayerItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    return NULL;
}
