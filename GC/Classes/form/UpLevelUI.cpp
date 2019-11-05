//
//  UpLevelUI.cpp
//  coc4
//
//  Created by chowray on 13-4-8.
//
//

#include "UpLevelUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/PersonMgr.h"
#include "../coc/PlayerScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/Building.h"
#include "../coc/BuildingMgr.h"
#include "../coc/BaseDef.h"
#include "../coc/TownhallLevelInfo.h"
#include "../coc/GuideMgr.h"


using namespace cocos2d;
USING_NS_COC;

UpLevelUI::UpLevelUI()
{
    
}

UpLevelUI::~UpLevelUI()
{
    unscheduleUpdate();
}

bool UpLevelUI::init()
{
    bool bRet = false;
    
    do{
        
        CC_BREAK_IF(!CUINode::init());
        {
            CCNode* upgradeUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("upgrade_item_0.ccbi", this);
            
            this->setContentSize(upgradeUI->getContentSize());
            this->setTargetTouch(true);
            this->addChild(upgradeUI);
            
            m_pScrollView = CCScrollView::create(m_pItemView->getContentSize());
            m_pScrollView->setDirection(kCCScrollViewDirectionHorizontal);
            m_pItemView->addChild(m_pScrollView);
            
            if(PlayerScene::getSingleton().m_pCurBuilding != NULL)
            {
                BuildingInfo info = PlayerScene::getSingleton().m_pCurBuilding->m_info;
                if (info.buildingid / 100 == BUILDING_TOWN_HALL) {
                    m_pTownHallItems = CUINodeCache::sharedUINodeCache()->UINodeFromFile("uplevel_item.ccbi", this);
                }
            }
        }
        bRet = true;
    }
    while (0);
    
    m_bUpgrade = true;
    refreshInfo();
    
    scheduleUpdate();
    
    return bRet;
}

void UpLevelUI::update(float dt)
{
    if(PlayerScene::getSingleton().m_pCurBuilding == NULL)
        return;
    
    Building* pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
    
    const BuildingInfo* info = &pBuilding->m_info;
    
    int buildid = info->buildingid / 100;
    
    char str[128];
    
    if(buildid == BUILDING_GOLD_MINE || buildid == BUILDING_ELIXIR_COLLECTOR)
    {
        int res = pBuilding->countRes();
        
        float percent = (float)res / pBuilding->m_info.maxstored[pBuilding->m_info.producesres - 1];
        changeProgress((CCScale9Sprite*)m_pBuildingAtt3->getChildByTag(ATT_TAG_PROGRESS), percent);
        
        sprintf(str, "%d/%d", res, (int)pBuilding->m_info.maxstored[pBuilding->m_info.producesres - 1]);
        ((CCLabelBMFont*)m_pBuildingAtt3->getChildByTag(ATT_TAG_VALUE))->setString(str);
    }
}

// a selector callback
void UpLevelUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
	(PlayerScene::getSingleton().m_pGameWorld)->hideUpLevelUI();
}

// a selector callback
void UpLevelUI::menuBackCallback(CCObject* pSender, CCControlEvent event)
{
	(PlayerScene::getSingleton().m_pGameWorld)->hideUpLevelUI();
}

// a selector callback
void UpLevelUI::menuUpCallback(CCObject* pSender, CCControlEvent event)
{
    Building* pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
	if(pBuilding != NULL)
	{
		if(pBuilding->m_pBuildingInfo != NULL)
		{
			int need = 0;
            
            int ret = LogicInfo::getSingleton().upLevel(*pBuilding, need);
            if(ret <= 0)
            {
                MessageUI::show(ret, NULL, NULL, need, GETGEM_UPLEVEL_BUILDING);
            }
            
            if (GuideMgr::getSingleton().isGuiding()) {
                GuideMgr::getSingleton().next();
            }
		}
	}
    
	PlayerScene::getSingleton().m_pGameWorld->hideUpLevelUI();
}

void UpLevelUI::refreshInfo()
{
    if(PlayerScene::getSingleton().m_pCurBuilding == NULL)
        return;
    
    m_pDesc = NULL;
    
    InfoUI::refreshInfo();
    
    BuildingInfo info = PlayerScene::getSingleton().m_pCurBuilding->m_info;
    
    int buildingid = info.buildingid;
    int bid = buildingid / 100;
    
    const BuildingInfo* info2 = BuildingMgr::getSingleton().getBuildingInfo(info.buildingid + 1);
    if(info2 == NULL)
    {
        m_pBtnUpgrade->setVisible(false);
        return;
    }
    
    const BuildingInfo* pMaxBuilding = BuildingMgr::getSingleton().getBuildingMaxInfo(bid);
    
    char str[1024];
    
    if(bid == BUILDING_CLAN_CASTLE)
    {
        sprintf(str, ViewString::getSingleton().getValue("upgrade_title").c_str(), (info.buildingid % 100));
    }
    else
    {
        sprintf(str, ViewString::getSingleton().getValue("upgrade_title").c_str(), (info.buildingid % 100) + 1);
    }
    m_pTitle->setString(str);
    
	m_pName->setString(info.name.c_str());
    
    int d = info2->buildtime[0];
    int h = info2->buildtime[1];
    int m = info2->buildtime[2];
    sprintf(str, "%s", "");
    if(d > 0)
        sprintf(str, "%dD", d);
    if(h > 0)
        sprintf(str, "%s %dH", str, h);
    if(m > 0)
        sprintf(str, "%s %dM", str, m);
    
    if(strlen(str) == 0)
    {
        sprintf(str, "%s", ViewString::getSingleton().getValue("none").c_str());
    }
    
    m_pUpgradeTime->setString(str);
    
    float percent = (float)info2->hp / pMaxBuilding->hp;
    changeProgress((CCScale9Sprite*)m_pBuildingAtt1->getChildByTag(ATT_TAG_PROGRESS_ADD), percent);

    if (bid == BUILDING_CLAN_CASTLE && info.buildingid % 100 == 1) {
        sprintf(str, ViewString::getSingleton().getValue("playerupinfo_hitpoint").c_str(), info.hp);
    }
    else {
        sprintf(str, ViewString::getSingleton().getValue("playeruplevel_hitpoint").c_str(), info.hp, info2->hp - info.hp);
    }
    
    ((CCLabelBMFont*)m_pBuildingAtt1->getChildByTag(ATT_TAG_VALUE))->setString(str);
    
    m_pUnlockTip->setVisible(false);
    m_pUpgradeDesc->setVisible(false);
    
    if(bid == BUILDING_TOWN_HALL)
    {
        sprintf(str, ViewString::getSingleton().getValue("uplevel_capacity").c_str(), (int)info.maxstored[0], (int)pMaxBuilding->maxstored[0]);
        ((CCLabelBMFont*)m_pBuildingAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS), 1.0);
        
        sprintf(str, ViewString::getSingleton().getValue("uplevel_capacity").c_str(), (int)info.maxstored[1], (int)pMaxBuilding->maxstored[1]);
        ((CCLabelBMFont*)m_pBuildingAtt3->getChildByTag(ATT_TAG_VALUE))->setString(str);
        changeProgress((CCScale9Sprite*)m_pBuildingAtt3->getChildByTag(ATT_TAG_PROGRESS), 1.0);
    }
    else if(bid == BUILDING_ELIXIR_COLLECTOR || bid == BUILDING_GOLD_MINE)
    {
        float percent = (float)info2->producesperhour / pMaxBuilding->producesperhour;
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS_ADD), percent);
        
        sprintf(str, ViewString::getSingleton().getValue("uplevel_production_rate").c_str(), (int)info.producesperhour, info2->producesperhour - info.producesperhour);
        ((CCLabelBMFont*)m_pBuildingAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
    }
    else if(bid == BUILDING_ELIXIR_STORAGE)
    {
        float percent = (float)info.maxstored[1] / pMaxBuilding->maxstored[1];
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS), percent);
        percent = (float)info2->maxstored[1] / pMaxBuilding->maxstored[1];
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS_ADD), percent);
        
        sprintf(str, ViewString::getSingleton().getValue("uplevel_storage_capacity").c_str(), (int)info.maxstored[1], info2->maxstored[1] - info.maxstored[1]);
        ((CCLabelBMFont*)m_pBuildingAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
    }
    else if(bid == BUILDING_GOLD_STORAGE)
    {
        float percent = (float)info.maxstored[0] / pMaxBuilding->maxstored[0];
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS), percent);
        percent = (float)info2->maxstored[0] / pMaxBuilding->maxstored[0];
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS_ADD), percent);
        
        sprintf(str, ViewString::getSingleton().getValue("uplevel_storage_capacity").c_str(), (int)info.maxstored[0], info2->maxstored[0] - info.maxstored[0]);
        ((CCLabelBMFont*)m_pBuildingAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
    }
    else if(bid == BUILDING_CANNON || bid == BUILDING_ARCHER_TOWER || bid == BUILDING_BIG_MORTAR
            || bid == BUILDING_AIR_DEFENSE || bid == BUILDING_WIZARD_TOWER)
    {
        
        float percent = (float)info2->damage / pMaxBuilding->damage;
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS_ADD), percent);
        
        sprintf(str, ViewString::getSingleton().getValue("uplevel_damage").c_str(), info.damage, info2->damage - info.damage);
        ((CCLabelBMFont*)m_pBuildingAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
    }
    else if(bid == BUILDING_BARRACKS)
    {
    }
    else if(bid == BUILDING_CLAN_CASTLE)
    {
        float percent = (float)info.housingspace / pMaxBuilding->housingspace;
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS), percent);
        
        percent = (float)info2->housingspace / pMaxBuilding->housingspace;
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS_ADD), percent);
        
        if (buildingid % 100 == 1) {
            sprintf(str, ViewString::getSingleton().getValue("player_troop_capacity").c_str(), info2->housingspace);
        }
        else {
            sprintf(str, ViewString::getSingleton().getValue("playeruplevel_troop_capacity").c_str(), info.housingspace, info2->housingspace-info.housingspace);
        }
    
        ((CCLabelBMFont*)m_pBuildingAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
        
        CCSpriteFrame *pFrame = ImgMgr::getSingleton().getFrame("info_icon_person");
        ((CCSprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_ICON))->setDisplayFrame(pFrame);
    }
    else if(bid == BUILDING_ARMY_CAMP)
    {
        float percent = (float)info.housingspace / pMaxBuilding->housingspace;
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS), percent);
        
        percent = (float)info2->housingspace / pMaxBuilding->housingspace;
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS_ADD), percent);

        sprintf(str, ViewString::getSingleton().getValue("playeruplevel_troop_capacity").c_str(), info.housingspace, info2->housingspace-info.housingspace);
        ((CCLabelBMFont*)m_pBuildingAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
    }
    
    m_pUnUpgradeTip->setVisible(false);
    if(info2->townhalllevel > LogicInfo::getSingleton().m_curTownhall.level)
    {
        m_pBtnUpgrade->setEnabled(false);
        m_pNote->setString(ViewString::getSingleton().getValue("uplevel_error_hall").c_str());
        m_pUnUpgradeTip->setVisible(true);
    }
    
    if(info2->buildres == MONEY_GOLD && info2->buildcost > LogicInfo::getSingleton().m_myInfo.maxgoldex)
    {
        m_pBtnUpgrade->setEnabled(false);
        m_pNote->setString(ViewString::getSingleton().getValue("uplevel_error_gold").c_str());
        m_pUnUpgradeTip->setVisible(true);
    }
    
    if(info2->buildres == MONEY_ELIXIR && info2->buildcost > LogicInfo::getSingleton().m_myInfo.maxoilex)
    {
        m_pBtnUpgrade->setEnabled(false);
        m_pNote->setString(ViewString::getSingleton().getValue("uplevel_error_oil").c_str());
        m_pUnUpgradeTip->setVisible(true);
    }
    
    CCSpriteFrame* pFrame = NULL;
    if(info.buildres == MONEY_GOLD)
    {
        pFrame = ImgMgr::getSingleton().getFrame("map_Coins_icon");
        
        if(info2->buildcost > LogicInfo::getSingleton().m_myInfo.goldex)
        {
            ((CCLabelBMFont*)m_pBtnUpgrade->getChildByTag(1))->setColor(ccRED);
        }
        else
        {
            ((CCLabelBMFont*)m_pBtnUpgrade->getChildByTag(1))->setColor(ccWHITE);
        }
    }
    else if(info.buildres == MONEY_ELIXIR)
    {
        pFrame = ImgMgr::getSingleton().getFrame("map_Elixir_icon");

        if(info2->buildcost > LogicInfo::getSingleton().m_myInfo.oilex)
        {
            ((CCLabelBMFont*)m_pBtnUpgrade->getChildByTag(1))->setColor(ccRED);
        }
        else
        {
            ((CCLabelBMFont*)m_pBtnUpgrade->getChildByTag(1))->setColor(ccWHITE);
        }
    }
    sprintf(str, "%d", (int)info2->buildcost);
    ((CCLabelBMFont*)m_pBtnUpgrade->getChildByTag(1))->setString(str);
    ((CCSprite*)m_pBtnUpgrade->getChildByTag(2))->setDisplayFrame(pFrame);
    
    if(bid == BUILDING_TOWN_HALL)
    {
        refreshTownHall();
    }
    
    if(bid == BUILDING_BARRACKS)
    {
        refreshBarrack();
    }
}

void UpLevelUI::refreshTownHall()
{
    if(!m_pUnUpgradeTip->isVisible())
    {
        BuildingInfo info = PlayerScene::getSingleton().m_pCurBuilding->m_info;
        
        const Townhall_BuildingNums* tInfo = TownhallLevelInfo::getSingleton().getInfo(info.buildingid % 100);
        const Townhall_BuildingNums* tInfo1 = TownhallLevelInfo::getSingleton().getInfo(info.buildingid % 100 + 1);
        
        vector<int> items = TownhallLevelInfo::getSingleton().getLevelBuildings();
        
        m_pUnlockTip->setString(ViewString::getSingleton().getValue("uplevel_unlocks_buildings").c_str());
        
        char str[1024];
        
        sprintf(str, ViewString::getSingleton().getValue("uplevel_unlocks_tip").c_str(), tInfo1->level);
        m_pUpgradeDesc->setString(str);
        m_pUpgradeDesc->setVisible(true);
        
        m_pUnlockTip->setVisible(true);
        
        float scale = 0.8*getResolutionScale();
        int offset = 120*scale + 10;
        int posX = 10;
       
        for (int i = 0; i < items.size(); i++) {
            
            int num = TownhallLevelInfo::getSingleton().getBuildingNums(tInfo1, items[i]) - TownhallLevelInfo::getSingleton().getBuildingNums(tInfo, items[i]);
            
            if (num > 0) {
                CCNode* pItem = m_pTownHallItems->getChildByTag(items[i]);
                pItem->setScale(scale);
                pItem->setAnchorPoint(ccp(0, 0));
                pItem->setPosition(posX, 0);
                
                sprintf(str, "X %d", num);
                ((CCLabelBMFont*)pItem->getChildByTag(1))->setString(str);
                
                pItem->removeFromParentAndCleanup(false);
                m_pScrollView->addChild(pItem);
                
                posX += offset;
            }
        }
        
        m_pScrollView->setContentSize(CCSizeMake(posX, m_pScrollView->getContentSize().height));
        m_pScrollView->setTouchEnabled(false);
        if (posX > m_pScrollView->getViewSize().width) {
            m_pScrollView->setTouchEnabled(true);
        }
    }
}

void UpLevelUI::refreshBarrack()
{
    
    BuildingInfo& binfo = PlayerScene::getSingleton().m_pCurBuilding->m_info;
    int soldierId = PERSON_BARBARIAN + binfo.buildingid%100;
    
    if(soldierId > 110)//目前只有10个兵种 
    {
        return;
    }
    
    m_pUnlockTip->setVisible(true);
    m_pUnlockTip->setString(ViewString::getSingleton().getValue("uplevel_unlocks_warrior").c_str());
    
    

    char str[128];
    sprintf(str, "player2_%d", soldierId);
    
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
    CCSprite *unLock = CCSprite::createWithSpriteFrame(pFrame);
    
    CCSize oriSize = pFrame->getRect().size;
    float scale = m_pItemView->getContentSize().height/oriSize.height;
    unLock->setScale(scale);
    
    unLock->setPosition(ccp(m_pItemView->getContentSize().width/2, m_pItemView->getContentSize().height/2));
    m_pItemView->addChild(unLock);
    
    if(m_pUnUpgradeTip->isVisible())
    {
        m_pUnlockTip->setVisible(false);
        m_pItemView->setVisible(false);
    }

}

bool UpLevelUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    InfoUI::onAssignCCBMemberVariable(pTarget, pMemberVariableName, pNode);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UpgradeTime", CCLabelBMFont*, m_pUpgradeTime);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UnlockTip", CCLabelBMFont*, m_pUnlockTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Note", CCLabelBMFont*, m_pNote);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UnUpgradeTip", CCNode*, m_pUnUpgradeTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UpgradeDesc", CCLabelBMFont*, m_pUpgradeDesc);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnUpgrade", CCControlButton*, m_pBtnUpgrade);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ItemView", CCLayer*, m_pItemView);
    
    return true;
}

SEL_CCControlHandler UpLevelUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", UpLevelUI::menuBackCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", UpLevelUI::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuUpCallback", UpLevelUI::menuUpCallback);
    return NULL;
}