#ifndef __COC_PATH_H__
#define __COC_PATH_H__

#include "BaseDef.h"
#include <queue>

NS_COC_BEGIN

struct PathNode
{
	PathNode() : x(-1), y(-1) {}
	PathNode(int _x, int _y) : x(_x), y(_y) {}

	int x, y;				// 坐标
};

typedef std::vector<PathNode>	Path;

NS_COC_END

#endif  // __COC_PATH_H__