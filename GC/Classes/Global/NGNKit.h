//
//  NGNKit.h
//  RobinHood
//
//  Created by qsun on 13-1-4.
//
//

#ifndef __Global__NGNKit__
#define __Global__NGNKit__

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "cocos2d.h"
#include "AppConfig.h"

using namespace std;
using namespace cocos2d;

#define kMaxStringLen (1024*100)
#define isKindOfClass(obj,class) (dynamic_cast<class*>(obj) != NULL)

extern int64_t m_currentTime;
extern int64_t m_currentMilliSecond;

inline static int64_t millisecondNow();
inline static void setCurrentTime(int64_t tm){ m_currentMilliSecond = millisecondNow(); m_currentTime = tm;}
inline static int64_t getCurrentTimeMs(){
    int64_t curMilliSecond = millisecondNow();
    int64_t milliSecond = curMilliSecond - m_currentMilliSecond;
    
    m_currentTime += milliSecond;
    
    m_currentMilliSecond = curMilliSecond;
    
    return m_currentTime;
}

inline static int64_t getCurrentTime(){
    
    return (getCurrentTimeMs()/1000);
}

static string CCStringFormat(const char* format, ...)
{
    string str;
    
    va_list ap;
    va_start(ap, format);
    bool bRet = false;
    char* pBuf = (char*)malloc(kMaxStringLen);
    memset(pBuf, 0, kMaxStringLen);
    if (pBuf != NULL)
    {
        vsnprintf(pBuf, kMaxStringLen, format, ap);
        str = pBuf;
        free(pBuf);
        bRet = true;
    }
    va_end(ap);
    
    return str;
}

static string CCStringFormat(string format, ...)
{
    string str;
    
    va_list ap;
    va_start(ap, format);
    bool bRet = false;
    char* pBuf = (char*)malloc(kMaxStringLen);
    memset(pBuf, 0, kMaxStringLen);
    if (pBuf != NULL)
    {
        vsnprintf(pBuf, kMaxStringLen, format.c_str(), ap);
        str = pBuf;
        free(pBuf);
        bRet = true;
    }
    va_end(ap);
    
    return str;
}

static string CCStringWithInt(int value)
{
    return CCStringFormat("%d", value);
}

static int64_t millisecondNow()
{
    struct timeval now;
    
    gettimeofday(&now,  0);
    
    int64_t time = ((int64_t)now.tv_sec * 1000 + (int64_t)(now.tv_usec / 1000));
    
    return time;
}

#define DATA_APP_VERSION "DATA_APP_VERSION"

class NGNKit {
public:
    
    static NGNKit* sharedKit();
    
    void init();
    
    bool connectedToNetwork();
    void getServerInfo();
    
public:
    
    string sessionId;
    string serverAddres;
};

#endif /* defined(__Global__NGNKit__) */
