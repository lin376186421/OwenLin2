//
//  CHttpRequest.cpp
//  RobinHood
//
//  Created by qsun on 13-4-18.
//
//

#include "CHttpRequest.h"
#include "../CNetData.h"
#include "AppConfig.h"
#include "NGNKit.h"
#include "../CJsonParse.h"
#include "../CUrlParse.h"
#include "CAppPlatform.h"

#define BUFFER_SIZE 1024*100

CHttpRequest* CHttpRequest::create()
{
    CHttpRequest *pHttpRequest = new CHttpRequest();
    if (pHttpRequest && pHttpRequest->init()){
        pHttpRequest->autorelease();
    }
    return pHttpRequest;
}

bool CHttpRequest::init()
{
    allowCompressedResponse = true;
    return true;
}

void CHttpRequest::setPostData(Json::Value data)
{
    mData = data;
    
    //set basic data
    mData["appId"] = APP_ID;
    mData["appVersion"] = CAppPlatform::sharedAppPlatform()->getBundleVersion();
    mData["deviceId"] = DEVICE_ID;
    mData["sid"] = NGNKit::sharedKit()->sessionId;
    
//    if (APP_DEBUG) {
//        string debug = mData.toStyledString();
//        CCLOG("-------- Request Post Data --------");
//        CCLOG("%s",debug.c_str());
//    }
}

void CHttpRequest::setResponseHeaders()
{
    if (allowCompressedResponse) {
        responseHeaders.push_back("Content-Type: application/x-gzip;charset=UTF-8");
    }
}

string CHttpRequest::encodePostData(Json::Value postData)
{
    int i = 0;
	int count = postData.size() - 1;

    string encodeData = "";
    
    Json::Value::Members members(postData.getMemberNames());
    for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it)
    {
        std::string &name = *it;
        
        string value = JsonValueToString(postData[name]);
        string data = CCStringFormat("%s=%s%s", encodeURL(name).c_str(), encodeURL(value).c_str(), (i<count ? "&":""));
        
        encodeData.append(data);
        i++;
    }
    
    return encodeData;
}

bool CHttpRequest::isResponseCompressed()
{
    string contextType;
    for (int index = 0; index < responseHeaders.size(); index ++) {
        if (responseHeaders[index].find("Content-Type") != string::npos) {
            contextType = responseHeaders[index];
            break;
        }
    }
    return contextType.find("gzip") != string::npos;
}

string testUrl = "http://cc.ngnsoft.com:8080/server/gr";
void CHttpRequest::sendPost()
{
    setResponseHeaders();
    
    Json::Value postData;
    postData["rq"] = mData;
    
    string encodeData = encodePostData(postData);
    
    char* requestData = new char[encodeData.length()];
    strcpy(requestData, encodeData.c_str());
    
    CCHttpRequest* request = new CCHttpRequest();
//    if (APP_DEBUG) {
//        request->setUrl(testUrl.c_str());
//    }else{
//        request->setUrl(SERVER_URL);
//    }

    request->setUrl(SERVER_URL);
    request->setRequestType(CCHttpRequest::kHttpPost);
    vector<std::string> headers;
    headers.push_back("Content-Type: application/x-www-form-urlencoded; charset=utf-8");
//    headers.push_back(CCStringFormat("Content-Length: %d", strlen(requestData)));
    request->setHeaders(headers);
    request->setResponseCallback(this, callfuncND_selector(CHttpRequest::onHttpRequestCompleted));
        
    string* key = new string(mData["key"].asString());
    request->setUserData(key);
    
    request->setRequestData(requestData, strlen(requestData));
    request->setTag("POST Request");
    CCHttpClient::getInstance()->send(request);
    request->release();
    delete []requestData;
}

void CHttpRequest::onHttpRequestCompleted(CCNode *sender, void *data)
{
    CCHttpResponse *response = (CCHttpResponse*)data;

    if (!response)
    {
        return;
    }
    
    int statusCode = response->getResponseCode();
    CCLOG("response code: %d", statusCode);
    vector<char> *buffer = response->getResponseData();
    string bufferStr(buffer->begin(), buffer->end());
    const char* bufferData = bufferStr.c_str();
    
    char* decodeBuffer = new char[BUFFER_SIZE];
    if (isResponseCompressed()) {
        CNetData netData;
        uLong len = BUFFER_SIZE;
        int err = netData.httpgzdecompress((Byte*)bufferData, (uLong)bufferStr.length(), (Byte*)decodeBuffer, &len);
        if (err < 0) {
            CCLOG("Response data Decompress Error!");
        }
    }else{
        strcpy(decodeBuffer, bufferData);
    }
    
    Json::Value responseData;
    Json::Reader reader;
    reader.parse(decodeBuffer, responseData);
    
    CCDictionary* responseDict = CCDictionary::create();
    
    string* key = (string*)(response->getHttpRequest()->getUserData());
    responseDict->setObject(CCString::create(key->c_str()), "key");
    JsonValueToDictionary(responseData, responseDict);
    
//    if (APP_DEBUG) {
//        string debug = responseData.toStyledString();
//        CCLOG("-------- Response Data --------");
//        CCLOG("key = %s", key->c_str());
//        CCLOG(debug.c_str());
//    }
    
    if (response->isSucceed())
    {
        if (pDelegate && onSucceedCallBack)
        {
            (pDelegate->*onSucceedCallBack)(responseDict);
        }
    }
    else
    {
        responseDict->setObject(CCString::create(response->getErrorBuffer()), "error");
        if (pDelegate && onExceptionCallBack)
        {
            (pDelegate->*onExceptionCallBack)(responseDict);
        }
    }
    
    delete key;
    delete []decodeBuffer;
}
