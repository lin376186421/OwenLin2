//
//  MilitaryRankMgr.h
//  GameClient
//
//  Created by owenlin on 14-5-19.
//
//

#ifndef __GameClient__MilitaryRankMgr__
#define __GameClient__MilitaryRankMgr__

#include <iostream>
#include "BaseDef.h"
#include "BaseCSV.h"

NS_COC_BEGIN

struct MilitaryInfo
{
    int rank;//军衔等级
    std::string name;//军衔名称
    int troops;//总兵力
    std::vector<int> oId;//obj id 列表
    std::vector<int> oTarget;//obj 需要达到的目标
    int cup;//奖杯数
    int medal;//奖章数
    
};

class MilitaryRankMgr : public BaseCSV
{
protected:
    MilitaryRankMgr();
    ~MilitaryRankMgr();
public:
	static MilitaryRankMgr& getSingleton();
public:
    virtual void onLoadEnd();
    
	virtual void onRelease();
//    void load(const char* name);
//    
//	void reloadFromMem(const char* date, unsigned long size, bool isNeedDecode);
    
    void countMilitaryRank();
    bool checkRank(int rank);
    string getMilitaryName(int rank);
    MilitaryInfo getMilitaryInfo(int rank);
    
public:
	Json::Value		m_jsonData;
    vector<int> split(const char *str);
    vector<MilitaryInfo> m_militaryInfoList;
};

NS_COC_END
#endif /* defined(__GameClient__MilitaryRankMgr__) */
