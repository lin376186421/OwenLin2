//
//  CHttpService.h
//  RobinHood
//
//  Created by qsun on 13-4-22.
//
//

#ifndef __Framework__CHttpService__
#define __Framework__CHttpService__

#include "../Utils/jsoncpp/json.h"
#include "cocos2d.h"
#include "../Utils/network/http/CHttpRequest.h"

using namespace cocos2d;

class CHttpService : public CCObject{
public:
    static CHttpService* sharedHttpService();
    
    void onRequestSucceed(CCDictionary* result);
    void onRequestException(CCDictionary* result);
    
    void serverList();
    void serverConfig();
    void saleLogs();
    void removeSaleLogs(string uuids);
};

#endif /* defined(__RobinHood__CHttpService__) */
