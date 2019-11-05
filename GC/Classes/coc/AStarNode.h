#ifndef __COC_ASTARNODE_H__
#define __COC_ASTARNODE_H__

#include "BaseDef.h"

NS_COC_BEGIN

#define _MAX_COST 100000000

class AStarNode
{
public:
	AStarNode();
	~AStarNode() {}
public:
	AStarNode* operator = (const AStarNode right) {
		costF = right.costF;
        costG = right.costG;
        costH = right.costH;
		step = right.step;
        
        distance_begin = right.distance_begin;
        distance_end = right.distance_end;
        
        preNode = right.preNode;
        
		return this;
	}

    AStarNode* reset()
    {
        costF = 0;
        costG = 0;
        costH = 0;
		step = 0;
        
        distance_begin = 0;
        distance_end = 0;
        
        preNode = NULL;
        
        return this;
    }
    
	bool operator == (const AStarNode& right) const
	{
		return (x == right.x && y == right.y);
	}

	bool operator < (const AStarNode& right) const
	{
		return costF < right.costF;
	}

	bool operator > (const AStarNode& right) const
	{
		return costF > right.costF;
	}
public:
	int x, y;				// 坐标
    int costF, costG, costH;
	int step;				// 步数
    
    int distance_begin;
    int distance_end;
    
    AStarNode* preNode;
};

typedef std::list<AStarNode>	AStarNodeList;

NS_COC_END

#endif  // __COC_ASTARNODE_H__