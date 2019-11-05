//
//  CHttpRequest.h
//  RobinHood
//
//  Created by qsun on 13-4-18.
//
//

#ifndef __network__CHttpRequest__
#define __network__CHttpRequest__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "../../jsoncpp/json.h"

using namespace cocos2d;
using namespace cocos2d::extension;
using namespace std;

using namespace Json;

typedef void (CCObject::*SEL_HttpRequestHandler)(CCDictionary*);
#define request_callback(_SELECTOR) (SEL_HttpRequestHandler)(&_SELECTOR)

class CHttpRequest : public CCObject{
public:
    
    string mUrl;
    Value mData;
    CCObject* pDelegate;
    SEL_HttpRequestHandler onSucceedCallBack;
    SEL_HttpRequestHandler onExceptionCallBack;
    
    bool allowCompressedResponse;
    
    static CHttpRequest* create();
    bool init();
    void setPostData(Value data);
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void sendPost();
private:
    
    vector<std::string> responseHeaders;
    
    void setResponseHeaders();
    bool isResponseCompressed();
    string encodePostData(Value postData);
};

#endif /* defined(__network__CHttpRequest__) */
