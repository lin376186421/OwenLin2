//
//  CCDevice.h
//  RobinHood
//
//  Created by qsun on 13-1-4.
//
//

#ifndef __Utils__CDevice__
#define __Utils__CDevice__

#include "cocos2d.h"

using namespace std;
using namespace cocos2d;

class CDevice{
    
public:
    static CDevice* sharedDevice();

    void init();

    string getDeviceModel();
    int    getDeviceFamily();
    
    string getDeviceZone();
    
    string macAddress();
    string md5MacAddress();

    string getUDID();
    string uniqueDeviceIdentifier();
    string getIDFA();
    
private:
    string macAddress_;
    string uniqueIdentifier_;
    string idfa_;
};

#endif /* defined(__Utils__CDevice__) */
