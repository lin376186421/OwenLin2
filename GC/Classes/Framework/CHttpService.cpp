//
//  CHttpService.cpp
//  RobinHood
//
//  Created by qsun on 13-4-22.
//
//

#include "CHttpService.h"
#include "AppConfig.h"

static CHttpService* s_pHttpService = NULL;
CHttpService* CHttpService::sharedHttpService()
{
    if (s_pHttpService == NULL) {
        s_pHttpService = new CHttpService();
    }
    return s_pHttpService;
}

void CHttpService::onRequestSucceed(CCDictionary* result)
{
//    bool ret = result->objectForKey("ret");
    string key = ((CCString*)result->objectForKey("key"))->m_sString;
    
    if (key == "upnf") {
        CCDictionary* pConfig = (CCDictionary*)result->objectForKey("out");
//        [self saveFile:LOCAL_FILE_CONFIG file:self.config];
//        [self serverConfig];
//        
//        int64_t serverTime = [[result objectForKey:@"tm"] longLongValue];
//        self.serverTime = serverTime;
    }else if(key == "lssl"){
        CCArray *salesArray = (CCArray*)result->objectForKey("out");
    
        if (salesArray) {

			for(int index = 0; index < salesArray->count(); index ++) {

				CCDictionary* pDict = (CCDictionary*)salesArray->objectAtIndex(index);

				string pid = ((CCString*)pDict->objectForKey("pid"))->m_sString;
				string uuid = ((CCString*)pDict->objectForKey("UUID"))->m_sString;

//				if(pid == PRODUCT_FREE_COINS)
//				{
//					//add point
//				}
				removeSaleLogs(uuid);
			}
        }
    }
}

void CHttpService::onRequestException(CCDictionary* result)
{
    if (result) {
        CCString* error = (CCString*)result->objectForKey("error");
//        CCLOG(error->getCString());
        CCLOG("%s",error->getCString());

        string key = ((CCString*)result->objectForKey("key"))->m_sString;
    }
}

void CHttpService::serverList()
{
    Value requestData;
    requestData["key"] = "LSEN";
    requestData["all"] = true;
    
    CHttpRequest* request = CHttpRequest::create();
    request->setPostData(requestData);
    request->pDelegate = this;
    request->onSucceedCallBack = request_callback(CHttpService::onRequestSucceed);
    request->onExceptionCallBack = request_callback(CHttpService::onRequestException);
    
    request->sendPost();
}

void CHttpService::serverConfig()
{
    Value requestData;
    requestData["key"] = "upnf";
    
    CHttpRequest* request = CHttpRequest::create();
    request->setPostData(requestData);
    request->pDelegate = this;
    request->onSucceedCallBack = request_callback(CHttpService::onRequestSucceed);
    request->onExceptionCallBack = request_callback(CHttpService::onRequestException);
    
    request->sendPost();
}

void CHttpService::saleLogs()
{
    Value requestData;
    requestData["key"] = "lssl";
    
    CHttpRequest* request = CHttpRequest::create();
    request->setPostData(requestData);
    request->pDelegate = this;
    request->onSucceedCallBack = request_callback(CHttpService::onRequestSucceed);
    request->onExceptionCallBack = request_callback(CHttpService::onRequestException);

    request->sendPost();
}

void CHttpService::removeSaleLogs(string uuids)
{
    Value requestData;
    requestData["key"] = "rmsl";
    requestData["uuids"] = uuids;
    
    CHttpRequest* request = CHttpRequest::create();
    request->setPostData(requestData);
    request->pDelegate = this;
    request->onSucceedCallBack = request_callback(CHttpService::onRequestSucceed);
    request->onExceptionCallBack = request_callback(CHttpService::onRequestException);
    
    request->sendPost();
}
