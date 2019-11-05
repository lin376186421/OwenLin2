#ifndef __GAMELAYER_H__
#define __GAMELAYER_H__

#include "cocos2d.h"
#include "coc/PlayerScene.h"

using namespace cocos2d;

struct _TouchNode{
    int     id;
    int     x;
    int     y;
    int     bx;
    int     by;
};

class GameLayer : public CCLayer
{
public:

	GameLayer();
	virtual ~GameLayer();
public:
    // implement the "static create()" method manually
    CREATE_FUNC(GameLayer);
public:
	bool init();

//	virtual void registerWithTouchDispatcher();
	//virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
 //   virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
 //   virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent);

	//virtual void draw();

	void onIdle(float fDelta);

	void onChgScene();

	void clearTouch();
	//void onZoomIn();
	//void onZoomOut();
public:

	float						m_fMulBeginScale;
    float						m_fBeginScale;
    //bool						m_isMulTouch;

	float						m_lastScaleDistance;
    
    std::map<int, _TouchNode>	m_lstTouch;

	bool						m_waitNextCtrl;		// 切换场景时会设置这个值，需要全部抬起手指才能正确操作
};

class TouchLayer : public CCLayer
{
public:
    
	TouchLayer();
	virtual ~TouchLayer();
public:
    // implement the "static create()" method manually
    CREATE_FUNC(TouchLayer);
public:
	bool init();
    
    virtual void registerWithTouchDispatcher();
	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
};

#endif  // __GAMELAYER_H__