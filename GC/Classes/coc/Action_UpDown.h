#ifndef __COC_ACTION_LIGHTFLASH_H__
#define __COC_ACTION_LIGHTFLASH_H__

#include "BaseDef.h"

NS_COC_BEGIN

class Action_UpDown : public CCActionInterval
{
 public:

	virtual void update(float time);
	virtual CCActionInterval* reverse(void);
	//virtual CCObject* copyWithZone(CCZone* pZone);
 
public:
	//创建一个动画
	static Action_UpDown* create(float d, int maxHeight);
public:
	int			m_maxHeight;
};

 NS_COC_END

#endif // __COC_ACTION_LIGHTFLASH_H__