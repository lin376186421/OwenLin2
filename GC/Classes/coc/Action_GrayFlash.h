#ifndef __COC_ACTION_GRAYFLASH_H__
#define __COC_ACTION_GRAYFLASH_H__

#include "BaseDef.h"

NS_COC_BEGIN

class Action_GrayFlash : public CCActionInterval
{
 public:

	virtual void update(float time);
	virtual CCActionInterval* reverse(void);
	//virtual CCObject* copyWithZone(CCZone* pZone);
 
public:
	//创建一个动画
	static Action_GrayFlash* create(float d, int min, int max);
public:
	int			m_minGray;
	int			m_maxGray;
};

 NS_COC_END

#endif // __COC_ACTION_GRAYFLASH_H__