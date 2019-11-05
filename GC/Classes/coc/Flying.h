#ifndef __COC_FLYING_H__
#define __COC_FLYING_H__

#include "BaseDef.h"
#include <map>

using namespace cocos2d;

NS_COC_BEGIN

class Flying : public CCSprite
{
	typedef std::pair<char, std::string>	_Pair;
	typedef std::map<char, std::string>		_Map;
public:
	Flying();
	virtual ~Flying();
public:
	static Flying* create(int cx, int cy, const char* spr, CCNode* pDestNode);
public:
	bool init(int cx, int cy, const char* spr, CCNode* pDestNode);

	//CCAction* beginAct(int cx, int cy, int dx, int dy, float param, const char* spr);

	void onAnim();

	//virtual void draw();
	void onIdle(float fDelta);
public:
	int			m_iFrame;
	CCPoint		m_ptLast;

	CCNode*		m_pDestNode;
};

NS_COC_END

#endif  // __COC_FLYING_H__