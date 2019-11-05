#include "ChgScene.h"
#include "../coc/ImgMgr.h"
#include "../GameWorldScene.h"
#include "../coc/HttpClient.h"
#include "../coc/PlayerScene.h"
#include "../coc/LogicInfo.h"
#include "../coc/GuideMgr.h"

using namespace cocos2d;

ChgScene::ChgScene()
{
}

ChgScene::~ChgScene()
{
//	HttpClient::getSingleton().removeListener(this);
}

ChgScene* ChgScene::create(int mode)
{
	ChgScene *pRet = new ChgScene();
	if (pRet && pRet->init(mode))
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

bool ChgScene::init(int mode)
{
	bool bRet = false;
    do 
    {
		CC_BREAK_IF(!CUINode::init());

		m_isTimeFinish = false;
		m_isMsgFinish = false;

        HttpClient::getSingleton().addListener(this);
        
        CCNode* pChgUI = CUINodeCache::sharedUINodeCache()->UINodeFromFile("find_searching.ccbi", this);
        this->setContentSize(pChgUI->getContentSize());
        this->addChild(pChgUI);

        schedule(schedule_selector(ChgScene::onIdle));
		schedule(schedule_selector(ChgScene::OnSplashTimer), 1.0f);

        if (mode != CHANGE_TYPE_FINDMATCH) {
            m_pTitle->setVisible(false);
            m_pBtnBack->setVisible(false);
        } else if (!GuideMgr::getSingleton().isGuideEnd()) {
            m_pBtnBack->setVisible(false);
        }
        
		PlayerScene::getSingleton().m_pGameWorld->hideMainUI();
        PlayerScene::getSingleton().m_pGameWorld->hideVisitUI();
        
        if (NETWORK_TIME_DEBUG) {
            m_pTestLog = CCLabelBMFont::create("", "Font/tbFont_b.fnt", -1, kCCTextAlignmentCenter);
            m_pTestLog->setAnchorPoint(ccp(0.5, 1));
            m_pTestLog->setScale(0.5);
            m_pTestLog->setColor(ccRED);
            m_pTestLog->setPosition(ccp(this->getContentSize().width/2, this->getContentSize().height * 0.8));
            this->addChild(m_pTestLog);
        }
        
		//m_bModal = true;
		bRet = true;
	}while(0);

	return bRet;
}

void ChgScene::onIdle(float dt)
{
    if (m_strDebugLog != LogicInfo::getSingleton().m_strDebugLog) {
        m_strDebugLog = LogicInfo::getSingleton().m_strDebugLog;
        
        m_pTestLog->setString(m_strDebugLog.c_str());
    }
}

bool ChgScene::onProc(const Json::Value& val)
{
    if(val["cid"].asString() == "otheruserok")
	{
		m_isMsgFinish = true;

		if(m_isTimeFinish && m_isMsgFinish)
		{
			onFinish();
		}
    }
    else if(val["cid"].asString() == "leagueuserinfook")
	{
		m_isMsgFinish = true;
		if(m_isTimeFinish && m_isMsgFinish)
		{
			onFinish();
		}
    }
    else if(val["cid"].asString() == "clanwaruserok")
	{
		m_isMsgFinish = true;
		if(m_isTimeFinish && m_isMsgFinish)
		{
			onFinish();
		}
    }
    else if(val["cid"].asString() == "groupwaruserok")
	{
		m_isMsgFinish = true;
		if(m_isTimeFinish && m_isMsgFinish)
		{
			onFinish();
		}
    }
	else if(val["cid"].asString() == "visitotherok")
	{
		PlayerScene::getSingleton().m_pGameWorld->hideChgScene();
        
        onFinish();
	}
    else if(val["cid"].asString() == "initok")
    {
        PlayerScene::getSingleton().m_pGameWorld->hideChgScene();
        
        onFinish();
    }
    else if(val["cid"].asString() == "replay")
    {
        PlayerScene::getSingleton().m_pGameWorld->hideChgScene();
        
        onFinish();
    }
    
    return true;
}

void ChgScene::OnSplashTimer(float dt)
{
	m_isTimeFinish = true;

	if(m_isTimeFinish && m_isMsgFinish)
	{
		onFinish();
	}
}

void ChgScene::onFinish()
{    
	if(LOGICSTATE_ATTACK == LogicInfo::getSingleton().m_curState)
	{
		PlayerScene::getSingleton().m_pGameWorld->showFightUI();
        
        if (GuideMgr::getSingleton().isGuiding()) {
            GuideMgr::getSingleton().next();
        }

		PlayerScene::getSingleton().m_pGameWorld->hideChgScene();
	}
	else if(LOGICSTATE_VIEW == LogicInfo::getSingleton().m_curState)
	{
		PlayerScene::getSingleton().m_pGameWorld->showVisitUI();
        
		PlayerScene::getSingleton().m_pGameWorld->hideChgScene();
	}
    else if(LOGICSTATE_NORMAL == LogicInfo::getSingleton().m_curState)
    {
        PlayerScene::getSingleton().m_pGameWorld->showMainUI();
        
        if (GuideMgr::getSingleton().isGuiding()) {
            GuideMgr::getSingleton().next();
        }

		PlayerScene::getSingleton().m_pGameWorld->hideChgScene();
        
        if (PlayerScene::getSingleton().m_backHomeView == BACK_HOME_PVE_MAP) {
            
            PlayerScene::getSingleton().m_pGameWorld->showFindMatchUI();
        }
        else if (PlayerScene::getSingleton().m_backHomeView == BACK_HOME_CLANWAR_BATTLE) {

            PlayerScene::getSingleton().m_pGameWorld->showClanWarMapUI();
            PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->showClanWarBattleUI();
        }
        else if (PlayerScene::getSingleton().m_backHomeView == BACK_HOME_GROUPWAR_BATTLE) {

            PlayerScene::getSingleton().m_pGameWorld->showClanWarMapUI();
            PlayerScene::getSingleton().m_pGameWorld->m_pClanWarMapUI->showGroupWarBattleUI();
        }
        
        PlayerScene::getSingleton().m_backHomeView = BACK_HOME_TOWN_HALL;
    }
    else if(LOGICSTATE_REPLAY == LogicInfo::getSingleton().m_curState)
    {
		PlayerScene::getSingleton().m_pGameWorld->showReplayUI();
		PlayerScene::getSingleton().m_pGameWorld->hideChgScene();
    }
    
    CCSize size = PlayerScene::getSingleton().m_pRoot->m_size;
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    PlayerScene::getSingleton().setScale(SCENE_NORMAL_SCALE, ccp(0, 0));
    PlayerScene::getSingleton().setPosition(-(size.width*SCENE_NORMAL_SCALE-winSize.width)/2, -(size.height*SCENE_NORMAL_SCALE-winSize.height)/2);
}

void ChgScene::menuBackCallback(CCObject* pSender, CCControlEvent event)
{
    m_pTitle->setVisible(false);
    m_pBtnBack->setVisible(false);
    
    PlayerScene::getSingleton().m_pGameWorld->showChgScene(CHANGE_TYPE_BACKHOME);
    
    LogicInfo::getSingleton().goHome();
}

bool ChgScene::onAssignCCBMemberVariable(CCObject* pTarget, const char* pMemberVariableName, CCNode* pNode)
{
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "Title", CCLabelBMFont*, m_pTitle);
    CCB_MEMBERVARIABLEASSIGNER_BIND(pTarget, "BtnBack", CCControlButton*, m_pBtnBack);
    
    return NULL;
}

SEL_CCControlHandler ChgScene::onResolveCCBCCControlSelector(CCObject * pTarget, const char* pSelectorName)
{
    CCB_SELECTORRESOLVER_CCCONTROL_GLUE(pTarget, "menuBackCallback", ChgScene::menuBackCallback);
    
    return NULL;
}
