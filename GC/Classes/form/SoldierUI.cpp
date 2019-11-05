//
//  soldierUI.cpp
//  GameClient
//
//  Created by chowray on 13-5-21.
//
//

#include "SoldierUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/PlayerScene.h"
#include "../coc/Building.h"
#include "../coc/BuildingMgr.h"
#include "../coc/BaseDef.h"
#include "../coc/LogicInfo.h"
#include "../coc/PersonMgr.h"
#include "../coc/HeroMgr.h"
#include "InfoUI.h"

using namespace cocos2d;
USING_NS_COC;

SoldierUI::SoldierUI()
{
}

SoldierUI::~SoldierUI()
{
    
}

bool SoldierUI::init()
{
    bool bRet = false;
    
    do{
        
        CC_BREAK_IF(!CUINode::init());
        
        {
            
            CCNode* infoUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("info_item_3.ccbi", this);
            this->setContentSize(infoUI->getContentSize());
            this->setTargetTouch(true);
            
            this->addChild(infoUI);
            
            
            
            m_pDesc->setVisible(false);
        }
        bRet = true;
    }
    while (0);
    
    refreshInfo();
    
    return bRet;
}

void SoldierUI::refreshInfo()
{
    const SoldierInfo* info = PersonMgr::getSingleton().getSoldierInfo(LogicInfo::getSingleton().getMySoldierLevel(LogicInfo::getSingleton().m_showinfo_soldierid));
    
    char str[128];

    sprintf(str, "soldier_%d", LogicInfo::getSingleton().m_showinfo_soldierid / 100);
       
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
    m_pIcon->setDisplayFrame(pFrame);
    m_pTitle->setString(info->name.c_str());
    
    
    
    m_pBuildingAtt1->getChildByTag(ATT_TAG_PROGRESS_ADD)->setVisible(false);
    m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS_ADD)->setVisible(false);
    m_pBuildingAtt3->getChildByTag(ATT_TAG_PROGRESS_ADD)->setVisible(false);
    
    {
        int soldierId = LogicInfo::getSingleton().m_showinfo_soldierid;
        const SoldierInfo* info2 = PersonMgr::getSingleton().getSoldierMaxInfo(soldierId);
        
        pFrame = ImgMgr::getSingleton().getFrame("info_icon_damage");
        ((CCSprite*)m_pBuildingAtt1->getChildByTag(ATT_TAG_ICON))->setDisplayFrame(pFrame);
        
        sprintf(str, ViewString::getSingleton().getValue("playerupinfo_damage").c_str(), abs(info->damageex));
        ((CCLabelBMFont*)m_pBuildingAtt1->getChildByTag(ATT_TAG_VALUE))->setString(str);
        
        float percent = (float)info->damageex / info2->damageex;
        changeProgress((CCScale9Sprite*)m_pBuildingAtt1->getChildByTag(ATT_TAG_PROGRESS), percent);
        
        pFrame = ImgMgr::getSingleton().getFrame("info_icon_hp");
        ((CCSprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_ICON))->setDisplayFrame(pFrame);
        
        sprintf(str, ViewString::getSingleton().getValue("playerupinfo_hitpoint").c_str(), (int)info->hpex);
        ((CCLabelBMFont*)m_pBuildingAtt2->getChildByTag(ATT_TAG_VALUE))->setString(str);
        
        percent = (float)info->hpex / info2->hpex;
        changeProgress((CCScale9Sprite*)m_pBuildingAtt2->getChildByTag(ATT_TAG_PROGRESS), percent);
        
        pFrame = ImgMgr::getSingleton().getFrame("info_icon_elixir");
        ((CCSprite*)m_pBuildingAtt3->getChildByTag(ATT_TAG_ICON))->setDisplayFrame(pFrame);
        
        sprintf(str, ViewString::getSingleton().getValue("playerupinfo_trainingcost").c_str(), (int)info->trainingCost);
        ((CCLabelBMFont*)m_pBuildingAtt3->getChildByTag(ATT_TAG_VALUE))->setString(str);
        
        percent = (float)info->trainingCost / info2->trainingCost;
        changeProgress((CCScale9Sprite*)m_pBuildingAtt3->getChildByTag(ATT_TAG_PROGRESS), percent);
    }
    
    {
        if(info->preferedTargetBuilding == 0)
        {
            sprintf(str, "%s", ViewString::getSingleton().getValue("playeruplevel_any").c_str());
        }
        else if(info->preferedTargetBuilding == 2)
        {
            sprintf(str, "%s", ViewString::getSingleton().getValue("playeruplevel_resources").c_str());
        }
        else if(info->preferedTargetBuilding == 1)
        {
            sprintf(str, "%s", ViewString::getSingleton().getValue("playeruplevel_defenses").c_str());
        }
        else if(info->preferedTargetBuilding == 5)
        {
            sprintf(str, "%s", ViewString::getSingleton().getValue("playeruplevel_walls").c_str());
        }
        ((CCLabelBMFont*)m_pAppendAtt[0]->getChildByTag(2))->setString(str);
        
        if(info->damageRadius == 0)
        {
            sprintf(str, "%s", ViewString::getSingleton().getValue("attack_single").c_str());
        }
        else
        {
            sprintf(str, "%s", ViewString::getSingleton().getValue("attack_area").c_str());
        }
        ((CCLabelBMFont*)m_pAppendAtt[1]->getChildByTag(2))->setString(str);
        
        if(info->airTargets == 1 && info->groundTargets == 1)
        {
            sprintf(str, "%s", ViewString::getSingleton().getValue("attack_target_all").c_str());
        }
        else
        {
            sprintf(str, "%s", ViewString::getSingleton().getValue("attack_target_ground").c_str());
        }
        ((CCLabelBMFont*)m_pAppendAtt[2]->getChildByTag(2))->setString(str);
        
        sprintf(str, "%d", (int)info->housingSpaceex);
        ((CCLabelBMFont*)m_pAppendAtt[3]->getChildByTag(2))->setString(str);
        
        ((CCLabelBMFont*)m_pAppendAtt[4]->getChildByTag(2))->setString(formatTime(info->trainingTime).c_str());
        
        sprintf(str, "%d", (int)info->speedex);
        ((CCLabelBMFont*)m_pAppendAtt[5]->getChildByTag(2))->setString(str);
    }
}

// a selector callback
void SoldierUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    
    LogicInfo::getSingleton().m_showinfo_soldierid = 0;
	PlayerScene::getSingleton().m_pGameWorld->hideSoldierUI();
    
    PlayerScene::getSingleton().m_pGameWorld->hideBarrackUI();
}

void SoldierUI::menuBackCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->m_pBarrackUI->setVisible(true);
    LogicInfo::getSingleton().m_showinfo_soldierid = 0;
    
	PlayerScene::getSingleton().m_pGameWorld->hideSoldierUI();
}

bool SoldierUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Title", CCLabelBMFont*, m_pTitle);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BuildingAtt1", CCNode*, m_pBuildingAtt1);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BuildingAtt2", CCNode*, m_pBuildingAtt2);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BuildingAtt3", CCNode*, m_pBuildingAtt3);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Icon", CCSprite*, m_pIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Desc", CCLabelBMFont*, m_pDesc);
    
    char str[128];
    for (int index = 1; index <= 6; index++) {
        sprintf(str, "AppendAtt%d", index);
        CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, str, CCNode*, m_pAppendAtt[index-1]);
    }
    
    return NULL;
}

SEL_CCControlHandler SoldierUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", SoldierUI::menuBackCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", SoldierUI::menuCloseCallback);
    return NULL;
}
