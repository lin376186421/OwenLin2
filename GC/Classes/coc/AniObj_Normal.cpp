/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "AniObj_Normal.h"
#include "ImgMgr.h"

NS_COC_BEGIN

AniObj_Normal::AniObj_Normal()
	: m_pAnimate(NULL)
	, m_pAction(NULL)
	, m_pSpr(NULL)
{
}

AniObj_Normal::~AniObj_Normal()
{
}

AniObj_Normal* AniObj_Normal::create(const char* str, int frames, float frameTime, float scale, bool isLoop, CCActionManager* amgr)
{
	AniObj_Normal *pRet = new AniObj_Normal();
	if (pRet && pRet->init(str, frames, frameTime, scale, isLoop, amgr))
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

bool AniObj_Normal::init(const char* str, int frames, float frameTime, float scale, bool isLoop, CCActionManager* amgr)
{
	bool bRet = false;
    do 
    {
		AniObj::init();

		m_pSpr = CCSprite::create();
		m_pSpr->retain();
        
		if(amgr != NULL)
			m_pSpr->setActionManager(amgr);

		CCAnimation* pAni = CCAnimation::create();

		for(int i = 0; i < frames; ++i)
		{
            char strname[32];
            sprintf(strname, "%s%02d", str, i + 1);
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(strname);
            CCAssert(pFrame != NULL, "AniObj no frame");
            pAni->addSpriteFrame(pFrame);

            if(i == 0) {
                m_pSpr->setDisplayFrame(pFrame);
            }
		}

		pAni->setDelayPerUnit(frameTime);
		m_pAnimate = CCAnimate::create(pAni);
		m_pAnimate->retain();

		//addChild(m_pSpr);

		m_pSpr->setScale(scale);
		m_pSpr->setVisible(false);

		m_bLoop = isLoop;

		bRet = true;
	}while(0);

	return bRet;
}

void AniObj_Normal::start()
{
	if(m_pAction != NULL)
		return ;

	m_pSpr->setVisible(true);

	if(m_bLoop)
	{
		CCRepeatForever* pRF = CCRepeatForever::create(m_pAnimate);
		m_pAction = m_pSpr->runAction(pRF);
		m_pAction->retain();
	}
	else
	{
		m_pAction = m_pSpr->runAction(m_pAnimate);
		m_pAction->retain();
	}
}

void AniObj_Normal::onIdle(float ot)
{
	AniObj::onIdle(ot);

	if(m_pAction != NULL && m_pAction->isDone())
	{
		m_pSpr->setVisible(false);

		m_pAction->release();

		m_pAction = NULL;
	}
}

bool AniObj_Normal::isFinish() 
{ 
	if(m_bLoop)
		return isLoopEnd();
	else
		return m_pAction == NULL || m_pAction->isDone(); 
}

void AniObj_Normal::releaseImp()
{
	CCNode* pParent = m_pSpr->getParent();
	if(pParent != NULL)
	{
		pParent->removeChild(m_pSpr, true);

		m_pSpr->release();
	}

	if(m_pAction != NULL)
	{
		m_pAction->release();
		m_pAction = NULL;
	}
}

void AniObj_Normal::setPosition(const CCPoint& pt)
{
	m_pSpr->setPosition(pt);
}

void AniObj_Normal::setParent(CCNode* pNode, int zorder)
{
	AniObj::setParent(pNode, zorder);

	pNode->addChild(m_pSpr, zorder);
}

void AniObj_Normal::setScale(float scale)
{
	m_pSpr->setScale(scale);
}

void AniObj_Normal::setScaleX(float scale)
{
	m_pSpr->setScaleX(scale);
}

void AniObj_Normal::setScaleY(float scale)
{
	m_pSpr->setScaleY(scale);
}

void AniObj_Normal::setAnchorPoint(const CCPoint& anchor)
{
	m_pSpr->setAnchorPoint(anchor);
}

NS_COC_END
