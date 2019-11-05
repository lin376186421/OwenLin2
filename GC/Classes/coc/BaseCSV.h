/****************************************************************
	cocos2d x coc
*****************************************************************/

#ifndef __COC_BASECSV_H__
#define __COC_BASECSV_H__

#include "BaseDef.h"

NS_COC_BEGIN
#define DATA_ENCRYPT 1

class BaseCSV
{
	typedef std::pair<std::string, int>	_PairHead;
	typedef std::map<std::string, int>	_MapHead;
public:
	BaseCSV();
	virtual ~BaseCSV();
public:
	void load(const char* name);

	void loadData(const char* data, unsigned long size, bool isNeedDecode);
    
	void release();

	const char* get(int x, int y) const;

	const char* get(const char* headname, int y) const;

	int getWidth() const { return m_iWidth; }

	int getHeight() const { return m_iHeight; }
public:
	virtual void onLoadEnd() {}

	virtual void onRelease() {}
public:
	char*			m_pBuff;
	char***			m_pArray;
	int				m_iWidth, m_iHeight;

	_MapHead		m_mapHead;

	bool			m_isRemote;
};

NS_COC_END

#endif  // __COC_BASECSV_H__