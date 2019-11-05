#ifndef __COC_PROJECTILE_H__
#define __COC_PROJECTILE_H__

#include "BaseDef.h"
#include <map>

using namespace cocos2d;

NS_COC_BEGIN

class Projectile : public CCSprite
{
	typedef std::pair<char, std::string>	_Pair;
	typedef std::map<char, std::string>		_Map;
public:
	Projectile();
	virtual ~Projectile();
public:
	CREATE_FUNC(Projectile);
public:
	bool init();

	CCAction* beginAct(int cx, int cy, int dx, int dy, float param, const char* spr);

	void onAnim();

	//virtual void draw();
	void onIdle(float fDelta);

	void procDir(int cx, int cy, int dx, int dy);
public:
	int			m_iFrame;
	CCPoint		m_ptLast;
};

NS_COC_END

#endif  // __COC_PROJECTILE_H__