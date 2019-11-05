//
//  BonusUI.cpp
//  GameClient
//
//  Created by chowray on 13-9-4.
//
//

#include "BonusUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/PlayerScene.h"
#include "../coc/ConfigValMgr.h"
#include "CAppPlatform.h"

using namespace cocos2d;
USING_NS_COC;

BonusUI::BonusUI()
{
}

BonusUI::~BonusUI()
{
    
}

bool BonusUI::init()
{
	bool bRet = false;
    do
    {
		CC_BREAK_IF(!CUINode::init());
        
        
        CCNode* bonusUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("earn_gems_bg.ccbi", this);
        this->setContentSize(bonusUI->getContentSize());
        this->setTargetTouch(true);
        this->addChild(bonusUI);
        
        PlayerScene::getSingleton().m_pGameWorld->m_pBonusUI = this;

        
        m_bonusList = ConfigValMgr::getSingleton().filterBonusInfo("bonus");
        
        if(CCUserDefault::sharedUserDefault()->getBoolForKey("FB_LOGIN") == true && CCUserDefault::sharedUserDefault()->getBoolForKey("FB_INVITE") == false)
        {
            CCUserDefault::sharedUserDefault()->setBoolForKey("FB_INVITE", true);
            CCUserDefault::sharedUserDefault()->flush();
            
            CCNode* pBonuTip = PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->m_pBtnFreeBonus->getChildByTag(1);
            pBonuTip->setVisible(false);
            
        }
        
        CCTableView *m_pTableView;
        m_pTableView = CCTableView::create(this, m_pItemView->getContentSize());
        m_pTableView->setDirection(kCCScrollViewDirectionVertical);
        m_pTableView->setDelegate(this);
        m_pTableView->setAnchorPoint(ccp(0, 0));
        m_pItemView->addChild(m_pTableView);
        
		bRet = true;
	}while(0);
    
	return bRet;
    
}
CCSize BonusUI::cellSizeForTable(CCTableView *table)
{
    return CCSizeMake(960, 116);
}

CCTableViewCell* BonusUI::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
    int rankIdx = numberOfCellsInTableView(table) - idx - 1;
    
    CCTableViewCell *cell = table->dequeueCell();
    if (cell == NULL) {
        cell = new CCTableViewCell();
        cell->autorelease();
    }
    
    cell->setIdx(idx);
    cell->setAnchorPoint(ccp(0, 0));
    
    if(rankIdx >= 0 && rankIdx < m_bonusList.size())
    {
        BonusItemInfo bonusInfo = m_bonusList[rankIdx];
        int bonusId = bonusInfo.bonusId;
        BonusItem* bonusItem = BonusItem::create(bonusId);
        bonusItem->refreshInfo();
        cell->addChild(bonusItem);
    }
    
    return cell;
    
}

void BonusUI::tableCellTouched(CCTableView *table, CCTableViewCell *cell)
{

}

unsigned int BonusUI::numberOfCellsInTableView(CCTableView *table)
{
    int size = 0;
    
    size = m_bonusList.size();
    
    return size;
}
// a selector callback
void BonusUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
	PlayerScene::getSingleton().m_pGameWorld->hideBonusUI();
}

bool BonusUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ItemView", CCLayer*, m_pItemView);
    return true;
}

SEL_CCControlHandler BonusUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", BonusUI::menuCloseCallback);
    
    return NULL;
}

BonusItem::BonusItem(int id)
{
    m_bonusId = id;
}

BonusItem::~BonusItem()
{
    
}

BonusItem* BonusItem::create(int id)
{
    BonusItem *pRet = new BonusItem(id);
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

bool BonusItem::init()
{
    if (CUINode::init()) {
        
        CCSize parentSize = PlayerScene::getSingleton().m_pGameWorld->m_pBonusUI->m_pItemView->getContentSize();
        parentSize = CCSize(PlayerScene::getSingleton().m_pGameWorld->m_pBonusUI->m_pItemView->getContentSize().width, 0);
        CCNode* itemUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("earn_gems_item.ccbi", this, parentSize);
        
        this->setContentSize(itemUI->getContentSize());
        this->addChild(itemUI);
    }
    return true;
}

void BonusItem::refreshInfo()
{
    BonusItemInfo info = ConfigValMgr::getSingleton().getBonusItemInfo(m_bonusId);
    
    
    
    char str[128];
    sprintf(str, "%d", info.val);
    m_pGems->setString(str);
    
    
    char namestr[128];
    sprintf(namestr, "bonus_title_%d", m_bonusId);
    m_pName->setString(ViewString::getValue(namestr).c_str());
    
    char descstr[128];
    sprintf(descstr, "bonus_desc_%d", m_bonusId);
    m_pDesc->setString(ViewString::getValue(descstr).c_str());
    
    int num = 0;
    if (m_bonusId == BONUS_ID_RATE_TYPE) {
        
        string name = CCStringFormat(ViewString::getValue(namestr).c_str(), CAppPlatform::sharedAppPlatform()->getAppName().c_str());
        m_pName->setString(name.c_str());
        
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("bonus_0");
        m_pIcon->setDisplayFrame(pFrame);
        
        if (isUserMask(LogicInfo::getSingleton().m_myInfo.mask[0], USER_MASK1_GRADEGEM)) {
            num = 1;
            ((CCLabelBMFont*)m_pBtnEarn->getChildByTag(1))->setString(ViewString::getSingleton().getValue("bonus_get").c_str());
            m_pBtnEarn->setEnabled(false);
        }
        else if(CCUserDefault::sharedUserDefault()->getBoolForKey(RATE_FREE_GEMS))
        {
            num = 1;
            ((CCLabelBMFont*)m_pBtnEarn->getChildByTag(1))->setString(ViewString::getSingleton().getValue("bonus_get").c_str());
            
            if (isUserMask(LogicInfo::getSingleton().m_myInfo.mask[0], USER_MASK1_GRADEGEM)) {
                m_pBtnEarn->setEnabled(false);
            }
        }
    }
    else if (m_bonusId == BONUS_ID_FIRST_PURCHASE_TYPE)
    {
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("bonus_1");
        m_pIcon->setDisplayFrame(pFrame);
        
        if(isUserMask(LogicInfo::getSingleton().m_myInfo.mask[0], USER_MASK1_ALREADYPAY))
        {
            ((CCLabelBMFont*)m_pBtnEarn->getChildByTag(1))->setString(ViewString::getSingleton().getValue("bonus_get").c_str());
            
            num = 1;
            m_pBtnEarn->setEnabled(false);
        }
        
        string desc = CCStringFormat(ViewString::getValue(descstr), CAppPlatform::sharedAppPlatform()->getAppName().c_str());
        m_pDesc->setString(desc.c_str());
    }
    else if (m_bonusId == BONUS_ID_ADS_TYPE)
    {
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("map_Gems_icon");
        m_pIcon->setDisplayFrame(pFrame);
        m_pIcon->setScale(0.85);
//        m_pGems->setString("+");
        m_pGems->setVisible(false);
        m_pEarnTypeIcon->setVisible(false);
        m_pNums->setVisible(false);
    }
    else if (m_bonusId == BONUS_ID_GIFT_TYPE)
    {
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("bonus_2");
        m_pIcon->setDisplayFrame(pFrame);
        m_pIcon->setScale(0.6);
        
        std::map <int,IapHighlightInfo>::iterator it = LogicInfo::getSingleton().m_iapGiftInfo.corIapMap.begin();
        int productID = it->first;
        IapHighlightInfo *info = &(it->second);
        int num = LogicInfo::getSingleton().getIAPInfoFromProductID(productID)->gem;
        int percent = 100*info->totalGem/num+100;
        
        sprintf(descstr, ViewString::getValue("bonus_desc_3").c_str(), percent);
        m_pDesc->setString(descstr);
        
        sprintf(str, "+%d%%",percent);
        m_pGems->setString(str);
        m_pNums->setVisible(false);
        
    }
    else if (m_bonusId == BONUS_FB_LOGIN)
    {
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("map_Facebookicon");
        m_pIcon->setDisplayFrame(pFrame);
        m_pIcon->setScale(1);
        m_pNums->setVisible(false);
    }
    else if (m_bonusId == BONUS_FB_INVITE)
    {
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("map_Facebookicon");
        m_pIcon->setDisplayFrame(pFrame);
        m_pIcon->setScale(1);
        m_pNums->setVisible(false);
    }
    else if (m_bonusId == BOUNS_GP_COMMUNITY_TYPE)
    {
        string name = CCStringFormat(ViewString::getValue(namestr).c_str(), CAppPlatform::sharedAppPlatform()->getAppName().c_str());
        m_pName->setString(name.c_str());
        
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("map_googleicon");
        m_pIcon->setDisplayFrame(pFrame);
        
        
        if (isUserMask(LogicInfo::getSingleton().m_myInfo.mask[0], USER_MASK1_GOOGLE_PLUS)) {
            num = 1;
            ((CCLabelBMFont*)m_pBtnEarn->getChildByTag(1))->setString(ViewString::getSingleton().getValue("bonus_get").c_str());
            m_pBtnEarn->setEnabled(false);
        }
        else if(CCUserDefault::sharedUserDefault()->getBoolForKey(GP_FREE_GEMS))
        {
            num = 1;
            ((CCLabelBMFont*)m_pBtnEarn->getChildByTag(1))->setString(ViewString::getSingleton().getValue("bonus_get").c_str());
            
            if (isUserMask(LogicInfo::getSingleton().m_myInfo.mask[0], USER_MASK1_GOOGLE_PLUS)) {
                m_pBtnEarn->setEnabled(false);
            }
        }
    }
    
    sprintf(str, "%d/%d", num, info.nums);
    m_pNums->setString(str);
   
}

void BonusItem::menuEarnCallback(CCObject *pSender, CCControlEvent event)
{
    if(m_bonusId == BONUS_ID_RATE_TYPE)
    {
        if(CCUserDefault::sharedUserDefault()->getBoolForKey(RATE_FREE_GEMS))
        {
            LogicInfo::getSingleton().requestGradeGem();
        }
        else
        {
            CAppPlatform::sharedAppPlatform()->rateApp();
            PlayerScene::getSingleton().m_pGameWorld->hideBonusUI();
        }
    }
    else if (m_bonusId == BONUS_ID_FIRST_PURCHASE_TYPE)
    {
        if(isUserMask(LogicInfo::getSingleton().m_myInfo.mask[0], USER_MASK1_ALREADYPAY))
        {
            LogicInfo::getSingleton().requestFirstPayGem();
        }
        else
        {
            PlayerScene::getSingleton().m_pGameWorld->showShop(1);
            PlayerScene::getSingleton().m_pGameWorld->hideBonusUI();
        }
    }
    else if (m_bonusId == BONUS_ID_ADS_TYPE)
    {
        if(LogicInfo::getSingleton().m_defaultAdsType == 6)
        {
            CAppPlatform::sharedAppPlatform()->showNativeXOfferAds(NATIVE_X_GET_GEMS);
        }
        else
        {
            (PlayerScene::getSingleton().m_pGameWorld)->showShop(SHOP_TYPE_TREASURE);
            PlayerScene::getSingleton().m_pGameWorld->m_pShop->showFreeGemsShop();
        }
        
        PlayerScene::getSingleton().m_pGameWorld->hideBonusUI();
    }
    else if (m_bonusId == BONUS_ID_GIFT_TYPE)
    {
        PlayerScene::getSingleton().m_pGameWorld->showGiftUI();
        PlayerScene::getSingleton().m_pGameWorld->hideBonusUI();
    }
    else if (m_bonusId == BONUS_FB_LOGIN)
    {
        PlayerScene::getSingleton().m_pGameWorld->hideBonusUI();
        PlayerScene::getSingleton().m_pGameWorld->showFBLoginUI(0);
    }
    else if (m_bonusId == BONUS_FB_INVITE)
    {
        PlayerScene::getSingleton().m_pGameWorld->hideBonusUI();
        if(FacebookMgr::getSingleton().isLogin())
            PlayerScene::getSingleton().m_pGameWorld->showFBInviteUI();
        else
            PlayerScene::getSingleton().m_pGameWorld->showFBLoginUI(0);
    }
    else if (m_bonusId == BOUNS_GP_COMMUNITY_TYPE)
    {
        if(CCUserDefault::sharedUserDefault()->getBoolForKey(GP_FREE_GEMS))
        {
            LogicInfo::getSingleton().requestGooglePlusGem();
        }
        else
        {
            CAppPlatform::sharedAppPlatform()->openGooglePlusLink();
            PlayerScene::getSingleton().m_pGameWorld->hideBonusUI();
        }
    }
    
    refreshInfo();
}

bool BonusItem::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Icon", CCSprite*, m_pIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "EarnTypeIcon", CCSprite*, m_pEarnTypeIcon);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Name", CCLabelBMFont*, m_pName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Desc", CCLabelBMFont*, m_pDesc);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Nums", CCLabelBMFont*, m_pNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Gems", CCLabelBMFont*, m_pGems);
  
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnEarn", CCControlButton*, m_pBtnEarn);
    
    return true;
}

SEL_CCControlHandler BonusItem::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuEarnCallback", BonusItem::menuEarnCallback);
    
    return NULL;
}
