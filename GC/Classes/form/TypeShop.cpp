#include "TypeShop.h"
#include "../coc/ImgMgr.h"
#include "../coc/LogicInfo.h"
#include "../GameWorldScene.h"
#include "../coc/PlayerScene.h"
#include "../coc/GuideMgr.h"
#include "CUINodeCache.h"

using namespace cocos2d;
using namespace coc;

TypeShop::TypeShop()
{
}

TypeShop::~TypeShop()
{
    LogicInfo::getSingleton().removeListener(this);
}

bool TypeShop::init()
{
	bool bRet = false;
    do 
    {
		CC_BREAK_IF(!CUINode::init());
        
        LogicInfo::getSingleton().addListener(this);
        
        CCNode* shopbg = CUINodeCache::sharedUINodeCache()->UINodeFromFile("shop_bg.ccbi", this);
        this->addChild(shopbg);
        
        m_pShopMenu = CUINodeCache::sharedUINodeCache()->UINodeFromFile("shop_menu.ccbi", this);
        setTargetTouch(true);
        setContentSize(m_pShopMenu->getContentSize());
        this->addChild(m_pShopMenu);
        
        if (LogicInfo::getSingleton().m_bShowShopTips1 && LogicInfo::getSingleton().m_shopid != SHOP_TYPE_RESOURCES)
        {
            CCNode *pNode = m_pShopMenu->getChildByTag(1);
            CCSprite *pSprite = (CCSprite *)pNode->getChildByTag(1);
            int num = LogicInfo::getSingleton().showNums(SHOP_TYPE_RESOURCES);
            if (num>0)
            {
                if(num > 99)
                    num = 99;
                pSprite->setVisible(true);
                CCLabelBMFont *pLabel = (CCLabelBMFont *)pSprite->getChildByTag(1);
                pLabel->setString(CCStringFormat("%d",num).c_str());
            }
        }
        
        if (LogicInfo::getSingleton().m_bShowShopTips2 && LogicInfo::getSingleton().m_shopid != SHOP_TYPE_DEFENSES)
        {
            CCNode *pNode = m_pShopMenu->getChildByTag(2);
            CCSprite *pSprite = (CCSprite *)pNode->getChildByTag(1);
            int num = LogicInfo::getSingleton().showNums(SHOP_TYPE_DEFENSES);
            if (num>0)
            {
                if(num > 99)
                    num = 99;
                pSprite->setVisible(true);
                CCLabelBMFont *pLabel = (CCLabelBMFont *)pSprite->getChildByTag(1);
                pLabel->setString(CCStringFormat("%d",num).c_str());
            }
        }
        if (LogicInfo::getSingleton().m_bShowShopTips3 && LogicInfo::getSingleton().m_shopid != SHOP_TYPE_ARMY)
        {
            CCNode *pNode = m_pShopMenu->getChildByTag(3);
            CCSprite *pSprite = (CCSprite *)pNode->getChildByTag(1);
            int num = LogicInfo::getSingleton().showNums(SHOP_TYPE_ARMY);
            if (num>0)
            {
                if(num > 99)
                    num = 99;
                pSprite->setVisible(true);
                CCLabelBMFont *pLabel = (CCLabelBMFont *)pSprite->getChildByTag(1);
                pLabel->setString(CCStringFormat("%d",num).c_str());
            }
        }
        guide();
        refreshMoney();
        
		bRet = true;
	}while(0);

	return bRet;
}

// a selector callback
void TypeShop::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideShop();
	PlayerScene::getSingleton().m_pGameWorld->hideTypeShop();
    
    PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->countShopNums();
	//getParent()->removeChild(this, false);
}

// a selector callback
void TypeShop::menuBackCallback(CCObject* pSender, CCControlEvent event)
{
	(PlayerScene::getSingleton().m_pGameWorld)->hideTypeShop();
	//getParent()->removeChild(this, false);
}

void TypeShop::menuShopCallback1(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideShop();
	PlayerScene::getSingleton().m_pGameWorld->showShop(SHOP_TYPE_TREASURE);
}

void TypeShop::menuShopCallback2(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideShop();
	PlayerScene::getSingleton().m_pGameWorld->showShop(SHOP_TYPE_RESOURCES);
    
    LogicInfo::getSingleton().m_bShowShopTips1 = false;
    CCUserDefault::sharedUserDefault()->setBoolForKey("shopTips1", false);
    CCUserDefault::sharedUserDefault()->flush();
    
    CCNode *pNode = m_pShopMenu->getChildByTag(1);
    CCSprite *pSprite = (CCSprite *)pNode->getChildByTag(1);
    pSprite->setVisible(false);
    
    PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->countShopNums();
    
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next();
        
        if (GuideMgr::getSingleton().val1 == GUIDE_BUILD_GOLD_STORAGE) {
            
            CCScrollView* pScrollView = PlayerScene::getSingleton().m_pGameWorld->m_pShop->m_pScrollView;
            pScrollView->setContentOffset(ccp(pScrollView->getContentOffset().x, 0));
            
            GuideMgr::getSingleton().refreshArrowPos();
        }
    }
}

void TypeShop::menuShopCallback3(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideShop();
	PlayerScene::getSingleton().m_pGameWorld->showShop(SHOP_TYPE_DEFENSES);
    
    LogicInfo::getSingleton().m_bShowShopTips2 = false;
    CCUserDefault::sharedUserDefault()->setBoolForKey("shopTips2", false);
    CCUserDefault::sharedUserDefault()->flush();
    
    CCNode *pNode = m_pShopMenu->getChildByTag(2);
    CCSprite *pSprite = (CCSprite *)pNode->getChildByTag(1);
    pSprite->setVisible(false);
    
    PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->countShopNums();
    
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next();
    }
}

void TypeShop::menuShopCallback4(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideShop();
	PlayerScene::getSingleton().m_pGameWorld->showShop(SHOP_TYPE_ARMY);
    
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next();
    }
    
    LogicInfo::getSingleton().m_bShowShopTips3 = false;
    CCUserDefault::sharedUserDefault()->setBoolForKey("shopTips3", false);
    CCUserDefault::sharedUserDefault()->flush();
    
    CCNode *pNode = m_pShopMenu->getChildByTag(3);
    CCSprite *pSprite = (CCSprite *)pNode->getChildByTag(1);
    pSprite->setVisible(false);
    
    PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->countShopNums();
}

void TypeShop::menuShopCallback5(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideShop();
	PlayerScene::getSingleton().m_pGameWorld->showShop(SHOP_TYPE_DECORATIONS);
}

void TypeShop::menuShopCallback6(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideShop();
	PlayerScene::getSingleton().m_pGameWorld->showShop(SHOP_TYPE_SHIELD);
}

void TypeShop::refreshMoney()
{
    char str[128];
	sprintf(str, "%d", (int)LogicInfo::getSingleton().m_myInfo.goldex);
    m_pGoldNums->setString(str);
    
    sprintf(str, "%d", (int)LogicInfo::getSingleton().m_myInfo.oilex);
    m_pElixirNums->setString(str);
    
    sprintf(str, "%d", (int)LogicInfo::getSingleton().m_myInfo.gemex);
    m_pGemNums->setString(str);
}

bool TypeShop::onNotify(int notifyid, const void* pVal)
{
	if(NOTIFY_BASEINFO == notifyid)
	{
		refreshMoney();
	}
    
	return true;
}

void TypeShop::guide()
{
    if(LogicInfo::getSingleton().m_localGuide == -1)
        return;
}

bool TypeShop::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GoldNums", CCLabelBMFont*, m_pGoldNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ElixirNums", CCLabelBMFont*, m_pElixirNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GemNums", CCLabelBMFont*, m_pGemNums);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnClose", CCControlButton*, m_pBtnClose);
    
    return NULL;
}

SEL_CCControlHandler TypeShop::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuShopCallback1", TypeShop::menuShopCallback1);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuShopCallback2", TypeShop::menuShopCallback2);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuShopCallback3", TypeShop::menuShopCallback3);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuShopCallback4", TypeShop::menuShopCallback4);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuShopCallback5", TypeShop::menuShopCallback5);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuShopCallback6", TypeShop::menuShopCallback6);
    
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", TypeShop::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", TypeShop::menuBackCallback);
    return NULL;
}
