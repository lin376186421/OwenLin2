#include "Shop.h"

#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/PlayerScene.h"
#include "../coc/BuildingMgr.h"
#include "../coc/BaseDef.h"
#include "../coc/GuideMgr.h"
#include "../coc/TownhallLevelInfo.h"
#include "../Framework/CUserData.h"

using namespace cocos2d;
using namespace coc;

enum ShopItemTag {
    ITEM_TAG_PRICE = 1,
    ITEM_TAG_NUM = 2,
    ITEM_TAG_NAME = 3,
    ITEM_TAG_TIME = 4,
    ITEM_TAG_FRAME = 5,
    ITEM_TAG_MONEY = 6
};

Shop::Shop(int id)
{
    m_shopId = id;
    m_shopList = LogicInfo::getSingleton().m_shoplst[m_shopId - 1];
    m_isFreeGemOn = LogicInfo::getSingleton().m_isShowFreeGemInBonus;
}

Shop::~Shop()
{
    LogicInfo::getSingleton().removeListener(this);
}

Shop* Shop::create(int id)
{
	Shop *pRet = new Shop(id);
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

bool Shop::init()
{
	bool bRet = false;
    do
    {
        CC_BREAK_IF(!CUINode::init());
        
        LogicInfo::getSingleton().addListener(this);
        
        const char* shopName[] = {"shop_treasure", "shop_resources", "shop_defenses", "shop_army",
            "shop_decorations", "shop_shield"};
        
        char shopFile[128];
        sprintf(shopFile, "%s.ccbi", shopName[m_shopId-1]);
        
//        CCNode* shopBg = CUINodeCache::sharedUINodeCache()->UINodeFromFile("shop_bg.ccbi", this);
//        this->addChild(shopBg);
        
        m_pShopNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile(shopFile, this);
//        setTargetTouch(true);
        setContentSize(m_pShopNode->getContentSize());
        this->addChild(m_pShopNode);
        
        m_pScrollView = (CCScrollView*)m_pShopNode->getChildByTag(1);
        m_pScrollView->setDelegate(this);
        m_nMaxItems = m_pScrollView->getContainer()->getChildrenCount();
        
        if (m_shopId == SHOP_TYPE_TREASURE)
        {
            if (m_isFreeGemOn)
            {
                CCNode *pNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("shop_treasureList_1.ccbi", this);
                m_pScrollView->setContainer(pNode);
                m_nMaxItems = m_pScrollView->getContainer()->getChildrenCount() - 1;
            }
        }
        
        m_pShopView = (CCLayer*)m_pScrollView->getContainer();
        
        if (m_shopId == SHOP_TYPE_TREASURE) {
//            if(LogicInfo::getSingleton().hasMask1_Pay())
//            {
                CCNode* pFreeBounsTip = m_pShopNode->getChildByTag(0);
                pFreeBounsTip->setVisible(false);
                CCLabelBMFont *tipInfo = (CCLabelBMFont *)pFreeBounsTip->getChildren()->objectAtIndex(0);
                tipInfo->setString(ViewString::getSingleton().getValue("shop_bonus_tip").c_str());
//            }
        }
        
        if (m_shopId == SHOP_TYPE_DECORATIONS)
        {
            m_pTableView = CCTableView::create(this, m_pScrollView->getViewSize());
            m_pTableView->setDirection(kCCScrollViewDirectionVertical);
            m_pTableView->setDelegate(this);
            m_pTableView->reloadData();
            m_pTableView->setAnchorPoint(ccp(0, 0));
            m_pTableView->setPosition(ccp(0, 0));
            m_pScrollView->setTouchEnabled(false);
            m_pScrollView->addChild(m_pTableView);
        }
        else
        {
            for (int index = 0; index < m_nMaxItems; index++)
            {
                this->refreshShopItem(index);
            }
            
            m_pScrollView->setContentOffset(ccp(0, m_pScrollView->getViewSize().height-m_pScrollView->getContentSize().height));
        }
        
        if (GuideMgr::getSingleton().m_pActiveBtn)
        {
            m_pScrollView->setTouchEnabled(false);
        }
        
        refreshMoney();
        
        m_isDrag = false;
        
		bRet = true;
	}while(0);
    
	return bRet;
}

// a selector callback
void Shop::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
	PlayerScene::getSingleton().m_pGameWorld->hideShop();
    PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->countShopNums();
    
}

// a selector callback
void Shop::menuBackCallback(CCObject* pSender, CCControlEvent event)
{
    if (m_shopId == SHOP_TYPE_FREEGEMS)
    {
        m_pShopNode->setVisible(true);
        m_shopId = SHOP_TYPE_TREASURE;
        this->removeChild(m_pShopItemNode);
    }
    else
    {
        PlayerScene::getSingleton().m_pGameWorld->showTypeShop();
        PlayerScene::getSingleton().m_pGameWorld->hideShop();
    }
}

void Shop::menuItemCallback(CCObject* pSender, CCControlEvent event)
{
    if (!checkTouchInside((CCNode*)pSender) && (event == CCControlEventTouchDown || event == CCControlEventTouchUpInside)) {
        return;
    }
    
    int tag = ((CCNode*)pSender)->getTag();
    
    if (event == CCControlEventTouchDown) {
        m_isDrag = false;
        m_dragOffset = m_pScrollView->getContentOffset();
    }
    else if (event == CCControlEventTouchUpInside || event == CCControlEventTouchCancel) {
        
        if (m_isDrag) {
            m_isDrag = false;
            return;
        }
        
        if (m_shopId == SHOP_TYPE_TREASURE) {
            
            if(tag < LogicInfo::getSingleton().getIAPNums())
            {
                LogicInfo::getSingleton().InPurchase(tag+1);
            }
            else if(tag == 12)
            {
                if(LogicInfo::getSingleton().m_defaultAdsType == 6)
                {
                    CAppPlatform::sharedAppPlatform()->showNativeXOfferAds(NATIVE_X_GET_GEMS);
                }
                else
                {
                    this->showFreeGemsShop();
                }
            }
            else
            {
                if(tag - LogicInfo::getSingleton().getIAPNums() < 3)
                {
                    int gold = getNums(tag - LogicInfo::getSingleton().getIAPNums());
                    int gem = LogicInfo::getSingleton().countOther2Gem(gold);
                    
                    char message[128];
                    sprintf(message, ViewString::getValue("buy_gold_confirm_message").c_str(), gold, gem);
                    
                    MessageUI::show(MSG_BUY_GOLD, ViewString::getValue("buy_gold_confirm_title").c_str(), message, gem, -1);
                }
                else
                {
                    int oil = getNums(tag - LogicInfo::getSingleton().getIAPNums());
                    int gem = LogicInfo::getSingleton().countOther2Gem(oil);
                    
                    char message[128];
                    sprintf(message, ViewString::getValue("buy_oil_confirm_message").c_str(), oil, gem);
                    
                    MessageUI::show(MSG_BUY_OIL, ViewString::getValue("buy_oil_confirm_title").c_str(), message, gem, -1);
                }
                
                for (int index = 0; index < m_nMaxItems; index++) {
                    this->refreshShopItem(index);
                }
            }
        }else if(m_shopId == SHOP_TYPE_SHIELD){
            
            int shieldTime = 0;
            if (tag == 0) {
                shieldTime = 1;
            }else if(tag == 1){
                shieldTime = 2;
            }else{
                shieldTime = 7;
            }
            
            int gem = LogicInfo::getSingleton().protectTimeGem(shieldTime);
            
            char message[128];
            sprintf(message, ViewString::getValue("buy_shield_confirm_message").c_str(), shieldTime, gem);
            
            MessageUI::show(MSG_BUY_SHIELD, ViewString::getValue("buy_shield_confirm_title").c_str(), message, shieldTime, -1);
        }
        else if(m_shopId == SHOP_TYPE_FREEGEMS)
        {
            if (tag == 1)
            {
                CAppPlatform::sharedAppPlatform()->showFlurryVideoAds();
            }
            else if (tag == 2)
            {
                CAppPlatform::sharedAppPlatform()->showTapJoyOfferswall();
                
            } else if (tag == 3)
            {
                CAppPlatform::sharedAppPlatform()->showChartBoostAds();
                
            }
            else if (tag == 4)
            {
                CAppPlatform::sharedAppPlatform()->showAarkiOfferAds();
            }
            else if (tag == 5)
            {
                CAppPlatform::sharedAppPlatform()->showSSAOfferAds();
            }
            else if (tag == 6)
            {
                CAppPlatform::sharedAppPlatform()->showNativeXOfferAds(NATIVE_X_GET_GEMS);
            }
        }
        else
        {
            const BuildingInfo* tmp = BuildingMgr::getSingleton().getBuildingInfo(m_shopList[tag]*100+1);
            
            if(tmp->buildres == MONEY_GOLD && tmp->buildcost > LogicInfo::getSingleton().m_myInfo.maxgoldex)
            {
                PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI->addNote(ViewString::getSingleton().getValue("uplevel_error_gold").c_str());
            }
            else if(tmp->buildres == MONEY_ELIXIR && tmp->buildcost > LogicInfo::getSingleton().m_myInfo.maxoilex)
            {
                PlayerScene::getSingleton().m_pGameWorld->m_pNotifyUI->addNote(ViewString::getSingleton().getValue("uplevel_error_oil").c_str());
            }
            else
            {
                LogicInfo::getSingleton().addBuilding(m_shopList[tag]*100+1, -1, -1);
                
                if (GuideMgr::getSingleton().isGuiding()) {
                    GuideMgr::getSingleton().next();
                }
                
                PlayerScene::getSingleton().m_pGameWorld->hideTypeShop();
                PlayerScene::getSingleton().m_pGameWorld->hideShop();
            }
        }
        
    }
}

void Shop::showFreeGemsShop()
{
    m_pShopNode->setVisible(false);
    m_pShopItemNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("shop_freegemes.ccbi", this);
    this->addChild(m_pShopItemNode);
    
    m_pScrollView = (CCScrollView*)m_pShopItemNode->getChildByTag(1);
    
    m_shopId = SHOP_TYPE_FREEGEMS;

    for (int i = 1; i<=6; i++)
    {
        refreshShopItem(i);
    }
    m_pScrollView->setContentOffset(ccp(0, m_pScrollView->getViewSize().height-m_pScrollView->getContentSize().height));
    
    if(LogicInfo::getSingleton().m_defaultAdsType > 0)
    {
        CCControlButton* shopItem = (CCControlButton*)m_pScrollView->getContainer()->getChildByTag(LogicInfo::getSingleton().m_defaultAdsType);
        if(shopItem)
        {
            menuItemCallback(shopItem, CCControlEventTouchCancel);
        }
        
    }

}

void Shop::refreshMoney()
{
//    char str[128];
//    sprintf(str, "%d", (int)LogicInfo::getSingleton().m_myInfo.goldex);
//    m_pGoldNums->setString(str);
//    
//    sprintf(str, "%d", (int)LogicInfo::getSingleton().m_myInfo.oilex);
//    m_pElixirNums->setString(str);
//    
//    sprintf(str, "%d", (int)LogicInfo::getSingleton().m_myInfo.gemex);
//    m_pGemNums->setString(str);
}

void Shop::refreshShopItem(int idx)
{
    char str[128];
    
    if (m_shopId == SHOP_TYPE_TREASURE) {
        CCControlButton* shopItem = (CCControlButton*)m_pShopView->getChildByTag(idx);
        if (idx < LogicInfo::getSingleton().getIAPNums()) {
            CCLabelBMFont* pPrice = (CCLabelBMFont*)shopItem->getChildByTag(ITEM_TAG_PRICE);
            sprintf(str, "%s", LogicInfo::getSingleton().getIAPInfo(idx)->strPrice.c_str());
            pPrice->setString(str);
            
            CCLabelBMFont* pNums = (CCLabelBMFont*)shopItem->getChildByTag(ITEM_TAG_NUM);
            sprintf(str, "%d", LogicInfo::getSingleton().getIAPInfo(idx)->gem);
            pNums->setString(str);
            
            const IapHighlightInfo *info = LogicInfo::getSingleton().m_iapGiftInfo.getIapGift(idx+1);
            if(info != NULL && LogicInfo::getSingleton().m_iapGiftInfo.corflag == 0)
            {
                CCSpriteFrame *frame = ImgMgr::getSingleton().getFrame("shop_best_deal");
                CCSprite *bestValue = CCSprite::createWithSpriteFrame(frame);
                bestValue->setPosition(ccp(shopItem->getContentSize().width*0.9, shopItem->getContentSize().height*0.60));
                bestValue->setScale(0.8);
                shopItem->addChild(bestValue, pPrice->getZOrder()-1);
                
                CCNode* pFreeBounsTip = m_pShopNode->getChildByTag(0);
                pFreeBounsTip->setVisible(true);
                
            }
            else if (idx == 5)
            {
                CCSpriteFrame *frame = ImgMgr::getSingleton().getFrame("shop_best_deal");
                CCSprite *bestValue = CCSprite::createWithSpriteFrame(frame);
                bestValue->setPosition(ccp(shopItem->getContentSize().width*0.9, shopItem->getContentSize().height*0.60));
                bestValue->setScale(0.8);
                shopItem->addChild(bestValue, pPrice->getZOrder()-1);
            }
            
        }else{
            int value = getValue(idx - LogicInfo::getSingleton().getIAPNums());
            
            if(value <= 0)
            {
                shopItem->setEnabled(false);
            }
            
            CCLabelBMFont* pPrice = (CCLabelBMFont*)shopItem->getChildByTag(ITEM_TAG_PRICE);
            
            int num = getNums(idx - LogicInfo::getSingleton().getIAPNums());
            sprintf(str, "%d", LogicInfo::getSingleton().countOther2Gem(num));
            pPrice->setString(str);
            
            if(LogicInfo::getSingleton().m_myInfo.gemex < LogicInfo::getSingleton().countOther2Gem(num))
            {
                pPrice->setColor(ccRED);
            }
            else
            {
                pPrice->setColor(ccWHITE);
            }
            
            CCLabelBMFont* pNums = (CCLabelBMFont*)shopItem->getChildByTag(ITEM_TAG_NUM);
            sprintf(str, "%d", num);
            pNums->setString(str);
        }
    }
    else if(m_shopId == SHOP_TYPE_SHIELD)
    {
        
    }
    else if(m_shopId == SHOP_TYPE_FREEGEMS)
    {
        CCControlButton* shopItem = (CCControlButton*)m_pScrollView->getContainer()->getChildByTag(idx);
        if (idx == 1)
        {
            if (LogicInfo::getSingleton().m_isFlurryAdOn)
                shopItem->setEnabled(false);//禁用flurry
            else
                shopItem->setEnabled(false);
            
        }
        else if (idx == 2)
        {
            if (LogicInfo::getSingleton().m_isTapjoyAdOn)
                shopItem->setEnabled(true);
            else
                shopItem->setEnabled(false);
        }
        else if (idx == 3)
        {
            if (LogicInfo::getSingleton().m_isChartboostFreeGemReady && LogicInfo::getSingleton().m_isChartboostAdOn)
                shopItem->setEnabled(true);
            else
                shopItem->setEnabled(false);
        }
        else if (idx == 4)
        {
            if (LogicInfo::getSingleton().m_isAarkiOn)
                shopItem->setEnabled(true);
            else
                shopItem->setEnabled(false);
        }
        else if (idx == 5)
        {
            if (LogicInfo::getSingleton().m_isSupersonicAdsOn)
                shopItem->setEnabled(true);
            else
                shopItem->setEnabled(false);
        }
        else if (idx == 6)
        {
            if (LogicInfo::getSingleton().m_isNativeXAdOn)
                shopItem->setEnabled(true);
            else
                shopItem->setEnabled(false);
        }
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
            if (LogicInfo::getSingleton().m_payType == PAY_TYPE_AMAZON && idx != 3)
            {
                shopItem->setEnabled(false);
            }
#endif
    }
    else
    {
        CCControlButton* shopItem = (CCControlButton*)m_pShopView->getChildByTag(idx);
        
        int curnum = TownhallLevelInfo::getSingleton().getBuildingCurNums(m_shopList[idx]);
        int maxnum = TownhallLevelInfo::getSingleton().getBuildingMaxNums(m_shopList[idx]);
        
        const BuildingInfo* info = BuildingMgr::getSingleton().getBuildingInfo(m_shopList[idx]*100 + 1);
        sprintf(str, "%d", (int)info->buildcost);
        if(info->buildingid/100 == BUILDING_BUILDER_HOME)
        {
            sprintf(str, "%d", LogicInfo::getSingleton().countCurWorkerBuildingPrice());
        }
        
        CCLabelBMFont* pPrice = (CCLabelBMFont*)shopItem->getChildByTag(ITEM_TAG_PRICE);
        pPrice->setString(str);
        
        if(info->buildcost > LogicInfo::getSingleton().getMoney(info->buildres))
            pPrice->setColor(ccRED);
        if(info->buildingid/100 == BUILDING_BUILDER_HOME)
        {
            if(LogicInfo::getSingleton().countCurWorkerBuildingPrice() > LogicInfo::getSingleton().getMoney(info->buildres))
                pPrice->setColor(ccRED);
        }
        
        
        CCLabelBMFont* itemName = (CCLabelBMFont*)shopItem->getChildByTag(ITEM_TAG_NAME);
        sprintf(str, "%s", info->name.c_str());
        itemName->setString(str);
        
        CCLabelBMFont* pNums = (CCLabelBMFont*)shopItem->getChildByTag(ITEM_TAG_NUM);
        
        if(maxnum > 0)
        {
            sprintf(str, "%d/%d", curnum, maxnum);
            pNums->setString(str);
        }
        else
        {
            sprintf(str, "%d/%d", curnum, maxnum);
            pNums->setString(str);
            shopItem->setEnabled(false);
        }
        if(curnum >= maxnum)
        {
            shopItem->setEnabled(false);
        }
    }
    
}

int Shop::getValue(int index)
{
    if(index == 0)
    {
        return LogicInfo::getSingleton().m_myInfo.maxgoldex * 0.9 - LogicInfo::getSingleton().m_myInfo.goldex;
    }
    else if(index == 1)
    {
        return LogicInfo::getSingleton().m_myInfo.maxgoldex * 0.5 - LogicInfo::getSingleton().m_myInfo.goldex;
    }
    else if(index == 2)
    {
        return LogicInfo::getSingleton().m_myInfo.maxgoldex - LogicInfo::getSingleton().m_myInfo.goldex;
    }
    else if(index == 3)
    {
        return LogicInfo::getSingleton().m_myInfo.maxoilex * 0.9 - LogicInfo::getSingleton().m_myInfo.oilex;
    }
    else if(index == 4)
    {
        return LogicInfo::getSingleton().m_myInfo.maxoilex * 0.5 - LogicInfo::getSingleton().m_myInfo.oilex;
    }
    else if(index == 5)
    {
        return LogicInfo::getSingleton().m_myInfo.maxoilex - LogicInfo::getSingleton().m_myInfo.oilex;
    }
    
    return 0;
}

int Shop::getNums(int index)
{
    int gem;
    int value;
    if(index == 0)
    {
        value = LogicInfo::getSingleton().m_myInfo.maxgoldex * 0.1;
        gem  = LogicInfo::getSingleton().countOther2Gem(value);
        return LogicInfo::getSingleton().countGem2Other(gem);
    }
    else if(index == 1)
    {
        value= LogicInfo::getSingleton().m_myInfo.maxgoldex * 0.5;
        gem  = LogicInfo::getSingleton().countOther2Gem(value);
        return LogicInfo::getSingleton().countGem2Other(gem);
    }
    else if(index == 2)
    {
        value= LogicInfo::getSingleton().m_myInfo.maxgoldex - LogicInfo::getSingleton().m_myInfo.goldex;
        gem  = LogicInfo::getSingleton().countOther2Gem(value);
        return LogicInfo::getSingleton().countGem2Other(gem);
    }
    else if(index == 3)
    {
        value= LogicInfo::getSingleton().m_myInfo.maxoilex * 0.1;
        gem  = LogicInfo::getSingleton().countOther2Gem(value);
        return LogicInfo::getSingleton().countGem2Other(gem);
    }
    else if(index == 4)
    {
        value= LogicInfo::getSingleton().m_myInfo.maxoilex * 0.5;
        gem  = LogicInfo::getSingleton().countOther2Gem(value);
        return LogicInfo::getSingleton().countGem2Other(gem);
    }
    else if(index == 5)
    {
        value= LogicInfo::getSingleton().m_myInfo.maxoilex - LogicInfo::getSingleton().m_myInfo.oilex;
        gem  = LogicInfo::getSingleton().countOther2Gem(value);
        return LogicInfo::getSingleton().countGem2Other(gem);
    }
    
    return 0;
}

bool Shop::onNotify(int notifyid, const void* pVal)
{
	if(NOTIFY_BASEINFO == notifyid)
	{
		refreshMoney();
        
        for (int index = 0; index < m_nMaxItems; index++)
        {
            this->refreshShopItem(index);
        }
	}
    
	return true;
}

bool Shop::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GoldNums", CCLabelBMFont*, m_pGoldNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ElixirNums", CCLabelBMFont*, m_pElixirNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GemNums", CCLabelBMFont*, m_pGemNums);
    
    return NULL;
}

SEL_CCControlHandler Shop::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuItemCallback", Shop::menuItemCallback);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", Shop::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", Shop::menuBackCallback);
    return NULL;
}

CCTableViewCell* Shop::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
    CCTableViewCell *cell = table->dequeueCell();
    if (cell == NULL) {
        cell = new CCTableViewCell();
        cell->autorelease();
    }
    cell->setIdx(idx);
    cell->setAnchorPoint(ccp(0, 0));
    
    char str[128];
    
    idx = ((m_shopList.size() - 1) / 2) - idx;
    
    for (int i = 0; i < 2 && (idx*2 + i) < m_shopList.size(); i ++) {
        CCNode* shopNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("shop_decorationsItem.ccbi", this);
        
        CUINode* pItemNode = CUINode::create();
        pItemNode->setTouchControlView(m_pShopView);
        pItemNode->addChild(shopNode);
        
        m_cellSize = shopNode->getContentSize();
        
        int posX = (i % 2) * m_cellSize.width;
        shopNode->setPosition(ccp(posX, 0));
        
        cell->addChild(pItemNode);
        CCControlButton* shopItem = (CCControlButton*)shopNode->getChildren()->objectAtIndex(0);
        shopItem->setTag(idx*2+i);
        //        shopItem->addTargetWithActionForControlEvents(this, cccontrol_selector(Shop::menuItemCallback), CCControlEventTouchUpInside);
        
        const BuildingInfo* info = BuildingMgr::getSingleton().getBuildingInfo(m_shopList[idx*2+i]*100+1);
        sprintf(str, "%s_2", info->res.c_str());
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        
        CCSprite* itemSprite = (CCSprite*)shopItem->getChildByTag(ITEM_TAG_FRAME);
        itemSprite->setDisplayFrame(pFrame);
        
        CCLabelBMFont* itemName = (CCLabelBMFont*)shopItem->getChildByTag(ITEM_TAG_NAME);
        sprintf(str, "%s", info->name.c_str());
        itemName->setString(str);
        
        CCLabelBMFont* itemTime = (CCLabelBMFont*)shopItem->getChildByTag(ITEM_TAG_TIME);
        sprintf(str, "None");
        itemTime->setString(str);
        
        CCLabelBMFont* itemPrice = (CCLabelBMFont*)shopItem->getChildByTag(ITEM_TAG_PRICE);
        sprintf(str, "%d", (int)info->buildcost);
        itemPrice->setString(str);
        
        if(info->buildcost > LogicInfo::getSingleton().getMoney(info->buildres))
            itemPrice->setColor(ccRED);
        else
            itemPrice->setColor(ccWHITE);
        
        if(info->buildres == MONEY_ELIXIR)
        {
            sprintf(str, "shop_Elixir_icon");
        }
        else if(info->buildres == MONEY_GOLD)
        {
            sprintf(str, "shop_Coins_icon");
        }
        else if(info->buildres == MONEY_GEM)
        {
            sprintf(str, "shop_Gems_icon");
        }
        
        CCSprite* itemMoney = (CCSprite*)shopItem->getChildByTag(ITEM_TAG_MONEY);
        
        pFrame = ImgMgr::getSingleton().getFrame(str);
        itemMoney->setDisplayFrame(pFrame);
        
        int curnum = TownhallLevelInfo::getSingleton().getBuildingCurNums(m_shopList[idx*2+i]);
        int maxnum = TownhallLevelInfo::getSingleton().getBuildingMaxNums(m_shopList[idx*2+i]);
        
        CCLabelBMFont* pNums = (CCLabelBMFont*)shopItem->getChildByTag(ITEM_TAG_NUM);
        
        if(maxnum > 0)
        {
            sprintf(str, "%d/%d", curnum, maxnum);
            pNums->setString(str);
        }
        else
        {
            shopItem->setEnabled(false);
        }
        if(curnum >= maxnum)
        {
            shopItem->setEnabled(false);
        }
        
    }
    
    return cell;
}

unsigned int Shop::numberOfCellsInTableView(CCTableView *table)
{
    return (m_shopList.size() + 1)/2;
}

CCSize Shop::cellSizeForTable(CCTableView *table)
{
    return CCSizeMake(m_cellSize.width, m_cellSize.height);
}

void Shop::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
    //CCLOG("cell touched at index: %i", cell->getIdx());
}

void Shop::scrollViewDidScroll(CCScrollView* view)
{
    if (fabs(m_dragOffset.y - view->getContentOffset().y) > 5) {
        m_isDrag = true;
        m_dragOffset = view->getContentOffset();
    }
}

bool Shop::checkTouchInside(CCNode* pSender)
{
    if (isTouchInside(((CCControlButton*)pSender)->getTouchPosition(), PlayerScene::getSingleton().m_pGameWorld->m_pTypeShop->m_pBtnClose)) {
        PlayerScene::getSingleton().m_pGameWorld->m_pTypeShop->menuCloseCallback(pSender, CCControlEventTouchDown);
        return false;
    }
    
//    CCRect rect1 = m_pScrollView->boundingBox();
//    CCRect rect2 = pSender->boundingBox();
//    
//    rect1.origin = CCPointZero;
//    rect2.origin = ccpAdd(rect2.origin, m_pScrollView->getContentOffset());
//    
//    if(!IsRectContains(rect1, rect2))
//       return false;
    
    if(!isTouchInside(((CCControlButton*)pSender)->getTouchPosition(), m_pScrollView)) {
        return false;
    }
    return true;
}
