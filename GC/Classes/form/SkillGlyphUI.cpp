//
//  SkillGlyphUI.cpp
//  GameClient
//
//  Created by chowray on 13-9-23.
//
//

#include "SkillGlyphUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/PlayerScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/HeroMgr.h"
#include "../coc/SkillMgr.h"
#include "../coc/GuideMgr.h"

using namespace cocos2d;

SkillGlyphUI::SkillGlyphUI()
{
    m_glyphNums = 8;
}

SkillGlyphUI::~SkillGlyphUI()
{
    LogicInfo::getSingleton().removeListener(this);
}

bool SkillGlyphUI::init()
{
    bool bRet = false;
    
    do{
        
        CC_BREAK_IF(!CUINode::init());
        {
            
            LogicInfo::getSingleton().addListener(this);
            
            CCNode* pGlyphUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("hero_glyph.ccbi", this);
            this->setTargetTouch(true);
            this->setContentSize(pGlyphUI->getContentSize());
            
            this->addChild(pGlyphUI);
            
            char str[128];
            CCSize itemSize(90*getResolutionScale(), 90*getResolutionScale());
            float offX = 10;
            float offY = (m_pItemView->getViewSize().height-itemSize.height)/2;
            
            for (int index = 0; index < m_glyphNums; index ++) {
                
                sprintf(str, "ui_skill_%d", 20001 + index);
                CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
                
                CCControlButton* pItem = CCControlButton::create();
                pItem->setScale(getResolutionScale());
                
                setControlButtonFrameForAllState(pItem, pFrame);
              
                pItem->addTargetWithActionForControlEvents(this, cccontrol_selector(SkillGlyphUI::menuItemCallback), CCControlEventTouchDown|CCControlEventTouchUpInside);
                pItem->setTag((20001+index)*100);
                
                pItem->setPreferredSize(itemSize);
                pItem->setPosition(ccp(index * (itemSize.width+offX), offY));
                
                m_pItemView->addChild(pItem);
                m_skillList.push_back(pItem);
            }
            
            m_pItemView->setDelegate(this);
            m_pItemView->setContentSize(CCSizeMake((itemSize.width+offX)*m_glyphNums, itemSize.height));
        }
        refresh();
        m_pBtnChange->setEnabled(false);
        
        if (GuideMgr::getSingleton().isEquVal(GUIDE_HERO_SKILL, 50)) {
            m_pItemView->setTouchEnabled(false);
        }
        
        m_isDrag = false;
    
        bRet = true;
    }
    while (0);
    
    return bRet;
    
}

void SkillGlyphUI::changeSkill()
{
    SkillMgr::getSingleton().chgSkill3(m_selectId);
    
    refresh();
    PlayerScene::getSingleton().m_pGameWorld->m_pHeroUI->refresh();
        
    m_pBtnChange->setEnabled(false);
        
    PlayerScene::getSingleton().m_pGameWorld->hideSkillGlyphUI();
        
    LogicInfo::getSingleton().onNotify(MSG_CHANGE_GLYPH_SKILL, &NOTIFY_MESSAGE_CLOSE);
}

void SkillGlyphUI::menuChangeCallback(CCObject *pSender, CCControlEvent event)
{
    changeSkill();
    
    if (GuideMgr::getSingleton().isGuiding()) {

        GuideMgr::getSingleton().next();
    }
}

void  SkillGlyphUI::menuItemCallback(CCObject *pSender, CCControlEvent event)
{
    if (event == CCControlEventTouchDown) {
        m_isDrag = false;
        m_dragOffset = m_pItemView->getContentOffset();
    }
    else if (event == CCControlEventTouchUpInside) {
        
        if (m_isDrag) {
            m_isDrag = false;
            return;
        }
        
        int id = ((CCNode*)pSender)->getTag();
        
        m_selectId = id + 1;
        
        char str[128];
        
        sprintf(str, "ui_skill_%d", id/100);
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
        
        m_pIcon->setDisplayFrame(pFrame);
        
        m_pName->setString(SkillMgr::getSingleton().getNode(m_selectId)->name.c_str());
        
        m_pInfo->setString(SkillMgr::getSingleton().getNode(m_selectId)->info.c_str());
        
        m_pBtnChange->setEnabled(true);
        if (m_selectId/100 == HeroMgr::getSingleton().m_myHero.skill3/100) {
            m_pBtnChange->setEnabled(false);
        }
        
        if (GuideMgr::getSingleton().isGuiding()) {
            GuideMgr::getSingleton().next();
        }
    }
}

void SkillGlyphUI::menuCloseCallback(CCObject* pSender, CCControlEvent event)
{
    (PlayerScene::getSingleton().m_pGameWorld)->hideSkillGlyphUI();
}

void SkillGlyphUI::refresh()
{
    char str[128];
    if (HeroMgr::getSingleton().m_myHero.skill3 != 0) {
        
        CCSpriteFrame* pFrame;
        
        sprintf(str, "ui_skill_%d", HeroMgr::getSingleton().m_myHero.skill3/100);
        pFrame = ImgMgr::getSingleton().getFrame(str);
        
        m_pIcon->setDisplayFrame(pFrame);
        
        m_pName->setString(SkillMgr::getSingleton().getNode(HeroMgr::getSingleton().m_myHero.skill3+1)->name.c_str());
        m_pInfo->setString(SkillMgr::getSingleton().getNode(HeroMgr::getSingleton().m_myHero.skill3+1)->info.c_str());
    }else{
        m_pName->setString("");
        m_pInfo->setString("");
    }
    
    if (GuideMgr::getSingleton().isEquVal(GUIDE_HERO_SKILL, 50)) {
        m_pItemView->setTouchEnabled(false);
    }
    
}

bool SkillGlyphUI::onNotify(int notifyid, const void* pVal)
{
    if(NOTIFY_RANDSKILL3 == notifyid)
    {
    }
    return true;
}

bool SkillGlyphUI::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Name", CCLabelBMFont*, m_pName);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Info", CCLabelBMFont*, m_pInfo);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Icon", CCSprite*, m_pIcon);
    
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnChange", CCControlButton*, m_pBtnChange);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "ItemView", CCScrollView*, m_pItemView);
    
    return true;
}

SEL_CCControlHandler SkillGlyphUI::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuChangeCallback", SkillGlyphUI::menuChangeCallback);
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuCloseCallback", SkillGlyphUI::menuCloseCallback);
    
    return NULL;
}

void SkillGlyphUI::scrollViewDidScroll(CCScrollView *view)
{
    if (fabs(m_dragOffset.x - view->getContentOffset().x) > 10) {
       
        m_dragOffset = view->getContentOffset();
        m_isDrag = true;
    }
}
