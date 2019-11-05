#ifndef __COC_ASTAR_H__
#define __COC_ASTAR_H__

#include "BaseDef.h"
#include "AStarNode.h"
#include "MapData.h"
#include "AStarMap.h"
#include <list>

NS_COC_BEGIN

class AStar
{
	//typedef std::pair<int, AStarNode>	_Pair;
	typedef std::multiset<AStarNode, std::less<AStarNode> >	_Set;
public:
	AStar();
	~AStar();
public:
    
    void init();
    
	bool hasNode(AStarNodeList& lst, const AStarNode& node);

	//const AStarNode findMinCost(AStarNodeList& lst);

	const AStarNode findNode(AStarNodeList& lst, int x, int y);

    void pushOpenSet(AStarNode* preNode, AStarNode* curNode);
    
    void pushCloseSet(AStarNode* preNode, AStarNode* curNode);
    
	bool search(int bx, int by, int& ex, int& ey, float length, bool bCanJumpWall, bool isDestWall, int maxfar, int mywidth);
    
    bool tryMove(int bx, int by, int ex, int ey, int mywidth, bool canJump);
    
    bool canMove(int bx, int by, int ex, int ey, int mywidth, bool canJump);

	int countDistance(int bx, int by, int ex, int ey);

	bool procDir(int& dir, int& px, int& py, int &cost, int x, int y);
    
	// 如果终点坐标不可达，则跳转到最接近起点的一个可达坐标
	void procDestPt(int bx, int by, int& ex, int& ey);

	// 最后倒着走一遍close节点
	bool procCloseList(AStarNode* startNode, AStarNode* endNode, int length, bool bCanJumpWall, bool isDestWall, int maxfar, int mywidth);

	void rebuildMapArea();

	bool isWall(int x, int y);

	bool findKeyPt_Result(int& ptx, int& pty);

	bool procResult(int kx, int ky);

	void saveMap();

	void saveMapArea();
private:
	// 这个函数依赖于rebuildMapArea算法，不能随便用的
	int _getCurAreaIndex(int x, int y);
    // 判断2个区域是联通的
    bool _isSameArea(int x, int y, int& id0, int& id1);
    // 改变区域id
    void _chgArea(int x, int y, int id0, int id1);
    
public:
	MapData				m_map;

	MapData				m_mapState;

	MapData				m_mapArea;
	MapData				m_mapSoldier;
    
	AStarNode			m_mapNode[84][84];

	//AStarNodeList		m_lstOpen;
	//AStarNodeList		m_lstClose;
	AStarNodeList		m_lstResult;
	AStarNodeList		m_lstResult2;

	_Set				m_setOpen;
	_Set				m_setClose;

	bool				m_resultOK;
};

NS_COC_END

#endif  // __COC_ASTAR_H__