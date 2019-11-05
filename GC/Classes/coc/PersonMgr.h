#ifndef __COC_PERSONMGR_H__
#define __COC_PERSONMGR_H__

#include "BaseDef.h"
#include "Person.h"
#include "BaseCSV.h"

NS_COC_BEGIN

class PersonMgr : public BaseCSV
{
	typedef std::pair<int, SoldierInfo>			_PairSoldierInfo;
	typedef std::map<int, SoldierInfo>			_MapSoldierInfo;
protected:
	PersonMgr();
	~PersonMgr();
public:
	static PersonMgr& getSingleton();

	Person* newPerson(int personid, int camp);

	const SoldierInfo* getSoldierInfo(int soldierid) const;
    
    const SoldierInfo* getSoldierMaxInfo(int soldierid) const;
public:
	virtual void onLoadEnd();

	virtual void onRelease();
public:
	//std::map<int, PersonResInfo>	m_mapPersonRes;
	_MapSoldierInfo					m_mapSoldier;
};

NS_COC_END

#endif  // __COC_PERSONMGR_H__