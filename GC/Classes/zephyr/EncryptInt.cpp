/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "Zephyr.h"

NS_CC_ZEPHYR_BEGIN

EncryptInt::EncryptInt() 
	: m_val(0)
{
}

EncryptInt::~EncryptInt() 
{
}

void EncryptInt::set(int val)
{
	int tmp[4];

	tmp[1] = val & 0xff;
	tmp[0] = (val >> 8) & 0xff;
	tmp[3] = (val >> 16) & 0xff;
	tmp[2] = (val >> 24) & 0xff;

	tmp[0] ^= _MASK0;
	tmp[1] ^= _MASK1;
	tmp[2] ^= _MASK2;
	tmp[3] ^= _MASK3;

	m_val = (tmp[0] & 0xff) | ((tmp[1] & 0xff) << 8) | ((tmp[2] & 0xff) << 16) | ((tmp[3] & 0xff) << 24);
}

int EncryptInt::get() const 
{ 
	int tmp[4];
	int ret = 0;

	tmp[0] = m_val & 0xff;
	tmp[1] = (m_val >> 8) & 0xff;
	tmp[2] = (m_val >> 16) & 0xff;
	tmp[3] = (m_val >> 24) & 0xff;

	tmp[0] ^= _MASK0;
	tmp[1] ^= _MASK1;
	tmp[2] ^= _MASK2;
	tmp[3] ^= _MASK3;

	ret = (tmp[1] & 0xff) | ((tmp[0] & 0xff) << 8) | ((tmp[3] & 0xff) << 16) | ((tmp[2] & 0xff) << 24);

	return ret; 
}

NS_CC_ZEPHYR_END