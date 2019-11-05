/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "AniObj_Flash.h"
#include "ImgMgr.h"

NS_COC_BEGIN

AniObj_Flash::AniObj_Flash()
	: m_pAnimate(NULL)
	, m_pAction(NULL)
	, m_pSpr(NULL)
{
}

AniObj_Flash::~AniObj_Flash()
{
}

AniObj_Flash* AniObj_Flash::create(const char* str, int frames, float frameTime, float scale, bool isLoop, float flashtime, CCActionManager* amgr)
{
	AniObj_Flash *pRet = new AniObj_Flash();
	if (pRet && pRet->init(str, frames, frameTime, scale, isLoop, flashtime, amgr))
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

bool AniObj_Flash::init(const char* str, int frames, float frameTime, float scale, bool isLoop, float flashtime, CCActionManager* amgr)
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

            if(i == 0)
            {
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

		m_fFlashTime = flashtime;

		bRet = true;
	}while(0);

	return bRet;
}

void AniObj_Flash::start()
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

void AniObj_Flash::onIdle(float ot)
{
	AniObj::onIdle(ot);

	int tag = m_timeBegin / m_fFlashTime;

	int op = 0;

	if(tag % 2 == 0)
	{
		//! Ë¥¼õ
		float tmp = (m_fFlashTime - (m_timeBegin - m_fFlashTime * tag)) / m_fFlashTime;
		op = 255 * tmp;
	}
	else
	{
		//! ÔöÇ¿
		float tmp = (m_timeBegin - m_fFlashTime * tag) / m_fFlashTime;
		op = 255 * tmp;
	}

	m_pSpr->setOpacity(op);

	if(m_pAction != NULL && m_pAction->isDone())
	{
		m_pSpr->setVisible(false);

		m_pAction->release();

		m_pAction = NULL;
	}
}

bool AniObj_Flash::isFinish() 
{ 
	if(m_bLoop)
		return isLoopEnd();
	else
		return m_pAction == NULL || m_pAction->isDone(); 
}

void AniObj_Flash::releaseImp()
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

void AniObj_Flash::setPosition(const CCPoint& pt)
{
	m_pSpr->setPosition(pt);
}

void AniObj_Flash::setParent(CCNode* pNode, int zorder)
{
	AniObj::setParent(pNode, zorder);

	pNode->addChild(m_pSpr, zorder);
}

void AniObj_Flash::setScale(float scale)
{
	m_pSpr->setScale(scale);
}

void AniObj_Flash::setAnchorPoint(const CCPoint& anchor)
{
	m_pSpr->setAnchorPoint(anchor);
}

NS_COC_END
