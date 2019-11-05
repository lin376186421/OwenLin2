#include "SceneBgr.h"
#include "PlayerScene.h"
#include "ImgMgr.h"
#include "CUINode.h"

NS_COC_BEGIN

SceneBgr::SceneBgr()
{
	m_pNodeBack = NULL;
}

SceneBgr::~SceneBgr()
{
	if(m_pNodeBack != NULL)
		m_pNodeBack->release();

	m_pNodeBack = NULL;
}

void SceneBgr::init(PlayerScene& scene)
{
	m_pNodeBack = CCNode::create();
	m_pNodeBack->retain();
    
    ImgMgr::getSingleton().addSpriteFrames("background_env");
    ImgMgr::getSingleton().loadImg("background");

    CCNode* pMapNode = CUINodeCache::sharedUINodeCache()->UINodeFromFile("background_player.ccbi", this, &m_pAnimationManager);
    m_pNodeBack->addChild(pMapNode);
    
	m_pNodeBack->setScale(2.0f);
	m_pNodeBack->setAnchorPoint(ccp(0, 0));
	m_pNodeBack->setPosition(ccp(0, 0));
    
	scene.m_pRoot->addChild(m_pNodeBack, 0);
}

void SceneBgr::ResumeBgAnimation()
{
    m_pAnimationManager->runAnimationsForSequenceNamed("background_player_am");
}

NS_COC_END