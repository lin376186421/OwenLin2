/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "WorldSceneNode.h"
#include "PlayerScene.h"

NS_COC_BEGIN

WorldSceneNode::WorldSceneNode()
	: m_maxScale(1.0f)
	, m_maxScaleEx(1.3f)
	, m_timeTotalScaleAni(0.5f)
    , m_bProcScaleAni(false)
{
}

WorldSceneNode::~WorldSceneNode()
{
}

WorldSceneNode* WorldSceneNode::create(float w, float h)
{
	WorldSceneNode *pRet = new WorldSceneNode();
	if (pRet && pRet->init(w, h))
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

bool WorldSceneNode::init(float w, float h)
{
	bool bRet = false;
    do 
    {
		CCNode::init();
        
        setAnchorPoint(ccp(0.0f, 0.0f));

		m_size.width = w;
		m_size.height = h;

		bRet = true;
	}while(0);

	return bRet;
}

void WorldSceneNode::chgPos(float x, float y)
{
	fixPos(x, y);
	setPosition(ccp(x, y));
}

void WorldSceneNode::chgCenterPos(float x, float y)
{
    x /= getScale();
    y /= getScale();
	float sw = CCDirector::sharedDirector()->getWinSize().width / getScale();
	float sh = CCDirector::sharedDirector()->getWinSize().height / getScale();

	chgPos(-(x - sw / 2) * getScale(), -(y - sh / 2) * getScale());
}

void WorldSceneNode::chgScale(float fScale)
{
    CCPoint ptCenter = getCenterPos();
    
    ptCenter.x /= getScale();
    ptCenter.y /= getScale();
    
	fixScale(fScale);
    
	setScale(fScale);
    
	chgCenterPos(ptCenter.x * getScale(), ptCenter.y * getScale());
}


void WorldSceneNode::chgScaleEx(float fScale, CCPoint& ptCenter, CCPoint& ptOff)
{
    ptCenter.x /= getScale();
    ptCenter.y /= getScale();
    
	fixScale(fScale);

	setScale(fScale);

	chgCenterPosEx(ptCenter.x * getScale(), ptCenter.y * getScale(), ptOff);
}

void WorldSceneNode::fixPos(float& x, float& y)
{
	float tx = x / getScale();
    float ty = y / getScale();

	float sw = CCDirector::sharedDirector()->getWinSize().width / getScale();
	float sh = CCDirector::sharedDirector()->getWinSize().height / getScale();

	if(tx > 0)
		tx = 0;
	if(tx < -(m_size.width - sw))
        tx = -(m_size.width - sw);
    
	if(ty > 0)
        ty = 0;
    if(ty < -(m_size.height - sh))
        ty = -(m_size.height - sh);
    
    x = tx * getScale();
    y = ty * getScale();
}

void WorldSceneNode::fixScale(float& fScale)
{
	if(fScale > m_maxScaleEx)
	{
		fScale = m_maxScaleEx;

		return;
	}

	float sw = CCDirector::sharedDirector()->getWinSize().width / m_size.width;
	float sh = CCDirector::sharedDirector()->getWinSize().height / m_size.height;

	float ms = MAX(sw, sh);

	if(fScale < ms)
		fScale = ms;
}

bool WorldSceneNode::onProcScaleAni(float ot)
{
    if(!m_bProcScaleAni)
        return false;
    
    if (PlayerScene::getSingleton().m_isTouched) {
        return false;
    }
    
	float cs = getScale();

	if(cs > m_maxScale)
	{
		cs -= fabs(m_maxScaleEx - m_maxScale) * ot / m_timeTotalScaleAni;

		if(cs < m_maxScale)
			cs = m_maxScale;

		chgScale(cs);

		return true;
	}

    m_bProcScaleAni = false;
	return false;
}

CCPoint WorldSceneNode::getCenterPos()
{
	float sw = CCDirector::sharedDirector()->getWinSize().width;
	float sh = CCDirector::sharedDirector()->getWinSize().height;

	CCPoint pt(-getPositionX() + sw / 2, -getPositionY() + sh / 2);

	return pt;
}

void WorldSceneNode::chgCenterPosEx(float x, float y, const CCPoint& ptOff)
{
    m_ptScreenCenter = ptOff;
    
    x /= getScale();
    y /= getScale();
	float sw = m_ptScreenCenter.x / getScale();
	float sh = m_ptScreenCenter.y / getScale();
    
	chgPos(-(x - sw) * getScale(), -(y - sh) * getScale());

}

CCPoint WorldSceneNode::getCenterPosEx(const CCPoint& ptOff)
{
    m_ptScreenCenter = ptOff;
    
	CCPoint pt(-getPositionX() + m_ptScreenCenter.x, -getPositionY() + m_ptScreenCenter.y);
    
	return pt;
}

NS_COC_END
