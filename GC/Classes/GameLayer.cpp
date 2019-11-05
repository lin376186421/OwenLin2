#include "GameLayer.h"
#include "coc/LogicInfo.h"
#include "coc/HttpClient.h"
#include "coc/PlayerScene.h"
#include "coc/ImgMgr.h"
#include "coc/AniObjMgr.h"
#include "coc/GuideMgr.h"
#include "coc/BaseDef.h"
#include "form/MessageUI.h"
#include "coc/ClanMgr.h"
#include "coc/ViewString.h"

USING_NS_COC;
using namespace cocos2d;

int m_timesOut = 0;

GameLayer::GameLayer()
//	: m_lastTime(0)
{
	m_waitNextCtrl = false;
}

GameLayer::~GameLayer()
{
	LogicInfo::getSingleton().setClearTouchFunc(NULL, NULL);
}

bool GameLayer::init()
{
	bool bRet = false;
    do 
    {
		CC_BREAK_IF(!CCLayer::init());

		//CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();

        ImgMgr::getSingleton().removeRes("loading");
        ImgMgr::getSingleton().removeRes("game_title");
        
		LogicInfo::getSingleton().setClearTouchFunc(this, callfunc_selector(GameLayer::clearTouch));

		PlayerScene::getSingleton().init(CCDirector::sharedDirector()->getWinSizeInPixels().width, CCDirector::sharedDirector()->getWinSizeInPixels().height);
//		m_scenePlayer.init(1024, 768);

		addChild(PlayerScene::getSingleton().m_pRoot, 0);

        addChild(&GuideMgr::getSingleton(), -1);
        
        addChild(TouchLayer::create());
        
		setTouchEnabled(true);

		schedule(schedule_selector(GameLayer::onIdle));
                        
        m_timesOut = PLAY_TIMES_OUT * 1000;
        
		bRet = true;
	}while(0);

	return bRet;
}

void GameLayer::onIdle(float fDelta)
{
	HttpClient::getSingleton().onIdle();
    
    if (!LogicInfo::getSingleton().isAttackMode() && LogicInfo::getSingleton().m_pMsg != MSG_TIMES_OUT) {
        m_timesOut -= (fDelta*1000);
        if (m_timesOut <= 0) { //time's out
            MessageUI::show(MSG_TIMES_OUT, ViewString::getValue("times_out_title").c_str(), ViewString::getValue("times_out_message").c_str());
            
            m_timesOut = PLAY_TIMES_OUT * 1000;
        }
    }

	if(LogicInfo::getSingleton().m_pauseTime > 0)
	{
		PlayerScene::getSingleton().onIdle(fDelta * 1000 + LogicInfo::getSingleton().m_pauseTime * 1000);

		LogicInfo::getSingleton().m_pauseTime = 0;
	}
	else
		PlayerScene::getSingleton().onIdle(fDelta * 1000);

	//m_lastTime = curTime;

	{
		time_t ct = getCurrentTime();// time(NULL);
		if(ct - LogicInfo::getSingleton().m_curTime[2] >= 60*3)
		{
			LogicInfo::getSingleton().m_curTime[2] = ct;

			HttpClient::getSingleton().keeplive(true);
		}
	}
    
    ClanMgr::getSingleton().onIdle(fDelta * 1000);
}

void GameLayer::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
    if (CCControlButton::hasCtrlSelected()) return;
    
    if(m_waitNextCtrl)
        return;
    
    if (PlayerScene::getSingleton().m_pGameWorld == NULL) {
        return;
    }
    
    for (CCSetIterator iter = pTouches->begin(); iter != pTouches->end(); ++iter)
    {
        CCTouch* pTouch = (CCTouch*)(*iter);
        
        if(m_lstTouch.find(pTouch->getID()) == m_lstTouch.end())
        {
            _TouchNode n;
            n.id = pTouch->getID();
            n.x = pTouch->getLocation().x;
            n.y = pTouch->getLocation().y;
            n.bx = n.x;
            n.by = n.y;
            
            m_lstTouch.insert(std::pair<int, _TouchNode>(n.id, n));
        }
    }
    
    PlayerScene::getSingleton().m_isTouched = true;
    if (m_lstTouch.size() >= 2) {
        PlayerScene::getSingleton().m_isCanDropSoldier = false;
    }
    
    if(m_lstTouch.size() == 1)
    {
        PlayerScene::getSingleton().onTouchBegan(m_lstTouch.begin()->second.x, m_lstTouch.begin()->second.y);
    }
    else if(m_lstTouch.size() == 2)
    {
        int i = 0;
        CCPoint p[2];
        for(std::map<int, _TouchNode>::const_iterator it = m_lstTouch.begin(); it != m_lstTouch.end(); ++it)
        {
            p[i].x = it->second.x;
            p[i].y = it->second.y;
            ++i;
        }
        
        m_fBeginScale = PlayerScene::getSingleton().getScale();
        
        m_fMulBeginScale = sqrt((p[0].x - p[1].x) * (p[0].x - p[1].x) + (p[0].y - p[1].y) * (p[0].y - p[1].y));
		m_lastScaleDistance = m_fMulBeginScale;
        
		PlayerScene::getSingleton().chgScaleMode(true);
    }
	else if(m_lstTouch.size() > 2)
	{
		PlayerScene::getSingleton().chgScaleMode(true);
	}
}

void GameLayer::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
    if (CCControlButton::hasCtrlSelected()) return;
    
    if(m_waitNextCtrl)
        return;
    
    for (CCSetIterator iter = pTouches->begin(); iter != pTouches->end(); ++iter)
    {
        CCTouch* pTouch = (CCTouch*)(*iter);
        std::map<int, _TouchNode>::iterator it = m_lstTouch.find(pTouch->getID());
        if(it != m_lstTouch.end())
        {
            it->second.x = pTouch->getLocation().x;
            it->second.y = pTouch->getLocation().y;
        }
    }
    
    if(m_lstTouch.size() == 1)
    {
        CCPoint pt;
        pt.x = (m_lstTouch.begin()->second.x - m_lstTouch.begin()->second.bx);
        pt.y = (m_lstTouch.begin()->second.y - m_lstTouch.begin()->second.by);
        
        PlayerScene::getSingleton().onTouchMoved(pt.x, pt.y);
        
        if (!PlayerScene::getSingleton().m_isHoldDropSoldier && (fabs(pt.x) > 10 || fabs(pt.y) > 10)) {
            PlayerScene::getSingleton().m_isCanHoldDropSoldier = false;
        }
        
    }
    else if(m_lstTouch.size() == 2)
    {
        if (LogicInfo::getSingleton().isAttackMode() && PlayerScene::getSingleton().m_isHoldDropSoldier)
        {
            return;
        }
        
        int i = 0;
        CCPoint p[2];
        for(std::map<int, _TouchNode>::const_iterator it = m_lstTouch.begin(); it != m_lstTouch.end(); ++it)
        {
            p[i].x = it->second.x;
            p[i].y = it->second.y;
            i++;
        }
        
        float cd = sqrt((p[0].x - p[1].x) * (p[0].x - p[1].x) + (p[0].y - p[1].y) * (p[0].y - p[1].y));
        
		m_lastScaleDistance = cd;
        
		CCPoint pt;
        
		pt.x = MIN(p[0].x, p[1].x) + fabs(p[0].x - p[1].x) / 2;
		pt.y = MIN(p[0].y, p[1].y) + fabs(p[0].y - p[1].y) / 2;
        
        PlayerScene::getSingleton().setScale(cd / m_fMulBeginScale * m_fBeginScale, pt);
        
        if (!PlayerScene::getSingleton().m_isHoldDropSoldier && fabs(m_lastScaleDistance - m_fMulBeginScale) >= 10) {
            PlayerScene::getSingleton().m_isCanHoldDropSoldier = false;
        }
    }
}

void GameLayer::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
    if (CCControlButton::hasCtrlSelected()) return;
    
    if(m_waitNextCtrl)
        return;
    
    _TouchNode curNode;
    CCSetIterator iter = pTouches->begin();
    std::map<int, _TouchNode>::iterator it = m_lstTouch.find(((CCTouch*)(*iter))->getID());
    curNode = it->second;
    
    PlayerScene::getSingleton().m_isCanDropSoldier = false;
    
    if (PlayerScene::getSingleton().m_isCanHoldDropSoldier) {
        PlayerScene::getSingleton().m_isCanDropSoldier = true;
    }
    
    if (m_lstTouch.size() >= 2) {
        if (!PlayerScene::getSingleton().m_isHoldDropSoldier) {
            PlayerScene::getSingleton().m_isCanDropSoldier = false;
            PlayerScene::getSingleton().m_isCanHoldDropSoldier = false;
        }
    }
    else if (m_lstTouch.size() == 1) {
        
        PlayerScene::getSingleton().onTouchEnded((curNode.x - curNode.bx), (curNode.y - curNode.by));
    }
    
    PlayerScene::getSingleton().m_isTouched = false;
        
    iter = pTouches->begin();
    for (; iter != pTouches->end(); ++iter)
    {
        CCTouch* pTouch = (CCTouch*)(*iter);
        std::map<int, _TouchNode>::iterator it = m_lstTouch.find(pTouch->getID());
        if(it != m_lstTouch.end())
        {
            m_lstTouch.erase(it);
        }
    }
    
    if (m_lstTouch.size() == 0) {
        PlayerScene::getSingleton().chgScaleMode(false);
        PlayerScene::getSingleton().m_pRoot->m_bProcScaleAni = true;
        
        PlayerScene::getSingleton().m_isCanHoldDropSoldier = true;
    }
    else if (m_lstTouch.size() == 1) {
        
        std::map<int, _TouchNode>::iterator it = m_lstTouch.begin();
        it->second.bx = it->second.x;
        it->second.by = it->second.y;
        
        PlayerScene::getSingleton().onTouchBegan(m_lstTouch.begin()->second.x, m_lstTouch.begin()->second.y);
    }
    else if (m_lstTouch.size() == 2) {
        CCPoint p[2];
        int i = 0;
        for(std::map<int, _TouchNode>::const_iterator it = m_lstTouch.begin(); it != m_lstTouch.end(); ++it)
        {
            p[i].x = it->second.x;
            p[i].y = it->second.y;
            i++;
        }
        
        m_fBeginScale = PlayerScene::getSingleton().getScale();
        
        m_fMulBeginScale = sqrt((p[0].x - p[1].x) * (p[0].x - p[1].x) + (p[0].y - p[1].y) * (p[0].y - p[1].y));
		m_lastScaleDistance = m_fMulBeginScale;
		PlayerScene::getSingleton().chgScaleMode(true);
    }
    
}

void GameLayer::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{
    if (CCControlButton::hasCtrlSelected()) return;
    
    if(m_waitNextCtrl)
        return;
    
    _TouchNode curNode;
    CCSetIterator iter = pTouches->begin();
    std::map<int, _TouchNode>::iterator it = m_lstTouch.find(((CCTouch*)(*iter))->getID());
    curNode = it->second;
    
    PlayerScene::getSingleton().m_isCanDropSoldier = false;
    
    if (PlayerScene::getSingleton().m_isCanHoldDropSoldier) {
        PlayerScene::getSingleton().m_isCanDropSoldier = true;
    }
    
    if (m_lstTouch.size() >= 2) {
        if (!PlayerScene::getSingleton().m_isHoldDropSoldier) {
            PlayerScene::getSingleton().m_isCanDropSoldier = false;
            PlayerScene::getSingleton().m_isCanHoldDropSoldier = false;
        }
    }
    else if (m_lstTouch.size() == 1) {
        
        PlayerScene::getSingleton().onTouchEnded((curNode.x - curNode.bx), (curNode.y - curNode.by));
    }
    
    PlayerScene::getSingleton().m_isTouched = false;
    
    iter = pTouches->begin();
    for (; iter != pTouches->end(); ++iter)
    {
        CCTouch* pTouch = (CCTouch*)(*iter);
        std::map<int, _TouchNode>::iterator it = m_lstTouch.find(pTouch->getID());
        if(it != m_lstTouch.end())
        {
            m_lstTouch.erase(it);
        }
    }
    
    if (m_lstTouch.size() == 0) {
        PlayerScene::getSingleton().chgScaleMode(false);
        PlayerScene::getSingleton().m_pRoot->m_bProcScaleAni = true;
        
        PlayerScene::getSingleton().m_isCanHoldDropSoldier = true;
    }
    else if (m_lstTouch.size() == 1) {
        
        std::map<int, _TouchNode>::iterator it = m_lstTouch.begin();
        it->second.bx = it->second.x;
        it->second.by = it->second.y;
        
        PlayerScene::getSingleton().onTouchBegan(m_lstTouch.begin()->second.x, m_lstTouch.begin()->second.y);
    }
    else if (m_lstTouch.size() == 2) {
        CCPoint p[2];
        int i = 0;
        for(std::map<int, _TouchNode>::const_iterator it = m_lstTouch.begin(); it != m_lstTouch.end(); ++it)
        {
            p[i].x = it->second.x;
            p[i].y = it->second.y;
            i++;
        }
        
        m_fBeginScale = PlayerScene::getSingleton().getScale();
        
        m_fMulBeginScale = sqrt((p[0].x - p[1].x) * (p[0].x - p[1].x) + (p[0].y - p[1].y) * (p[0].y - p[1].y));
		m_lastScaleDistance = m_fMulBeginScale;
		PlayerScene::getSingleton().chgScaleMode(true);
    }
}

void GameLayer::onChgScene()
{
	m_waitNextCtrl = true;
    LogicInfo::getSingleton().onChangeScene();
}

void GameLayer::clearTouch()
{
	m_lstTouch.clear();
	m_waitNextCtrl = false;
}

TouchLayer::TouchLayer()
{
    
}

TouchLayer::~TouchLayer()
{
    
}

bool TouchLayer::init()
{
    this->setTouchEnabled(true);
    
    return true;
}

void TouchLayer::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -9999999, false);
}

bool TouchLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    m_timesOut = PLAY_TIMES_OUT * 1000;
    
    return true;
}

void TouchLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

void TouchLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
    
}

