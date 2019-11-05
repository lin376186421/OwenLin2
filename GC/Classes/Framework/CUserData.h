//
//  CUserArchive.h
//  GameClient
//
//  Created by qsun on 13-8-23.
//
//

#ifndef __GameClient__CUserData__
#define __GameClient__CUserData__

#include <iostream>
#include "cocos2d.h"

using namespace std;
using namespace cocos2d;

#define RATE_FREE_GEMS                  "RATE_FREE_GEMS"
#define RATER_ALERT_COUNT               "RATER_ALERT_COUNT"

#define GP_FREE_GEMS                    "GP_FREE_GEMS"

#define GAME_SETTING_MUSIC              "GAME_SETTING_MUSIC"
#define GAME_SETTING_SOUND              "GAME_SETTING_SOUND"
#define LEAGUE_ID                       "LEAGUE_ID"
#define CLANWAR_ID                      "CLANWAR_ID"
#define GROUPWAR_ID                     "GROUPWAR_ID"

#define RATER_ALERT_MAX_COUNT 2

class CUserData {
    
public:
    
    static void setUserDataForKey(string key, string value)
    {
        CCUserDefault::sharedUserDefault()->setStringForKey(key.c_str(), value);
        CCUserDefault::sharedUserDefault()->flush();
    }
    
    static void setUserDataForKey(string key, bool value)
    {
        CCUserDefault::sharedUserDefault()->setBoolForKey(key.c_str(), value);
        CCUserDefault::sharedUserDefault()->flush();
    }
    
    static void setUserDataForKey(string key, int value)
    {
        CCUserDefault::sharedUserDefault()->setIntegerForKey(key.c_str(), value);
        CCUserDefault::sharedUserDefault()->flush();
    }
    
    static string getUserDataValue(string key)
    {
        return CCUserDefault::sharedUserDefault()->getStringForKey(key.c_str(), "");
    }
    
    static int getUserDataInt(string key)
    {
        return CCUserDefault::sharedUserDefault()->getIntegerForKey(key.c_str(), -1);
    }
    
    static bool getUserDataBool(string key)
    {
        return CCUserDefault::sharedUserDefault()->getBoolForKey(key.c_str(), false);
    }
};

#endif /* defined(__GameClient__CUserArchive__) */
