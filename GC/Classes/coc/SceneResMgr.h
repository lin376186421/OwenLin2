#ifndef __SANGUO__SCENERESMGR__
#define __SANGUO__SCENERESMGR__

#include "BaseDef.h"

NS_COC_BEGIN

class SceneResMgr
{
public:
	struct _ResNode{
		std::string		resname;
		int				nums;
	};

	typedef std::pair<std::string, _ResNode>	_Pair;
	typedef std::map<std::string, _ResNode>		_Map;
public:
    SceneResMgr();
    ~SceneResMgr();
public:
	static SceneResMgr& getSingleton();
public:
	void loadRes(const char* resname);

	void freeRes(const char* resname);

	void clearRes();
public:
	_Map		m_map;
};

NS_COC_END

#endif // __SANGUO__SCENERESMGR__
