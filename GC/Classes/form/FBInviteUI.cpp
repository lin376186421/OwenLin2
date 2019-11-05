//
//  FBInviteUI.cpp
//  GameClient
//
//  Created by chowray on 13-9-4.
//
//

#include "FBInviteUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/PlayerScene.h"
#include "../coc/ConfigValMgr.h"
#include "CAppPlatform.h"
#include "../coc/SoundMgr.h"
#include "CFacebookService.h"

using namespace cocos2d;
USING_NS_COC;

#define INVITE_MAX 20

FBInviteUI::FBInviteUI()
{
    m_isReady = false;
    
    m_selCount = 0;
}

FBInviteUI::~FBInviteUI()
{
    LogicInfo::getSingleton().removeListener(this);
//    FacebookMgr::getSingleton().fbClearFriends();
}

bool FBInviteUI::init()
{
	bool bRet = false;
    do
    {
		CC_BREAK_IF(!CUINode::init());
        
        LogicInfo::getSingleton().addListener(this);
        
        m_pCCBNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("ranking_friends.ccbi", this);
        this->setContentSize(m_pCCBNode->getContentSize());
        this->setTargetTouch(true);
        this->addChild(m_pCCBNode);
        
        m_pLoading = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading.ccbi", this);
        m_pLoading->setVisible(true);
        
        if (m_pView)
        {
            m_pLoading->setPosition(ccp(m_pView->getContentSize().width/2, m_pView->getContentSize().height/2));
            m_pView->addChild(m_pLoading,10);
        }
        else
        {
            m_pLoading->setPosition(ccp(getContentSize().width/2, getContentSize().height/2));
            addChild(m_pLoading,10);
        }
    
        if(FacebookMgr::getSingleton().m_fbFriendsList.size() == 0)
        {
            CFacebookService::sharedFaceBook()->fbGetFriends();
        }
        else
        {
            HttpClient::getSingleton().requestFBAvailableFriends();
        }
        
        m_pTableView = CCTableView::create(this, m_pView->getContentSize());
        m_pTableView->setDirection(kCCScrollViewDirectionVertical);
        m_pTableView->setDelegate(this);
        m_pTableView->setAnchorPoint(ccp(0, 0));
        m_pView->addChild(m_pTableView);
        m_pTableView->setVisible(false);
        
        
        m_pBtnAll->setEnabled(false);
        m_pBtnInvite->setEnabled(false);
        
        m_pNOFriends->setVisible(false);
        
        char str[124];
        
        sprintf(str, "0/%lu",FacebookMgr::getSingleton().m_fbFriendsList.size());
        m_pSelCount->setString(str);
        
        sprintf(str, "0");
        m_pFreeGem->setString(str);
        
        refreshInfo();
        
        
		bRet = true;
	}while(0);
    
	return bRet;
    
}
bool FBInviteUI::onNotify(int notifyid, const void* pVal)
{
    if(notifyid == NOTIFY_FB_FRIENDS_OK)
    {
        HttpClient::getSingleton().requestFBAvailableFriends();
    }
    else if (notifyid == NOTIFY_FB_INVITE_OK)
    {
        m_pTableView->reloadData();
        if(FacebookMgr::getSingleton().m_fbFriendsList.size() == 0)
            m_pNOFriends->setVisible(true);
    }
    else if (notifyid == NOTIFY_FB_FRIEND_CHECK_OK)
    {
        m_isReady = true;
        m_pLoading->setVisible(false);
        m_pTableView->setVisible(true);
        m_pTableView->reloadData();
        
        if(FacebookMgr::getSingleton().m_fbFriendsList.size() == 0)
        {
            m_pNOFriends->setVisible(true);
        }
        else
        {
            m_pBtnAll->setEnabled(true);
            m_pBtnInvite->setEnabled(true);
            char str[124];
            sprintf(str, "0/%lu",FacebookMgr::getSingleton().m_fbFriendsList.size());
            m_pSelCount->setString(str);
        }
    }
    return true;
}
void FBInviteUI::refreshInfo()
{
    for(int i = 0; i < FacebookMgr::getSingleton().m_fbFriendsList.size(); i++)
    {
        FBFriendInfo *finfo = FacebookMgr::getSingleton().m_fbFriendsList[i];
        if(FacebookMgr::getSingleton().m_inviteType == 0)
        	finfo->isSelect = false;
        else
        	finfo->isSelect = true;
        if(APP_DEBUG)
        {
            finfo->isSelect = false;
        }
    }
}



CCSize FBInviteUI::cellSizeForTable(CCTableView *table)
{
    return CCSizeMake(960, 95);
}

CCTableViewCell* FBInviteUI::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
    int rankIdx = numberOfCellsInTableView(table) - idx - 1;
    
    CCTableViewCell *cell = table->dequeueCell();
    if (cell == NULL) {
        cell = new CCTableViewCell();
        cell->autorelease();
    }
    
    cell->setIdx(idx);
    cell->setAnchorPoint(ccp(0, 0));

        if(rankIdx >= 0 && rankIdx < FacebookMgr::getSingleton().m_fbFriendsList.size())
        {
            FBFriendInfo *info = FacebookMgr::getSingleton().m_fbFriendsList[rankIdx];
            FBUserItem *item = FBUserItem::create(table->getViewSize(),this,info);
            item->setTag(1);
            item->setTouchControlView(table);
            cell->addChild(item);
        }
    
    return cell;
    
}

void FBInviteUI::tableCellTouched(CCTableView *table, CCTableViewCell *cell)
{
    FBUserItem* item = (FBUserItem*)cell->getChildByTag(1);
    if(item) {
        item->touchMySelf();
    }
}

unsigned int FBInviteUI::numberOfCellsInTableView(CCTableView *table)
{
    int size = 0;
    
    size = FacebookMgr::getSingleton().m_fbFriendsList.size();
    
//    if (size > 100) { // top 100
//        size += 2;
//    }
//    else if (size > 10) { //top 10
//        size += 1;
//    }
    
    return size;
}

bool FBInviteUI::changSelCount(bool isAdd)
{
     char str[124];
    if(m_selCount >= INVITE_MAX && isAdd)
    {
        sprintf(str, "%d friends at most",INVITE_MAX);
        PlayerScene::getSingleton().addNote(str);
        return false;
    }
    
    if(m_selCount <= 0 && !isAdd)
    {
        return false;
    }

    if(isAdd)
        m_selCount++;
    else
        m_selCount--;
   
    sprintf(str, "%d/%lu",m_selCount,FacebookMgr::getSingleton().m_fbFriendsList.size());
    m_pSelCount->setString(str);
    
    sprintf(str, "%d",m_selCount*5);
    
    m_pFreeGem->setString(str);
    
    if(m_selCount >= INVITE_MAX || m_selCount == FacebookMgr::getSingleton().m_fbFriendsList.size())
        m_pBtnAll->setEnabled(false);
    else
        m_pBtnAll->setEnabled(true);
    
    if(m_selCount <= 0)
        m_pBtnInvite->setEnabled(false);
    else
        m_pBtnInvite->setEnabled(true);
        
    return true;
}
void FBInviteUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideFBInviteUI();
//    FacebookMgr::getSingleton().fbClearFriends();
}

void FBInviteUI::menuAllCallback(CCObject* pSender, CCControlEvent event)
{
    int count = 0;
    for(int i = 0; i < FacebookMgr::getSingleton().m_fbFriendsList.size(); i++)
    {
        FBFriendInfo *finfo = FacebookMgr::getSingleton().m_fbFriendsList[i];
        if(i < 20)
        {
            count++;
            finfo->isSelect = true;
        }
        else
        {
            finfo->isSelect = false;
        }
    }
    m_selCount = count;
    char str[124];
    sprintf(str, "%d/%lu",m_selCount,FacebookMgr::getSingleton().m_fbFriendsList.size());
    m_pSelCount->setString(str);
    
    sprintf(str, "%d",m_selCount*5);
    
    m_pFreeGem->setString(str);
    
    if(m_selCount >= INVITE_MAX || m_selCount == FacebookMgr::getSingleton().m_fbFriendsList.size())
        m_pBtnAll->setEnabled(false);
    else
        m_pBtnAll->setEnabled(true);
    
    if(m_selCount <= 0)
        m_pBtnInvite->setEnabled(false);
    else
        m_pBtnInvite->setEnabled(true);
    
    m_pTableView->reloadData();
}

void FBInviteUI::menuInviteCallback(CCObject* pSender, CCControlEvent event)
{
    FacebookMgr::getSingleton().m_pendingInviteFriendIds.clear();
    for(int i = 0; i < FacebookMgr::getSingleton().m_fbFriendsList.size(); i++)
    {
        FBFriendInfo *finfo = FacebookMgr::getSingleton().m_fbFriendsList[i];
        if(finfo->isSelect)
            FacebookMgr::getSingleton().m_pendingInviteFriendIds.push_back(finfo->fid);
    }
    if(FacebookMgr::getSingleton().m_pendingInviteFriendIds.size() > 0)
    {
        CFacebookService::sharedFaceBook()->fbInviteFriends(FacebookMgr::getSingleton().m_pendingInviteFriendIds);
        PlayerScene::getSingleton().m_pGameWorld->hideFBInviteUI();
    }
}
bool FBInviteUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "MoreNode", CCNode*, m_pMoreNode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "View", CCLayer*, m_pView);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "NOFriends", CCLabelBMFont*, m_pNOFriends);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "FreeGem", CCLabelBMFont*, m_pFreeGem);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CountLabel", CCLabelBMFont*, m_pSelCount);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnAll", CCControlButton*, m_pBtnAll);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnInvite", CCControlButton*, m_pBtnInvite);
    return true;
}

SEL_CCControlHandler FBInviteUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", FBInviteUI::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuInviteCallback", FBInviteUI::menuInviteCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuAllCallback", FBInviteUI::menuAllCallback);
    return NULL;
}


FBUserItem::FBUserItem(CCSize parentSize, FBInviteUI *FBInviteLayer, FBFriendInfo *info)
{
    m_fbInfo = info;
    m_pFBInviteLayer = FBInviteLayer;
    m_size = parentSize;
    
}

FBUserItem::~FBUserItem()
{
    
}

FBUserItem* FBUserItem::create(CCSize parentSize, FBInviteUI *FBInviteLayer,FBFriendInfo *info)
{
    FBUserItem *pRet = new FBUserItem(parentSize,FBInviteLayer,info);
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
void FBUserItem::onExit()
{
    CCLOG("onExit");
    LogicInfo::getSingleton().removeListener(this);
    CUINode::onExit();
}
void FBUserItem::onEnter()
{
    CCLOG("onEnter");
    LogicInfo::getSingleton().addListener(this);
    CUINode::onEnter();
}

bool FBUserItem::init()
{
    if (CUINode::init()) {
                
        m_pItemNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("ranking_friendsitem_1.ccbi", this,m_size);
        this->setContentSize(m_pItemNode->getContentSize());
        this->addChild(m_pItemNode);
        
        this->setTouchEnabled(true);
        
        m_pAddFBSprite = NULL;
        
        m_sca = m_pFBSprite->getContentSize().width/50.0;
        
        m_isGetImg = false;
        
        refresh();
        
        return true;
    }
    
    return false;
}

bool FBUserItem::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    return false;
}
bool FBUserItem::onNotify(int notifyid, const void* pVal)
{
    if(NOTIFY_FB_GET_PIC_OK == notifyid)
    {
//        if(m_fbInfo->fSpriteFrame && m_pAddFBSprite == NULL)
        if(m_fbInfo->fSpriteFrame && m_isGetImg == false)
        {
            getImageComplete();
        }
    }
    
    return true;
}
void FBUserItem::getImageComplete()
{
//    if(m_fbInfo->fSpriteFrame && m_pAddFBSprite == NULL)
    if(m_fbInfo->fSpriteFrame && m_isGetImg == false)
    {
        m_isGetImg = true;
        
//        m_pAddFBSprite = CCSprite::createWithSpriteFrame(m_fbInfo->fSpriteFrame);
//        m_pAddFBSprite->setPosition(m_pFBSprite->getPosition());
//        m_pAddFBSprite->setAnchorPoint(m_pFBSprite->getAnchorPoint());
//        float sca = m_pFBSprite->getContentSize().width/m_fbInfo->fSpriteFrame->getRect().size.width;
//        m_pAddFBSprite->setScale(sca);
//        m_pItemNode->addChild(m_pAddFBSprite, m_pFBSprite->getZOrder()+1);
//        m_pFBSprite->setVisible(false);
        
        m_pFBSprite->setDisplayFrame(m_fbInfo->fSpriteFrame);
        m_pFBSprite->setScale(m_sca);

    }
}
void FBUserItem::refresh()
{
    if(m_fbInfo->fSpriteFrame)
    {
//        if(m_pAddFBSprite == NULL)
        if(m_isGetImg == false)
        {
            m_isGetImg = true;
            
//            m_pAddFBSprite = CCSprite::createWithSpriteFrame(m_fbInfo->fSpriteFrame);
//            m_pAddFBSprite->setPosition(m_pFBSprite->getPosition());
//            m_pAddFBSprite->setAnchorPoint(m_pFBSprite->getAnchorPoint());
//            float sca = m_pFBSprite->getContentSize().width/m_fbInfo->fSpriteFrame->getRect().size.width;
//            m_pAddFBSprite->setScale(sca);
//            m_pItemNode->addChild(m_pAddFBSprite, m_pFBSprite->getZOrder()+1);
//            m_pFBSprite->setVisible(false);
            
            
            m_pFBSprite->setDisplayFrame(m_fbInfo->fSpriteFrame);
            m_pFBSprite->setScale(m_sca);
            
        }
    }
    else if(m_pFBInviteLayer->m_isReady == true)
    {
        m_fbInfo->getFBUserImage((int)m_pFBSprite->getContentSize().width,(int)m_pFBSprite->getContentSize().height);
    }
    
    if(m_fbInfo->isSelect)
        m_pMarkSprite->setVisible(true);
    else
        m_pMarkSprite->setVisible(false);

    
    m_pFBName->setString(m_fbInfo->fname.c_str());
}
void FBUserItem::touchMySelf()
{
    SoundMgr::getSingleton().playEffect("btn.m4a", 0);
    bool res = true;
    if(m_fbInfo->isSelect)
        res = m_pFBInviteLayer->changSelCount(false);
    else
        res = m_pFBInviteLayer->changSelCount(true);
    if(res)
    {
        m_fbInfo->isSelect = !m_fbInfo->isSelect;
        m_pMarkSprite->setVisible(!m_pMarkSprite->isVisible());
    }
    
}
void FBUserItem::menuInviteCallback(CCObject* pSender, CCControlEvent event)
{
    FacebookMgr::getSingleton().m_pendingInviteFriendIds.clear();
    FacebookMgr::getSingleton().m_pendingInviteFriendIds.push_back(m_fbInfo->fid);
    CFacebookService::sharedFaceBook()->fbInviteFriends(FacebookMgr::getSingleton().m_pendingInviteFriendIds);
}
bool FBUserItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "FBName", CCLabelTTF*, m_pFBName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UserSprite", CCSprite*, m_pFBSprite);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "MarkSprite", CCSprite*, m_pMarkSprite);
    return false;
}

SEL_CCControlHandler FBUserItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuInviteCallback", FBUserItem::menuInviteCallback);
    
    return NULL;
}
