#ifndef __COC_BUILDING_WALL_H__
#define __COC_BUILDING_WALL_H__

#include "BaseDef.h"
#include "Building.h"

NS_COC_BEGIN

class WallCombo;

class Building_Wall : public Building
{
public:
	Building_Wall(BuildingInfo info);
	~Building_Wall();
public:
	virtual void init(int buildingid, int lx, int ly);

	virtual void setLogicPos(int lx, int ly);
    
    virtual void chgHP(int off, bool flyfont = false, bool isReal = false);

	virtual bool buildOK();
        
	//! 整排修墙
	bool buildOKLine();
	//! 整排墙的设置Tile
	void setTileLine(bool show, int index);
	//! 整排墙复位
	void resetWallLine();

	virtual void reorder(int zorder);

	virtual void releaseFromScene();

	virtual void chgSelMode(bool isSel);
	
	void chgSelModeLine(bool isSel);

	void rebuildWall(Building_Wall *pWall = NULL);
        
	virtual void setBeAttackMode();
    
    void buildEditWall();
    
    int getWallDirNums();
    
public:
	CCSprite*		m_pUp;
	CCSprite*		m_pRight;

	CCSprite*		m_pBulding;
	//CCSprite*		m_pShadow;

	CCAction*		m_pSelAction;

	float			m_worth;				// 墙的价值，每次寻路需要找墙时，这个值才有效

    Building_Wall*	m_pWallLeft;
	Building_Wall*	m_pWallRight;
	Building_Wall*	m_pWallUp;
	Building_Wall*	m_pWallDown;
    
    Building_Wall* m_pWallEdit[8];
    bool m_isEditWall;
    bool m_isEditMove;
    
	int				m_iAreaNums;				// 该墙周围的不同区域数量，如果该墙周围都不可到达，该值为0
												// 该值为0，则不可被攻击
	WallCombo*		m_pCmb;

	CCSpriteBatchNode*		m_pBatchNode;
    
    int m_wallDirNums;
};

NS_COC_END

#endif  // __COC_BUILDING_WALL_H__
