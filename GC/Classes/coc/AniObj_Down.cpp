/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "AniObj_Down.h"
#include "ImgMgr.h"

NS_COC_BEGIN

AniObj_Down::AniObj_Down()
	: m_pAnimate(NULL)
	, m_pAction1(NULL)
	, m_pAction2(NULL)
	, m_pSpr(NULL)
	, m_bFinish(false)
{
}

AniObj_Down::~AniObj_Down()
{
}

AniObj_Down* AniObj_Down::create(const char* str, int frames, float frameTime, int height, float time, int rotate, CCActionManager* amgr)
{
	AniObj_Down *pRet = new AniObj_Down();
	if (pRet && pRet->init(str, frames, frameTime, height, time, rotate, amgr))
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

bool AniObj_Down::init(const char* str, int frames, float frameTime, int height, float time, int rotate, CCActionManager* amgr)
{
	bool bRet = false;
    do 
    {
		AniObj::init();

		m_pSpr = CCSprite::create();
		m_pSpr->retain();

		if(amgr != NULL)
			m_pSpr->setActionManager(amgr);

		CCAnimation* pAni1 = CCAnimation::create();

		for(int i = 0; i < frames; ++i)
		{
			{
				char strname[32];
				sprintf(strname, "%s%03d", str, i + 1);
				CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(strname);
				CCAssert(pFrame != NULL, "AniObj no frame");
				pAni1->addSpriteFrame(pFrame);

				if(i == 0)
				{
					m_pSpr->setDisplayFrame(pFrame);
				}

			}
		}

		pAni1->setDelayPerUnit(frameTime);
		m_pAnimate = CCAnimate::create(pAni1);
		m_pAnimate->retain();

		m_pSpr->setVisible(false);

		m_iRotate = rotate;
		m_iHeight = height;
		m_fAllTime = time;

		bRet = true;
	}while(0);

	return bRet;
}

void AniObj_Down::start()
{
	if(m_pAction1 != NULL)
		return ;

	CCPoint pt = m_pSpr->getPosition();

	pt.x -= m_iHeight * tan(m_iRotate * 3.14159265f / 180.0f);
	pt.y += m_iHeight;
	m_pSpr->setPosition(pt);

	m_pSpr->setRotation(360 - m_iRotate);

	CCRepeatForever* pRF = CCRepeatForever::create(m_pAnimate);
	m_pAction1 = m_pSpr->runAction(pRF);
	m_pAction1->retain();

	pt.x += m_iHeight * tan(m_iRotate * 3.14159265f / 180.0f);
	pt.y -= m_iHeight;

	CCMoveTo* mv1 = CCMoveTo::create(m_fAllTime, pt);
	CCEaseSineIn* sinein = CCEaseSineIn::create(mv1);

	m_pAction2 = m_pSpr->runAction(sinein);
	m_pAction2->retain();

	m_pSpr->setVisible(true);
}

void AniObj_Down::onIdle(float ot)
{
	AniObj::onIdle(ot);

	if(m_pAction2 != NULL && m_pAction2->isDone())
	{
		m_pSpr->stopAllActions();

		m_pAction1->release();
		m_pAction1 = NULL;
		m_pAction2->release();
		m_pAction2 = NULL;

		m_pSpr->setRotation(0);
	}
}

bool AniObj_Down::isFinish() 
{ 
	return m_bFinish; 
}

void AniObj_Down::releaseImp()
{
	CCNode* pParent = m_pSpr->getParent();
	if(pParent != NULL)
	{
		pParent->removeChild(m_pSpr, true);

		m_pSpr->release();
	}

	if(m_pAnimate != NULL)
	{
		m_pAnimate->release();
		m_pAnimate = NULL;
	}

	if(m_pAction1 != NULL)
	{
		m_pAction1->release();
		m_pAction1 = NULL;
	}

	if(m_pAction2 != NULL)
	{
		m_pAction2->release();
		m_pAction2 = NULL;
	}
}

void AniObj_Down::setPosition(const CCPoint& pt)
{
	m_pSpr->setPosition(pt);
}

void AniObj_Down::setParent(CCNode* pNode, int zorder)
{
	AniObj::setParent(pNode, zorder);

	pNode->addChild(m_pSpr, zorder);
}

void AniObj_Down::setScale(float scale)
{
	m_pSpr->setScale(scale);
}

void AniObj_Down::setAnchorPoint(const CCPoint& anchor)
{
	m_pSpr->setAnchorPoint(anchor);
}

NS_COC_END
