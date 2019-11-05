#include "BuildUI.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/PlayerScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/GuideMgr.h"

using namespace cocos2d;

BuildUI::BuildUI()
{
}

BuildUI::~BuildUI()
{
    LogicInfo::getSingleton().removeListener(this);
}

bool BuildUI::init()
{
	bool bRet = false;
    do 
    {
		CC_BREAK_IF(!CUINode::init());

		{
			LogicInfo::getSingleton().addListener(this);
        
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame("ui_btn_okbuilding");
            m_pBtnOK = CCControlButton::create(CCScale9Sprite::createWithSpriteFrame(pFrame));
            m_pBtnOK->setPreferredSize(CCSizeMake(144, 144));
            m_pBtnOK->setBackgroundSpriteFrameForState(pFrame, CCControlStateHighlighted);
            pFrame = ImgMgr::getSingleton().getFrame("ui_btn_okbuilding_1");
            m_pBtnOK->setBackgroundSpriteFrameForState(pFrame, CCControlStateDisabled);
            
            m_pBtnOK->addTargetWithActionForControlEvents(this, cccontrol_selector(BuildUI::menuOKCallback), CCControlEventTouchUpInside);
            
            CCSpriteFrame* pFrame2 = ImgMgr::getSingleton().getFrame("ui_btn_cancelbuilding");
            m_pBtnCancel = CCControlButton::create(CCScale9Sprite::createWithSpriteFrame(pFrame2));
            m_pBtnCancel->setPreferredSize(CCSizeMake(144, 144));
            m_pBtnCancel->setBackgroundSpriteFrameForState(pFrame2, CCControlStateHighlighted);
            
            m_pBtnCancel->addTargetWithActionForControlEvents(this, cccontrol_selector(BuildUI::menuCancelCallback), CCControlEventTouchUpInside);
            
            this->addChild(m_pBtnOK);
            this->addChild(m_pBtnCancel);
            
			m_pBtnOK->setVisible(false);
			m_pBtnCancel->setVisible(false);
            
			PlayerScene::getSingleton().setBuildUI(this);

		}
		bRet = true;
	}while(0);

	return bRet;
}

// a selector callback
void BuildUI::menuOKCallback(CCObject* pSender, CCControlEvent event)
{
	if(PlayerScene::getSingleton().m_pCurBuilding != NULL)
	{
        int need = 0;
        int ret = LogicInfo::getSingleton().buildBuilding(*PlayerScene::getSingleton().m_pCurBuilding, need);
        if(ret <= 0)
        {
            // msg
            MessageUI::show(ret, "", "", need, GETGEM_BUILD_BUILDING);
        }
        else
        {
            if (GuideMgr::getSingleton().isGuiding()) {
                GuideMgr::getSingleton().next();
            }
        }
	}
}

// a selector callback
void BuildUI::menuCancelCallback(CCObject* pSender, CCControlEvent event)
{
    if(!GuideMgr::getSingleton().isGuideEnd())
        return;
        
	if(PlayerScene::getSingleton().m_pCurBuilding != NULL) {
        if (PlayerScene::getSingleton().m_isEditMode) {
            PlayerScene::getSingleton().moveBuilding2EditList(*PlayerScene::getSingleton().m_pCurBuilding);
            PlayerScene::getSingleton().hideBuildUI();
        }
        else {
            LogicInfo::getSingleton().cancelFirstBuinding(*PlayerScene::getSingleton().m_pCurBuilding);
        }
    }


}

bool BuildUI::onNotify(int notifyid, const void* pVal)
{
    
	return true;
}

void BuildUI::setVisible(bool visible)
{
    CUINode::setVisible(visible);
}
