#ifndef __COC_MAPDATA_H__
#define __COC_MAPDATA_H__

#include "BaseDef.h"

NS_COC_BEGIN

class MapData
{
public:
	MapData();
	~MapData();
public:
	void init(int w, int h);

	void release();

	void set(int x, int y, int w, int h, int val);

	void set(int x, int y, int val);

	bool isValidPos(int x, int y) const;

	int get(int x, int y) const;

	void cleanup();

	int add(int x, int y, int value);
	int sub(int x, int y, int value);

public:
	int**				m_pMap;

	int					m_width;
	int					m_height;
};

NS_COC_END

#endif  // __COC_MAPDATA_H__