#ifndef __COC_ASTARMAP_H__
#define __COC_ASTARMAP_H__

#include "BaseDef.h"
#include "AStarNode.h"
#include "MapData.h"
#include <list>

NS_COC_BEGIN

class AStarMap
{
public:
	AStarMap(int w, int h);
	~AStarMap();
public:
	void init(int w, int h);

	void clear();
public:
	AStarNode**			m_pMapNode;

	int					m_width;
	int					m_height;
};

NS_COC_END

#endif  // __COC_ASTARMAP_H__