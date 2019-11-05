/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "AniObj_Move.h"
#include "ImgMgr.h"

NS_COC_BEGIN

AniObj_Move::AniObj_Move()
	: m_pAnimate(NULL)
	, m_pAction1(NULL)
	, m_pAction2(NULL)
	, m_pSpr(NULL)
	, m_bSineIn(false)
{
}

AniObj_Move::~AniObj_Move()
{
}

AniObj_Move* AniObj_Move::create(const char* str, int frames, float frameTime, int bx, int by, int ex, int ey, float time, bool sinein, CCActionManager* amgr)
{
	AniObj_Move *pRet = new AniObj_Move();
	if (pRet && pRet->init(str, frames, frameTime, bx, by, ex, ey, time, sinein, amgr))
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

bool AniObj_Move::init(const char* str, int frames, float frameTime, int bx, int by, int ex, int ey, float time, bool sinein, CCActionManager* amgr)
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
			{
				char strname[32];
				sprintf(strname, "%s%03d", str, i + 1);
				CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(strname);
				CCAssert(pFrame != NULL, "AniObj no frame");
				pAni->addSpriteFrame(pFrame);

				if(i == 0)
				{
					m_pSpr->setDisplayFrame(pFrame);
				}

			}
		}

		pAni->setDelayPerUnit(frameTime);
		m_pAnimate = CCAnimate::create(pAni);
		m_pAnimate->retain();

		m_iBX = bx;
		m_iBY = by;
		m_iEX = ex;
		m_iEY = ey;
		m_fTime = time;
		m_bSineIn = sinein;

		m_pSpr->setVisible(false);

		bRet = true;
	}while(0);

	return bRet;
}

void AniObj_Move::start()
{
	if(m_pAction1 != NULL)
		return ;

	m_pSpr->setPosition(ccp(m_iBX, m_iBY));

	//! Ñ­»·
	CCRepeatForever* pRF = CCRepeatForever::create(m_pAnimate);
	m_pAction1 = m_pSpr->runAction(pRF);
	m_pAction1->retain();

	CCMoveTo* mv1 = CCMoveTo::create(m_fTime, ccp(m_iEX, m_iEY));

	if(m_bSineIn)
	{
		CCEaseSineIn* sinein = CCEaseSineIn::create(mv1);
		m_pAction2 = m_pSpr->runAction(sinein);
	}
	else
		m_pAction2 = m_pSpr->runAction(mv1);

	
	m_pAction2->retain();

	m_pSpr->setVisible(true);
}

void AniObj_Move::onIdle(float ot)
{
	AniObj::onIdle(ot);

	if(m_pAction2 != NULL && m_pAction2->isDone())
	{
		m_pSpr->setVisible(false);
		m_pSpr->stopAllActions();

		m_pAction1->release();
		m_pAction1 = NULL;
		m_pAction2->release();
		m_pAction2 = NULL;
	}
}

bool AniObj_Move::isFinish() 
{ 
	return m_pAction2 == NULL || m_pAction2->isDone(); 
}

void AniObj_Move::releaseImp()
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

void AniObj_Move::setPosition(const CCPoint& pt)
{
	m_pSpr->setPosition(pt);
}

void AniObj_Move::setParent(CCNode* pNode, int zorder)
{
	AniObj::setParent(pNode, zorder);

	pNode->addChild(m_pSpr, zorder);
}

void AniObj_Move::setScale(float scale)
{
	m_pSpr->setScale(scale);
}

void AniObj_Move::setAnchorPoint(const CCPoint& anchor)
{
	m_pSpr->setAnchorPoint(anchor);
}

NS_COC_END
