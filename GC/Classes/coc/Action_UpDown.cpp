#include "Action_UpDown.h"

NS_COC_BEGIN

Action_UpDown* Action_UpDown::create(float d, int maxHeight)
{
	Action_UpDown* pAction = new Action_UpDown();
	
	pAction->initWithDuration(d);
	pAction->autorelease();

	pAction->m_maxHeight = maxHeight;
	
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


void Action_UpDown::update(float time)
{
	//m_pTarget
	//CCRGBAProtocol *pRGBAProtocol = dynamic_cast<CCRGBAProtocol*>(m_pTarget);
	//if (pRGBAProtocol)
	//{
	//	if(time <= 0.5f)
	//		pRGBAProtocol->setOpacity((GLubyte)(m_minAlpha + (m_maxAlpha - m_minAlpha) * time * 2));
	//	else
	//		pRGBAProtocol->setOpacity((GLubyte)(m_minAlpha + (m_maxAlpha - m_minAlpha) * (1 - time) * 2));
	//}
}

CCActionInterval* Action_UpDown::reverse(void)
{
	return create(m_fDuration, m_maxHeight);
}

NS_COC_END