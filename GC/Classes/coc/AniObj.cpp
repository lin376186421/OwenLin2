/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "AniObj.h"
#include "ImgMgr.h"

NS_COC_BEGIN

AniObj::AniObj()
	: m_bAutoRelease(false)
	, m_pParent(NULL)
	, m_bLoop(false)
	, m_timeBegin(0.0f)
	, m_timeMax(-1.0f)
{
}

AniObj::~AniObj()
{
}

AniObj* AniObj::create()
{
	AniObj *pRet = new AniObj();
	if (pRet && pRet->init())
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

bool AniObj::init()
{
	bool bRet = false;
    do 
    {
		//CCObject::init();

		bRet = true;
	}while(0);

	return bRet;
}

void AniObj::start()
{
}

void AniObj::onIdle(float ot)
{
	m_timeBegin += ot;
}

bool AniObj::isLoopEnd()
{
	if(!m_bLoop)
		return false;

	if(m_timeMax <= 0)
		return false;

	return m_timeBegin >= m_timeMax;
}

NS_COC_END
