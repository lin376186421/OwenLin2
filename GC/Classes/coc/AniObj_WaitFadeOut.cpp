/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "AniObj_WaitFadeOut.h"
#include "ImgMgr.h"

NS_COC_BEGIN

AniObj_WaitFadeOut::AniObj_WaitFadeOut()
	: m_pSpr(NULL)
	, m_curTime(-1)
{
}

AniObj_WaitFadeOut::~AniObj_WaitFadeOut()
{
}

AniObj_WaitFadeOut* AniObj_WaitFadeOut::create(const char* str, int time1, int time2, int time3, CCActionManager* amgr)
{
	AniObj_WaitFadeOut *pRet = new AniObj_WaitFadeOut();
	if (pRet && pRet->init(str, time1, time2, time3, amgr))
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

bool AniObj_WaitFadeOut::init(const char* str, int time1, int time2, int time3, CCActionManager* amgr)
{
	bool bRet = false;
    do 
    {
		AniObj::init();

		CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(str);
		m_pSpr = CCSprite::create();
		m_pSpr->setDisplayFrame(pFrame);
		m_pSpr->retain();

		if(amgr != NULL)
			m_pSpr->setActionManager(amgr);

		m_time[0] = time1;
		m_time[1] = time2;
		m_time[2] = time3;

		//CCAnimation* pAni = CCAnimation::create();

		//for(int i = 0; i < frames; ++i)
		//{
		//	{
		//		char strname[32];
		//		sprintf(strname, "%s%03d", str, i + 1);
		//		CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(strname);
		//		CCAssert(pFrame != NULL, "AniObj no frame");
		//		pAni->addSpriteFrame(pFrame);

		//		if(i == 0)
		//		{
		//			m_pSpr->setDisplayFrame(pFrame);
		//		}

		//	}
		//}

		//pAni->setDelayPerUnit(0.12f);
		//m_pAnimate = CCAnimate::create(pAni);
		//m_pAnimate->retain();

		//addChild(m_pSpr);

		m_pSpr->setVisible(false);

		bRet = true;
	}while(0);

	return bRet;
}

void AniObj_WaitFadeOut::start()
{
	if(m_curTime >= 0)
		return ;

	m_curTime = 0;
	//if(m_pAction != NULL)
	//	return ;

	m_pSpr->setVisible(false);
	//m_pAction = m_pSpr->runAction(m_pAnimate);
	//m_pAction->retain();
}

void AniObj_WaitFadeOut::onIdle(float ot)
{
	AniObj::onIdle(ot);

	if(m_curTime < 0)
		return ;

	int oldt = m_curTime;

	m_curTime += ot * 1000;

	if(oldt < m_time[0] && m_curTime >= m_time[0])
	{
		m_pSpr->setVisible(true);
		m_pSpr->setOpacity(255);
	}
	else if(oldt < m_time[0] + m_time[1] && m_curTime >= m_time[0] + m_time[1])
	{
		m_pSpr->setOpacity(255);
	}
	else if(m_curTime >= m_time[0] + m_time[1] && m_curTime <= m_time[0] + m_time[1] + m_time[2])
	{
		float ott = m_curTime - (m_time[0] + m_time[1]);

		ott /= m_time[2];

		m_pSpr->setOpacity(255 * (1 - ott));
	}
}

bool AniObj_WaitFadeOut::isFinish() 
{ 
	return m_curTime < 0 || m_curTime > m_time[0] + m_time[1] + m_time[2]; 
}

void AniObj_WaitFadeOut::releaseImp()
{
	CCNode* pParent = m_pSpr->getParent();
	if(pParent != NULL)
	{
		pParent->removeChild(m_pSpr, true);

		m_pSpr->release();
	}
}

void AniObj_WaitFadeOut::setPosition(const CCPoint& pt)
{
	m_pSpr->setPosition(pt);
}

void AniObj_WaitFadeOut::setParent(CCNode* pNode, int zorder)
{
	AniObj::setParent(pNode, zorder);

	pNode->addChild(m_pSpr, zorder);
}

void AniObj_WaitFadeOut::setScale(float scale)
{
	m_pSpr->setScale(scale);
}

NS_COC_END
