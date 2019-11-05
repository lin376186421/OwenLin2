//
//  VisitUI.cpp
//  GameClient
//
//  Created by chowray on 13-5-28.
//
//

#include "VisitUI.h"
#include "../coc/BaseDef.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/PlayerScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/ExperienceLevels.h"
#include "../coc/ClanMgr.h"

USING_NS_COC;

VisitUI::VisitUI()
{
    
}

VisitUI::~VisitUI()
{

}

bool VisitUI::init()
{
    bool bRet = false;
    do
    {
		CC_BREAK_IF(!CUINode::init());
        
        {
            PlayerScene::getSingleton().addSelectBuildingListener(this);
            
            CCNode* visitUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("visit_ui.ccbi", this);
            this->addChild(visitUI);
            
            CCNode* menuNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("main_button.ccbi", this);
            visitUI->addChild(menuNode);
            
            for (int i = 0; i < menuNode->getChildrenCount(); i ++) {
                CCNode* itemNode = (CCNode*)menuNode->getChildren()->objectAtIndex(i);
                itemNode->setVisible(false);
            }

            UserInfo &info = LogicInfo::getSingleton().m_otherInfo;
            UserInfo &myInfo = LogicInfo::getSingleton().m_myInfo;
            
            m_pName->setString(info.uname.c_str());

            char str[128];
            sprintf(str, "%d", info.curlevel);
            m_pLevel->setString(str);
            
            sprintf(str, "%d", (int)info.cupex);
            m_pCup->setString(str);
            
            sprintf(str, "%d", (int)myInfo.goldex);
            m_pGoldCurNums->setString(str);
            sprintf(str, "%d", (int)myInfo.maxgoldex);
            m_pGoldMaxNums->setString(str);
            
            sprintf(str, "%d", (int)myInfo.oilex);
            m_pElixirCurNums->setString(str);
            sprintf(str, "%d", (int)myInfo.maxoilex);
            m_pElixirMaxNums->setString(str);
            
            sprintf(str, "%d", (int)LogicInfo::getSingleton().m_myInfo.gemex);
            m_pGemNums->setString(str);
            
            float percent = (float)myInfo.goldex / myInfo.maxgoldex;
            changeProgress(m_pProgressGold, percent);
            
            percent = (float)myInfo.oilex / myInfo.maxoilex;
            changeProgress(m_pProgressElixir, percent);
            
            sprintf(str, "player_mr_%d", info.military);
            CCSpriteFrame *frame = ImgMgr::getSingleton().getFrame(str);
            m_pIconMilitary->setDisplayFrame(frame);
            
            sprintf(str, "player_cn_%d", info.military);
            frame = ImgMgr::getSingleton().getFrame(str);
            m_pMiletaryName->setDisplayFrame(frame);
        
            CCSprite* pExpSprite = ImgMgr::getSingleton().loadSprite("player_level_progress");
            
            m_pExpProgressTimer = CCProgressTimer::create(pExpSprite);
            m_pExpProgressTimer->setType(kCCProgressTimerTypeRadial);
            m_pExpProgressTimer->setPosition(ccp(m_pExpProgress->getContentSize().width/2, m_pExpProgress->getContentSize().height/2));
            m_pExpProgress->addChild(m_pExpProgressTimer);
            
            percent = (float)info.exp / ExperienceLevels::getSingleton().getCurExp(info.curlevel);
            m_pExpProgressTimer->setPercentage(percent*100);
        }
                
		bRet = true;
	}while(0);
    
	return bRet;
}

bool VisitUI::onSelect(bool isSelect, Building *pBuilding)
{
    m_pBtnInfo->setVisible(false);
    m_lstBuildingBtn.clear();
            
	if(isSelect)
	{
        if(pBuilding->m_pBuildingInfo->buildingid <= 50000)
        {
            m_lstBuildingBtn.push_back(m_pBtnInfo);
        }
    }
    
    onRefurbishBuildingBtn();
    
    return true;
}

void VisitUI::onRefurbishBuildingBtn()
{
	if(m_lstBuildingBtn.empty())
		return ;
    
	int nums = m_lstBuildingBtn.size();
	int sw = CCDirector::sharedDirector()->getWinSize().width;
	int bw = m_lstBuildingBtn[0]->getContentSize().width;
	int ox;
    
	if(nums == 1)
		ox = (sw - bw) / 2 + bw/2;
	else
		ox = (sw - bw * nums - (nums - 1) * 10) / 2 + bw/2;
    
	for(int i = 0; i < nums; ++i)
	{
        m_lstBuildingBtn[i]->setOpacity(0);
		m_lstBuildingBtn[i]->setVisible(true);
        
		m_lstBuildingBtn[i]->setPosition(ccp(ox, 60));
        
        //        CCFadeOut* fo1 = CCFadeOut::create(0.0f);
        CCFadeIn* fi1 = CCFadeIn::create(0.5f * (i + 1) * 0.3f);
        CCActionInterval* action1 = CCSequence::create(fi1, NULL);
        
        CCMoveTo* mt1 = CCMoveTo::create(0.0f, ccp(ox, 60));
        CCMoveTo* mt2 = CCMoveTo::create(0.3f * (i + 1) * 0.2f, ccp(ox, 120));
        CCMoveTo* mt3 = CCMoveTo::create(0.1f, ccp(ox, 100));
        CCActionInterval* action2 = CCSequence::create(mt1, mt2, mt3, NULL);
        
        m_lstBuildingBtn[i]->runAction(action1);
        m_lstBuildingBtn[i]->runAction(action2);
        
		ox += bw + 10;
	}
}

void VisitUI::menuInfoCallback(CCObject* pSender, CCControlEvent event)
{
    if (PlayerScene::getSingleton().m_pCurBuilding) {
        PlayerScene::getSingleton().m_pGameWorld->showInfoUI();
    }
}

void VisitUI::menuRankingCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->showRankingUI();
}

void VisitUI::menuBackCallback(CCObject* pSender, CCControlEvent event)
{
    GameWorld* pGameWorld = PlayerScene::getSingleton().m_pGameWorld;
    
	pGameWorld->hideVisitUI();
    
    pGameWorld->showChgScene(CHANGE_TYPE_NORMAL);
    
    LogicInfo::getSingleton().goHome();
}

bool VisitUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerName", CCLabelTTF*, m_pName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerLevel", CCLabelBMFont*, m_pLevel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "PlayerCup", CCLabelBMFont*, m_pCup);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ProgressGold", CCScale9Sprite*, m_pProgressGold);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ProgressElixir", CCScale9Sprite*, m_pProgressElixir);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GoldCurNums", CCLabelBMFont*, m_pGoldCurNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GoldMaxNums", CCLabelBMFont*, m_pGoldMaxNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ElixirCurNums", CCLabelBMFont*, m_pElixirCurNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ElixirMaxNums", CCLabelBMFont*, m_pElixirMaxNums);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "GemNums", CCLabelBMFont*, m_pGemNums);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ExpProgress", CCSprite*, m_pExpProgress);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnInfo", CCControlButton*, m_pBtnInfo);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "IconMilitary", CCSprite*, m_pIconMilitary);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "militaryName", CCSprite*, m_pMiletaryName);
    return true;
}

SEL_CCControlHandler VisitUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuInfoCallback", VisitUI::menuInfoCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuRankingCallback", VisitUI::menuRankingCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", VisitUI::menuBackCallback);
    
    return NULL;
}

