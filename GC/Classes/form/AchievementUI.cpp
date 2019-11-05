//
//  AchievementUI.cpp
//  GameClient
//
//  Created by chowray on 13-5-3.
//
//

#include "AchievementUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/PlayerScene.h"

#include "../coc/HttpClient.h"
#include "../coc/LogicInfo.h"
#include "../coc/AchievementInfo.h"
#include "CAppPlatform.h"

using namespace cocos2d;
using namespace coc;

CCSize achievementViewSize;

AchievementUI::AchievementUI()
{
    
}

AchievementUI::~AchievementUI()
{
    LogicInfo::getSingleton().removeListener(this);
}

bool AchievementUI::init()
{
    bool bRet = false;
    
    do{
        CC_BREAK_IF(!CUINode::init());
        {
            LogicInfo::getSingleton().addListener(this);
            
            CCNode* achievementUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("achievement_bg.ccbi", this);
            this->setContentSize(achievementUI->getContentSize());
            this->setTargetTouch(true);
            this->addChild(achievementUI);
            
            achievementViewSize = m_pView->getContentSize();
            
            m_pTableView = CCTableView::create(this, m_pView->getContentSize());
            m_pTableView->setDirection(kCCScrollViewDirectionVertical);
            m_pTableView->setDelegate(this);
            m_pView->addChild(m_pTableView);
            
            m_pLoading = NULL;
            
            if (LogicInfo::getSingleton().m_vecAchievement.size() <= 0) {
                
                m_pLoading = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading1.ccbi", this);
                m_pLoading->setPosition(ccp(m_pView->getContentSize().width/2, m_pView->getContentSize().height/2));
                m_pView->addChild(m_pLoading);
                
                HttpClient::getSingleton().requestAchievement();
            }
            
            m_lstAchievement = LogicInfo::getSingleton().m_vecAchievement;
            m_pTableView->reloadData();
            refresh();
            
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            m_pGameCenterNode->setVisible(false);
#endif
        }
        
        bRet = true;
    }
    while (0);
    
    return bRet;
}

void AchievementUI::refresh()
{
    int nums = 0;
    for(int i = 0; i < m_lstAchievement.size(); ++i)
    {
        nums += LogicInfo::getSingleton().getAchievementStar(m_lstAchievement[i]);
    }
    
    char str[128];
    sprintf(str, "%d/%d", nums, (int)m_lstAchievement.size()*3);
    
    m_pCompleted->setString(str);
}

void AchievementUI::menuGameCenterCallback(CCObject *pSender, CCControlEvent event)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    CAppPlatform::sharedAppPlatform()->showGameCenterLeaderboard(GAME_CENTER_TOP_ACHIEVEMENT);
#endif
}

void AchievementUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    (PlayerScene::getSingleton().m_pGameWorld)->hideAchievementUI();
    
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next();
    }
}

bool AchievementUI::onNotify(int notifyid, const void* pVal)
{
    if(NOTIFY_ACHIEVEMENT == notifyid)
	{
        if (m_pLoading)
            m_pLoading->setVisible(false);
        
        m_lstAchievement = LogicInfo::getSingleton().m_vecAchievement;
        
        m_pTableView->reloadData();
        refresh();
    }
    else if (NOTIFY_ACHIEVEMENT_OK == notifyid)
    {
        refresh();
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        
        int nums = 0;
        for(int i = 0; i < m_lstAchievement.size(); ++i)
        {
            nums += LogicInfo::getSingleton().getAchievementStar(m_lstAchievement[i]);
        }
        
        CAppPlatform::sharedAppPlatform()->reportScore(nums, GAME_CENTER_TOP_ACHIEVEMENT);
#endif
    }
    
    return true;
}

bool AchievementUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Completed", CCLabelBMFont*, m_pCompleted);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "AchievementView", CCLayer*, m_pView);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GameCenterNode", CCNode*, m_pGameCenterNode);
    
    return false;
}

SEL_CCControlHandler AchievementUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuGameCenterCallback", AchievementUI::menuGameCenterCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", AchievementUI::menuCloseCallback);
    return NULL;
}

void AchievementUI::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
}

CCSize AchievementUI::cellSizeForTable(CCTableView *table)
{
    return CCSizeMake(820, 130 + 10);
}

CCTableViewCell* AchievementUI::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
    int itemId = m_lstAchievement[m_lstAchievement.size()-1-idx];
    
    int level = LogicInfo::getSingleton().getAchievementStar(itemId);
    const AchievementNode* pA;
    if(level == 3)
    {
        pA = AchievementInfo::getSingleton().getNode(itemId * 100 + 100 + level - 1);
    }
    else
    {
        pA = AchievementInfo::getSingleton().getNode(itemId * 100 + 100 + level);
    }
    
    if (pA == NULL) {
        return NULL;
    }
    
    CCTableViewCell *cell = table->dequeueCell();
    if (!cell) {
        cell = new CCTableViewCell();
        cell->autorelease();
    }
    cell->setIdx(idx);
    
    AchievementItem* achievementItem = NULL;
    
    achievementItem = AchievementItem::create(itemId);
    achievementItem->m_cellIndex = idx;
    achievementItem->m_pTableView = table;
    achievementItem->setTouchControlView(table);
    achievementItem->setPositionX((m_pTableView->getContentSize().width-achievementItem->getContentSize().width)/2);
    achievementItem->setTag(1);
    cell->addChild(achievementItem);
    achievementItem->refresh();

    return cell;
}

unsigned int AchievementUI::numberOfCellsInTableView(CCTableView *table)
{
	return m_lstAchievement.size();
}

AchievementItem::AchievementItem(int id)
{
    m_achievementId = id;
}

AchievementItem::~AchievementItem()
{
    
}
    
AchievementItem* AchievementItem::create(int id)
{
    AchievementItem *pRet = new AchievementItem(id);
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

bool AchievementItem::init()
{
    if (CUINode::init()) {
        
        CCNode* achievementItem = CUINodeCache::sharedUINodeCache()->UINodeFromFile("achievement_item.ccbi", this, achievementViewSize);
        this->setContentSize(achievementItem->getContentSize());
        this->addChild(achievementItem);
        
        m_proCompletedSize = m_pProgressCompleted->getPreferredSize();
        return true;
    }
    
    return false;
}
    
void AchievementItem::refresh()
{
    char str[128];
    int level = LogicInfo::getSingleton().getAchievementStar(m_achievementId);
    const AchievementNode* pA;
    if(level == 3)
    {
        pA = AchievementInfo::getSingleton().getNode(m_achievementId * 100 + 100 + level - 1);
    }
    else
    {
        pA = AchievementInfo::getSingleton().getNode(m_achievementId * 100 + 100 + level);
    }
        
    int starNums = LogicInfo::getSingleton().getAchievementStar(m_achievementId);
    
    for (int star = 1; star <= 3; star++) {
        if (starNums >= star) {
            CCSpriteFrame* spriteFrame = ImgMgr::getSingleton().getFrame("star_b_1");
            CCSprite* pStar = (CCSprite*)m_pCompleteStars->getChildByTag(star);
            pStar->setDisplayFrame(spriteFrame);
        }
    }
    
    sprintf(str, "%s", pA->name.c_str());
    m_pName->setString(str);
    
    sprintf(str, "%s", pA->info.c_str());
    m_pDesc->setString(str);
    
    if (level == 3) {
        m_pCompletedTip->setVisible(true);
        m_pStatusNode->setVisible(false);
        
        m_pGetReward->setVisible(false);
    }
    else {
        m_pCompletedTip->setVisible(false);
        
        sprintf(str, "%d", pA->exp);
        m_pRewardsExp->setString(str);
        
        sprintf(str, "%d", pA->gem);
        m_pRewardsGems->setString(str);
        
        const AchievementNode* pNode = LogicInfo::getSingleton().getCurAchievementNode(m_achievementId);
        
        if (pNode) {
            float val = pNode->val;
            float progress = LogicInfo::getSingleton().getAchievementVal(m_achievementId);
            
            if (progress > val) {
                progress = val;
            }
            
            sprintf(str, "%d/%d", (int)progress, (int)val);
            float percent = progress/val;
            m_pCompleted->setString(str);
            
            m_pProgressCompleted->setVisible(true);
            m_pProgressCompletedBg->setVisible(true);
            
            changeProgress(m_pProgressCompleted, percent);
        }
        else {
            
            m_pProgressCompleted->setVisible(false);
            m_pProgressCompletedBg->setVisible(false);
        }
        
        if(LogicInfo::getSingleton().canRequireAchievementAward(m_achievementId))
        {
            m_pGetReward->setVisible(true);
        }
        else
        {
            m_pGetReward->setVisible(false);
        }
    }
}

void AchievementItem::getRewardsCallback(CCObject* pSender, CCControlEvent event)
{
    LogicInfo::getSingleton().requireAchievementAward(m_achievementId);
    this->refresh();
    PlayerScene::getSingleton().m_pGameWorld->m_pAchievementUI->refresh();
}

bool AchievementItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CompleteStars", CCSprite*, m_pCompleteStars);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Name", CCLabelBMFont*, m_pName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Desc", CCLabelBMFont*, m_pDesc);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ProgressCompleted", CCScale9Sprite*, m_pProgressCompleted);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ProgressCompletedBg", CCScale9Sprite*, m_pProgressCompletedBg);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Completed", CCLabelBMFont*, m_pCompleted);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RewardsExp", CCLabelBMFont*, m_pRewardsExp);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "RewardsGems", CCLabelBMFont*, m_pRewardsGems);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GetReward", CCControlButton*, m_pGetReward);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CompletedTip", CCLabelBMFont*, m_pCompletedTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "StatusNode", CCNode*, m_pStatusNode);
    
    return true;
}

SEL_CCControlHandler AchievementItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "getRewardsCallback", AchievementItem::getRewardsCallback);
   
    return NULL;
}
