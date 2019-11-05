//
//  InfoUI.cpp
//  coc4
//
//  Created by chowray on 13-4-7.
//
//

#include "InfoUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/PlayerScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/Building.h"
#include "../coc/BuildingMgr.h"
#include "../coc/BuildingInfoMgr.h"
#include "../coc/HeroMgr.h"
#include "../coc/PersonMgr.h"

#include "../coc/ClanMgr.h"

using namespace cocos2d;
USING_NS_COC;

InfoUI::InfoUI()
{
    
}

InfoUI::~InfoUI()
{
    unscheduleUpdate();
}

bool InfoUI::init()
{
    bool bRet = false;
    
    do{
        
        CC_BREAK_IF(!CUINode::init());
        {
            if(PlayerScene::getSingleton().m_pCurBuilding == NULL)
                return false;
            
            Building* pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
            
            BuildingInfo info = PlayerScene::getSingleton().m_pCurBuilding->m_info;
            CCNode* infoUI = NULL;
            if(pBuilding->isAttackBuilding()){
                infoUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("info_item_1.ccbi", this);
            }
            else if(info.buildingid / 100 == BUILDING_ARMY_CAMP || info.buildingid / 100 == BUILDING_CLAN_CASTLE)
            {
                infoUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("info_item_2.ccbi", this);
                
                m_pScrollView = CCScrollView::create(m_pTroopsView->getContentSize());
                m_pScrollView->setDirection(kCCScrollViewDirectionHorizontal);
                m_pTroopsView->addChild(m_pScrollView);
            }
            else{
                infoUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("info_item_0.ccbi", this);
            }
             
            this->setContentSize(infoUI->getContentSize());
            this->setTargetTouch(true);
            this->addChild(infoUI);
            
        }
        
        bRet = true;
    }
    while (0);
    
    m_bUpgrade = false;
    refreshInfo();
    
    scheduleUpdate();
    
    return bRet;
}

void InfoUI::update(float dt)
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
void InfoUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
	(PlayerScene::getSingleton().m_pGameWorld)->hideInfoUI();
}

// a selector callback
void InfoUI::menuBackCallback(CCObject* pSender, CCControlEvent event)
{
	(PlayerScene::getSingleton().m_pGameWorld)->hideInfoUI();
}

void InfoUI::refreshInfo()
{
    if(PlayerScene::getSingleton().m_pCurBuilding == NULL)
        return;
    
    Building* pBuilding = PlayerScene::getSingleton().m_pCurBuilding;
    
    const BuildingInfo* info = &pBuilding->m_info;
    UserBuildingInfo* buildingInfo = pBuilding->m_pBuildingInfo;
    
    if (buildingInfo->state == BUILDING_STATE_UPGRADING) {
        info = BuildingMgr::getSingleton().getBuildingInfo(info->buildingid - 1);
    }
    
    int buildid = info->buildingid / 100;
    
    string res = info->res;
    if (m_bUpgrade) {
        const BuildingInfo* info2 = BuildingMgr::getSingleton().getBuildingInfo(info->buildingid + 1);
        res = info2->res;
    }
    
    const BuildingInfo* pMaxBuilding = BuildingMgr::getSingleton().getBuildingMaxInfo(buildid);
    
    char str[1024];
    
    sprintf(str, ViewString::getSingleton().getValue("info_title").c_str(), info->name.c_str(), info->buildingid % 100);
    m_pTitle->setString(str);
    
    m_pIcon->setTextureRect(CCRectZero);
    
    if(buildid == BUILDING_WALL)
    {
        sprintf(str,"%s_1_2", res.c_str());
        CCSprite* pSprite = ImgMgr::getSingleton().loadSprite(str);
        if (pSprite) {
            m_pIcon->addChild(pSprite);
        }
    }
    else
    {
        sprintf(str,"%s_1", res.c_str());
        CCSprite* pSprite = ImgMgr::getSingleton().loadSprite(str);
        if (pSprite) {
            m_pIcon->addChild(pSprite);
        }
        
        sprintf(str,"%s_2", res.c_str());
        pSprite = ImgMgr::getSingleton().loadSprite(str);
        if (pSprite) {
            m_pIcon->addChild(pSprite);
        }
        
        if(buildid == BUILDING_CANNON || buildid == BUILDING_WIZARD_TOWER)
        {
            sprintf(str,"%s_2_5", res.c_str());
            pSprite = ImgMgr::getSingleton().loadSprite(str);
            if (pSprite) {
                m_pIcon->addChild(pSprite);
            }
        }
        else if (buildid == BUILDING_ELIXIR_STORAGE || buildid == BUILDING_GOLD_STORAGE){
            sprintf(str,"%s_2_0", res.c_str());
            
            pSprite = ImgMgr::getSingleton().loadSprite(str);
            if (pSprite) {
                m_pIcon->addChild(pSprite);
            }
        }
    }
    
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("info_icon_hp");
    ((CCSprite*)m_pBuildingAtt1->getChildByTag(ATT_TAG_ICON))->setDisplayFrame(pFrame);
    
    int hp = pBuilding->getHP();
    if (hp > info->hp) {
        hp = info->hp;
    }
    
    if (!m_bUpgrade) {
        
        changeProgress((CCScale9Sprite*)m_pBuildingAtt1->getChildByTag(ATT_TAG_PROGRESS), (float)hp / (float)info->hp);
    }else{
        changeProgress((CCScale9Sprite*)m_pBuildingAtt1->getChildByTag(ATT_TAG_PROGRESS), (float)info->hp/(float)pMaxBuilding->hp);
    }

    sprintf(str, ViewString::getSingleton().getValue("info_hitpoint").c_str(), hp, info->hp);
    ((CCLabelBMFont*)m_pBuildingAtt1->getChildByTag(ATT_TAG_VALUE))->setString(str);
    
    m_pBuildingAtt3->setVisible(false);
    
    if(buildid == BUILDING_TOWN_HALL)
    {
        m_pBuildingAtt3->setVisible(true);
        
        pFrame = ImgMgr::getSingleton().getFrame("info_icon_coins");
        ((CCSprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_ICON))->setDisplayFrame(pFrame);
        float percent = (float)buildingInfo->store[0] / info->maxstored[0];
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS), percent);
        sprintf(str, ViewString::getSingleton().getValue("info_storage_capacity").c_str(), (int)buildingInfo->store[0], (int)info->maxstored[0]);
        ((CCLabelBMFont*)m_pBuildingAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
        
        pFrame = ImgMgr::getSingleton().getFrame("info_icon_elixirs");
        ((CCSprite*)m_pBuildingAtt3->getChildByTag(ATT_TAG_ICON))->setDisplayFrame(pFrame);
        percent = (float)buildingInfo->store[1] / info->maxstored[1];
        changeProgress((CCScale9Sprite*)m_pBuildingAtt3->getChildByTag(ATT_TAG_PROGRESS), percent);
        sprintf(str, ViewString::getSingleton().getValue("info_storage_capacity").c_str(), (int)buildingInfo->store[1], (int)info->maxstored[1]);
        ((CCLabelBMFont*)m_pBuildingAtt3->getChildByTag(ATT_TAG_VALUE))->setString(str);
    }
    else if(buildid == BUILDING_GOLD_MINE || buildid == BUILDING_ELIXIR_COLLECTOR)
    {
        if (buildid == BUILDING_GOLD_MINE) {
            pFrame = ImgMgr::getSingleton().getFrame("info_icon_coin");
        }
        else if(buildid == BUILDING_ELIXIR_COLLECTOR)
        {
            pFrame = ImgMgr::getSingleton().getFrame("info_icon_elixir");
        }
        ((CCSprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_ICON))->setDisplayFrame(pFrame);
        
        float percent = (float)info->producesperhour / pMaxBuilding->producesperhour;
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS), percent);
       
        sprintf(str, ViewString::getSingleton().getValue("info_production").c_str(), (int)info->producesperhour);
        ((CCLabelBMFont*)m_pBuildingAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
        
        int res = pBuilding->countRes();
        if(info->producesres == MONEY_GOLD)
        {
            pFrame = ImgMgr::getSingleton().getFrame("info_icon_coins");
            
        }
        else if(info->producesres == MONEY_ELIXIR)
        {
            pFrame = ImgMgr::getSingleton().getFrame("info_icon_elixirs");
        }
        
        m_pBuildingAtt3->setVisible(true);
        if (m_bUpgrade) {
            m_pBuildingAtt3->getChildByTag(ATT_TAG_PROGRESS_ADD)->setVisible(false);
        }
        ((CCSprite*)m_pBuildingAtt3->getChildByTag(ATT_TAG_ICON))->setDisplayFrame(pFrame);
        
        percent = (float)res / pBuilding->m_info.maxstored[pBuilding->m_info.producesres - 1];
        changeProgress((CCScale9Sprite*)m_pBuildingAtt3->getChildByTag(ATT_TAG_PROGRESS), percent);
        
        sprintf(str, "%d/%d", res, (int)pBuilding->m_info.maxstored[pBuilding->m_info.producesres - 1]);
        ((CCLabelBMFont*)m_pBuildingAtt3->getChildByTag(ATT_TAG_VALUE))->setString(str);
    }
    else if(buildid == BUILDING_ELIXIR_STORAGE)
    {
        pFrame = ImgMgr::getSingleton().getFrame("info_icon_elixir");
        ((CCSprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_ICON))->setDisplayFrame(pFrame);
        
        float percent = (float)buildingInfo->store[1] / info->maxstored[1];
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS), percent);
        
        sprintf(str, ViewString::getSingleton().getValue("info_storage_capacity").c_str(), (int)buildingInfo->store[1], (int)info->maxstored[1]);
        ((CCLabelBMFont*)m_pBuildingAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
    }
    else if(buildid == BUILDING_GOLD_STORAGE)
    {
        pFrame = ImgMgr::getSingleton().getFrame("info_icon_coin");
        ((CCSprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_ICON))->setDisplayFrame(pFrame);
        
        float percent = (float)buildingInfo->store[0] / info->maxstored[0];
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS), percent);

        sprintf(str, ViewString::getSingleton().getValue("info_storage_capacity").c_str(), (int)buildingInfo->store[0], (int)info->maxstored[0]);
        ((CCLabelBMFont*)m_pBuildingAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
    }
    else if(buildid == BUILDING_CANNON || buildid == BUILDING_ARCHER_TOWER || buildid == BUILDING_BIG_MORTAR
            || buildid == BUILDING_AIR_DEFENSE || buildid == BUILDING_WIZARD_TOWER)
    {
        
        pFrame = ImgMgr::getSingleton().getFrame("info_icon_damage");
        ((CCSprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_ICON))->setDisplayFrame(pFrame);
        
        float percent = (float)info->damage / pMaxBuilding->damage;
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS), percent);
        
        sprintf(str, ViewString::getSingleton().getValue("info_damage").c_str(), info->damage, pMaxBuilding->damage);
        ((CCLabelBMFont*)m_pBuildingAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
    }
    else if(buildid == BUILDING_BARRACKS)
    {
        m_pBuildingAtt2->setVisible(false);
    }
    else if(buildid == BUILDING_ARMY_CAMP)
    {
        list<FightSoldierInfoEx> lstSoldier;
        MapTroophousingInfo troophousingInfo;
        if(LogicInfo::getSingleton().isNormalMode())
        {
            LogicInfo::getSingleton().refurbishFightSoldierInfo();
            lstSoldier = LogicInfo::getSingleton().m_lstFightSoldier;
            troophousingInfo = LogicInfo::getSingleton().m_mapTroophousing;
        }
        else
        {
            LogicInfo::getSingleton().refurbishOtherFightSoldierInfo();
            lstSoldier = LogicInfo::getSingleton().m_lstOtherFightSoldier;
            troophousingInfo = LogicInfo::getSingleton().m_mapOtherTroophousing;
        }
        
        int curspace = 0;
        int maxspace = 0;
        
        LogicInfo::getSingleton().countTroopSpace(troophousingInfo, curspace, maxspace);
        
        pFrame = ImgMgr::getSingleton().getFrame("info_icon_person");
        ((CCSprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_ICON))->setDisplayFrame(pFrame);
        
        if (!m_bUpgrade)
        {
            float percent = (float)curspace / (float)maxspace;
            changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS), percent);
            
            sprintf(str, ViewString::getSingleton().getValue("troop_capacity").c_str(), curspace, maxspace);
            ((CCLabelBMFont*)m_pBuildingAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
            
            CCSize oriSize = ImgMgr::getSingleton().getFrame("player1_101")->getRect().size;
            float scale = m_pTroopsView->getContentSize().height/oriSize.height;
            int offset = oriSize.width*scale + 10;
            float posX = 10;
            
            int i = 0;
            for(std::list<FightSoldierInfoEx>::iterator it = lstSoldier.begin(); it != lstSoldier.end(); ++it)
            {
                if (it->pid < 30000) {
                    sprintf(str, "player1_%d", (int)(it->pid / 100));
                    CCSprite* pSoldier = ImgMgr::getSingleton().loadSprite(str);
                    pSoldier->setAnchorPoint(ccp(0, 0));
                    pSoldier->setScale(scale);
                    pSoldier->setPositionX(posX);
                    
                    sprintf(str, "x%d", (int)it->numsex);
                    CCLabelBMFont* pNums = CCLabelBMFont::create(str, "Font/tbFont_b.fnt");
                    pNums->setAnchorPoint(ccp(0, 0.5));
                    pNums->setPosition(ccp(11, 25));
                    pNums->setScale(0.5);
                    
                    pSoldier->addChild(pNums);
                    
                    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("player_star");
                    
                    int star = it->pid % 100;
                    for(int j = 0; j < star; ++j)
                    {
                        CCSprite* pStar = CCSprite::createWithSpriteFrame(pFrame);
                        pStar->setPosition(ccp(18 + 17 * j, 50));
                        pSoldier->addChild(pStar, 10);
                    }
                    
                    m_pScrollView->addChild(pSoldier);
                    
                    posX += offset;
                    i++;
                }
            }
            
            m_pScrollView->setContentSize(CCSizeMake(posX, m_pScrollView->getContentSize().height));
            m_pScrollView->setTouchEnabled(false);
            if (posX > m_pScrollView->getViewSize().width) {
                m_pScrollView->setTouchEnabled(true);
            }
        }
    }
    else if (buildid == BUILDING_LABORATORY)
    {
        m_pBuildingAtt2->setVisible(false);
    }
    else if (buildid == BUILDING_WALL || buildid == BUILDING_BUILDER_HOME || buildid == BUILDING_HERO_ALTAR)
    {
        m_pBuildingAtt2->setVisible(false);
    }
    else if (buildid == BUILDING_CLAN_CASTLE)
    {
        if(info->buildingid % 100 == 1)
        {
            sprintf(str, "%s", info->name.c_str());
            m_pTitle->setString(str);
            
            if (!m_bUpgrade) {
                
                m_pBuildingAtt2->setVisible(false);
                m_pTroopsTitle->setVisible(false);
            }
        }
        else
        {
            sprintf(str, ViewString::getSingleton().getValue("info_title").c_str(), info->name.c_str(), info->buildingid % 100 - 1);
            m_pTitle->setString(str);
            
            CCSpriteFrame *pFrame = ImgMgr::getSingleton().getFrame("info_icon_person");
            ((CCSprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_ICON))->setDisplayFrame(pFrame);
            
            if (!m_bUpgrade) {
                
                int maxspace = 0;
                int curspace = 0;
                
                vector<int> lstSoldier;
                
                if(LogicInfo::getSingleton().isNormalMode())
                {
                    maxspace = info->housingspace;
                    curspace = ClanMgr::getSingleton().m_myCurSpace;
                    
                    lstSoldier = ClanMgr::getSingleton().m_myListSoldier;
                }
                else
                {
                    maxspace = info->housingspace;
                    curspace = ClanMgr::getSingleton().m_otherCurSpace;
                    
                    lstSoldier = ClanMgr::getSingleton().m_otherListSoldier;
                }
                
                float percent = (float)curspace / (float)maxspace;
                changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS), percent);
                
                sprintf(str, ViewString::getSingleton().getValue("troop_capacity").c_str(), curspace, maxspace);
                ((CCLabelBMFont*)m_pBuildingAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
                
                if (LogicInfo::getSingleton().isNormalMode() || ClanMgr::getSingleton().m_otherclan.id == ClanMgr::getSingleton().m_myclan.id) {
                    
                    map<int, int> mapTroopsData;
                    for (int index = 0; index < lstSoldier.size(); index++) {
                        
                        int sid = lstSoldier[index];
                        
                        map<int, int>::iterator it = mapTroopsData.find(sid);
                        
                        if (it == mapTroopsData.end()) {
                            mapTroopsData.insert(pair<int, int>(sid, 1));
                        }
                        else {
                            it->second ++;
                        }
                    }
                    
                    CCSize oriSize = ImgMgr::getSingleton().getFrame("player1_101")->getRect().size;
                    float scale = m_pTroopsView->getContentSize().height/oriSize.height;
                    int offset = oriSize.width*scale + 10;
                    float posX = 10;
                    
                    for (map<int, int>::iterator it = mapTroopsData.begin(); it != mapTroopsData.end(); it++) {
                        
                        int soldierid = it->first;
                        int nums = it->second;
                        
                        sprintf(str, "player1_%d", (int)(soldierid / 100));
                        CCSprite* pSoldier = ImgMgr::getSingleton().loadSprite(str);
                        pSoldier->setAnchorPoint(ccp(0, 0));
                        pSoldier->setScale(scale);
                        pSoldier->setPositionX(posX);
                        
                        sprintf(str, "x%d", nums);
                        CCLabelBMFont* pNums = CCLabelBMFont::create(str, "Font/tbFont_b.fnt");
                        pNums->setAnchorPoint(ccp(0, 0.5));
                        pNums->setPosition(ccp(11, 25));
                        pNums->setScale(0.5);
                        
                        pSoldier->addChild(pNums);
                        
                        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("player_star");
                        
                        int star = soldierid % 100;
                        for(int j = 0; j < star; ++j)
                        {
                            CCSprite* pStar = CCSprite::createWithSpriteFrame(pFrame);
                            pStar->setPosition(ccp(18 + 17 * j, 50));
                            pSoldier->addChild(pStar, 10);
                        }
                        
                        m_pScrollView->addChild(pSoldier);
                        
                        posX += offset;
                    }
                    
                    m_pScrollView->setContentSize(CCSizeMake(posX, m_pScrollView->getContentSize().height));
                    m_pScrollView->setTouchEnabled(false);
                    if (posX > m_pScrollView->getViewSize().width) {
                        m_pScrollView->setTouchEnabled(true);
                    }
                }
            }
        }
    }

    if (!m_bUpgrade) {
        CCDictionary* pInfoList = CCDictionary::createWithContentsOfFile("buildingInfo.plist");
        CCDictElement* pElement = NULL;
        
        sprintf(str, "%d", int(info->buildingid / 100) * 100 + 1);
        CCDictionary* pInfoDict = (CCDictionary*)pInfoList->objectForKey(str);
        if (pInfoDict) {
            
            int index = 0;
            CCDICT_FOREACH(pInfoDict, pElement)
            {
                string name = pElement->getStrKey();
                string value = ((CCString*)pElement->getObject())->m_sString;
                
                if(name != "Desc")
                {
                    ((CCLabelBMFont*)m_pAppendAtt[index]->getChildByTag(1))->setString(name.c_str());
                    ((CCLabelBMFont*)m_pAppendAtt[index]->getChildByTag(2))->setString(value.c_str());
                    index++;
                }
            }
            
            CCString* pDesc = (CCString*)pInfoDict->objectForKey("Desc");
            if (m_pDesc != NULL && pDesc) {
                sprintf(str, "%s", pDesc->getCString());
                m_pDesc->setString(str);
            }
        }
    }
}

bool InfoUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Title", CCLabelBMFont*, m_pTitle);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Name", CCLabelBMFont*, m_pName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BuildingAtt1", CCNode*, m_pBuildingAtt1);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BuildingAtt2", CCNode*, m_pBuildingAtt2);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BuildingAtt3", CCNode*, m_pBuildingAtt3);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Icon", CCSprite*, m_pIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Desc", CCLabelBMFont*, m_pDesc);
    
    char str[128];
    for (int index = 1; index <= 4; index++) {
        sprintf(str, "AppendAtt%d", index);
        CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, str, CCNode*, m_pAppendAtt[index-1]);
    }

    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TroopsTitle", CCLabelBMFont*, m_pTroopsTitle);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TroopsView", CCLayer*, m_pTroopsView);
    
    return NULL;
}

SEL_CCControlHandler InfoUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", InfoUI::menuBackCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", InfoUI::menuCloseCallback);
    return NULL;
}