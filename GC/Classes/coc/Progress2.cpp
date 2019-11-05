#include "Progress2.h"
#include "ImgMgr.h"

NS_COC_BEGIN

Progress2::Progress2()
{
}

Progress2::~Progress2()
{
}

Progress2* Progress2::create(const char* strImage, const char* strBack, CCRect& rectInsets)
{
	Progress2 *pRet = new Progress2();
	if (pRet && pRet->init(strImage, strBack, rectInsets))
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
    
    if (true) {
        
    }
}

void Progress2::setProgress(float fProgress)
{
	m_fProgress = fProgress;

	if(m_fProgress > 1.0f)
		m_fProgress = 1.0f;
	else if(m_fProgress < 0.0f)
		m_fProgress = 0.0f;

	if(m_fProgress == 0.0f)
	{
		m_pImage->setVisible(false);
	}
	else
    {
		m_pImage->setVisible(true);
        
        float cur = m_maxWidth * fProgress;
        if(cur < m_minWidth)
            cur = m_minWidth;

		CCSize ws = CCSizeMake(cur, m_pImage->getContentSize().height);

		m_pImage->setContentSize(ws);

		//CCRect r = m_pImage[1]->getTextureRect();
		//CCSize s;
		//s.width = (m_maxWidth - m_minWidth) * progress;
		//s.height = m_pImage[1]->getContentSize().height;
		//r.size.width = (m_maxWidth - m_minWidth) * progress;
		//m_pImage[1]->setTextureRect(r, m_pImage[1]->isTextureRectRotated(), s);
		//m_pImage[1]->setContentSize(s);
		//m_pImage[1]->setScaleX((m_maxWidth - m_minWidth) * m_fProgress + 8);
  //      m_pImage[1]->setPosition(ccp(m_pImage[0]->getContentSize().width - 2, 0));
		//m_pImage[2]->setPosition(ccp(m_pImage[0]->getContentSize().width + m_pImage[1]->getContentSize().width * (m_maxWidth - m_minWidth) * m_fProgress, 0));
	}
}

bool Progress2::init(const char* strImage, const char* strBack, CCRect& rectInsets)
{
	bool bRet = false;
    do 
    {
		CCSpriteFrame* pImage = ImgMgr::getSingleton().getFrame(strImage);
		m_pImage = CCScale9Sprite::createWithSpriteFrame(pImage, rectInsets);
		m_pImage->setAnchorPoint(ccp(0, 0));

		m_pBack = ImgMgr::getSingleton().loadSprite(strBack);
		m_pBack->setAnchorPoint(ccp(0, 0));
		m_pBack->setPosition(ccp(0, 0));

		addChild(m_pBack);
		addChild(m_pImage);

		m_maxWidth = m_pBack->getContentSize().width;
		m_minWidth = m_pImage->getContentSize().width;
		CCAssert(m_maxWidth >= m_minWidth, "[Progress2 init] -- invalid maxWidth");

		setProgress(1.0f);

		setAnchorPoint(ccp(0, 0));

		bRet = true;
	}while(0);

	return bRet;
}

void Progress2::chgImg(const char* strImage)
{
	CCSpriteFrame* pImage = ImgMgr::getSingleton().getFrame(strImage);
	m_pImage->initWithSpriteFrame(pImage);
	m_pImage->setAnchorPoint(ccp(0, 0));
	//m_pImage->setPosition(ccp(2, 2));

	m_pImage->setPosition(ccp(2, (m_pBack->getContentSize().height - pImage->getRect().size.height) / 2));

	setProgress(m_fProgress);
}

void Progress2::setOpacity(GLubyte opacity)
{
	m_pBack->setOpacity(opacity);
	m_pImage->setOpacity(opacity);
}

NS_COC_END
