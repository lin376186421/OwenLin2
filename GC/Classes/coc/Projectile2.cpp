#include "Projectile2.h"
#include "ImgMgr.h"
#include "Person.h"
#include "PlayerScene.h"
#include "AniObjMgr.h"

NS_COC_BEGIN

// Bezier cubic formula:
//    ((1 - t) + t)3 = 1 
// Expands to… 
//   (1 - t)3 + 3t(1-t)2 + 3t2(1 - t) + t3 = 1 
static inline float bezierat( float a, float b, float c, float d, float t )
{
    return (powf(1-t,3) * a + 
            3*t*(powf(1-t,2))*b + 
            3*powf(t,2)*(1-t)*c +
            powf(t,3)*d );
}

Projectile2::Projectile2()
	: m_pDest(NULL)
	, m_isChg2EndFrame(false)
	//, m_pAnimate(NULL)
	//, m_pAction(NULL)
	, m_isRelease(false)
	, m_isShootDest(false)
    , m_speed(0)
{
}

Projectile2::~Projectile2()
{
}

bool Projectile2::init()
{
	bool bRet = false;
    do 
    {
		CCSprite::init();
        
		schedule(schedule_selector(Projectile2::onIdle));  

		bRet = true;
	}while(0);

	return bRet;
}

void Projectile2::onIdle(float dt)
{
	m_curTime += dt;

	if(m_curTime >= m_totalTime)
	{
		if(!m_isShootDest)
		{
			if(m_isAOE)
			{
                if (m_pDest) {
                    PlayerScene::getSingleton().procAOE(m_pDest->getCenterPt().x, m_pDest->getCenterPt().y, m_damage, m_rAOE, m_camp, 2, false, m_damage, m_bFlyFont);
                }
                else {
                    PlayerScene::getSingleton().procAOE(m_centerPoint.x, m_centerPoint.y, m_damage, m_rAOE, m_camp, 2, false, m_damage, m_bFlyFont);
                }
			}
			else if(m_pDest != NULL && !m_pDest->isEmptyHP())
			{
				m_pDest->chgHP(-m_damage, m_bFlyFont);
			}

			m_isShootDest = true;
		}
        
        {
            if(m_isChg2EndFrame)
                return ;

            if(!m_strEndFrame.empty())
            {
                CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(m_strEndFrame.c_str());
                CCAssert(pFrame, "Projectile2::shoot2Person no img!");

                setDisplayFrame(pFrame);

                if(m_pDest != NULL && OBJTYPE_BUILDING == m_pDest->getObjType())
                {
                    m_pDest->addProjectile2(*this);
                }
            }

            setRotation(m_lastDegree);

            m_isChg2EndFrame = true;
        }

		return;
	}
    
	CCPoint pt = countPos();
	pt.x += m_ptSrc.x;
	pt.y += m_ptSrc.y;

	CCPoint cpt = getPosition();

    procDir(cpt.x, cpt.y, pt.x, pt.y);

	setPosition(pt);
    
    if (m_isTrack) {
        
        m_ptDest = m_pDest->getAttackPos();
        if (m_bDrop) {
            m_ptDest.x = m_ptSrc.x;
        }
        
        int bulletLen = (1 - getAnchorPoint().y) * getContentSize().height;
        int len = sqrt((m_ptDest.x - pt.x) * (m_ptDest.x - pt.x) + (m_ptDest.y - pt.y) * (m_ptDest.y - pt.y));
        
        if (len <= bulletLen) {
            m_totalTime = 0;
            this->setVisible(false);
        }
    }
}

void Projectile2::procDir(float cx, float cy, float dx, float dy)
{
	int sx = dx - cx;
	int sy = dy - cy;
	float degree = 0;

	if (sx == 0 && sy == 0)
		return ;

	if(sy == 0 && sx > 0)
		degree = 90;
	else if(sy == 0 && sx < 0)
		degree = -90;
	else if(sx == 0 && sy > 0)
		degree = 0;
	else if(sx == 0 && sy < 0)
		degree = 180;
	else
	{
		float radians = atanf(sx / (float)sy);
		degree = CC_RADIANS_TO_DEGREES(radians);

		if(sx < 0 && sy < 0)
			degree = 180 + degree;
		else if(sx > 0 && sy < 0)
			degree = 180 + degree;
	}

	if(degree != 0)
		m_lastDegree = degree;

	setRotation(degree);
}

//! speed是1秒移动多少像素
void Projectile2::shoot2Obj(const char* name, int bx, int by, BaseObj* pObj, float speed, float bendVal,
        const char* strBoom, bool isDrop, bool earthquake, bool bRotate, const char* strEndFrame, float endTime,
        bool isTrack, bool isAOE, float damage, int camp, float rAOE, bool flyfont)
{
    
    AniObjMgr::_Map::const_iterator it = AniObjMgr::getSingleton().m_map.find(name);
	if(it == AniObjMgr::getSingleton().m_map.end())
		return;
    
    string aniname = it->second.aniname;
    int aniframes = it->second.iParam[0];
    
	m_camp = camp;
	m_rAOE = rAOE;

	m_isTrack = isTrack;
	m_isAOE = isAOE;
	m_damage = damage;

	m_strEndFrame = strEndFrame;
	m_lastEndFrame = endTime;

    m_speed = speed;

	if(strBoom != NULL)
	{
		m_strBoom = strBoom;
		m_bEarthquake = earthquake;
	}

	m_chgDirTime = 0.0f;

    m_bDrop = isDrop;
	m_bendVal = bendVal;

	m_ptSrc.x = bx;
	m_ptSrc.y = by;

	m_bFlyFont = flyfont;

	m_ptDest = pObj->getAttackPos();
    if (m_bDrop) {
        m_ptDest.x = bx;
    }
    
	if(aniframes > 1)
	{
		CCAnimation* pAni = CCAnimation::create();

		for(int i = 0; i < aniframes; ++i)
		{
            char strname[32];
            sprintf(strname, "%s_%03d", aniname.c_str(), i + 1);
            CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(strname);
            CCAssert(pFrame != NULL, "Projectile2 no frame");
            pAni->addSpriteFrame(pFrame);

            if(i == 0)
            {
                setDisplayFrame(pFrame);
            }
		}

		pAni->setDelayPerUnit(0.12f);
		CCAnimate* pAnimate = CCAnimate::create(pAni);

		CCRepeatForever* pRF = CCRepeatForever::create(pAnimate);
		runAction(pRF);
	}
	else
	{
        char strname[32];
        sprintf(strname, "%s_001", aniname.c_str());
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(strname);
        
		CCAssert(pFrame, "Projectile2::shoot2Person no img!");

		initWithSpriteFrame(pFrame);
	}
    
    setAnchorPoint(ccp(0.5, 1));
    setPosition(ccp(bx, by));
    
    int len = sqrt((m_ptDest.x - bx) * (m_ptDest.x - bx) + (m_ptDest.y - by) * (m_ptDest.y - by));
    if(len <= 0)
		setVisible(false);
    
    m_totalTime = len / m_speed;
	m_curTime = 0.0f;
    
	m_pDest = pObj;
    
	procDir(bx, by, m_ptDest.x, m_ptDest.y);

	if(bRotate)
	{
		CCRotateTo* pRot1 = CCRotateTo::create(0.2f, 180);
		CCRotateTo* pRot2 = CCRotateTo::create(0.3f, 360);

		CCFiniteTimeAction* act1 = CCSequence::create(pRot1, pRot2, NULL);  

		runAction(act1);
	}
}

void Projectile2::shoot2Point(const char* name, int bx, int by, CCPoint point, CCPoint centerPoint, float speed,
            float bendVal, const char* strBoom, bool isDrop, bool earthquake, bool bRotate, const char* strEndFrame,
            float endTime, bool isTrack, bool isAOE, float damage, int camp, float rAOE, bool flyfont)
{
    
    AniObjMgr::_Map::const_iterator it = AniObjMgr::getSingleton().m_map.find(name);
	if(it == AniObjMgr::getSingleton().m_map.end())
		return;
    
    string aniname = it->second.aniname;
    int aniframes = it->second.iParam[0];
    
	m_camp = camp;
	m_rAOE = rAOE;
    
	m_isTrack = isTrack;
	m_isAOE = isAOE;
	m_damage = damage;
    
	m_strEndFrame = strEndFrame;
	m_lastEndFrame = endTime;
    
    m_speed = speed;
    
    m_centerPoint = centerPoint;
    
	if(strBoom != NULL)
	{
		m_strBoom = strBoom;
		m_bEarthquake = earthquake;
	}
    
	m_chgDirTime = 0.0f;
    
    m_bDrop = isDrop;
	m_bendVal = bendVal;
    
	m_ptSrc.x = bx;
	m_ptSrc.y = by;
    
	m_bFlyFont = flyfont;
    
	m_ptDest = point;
    if (m_bDrop) {
        m_ptDest.x = bx;
    }
        
	m_pDest = NULL;
    
	if(aniframes > 1)
	{
		CCAnimation* pAni = CCAnimation::create();
        
		for(int i = 0; i < aniframes; ++i)
		{
			{
				char strname[32];
				sprintf(strname, "%s_%03d", aniname.c_str(), i + 1);
				CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(strname);
				CCAssert(pFrame != NULL, "Projectile2 no frame");
				pAni->addSpriteFrame(pFrame);
                
				if(i == 0)
				{
					setDisplayFrame(pFrame);
				}
                
			}
		}
        
		pAni->setDelayPerUnit(0.12f);
		CCAnimate* pAnimate = CCAnimate::create(pAni);
        
		CCRepeatForever* pRF = CCRepeatForever::create(pAnimate);
		runAction(pRF);
	}
	else
	{
		char strname[32];
        sprintf(strname, "%s_001", aniname.c_str());
        CCSpriteFrame* pFrame = ImgMgr::getSingleton().getFrame(strname);
        
		CCAssert(pFrame, "Projectile2::shoot2Person no img!");
        
		initWithSpriteFrame(pFrame);
	}
    
	setPosition(ccp(bx, by));
    setAnchorPoint(ccp(0.5, 1));
    
    int len = sqrt((m_ptDest.x - bx) * (m_ptDest.x - bx) + (m_ptDest.y - by) * (m_ptDest.y - by));
    if(len <= 0)
		setVisible(false);
    
    m_totalTime = len / m_speed;
	m_curTime = 0.0f;
    
	procDir(bx, by, m_ptDest.x, m_ptDest.y);
    
	if(bRotate)
	{
		CCRotateTo* pRot1 = CCRotateTo::create(0.2f, 180);
		CCRotateTo* pRot2 = CCRotateTo::create(0.3f, 360);
        
		CCFiniteTimeAction* act1 = CCSequence::create(pRot1, pRot2, NULL);  
        
		runAction(act1);
	}
}

CCPoint Projectile2::countPos()
{
    CCPoint pt;
    
	float dx = m_ptDest.x;
	float dy = m_ptDest.y;

	float sx = m_ptSrc.x;    
	float sy = m_ptSrc.y;

	int len = sqrt((dx - sx) * (dx - sx) + (dy - sy) * (dy - sy));
	
	if(m_bendVal == 0)
	{
		pt.x = (dx - sx) * m_curTime / m_totalTime;
		pt.y = (dy - sy) * m_curTime / m_totalTime;
	}
    else {
        
        float mx = sx + (dx - sx) * 0.3;
        float my = MAX(sy, dy) + m_bendVal * len;
        
        ccBezierConfig bezier; // 创建贝塞尔曲线
        bezier.controlPoint_1 = ccp(sx, sy); // 起始点
        bezier.controlPoint_2 = ccp(mx, my); //控制点
        bezier.endPosition = ccp(dx, dy); // 结束位置
        
        bezier.controlPoint_1 = ccpSub(bezier.controlPoint_1, m_ptSrc);
        bezier.controlPoint_2 = ccpSub(bezier.controlPoint_2, m_ptSrc);
        bezier.endPosition = ccpSub(bezier.endPosition, m_ptSrc);
        
        float xa = 0;
        float xb = bezier.controlPoint_1.x;
        float xc = bezier.controlPoint_2.x;
        float xd = bezier.endPosition.x;
        
        float ya = 0;
        float yb = bezier.controlPoint_1.y;
        float yc = bezier.controlPoint_2.y;
        float yd = bezier.endPosition.y;
        
        pt.x = bezierat(xa, xb, xc, xd, m_curTime / m_totalTime);
        pt.y = bezierat(ya, yb, yc, yd, m_curTime / m_totalTime);
    }
    
    return pt;
}

NS_COC_END
