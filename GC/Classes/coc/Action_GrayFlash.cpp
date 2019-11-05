#include "Action_GrayFlash.h"

NS_COC_BEGIN

Action_GrayFlash* Action_GrayFlash::create(float d, int min, int max)
{
	Action_GrayFlash* pAction = new Action_GrayFlash();
	
	pAction->initWithDuration(d);
	pAction->autorelease();

	pAction->m_minGray = min;
	pAction->m_maxGray = max;
	
	return pAction;
}

////创建一个拷贝Action_LightFlash
//CCObject* Action_LightFlash::copyWithZone(CCZone *pZone)
//{
//	CCZone* pNewZone = NULL;
//	Action_LightFlash* pCopy = NULL;
//	if(pZone && pZone->m_pCopyObject)
//	{
//		//in case of being called at sub class
//		pCopy = (Action_LightFlash*)(pZone->m_pCopyObject);
//	}
//	else
//	{
//		pCopy = new Action_LightFlash();
//		pZone = pNewZone = new CCZone(pCopy);
//	}
//
//	CCActionInterval::copyWithZone(pZone);
//
//	CC_SAFE_DELETE(pNewZone);
//
//	return pCopy;
//}


void Action_GrayFlash::update(float time)
{
	CCRGBAProtocol *pRGBAProtocol = dynamic_cast<CCRGBAProtocol*>(m_pTarget);
	if (pRGBAProtocol)
	{
		ccColor3B color;

		if(time <= 0.5f)
			color.r = color.g = color.b = (GLubyte)(m_minGray + (m_maxGray - m_minGray) * time * 2);
		else
			color.r = color.g = color.b = (GLubyte)(m_minGray + (m_maxGray - m_minGray) * (1 - time) * 2);

		pRGBAProtocol->setColor(color);
	}
}

CCActionInterval* Action_GrayFlash::reverse(void)
{
	return create(m_fDuration, m_minGray, m_maxGray);
}

NS_COC_END