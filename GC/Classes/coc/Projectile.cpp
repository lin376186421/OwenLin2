#include "Projectile.h"
#include "ImgMgr.h"

NS_COC_BEGIN

Projectile::Projectile()
{
}

Projectile::~Projectile()
{
}

bool Projectile::init()
{
	bool bRet = false;
    do 
    {
		CCSprite::init();

		schedule(schedule_selector(Projectile::onIdle));  

		bRet = true;
	}while(0);

	return bRet;
}

CCAction* Projectile::beginAct(int cx, int cy, int dx, int dy, float param, const char* spr)
{
	m_iFrame = 0;

	CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(spr);
	if(pFrame == NULL)
		return NULL;

	setPosition(ccp(cx, cy));

	initWithSpriteFrame(pFrame);

	float sx = cx;    
	float sy = cy;     
	float ex = dx + 50 * param;    
	float ey = dy + 150 * param;

	ccBezierConfig bezier; // 创建贝塞尔曲线    
	bezier.controlPoint_1 = ccp(cx, cy); // 起始点    
	bezier.controlPoint_2 = ccp(sx + (ex - sx) * 0.5, sy + (ey - sy) * 0.5 + 200 * param); //控制点    
	bezier.endPosition = ccp(dx, dy); // 结束位置       

	CCBezierTo *mv = CCBezierTo::create(0.2f, bezier);
	//CCMoveTo* mv1 = CCMoveTo::create(1, ccp(dx, dy));

	//CCFiniteTimeAction* actions = CCSequence::create(mv, CCCallFunc::create(this,callfunc_selector(Projectile::onAnim)), NULL);

	CCAction* pAct = runAction(mv);
	pAct->retain();

	procDir(cx, cy, dx, dy);

	return pAct;
}

void Projectile::onAnim()
{
	if(m_iFrame > 0)
	{
		CCPoint ptRole = getPosition();
		CCPoint ptTarget = m_ptLast;

		procDir(ptTarget.x, ptTarget.y, ptRole.x, ptRole.y);
		//int sx = ptTarget.x - ptRole.x;
		//int sy = ptTarget.y - ptRole.y;
		//int degree = 0;

		//if (sx == 0 && sy == 0)
		//	return ;

		//if (sy == 0 && sx > 0)
		//	degree = 90;
		//else if (sy == 0 && sx < 0)
		//	degree = 180;
		//else
		//{
		//	float radians = atanf(sx / sy);
		//	degree = CC_RADIANS_TO_DEGREES(radians);
		//}

		//setRotation(degree);
	}

	m_ptLast = getPosition();
	++m_iFrame;
}

void Projectile::onIdle(float fDelta)
{
	//CCSprite::draw();

	onAnim();
}

void Projectile::procDir(int cx, int cy, int dx, int dy)
{
	int sx = dx - cx;
	int sy = dy - cy;
	int degree = 0;

	if (sx == 0 && sy == 0)
		return ;

	if (sy == 0 && sx > 0)
		degree = 90;
	else if (sy == 0 && sx < 0)
		degree = -90;
	else
	{
		float radians = atanf(sx / (float)sy);
		degree = CC_RADIANS_TO_DEGREES(radians);

		if(sx < 0 && sy < 0)
			degree = 180 + degree;
	}

	setRotation(degree);
}

NS_COC_END
