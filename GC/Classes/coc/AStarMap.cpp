#include "AStarMap.h"

NS_COC_BEGIN

AStarMap::AStarMap(int w, int h)
	: m_pMapNode(NULL)
	, m_width(0)
	, m_height(0)
{
	init(w, h);
}

AStarMap::~AStarMap()
{
    for(int y = 0; y < m_height; y++)
    {
        delete [] m_pMapNode[y];
    }
    
    delete [] m_pMapNode;
}

void AStarMap::init(int w, int h)
{
	m_pMapNode = new AStarNode*[h];

	for(int y = 0; y < h; ++y)
	{
		m_pMapNode[h] = new AStarNode[w];
	}

	memset(m_pMapNode[0], 0, sizeof(AStarNode) * w * h);

	m_width = w;
	m_height = h;
}

void AStarMap::clear()
{
	memset(m_pMapNode[0], 0, sizeof(AStarNode) * m_width * m_height);
}

NS_COC_END
