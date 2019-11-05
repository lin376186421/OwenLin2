//
//  MilitaryRankUI.cpp
//  GameClient
//
//  Created by Lee on 14-5-5.
//
//

#include "MilitaryRankUI.h"
#include "cocos2d.h"
#include "../coc/PlayerScene.h"
#include "GameWorldScene.h"
#include "../coc/MilitaryRankMgr.h"
#include "../coc/PersonMgr.h"
#include "../coc/SoundMgr.h"
#include "../coc/BuildingMgr.h"


using namespace cocos2d;
USING_NS_COC;

MilitaryRankUI::MilitaryRankUI()
{
}

MilitaryRankUI::~MilitaryRankUI()
{
}

bool MilitaryRankUI::init()
{
    bool bRet = false;
    do{
        CC_BREAK_IF(!CUINode::init());
        {
            m_pMilitaryRankUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("military_rank_0.ccbi", this);
            this->setTargetTouch(true);
            this->setContentSize(m_pMilitaryRankUI->getContentSize());
            this->addChild(m_pMilitaryRankUI);
            LogicInfo::getSingleton().countMilitaryRank();
            refreshInfo();
            bRet = true;
        }
    }
    while (0);
    return bRet;
}

void MilitaryRankUI::refreshInfo()
{
    int myRank = LogicInfo::getSingleton().m_myInfo.military;
    for (int i = 1; i<=11; i++)
    {
        CCNode *pRoot = m_pMilitaryRankUI->getChildByTag(i);
        CCNode *pEffect = pRoot->getChildByTag(1);
        CCNode *pGrayFrame = pRoot->getChildByTag(2);
        CCNode *pLightFrame = pRoot->getChildByTag(3);
        CCNode *pGrayLayer= pRoot->getChildByTag(4);
        CCNode *pLightLayer= pRoot->getChildByTag(5);
        if (i ==myRank)
        {
            pEffect->setVisible(true);
//            if (i == rank)
//                pEffect->setVisible(true);
//            else
//                pEffect->setVisible(false);
            pGrayFrame->setVisible(false);
            pLightFrame->setVisible(true);
            pGrayLayer->setVisible(false);
            pLightLayer->setVisible(true);
        }
        else
        {
            pEffect->setVisible(false);
            pGrayFrame->setVisible(true);
            pLightFrame->setVisible(false);
            pGrayLayer->setVisible(true);
            pLightLayer->setVisible(false);
            if(i < myRank)
            {
                pLightLayer->setVisible(true);
            }
        }
    }
    
    
    
    string militaryName = MilitaryRankMgr::getSingleton().getMilitaryName(myRank);
    m_pMilitaryRankLabel->setString(militaryName.c_str());
    
    MilitaryInfo curInfo = MilitaryRankMgr::getSingleton().getMilitaryInfo(myRank);
    MilitaryInfo nextInfo;
    if(LogicInfo::getSingleton().m_myInfo.military == MilitaryRankMgr::getSingleton().m_militaryInfoList.size())//达到最高军衔
        nextInfo = curInfo;
    else
        nextInfo = MilitaryRankMgr::getSingleton().getMilitaryInfo(myRank+1);
    
    vector<CCPoint> posList;
    char str[124];
    for(int i = 21; i <= 24; i++)
    {
        CCNode *pRoot = m_pMilitaryRankUI->getChildByTag(i);
        posList.push_back(pRoot->getPosition());
        CCLabelBMFont *pInfo = (CCLabelBMFont *)pRoot->getChildByTag(3);
        if(i == 22)
        {
            
            CCLabelBMFont *pObjNameLabel = (CCLabelBMFont *)pRoot->getChildByTag(4);
            
            CCSprite *pSprite = (CCSprite *)pRoot->getChildByTag(2);
            int oid = nextInfo.oId[0];
            int otarget = nextInfo.oTarget[0];
            
            
            int curNum;
            if(oid > 10000)//配置表中 兵种的id只取前3位 建筑的id取五位
            {
                if(oid/100 == BUILDING_WALL)
                    curNum = LogicInfo::getSingleton().getBuildNumsByBid(oid);
                else
                    curNum = LogicInfo::getSingleton().getBuildMaxLevel(oid/100);
                
                sprintf(str, "building2_%d", (int)(oid/100));
                CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
                pSprite->setDisplayFrame(pFrame);
                
                const BuildingInfo *bInfo = BuildingMgr::getSingleton().getBuildingInfo(oid);
                pObjNameLabel->setString(bInfo->name.c_str());
            }
            else
            {
                curNum = LogicInfo::getSingleton().getSoldierlevel(oid%100-1);
                
                sprintf(str, "player2_%d", (int)(oid));
                CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
                pSprite->setDisplayFrame(pFrame);
                
                const SoldierInfo *sInfo = PersonMgr::getSingleton().getSoldierInfo(oid*100+1);
                pObjNameLabel->setString(sInfo->name.c_str());
                
            }

            if(oid/100 == BUILDING_WALL)
                sprintf(str, "Level%d+ (%d/%d)",oid%100,curNum,otarget);
            else
                sprintf(str, "Level %d+",otarget);
            
            pInfo->setString(str);
            
            if(curNum < otarget)
                pInfo->setColor(ccRED);
            else
                pInfo->setColor(ccGREEN);
        }
        else
        {
            int curNum,nextNum;
            if(i == 21)
            {
                int curspace, maxspace;
                LogicInfo::getSingleton().countTroopSpace(LogicInfo::getSingleton().m_mapTroophousing, curspace, maxspace);
                curNum = maxspace;//当前兵营最大数量
                nextNum = nextInfo.troops;
                if(nextNum == 0)
                    pRoot->setVisible(false);
                sprintf(str, "%d/%d",curNum,nextNum);
                if(curNum < nextNum)
                    pInfo->setColor(ccRED);
                else
                    pInfo->setColor(ccGREEN);
                pInfo->setString(str);
                
            }
            else if(i == 23)
            {
                curNum = LogicInfo::getSingleton().m_myInfo.cupex;
                nextNum = nextInfo.cup;
                if(nextNum == 0)
                    pRoot->setVisible(false);
                sprintf(str, "%d/%d",curNum,nextNum);
                if(curNum < nextNum)
                    pInfo->setColor(ccRED);
                else
                    pInfo->setColor(ccGREEN);
                pInfo->setString(str);
            }
            else if (i == 24)
            {
                curNum = LogicInfo::getSingleton().m_myInfo.medal;
                nextNum = nextInfo.medal;
                if(nextNum == 0)
                    pRoot->setVisible(false);
                sprintf(str, "%d/%d",curNum,nextNum);
                if(curNum < nextNum)
                    pInfo->setColor(ccRED);
                else
                    pInfo->setColor(ccGREEN);
                pInfo->setString(str);
            }
        }
    }
    
    int order = 0;
    for(int i = 21; i <= 24; i++)
    {
        CCNode *pRoot = m_pMilitaryRankUI->getChildByTag(i);
        if(pRoot->isVisible())
        {
            pRoot->setPosition(posList[order]);
            order++;
        }
    }
    
    posList.clear();
}

#pragma mark - ccb

void MilitaryRankUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideMilitaryRankUI();
    if(CCUserDefault::sharedUserDefault()->getBoolForKey("LEAGUE_TIP") == false)
    {
        PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->showBtnTip();
    }
}

bool MilitaryRankUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GlobalLabel", CCLabelBMFont*, m_pGlobalLabel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "MilitaryRankLabel", CCLabelBMFont*, m_pMilitaryRankLabel);
    return true;
}

SEL_CCControlHandler MilitaryRankUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", MilitaryRankUI::menuCloseCallback);
    return NULL;
}




MilitaryUpgradeUI::MilitaryUpgradeUI()
{
}

MilitaryUpgradeUI::~MilitaryUpgradeUI()
{
}

bool MilitaryUpgradeUI::init()
{
    bool bRet = false;
    do{
        CC_BREAK_IF(!CUINode::init());
        {
            CCNode *pMilitaryUpgradeUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("military_rank_1.ccbi", this);
            this->setTargetTouch(true);
            this->setContentSize(pMilitaryUpgradeUI->getContentSize());
            this->addChild(pMilitaryUpgradeUI);
            refreshInfo();
            SoundMgr::getSingleton().playEffect("military_bg.mp3");
            scheduleOnce(schedule_selector(MilitaryUpgradeUI::playEffect), 1.8);
            bRet = true;
        }
    }
    while (0);
    return bRet;
}
void MilitaryUpgradeUI::playEffect(float ot)
{
    SoundMgr::getSingleton().playEffect("military_effect.mp3");
}
void MilitaryUpgradeUI::refreshInfo()
{
    char str[124];
    
    sprintf(str, "player_mr_%d",LogicInfo::getSingleton().m_myInfo.military);
    CCSpriteFrame *frame;
    frame = ImgMgr::getSingleton().getFrame(str);
    m_pNowIcon->setDisplayFrame(frame);
    
    sprintf(str, "player_mr_%d",LogicInfo::getSingleton().m_myInfo.military - 1);
    frame = ImgMgr::getSingleton().getFrame(str);
    m_pLastIcon->setDisplayFrame(frame);
    
//    string lastName = MilitaryRankMgr::getSingleton().getMilitaryName(LogicInfo::getSingleton().m_myInfo.military-1);
    string nowName = MilitaryRankMgr::getSingleton().getMilitaryName(LogicInfo::getSingleton().m_myInfo.military);
    sprintf(str, ViewString::getValue("military_upgrade_info").c_str(),nowName.c_str());
    m_pInfoLabel->setString(str);
    
    if(FacebookMgr::getSingleton().isShare() == false)
        m_pBtnFBShare->setVisible(false);
}

#pragma mark - ccb
void MilitaryUpgradeUI::menuBtnFBShareCallback(CCObject* pSender, CCControlEvent event)
{
    FacebookMgr::getSingleton().m_pCurBtn = (CCControlButton *)pSender;
    FacebookMgr::getSingleton().m_shareType = FBShare_Military;
    PlayerScene::getSingleton().m_pGameWorld->showFBShareUI();
}
void MilitaryUpgradeUI::menuOkCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideMilitaryUpgradeUI();
}

bool MilitaryUpgradeUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "infoLabel", CCLabelBMFont*, m_pInfoLabel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "nowIcon", CCSprite*, m_pNowIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "lastIcon", CCSprite*, m_pLastIcon);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnFBShare", CCControlButton*, m_pBtnFBShare);
    

    return true;
}

SEL_CCControlHandler MilitaryUpgradeUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuOkCallback", MilitaryUpgradeUI::menuOkCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBtnFBShareCallback", MilitaryUpgradeUI::menuBtnFBShareCallback);

    return NULL;
}

