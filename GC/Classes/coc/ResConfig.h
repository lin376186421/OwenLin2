//
//  ResConfig.h
//  GameClient
//
//  Created by qsun on 14-1-14.
//
//

#ifndef __GameClient__ResConfig__
#define __GameClient__ResConfig__

#include "BaseDef.h"

NS_COC_BEGIN

class ResConfig
{
public:
	static ResConfig& getSingleton();
protected:
	ResConfig();
	virtual ~ResConfig();
public:
	string getValue(const char* key);
    int    getIntValue(const char* key);
    ccColor3B getColorValue(const char*key);
    
public:
    CCDictionary* m_pResConfig;
};

NS_COC_END

#endif /* defined(__GameClient__ResConfig__) */
