//
//  SettingUI.cpp
//  GameClient
//
//  Created by qsun on 13-11-14.
//
//

#include "SettingUI.h"
#include "CUserData.h"
#include "../coc/PlayerScene.h"
#include "../coc/SoundMgr.h"
#include "../coc/ImgMgr.h"
#include "GameWorldScene.h"

USING_NS_COC;
USING_NS_CC_ZEPHYR;

SettingUI::SettingUI()
{
    
}

SettingUI::~SettingUI()
{
}

bool SettingUI::init()
{
    if (CUINode::init()) {
        CCNode* pSettingUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("setting.ccbi", this);
        
        this->setContentSize(pSettingUI->getContentSize());
        this->setTargetTouch(true);
        
        this->addChild(pSettingUI);
        
        m_pIconGmail->setVisible(false);
        m_pIconGameCenter->setVisible(false);
        m_googleLabel->setVisible(false);
        m_googleButton->setVisible(false);
        
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        m_pIconGmail->setVisible(true);
        m_googleLabel->setVisible(true);
        m_googleButton->setVisible(true);
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        m_pIconGameCenter->setVisible(true);
#endif
        
        if(FacebookMgr::getSingleton().isLogin() && FacebookMgr::getSingleton().isOpenState())
        {
            m_pIconGmail->setVisible(false);
            m_pIconGameCenter->setVisible(false);
        }
        
        if (CCUserDefault::sharedUserDefault()->getBoolForKey(GP_FREE_GEMS)
            || isUserMask(LogicInfo::getSingleton().m_myInfo.mask[0], USER_MASK1_GOOGLE_PLUS)) {
            m_googleGemSprite->setVisible(false);
        }
            
        refreshInfo();
    }
    
    return true;
}

void SettingUI::refreshInfo()
{
    bool musicOn = SoundMgr::getSingleton().m_musicOn;
    bool soundOn = SoundMgr::getSingleton().m_soundOn;
    
    CCSpriteFrame* pFrame0 = ImgMgr::getSingleton().getFrame("setting_on");
    CCSpriteFrame* pFrame1 = ImgMgr::getSingleton().getFrame("setting_off");
    if (musicOn) {
        
        m_pBtnMusic->setBackgroundSpriteFrameForState(pFrame0, CCControlStateNormal);
        m_pBtnMusic->setBackgroundSpriteFrameForState(pFrame1, CCControlStateHighlighted);
        
        ((CCLabelBMFont*)m_pBtnMusic->getChildByTag(1))->setString(ViewString::getValue("sound_on").c_str());
        
    }else{
        
        m_pBtnMusic->setBackgroundSpriteFrameForState(pFrame1, CCControlStateNormal);
        m_pBtnMusic->setBackgroundSpriteFrameForState(pFrame0, CCControlStateHighlighted);
        
        ((CCLabelBMFont*)m_pBtnMusic->getChildByTag(1))->setString(ViewString::getValue("sound_off").c_str());
    }
    
    if (soundOn) {
        
        m_pBtnSound->setBackgroundSpriteFrameForState(pFrame0, CCControlStateNormal);
        m_pBtnSound->setBackgroundSpriteFrameForState(pFrame1, CCControlStateHighlighted);
        
        ((CCLabelBMFont*)m_pBtnSound->getChildByTag(1))->setString(ViewString::getValue("sound_on").c_str());
    }else{
        
        m_pBtnSound->setBackgroundSpriteFrameForState(pFrame1, CCControlStateNormal);
        m_pBtnSound->setBackgroundSpriteFrameForState(pFrame0, CCControlStateHighlighted);
        
        ((CCLabelBMFont*)m_pBtnSound->getChildByTag(1))->setString(ViewString::getValue("sound_off").c_str());
    }
    
    
    CCNode *m_disFBSprite = m_pBtnFB->getChildByTag(1);
    CCNode *m_disFBLabel = m_pBtnFB->getChildByTag(2);
    CCNode *m_FBSprite = m_pBtnFB->getChildByTag(3);
    CCNode *m_FBLabel = m_pBtnFB->getChildByTag(4);
    
    if(FacebookMgr::getSingleton().isLogin())
    {
        m_disFBSprite->setVisible(false);
        m_disFBLabel->setVisible(false);
        m_FBSprite->setVisible(true);
        m_FBLabel->setVisible(true);
    }
    else
    {
        m_disFBSprite->setVisible(true);
        m_disFBLabel->setVisible(true);
        m_FBSprite->setVisible(false);
        m_FBLabel->setVisible(false);
    }
    
    if(!FacebookMgr::getSingleton().isOpenState())
    {
        m_pBtnFB->setVisible(false);
        m_pFBLabel->setVisible(false);
    }

}

void SettingUI::menuMusicCallback(CCObject* pSender, CCControlEvent event)
{
    bool musicOn = !SoundMgr::getSingleton().m_musicOn;
    SoundMgr::getSingleton().m_musicOn = musicOn;
    
    CUserData::setUserDataForKey(GAME_SETTING_MUSIC, musicOn);
    
    refreshInfo();
    
    if (musicOn) {
        
        if(SoundMgr::getSingleton().isBackgroundMusicPaused())
        {
            SoundMgr::getSingleton().resumeBackgroundMusic();
        }
        else
        {
            SoundMgr::getSingleton().playBackgroundMusic("back.mp3");
        }
    }
    else
    {
        if (SoundMgr::getSingleton().isBackgroundMusicPlaying()) {
            SoundMgr::getSingleton().pauseBackgroundMusic();
        }
        else{
            SoundMgr::getSingleton().stopBackgroundMusic();
        }
    }
}

void SettingUI::menuSoundCallback(CCObject* pSender, CCControlEvent event)
{
    bool soundOn = !SoundMgr::getSingleton().m_soundOn;
    SoundMgr::getSingleton().m_soundOn = soundOn;
    
    CUserData::setUserDataForKey(GAME_SETTING_SOUND, soundOn);
    
    refreshInfo();
}

void SettingUI::menuSupportCallback(CCObject *pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->showSupportUI();
    
}
void SettingUI::menuFBCallback(CCObject *pSender, CCControlEvent event)
{
    if(FacebookMgr::getSingleton().m_fbUser.fid.length() > 0)
    {
        PlayerScene::getSingleton().m_pGameWorld->hideSettingUI();
        PlayerScene::getSingleton().m_pGameWorld->showFBLoginUI(1);
    }
    else
    {
        PlayerScene::getSingleton().m_pGameWorld->hideSettingUI();
        PlayerScene::getSingleton().m_pGameWorld->showFBLoginUI(0);
    }
}

void SettingUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    PlayerScene::getSingleton().m_pGameWorld->hideSettingUI();
}

void SettingUI::menuAboutCallback(CCObject* pSender, CCControlEvent event)
{
    m_pAboutNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("setting_about.ccbi",this, &m_pAnimationManager);
    m_pAnimationManager->setDelegate(this);
    addChild(m_pAboutNode, 100);
}

void SettingUI::menuHideCallback(CCObject* pSender, CCControlEvent event)
{
    if(m_pAboutNode)
    {
        m_pAboutNode->removeFromParent();
        m_pAboutNode = NULL;
        m_pAnimationManager->setDelegate(NULL);
    }
}

void SettingUI::menuGoogleCallback(CCObject* pSender, CCControlEvent event)
{
    CAppPlatform::sharedAppPlatform()->openGooglePlusLink();
}

void SettingUI::completedAnimationSequenceNamed(const char *name)
{
    menuHideCallback(NULL,NULL);
}

bool SettingUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnMusic", CCControlButton*, m_pBtnMusic);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnSound", CCControlButton*, m_pBtnSound);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "IconGmail", CCSprite*, m_pIconGmail);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "IconGameCenter", CCSprite*, m_pIconGameCenter);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnSupport", CCControlButton*, m_pBtnSupport);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnAbout", CCControlButton*, m_pBtnAbout);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnFB", CCControlButton*, m_pBtnFB);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "m_googleLabel", CCLabelBMFont*, m_googleLabel);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "m_googleButton", CCControlButton*, m_googleButton);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "m_googleGemSprite", CCSprite*, m_googleGemSprite);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Facebooklabel", CCLabelBMFont*, m_pFBLabel);

    return false;
}

SEL_CCControlHandler SettingUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSupportCallback", SettingUI::menuSupportCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuMusicCallback", SettingUI::menuMusicCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuSoundCallback", SettingUI::menuSoundCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", SettingUI::menuCloseCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuAboutCallback", SettingUI::menuAboutCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuHideCallback", SettingUI::menuHideCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuFBCallback", SettingUI::menuFBCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuGoogleCallback", SettingUI::menuGoogleCallback);
    
    return NULL;
}
