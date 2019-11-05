#include "MapData.h"
#include "PlayerScene.h"

NS_COC_BEGIN

MapData::MapData()
	: m_pMap(NULL)
	, m_width(0)
	, m_height(0)
{
}

MapData::~MapData()
{
	release();
}

void MapData::init(int w, int h)
{
	m_width = w;
	m_height = h;

	m_pMap = (int**)malloc(sizeof(int*) * h + sizeof(int) * w * h);
	int* pTmp = (int*)((char*)m_pMap + sizeof(int*) * h);

	memset(pTmp, 0, sizeof(int) * w * h);

	for(int i = 0; i < h; ++i)
	{
		m_pMap[i] = pTmp;
		pTmp = (int*)((char*)pTmp + sizeof(int) * w);
	}
}

void MapData::release()
{
	if(m_pMap != NULL)
	{
		free(m_pMap);

		m_pMap = NULL;
	}

	m_width = 0;
	m_height = 0;
}

void MapData::set(int x, int y, int val)
{
	if(isValidPos(x, y))
		m_pMap[y][x] = val;
}

bool MapData::isValidPos(int x, int y) const
{
	return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

int MapData::get(int x, int y) const
{
	if(isValidPos(x, y))
		return m_pMap[y][x];

	return 0;
}

void MapData::set(int x, int y, int w, int h, int val)
{
	if(x < 0)
		x = 0;
	if(y < 0)
		y = 0;

	for(int ty = y; ty < y + h && ty < m_height; ++ty)
	{
		for(int tx = x; tx < x + w && tx < m_width; ++tx)
		{            
			m_pMap[ty][tx] = val;
		}
	}
}

void MapData::cleanup()
{
	memset(&(m_pMap[0][0]), 0, sizeof(int) * m_width * m_height);
}

int MapData::add(int x, int y, int value)
{
	if(isValidPos(x, y))
	{
		m_pMap[y][x] += value;
		return m_pMap[y][x];
	}

	return 0;
}

int MapData::sub(int x, int y, int value)
{
	if(isValidPos(x, y))
	{
		m_pMap[y][x] -= value;
		return m_pMap[y][x];
	}

	return 0;
}

NS_COC_END