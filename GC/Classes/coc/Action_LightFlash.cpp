#include "Action_LightFlash.h"

NS_COC_BEGIN

Action_LightFlash* Action_LightFlash::create(float d, int min, int max)
{
	Action_LightFlash* pAction = new Action_LightFlash();
	
	pAction->initWithDuration(d);
	pAction->autorelease();

	pAction->m_minAlpha = min;
	pAction->m_maxAlpha = max;
	
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


void Action_LightFlash::update(float time)
{
	CCRGBAProtocol *pRGBAProtocol = dynamic_cast<CCRGBAProtocol*>(m_pTarget);
	if (pRGBAProtocol)
	{
		if(time <= 0.5f)
			pRGBAProtocol->setOpacity((GLubyte)(m_minAlpha + (m_maxAlpha - m_minAlpha) * time * 2));
		else
			pRGBAProtocol->setOpacity((GLubyte)(m_minAlpha + (m_maxAlpha - m_minAlpha) * (1 - time) * 2));
	}
}

CCActionInterval* Action_LightFlash::reverse(void)
{
	return create(m_fDuration, m_minAlpha, m_maxAlpha);
}

NS_COC_END