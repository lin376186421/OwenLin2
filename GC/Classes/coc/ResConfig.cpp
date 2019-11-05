//
//  ResConfig.cpp
//  GameClient
//
//  Created by qsun on 14-1-14.
//
//

#include <iostream>
#include "ResConfig.h"
#include "cocos2d.h"
#include "CSVMgr.h"

using namespace std;
using namespace cocos2d;

NS_COC_BEGIN

ResConfig& ResConfig::getSingleton()
{
	static ResConfig s_resConfig;
    
	return s_resConfig;
}

ResConfig::ResConfig()
{
    m_pResConfig = CCDictionary::createWithContentsOfFile("ResConfig.plist");
    m_pResConfig->retain();
  
    int restype = CCUserDefault::sharedUserDefault()->getIntegerForKey("res_type", -1);
  
    if (restype != -1 && restype != this->getIntValue("RES_TYPE")) {
        for(int i = 0; i < MAX_CSVNUMS; ++i)
            CSVMgr::getSingleton().m_csvVer[i] = 0;
        CSVMgr::getSingleton().saveAll();
    }
        
    CCUserDefault::sharedUserDefault()->setIntegerForKey("res_type", this->getIntValue("RES_TYPE"));
}

ResConfig::~ResConfig()
{
    m_pResConfig->release();
}

std::string ResConfig::getValue(const char* key)
{
    string str = m_pResConfig->valueForKey(key)->getCString();
    return str.c_str();
}

int ResConfig::getIntValue(const char* key)
{
    int value = m_pResConfig->valueForKey(key)->intValue();
    return value;
}


ccColor3B ResConfig::getColorValue(const char *key)
{
    std::string str = getValue(key);
    int r,g,b;
    
    int index = 0;
    int len = 0;
    
    len = str.find (",", index, 1);
    
    r = atoi(str.substr(index, len).c_str());
    
    index = index+len+1;
    len = str.find(",", index, 1) - index;
    
    g = atoi(str.substr(index, len).c_str());
    
    index = index+len+1;
    len = str.length() - index;
    
    b = atoi(str.substr(index, len).c_str());
    
    return ccc3(r, g, b);
}


NS_COC_END