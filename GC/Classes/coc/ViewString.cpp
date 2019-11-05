#include "ViewString.h"

ViewString& ViewString::getSingleton()
{
	static ViewString s_mgr;
    
	return s_mgr;
}

ViewString::ViewString()
{
    m_pStrDict = CCDictionary::createWithContentsOfFile("strings.plist");
    m_pStrDict->retain();
}

ViewString::~ViewString()
{
    m_pStrDict->release();
}

string replace(string source, string pattern, string dstPattern)
{
    string result;
    string::size_type pos;
    
    int lenSource = source.length();
    int i = 0;
    
    for(i = 0; i < lenSource; ++i)
    {
        pos = source.find(pattern, i);
        if(string::npos == pos)
            break;
        
        if(pos < lenSource)
        {
            string s = source.substr(i, pos - i);
            result += s;
            result += dstPattern;
            i = pos + pattern.length() - 1;
        }
    }
    result += source.substr(i);
    return result;
}

string ViewString::getValue(const char *key)
{
    string str = replace(getSingleton().m_pStrDict->valueForKey(key)->m_sString, "\\n", "\n");
    return str.c_str();
}

CCDictionary* ViewString::getValueDict(const char* key)
{
    CCDictionary* pDict = (CCDictionary*)getSingleton().m_pStrDict->objectForKey(key);
    return pDict;
}