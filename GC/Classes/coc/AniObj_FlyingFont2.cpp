/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "AniObj_FlyingFont2.h"
#include "ImgMgr.h"

NS_COC_BEGIN

AniObj_FlyingFont2::AniObj_FlyingFont2()
	: m_pFont(NULL)
	, m_pAction(NULL)
	, m_width(0)
{
}

AniObj_FlyingFont2::~AniObj_FlyingFont2()
{
}

AniObj_FlyingFont2* AniObj_FlyingFont2::create(int time, int yoff, CCActionManager* amgr)
{
	AniObj_FlyingFont2 *pRet = new AniObj_FlyingFont2();
	if (pRet && pRet->init(time, yoff, amgr))
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

bool AniObj_FlyingFont2::init(int time, int yoff, CCActionManager* amgr)
{
	bool bRet = false;
    do 
    {
		AniObj::init();

		m_size = 30;
		m_width = 300;

		m_pFont = CCLabelBMFont::create("", "Font/tbFont_b.fnt", 300, kCCTextAlignmentCenter);
        m_pFont->setScale(0.5);
		m_pFont->retain();

		//m_pFont->setScale((float)size / 30);

		if(amgr != NULL)
			m_pFont->setActionManager(amgr);

		m_ctime = time;
		m_yoff = yoff;

		m_pFont->setAnchorPoint(ccp(0.5f, 0.5f));
		
		m_pFont->setVisible(false);

		bRet = true;
	}while(0);

	return bRet;
}

void AniObj_FlyingFont2::start()
{
	if(m_pAction != NULL)
		return ;

	CCPoint pt = m_pFont->getPosition();

	m_pFont->runAction(CCFadeOut::create(m_ctime / 1000.0f));

	pt.y += m_yoff;

	CCMoveTo* mv1 = CCMoveTo::create(m_ctime / 1000.0f, pt);

	m_pAction = m_pFont->runAction(mv1);
	m_pAction->retain();

	m_pFont->setVisible(true);
}

void AniObj_FlyingFont2::onIdle(float ot)
{
	AniObj::onIdle(ot);

	if(m_pAction != NULL && m_pAction->isDone())
	{
		m_pFont->setVisible(false);

		m_pAction->release();

		m_pAction = NULL;
	}
}

bool AniObj_FlyingFont2::isFinish() 
{ 
	return m_pAction == NULL || m_pAction->isDone(); 
}

void AniObj_FlyingFont2::releaseImp()
{
	CCNode* pParent = m_pFont->getParent();
	if(pParent != NULL)
	{
		pParent->removeChild(m_pFont, true);

		m_pFont->release();
	}

	if(m_pAction != NULL)
	{
		m_pAction->release();
		m_pAction = NULL;
	}
}

void AniObj_FlyingFont2::setPosition(const CCPoint& pt)
{
	m_pFont->setPosition(pt);
}

void AniObj_FlyingFont2::setParent(CCNode* pNode, int zorder)
{
	AniObj::setParent(pNode, zorder);

	pNode->addChild(m_pFont, zorder);
	//pNode->addChild(m_pSpr, zorder);
}

void AniObj_FlyingFont2::setString(const char* str)
{
	m_pFont->setString(str);

	//m_pFont->setPositionX(0 - 300 / 2);
}

float AniObj_FlyingFont2::getWidth()
{
	return m_width;
}

NS_COC_END
