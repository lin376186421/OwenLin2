#ifndef __COC_SCENEBGR_H__
#define __COC_SCENEBGR_H__

#include "BaseDef.h"

NS_COC_BEGIN

enum{
    TILE_W	=	56,
    TILE_H	=	42,
    
    TILE_CELL = 2,
    EDGE_W = 2,
    
    LOGIC_W	=	84,
    
    LOGIC_METER = 50,
    
    _BEGIN_X	=	160*2 - 56*2,
    _BEGIN_Y	=	294*2 - 42*2,
    
    CTRL_BUILD		=	1,
    CTRL_ATTACK		=	2
};

class SceneBgr : public CCNode
{
public:
	SceneBgr();
	~SceneBgr();
public:
    
    CCBAnimationManager* m_pAnimationManager;
    
	void init(PlayerScene& scene);
    void ResumeBgAnimation();
public:
	CCNode*		m_pNodeBack;
};

NS_COC_END

#endif  // __COC_SCENEBGR_H__