#include "Flying.h"
#include "ImgMgr.h"

NS_COC_BEGIN

Flying::Flying()
	: m_pDestNode(NULL)
{
}

Flying::~Flying()
{
}

Flying* Flying::create(int cx, int cy, const char* spr, CCNode* pDestNode)
{
	Flying* pFlying = new Flying();
	
	if(pFlying->init(cx, cy, spr, pDestNode))
	{
		pFlying->autorelease();
	
		return pFlying;
	}

	delete pFlying;

	return NULL;
}

bool Flying::init(int cx, int cy, const char* spr, CCNode* pDestNode)
{
	bool bRet = false;
    do 
    {
		CCSprite::init();

		CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(spr);
		if(pFrame == NULL)
			return NULL;

		setPosition(ccp(cx, cy));

		initWithSpriteFrame(pFrame);

		schedule(schedule_selector(Flying::onIdle));  

		bRet = true;
	}while(0);

	return bRet;
}

//CCAction* Flying::beginAct(int cx, int cy, int dx, int dy, float param, const char* spr)
//{
//	m_iFrame = 0;
//
//	CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(spr);
//	if(pFrame == NULL)
//		return NULL;
//
//	setPosition(ccp(cx, cy));
//
//	initWithSpriteFrame(pFrame);
//
//	float sx = cx;    
//	float sy = cy;     
//	float ex = dx + 50 * param;    
//	float ey = dy + 150 * param;
//
//	ccBezierConfig bezier; // 创建贝塞尔曲线    
//	bezier.controlPoint_1 = ccp(cx, cy); // 起始点    
//	bezier.controlPoint_2 = ccp(sx + (ex - sx) * 0.5, sy + (ey - sy) * 0.5 + 200 * param); //控制点    
//	bezier.endPosition = ccp(dx, dy); // 结束位置       
//
//	CCBezierTo *mv = CCBezierTo::create(0.2f, bezier);
//	//CCMoveTo* mv1 = CCMoveTo::create(1, ccp(dx, dy));
//
//	//CCFiniteTimeAction* actions = CCSequence::create(mv, CCCallFunc::create(this,callfunc_selector(Projectile::onAnim)), NULL);
//
//	CCAction* pAct = runAction(mv);
//	pAct->retain();
//
//	return pAct;
//}

void Flying::onAnim()
{
	if(m_iFrame > 0)
	{
		CCPoint ptRole = m_ptLast;
		CCPoint ptTarget = getPosition();

		int sx = ptTarget.x - ptRole.x;
		int sy = ptTarget.y - ptRole.y;
		int degree = 0;

		if (sx == 0 && sy == 0)
			return ;

		if (sy == 0 && sx > 0)
			degree = 90;
		else if (sy == 0 && sx < 0)
			degree = -90;
		else
		{
			float radians = atanf(sx / sy);
			degree = CC_RADIANS_TO_DEGREES(radians);
		}

		setRotation(degree);
	}

	m_ptLast = getPosition();
	++m_iFrame;
}

void Flying::onIdle(float fDelta)
{
	//CCSprite::draw();

	onAnim();
}

NS_COC_END
