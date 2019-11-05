#include "AStarNode.h"

NS_COC_BEGIN

AStarNode::AStarNode()
	: x(0)
	, y(0)
	, costF(0)
    , costG(0)
    , costH(0)
	, step(0)
    , preNode(NULL)
{
}

NS_COC_END