//
//  SpinUI.cpp
//  GameClient
//
//  Created by Lee  on 13-12-27.
//
//

#include "SpinUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/PlayerScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/HeroMgr.h"
#include "../coc/SkillMgr.h"
#include "../coc/HttpClient.h"
#include "../coc/GuideMgr.h"
#include "../coc/RandMgr.h"
#include "../coc/ConfigValMgr.h"
#include "../coc/SoundMgr.h"

using namespace cocos2d;

enum
{
    kGetAwardSpiteStateOut = 1,
    kGetAwardSpiteStateIn  = 2
};


SpinUI::SpinUI()
{
}

SpinUI::~SpinUI()
{
    LogicInfo::getSingleton().removeListener(this);
    m_pLights->release();
}

void SpinUI::onExit()
{
    CUINode::onExit();
    m_pAnimationManager->setDelegate(NULL);
}

bool SpinUI::init()
{
    bool bRet = false;
    do{
        CC_BREAK_IF(!CUINode::init());
        {
            LogicInfo::getSingleton().addListener(this);
            
            m_pSpinUICCB = CUINodeCache::sharedUINodeCache()->UINodeFromFile("spin.ccbi", this, &m_pAnimationManager);
            m_pAnimationManager->setDelegate(this);
            this->setTargetTouch(true);
            this->setContentSize(m_pSpinUICCB->getContentSize());
            this->addChild(m_pSpinUICCB);
            
            m_pSpinBtn->setEnabled(false);
            
            m_pLoading = CUINodeCache::sharedUINodeCache()->UINodeFromFile("loading1.ccbi", this);
            m_pLoading->setPosition(ccp(m_pSpinUICCB->getContentSize().width/2, m_pMainNode->getContentSize().height/2));
            addChild(m_pLoading);
            
            m_nTBonus = 100000;
            m_pLastAwardSprite = NULL;
            
            m_nGetAwardIndex = m_nLastAwardIndex = -1;
            m_nAwardSpriteState = kGetAwardSpiteStateIn;
            m_fSpeed = 2;
            m_nEndRotation = -1;
            m_fCurRotation = 0;
            m_nLastRotation = 0;
            m_fSoundDt = 0.0;
            
            initLights();
            
            bRet = true;
        }
    }
    while (0);
    return bRet;
}

void SpinUI::update(float dt)
{
    if (!m_pSpinBtn->isEnabled())
    {
        m_fCurRotation+=m_fSpeed;
        m_pMainNode->setRotation(m_nLastRotation + m_fCurRotation);
        
        if (m_fCurRotation > m_nEndRotation && m_nEndRotation>0)
        {
            runEnd();
            return;
        }
        if (m_fCurRotation > 90 && m_fCurRotation < 360*3)
        {
            m_fSpeed *=1.1;
            if (m_fSpeed>8)
                m_fSpeed = 8;
        } else if (m_fCurRotation > 360*3 && m_nEndRotation<0.0)
        {
            m_fSpeed/=1.01;
            if (m_fSpeed < 2)
            {
                m_fSpeed = 2;
                m_nEndRotation = ( (int)m_pMainNode->getRotation()/360 + 1 ) * 360 + m_nGetAwardIndex*20-m_nLastRotation;
            }
        }  else if (m_nEndRotation - m_fCurRotation>90)
        {
            m_fSpeed/=1.01;
            if (m_fSpeed < 1)
                m_fSpeed = 1;
        }  else if (m_nEndRotation - m_fCurRotation < 90 && m_nEndRotation>0)
        {
            m_fSpeed/=1.01;
            if (m_fSpeed < 0.5)
                m_fSpeed = 0.5;
        }
        
        m_fSoundDt+=dt;
        if (m_fSoundDt>0.8/m_fSpeed)
        {
            SoundMgr::getSingleton().playEffect("wheel_spin.mp3",0);
            m_fSoundDt = 0.0;
        }
    }
    
    
}

void SpinUI::updateLightAction(float dt)
{
    m_nLightIndex++;
    if (m_nLightIndex == m_pLights->count())
    {
        m_nLightIndex = 0;
    }
    CCSprite *pCurLight = (CCSprite *)m_pLights->objectAtIndex(m_nLightIndex);
    
    char name[64];
    sprintf(name, "spin_light_%d_1",pCurLight->getTag()%3);
    CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(name);
    pCurLight->setDisplayFrame(pFrame);
    
    sprintf(name, "spin_light_%d_0",m_pLastLight->getTag()%3);
    pFrame = ImgMgr::getSingleton().getFrame(name);
    m_pLastLight->setDisplayFrame(pFrame);
    m_pLastLight = pCurLight;
}


void SpinUI::initLights()
{
    m_pLights = CCArray::create();
    m_pLights->retain();
    m_pLastLight = NULL;
    m_nLightIndex = 0;
    for (int i = 9; i>=0; i--)
    {
        CCSprite *sprite = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("spin_light_%d_0.png",i%3)->getCString());
        m_pMainNode->getParent()->addChild(sprite,10);
        sprite->setTag(i);
        
        CCPoint pos;
        pos.x = cos(CC_DEGREES_TO_RADIANS(108+i*9.5)) *320 + m_pMainNode->getPosition().x;
        pos.y = sin(CC_DEGREES_TO_RADIANS(108+i*9.5)) *320 + m_pMainNode->getPosition().y;
        sprite->setPosition(pos);
        m_pLights->addObject(sprite);
    }
    
    for (int i = 9; i>=0; i--)
    {
        CCSprite *sprite = CCSprite::createWithSpriteFrameName(CCString::createWithFormat("spin_light_%d_0.png",i%3)->getCString());
        m_pMainNode->getParent()->addChild(sprite,10);
        sprite->setTag(i);
        
        CCPoint pos;
        pos.x = cos(CC_DEGREES_TO_RADIANS(-13+i*9.5)) *320 + m_pMainNode->getPosition().x;
        pos.y = sin(CC_DEGREES_TO_RADIANS(-13+i*9.5)) *320 + m_pMainNode->getPosition().y;
        sprite->setPosition(pos);
        m_pLights->addObject(sprite);
    }
    m_pLastLight = (CCSprite *)m_pLights->objectAtIndex(m_nLightIndex);
    this->schedule(schedule_selector(SpinUI::updateLightAction),0.15);
}


void SpinUI::refresh(const void *pVal)
{
    if (!pVal)
        return;
    char pString[64];
    const Json::Value jVal = *(Json::Value *)pVal;
    
    m_nCost = getAsInt(jVal["cost"]);//jVal["cost"].asInt();
	m_nTBonus = getAsInt(jVal["pool"]);
    //std::string  sBonus = jVal["pool"].asString();
    //m_nTBonus = atoi(sBonus.c_str());
    
    m_nGetAwardIndex = jVal["index"].asInt()-1;
    
    sprintf(pString, "%d",m_nCost);
    m_pCostLabel->setString(pString);
    sprintf(pString, "%d",m_nTBonus);
    m_pTBonusLabel->setString(pString);
    
    for (int i = 0; i<18; i++)
    {
        CCNode *pNode = m_pMainNode->getChildByTag(i);
        CCSprite *pType = (CCSprite *)pNode->getChildByTag(1);
        CCLabelBMFont *pValLabel = (CCLabelBMFont *)pNode->getChildByTag(2);
        SpinItemInfo info = ConfigValMgr::getSingleton().getSpinItemInfo(i%9);
        
        if (std::strcmp(info.type.c_str(), "gem") == 0)
        {
            pType->setDisplayFrame(ImgMgr::getSingleton().getFrame("map_Gems_icon"));
            sprintf(pString, "%d",(int)info.val);
            pValLabel->setString(pString);
        } else if (std::strcmp(info.type.c_str(), "gem_per") == 0)
        {
            pType->setDisplayFrame(ImgMgr::getSingleton().getFrame("map_Gems_icon"));
            sprintf(pString, "%d",(int)(info.val * m_nTBonus));
            pValLabel->setString(pString);
            
        } else if (std::strcmp(info.type.c_str(), "gold") == 0)
        {
            pType->setDisplayFrame(ImgMgr::getSingleton().getFrame("map_Coins_icon"));
            sprintf(pString, "%d",(int)info.val);
            pValLabel->setString(pString);
        } else if (std::strcmp(info.type.c_str(), "oil") == 0)
        {
            pType->setDisplayFrame(ImgMgr::getSingleton().getFrame("map_Elixir_icon"));
            sprintf(pString, "%d",(int)info.val);
            pValLabel->setString(pString);
        }
    }
    
    //    CCLOG(" val is %s",jVal.toStyledString().c_str());
    
}

#pragma mark - run

void SpinUI::beginRun()
{
    if (m_nAwardSpriteState &&  m_nAwardSpriteState == kGetAwardSpiteStateIn)
    {
        scheduleUpdate();
    }
}

void SpinUI::runEnd()
{
    m_nLastAwardIndex = m_nGetAwardIndex;
    
    playSound();
    runOut();
    cmpBonus();
    
    m_pMainNode->setRotation(m_nLastRotation + m_nEndRotation);
    m_fSpeed        = 2;
    m_nEndRotation  = -1;
    m_fCurRotation  = 0;
    m_nLastRotation = m_pMainNode->getRotation();
    
    m_pLoading->setVisible(true);
    m_pCloseBtn->setEnabled(true);

    unscheduleUpdate();
}

void SpinUI::runOut()
{
    m_pLastAwardSprite = (CCSprite *)m_pMainNode->getChildByTag(m_nLastAwardIndex);
    CCPoint pos;
    pos.x = cos(CC_DEGREES_TO_RADIANS(90+m_nLastAwardIndex*20)) *250 + m_pLastAwardSprite->getPosition().x;
    pos.y = sin(CC_DEGREES_TO_RADIANS(90+m_nLastAwardIndex*20)) *250 + m_pLastAwardSprite->getPosition().y;
    CCEaseElasticOut *pEaseIn = CCEaseElasticOut::create(CCMoveTo::create(1.0, pos), 0.8);
//    CCCallFunc *pFun = CCCallFunc::create(this, callfunc_selector(SpinUI::fly));
    m_pLastAwardSprite->runAction(pEaseIn);
    m_nAwardSpriteState = kGetAwardSpiteStateOut;
}


void SpinUI::runIn(bool bIsBeginRun)
{
    CCPoint pos;
    pos.x = cos(CC_DEGREES_TO_RADIANS(90+m_nLastAwardIndex*20)) *-250 + m_pLastAwardSprite->getPosition().x;
    pos.y = sin(CC_DEGREES_TO_RADIANS(90+m_nLastAwardIndex*20)) *-250 + m_pLastAwardSprite->getPosition().y;
    CCEaseElasticIn *easeIn = CCEaseElasticIn::create(CCMoveTo::create(1.0, pos), 0.8);
    CCCallFunc *fun = CCCallFunc::create(this, callfunc_selector(SpinUI::beginRun));
    
    if (bIsBeginRun)
    {
        m_pLastAwardSprite->runAction(CCSequence::createWithTwoActions(easeIn, fun));
    } else
    {
        m_pLastAwardSprite->runAction(easeIn);
    }
    m_nAwardSpriteState = kGetAwardSpiteStateIn;
}

void SpinUI::cmpBonus()
{
    SpinItemInfo info = ConfigValMgr::getSingleton().getSpinItemInfo(m_nLastAwardIndex);
    if (std::strcmp(info.type.c_str(), "gem") == 0)
    {
        LogicInfo::getSingleton().chgGem(info.val);
    }else if (std::strcmp(info.type.c_str(), "gem_per") == 0)
    {
        LogicInfo::getSingleton().chgGold((int)(info.val*m_nTBonus));
        
    }  else if (std::strcmp(info.type.c_str(), "gold") == 0)
    {
        LogicInfo::getSingleton().chgGold(info.val);
        
    } else if (std::strcmp(info.type.c_str(), "oil") == 0)
    {
        LogicInfo::getSingleton().chgOil(info.val);
    }
    LogicInfo::getSingleton().onNotify(NOTIFY_BASEINFO);
}

void SpinUI::playSound()
{
    if (m_nLastAwardIndex == 0 || m_nLastAwardIndex == 2 ||m_nLastAwardIndex == 7)
    {
        SoundMgr::getSingleton().playEffect("jackpot_win.mp3");
    } else  if (m_nLastAwardIndex == 1 || m_nLastAwardIndex == 3 ||m_nLastAwardIndex == 6 ||m_nLastAwardIndex == 8)
    {
        SoundMgr::getSingleton().playEffect("special_win.mp3");
    } else
    {
        SoundMgr::getSingleton().playEffect("standard_win.mp3");
    }
    SoundMgr::getSingleton().playEffect("wheel_stop.mp3");
}

void SpinUI::fly()
{
    SpinItemInfo info = ConfigValMgr::getSingleton().getSpinItemInfo(m_nLastAwardIndex);
    CCPoint startPos = m_pLastAwardSprite->convertToWorldSpace(m_pLastAwardSprite->getChildByTag(1)->getPosition());
    CCPoint flyToPos;
    std::string frameName;
    if (std::strcmp(info.type.c_str(), "gem") == 0)
    {
        frameName = "map_Gems_icon.png";
        flyToPos = PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->m_pIconGem->getPosition();
    }else if (std::strcmp(info.type.c_str(), "gem_per") == 0)
    {
        frameName = "map_Gems_icon.png";
        flyToPos = PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->m_pIconGem->getPosition();
    }  else if (std::strcmp(info.type.c_str(), "gold") == 0)
    {
        frameName = "map_Coins_icon.png";
        flyToPos = PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->m_pIconGold->getPosition();
        
    } else if (std::strcmp(info.type.c_str(), "oil") == 0)
    {
        frameName = "map_Elixir_icon.png";
        flyToPos = PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->m_pIconElixir->getPosition();
    }else
    {
        return;
    }
    
    for (int  i = 0; i<10;i++)
    {
        CCSprite *pSprite = CCSprite::createWithSpriteFrameName(frameName.c_str());
        pSprite->setPosition(startPos);
        CCDelayTime *pDelay = CCDelayTime::create(i*0.1);
        CCMoveTo    *pMove  = CCMoveTo::create(0.3, flyToPos);
        CCScaleTo   *pScale = CCScaleTo::create(0.3, 0.8);
        CCCallFuncN *pFun   = CCCallFuncN::create(this, callfuncN_selector(SpinUI::flyEnd));
        pSprite->runAction(CCSequence::create(pDelay,CCSpawn::createWithTwoActions(pMove, pScale), pFun, NULL));
        addChild(pSprite,100-i);
        
        pSprite->setTag(i);
    }
}

void SpinUI::flyEnd(cocos2d::CCNode *pSender)
{
    pSender->removeFromParentAndCleanup(true);
    if (pSender->getTag() == 9 &&  m_nAwardSpriteState == kGetAwardSpiteStateOut)
    {
        runIn(false);
    }
    SoundMgr::getSingleton().playEffect("btn.m4a", 0);
}

#pragma mark - notify
bool SpinUI::onNotify(int notifyid, const void* pVal)
{
    if(notifyid == NOTIFY_SPIN_AWARD)
    {
        m_pSpinBtn->setEnabled(true);
        m_pLoading->setVisible(false);
        refresh(pVal);
    }
    return true;
}

#pragma mark - ccb

void SpinUI::menuSpinCallback(CCObject* pSender, CCControlEvent event)
{
    if (m_pLastAwardSprite && m_pLastAwardSprite->numberOfRunningActions())
    {
        return;
    }
    
    if (!m_pCloseBtn->isEnabled())
    {
        return;
    }
    if (LogicInfo::getSingleton().m_myInfo.gemex < m_nCost)
    {
        MessageUI::show(MSG_NOGEM, NULL, NULL);
        return;
    }
    m_pSpinBtn->setEnabled(false);
    m_pCloseBtn->setEnabled(false);
    if (m_pLastAwardSprite && m_nAwardSpriteState == kGetAwardSpiteStateOut)
    {
        runIn(true);
    } else
    {
        beginRun();
    }
    LogicInfo::getSingleton().chgGem(-m_nCost);
    LogicInfo::getSingleton().onNotify(NOTIFY_BASEINFO);
    SoundMgr::getSingleton().playEffect("spin_button.m4a");
}

void SpinUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    m_pSpinBtn->setEnabled(false);
    m_pCloseBtn->setEnabled(false);
    m_pLoading->setVisible(false);
    m_pAnimationManager->runAnimationsForSequenceNamed("spin_am_out");
    
}

bool SpinUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "MainNode", CCNode*, m_pMainNode);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "SpinBtn", CCControlButton*, m_pSpinBtn);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CloseBtn", CCControlButton*, m_pCloseBtn);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "TBonusLabel", CCLabelBMFont*, m_pTBonusLabel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "CostLabel", CCLabelBMFont*, m_pCostLabel);
    
    return true;
}

SEL_CCControlHandler SpinUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSpinCallback", SpinUI::menuSpinCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", SpinUI::menuCloseCallback);
    
    return NULL;
}

void SpinUI::completedAnimationSequenceNamed(const char *name)
{
    if (strcmp(name, "spin_am_out") == 0)
    {
        PlayerScene::getSingleton().m_pGameWorld->hideSpinUI();
    }
}

