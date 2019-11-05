//
//  CreateUI.cpp
//  GameClient
//
//  Created by chowray on 13-5-13.
//
//

#include "CreateUI.h"
#include "../coc/ImgMgr.h"
#include "../coc/PlayerScene.h"
#include "../GameWorldScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/HttpClient.h"
#include "../coc/HeroMgr.h"
#include "../coc/PersonMgr.h"
#include "../coc/GuideMgr.h"

using namespace cocos2d;

CreateUI::CreateUI()
{
    
}

CreateUI::~CreateUI()
{
    
}

bool CreateUI::init()
{
    bool bRet = false;
    do
    {
		CC_BREAK_IF(!CUINode::init());
        {

            m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("named.ccbi", this);
            m_pView->setTag(2);
            
            m_pInput = CTextInputField::create(m_pInputBg->getContentSize(), CCScale9Sprite::create());
            m_pInput->setFontColor(ccBLACK);
            m_pInput->setMaxLength(16);
            m_pInput->setAnchorPoint(ccp(0, 0));
            m_pInput->setPosition(ccp(0, 0));
            m_pInput->setReturnType(kKeyboardReturnTypeDone);
            m_pInputBg->addChild(m_pInput);
            
            if(!LogicInfo::getSingleton().m_defaultUserName.empty())
            {
                m_pInput->setText(LogicInfo::getSingleton().m_defaultUserName.c_str());
                m_pInput->setPlaceHolder(LogicInfo::getSingleton().m_defaultUserName.c_str());
            }
            
            this->setContentSize(m_pView->getContentSize());
            this->setTargetTouch(true);
            this->addChild(m_pView);
            
            m_sex = 0;

        }
        
		bRet = true;
	}while(0);
    
	return bRet;
}

void CreateUI::menuOKCallback(CCObject* pSender, CCControlEvent cce)
{
//    if (m_pView->getTag() == 1) {
//        
//        int heroid = 0;
//        if(m_sex == 0)
//        {
//            heroid = 30101;
//        }
//        else
//        {
//            heroid = 30201;
//        }
//        
//        HeroMgr::getSingleton().myHero.heroid = heroid;
//        HeroMgr::getSingleton().myHero.hp = PersonMgr::getSingleton().getSoldierInfo(heroid)->hp;
//        HeroMgr::getSingleton().myHero.maxhp = PersonMgr::getSingleton().getSoldierInfo(heroid)->hp;
//        HeroMgr::getSingleton().myHero.curexp = 0;
//        HeroMgr::getSingleton().myHero.maxexp = PersonMgr::getSingleton().getSoldierInfo(heroid)->needexp;
//        HeroMgr::getSingleton().myHero.skill0 = 0;
//        HeroMgr::getSingleton().myHero.skill3 = 0;
//        
//        LogicInfo::getSingleton().selectHero(heroid);
//        
//        //named view
//        m_pView->removeFromParent();
//        m_pView = CUINodeCache::sharedUINodeCache()->UINodeFromFile("named.ccbi", this);
//        m_pView->setTag(2);
//        
//        m_pInput = CCEditBox::create(m_pInputBg->getContentSize(), CCScale9Sprite::create());
//        m_pInput->setFontColor(ccBLACK);
//        m_pInput->setMaxLength(16);
//        m_pInput->setAnchorPoint(ccp(0, 0));
//        m_pInput->setPosition(ccp(0, 0));
//        
//        m_pInputBg->addChild(m_pInput);
//        
//        m_pInput->setReturnType(kKeyboardReturnTypeDone);
//        
//        char str[128];
//        sprintf(str, "hero_%d", heroid/100);
//        m_pHero->setDisplayFrame(ImgMgr::getSingleton().getFrame(str));
////        if (m_sex == 1) {
////            m_pHero->setFlipX(true);
////        }
//        
//        if(LogicInfo::getSingleton().m_defaultUserName.empty())
//        {
//            char str[128];
//            sprintf(str, "player#%d#", LogicInfo::getSingleton().m_myInfo.uid);
//            m_pInput->setText(str);
//            m_pInput->setPlaceHolder(str);
//        }
//        else
//        {
//            m_pInput->setText(LogicInfo::getSingleton().m_defaultUserName.c_str());
//            m_pInput->setPlaceHolder(LogicInfo::getSingleton().m_defaultUserName.c_str());
//        }
//        
//        this->addChild(m_pView);
//    }
//    else
//    {
        std::string str = m_pInput->getText();
        if (str.size() <= 0) {
            str = m_pInput->getPlaceHolder();
        }
        
        if (str.size() <= 0) {
            return;
        }
        else
        {
            LogicInfo::getSingleton().updateUserName(str.c_str());
        }
        
        PlayerScene::getSingleton().m_pGameWorld->showMainUI();
        PlayerScene::getSingleton().m_pGameWorld->hideCreateUI();
    
        if (GuideMgr::getSingleton().isGuiding()) {
            GuideMgr::getSingleton().next();
        }
    
//    }
}

void CreateUI::menuBoyCallback(CCObject* pSender, CCControlEvent cce)
{
    m_pBtnHeroBoy->setEnabled(false);
    m_pBtnHeroGirl->setEnabled(true);
    m_pBtnHeroBoy->setColor(ccc3(255, 255, 255));
    m_pBtnHeroGirl->setColor(ccc3(128, 128, 128));
    
    m_sex = 0;
}

void CreateUI::menuGirlCallback(CCObject* pSender, CCControlEvent cce)
{
    m_pBtnHeroBoy->setEnabled(true);
    m_pBtnHeroGirl->setEnabled(false);
    m_pBtnHeroBoy->setColor(ccc3(128, 128, 128));
    m_pBtnHeroGirl->setColor(ccc3(255, 255, 255));
    
    m_sex = 1;
}

bool CreateUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "InputBg", CCScale9Sprite*, m_pInputBg);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnHeroBoy", CCControlButton*, m_pBtnHeroBoy);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnHeroGirl", CCControlButton*, m_pBtnHeroGirl);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Hero", CCSprite*, m_pHero);
    
    return NULL;
}

SEL_CCControlHandler CreateUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBoyCallback", CreateUI::menuBoyCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuGirlCallback", CreateUI::menuGirlCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuOKCallback", CreateUI::menuOKCallback);
    
    return NULL;
}

