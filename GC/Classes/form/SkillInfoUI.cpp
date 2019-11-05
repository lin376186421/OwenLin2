//
//  SkillInfoUI.cpp
//  GameClient
//
//  Created by qsun on 13-11-5.
//
//

#include "SkillInfoUI.h"
#include "../coc/BaseDef.h"
#include "../coc/SkillMgr.h"
#include "../coc/HeroMgr.h"
#include "../coc/PlayerScene.h"
#include "../GameWorldScene.h"
#include "../coc/GuideMgr.h"

USING_NS_COC;

SkillInfoUI::SkillInfoUI(int id, bool isTrain)
{
    m_skillId = id;
    m_isTrain = isTrain;
    
    m_trainGems = SkillMgr::getSingleton().getSkillTrainGem(id);
    m_leapGems = SkillMgr::getSingleton().getSkillTotalTrainGem(id);
    
    m_pTrainNode = NULL;
    m_pTrainAnimation = NULL;
    
    m_pBtnBack = NULL;
}

SkillInfoUI::~SkillInfoUI()
{
    if (m_pTrainAnimation) {
        m_pTrainAnimation->setDelegate(NULL);
    }
}

SkillInfoUI* SkillInfoUI::create(int id, bool isTrain)
{
    SkillInfoUI *pRet = new SkillInfoUI(id, isTrain);
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

bool SkillInfoUI::init()
{
    bool bRet = false;
    
    do{
        
        CC_BREAK_IF(!CUINode::init());
        {
            
            CCNode* pInfoUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("skill_info.ccbi", this);
            this->setTargetTouch(true);
            this->setContentSize(pInfoUI->getContentSize());
            this->addChild(pInfoUI);
            
            
            
            refresh();
        }
        
        bRet = true;
    }
    while (0);
    
    return bRet;
}

void SkillInfoUI::refresh()
{
    int index = (m_skillId/100 % 10000)-1;
    int skillId = m_skillId + HeroMgr::getSingleton().m_myHero.bskill[index];
    
    char str[128];
    
    m_trainGems = SkillMgr::getSingleton().getSkillTrainGem(skillId);
    m_leapGems = SkillMgr::getSingleton().getSkillTotalTrainGem(skillId);
    
    const SkillInfo* pInfo = SkillMgr::getSingleton().getNode(skillId);
    
    m_pName->setString(pInfo->name.c_str());
    m_pDesc->setString(pInfo->info.c_str());
    
    sprintf(str, "ui_skill_%d", skillId/100);
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
    if(pFrame != NULL)
        m_pIcon->setDisplayFrame(pFrame);
    
    m_pCoolDown->setString(formatTimeMs(pInfo->cd).c_str());
    
    m_pDuration->setString(formatTimeMs(pInfo->btime1).c_str());
    
    sprintf(str, "%d", HeroMgr::getSingleton().m_myHero.bskill[index]);
    m_pLevel->setString(str);
    
    sprintf(str, "%d", pInfo->maxlevel);
    m_pMaxLevel->setString(str);
    
    changeProgress(m_pExpProgress, (float)HeroMgr::getSingleton().m_myHero.bskillexp[index] / (float)pInfo->maxExp);
    
    sprintf(str, "%d", HeroMgr::getSingleton().m_myHero.bskillexp[index]);
    m_pExp->setString(str);
    
    if (m_isTrain) {
        
        if (HeroMgr::getSingleton().m_myHero.bskill[index] < pInfo->maxlevel) {
            
            if (HeroMgr::getSingleton().getHeroLevel() < pInfo->needlevel) {
                m_pUnUpgradeNode->setVisible(true);
                m_pUpgradeNode->setVisible(false);
                
                sprintf(str, ViewString::getValue("skill_unupgrade_tip").c_str(), pInfo->needlevel);
                m_pUnUpgradeTip->setString(str);
            }else{
                
                const SkillInfo* pNextInfo = SkillMgr::getSingleton().getNode(skillId+1);
                
                m_pUnUpgradeNode->setVisible(false);
                m_pUpgradeNode->setVisible(true);
                
                m_pUpgradeDesc->setString(pNextInfo->info.c_str());
                
                sprintf(str, "%d", m_trainGems);
                ((CCLabelBMFont*)m_pBtnTrain->getChildByTag(1))->setString(str);
                sprintf(str, "%d", m_leapGems);
                ((CCLabelBMFont*)m_pBtnLeap->getChildByTag(1))->setString(str);
                if(HeroMgr::getSingleton().m_isFreeTrain)
                    ((CCLabelBMFont*)m_pBtnTrain->getChildByTag(1))->setString("Free");
            }
        }else{
            m_pUpgradeNode->setVisible(false);
        }
        
        m_pInfoNode->setVisible(false);
    
    }else{
        
        const SkillInfo* pBaseInfo = SkillMgr::getSingleton().getNode(m_skillId + 1);

        m_pInfoNode->getChildByTag(1)->setVisible(false);
        if (HeroMgr::getSingleton().getHeroLevel() < pBaseInfo->needlevel) {
            m_pInfoNode->getChildByTag(1)->setVisible(true);
            sprintf(str, ViewString::getValue("skill_unlock_tip").c_str(), pBaseInfo->needlevel);
            m_pUnLockTip->setString(str);
        }
        
        m_pUnUpgradeTip->setVisible(false);
        
        m_pUpgradeNode->setVisible(false);
        m_pInfoNode->setVisible(true);
    }
}

void SkillInfoUI::menuTrainCallback(CCObject *pSender, CCControlEvent event)
{
    if (m_pTrainAnimation) {
        return;
    }
    
    int index = (m_skillId/100 % 10000) - 1;
    int skillId = m_skillId + HeroMgr::getSingleton().m_myHero.bskill[index];
    
    int tag = ((CCControlButton*)pSender)->getTag();
    
    int exp = 0;
    if (tag == 1) {
        exp = SkillMgr::getSingleton().upgradeSkill(m_trainGems, skillId, 1);
    }
    else if (tag == 2) {
        exp = SkillMgr::getSingleton().upgradeSkill(m_leapGems, skillId, 2);
    }
    
    if (exp > 0) {
        char str[128];
        
        if (HeroMgr::getSingleton().m_myHero.heroid / 100 == 301) {
            sprintf(str, "hero_train_boy_%d.ccbi", (tag-1));
        }else{
            sprintf(str, "hero_train_girl_%d.ccbi", (tag-1));
        }
        
        m_pTrainNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile(str, this, &m_pTrainAnimation);
        if(m_pBtnBack != NULL)
            m_pBtnBack->setEnabled(false);
        
        
        sprintf(str, "ui_skill_%d", 10001 + index);
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        
        CCNode* pSkillNode = m_pTrainNode->getChildByTag(1);
        
        ((CCSprite*)pSkillNode->getChildByTag(1))->setDisplayFrame(pFrame);
        
        sprintf(str, ViewString::getValue("skill_exp_add_tip").c_str(), exp);
        ((CCLabelBMFont*)pSkillNode->getChildByTag(2))->setString(str);
        
        m_pTrainAnimation->setDelegate(this);
        this->addChild(m_pTrainNode);
        
        m_pBtnTrain->setEnabled(false);
        m_pBtnLeap->setEnabled(false);
    }
    
    if (GuideMgr::getSingleton().isGuiding()) {
        GuideMgr::getSingleton().next();
    }
}

void SkillInfoUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->m_pSkillUI->hideSkillInfoUI();
}

void SkillInfoUI::completedAnimationSequenceNamed(const char *name)
{
    if (strcmp(name, "train_skill_am") == 0) {
        this->removeChild(m_pTrainNode);
        m_pTrainAnimation = NULL;
        
        m_pBtnTrain->setEnabled(true);
        m_pBtnLeap->setEnabled(true);
        
        refresh();
        PlayerScene::getSingleton().m_pGameWorld->m_pSkillUI->refresh();
        
        if (GuideMgr::getSingleton().isGuiding()) {
            GuideMgr::getSingleton().next();
        }
    }
    
    if(m_pBtnBack != NULL)
        m_pBtnBack->setEnabled(true);
}


bool SkillInfoUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Name", CCLabelBMFont*, m_pName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Level", CCLabelBMFont*, m_pLevel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "MaxLevel", CCLabelBMFont*, m_pMaxLevel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Exp", CCLabelBMFont*, m_pExp);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CoolDown", CCLabelBMFont*, m_pCoolDown);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Duration", CCLabelBMFont*, m_pDuration);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Desc", CCLabelBMFont*, m_pDesc);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UpgradeDesc", CCLabelBMFont*, m_pUpgradeDesc);

    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnLeap", CCControlButton*, m_pBtnLeap);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnTrain", CCControlButton*, m_pBtnTrain);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "btnBack", CCControlButton*, m_pBtnBack);
    
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UnLockTip", CCLabelBMFont*, m_pUnLockTip);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UnUpgradeTip", CCLabelBMFont*, m_pUnUpgradeTip);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UnUpgradeNode", CCNode*, m_pUnUpgradeNode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "InfoNode", CCNode*, m_pInfoNode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "UpgradeNode", CCNode*, m_pUpgradeNode);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ExpProgress", CCScale9Sprite*, m_pExpProgress);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Icon", CCSprite*, m_pIcon);
    
    return true;
}

SEL_CCControlHandler SkillInfoUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuTrainCallback", SkillInfoUI::menuTrainCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", SkillInfoUI::menuCloseCallback);
    
    return NULL;
}