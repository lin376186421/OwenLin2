//
//  EnvMapMgr.h
//  GameClient
//
//  Created by qsun on 14-2-20.
//
//

#ifndef __GameClient__EnvMapMgr__
#define __GameClient__EnvMapMgr__

#include "BaseDef.h"
#include "BaseCSV.h"

NS_COC_BEGIN

class EnvMapMgr : public BaseCSV
{
private:
    typedef std::pair<int, EnvStageInfo>	_PairEnvStageInfo;
	typedef std::map<int, EnvStageInfo>		_MapEnvStageInfo;
protected:
	EnvMapMgr();
	~EnvMapMgr();
public:
	static EnvMapMgr& getSingleton();
    
	EnvStageInfo* getEnvStageInfo(int stageid);
    
    int getEnvStageId(int index);
    
    int getEnvStageNums();
public:
	virtual void onLoadEnd();
    
	virtual void onRelease();

    _MapEnvStageInfo m_mapEvnStages;
    
    vector<int> m_stageidVec;
};

NS_COC_END

#endif /* defined(__GameClient__EnvMapMgr__) */
