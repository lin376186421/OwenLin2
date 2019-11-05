/****************************************************************
 cocos2d x zephyr
 *****************************************************************/

#ifndef __ZEPHYR_ViewString_H__
#define __ZEPHYR_ViewString_H__

#include "BaseDef.h"

USING_NS_CC;
using namespace std;

class ViewString
{
public:
	static ViewString& getSingleton();
protected:
	ViewString();
	virtual ~ViewString();
public:
	static std::string getValue(const char* key);
    
    static CCDictionary* getValueDict(const char* key);
    
public:
    CCDictionary* m_pStrDict;
};

#endif  // __ZEPHYR_ViewString_H__