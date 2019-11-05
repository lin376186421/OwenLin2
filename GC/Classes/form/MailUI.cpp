//
//  MailUI.cpp
//  coc4
//
//  Created by chowray on 13-4-3.
//
//

#include "MailUI.h"
#include "../coc/ImgMgr.h"
#include "../coc/PlayerScene.h"
#include "../coc/ChatMgr.h"
#include "../coc/HttpClient.h"
#include "../coc/LogicInfo.h"
#include "../coc/ClanMgr.h"
#include "../GameWorldScene.h"

using namespace cocos2d;
using namespace coc;

int m_pageIndex = 0;
CCSize mailViewSize;

MailUI::MailUI()
{
    
}

MailUI::~MailUI()
{
    LogicInfo::getSingleton().removeListener(this);
}

bool MailUI::init()
{
    bool bRet = false;
    
    do
    {
        CC_BREAK_IF(!CUINode::init());
        
        {
            LogicInfo::getSingleton().addListener(this);
            
            CCNode* logBg = CUINodeCache::sharedUINodeCache()->UINodeFromFile("mail_bg.ccbi", this);
            this->setContentSize(logBg->getContentSize());
            this->setTargetTouch(true);
            this->addChild(logBg);
            
            mailViewSize = m_pView->getContentSize();
            
            m_pTableView = CCTableView::create(this, m_pView->getContentSize());
            m_pTableView->setDirection(kCCScrollViewDirectionVertical);
            m_pTableView->setDelegate(this);
            m_pTableView->reloadData();
            m_pTableView->setAnchorPoint(ccp(0, 0));
            m_pView->addChild(m_pTableView);
                        
            m_pLoading = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading1.ccbi", this);
            m_pLoading->setPosition(ccp(m_pView->getContentSize().width/2, m_pView->getContentSize().height/2));
            m_pView->addChild(m_pLoading);
            
            char str[128];
            for (int i = 0; i < 3; i++) {
                
                int nums = PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->countMailNums(i);
                sprintf(str, "%d", nums);
                ((CCLabelBMFont*)(m_pBtnPages[i]->getChildByTag(1))->getChildByTag(1))->setString(str);
                
                if (nums <= 0) {
                    (m_pBtnPages[i]->getChildByTag(1))->setVisible(false);
                }
                else {
                    (m_pBtnPages[i]->getChildByTag(1))->setVisible(true);
                }
            }
            
            m_pItemMenu = ItemMenuUI::create(m_pView);
            m_pItemMenu->setVisible(false);
            m_pView->addChild(m_pItemMenu);
            
            m_pNoTip = CCLabelBMFont::create("No News", "Font/tbFont_b.fnt", -1, kCCTextAlignmentCenter);
            m_pNoTip->setScale(0.8);
            m_pNoTip->setPosition(ccp(m_pView->getContentSize().width/2, m_pView->getContentSize().height*0.6));
            m_pNoTip->setVisible(false);
            m_pView->addChild(m_pNoTip);
            
            refreshPage();
            
            if (m_pageIndex == 0 && LogicInfo::getSingleton().m_vecDefenseLog.size() <= 0)
            {
                HttpClient::getSingleton().requestDefenseLog();
                m_pLoading->setVisible(true);
                m_pNoTip->setVisible(false);
            }
            else if (m_pageIndex == 1 && LogicInfo::getSingleton().m_vecAttackLog.size() <= 0)
            {
                HttpClient::getSingleton().requestAttackLog(0);
                m_pLoading->setVisible(true);
                m_pNoTip->setVisible(false);
            }
        }
        
        bRet = true;
    }while (0);
    
    return bRet;
}

void MailUI::refreshPage()
{
    for (int i = 0; i < 3; i++)
    {
        m_pBtnPages[i]->setEnabled(true);
    }
    m_pBtnPages[m_pageIndex]->setEnabled(false);
    
    m_pNoTip->setVisible(false);
    
    if (m_pageIndex == 0)
    {
        m_pLoading->setVisible(false);
        m_lstLog.clear();
        
        for(int i = 0; i < LogicInfo::getSingleton().m_vecDefenseLog.size(); ++i)
        {
            FightMsg msg = LogicInfo::getSingleton().m_vecDefenseLog[i];
            if(msg.msgid != -1)
            {
                m_lstLog.push_back(msg);
            }
        }
        
        m_pTableView->reloadData();
        
        CCFadeTo* pFadeTo = CCFadeTo::create(1.0f, 0);
        m_pBtnPages[0]->getChildByTag(1)->runAction(pFadeTo);
        pFadeTo = CCFadeTo::create(1.0f, 0);
        m_pBtnPages[0]->getChildByTag(1)->getChildByTag(1)->runAction(pFadeTo);
        
        CCUserDefault::sharedUserDefault()->setIntegerForKey("DEFENSE_LOG_READ_TIME", getCurrentTime());
        CCUserDefault::sharedUserDefault()->flush();
    }
    else if (m_pageIndex == 1)
    {
        m_pLoading->setVisible(false);
        m_lstLog.clear();
        
        for(int i = 0; i < LogicInfo::getSingleton().m_vecAttackLog.size(); ++i)
        {
            FightMsg msg = LogicInfo::getSingleton().m_vecAttackLog[i];
            if(msg.msgid != -1)
            {
                m_lstLog.push_back(msg);
            }
        }
        
        m_pTableView->reloadData();
        
        CCFadeTo* pFadeTo = CCFadeTo::create(1.0f, 0);
        m_pBtnPages[1]->getChildByTag(1)->runAction(pFadeTo);
        pFadeTo = CCFadeTo::create(1.0f, 0);
        m_pBtnPages[1]->getChildByTag(1)->getChildByTag(1)->runAction(pFadeTo);
        
        CCUserDefault::sharedUserDefault()->setIntegerForKey("ATTACK_LOG_READ_TIME", getCurrentTime());
        CCUserDefault::sharedUserDefault()->flush();
    }
    else if (m_pageIndex == 2)
    {
        m_pLoading->setVisible(false);
        m_lstLog.clear();
        
        m_pTableView->reloadData();
        
        CCFadeTo* pFadeTo = CCFadeTo::create(1.0f, 0);
        m_pBtnPages[2]->getChildByTag(1)->runAction(pFadeTo);
        pFadeTo = CCFadeTo::create(1.0f, 0);
        m_pBtnPages[2]->getChildByTag(1)->getChildByTag(1)->runAction(pFadeTo);
        
        CCUserDefault::sharedUserDefault()->setIntegerForKey("NEWS_READ_TIME", getCurrentTime());
        CCUserDefault::sharedUserDefault()->flush();
    }
    
    PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->countMailNums(-1);
}

bool MailUI::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    m_dragOffset = m_pTableView->getContentOffset();
    m_isDrag = false;
    
    return true;
}


void MailUI::menuPageCallback(CCObject* pSender, CCControlEvent event)
{
    int tag = ((CCControlButton*)pSender)->getTag();
    
    if (tag != m_pageIndex)
    {
        m_pageIndex = tag;
        refreshPage();
        
        if (m_pageIndex == 0)
        {
            if (LogicInfo::getSingleton().m_vecDefenseLog.size() <= 0) {
                HttpClient::getSingleton().requestDefenseLog();
                m_pLoading->setVisible(true);
                m_pNoTip->setVisible(false);
            }
        }
        else if(m_pageIndex == 1)
        {
            if (LogicInfo::getSingleton().m_vecAttackLog.size() <= 0) {
                HttpClient::getSingleton().requestAttackLog(0);
                m_pLoading->setVisible(true);
                m_pNoTip->setVisible(false);
            }
        }
        else if (m_pageIndex == 2)
        {
            if (LogicInfo::getSingleton().m_vecMail.size() <= 0) {
                HttpClient::getSingleton().requestMailInfo();
                m_pLoading->setVisible(true);
                m_pNoTip->setVisible(false);
            }
        }
    }
}

void MailUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
	(PlayerScene::getSingleton().m_pGameWorld)->hideMailUI();
}

bool MailUI::onNotify(int notifyid, const void* pVal)
{
	if(NOTIFY_MAIL_BATTLELOG == notifyid)
	{
        refreshPage();
        
        if (m_lstLog.size() == 0) {
            m_pNoTip->setString(ViewString::getValue("no_battlelog_tip").c_str());
            m_pNoTip->setVisible(true);
        }
    }
    else if (NOTIFY_MAIL_ATTACKLOG == notifyid)
    {
        refreshPage();
        
        if (m_lstLog.size() == 0) {
            m_pNoTip->setString(ViewString::getValue("no_battlelog_tip").c_str());
            m_pNoTip->setVisible(true);
        }
    }
    else if (NOTIFY_MAIL_INBOX == notifyid)
    {
        refreshPage();
        
        if (m_lstLog.size() == 0) {
            m_pNoTip->setString(ViewString::getValue("no_news_tip").c_str());
            m_pNoTip->setVisible(true);
        }
    }
    else if (NOTIFY_USER_ONLINE == notifyid)
    {
        PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI->addNote(ViewString::getSingleton().getValue("revenge_user_online").c_str());
    }
    else if (NOTIFY_USER_SHIELD == notifyid)
    {
        PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI->addNote(ViewString::getSingleton().getValue("revenge_user_shield").c_str());
    }
    else if (NOTIFY_USER_ATTACK == notifyid)
    {
        PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI->addNote(ViewString::getSingleton().getValue("revenge_user_attack").c_str());
    }

    return true;
}

bool MailUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    char name[128];
    for (int i = 0; i < 3; i++)
    {
        sprintf(name, "BtnPage%d", i);
        CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, name, CCControlButton*, m_pBtnPages[i]);
    }
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnClose", CCControlButton*, m_pBtnClose);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "MailView", CCLayer*, m_pView);
    
    return false;
}

SEL_CCControlHandler MailUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuPageCallback", MailUI::menuPageCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", MailUI::menuCloseCallback);
    return NULL;
}

CCSize MailUI::cellSizeForTable(CCTableView *table)
{
    return CCSizeMake(820, 170 + 5);
}

CCTableViewCell* MailUI::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
    CCTableViewCell *cell = table->dequeueCell();
    if (cell == NULL) {
        cell = new CCTableViewCell();
        cell->autorelease();
    }
    cell->setIdx(idx);
    cell->setAnchorPoint(ccp(0, 0));
    
    int index = (m_lstLog.size() - 1) - idx;
    if (m_pageIndex == 2)
    {
        index = (LogicInfo::getSingleton().m_vecMail.size()-1)-idx;
    }
    
    if (m_pageIndex == 2)
    {
        
        MailItem* mailItem = MailItem::create(index);
        mailItem->setTouchControlView(table);
        mailItem->setPositionX((m_pView->getContentSize().width-mailItem->getContentSize().width)/2);
        mailItem->setTag(1);
        cell->addChild(mailItem);
        
        mailItem->refresh();
    }
    else
    {
        FightMsg msg = m_lstLog[index];
        
        BattleLogItem* logItem = BattleLogItem::create(msg);
        logItem->setTouchControlView(m_pView);
        logItem->setPositionX((m_pView->getContentSize().width-logItem->getContentSize().width)/2);
        logItem->setTag(1);
        cell->addChild(logItem);
        
        logItem->refresh();
    }
    
    return cell;
}

void MailUI::tableCellTouched(CCTableView *table, CCTableViewCell *cell)
{
    if (m_pageIndex != 2) {
        CCSize cellSize = cellSizeForTable(table);
        
        BattleLogItem* item = (BattleLogItem*)cell->getChildByTag(1);
        
        if (item->m_isDrag) {
            return;
        }
        
        FightUserInfoEx2 userInfo;
        
        if (m_pageIndex == 0) {
            userInfo = item->m_fightMsg.fsi.user[0];
        }
        else {
            userInfo = item->m_fightMsg.fsi.user[1];
        }
        
        m_pItemMenu->resetMenu(userInfo.uid, userInfo.name, userInfo.clanid, -1);
        m_pItemMenu->setClanInfo(userInfo.clanname, userInfo.clanflag, userInfo.clanjointype, userInfo.clancup, userInfo.clanneedcup);
        
        m_pItemMenu->setPositionX(0.4*cellSize.width);
        m_pItemMenu->setPositionY(cell->getIdx() * cellSize.height + table->getContentOffset().y + cellSize.height/2);
        
        m_pItemMenu->adjuestPosition();
    }
    
}

unsigned int MailUI::numberOfCellsInTableView(CCTableView *table)
{
    if (m_pageIndex == 2)
    {
        return LogicInfo::getSingleton().m_vecMail.size();
    }
    else
    {
        return m_lstLog.size();
    }
}



BattleLogItem* BattleLogItem::create(FightMsg msg)
{
    BattleLogItem *pRet = new BattleLogItem(msg);
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

BattleLogItem::BattleLogItem(FightMsg msg)
{
    m_fightMsg = msg;
    m_isDrag = false;
}

BattleLogItem::~BattleLogItem()
{
    
}

bool BattleLogItem::init()
{
    if (CUINode::init())
    {
        CCNode* logNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("battlelog_item.ccbi", this, mailViewSize);
        this->setContentSize(logNode->getContentSize());
        this->addChild(logNode);
        
        this->setTouchEnabled(true);
        
        if (COC_CLAN == 0) {
        }
    }
    
    return true;
}

bool BattleLogItem::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    m_isDrag = false;
    m_dragOffset = m_pTroopsView->getContentOffset();
    
    return false;
}

void BattleLogItem::refresh()
{
    char str[128];
    {
        m_pClanName->setVisible(false);
        m_pClanIcon->setVisible(false);
        
        m_pBattleTime->setString(formatTimeAgo(m_fightMsg.time).c_str());
                
        FightScoreInfoEx3 info = m_fightMsg.fsi;
        
        FightUserInfoEx2 revenUser;
        
        if (m_pageIndex == 0)
        {
            revenUser = info.user[0];
        }
        else
        {
            revenUser = info.user[1];
        }
        
        m_pPlayerName->setString(revenUser.name);
        
        sprintf(str, "%d", (int)(revenUser.cup));
        m_pPlayerCup->setString(str);
        
        sprintf(str, "player_mr_%d",revenUser.military);
        CCSpriteFrame *frame = ImgMgr::getSingleton().getFrame(str);
        m_pIconMilitary->setDisplayFrame(frame);
        
        if (revenUser.clanid > 0) {
            
            m_pClanName->setString(revenUser.clanname);
            
            sprintf(str, "clan_icon_%d", revenUser.clanflag);
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
            m_pClanIcon->setDisplayFrame(pFrame);
            
            m_pClanName->setVisible(true);
            m_pClanIcon->setVisible(true);
        }
        
        sprintf(str, "%d", (int)info.goldex);
        m_pGainGold->setString(str);
        
        sprintf(str, "%d", (int)info.oilex);
        m_pGainElixir->setString(str);

        sprintf(str, "%d%%", (int)(info.progress * 100));
        m_pBattleProgress->setString(str);
        
        //troops
        char str[128];
        CCSize itemSize;
        for (int i = 0; i < m_fightMsg.fsi.numsfsi; i++)
        {
            int soldierId = m_fightMsg.fsi.fsi[i].pid;
            
            CCSprite* pItem = NULL;
            if (soldierId == CLAN_SOLDIER_ID) {
            
                pItem = ImgMgr::getSingleton().loadSprite("player0_0");
                pItem->setScale(0.5);
                sprintf(str, "clan_icon_%d", m_fightMsg.fsi.user[0].clanflag);
                CCSprite* pClanIcon = ImgMgr::getSingleton().loadSprite(str);
                pClanIcon->setPosition(ccp(pItem->getContentSize().width/2, pItem->getContentSize().height/2));
                pItem->addChild(pClanIcon);
            }
            else if (soldierId < 30000){
                sprintf(str, "player1_%d", (int)(soldierId / 100));
                pItem = ImgMgr::getSingleton().loadSprite(str);
                pItem->setAnchorPoint(ccp(0, 0));
                pItem->setScale(0.5);
                
                itemSize = CCSize(pItem->getContentSize().width * pItem->getScale()+2, pItem->getContentSize().height * pItem->getScale());
                
                sprintf(str, "x%d", (int)m_fightMsg.fsi.fsi[i].numsex);
                CCLabelBMFont* pNums = CCLabelBMFont::create(str, "Font/tbFont_b.fnt");
                pNums->setAnchorPoint(ccp(0, 0.5));
                pNums->setPosition(ccp(11, 25));
                pNums->setScale(0.7);
                pItem->addChild(pNums);
                
                CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("player_star");
                
                int n = (m_fightMsg.fsi.fsi[i].pid) % 10;
                for(int j = 0; j < n; ++j)
                {
                    CCSprite* pStar = CCSprite::createWithSpriteFrame(pFrame);
                    pStar->setPosition(ccp(18 + 17 * j, 50));
                    pItem->addChild(pStar, 10);
                }
            }
            else
            {
                sprintf(str, "player1_%d", (int)(soldierId / 100));
                pItem = ImgMgr::getSingleton().loadSprite(str);
                pItem->setAnchorPoint(ccp(0, 0));
                pItem->setScale(0.5);
            }
            
            pItem->setPosition(ccp(itemSize.width*i, 0));
            m_pTroopsView->addChild(pItem);
        }
        
        m_pTroopsView->setTouchEnabled(false);
        int width = itemSize.width * m_fightMsg.fsi.numsfsi;
        if (width > m_pTroopsView->getViewSize().width) {
            m_pTroopsView->setContentSize(CCSizeMake(width, m_pTroopsView->getViewSize().height));
            m_pTroopsView->setTouchEnabled(true);
        }
        
        for (int star = 1; star <= 3; star++)
        {
            if (m_fightMsg.star >= star)
            {
                CCSpriteFrame* spriteFrame = ImgMgr::getSingleton().getFrame("star_s_1");
                CCSprite* pStar = (CCSprite*)m_pStars->getChildByTag(star);
                pStar->setDisplayFrame(spriteFrame);
            }
        }
     
        int cup = m_fightMsg.defendCup;
        if (m_pageIndex == 1)
        {
            cup = m_fightMsg.attackCup;
        }
        
        sprintf(str, "%d", cup);
        m_pGainCup->setString(str);
        
        bool win = false;
        
        if (m_pageIndex == 0)
        {
            if (cup > 0) {
                m_pBattleResult->setString("Your Defense Won");
                win = true;
            }
            else {
                m_pBattleResult->setString("Your Defense Lost");
            }
        }
        else {
            if (cup > 0) {
                m_pBattleResult->setString("You Won");
                win = true;
            }
            else {
                m_pBattleResult->setString("You Lost");
            }
        }
        
        if (win) {
            CCSize contentSize = m_pBattleResultBg->getContentSize();
            CCSpriteFrame* spriteFrame = ImgMgr::getSingleton().getFrame("log_wonitem");
            m_pBattleResultBg->setSpriteFrame(spriteFrame);
            m_pBattleResultBg->setContentSize(contentSize);
            
            contentSize = m_pLogBg->getContentSize();
            spriteFrame = ImgMgr::getSingleton().getFrame("log_wonbg");
            m_pLogBg->setSpriteFrame(spriteFrame);
            m_pLogBg->setContentSize(contentSize);
        }
        else {
            CCSize contentSize = m_pBattleResultBg->getContentSize();
            CCSpriteFrame* spriteFrame = ImgMgr::getSingleton().getFrame("log_lostitem");
            m_pBattleResultBg->setSpriteFrame(spriteFrame);
            m_pBattleResultBg->setContentSize(contentSize);
            
            contentSize = m_pLogBg->getContentSize();
            spriteFrame = ImgMgr::getSingleton().getFrame("log_lostbg");
            m_pLogBg->setSpriteFrame(spriteFrame);
            m_pLogBg->setContentSize(contentSize);
        }
    
        if(m_fightMsg.canReplay())
        {
            m_pBtnReplay->setVisible(true);
            m_pNotReplay->setVisible(false);
            
            m_pBtnShare->setVisible(true);
        }
		else
		{
            m_pBtnReplay->setVisible(false);
            m_pNotReplay->setVisible(true);
            
            m_pBtnShare->setVisible(false);
        }
        
//        m_pBtnShare->setVisible(false);
        
        if (m_pageIndex == 0) {
            if(m_fightMsg.isAlreadyAttack == 0)
            {
                m_pBtnRevenge->setVisible(true);
                m_pNotRevenge->setVisible(false);
            }
            else
            {
                m_pBtnRevenge->setVisible(false);
                m_pNotRevenge->setVisible(true);
            }
        }
        else
        {
            m_pBtnRevenge->setVisible(false);
            m_pNotRevenge->setVisible(false);
        }
    }
}

bool BattleLogItem::checkTouchInside(CCObject* pSender)
{
    PlayerScene::getSingleton().m_pGameWorld->m_pMailUI->m_pItemMenu->setVisible(false);
    
    if (isTouchInside(((CCControlButton*)pSender)->getTouchPosition(), PlayerScene::getSingleton().m_pGameWorld->m_pMailUI->m_pBtnClose)) {
        PlayerScene::getSingleton().m_pGameWorld->m_pMailUI->menuCloseCallback(pSender, CCControlEventTouchDown);
        return false;
    }
    
    if(!isTouchInside(((CCControlButton*)pSender)->getTouchPosition(), PlayerScene::getSingleton().m_pGameWorld->m_pMailUI->m_pView)) {
        return false;
    }
    return true;
}

void BattleLogItem::menuReplayCallback(CCObject* pSender, CCControlEvent event)
{    
    if (!checkTouchInside(pSender))
    {
        return;
    }
    
    if (m_pageIndex == 0)
    {
        LogicInfo::getSingleton().m_fightReplayType = FIGHT_REPLAY_DEFENSE;
        LogicInfo::getSingleton().m_winCups = m_fightMsg.defendCup;
    }
    else if (m_pageIndex == 1)
    {
        LogicInfo::getSingleton().m_fightReplayType = FIGHT_REPLAY_ATTACK;
        LogicInfo::getSingleton().m_winCups = m_fightMsg.attackCup;
    }
    
    LogicInfo::getSingleton().chg2Replay(m_fightMsg, ATTACK_TYPE_PVP);

    PlayerScene::getSingleton().m_pGameWorld->hideMainUI();
    
    PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_NORMAL);
}

void BattleLogItem::menuRevengeCallback(CCObject* pSender, CCControlEvent event)
{
    if (!checkTouchInside(pSender))
    {
        return;
    }
    
    if (LogicInfo::getSingleton().m_revengeBattleId > 0)
    {
        return;
    }
    
    LogicInfo::getSingleton().refurbishFightSoldierInfo();
    if(LogicInfo::getSingleton().m_lstFightSoldier.size() == 0)
    {
        MessageUI::show(MSG_NO_SOLDIER, "", ViewString::getSingleton().getValue("no_soldier").c_str());
        return;
    }
    
    if(LogicInfo::getSingleton().m_myInfo.lastProtectTime > 0)
    {
        MessageUI *message = MessageUI::show(MSG_HAVE_PROTECT, "", ViewString::getSingleton().getValue("have_protect").c_str(), m_fightMsg.msgid, -1);
        message->setObject(&m_fightMsg);
        return;
    }
    
    HttpClient::getSingleton().requestRevenge(m_fightMsg.fsi.user[0].uid, m_fightMsg.msgid);
}

void BattleLogItem::menuShareCallback(CCObject *pSender, CCControlEvent event)
{
    if (!checkTouchInside(pSender))
    {
        return;
    }
    
    MessageUI *message = MessageUI::show(MSG_SHARE_REPLAY, ViewString::getSingleton().getValue("share_replay").c_str(), NULL, 0);
    message->setObject(&m_fightMsg);
}

bool BattleLogItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleTime", CCLabelBMFont*, m_pBattleTime);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerName", CCLabelTTF*, m_pPlayerName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerCup", CCLabelBMFont*, m_pPlayerCup);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GainGold", CCLabelBMFont*, m_pGainGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GainElixir", CCLabelBMFont*, m_pGainElixir);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GainCup", CCLabelBMFont*, m_pGainCup);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleProgress", CCLabelBMFont*, m_pBattleProgress);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleResult", CCLabelBMFont*, m_pBattleResult);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "NotReplay", CCLabelBMFont*, m_pNotReplay);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "NotRevenge", CCLabelBMFont*, m_pNotRevenge);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Stars", CCNode*, m_pStars);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "IconMilitary", CCSprite*, m_pIconMilitary);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanName", CCLabelTTF*, m_pClanName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ClanIcon", CCSprite*, m_pClanIcon);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TroopsView", CCScrollView*, m_pTroopsView);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BattleResultBg", CCScale9Sprite*, m_pBattleResultBg);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "LogBg", CCScale9Sprite*, m_pLogBg);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnReplay", CCControlButton*, m_pBtnReplay);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnRevenge", CCControlButton*, m_pBtnRevenge);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnShare", CCControlButton*, m_pBtnShare);
    
    return false;
}

SEL_CCControlHandler BattleLogItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuReplayCallback", BattleLogItem::menuReplayCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuRevengeCallback", BattleLogItem::menuRevengeCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuShareCallback", BattleLogItem::menuShareCallback);
    
    return NULL;
}

void BattleLogItem::scrollViewDidScroll(CCScrollView *view)
{
    if (fabs(view->getContentOffset().x - m_dragOffset.x) > 10) {
        m_isDrag = true;
        
        m_dragOffset = view->getContentOffset();
    }
    
    if (m_isDrag) {
        CCPoint offset = PlayerScene::getSingleton().m_pGameWorld->m_pMailUI->m_dragOffset;
        PlayerScene::getSingleton().m_pGameWorld->m_pMailUI->m_pTableView->setContentOffset(offset);
    }
}


MailItem* MailItem::create(int index)
{
    MailItem *pRet = new MailItem(index);
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

MailItem::MailItem(int index)
{
    m_index = index;
}

MailItem::~MailItem()
{
    
}

bool MailItem::init()
{
    if (CUINode::init())
    {
        CCNode* mailNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("mail_item.ccbi", this, mailViewSize);
        this->setContentSize(mailNode->getContentSize());
        this->addChild(mailNode);
    }
    
    return true;
}

void MailItem::refresh()
{
    MailInfo mailInfo = LogicInfo::getSingleton().m_vecMail[m_index];
    
    m_pFromName->setString(mailInfo.uname2.c_str());
    
    m_pTitle->setString(mailInfo.title.c_str());
    m_pMessage->setString(mailInfo.message.c_str());
    
    m_pTime->setString(formatTimeAgo(mailInfo.time, false).c_str());
    
    for (int i = 0; i < 3; i++)
    {
        m_pEarnAwads[i]->setVisible(false);
    }
    
    int index = 0;
    char str[128];
    
    if (mailInfo.state == 0)
    {
        
        CCSpriteFrame* pFrame = NULL;
        
        if (mailInfo.gold > 0)
        {
            pFrame = ImgMgr::getSingleton().getFrame("map_Coins_icon");
            m_pEarnAwads[index]->setDisplayFrame(pFrame);
            
            sprintf(str, "%d", mailInfo.gold);
            ((CCLabelBMFont*)m_pEarnAwads[index]->getChildByTag(1))->setString(str);
            m_pEarnAwads[index]->setVisible(true);
            index++;
        }
        
        if (mailInfo.elixir > 0)
        {
            pFrame = ImgMgr::getSingleton().getFrame("map_Elixir_icon");
            m_pEarnAwads[index]->setDisplayFrame(pFrame);
            sprintf(str, "%d", mailInfo.elixir);
            ((CCLabelBMFont*)m_pEarnAwads[index]->getChildByTag(1))->setString(str);
            m_pEarnAwads[index]->setVisible(true);
            index++;
        }
        
        if (mailInfo.gem > 0)
        {
            pFrame = ImgMgr::getSingleton().getFrame("map_Gems_icon");
            m_pEarnAwads[index]->setDisplayFrame(pFrame);
            sprintf(str, "%d", mailInfo.gem);
            ((CCLabelBMFont*)m_pEarnAwads[index]->getChildByTag(1))->setString(str);
            m_pEarnAwads[index]->setVisible(true);
            index++;
        }
    }
    
    if (index == 0)
    {
        m_pBtnGetAwards->setVisible(false);
    }
}

void MailItem::menuGetAwardsCallback(CCObject *pSender, CCControlEvent event)
{
    MailInfo* mailInfo = &LogicInfo::getSingleton().m_vecMail[m_index];
    
    LogicInfo::getSingleton().chgGold(mailInfo->gold);
    LogicInfo::getSingleton().chgOil(mailInfo->elixir);
    LogicInfo::getSingleton().chgGem(mailInfo->gem);
    
    LogicInfo::getSingleton().onNotify(NOTIFY_BASEINFO);
    
    HttpClient::getSingleton().getMailAward(mailInfo->mid);
    
    mailInfo->state = 1;
    
    refresh();
}

bool MailItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "FromName", CCLabelTTF*, m_pFromName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Title", CCLabelTTF*, m_pTitle);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Message", CCLabelTTF*, m_pMessage);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Time", CCLabelBMFont*, m_pTime);
    
    char str[128];
    for (int i = 0; i < 3; i++) {
        sprintf(str, "EarnAwads%d", (i+1));
        CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, str, CCSprite*, m_pEarnAwads[i]);
    }
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnGetAwards", CCControlButton*, m_pBtnGetAwards);
    
    return false;
}

SEL_CCControlHandler MailItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuGetAwardsCallback", MailItem::menuGetAwardsCallback);

    return NULL;
}