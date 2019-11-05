/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "AniObj_FlyingFont.h"
#include "ImgMgr.h"

NS_COC_BEGIN

AniObj_FlyingFont::AniObj_FlyingFont()
	: m_pFont(NULL)
	, m_pAction(NULL)
{
}

AniObj_FlyingFont::~AniObj_FlyingFont()
{
}

AniObj_FlyingFont* AniObj_FlyingFont::create(const char* str, int time, int yoff, CCActionManager* amgr)
{
	AniObj_FlyingFont *pRet = new AniObj_FlyingFont();
	if (pRet && pRet->init(str, time, yoff, amgr))
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

bool AniObj_FlyingFont::init(const char* str, int time, int yoff, CCActionManager* amgr)
{
	bool bRet = false;
    do 
    {
		AniObj::init();

		m_pFont = CCLabelBMFont::create(str, "Font/tbFont_b.fnt", -1, kCCTextAlignmentCenter);
        m_pFont->setScale(0.8);
		m_pFont->retain();

		if(amgr != NULL)
			m_pFont->setActionManager(amgr);

		m_ctime = time;
		m_yoff = yoff;

		m_pFont->setVisible(false);

		bRet = true;
	}while(0);

	return bRet;
}

void AniObj_FlyingFont::start()
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

void AniObj_FlyingFont::onIdle(float ot)
{
	AniObj::onIdle(ot);

	if(m_pAction != NULL && m_pAction->isDone())
	{
		m_pFont->setVisible(false);

		m_pAction->release();

		m_pAction = NULL;
	}
}

bool AniObj_FlyingFont::isFinish() 
{ 
	return m_pAction == NULL || m_pAction->isDone(); 
}

void AniObj_FlyingFont::releaseImp()
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

void AniObj_FlyingFont::setPosition(const CCPoint& pt)
{
	m_pFont->setPosition(pt);
}

void AniObj_FlyingFont::setParent(CCNode* pNode, int zorder)
{
	AniObj::setParent(pNode, zorder);

	pNode->addChild(m_pFont, zorder);
}

void AniObj_FlyingFont::setString(const char* str)
{
	m_pFont->setString(str);
}

void AniObj_FlyingFont::setColor(const ccColor3B &color)
{
    m_pFont->setColor(color);
}

float AniObj_FlyingFont::getWidth()
{
	return m_pFont->getContentSize().width;
}

NS_COC_END
