//#include "HttpClient.h"
#include "BaseDef.h"
#include "../Utils/jsoncpp/json.h"
#include "PlayerScene.h"
#include "EnvMapMgr.h"
#include "HeroMgr.h"
#include "ClanMgr.h"
#include "ClanWarMgr.h"
#include "GroupWarMgr.h"
#include "base32.h"
#include "../Utils/Base64.h"
#include "../HelloWorldScene.h"
#include "../Utils/md5.h"
#include "../Utils/network/CJsonParse.h"
#include "../Global/AppConfig.h"
#include "../Framework/CAppPlatform.h"
#include "CSVMgr.h"
#include "ResConfig.h"
#include "LeagueMgr.h"
#include "zlib.h"
#include "Building_Wall.h"

using namespace cocos2d;
using namespace cocos2d::extension;

NS_COC_BEGIN

HttpClient::HttpClient()
: m_pCurRequest(NULL)
, m_pHelloWorld(NULL)
, m_curClientState(HTTPCLIENT_STATE_WAIT)
, m_numsRequestFail(0)
, m_totalRequestTime(0)
, m_urlroot("")
{
    //	init();
	//m_pCURL = curl_easy_init();
    
	CCLOG("----- urlroot null1");

	m_isProcListener = false;
    
    CCHttpClient::getInstance()->setTimeoutForConnect(10);
    CCHttpClient::getInstance()->setTimeoutForRead(20);
}

HttpClient::~HttpClient()
{
}

HttpClient& HttpClient::getSingleton()
{
	static HttpClient s_client;
    
	return s_client;
}

void HttpClient::procMsgNums(int msgNums)
{
//    bool isSave = false;
//    for (list<_RequestNode*>::iterator it = m_queueRequest.begin(); it != m_queueRequest.end();) {
//        if ((*it)->msgNums <= msgNums) {
//            if ((*it)->bSave) {
//                isSave = true;
//            }
//            it = m_queueRequest.erase(it);
//        }
//        else {
//            ++it;
//        }
//    }
//    
//    if (isSave) {
//        save();
//    }
//    
//    if (m_pCurRequest != NULL) {
//        _RequestNode* pNode = (_RequestNode*)m_pCurRequest->getUserData();
//        
//        if(pNode->msgNums < msgNums || pNode->msgNums > msgNums) {
//            nextRequest(m_pCurRequest);
//        }
//    }
}

void HttpClient::requestSupersonicadsGems(const char* data)
{
	_RequestParam param;

	param.addParam("ctrl", 1);
	param.addParam("ssads", data);
	param.addParam("deviceid", DEVICE_ID);
    
	doHttpRequestPostEx("ctrl_supersonicads.php", param, false, false, true);
}

void HttpClient::requestSupersonicadsData()
{
    char str[248];
    sprintf(str,"http://www.supersonicads.com/api/v1/guc.php?aid=%s&auid=%s&format=json",SSA_APP_KEY,DEVICE_ID);
    std::string fullurl(str);
    
    CCLOG("requestSupersonicadsData__%s",fullurl.c_str());
    
	CCHttpRequest* request = new CCHttpRequest();
	request->setUrl(fullurl.c_str());
    request->setRequestType(CCHttpRequest::kHttpPost);
    request->setResponseCallback(this, httpresponse_selector(HttpClient::onHttpRequestAdsCompleted));
	request->setRequestData(fullurl.c_str(), fullurl.length());
    request->setTag("POST Ads");
    CCHttpClient::getInstance()->send(request);
    request->release();
}

void HttpClient::onHttpRequestAdsCompleted(cocos2d::CCNode *sender,CCHttpResponse *response)
{
    if (!response)
    {
        return;
    }
    std::vector<char> *buffer = response->getResponseData();
	std::string str;
    
	for (unsigned int i = 0; i < buffer->size(); i++)
		str += (buffer->at(i));
    
    if(str.length() == 0)
        return;
    
    Json::Reader r;
    Json::Value val;
    
    r.parse(str, val);
    if(val.type() != Json::objectValue)
        return;
    
    vector<string> member =  val.getMemberNames();
    if(member.size() == 0)
        return;
    
    list<string> timeList;
    for(int i = 0; i < member.size(); i++)
    {
        Json::Value v = val[member[i]];
        string timeStr = v["t"].asCString();
        timeList.push_back(timeStr);
    }
    timeList.sort();//时间排序

    //截取时间最近的5个
    while (timeList.size() > 5) {
        string frontStr = timeList.front();
        timeList.remove(frontStr);
    }
    
    for(int i = 0; i < member.size(); i++)
    {
        Json::Value v = val[member[i]];
        string timeStr = v["t"].asCString();
        
        bool isRemove = true;
        for(list<string>::iterator it = timeList.begin() ; it != timeList.end(); ++it)
        {
            string tStr = *it;
            if(tStr == timeStr)
            {
                isRemove = false;
                break;
            }
        }
        
        if(isRemove)
        {
            val.removeMember(member[i]);
        }
    }
    
    Json::FastWriter write;
    string dataStr = write.write(val);
    dataStr = dataStr.substr(0,dataStr.size()-1);
    
    requestSupersonicadsGems(dataStr.c_str());

}
void HttpClient::onHttpRequestCompleted(cocos2d::CCNode *sender, CCHttpResponse *response)
{
    CCHttpRequest* pRequest = response->getHttpRequest();
    
    if (!response || pRequest->getUserData() == NULL)
    {
        return;
    }
    
    CCLOG("onHttpRequestCompleted");
    
    // You can get original request type from: response->request->reqType
    if (0 != strlen(response->getHttpRequest()->getTag()))
    {
        CCLOG("%s completed", response->getHttpRequest()->getTag());
    }
    
    int statusCode = response->getResponseCode();
    //char statusString[64] = {};
    //sprintf(statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
    //m_labelStatusCode->setString(statusString);
    CCLOG("response code: %d", statusCode);
    
	AutoLock<LockImp> lock(m_lockRequest);
    
	onRequestCompleted(response);
    
    if (!response->isSucceed())
    {
        CCLOG("response failed");
        CCLOG("error buffer: %s", response->getErrorBuffer());
        
        return;
    }
    
	m_numsRequestFail = 0;
    
    // dump data
    std::vector<char> *buffer = response->getResponseData();
	std::string str;
#ifdef COC_HTTP_ZLIB
	unsigned char* pBuff = (unsigned char*)malloc(buffer->size());
	int begin = 0;
	unsigned long destLen = 0;
	for (unsigned int i = 0; i < buffer->size(); i++)
	{
		if(begin == 0 && buffer->at(i) != ' ')
		{
			str += (buffer->at(i));
		}
		else if(begin == 0 && buffer->at(i) == ' ')
		{
			begin = i + 1;
            
			destLen = atoi(str.c_str());
		}
		else if(begin > 0)
		{
			pBuff[i - begin] = buffer->at(i);
		}
	}
    
    str = "";
	if(destLen == 0)
	{
		free(pBuff);
        
		for(unsigned int i = 0; i < buffer->size(); i++)
			str += (buffer->at(i));
	}
	else
	{
		char* pDest = (char*)malloc(destLen + 1);
		uncompress((unsigned char*)pDest, &destLen, pBuff, buffer->size() - begin);
		pDest[destLen] = '\0';
		str = pDest;
        
		free(pBuff);
		free(pDest);
	}
#else
	for (unsigned int i = 0; i < buffer->size(); i++)
		str += (buffer->at(i));
#endif
    
	{
		AutoLock<LockImp> lock(m_lockRecv);
        
		m_msgCache.push(str);
	}
}

void HttpClient::onIdle()
{    
	AutoLock<LockImp> lock(m_lockRecv);
    
	if(!m_msgCache.empty())
	{
		std::string str = m_msgCache.front();
		m_msgCache.pop();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		FILE* fp = fopen("msg.txt", "w");
		if(fp != NULL)
		{
			//fwrite(str.c_str(), str.length(), 1, fp);
			fprintf(fp, str.c_str());

			fclose(fp);
		}
#endif
        
		Json::Reader r;
		Json::Value val;
        
		if(r.parse(str, val))
		{
            if(Json::objectValue == val.type())
            {
                if (val["cid"].asString() == "lsen") {
                    
                    LogicInfo::getSingleton().addTestLog("lsen end");
                    
                    string host = val["host"].asString();
                    int status = val["status"].asInt();
                    string desc = val["statusDesc"].asString();
                    
                    m_urlroot = host;
                    
                    if (APP_DEBUG == 2) {
                        m_urlroot = "http://180.166.115.46/w2serv/dv/";
                    }
                    
                    if (status < 0) {
                        LogicInfo::getSingleton().onNotify(NOTIFY_NETWORK_ERROR, (const char*)desc.c_str());
                    }
                    
                    procCtrl(val);
                }
                else if (val["cid"].asString() == "msgnums") {
                    int msgnums = getAsInt(val["msgnums"]);
                    procMsgNums(msgnums);
                }
                else if(val["cid"].asString() == "cmbmsg")
                {
                    int nums = val["nums"].asInt();
                    for(int i = 0; i < nums; ++i)
                    {
                        char strname[32];
                        sprintf(strname, "c%d", i);
#if(APP_DEBUG)
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
                        CCLOG("------resquest------\n%s", val[strname].toStyledString().c_str());
#endif
#endif
                        procCtrl(val[strname]);
                    }
                }
                else {
#if(APP_DEBUG)
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
                    CCLOG("------resquest------\n%s", val.toStyledString().c_str());
#endif
#endif
                    procCtrl(val);
                }
            }
		}
	}
}

void HttpClient::procCtrl(Json::Value& val)
{
    AutoLock<LockImp> lock(m_lockRecv);

	m_isProcListener = true;
	bool noproc = false;

	for(std::list<NetListener*>::iterator it = m_lstListener.begin(); it != m_lstListener.end(); )
	{
		if(!noproc && (*it) != NULL)
		{
			bool ret = (*it)->onProc(val);
			if(!ret) {
				noproc = true;
            }
		}
        
        if ((*it) == NULL) {
            it = m_lstListener.erase(it);
        }
        else {
            ++it;
        }
	}

	m_isProcListener = false;
}

void HttpClient::addListener(NetListener* pListener)
{
    CCLOG("add pListener: 0x%p", pListener);
    
	AutoLock<LockImp> lock(m_lockRecv);
    
	for(std::list<NetListener*>::iterator it = m_lstListener.begin(); it != m_lstListener.end(); )
	{
		if(*it == pListener)
		{
			it = m_lstListener.erase(it);
		}
		else
			++it;
	}
    
	m_lstListener.push_back(pListener);
}

void HttpClient::removeListener(NetListener* pListener)
{
    CCLOG("remove pListener: 0x%p", pListener);
    
	AutoLock<LockImp> lock(m_lockRecv);
    
    for(std::list<NetListener*>::iterator it = m_lstListener.begin(); it != m_lstListener.end(); ++it)
    {
        if (*it == pListener) {
            
            if(pListener->m_pDelegate != NULL && pListener->m_funcRelease != NULL)
            {
                ((pListener->m_pDelegate)->*(pListener->m_funcRelease))();
            }
            
            (*it)->m_isRelease = true;
            (*it) = NULL;
            
//            if (!m_isProcListener) {
//                m_lstListener.erase(it);
//            }
            break;
        }
    }
}

void HttpClient::lsenRequest()
{
    if (m_urlroot.length() > 0) {
        
        checkVer3();
        return;
    }
    
	_RequestParam param;

	param.addParam("sver", SERVER_VERSION);
    
    doHttpRequestPostEx(SERVER_URL, param, false, false, true);
    
    LogicInfo::getSingleton().addTestLog("lsen");
}

void HttpClient::reg()
{
#ifdef COC_LOGINWITHDEVICEID
	loginWithDeviceID();
	return ;
#endif // COC_LOGINWITHDEVICEID
    
	//AnalyticX::flurryLogEvent("new user");
	//flurry_logEvent("new user");

	_RequestParam param;

	param.addParam("ctrl", 2);
    
	//char param[128];
 //   
	//sprintf(param, "ctrl=2");
    
	doHttpRequestPostEx("userctrl2.php", param, false, false, true);
}

void HttpClient::login()
{
    LogicInfo::getSingleton().m_isLogin = true;
    
#if (LOGIN_WITH_PLAYER_UID == 0)
	loginWithDeviceID();
#else
    
	char userid[128];
	sprintf(userid, "%s[%d]", LogicInfo::getSingleton().m_localUName.c_str(), LogicInfo::getSingleton().m_localUID);
    
	_RequestParam param;

	std::string str = UrlEncode(LogicInfo::getSingleton().m_localUName.c_str());

	param.addParam("ctrl", 16);
//	param.addParam("uid", LogicInfo::getSingleton().m_localUID);
//    param.addParam("uid", LOGIN_WITH_PLAYER_UID);
    param.addParam("uid", LOGIN_WITH_PLAYER_UID);
	param.addParam("uname", str.c_str());
    
	doHttpRequestPostEx("userctrl3.php", param, false, false, true, true, true);
#endif
}

void HttpClient::loginWithDeviceID()
{
	if(!LogicInfo::getSingleton().m_isVerOK || LogicInfo::getSingleton().m_announcementType == ANNOUNCEMENT_TYPE_FORCE)
		return ;
    
	char deviceid[128];
	sprintf(deviceid, "%s", DEVICE_ID);

	_RequestParam param;

	param.addParam("ctrl", 15);
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    char olddevice[128];
	sprintf(olddevice, "%s", CDevice::sharedDevice()->uniqueDeviceIdentifier().c_str());
	param.addParam("olddevice", olddevice);
#endif
	
    param.addParam("deviceid", deviceid);
    
	doHttpRequestPostEx("userctrl3.php", param, false, false, true);
    
    LogicInfo::getSingleton().addTestLog("login");
}

void HttpClient::loginok()
{
    
}

void HttpClient::updateUserMask2(int mask)
{
    _RequestParam param;
    
	param.addParam("ctrl", 17);
	param.addParam("mask", mask);
    
	doHttpRequestPostEx("userctrl3.php", param, false, false, true);
}

void HttpClient::checkVer()
{
	std::string ub = UrlEncode(CAppPlatform::sharedAppPlatform()->getBundleId().c_str());
	std::string ver = UrlEncode(CAppPlatform::sharedAppPlatform()->getBundleVersion().c_str());

	_RequestParam param;

	param.addParam("ctrl", 2);
	param.addParam("boundid", ub.c_str());
	param.addParam("ver", ver.c_str());
    
	doHttpRequestPostEx("verctrl2.php", param, false, false, true);
}

void HttpClient::payProtectTime(int day)
{
	_RequestParam param;

	param.addParam("ctrl", 7);
	param.addParam("day", day);
    
	//char param[128];
 //   
	//sprintf(param, "ctrl=7&day=%d", day);
    
	doHttpRequestPostEx("otherctrl2.php", param, false, true, true);
}

void HttpClient::updUserGameTime(int timeoff)
{
	_RequestParam param;

	param.addParam("ctrl", 17);
	param.addParam("time", timeoff);
    
	//char param[128];
 //   
	//sprintf(param, "ctrl=17&time=%d", timeoff);
    
	doHttpRequestPostEx("otherctrl2.php", param, false, true, true);
}

void HttpClient::updPushDeviceID(const char* deviceid)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	updRegistrationID(deviceid);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	updDeviceToken(deviceid);
#endif
    
	//updRegistrationID(deviceid);
    //	updDeviceToken("device_token");
}

void HttpClient::updRegistrationID(const char* deviceid)
{
	if(deviceid == NULL || strlen(deviceid) <= 0)
		return ;
    
	std::string device = UrlEncode(deviceid);

	_RequestParam param;

	param.addParam("ctrl", 11);
	param.addParam("registration_id", device.c_str());
    
    //string param = CCStringFormat("ctrl=11&registration_id=%s", device.c_str());
    
	doHttpRequestPostEx("otherctrl2.php", param, false, false, true);
}

void HttpClient::updDeviceToken(const char* deviceid)
{
	if(deviceid == NULL || strlen(deviceid) <= 0)
		return ;
    
	std::string code;
	int len = strlen(deviceid);
	for(int i = 0; i < len; ++i)
	{
		if(deviceid[i] >= '0' && deviceid[i] <= '9')
			code.append(1, deviceid[i]);
		else if(deviceid[i] >= 'a' && deviceid[i] <= 'z')
			code.append(1, deviceid[i]);
		else if(deviceid[i] >= 'A' && deviceid[i] <= 'Z')
			code.append(1, deviceid[i]);
	}
    
	std::string device = UrlEncode(deviceid);

	_RequestParam param;

	param.addParam("ctrl", 12);
	param.addParam("device_token", device.c_str());
    
    //string param = CCStringFormat("ctrl=12&device_token=%s", device.c_str());
    
	doHttpRequestPostEx("otherctrl2.php", param, false, false, true);
}

//void HttpClient::revenge(int msgid)
//{
////	char param[128];
////    
////	sprintf(param, "ctrl=13&msgid=%d", msgid);
////    
////	doHttpRequestPost("otherctrl2.php", param, false, true, true);
//}

void HttpClient::updUserInfo()
{
	std::string devicever = UrlEncode(CDevice::sharedDevice()->getDeviceModel().c_str());
	std::string clientver = UrlEncode(CAppPlatform::sharedAppPlatform()->getBundleId().c_str());
    std::string softver = UrlEncode(CAppPlatform::sharedAppPlatform()->getBundleVersion().c_str());
	std::string deviceinfo = UrlEncode("unknow");

	_RequestParam param;

	param.addParam("ctrl", 9);
	param.addParam("devicever", devicever.c_str());
	param.addParam("clientver", clientver.c_str());
	param.addParam("deviceinfo", deviceinfo.c_str());
	param.addParam("softver", softver.c_str());
    
	doHttpRequestPostEx("otherctrl2.php", param, false, true, true);
}

void HttpClient::requestGradeGem()
{
	_RequestParam param;
	param.addParam("ctrl", 10);
    
	doHttpRequestPostEx("otherctrl2.php", param, false, true, true);
}

void HttpClient::requestFirstPayGem()
{
	_RequestParam param;
	param.addParam("ctrl", 15);
    
	doHttpRequestPostEx("otherctrl2.php", param, false, true, true);
}

void HttpClient::updGuideVal(int val)
{
	_RequestParam param;
	param.addParam("ctrl", 16);
	param.addParam("zyval", val);
    
	doHttpRequestPostEx("otherctrl2.php", param, false, true, true);
}

void HttpClient::upLevelHero()
{
    _RequestParam param;
    param.addParam("ctrl", 4);
    
    doHttpRequestPostEx("heroctrl.php", param, false, true, true);
}

void HttpClient::finishUpLevelHero(int gem)
{
    _RequestParam param;
    param.addParam("ctrl", 5);
    param.addParam("m0", gem);
    
    doHttpRequestPostEx("heroctrl.php", param, false, true, true);
}

void HttpClient::cancelUpLevelHero()
{
    _RequestParam param;
    param.addParam("ctrl", 6);
    
    doHttpRequestPostEx("heroctrl.php", param, false, true, true);
}

void HttpClient::chgSkill3(int skillid)
{
    HeroMgr::getSingleton().m_cgnums++;

	_RequestParam param;
	param.addParam("ctrl", 13);
	param.addParam("skillid", skillid);
    
	doHttpRequestPostEx("heroctrl2.php", param, false, true, true);
}

void HttpClient::upgradeSkill(int skillindex, int type)
{
    HeroMgr::getSingleton().m_lbnums++;

	_RequestParam param;

	param.addParam("ctrl", 15);
	param.addParam("skillindex", skillindex);
	param.addParam("type", type);
    
	doHttpRequestPostEx("heroctrl2.php", param, false, true, true);
}

void HttpClient::chgHeroInBuilding(int bx, int by)
{
	bx = bx + 1;
	by = by + 1;

	_RequestParam param;

	param.addParam("ctrl", 15);
	param.addParam("bx", bx);
	param.addParam("by", by);
    
	doHttpRequestPostEx("heroctrl2.php", param, false, true, true);
}

void HttpClient::checkVer3()
{
    std::string ub = UrlEncode(CAppPlatform::sharedAppPlatform()->getBundleId().c_str());
    std::string ver = UrlEncode(CAppPlatform::sharedAppPlatform()->getBundleVersion().c_str());

	_RequestParam param;

	param.addParam("ctrl", 8);
	param.addParam("boundid", ub.c_str());
	param.addParam("ver", ver.c_str());
	param.addParam("ver0", CSVMgr::getSingleton().m_csvVer[0]);
	param.addParam("ver1", CSVMgr::getSingleton().m_csvVer[1]);
	param.addParam("ver2", CSVMgr::getSingleton().m_csvVer[2]);
	param.addParam("ver3", CSVMgr::getSingleton().m_csvVer[3]);
	param.addParam("ver4", CSVMgr::getSingleton().m_csvVer[4]);
	param.addParam("ver5", CSVMgr::getSingleton().m_csvVer[5]);
    param.addParam("deviceid", DEVICE_ID);
    
	doHttpRequestPostEx("verctrl2.php", param, false, false, true);
    
    LogicInfo::getSingleton().addTestLog("csv ver");
}

void HttpClient::checkCSV(string key[6])
{
    std::string ub = UrlEncode(CAppPlatform::sharedAppPlatform()->getBundleId().c_str());
    std::string ver = UrlEncode(CAppPlatform::sharedAppPlatform()->getBundleVersion().c_str());

	_RequestParam param;

	param.addParam("ctrl", 9);
	param.addParam("boundid", ub.c_str());
	param.addParam("ver", ver.c_str());
	param.addParam("ver0", CSVMgr::getSingleton().m_csvVer[0]);
	param.addParam("ver1", CSVMgr::getSingleton().m_csvVer[1]);
	param.addParam("ver2", CSVMgr::getSingleton().m_csvVer[2]);
	param.addParam("ver3", CSVMgr::getSingleton().m_csvVer[3]);
	param.addParam("ver4", CSVMgr::getSingleton().m_csvVer[4]);
	param.addParam("ver5", CSVMgr::getSingleton().m_csvVer[5]);
	param.addParam("key0", key[0].c_str());
	param.addParam("key1", key[1].c_str());
	param.addParam("key2", key[2].c_str());
	param.addParam("key3", key[3].c_str());
	param.addParam("key4", key[4].c_str());
	param.addParam("key5", key[5].c_str());
    
	doHttpRequestPostEx("verctrl2.php", param, false, false, true, true);
    
}

void HttpClient::checkUserCenterBind(const char* uidUserCenter)
{
	std::string uname = UrlEncode(uidUserCenter);
    
	_RequestParam param;

	param.addParam("ctrl", 11);
	param.addParam("uname", uname.c_str());
    param.addParam("type", LogicInfo::getSingleton().m_bindType);
    
	doHttpRequestPostEx("userctrl3.php", param, false, false, true);
}

void HttpClient::rebindGameCenter(const char* uidUserCenter, int uid)
{
#if LOGIN_WITH_PLAYER_UID
    return;
#endif
    
	std::string uname = UrlEncode(uidUserCenter);

	_RequestParam param;

	param.addParam("ctrl", 13);
	param.addParam("uname", uname.c_str());
	param.addParam("destuid", uid);
	param.addParam("deviceid", DEVICE_ID);
    param.addParam("type", LogicInfo::getSingleton().m_bindType);
    
	doHttpRequestPostEx("userctrl3.php", param, false, false, true);
}

void HttpClient::build(int id, int buildingid, int lx, int ly, int state)
{
#if LOGIN_WITH_PLAYER_UID
    return;
#endif
    
	if(!PlayerScene::getSingleton().m_isInit)
		return;
    
	lx = lx + 1;
	ly = ly + 1;

	int ct = getCurrentTime();// time(NULL) + LogicInfo::getSingleton().m_offServiceTime;

	_RequestParam param;

	param.addParam("ctrl", 1);
    param.addParam("id", id);
	param.addParam("bid", buildingid);
    param.addParam("level", 1);
	param.addParam("x", lx);
	param.addParam("y", ly);
	param.addParam("state", state);
	param.addParam("curtime", ct);
    
	doHttpRequestPostEx("buildingctrl3.php", param, false, true, true);
}

void HttpClient::makeCommonPostParamEx(std::string& param)
{
	char str[10240 * 2];
#ifdef COC_HTTP_ZLIB
	sprintf(str, "zlib=1&");
#else
	sprintf(str, "");
#endif
    
	std::string str1 = str;
    
	str1 += param;
	param = str1;
}

void HttpClient::makeCommonGetURLEx(std::string& fullurl, const char* url, const char* param)
{
	char str[10240 * 2];
    
#ifdef COC_HTTP_ZLIB
	sprintf(str, "zlib=1&");
#else
	sprintf(str, "");
#endif
    
	fullurl = m_urlroot;
    
	fullurl += url;
	fullurl += "?";
	fullurl += str;
	fullurl += param;
}

void HttpClient::makeCommonPostParam(std::string& param)
{
	char str[10240 * 2];
#ifdef COC_HTTP_ZLIB
	sprintf(str, "uid=%d&sid=%d&handle=%s&zlib=1&msgnums=%d&", LogicInfo::getSingleton().m_myInfo.uid, LogicInfo::getSingleton().m_myInfo.sessionid, LogicInfo::getSingleton().m_myInfo.handle.c_str(), ++LogicInfo::getSingleton().m_myMsgNums);
#else
	sprintf(str, "uid=%d&sid=%d&handle=%s&msgnums=%d&", LogicInfo::getSingleton().m_myInfo.uid, LogicInfo::getSingleton().m_myInfo.sessionid, LogicInfo::getSingleton().m_myInfo.handle.c_str(), ++LogicInfo::getSingleton().m_myMsgNums);
#endif
    
	std::string str1 = str;
    
	str1 += param;
	param = str1;
}

void HttpClient::makeCommonGetURL(std::string& fullurl, const char* url, const char* param)
{
	char str[10240 * 2];
    
#ifdef COC_HTTP_ZLIB
	sprintf(str, "uid=%d&sid=%d&handle=%s&zlib=1&msgnums=%d&", LogicInfo::getSingleton().m_myInfo.uid, LogicInfo::getSingleton().m_myInfo.sessionid, LogicInfo::getSingleton().m_myInfo.handle.c_str(), ++LogicInfo::getSingleton().m_myMsgNums);
#else
	sprintf(str, "uid=%d&sid=%d&handle=%s&msgnums=%d&", LogicInfo::getSingleton().m_myInfo.uid, LogicInfo::getSingleton().m_myInfo.sessionid, LogicInfo::getSingleton().m_myInfo.handle.c_str(), ++LogicInfo::getSingleton().m_myMsgNums);
#endif
    
	fullurl = m_urlroot;
    
	fullurl += url;
	fullurl += "?";
	fullurl += str;
    
	fullurl += param;
}

void HttpClient::move(int id, int bx, int by, int ex, int ey)
{
    if (PlayerScene::getSingleton().m_isEditMode) {
        return;
    }
    
	bx = bx + 1;
	by = by + 1;
    
	ex = ex + 1;
	ey = ey + 1;
    
	if(!PlayerScene::getSingleton().m_isInit)
		return;

	_RequestParam param;

	param.addParam("ctrl", 2);
    param.addParam("id", id);
	param.addParam("bx", bx);
	param.addParam("by", by);
	param.addParam("ex", ex);
	param.addParam("ey", ey);
    
	doHttpRequestPostEx("buildingctrl2.php", param, false, true, true);
}

void HttpClient::keeplive(bool bCanFail)
{
	if(!LogicInfo::getSingleton().m_isProcKeepLife)
		return;
    
	_RequestParam param;
	param.addParam("ctrl", 1);
    
	doHttpRequestPostEx("keeplive.php", param, bCanFail, false, true, true);
}

void HttpClient::updateBuilding(int id, int buildingid, int x, int y, int level, int lasttime)
{
#if LOGIN_WITH_PLAYER_UID
    return;
#endif
    
	x = x + 1;
	y = y + 1;

	_RequestParam param;

	param.addParam("ctrl", 3);
    param.addParam("id", id);
    param.addParam("bid", buildingid);
	param.addParam("x", x);
	param.addParam("y", y);
	param.addParam("lasttime", lasttime);
    
	doHttpRequestPostEx("buildingctrl3.php", param, false, true, true);
}

void HttpClient::harvest(int id, int x, int y, int restime)
{
	x = x + 1;
	y = y + 1;

	_RequestParam param;

	param.addParam("ctrl", 4);
    param.addParam("id", id);
	param.addParam("x", x);
	param.addParam("y", y);
	param.addParam("restime", restime);
    
	doHttpRequestPostEx("buildingctrl5.php", param, false, true, true);
}

void HttpClient::levelupBuilding(int id, int buildingid, int x, int y)
{
#if LOGIN_WITH_PLAYER_UID
    return;
#endif
    
	x = x + 1;
	y = y + 1;

	int ct = getCurrentTime();// time(NULL) + LogicInfo::getSingleton().m_offServiceTime;

	_RequestParam param;

	param.addParam("ctrl", 7);
    param.addParam("id", id);
	param.addParam("x", x);
	param.addParam("y", y);
	param.addParam("bid", buildingid);
	param.addParam("curtime", ct);
    
	doHttpRequestPostEx("buildingctrl3.php", param, false, true, true);
}

void HttpClient::fastBuilding(int id, int buildingid, int x, int y, int level, int lasttime)
{
#if LOGIN_WITH_PLAYER_UID
    return;
#endif
    
	x = x + 1;
	y = y + 1;

	_RequestParam param;
	param.addParam("ctrl", 8);
    param.addParam("id", id);
	param.addParam("x", x);
	param.addParam("y", y);
	param.addParam("bid", buildingid);
    param.addParam("level", level);
	param.addParam("lasttime", lasttime);

	doHttpRequestPostEx("buildingctrl3.php", param, false, true, true);
}

void HttpClient::removeBuilding(int id, int buildingid, int x, int y)
{
#if LOGIN_WITH_PLAYER_UID
    return;
#endif
    
	x = x + 1;
	y = y + 1;

	_RequestParam param;

	param.addParam("ctrl", 9);
    param.addParam("id", id);
    param.addParam("bid", buildingid);
	param.addParam("x", x);
	param.addParam("y", y);
    
	doHttpRequestPostEx("buildingctrl3.php", param, false, true, true);
}

void HttpClient::findUser()
{
	_RequestParam param;
	param.addParam("ctrl", 5);
    
	doHttpRequestPostEx("warctrl5.php", param, false, false, true);
    
    LogicInfo::getSingleton().addTestLog("find user");
}

void HttpClient::updateBarrackInfo(BarrackInfo& info)
{
#if LOGIN_WITH_PLAYER_UID
    return;
#endif
    
	_RequestParam param;

	param.addParam("ctrl", 10);
	param.addParam("barrackid", info.barrackid);
	param.addParam("lasttime", info.lasttime);
	param.addParam("nums", (int)info.lstSoldier.size());
	param.addParam("curtime", info.curtime);

	int i = 0;
	for(std::list<BarrackSoldierInfo>::iterator it = info.lstSoldier.begin(); it != info.lstSoldier.end(); ++it)
	{
		char key0[128];
		char key1[128];

		sprintf(key0, "pid%d", i);
		sprintf(key1, "num%d", i);

		param.addParam(key0, it->pid);
		param.addParam(key1, (int)it->numsex);

		++i;
        
//		CCLOG("updateBarrackInfo %d %d %d %d", ct, info.barrackid, it->pid, (int)it->numsex);
	}
    
    CCLOG("updateBarrackInfo %d %d", info.barrackid, info.curtime);
    
	doHttpRequestPostEx("buildingctrl2.php", param, false, true, true);
    
	info.ischg = false;
}

void HttpClient::updateTroophousingInfo(TroophousingInfo& info)
{
#if LOGIN_WITH_PLAYER_UID
    return;
#endif
    
	_RequestParam param;

	param.addParam("ctrl", 11);
	param.addParam("troophousingid", info.troophousingid);
	param.addParam("maxnums", info.maxnumsex);
	param.addParam("nums", (int)info.lstSoldier.size());

	int i = 0;
	for(std::list<TroophousingSoldierInfo>::iterator it = info.lstSoldier.begin(); it != info.lstSoldier.end(); ++it)
	{
		char key0[128];
		char key1[128];

		sprintf(key0, "pid%d", i);
		sprintf(key1, "num%d", i);

		param.addParam(key0, it->pid);
		param.addParam(key1, (int)it->numsex);

		++i;
        
		CCLOG("updateTroophousingInfo %d %d %d", info.troophousingid, it->pid, (int)it->numsex);
	}
    
	doHttpRequestPostEx("buildingctrl2.php", param, false, true, true);
    
	info.ischg = false;
}

void HttpClient::updateUpgradeSoldier(SoldierUpgradeInfo& sui)
{
#if LOGIN_WITH_PLAYER_UID
    return;
#endif
    
	int ct = getCurrentTime();// time(NULL) + LogicInfo::getSingleton().m_offServiceTime;

	_RequestParam param;

	param.addParam("ctrl", 3);
	param.addParam("nums", sui.nums);
	param.addParam("offtime", sui.offtime);
	param.addParam("curtime", ct);
    
	for(int i = 0; i < sui.nums; ++i)
	{
		char key0[128];
		char key1[128];

		sprintf(key0, "soldier%d", i);
		sprintf(key1, "lasttime%d", i);

		param.addParam(key0, sui.soldierid[i]);
		param.addParam(key1, sui.lasttime[i]);
	}
    
	doHttpRequestPostEx("soldierupgradectrl.php", param, false, true, true);
    
	sui.ischg = false;
}

void HttpClient::subMoney(int m0, int m1, int m2)
{
#if LOGIN_WITH_PLAYER_UID
    return;
#endif
    
	_RequestParam param;

	param.addParam("ctrl", 12);
	param.addParam("m0", m0);
	param.addParam("m1", m1);
	param.addParam("m2", m2);
    
	doHttpRequestPostEx("buildingctrl2.php", param, false, true, true);
}

void HttpClient::getGold(int gem)
{
	_RequestParam param;

	param.addParam("ctrl", 13);
	param.addParam("gem", gem);
    
	doHttpRequestPostEx("buildingctrl2.php", param, false, true, true);
}

void HttpClient::getOil(int gem)
{
	_RequestParam param;

	param.addParam("ctrl", 14);
	param.addParam("gem", gem);
    
	doHttpRequestPostEx("buildingctrl2.php", param, false, true, true);
}

void HttpClient::addClanUseSoldier(_RequestParam &param)
{
    if (LogicInfo::getSingleton().m_clanSoldierUsed) {
        string str1;
        for(std::vector<int>::iterator it = ClanMgr::getSingleton().m_fightAttackSoldier.begin(); it != ClanMgr::getSingleton().m_fightAttackSoldier.end(); ++it)
        {
            if (it != ClanMgr::getSingleton().m_fightAttackSoldier.begin()) {
                str1 += ",";
            }
            
            char sid[128];
            sprintf(sid, "%d", (*it));
            
            str1 += sid;
        }
        param.addParam("atkclansoldier", str1.c_str());
    }
    
    string str2;
    for(std::vector<int>::iterator it = ClanMgr::getSingleton().m_fightDefendSoldier.begin(); it != ClanMgr::getSingleton().m_fightDefendSoldier.end(); ++it)
	{
        if (it != ClanMgr::getSingleton().m_fightDefendSoldier.begin()) {
            str2 += ",";
        }
        
        char sid[128];
        sprintf(sid, "%d", (*it));
        
        str2 += sid;
	}
    param.addParam("defclansoldier", str2.c_str());
}

void HttpClient::addBattleMap(_RequestParam &param)
{
    Json::Value map;
    
    int index = 0;
    int buildingNums = LogicInfo::getSingleton().m_otherBuilding.size();
    FightBuilding buildingList[buildingNums];
    
    for(std::list<UserBuildingInfo*>::iterator it = LogicInfo::getSingleton().m_otherBuilding.begin(); it != LogicInfo::getSingleton().m_otherBuilding.end(); ++it)
    {
        UserBuildingInfo* pBuildingInfo = *it;
    
        FightBuilding building;
        building.id = pBuildingInfo->id;
        building.bid = pBuildingInfo->buildingid;
        building.x = pBuildingInfo->x;
        building.y = pBuildingInfo->y;
        building.state = pBuildingInfo->state;
        building.hp = pBuildingInfo->hp;
        building.maxhp = pBuildingInfo->maxhp;
        building.store[0] = pBuildingInfo->store[0];
        building.store[1] = pBuildingInfo->store[1];
        
        buildingList[index++] = building;
    }
    
    int nodesize = sizeof(FightBuilding);
    int datasize = nodesize * buildingNums;
    unsigned char* mapdata = new unsigned char[datasize];
    memcpy(mapdata, &buildingList, datasize);
    
    unsigned long destlen = datasize*2;
    unsigned char* pDest = (unsigned char*)malloc(destlen);
    
    compress(pDest, &destlen, mapdata, datasize);
    
    string strdata = Base64::Encode(pDest, destlen);
        
    param.addParam("map", strdata.c_str());
        
    delete []mapdata;
    free(pDest);
}

void HttpClient::endBattle(int destuid)
{
	_RequestParam param;

	param.addParam("ctrl", 21);
	param.addParam("destuid", destuid);
	param.addParam("star", -1);
	param.addParam("result", "null");
	param.addParam("detail", "null");
    
	doHttpRequestPostEx("otherctrl2.php", param, false, true, true, true);
}

void HttpClient::addBattleLog(int destuid, int star, int offlevel, FightScoreInfoEx3& fsi, FightLog& nlog, WarParam& warparam, int usegems)
{
	_RequestParam param;

	param.addParam("ctrl", 21);
	param.addParam("destuid", destuid);
	param.addParam("star", star);
	param.addParam("offlevel", offlevel);
	param.addParam("wall", warparam.numsWall);
	param.addParam("townhall", warparam.numsTownhall);
	param.addParam("worker", warparam.numsWorker);
	param.addParam("mortar", warparam.numsMortar);
	param.addParam("gold", fsi.goldex);
	param.addParam("oil", fsi.oilex);
    param.addParam("storegold", (int)fsi.storegoldex);
    param.addParam("storeoil", (int)fsi.storeoilex);
	param.addParam("clientver", COC_ATTACK_VER);
	param.addParam("attackexp", HeroMgr::getSingleton().m_curAttackExp);
	param.addParam("usedgems", usegems);
	param.addParam("herohpoff", HeroMgr::getSingleton().m_curAttackHPOff);
    
    addBattleMap(param);
    addClanUseSoldier(param);
    
    //COC_ATTACK_VER_FIX3
	{
        unsigned char* pDest = (unsigned char*)malloc(sizeof(fsi) * 2);
        unsigned long destlen = sizeof(fsi) * 2;
        compress(pDest, &destlen, (unsigned char*)&fsi, sizeof(fsi));
        
        int len = EncodeLengthBase32(destlen);
        char* buff = new char[len + 1];
        
        param.addParam("result", EncodeBase32(pDest, destlen, buff));
        
        delete []buff;
        free(pDest);
	}
    
    //COC_ATTACK_VER_FIX4
    {
        unsigned char* pDest = (unsigned char*)malloc(sizeof(fsi)*2);
        unsigned long destlen = sizeof(fsi)*2;
        compress(pDest, &destlen, (unsigned char*)&fsi, sizeof(fsi));
        
        string strdata = Base64::Encode(pDest, destlen);
        
        param.addParam("result2", strdata.c_str());
        
        free(pDest);
    }
    
	{
		unsigned char* pSrc = nlog.get();
		unsigned char* pDest = (unsigned char*)malloc(nlog.size()*2);
		unsigned long destlen = nlog.size()*2;
		compress(pDest, &destlen, pSrc, nlog.size());

        string strdata = Base64::Encode(pDest, destlen);
        
		param.addParam("detail", strdata.c_str());

		delete []pSrc;
		free(pDest);
	}
    
	doHttpRequestPostEx("otherctrl2.php", param, false, true, true);
}

void HttpClient::addLeagueBattleLog(int destuid, int leagueid, int groupid, int star, int offlevel, FightScoreInfoEx3 &fsi, FightLog &nlog, WarParam &warparam, int usegems)
{
	_RequestParam param;

	param.addParam("ctrl", 3);
	param.addParam("destuid", destuid);
    param.addParam("leagueId", leagueid);
    param.addParam("groupId", groupid);
	param.addParam("star", star);
	param.addParam("offlevel", offlevel);
	param.addParam("wall", warparam.numsWall);
	param.addParam("townhall", warparam.numsTownhall);
	param.addParam("worker", warparam.numsWorker);
	param.addParam("mortar", warparam.numsMortar);
	param.addParam("gold", fsi.goldex);
	param.addParam("oil", fsi.oilex);
	param.addParam("clientver", COC_ATTACK_VER);
	param.addParam("attackexp", HeroMgr::getSingleton().m_curAttackExp);
	param.addParam("usedgems", usegems);
	param.addParam("herohpoff", HeroMgr::getSingleton().m_curAttackHPOff);
    
    addBattleMap(param);
    addClanUseSoldier(param);
    
    //COC_ATTACK_VER_FIX3
	{
        unsigned char* pDest = (unsigned char*)malloc(sizeof(fsi) * 2);
        unsigned long destlen = sizeof(fsi) * 2;
        compress(pDest, &destlen, (unsigned char*)&fsi, sizeof(fsi));
        
        int len = EncodeLengthBase32(destlen);
        char* buff = new char[len + 1];
        
        param.addParam("result", EncodeBase32(pDest, destlen, buff));
        
        delete []buff;
        free(pDest);
	}
    
    //COC_ATTACK_VER_FIX4
    {
        
        unsigned char* pDest = (unsigned char*)malloc(sizeof(fsi)*2);
        unsigned long destlen = sizeof(fsi)*2;
        compress(pDest, &destlen, (unsigned char*)&fsi, sizeof(fsi));
        
        string strdata = Base64::Encode(pDest, destlen);
        
        param.addParam("result2", strdata.c_str());
        
        free(pDest);
    }
    
	{
		unsigned char* pSrc = nlog.get();
		unsigned char* pDest = (unsigned char*)malloc(nlog.size() * 2);
		unsigned long destlen = nlog.size() * 2;
		compress(pDest, &destlen, pSrc, nlog.size());
        
        string strdata = Base64::Encode(pDest, destlen);
        
		param.addParam("detail", strdata.c_str());
        
		delete []pSrc;
		free(pDest);
	}
    
	doHttpRequestPostEx("ctrl_league.php", param, false, true, true);
}

void HttpClient::addEnvBattleLog(int stage, int star, FightScoreInfoEx3& fsi)
{
    EnvStageInfo* info = EnvMapMgr::getSingleton().getEnvStageInfo(stage);
    if (info->star > star) {
        star = info->star;
    }
    
	_RequestParam param;
    
	param.addParam("ctrl", 2);
	param.addParam("stage", stage);
	param.addParam("star", star);
	param.addParam("gold", fsi.goldex);
	param.addParam("oil", fsi.oilex);
	param.addParam("attackexp", HeroMgr::getSingleton().m_curAttackExp);
	param.addParam("herohpoff", HeroMgr::getSingleton().m_curAttackHPOff);
    
    addBattleMap(param);
    addClanUseSoldier(param);
    
    //   char param[128];
	//sprintf(param, "ctrl=2&stage=%d&star=%d&gold=%d&oil=%d&attackexp=%d&herohpoff=%d&clansoldier=%d", stage, star, fsi.gold, fsi.oil, HeroMgr::getSingleton().m_curAttackExp, HeroMgr::getSingleton().m_curAttackHPOff, LogicInfo::getSingleton().m_clanSoldierUsed);
    
	doHttpRequestPostEx("pvectrl.php", param, false, true, true);
}

static int logid = 1000;
void HttpClient::addClanWarBattleLog(int destuid, int star, int offlevel, FightScoreInfoEx3& fsi, FightLog& nlog, WarParam& warparam, int usegems)
{
    _RequestParam param;
    
	param.addParam("ctrl", 7);
    param.addParam("cityid", ClanWarMgr::getSingleton().m_warSignupCityId);
	param.addParam("destuid", destuid);
	param.addParam("star", star);
	param.addParam("offlevel", offlevel);
	param.addParam("wall", warparam.numsWall);
	param.addParam("townhall", warparam.numsTownhall);
	param.addParam("worker", warparam.numsWorker);
	param.addParam("mortar", warparam.numsMortar);
	param.addParam("gold", fsi.goldex);
	param.addParam("oil", fsi.oilex);
	param.addParam("clientver", COC_ATTACK_VER);
	param.addParam("attackexp", HeroMgr::getSingleton().m_curAttackExp);
	param.addParam("usedgems", usegems);
	param.addParam("herohpoff", HeroMgr::getSingleton().m_curAttackHPOff);
    
    addBattleMap(param);
    addClanUseSoldier(param);
    
    //COC_ATTACK_VER_FIX3
	{
        unsigned char* pDest = (unsigned char*)malloc(sizeof(fsi) * 2);
        unsigned long destlen = sizeof(fsi) * 2;
        compress(pDest, &destlen, (unsigned char*)&fsi, sizeof(fsi));
        
        int len = EncodeLengthBase32(destlen);
        char* buff = new char[len + 1];
        
        param.addParam("result", EncodeBase32(pDest, destlen, buff));
        
        delete []buff;
        free(pDest);
	}
    
    //COC_ATTACK_VER_FIX4
    {
        
        unsigned char* pDest = (unsigned char*)malloc(sizeof(fsi)*2);
        unsigned long destlen = sizeof(fsi)*2;
        compress(pDest, &destlen, (unsigned char*)&fsi, sizeof(fsi));
        
        string strdata = Base64::Encode(pDest, destlen);
        
        param.addParam("result2", strdata.c_str());
        
        free(pDest);
    }
    
	{
		unsigned char* pSrc = nlog.get();
		unsigned char* pDest = (unsigned char*)malloc(nlog.size() * 2);
		unsigned long destlen = nlog.size() * 2;
		compress(pDest, &destlen, pSrc, nlog.size());
        
        string strdata = Base64::Encode(pDest, destlen);
        
		param.addParam("detail", strdata.c_str());
        
		delete []pSrc;
		free(pDest);
	}
    
	doHttpRequestPostEx("ctrl_clanwar.php", param, false, true, true);
}

void HttpClient::addGroupWarBattleLog(int destuid, int star, int offlevel, FightScoreInfoEx3& fsi, FightLog& nlog, WarParam& warparam, int usegems)
{
    _RequestParam param;
    
	param.addParam("ctrl", 5);
    param.addParam("cityid", GroupWarMgr::getSingleton().m_pSelectCityInfo->cityid);
    param.addParam("defenselevel", GroupWarMgr::getSingleton().m_warUserInfo.defenseLevel);
    param.addParam("defenseratio", GroupWarMgr::getSingleton().m_warUserInfo.defenseRatio);
	param.addParam("destuid", destuid);
	param.addParam("star", star);
	param.addParam("offlevel", offlevel);
	param.addParam("wall", warparam.numsWall);
	param.addParam("townhall", warparam.numsTownhall);
	param.addParam("worker", warparam.numsWorker);
	param.addParam("mortar", warparam.numsMortar);
	param.addParam("gold", fsi.goldex);
	param.addParam("oil", fsi.oilex);
	param.addParam("clientver", COC_ATTACK_VER);
	param.addParam("attackexp", HeroMgr::getSingleton().m_curAttackExp);
	param.addParam("usedgems", usegems);
	param.addParam("herohpoff", HeroMgr::getSingleton().m_curAttackHPOff);
    
    addBattleMap(param);
    addClanUseSoldier(param);
    
    //COC_ATTACK_VER_FIX3
	{
        unsigned char* pDest = (unsigned char*)malloc(sizeof(fsi) * 2);
        unsigned long destlen = sizeof(fsi) * 2;
        compress(pDest, &destlen, (unsigned char*)&fsi, sizeof(fsi));
        
        int len = EncodeLengthBase32(destlen);
        char* buff = new char[len + 1];
        
        param.addParam("result", EncodeBase32(pDest, destlen, buff));
        
        delete []buff;
        free(pDest);
	}
    
    //COC_ATTACK_VER_FIX4
    {
        unsigned char* pDest = (unsigned char*)malloc(sizeof(fsi)*2);
        unsigned long destlen = sizeof(fsi)*2;
        compress(pDest, &destlen, (unsigned char*)&fsi, sizeof(fsi));
        
        string strdata = Base64::Encode(pDest, destlen);
        
        param.addParam("result2", strdata.c_str());
        
        free(pDest);
    }
    
	{
		unsigned char* pSrc = nlog.get();
		unsigned char* pDest = (unsigned char*)malloc(nlog.size() * 2);
		unsigned long destlen = nlog.size() * 2;
		compress(pDest, &destlen, pSrc, nlog.size());
        
        string strdata = Base64::Encode(pDest, destlen);
        
		param.addParam("detail", strdata.c_str());
        
		delete []pSrc;
		free(pDest);
	}
    
	doHttpRequestPostEx("ctrl_groupwar.php", param, false, true, true);
}

void HttpClient::endClanWarBattle(int destuid)
{
	_RequestParam param;
    
	param.addParam("ctrl", 7);
    param.addParam("cityid", ClanWarMgr::getSingleton().m_warSignupCityId);
	param.addParam("destuid", destuid);
	param.addParam("star", -1);
	param.addParam("result", "null");
	param.addParam("detail", "null");
    
	doHttpRequestPostEx("ctrl_clanwar.php", param, false, true, true, true);
}

void HttpClient::requestDefenseLog()
{
	_RequestParam param;
	param.addParam("ctrl", 2);
    
	doHttpRequestPostEx("otherctrl3.php", param, true, false, true, true);
}

void HttpClient::requestAttackLog(int logid)
{
    LogicInfo::getSingleton().m_isAttackLogLoad = true;

	_RequestParam param;

	param.addParam("ctrl", 20);
	param.addParam("logid", logid);
    
	doHttpRequestPostEx("otherctrl3.php", param, true, false, true, true);
}

void HttpClient::requestBattleLogDetail(int logid)
{
	_RequestParam param;

	param.addParam("ctrl", 3);
	param.addParam("logid", logid);
    
	doHttpRequestPostEx("otherctrl2.php", param, true, false, true, true);
}

void HttpClient::selectHero(int uid)
{
	_RequestParam param;

	param.addParam("ctrl", 1);
	param.addParam("heroid", uid);
    
	doHttpRequestPostEx("heroctrl2.php", param, false, true, true);
}

void HttpClient::selectSkill(int skillid, int pos)
{
	_RequestParam param;

	param.addParam("ctrl", 9);
	param.addParam("skillid", skillid);
	param.addParam("station", pos);
    
	doHttpRequestPostEx("heroctrl2.php", param, false, true, true);
    
    if(pos == 0)
        HeroMgr::getSingleton().m_myHero.skill0 = skillid;
    else if(pos == 1)
        HeroMgr::getSingleton().m_myHero.skill1 = skillid;
    else if(pos == 2)
        HeroMgr::getSingleton().m_myHero.skill2 = skillid;
}

void HttpClient::unselectSkill(int pos)
{
	_RequestParam param;

	param.addParam("ctrl", 9);
	param.addParam("skillid", 0);
	param.addParam("station", pos);
    
	doHttpRequestPostEx("heroctrl2.php", param, false, true, true);
    
    if(pos == 0)
        HeroMgr::getSingleton().m_myHero.skill0 = 0;
    else if(pos == 1)
        HeroMgr::getSingleton().m_myHero.skill1 = 0;
    else if(pos == 2)
        HeroMgr::getSingleton().m_myHero.skill2 = 0;
}

void HttpClient::upddateUserName(const char* name)
{
	std::string str = UrlEncode(name);

	_RequestParam param;

	param.addParam("ctrl", 4);
	param.addParam("uname", str.c_str());
    
	doHttpRequestPostEx("otherctrl2.php", param, false, true, true);
}

void HttpClient::addMoney(int m0, int m1, int m2)
{
#if LOGIN_WITH_PLAYER_UID
    return;
#endif
    
	if(m0 == 0 && m1 == 0 && m2 == 0)
		return;

	_RequestParam param;

	param.addParam("ctrl", 15);
	param.addParam("m0", m0);
	param.addParam("m1", m1);
	param.addParam("m2", m2);
    
	doHttpRequestPostEx("buildingctrl2.php", param, false, true, true);
}

void HttpClient::cancelLevelupBuilding(int buildingid, int x, int y)
{
#if LOGIN_WITH_PLAYER_UID
    return;
#endif
    
	x = x + 1;
	y = y + 1;

	_RequestParam param;

	param.addParam("ctrl", 16);
	param.addParam("x", x);
	param.addParam("y", y);
	param.addParam("bid", buildingid);
    
	doHttpRequestPostEx("buildingctrl3.php", param, false, true, true);
}

void HttpClient::requestAchievement()
{
    LogicInfo::getSingleton().m_vecAchievement.clear();

	_RequestParam param;

	param.addParam("ctrl", 17);
    
	doHttpRequestPostEx("buildingctrl2.php", param, false, false, true, true);
}

void HttpClient::requireAchievementAward(const char* str)
{
	_RequestParam param;

	param.addParam("ctrl", 1);
	param.addParam("achievement", str);
    
	doHttpRequestPostEx("achievementctrl.php", param, false, true, true);
}

void HttpClient::updUserKeys(const char* key0, const char* key1, const char* key2)
{

}

void HttpClient::chgExp(int leveloff, int exp)
{
	_RequestParam param;

	param.addParam("ctrl", 18);
	param.addParam("leveloff", leveloff);
	param.addParam("exp", exp);
    
	doHttpRequestPostEx("buildingctrl2.php", param, false, true, true);
}

void HttpClient::requestRank()
{
	_RequestParam param;

	param.addParam("ctrl", 1);
	param.addParam("type", "cup");
    
	doHttpRequestPostEx("rankctrl2.php", param, false, false, true, true);
}

void HttpClient::requestRevenge(int uid, int msgid)
{
	_RequestParam param;

	param.addParam("ctrl", 1);
	param.addParam("destuid", uid);
	param.addParam("msgid", msgid);
    
	doHttpRequestPostEx("queryuser.php", param, false, false, true);
}

void HttpClient::attackUserEx(int uid)
{
	_RequestParam param;

	param.addParam("ctrl", 7);
	param.addParam("destuid", uid);
    
	doHttpRequestPostEx("warctrl5.php", param, false, false, true);
    
    LogicInfo::getSingleton().addTestLog("find user");
}

void HttpClient::revengeUserEx(int uid, int msgid)
{
	_RequestParam param;

	param.addParam("ctrl", 6);
	param.addParam("destuid", uid);
	param.addParam("battleid", msgid);
    
	doHttpRequestPostEx("warctrl5.php", param, false, false, true);
}

void HttpClient::viewUserEx(int uid)
{
	_RequestParam param;

	param.addParam("ctrl", 4);
	param.addParam("destuid", uid);
    
	doHttpRequestPostEx("warctrl5.php", param, false, false, true);
}

void HttpClient::refurbishHero()
{
	_RequestParam param;

	param.addParam("ctrl", 3);
    
	doHttpRequestPostEx("heroctrl2.php", param, false, false, true);
}

void HttpClient::updateHeroHP(int off)
{
	int ct = getCurrentTime();// time(NULL) + LogicInfo::getSingleton().m_offServiceTime;

	_RequestParam param;

	param.addParam("ctrl", 2);
	param.addParam("hp", off);
	param.addParam("curtime", ct);
    
	doHttpRequestPostEx("heroctrl2.php", param, false, true, true);
}

void HttpClient::fastHeroHP(int offtime)
{
	_RequestParam param;

	param.addParam("ctrl", 4);
	param.addParam("offtime", offtime);
    
	doHttpRequestPostEx("heroctrl2.php", param, false, true, true);
}

void HttpClient::pay(int productid, const char* orderid)
{
	char strKey[1024];
	sprintf(strKey, "coc%d%s%s", productid, orderid, getServiceKey());
    
	MD5 md5Str(strKey);

	_RequestParam param;

	param.addParam("ctrl", 1);
	param.addParam("productid", productid);
	param.addParam("orderid", orderid);
	param.addParam("orderid", md5Str.toString().c_str());
    
	doHttpRequestPostEx("payctrl2.php", param, false, true, true);
}

void HttpClient::pay2(int productid, bool result, const char* receiptdata)
{
    std::string strBase64 = Base64::Encode((const unsigned char*)receiptdata, strlen(receiptdata));//"eyJyZWNlaXB0Ijp7InB1cmNoYXNlSW5mbyI6IntcIm9yZGVySWRcIjpcIjEyOTk5NzYzMTY5MDU0NzA1NzU4LjEzNjk4OTc0MDMwMDcyMzhcIixcInBhY2thZ2VOYW1lXCI6XCJjb20ua29vbGdpZy5nb2JcIixcInByb2R1Y3RJZFwiOlwiZ2Vtc18wMV90aXJlMl9nYW1lb2ZiYXR0bGVzMDEyMlwiLFwicHVyY2hhc2VUaW1lXCI6MTM5MjYxMzc5NzE0MixcInB1cmNoYXNlU3RhdGVcIjowLFwiZGV2ZWxvcGVyUGF5bG9hZFwiOlwiY2M1ZDc1MmNmNTJhNzc1YjRmNzQ5MmRkZjY0OWRlNjZcIixcInB1cmNoYXNlVG9rZW5cIjpcInZtd29rY2FvaWtnenVucnVudXR5c3l4eS5BTy1KMU96U1J2aTFLYS1WLS1MdjBaRXFEemdlNlBGWmtJOElzcUZzSERPMC1KZklYaEM2NTAwWmNZMmNFZ25rWTQzX0s3LXRqSGtfb1hfYmJ2WDhHS3BRS3gxQ1ZYU2FVbzMtWk1EOThVNWgxWnV2VERvVVpFOFRNdHJUYVVRVEV0bnMzOERJUXBiSFVtUmZzS1VUcXYzeDNaQk4xQnR1bUFcIn0iLCJzaWduYXR1cmUiOiJFNmdJS1hidytwd0lSalwvbGJNTnZGUWhnTkFRTVU1TzVwVEt1WDR2UWZFdkU5WExyRnVEWHdtOGxtc3pxZzNPYm1sd0Rhc2liV2s4SERTNE1mYTVIaFF6YjlrMWFuMlFoanFJQ0ZIUjZwUHNSWjRkZ2VsQWxnWjZEN1NMSUFIVGhYd042Smp2eWY2ZUIxXC9LQlVTVW13TUc3c1czMUZVKzd3cUt6K2xCeWtaT2pCZEZcL2hZbW91RkY3MmpPSTk2ajZra1oxajFibHRjSE1BK05NOTBqYTNZOTgyajI3WUtRVmx6MElHbmtweFJnekVNQUgrclpHRHVrQWxnaTBab043b0FJcU9xdU13Nnc0SDl1bmZYUHBzRVFyNEIrRTY0SEtpUEt2TWZwOWRkRnVWTE0zaUhiTFg0VlZKcDZ5bEpJSUZ1Q0hzRUtGUHRWNzlFRWRBWGxaT3c9PSJ9fQ==";//base64Encode((const unsigned char*)result, strlen(result));
    
	//char strKey[1024];
	//sprintf(strKey, "coc%d%s%s", productid, orderid, getServiceKey());
    
	////std::string strSrc = "coc";
	////strSrc += productid;
	////strSrc += orderid;
	////strSrc += getServiceKey();
    
	//CCLOG(strKey);
    
	//MD5 md5Str(strKey);

	CCLOG("%s", receiptdata);

	std::string ub = UrlEncode(CAppPlatform::sharedAppPlatform()->getBundleId().c_str());

	_RequestParam param;

	param.addParam("ctrl", 8);
	param.addParam("productid", productid);
	param.addParam("result", result);
	param.addParam("orderinfo", strBase64.c_str());
	param.addParam("boundid", ub.c_str());
#ifdef ECPURCHASE_TEST_SERVER
	param.addParam("sandbox", 1);
#else
	param.addParam("sandbox", 0);
#endif
    
//	char param[10240];
//    
//	std::string ub = UrlEncode(CAppPlatform::sharedAppPlatform()->getBundleId().c_str());
//    
//#ifdef ECPURCHASE_TEST_SERVER
//	sprintf(param, "ctrl=8&productid=%d&result=%d&orderinfo=%s&boundid=%s&sandbox=1", productid, result, strBase64.c_str(), ub.c_str());
//#else
//	sprintf(param, "ctrl=8&productid=%d&result=%d&orderinfo=%s&boundid=%s&sandbox=0", productid, result, strBase64.c_str(), ub.c_str());
//#endif //
    
	doHttpRequestPostEx("payctrl2.php", param, false, true, true);

}
void HttpClient::pay4(int productid, bool result, const char* receiptdata)
{
	std::string strBase64 = Base64::Encode((const unsigned char*)receiptdata, strlen(receiptdata));//"eyJyZWNlaXB0Ijp7InB1cmNoYXNlSW5mbyI6IntcIm9yZGVySWRcIjpcIjEyOTk5NzYzMTY5MDU0NzA1NzU4LjEzNjk4OTc0MDMwMDcyMzhcIixcInBhY2thZ2VOYW1lXCI6XCJjb20ua29vbGdpZy5nb2JcIixcInByb2R1Y3RJZFwiOlwiZ2Vtc18wMV90aXJlMl9nYW1lb2ZiYXR0bGVzMDEyMlwiLFwicHVyY2hhc2VUaW1lXCI6MTM5MjYxMzc5NzE0MixcInB1cmNoYXNlU3RhdGVcIjowLFwiZGV2ZWxvcGVyUGF5bG9hZFwiOlwiY2M1ZDc1MmNmNTJhNzc1YjRmNzQ5MmRkZjY0OWRlNjZcIixcInB1cmNoYXNlVG9rZW5cIjpcInZtd29rY2FvaWtnenVucnVudXR5c3l4eS5BTy1KMU96U1J2aTFLYS1WLS1MdjBaRXFEemdlNlBGWmtJOElzcUZzSERPMC1KZklYaEM2NTAwWmNZMmNFZ25rWTQzX0s3LXRqSGtfb1hfYmJ2WDhHS3BRS3gxQ1ZYU2FVbzMtWk1EOThVNWgxWnV2VERvVVpFOFRNdHJUYVVRVEV0bnMzOERJUXBiSFVtUmZzS1VUcXYzeDNaQk4xQnR1bUFcIn0iLCJzaWduYXR1cmUiOiJFNmdJS1hidytwd0lSalwvbGJNTnZGUWhnTkFRTVU1TzVwVEt1WDR2UWZFdkU5WExyRnVEWHdtOGxtc3pxZzNPYm1sd0Rhc2liV2s4SERTNE1mYTVIaFF6YjlrMWFuMlFoanFJQ0ZIUjZwUHNSWjRkZ2VsQWxnWjZEN1NMSUFIVGhYd042Smp2eWY2ZUIxXC9LQlVTVW13TUc3c1czMUZVKzd3cUt6K2xCeWtaT2pCZEZcL2hZbW91RkY3MmpPSTk2ajZra1oxajFibHRjSE1BK05NOTBqYTNZOTgyajI3WUtRVmx6MElHbmtweFJnekVNQUgrclpHRHVrQWxnaTBab043b0FJcU9xdU13Nnc0SDl1bmZYUHBzRVFyNEIrRTY0SEtpUEt2TWZwOWRkRnVWTE0zaUhiTFg0VlZKcDZ5bEpJSUZ1Q0hzRUtGUHRWNzlFRWRBWGxaT3c9PSJ9fQ==";//base64Encode((const unsigned char*)result, strlen(result));

		//char strKey[1024];
		//sprintf(strKey, "coc%d%s%s", productid, orderid, getServiceKey());

		////std::string strSrc = "coc";
		////strSrc += productid;
		////strSrc += orderid;
		////strSrc += getServiceKey();

		//CCLOG(strKey);

		//MD5 md5Str(strKey);

		CCLOG("amazon result %d %s",productid, receiptdata);

		std::string ub = UrlEncode(CAppPlatform::sharedAppPlatform()->getBundleId().c_str());

		_RequestParam param;

		param.addParam("ctrl", 10);
		param.addParam("productid", productid);
		param.addParam("result", result);
		param.addParam("orderinfo", strBase64.c_str());
		param.addParam("boundid", ub.c_str());
	#ifdef ECPURCHASE_TEST_SERVER
		param.addParam("sandbox", 1);
	#else
		param.addParam("sandbox", 0);
	#endif

	doHttpRequestPostEx("payctrl2.php", param, false, true, true);
}
void HttpClient::pay3(int productid, bool result, const char* receiptdata)
{
	//std::string strBase64 = base64Encode((const unsigned char*)result, strlen(result));
    
	//char strKey[1024];
	//sprintf(strKey, "coc%d%s%s", productid, orderid, getServiceKey());
    
	////std::string strSrc = "coc";
	////strSrc += productid;
	////strSrc += orderid;
	////strSrc += getServiceKey();
    
	//CCLOG(strKey);
    
	//MD5 md5Str(strKey);

	std::string ub = UrlEncode(CAppPlatform::sharedAppPlatform()->getBundleId().c_str());

	_RequestParam param;

	param.addParam("ctrl", 9);
	param.addParam("productid", productid);
	param.addParam("result", result);
	param.addParam("receiptdata", receiptdata);
	param.addParam("boundid", ub.c_str());
#ifdef ECPURCHASE_TEST_SERVER
	param.addParam("sandbox", 1);
#else
	param.addParam("sandbox", 0);
#endif
    
//	char param[10240];
//    
//	std::string ub = UrlEncode(CAppPlatform::sharedAppPlatform()->getBundleId().c_str());
//    
//#ifdef ECPURCHASE_TEST_SERVER
//	sprintf(param, "ctrl=9&productid=%d&result=%d&receiptdata=%s&boundid=%s&sandbox=1", productid, result, receiptdata, ub.c_str());
//#else
//	sprintf(param, "ctrl=9&productid=%d&result=%d&receiptdata=%s&boundid=%s&sandbox=0", productid, result,receiptdata, ub.c_str());
//#endif //
	//sprintf(param, "ctrl=3&productid=%d&receiptdata=%s", productid, receiptdata);
    
	doHttpRequestPostEx("payctrl2.php", param, false, true, true);
}

void HttpClient::addBug(const char* str, const char* email)
{
	//char param[10240];
    
	std::string bug1 = UrlEncode(str);
	std::string email1 = UrlEncode(email);

	char clientver[1024];
	sprintf(clientver, "%s %s", CAppPlatform::sharedAppPlatform()->getBundleId().c_str(), CAppPlatform::sharedAppPlatform()->getBundleVersion().c_str());

	std::string cv = UrlEncode(clientver);

	_RequestParam param;

	param.addParam("ctrl", 19);
	param.addParam("bug", bug1.c_str());
	param.addParam("email", email1.c_str());
	param.addParam("clientinfo", cv.c_str());
    
	//sprintf(param, "ctrl=19&bug=%s&email=%s&clientinfo=%s", bug1.c_str(), email1.c_str(), cv.c_str());
    
	doHttpRequestPostEx("otherctrl2.php", param, false, true, true, true);
}

void HttpClient::signin()
{
	_RequestParam param;

	param.addParam("ctrl", 1);
    
	doHttpRequestPostEx("signinctrl.php", param, false, false, true);
}
void HttpClient::requestFBShareLog()
{
    _RequestParam param;
    param.addParam("ctrl", 3);
	param.addParam("type", FacebookMgr::getSingleton().m_shareType);
    param.addParam("fbid", FacebookMgr::getSingleton().m_fbUser.fid.c_str());
    param.addParam("fbname", FacebookMgr::getSingleton().m_fbUser.fname.c_str());
    doHttpRequestPostEx("ctrl_facebook.php", param, false, false, true, true);
}
void HttpClient::requestAdFreeGemFlag(const char *str)
{
	_RequestParam param;

	param.addParam("ctrl", 1);
	param.addParam("fcode", str);
    
	doHttpRequestPostEx("ctrl_freegem.php", param, false, false, true);
}
void HttpClient::requestFBInviteFriends(const vector<string> friendIds)
{
    CCArray *array = CCArray::create();
    string fstring;
    for(int i = 0 ; i < friendIds.size(); i ++)
    {
        FBFriendInfo *finfo = FacebookMgr::getSingleton().getFriendById(friendIds[i]);
        if(finfo)
        {
            fstring = finfo->fname;
            CCString *str = CCString::create(fstring);
            array->addObject(str);
//            if(i < (friendIds.size()-1))
//                fstring += "㊣";
        }
        
    }
    
    Json::Value val;
    ArrayToJsonArray(array, val);
    
    string jsonStr = JsonValueToString(val);
    _RequestParam param;
    
	param.addParam("ctrl", 1);
	param.addParam("fbids", jsonStr.c_str());
    
	doHttpRequestPostEx("ctrl_facebook.php", param, false, false, true, true);
}

void HttpClient::requestFBAvailableFriends()
{
    CCArray *array = CCArray::create();
    string fnameString;
    for(int i = 0 ; i < FacebookMgr::getSingleton().m_fbFriendsList.size(); i ++)
    {
        FBFriendInfo *finfo = FacebookMgr::getSingleton().m_fbFriendsList[i];
        fnameString = finfo->fname;
        CCString *str = CCString::create(fnameString);
        array->addObject(str);
//        if(i < (FacebookMgr::getSingleton().m_fbFriendsList.size()-1))
//            fnameString += "㊣";//特殊分隔符
        
    }
    
    Json::Value val;
    ArrayToJsonArray(array, val);
    
    string jsonStr = JsonValueToString(val);
    
    _RequestParam param;
    
	param.addParam("ctrl", 2);
	param.addParam("fbids", jsonStr.c_str());
    
	doHttpRequestPostEx("ctrl_facebook.php", param, false, false, true, true);
}

void HttpClient::requestChartboostClickGems()
{
	_RequestParam param;

	param.addParam("ctrl", 2);
	param.addParam("fcode", "chartboost");
	param.addParam("gnum", LogicInfo::getSingleton().m_chartboostClickGem);

	doHttpRequestPostEx("ctrl_freegem.php", param, false, false, true);

}

void HttpClient::getIapClaim(int bid)
{
	_RequestParam param;

	param.addParam("ctrl", 2);
	param.addParam("product_id", bid);
    
	doHttpRequestPostEx("ctrl_cor.php", param, false, false, true);
}

void HttpClient::updateMilitaryRank()
{
	_RequestParam param;

	param.addParam("ctrl", 5);
	param.addParam("mrank", LogicInfo::getSingleton().m_myInfo.military);
    
	doHttpRequestPostEx("ctrl_league.php", param, false, false, true, true);
}

void HttpClient::requestSysNotify()
{
	_RequestParam param;

	param.addParam("ctrl", 1);
    
	doHttpRequestPostEx("sysnotifyctrl.php", param, false, false, true, true);
}

void HttpClient::requestEnvMap()
{
    LogicInfo::getSingleton().m_bEnvMapInit = false;

	_RequestParam param;

	param.addParam("ctrl", 1);
    
	doHttpRequestPostEx("pvectrl.php", param, false, false, true, true);
}

void HttpClient::requestAttackEnv(int stage)
{
	_RequestParam param;

	param.addParam("ctrl", 3);
	param.addParam("stage", stage);
    
	doHttpRequestPostEx("pvectrl.php", param, false, false, true);

}

void HttpClient::requestLeagueInfo()
{
	_RequestParam param;

	param.addParam("ctrl", 1);
    
	doHttpRequestPostEx("ctrl_league.php", param, false, false, true);
}

void HttpClient::findLeagueAttactUser()
{
	_RequestParam param;

	param.addParam("ctrl", 2);
    
	doHttpRequestPostEx("ctrl_league.php", param, false, false, true, false);
}

void HttpClient::getLeagueAward(int leagueid)
{
	_RequestParam param;

	param.addParam("ctrl", 4);
	param.addParam("awardId", leagueid);
    
 //   char param[128];
	//sprintf(param, "ctrl=4&awardId=%d",leagueid);
	doHttpRequestPostEx("ctrl_league.php", param, false, false, true);
}

void HttpClient::clearRequestList()
{
	for(std::list<_RequestNode*>::iterator it = m_queueRequest.begin(); it != m_queueRequest.end(); ++it)
	{
        (*it)->pRequest->setUserData(NULL);
		(*it)->pRequest->release();

		delete *it;
	}

	m_queueRequest.clear();
    
    m_pCurRequest = NULL;
    
    m_curClientState = HTTPCLIENT_STATE_WAIT;
}

void HttpClient::addAttackEnvVideo(int stage, int destuid, int star, int offlevel, FightScoreInfoEx3& fsi, FightLog& nlog, WarParam& warparam, int usegems)
{
	_RequestParam param;

	param.addParam("ctrl", 8);
	param.addParam("destuid", destuid);
	param.addParam("star", star);
	param.addParam("offlevel", offlevel);
	param.addParam("wall", warparam.numsWall);
	param.addParam("townhall", warparam.numsTownhall);
	param.addParam("worker", warparam.numsWorker);
	param.addParam("mortar", warparam.numsMortar);
	param.addParam("gold", fsi.goldex);
	param.addParam("oil", fsi.oilex);
	param.addParam("clientver", COC_ATTACK_VER);
	param.addParam("attackexp", HeroMgr::getSingleton().m_curAttackExp);
	param.addParam("usedgems", usegems);
	param.addParam("herohpoff", HeroMgr::getSingleton().m_curAttackHPOff);
	param.addParam("clansoldier", LogicInfo::getSingleton().m_clanSoldierUsed);
    
    addClanUseSoldier(param);
    addBattleMap(param);
    
//	{
//		unsigned char* pDest = (unsigned char*)malloc(sizeof(fsi) * 2);
//		unsigned long destlen = sizeof(fsi) * 2;
//		compress(pDest, &destlen, (unsigned char*)&fsi, sizeof(fsi));
//        
//		int len = EncodeLengthBase32(destlen);
//		char* buff = new char[len + 1];
//        
//		param.addParam("result", EncodeBase32(pDest, destlen, buff));
//        
//		delete []buff;
//		free(pDest);
//	}
//    
//	{
//		unsigned char* pSrc = nlog.get();
//		unsigned char* pDest = (unsigned char*)malloc(nlog.size() * 2);
//		unsigned long destlen = nlog.size() * 2;
//		compress(pDest, &destlen, pSrc, nlog.size());
//        
//		int len = EncodeLengthBase32(destlen);
//		char* buff = new char[len + 1];
//
//		param.addParam("detail", EncodeBase32(pDest, destlen, buff));
//        
//		delete pSrc;
//		delete buff;
//		free(pDest);
//	}
    
	doHttpRequestPostEx("pvectrl.php", param, false, true, true);
}

void HttpClient::requestAttackEnvVideo(int stage)
{
	_RequestParam param;

	param.addParam("ctrl", 9);
	param.addParam("stageid", stage);
    
 //   char param[128];
	//sprintf(param, "ctrl=9&stageid=%d", stage);
    
	doHttpRequestPostEx("pvectrl.php", param, false, false, true, false);
}

void HttpClient::addPVELife()
{
	_RequestParam param;

	param.addParam("ctrl", 7);
    
 //   char param[128];
	//sprintf(param, "ctrl=7");
    
	doHttpRequestPostEx("pvectrl.php", param, false, false, true);
}

void HttpClient::addPVELife(int life, int gems)
{
	_RequestParam param;

	param.addParam("ctrl", 6);
	param.addParam("nums", life);
	param.addParam("gems", gems);
    
 //   char param[128];
	//sprintf(param, "ctrl=6&nums=%d&gems=%d", life, gems);
    
	doHttpRequestPostEx("pvectrl.php", param, false, false, true);
}

void HttpClient::getUserState(int uid)
{
	_RequestParam param;

	param.addParam("ctrl", 1);
	param.addParam("destuid", uid);
    
    //char param[512];
    //sprintf(param, "ctrl=1&destuid=%d", uid);
    
	doHttpRequestPostEx("queryuser.php", param, false, true, true);
}

void HttpClient::requestMailInfo()
{
	_RequestParam param;

	param.addParam("ctrl", 3);
    
    //char param[512];
    //sprintf(param, "ctrl=3");
    
	doHttpRequestPostEx("mailctrl.php", param, false, false, true, false);
}

void HttpClient::getMailAward(int mailid)
{
	_RequestParam param;

	param.addParam("ctrl", 2);
	param.addParam("mailid", mailid);
    
    //char param[512];
    //sprintf(param, "ctrl=2&mailid=%d", mailid);
    
	doHttpRequestPostEx("mailctrl.php", param, false, true, true);
}

void HttpClient::doHttpRequestEx(const char* url, _RequestParam& param, bool bCanFail, bool isSave, bool isNeedCommonParam, bool bAsync, bool isupload)
{
    if (HttpClient::getSingleton().m_curClientState == HTTPCLIENT_STATE_PROCLAST && !isupload) {
        return;
    }

	std::string fullurl = m_urlroot;
	fullurl += url;

	std::string sparam;
	makeCommonParam2(sparam, param, isNeedCommonParam, bAsync, isupload);

	fullurl += "?";
	fullurl += sparam;

	CCHttpRequest* request = new CCHttpRequest();
	request->setUrl(fullurl.c_str());
    request->setRequestType(CCHttpRequest::kHttpGet);
    request->setResponseCallback(this, httpresponse_selector(HttpClient::onHttpRequestCompleted));
    request->setTag("GET coc");

	pushRequestEx(request, url, param, bCanFail, false, isSave, isNeedCommonParam, fullurl.c_str(), sparam.c_str(), bAsync);

    request->release();
}

void HttpClient::doHttpRequestPostEx(const char* url, _RequestParam& param, bool bCanFail, bool isSave, bool isNeedCommonParam, bool bAsync, bool isupload)
{
    if (HttpClient::getSingleton().m_curClientState == HTTPCLIENT_STATE_PROCLAST && !isupload) {
        return;
    }
    
	std::string fullurl = m_urlroot;
	fullurl += url;

	std::string sparam;

	makeCommonParam2(sparam, param, isNeedCommonParam, bAsync, isupload);

	CCHttpRequest* request = new CCHttpRequest();
	request->setUrl(fullurl.c_str());
    request->setRequestType(CCHttpRequest::kHttpPost);
    request->setResponseCallback(this, httpresponse_selector(HttpClient::onHttpRequestCompleted));
	request->setRequestData(sparam.c_str(), sparam.length());
    request->setTag("POST coc");

	pushRequestEx(request, url, param, bCanFail, true, isSave, isNeedCommonParam, fullurl.c_str(), sparam.c_str(), bAsync);

    request->release();
}

void HttpClient::pushRequestEx(CCHttpRequest* pRequest, const char* strURL, _RequestParam& param, bool bCanFail, bool isPost, bool isSave, bool isNeedCommonParam, const char* fullurl, const char* fullparam, bool bAsync)
{
	_RequestNode* pNode = new _RequestNode();
    
	pNode->bCanFail = bCanFail;
	pNode->failNums = 0;
	pNode->strRequest = strURL;
	pNode->pRequest = pRequest;
	pNode->param = param;
	pNode->bPost = isPost;
	pNode->bSave = isSave;
	pNode->bNeedCommonParam = isNeedCommonParam;
    pNode->bAsync = bAsync;
	pNode->fullurl = fullurl;
	pNode->fullparam = fullparam;
    
	pRequest->retain();
	pRequest->setUserData(pNode);
    
    AutoLock<LockImp> lock(m_lockRequest);
    
    if (bAsync) {
        
        sendRequest(pNode->pRequest);
    }
    else {
        m_queueRequest.push_back(pNode);
        
        if(pNode->bSave)
            save();
        
        if(m_pCurRequest == NULL)
        {
            _RequestNode* pNode = m_queueRequest.front();
            
            m_pCurRequest = pNode->pRequest;
            m_totalRequestTime = getCurrentTime();// time(NULL);
            
            sendRequest(m_pCurRequest);
        }
    }
}

void HttpClient::makeCommonParam2(std::string& sparam, _RequestParam& param, bool isNeedBaseParam, bool bAsync, bool isupload)
{
	if(isNeedBaseParam)
	{
		if(!isupload)
			param.addParam("uid", LogicInfo::getSingleton().m_myInfo.uid);

		param.addParam("sid", LogicInfo::getSingleton().m_myInfo.sessionid);
		param.addParam("handle", LogicInfo::getSingleton().m_myInfo.handle.c_str());
	}

	if(!isupload && !bAsync)
		param.addParam("msgnums", ++LogicInfo::getSingleton().m_myMsgNums);

#ifdef COC_HTTP_ZLIB
	param.addParam("zlib", 1);
#endif

	param.lst.sort();

	sparam.clear();
	for(std::list<_RequestParamNode>::iterator it = param.lst.begin(); it != param.lst.end(); ++it)
	{
		if(!sparam.empty())
			sparam += "&";

		sparam += it->key;
		sparam += "=";
		sparam += it->val;
	}

	std::string md5param = sparam;
	md5param += CUR_SERVER_KEY;
	MD5 md5(md5param.c_str(), md5param.length());
	sparam += "&sign=";
	sparam += md5.toString();
}

void HttpClient::requestChgGemOnly(int value)
{
    const char *mykey = "yqwdgdbckjsdbcywefgwifanbdYIQD";

    char tmp[512];
    int ct = (int)getCurrentTime();
    sprintf(tmp, "%d%d%d%s",value,ct,LogicInfo::getSingleton().m_myInfo.uid,mykey);
    
    std::string md5key =  MD5(std::string(tmp)).toString();
    
	_RequestParam param;

	param.addParam("ctrl", 1);
	param.addParam("gem", value);
	param.addParam("ct", ct);
    param.addParam("type", LogicInfo::getSingleton().m_addGemType);
	param.addParam("ckey", md5key.c_str());

    //char param[512];
    //sprintf(param, "ctrl=1&gem=%d&ct=%d&ckey=%s", value,ct,md5key.c_str());
    doHttpRequestPostEx("payotherctrl.php", param, false, true, true);
}

void HttpClient::sendRequest(CCHttpRequest *pRequest)
{
    if(LogicInfo::getSingleton().m_willPause || pRequest == NULL)
        return;
    
	_RequestNode* pNode = (_RequestNode*)pRequest->getUserData();
    
	if(pNode != NULL)
	{
		//CCLOG(">>>>>>>>>>>>>>%s", pRequest->getUrl());

		if (pRequest->getRequestType() == CCHttpRequest::kHttpPost) {
			string param = pNode->fullparam;
			//CCLOG(">>>>>>>>>>>>>>%s", param.c_str());
			if (getCurrentTime() > 0) {
				char time[128];
				sprintf(time, "&systime=%d", (int)getCurrentTime());
				param += time;
			}
			else
			{
				//char time[128];
				//sprintf(time, "&msgnums=%d", ++LogicInfo::getSingleton().m_myMsgNums);
				//param += time;
			}
			pRequest->setRequestData(param.c_str(), param.length());

	        CCLOG("%s %s", pRequest->getUrl(), param.c_str());
		}
		else
		{
			string url = pNode->fullurl;
			if (getCurrentTime() > 0) {
				char time[128];
				sprintf(time, "&systime=%d", (int)getCurrentTime());
				url += time;
			}
			else
			{
				//char time[128];
				//sprintf(time, "&msgnums=%d", ++LogicInfo::getSingleton().m_myMsgNums);
				//url += time;
			}
			pRequest->setUrl(url.c_str());

	        CCLOG("%s", url.c_str());
		}
        
        CCHttpClient::getInstance()->send(pRequest);
	}
}

void HttpClient::onRequestCompleted(CCHttpResponse* pResponse)
{
    CCHttpRequest* pRequest = pResponse->getHttpRequest();
    
    _RequestNode* pNode = (_RequestNode*)pRequest->getUserData();
    
    if(!pResponse->isSucceed())
	{
        ++m_numsRequestFail;
        
		AutoLock<LockImp> lock(m_lockRequest);
        
		if(!pNode->bCanFail)
		{
            string param(pRequest->getRequestData(), pRequest->getRequestDataSize());
            
			CCLOG("response failed -> %s %s", pRequest->getUrl(), param.c_str());
            
			if(m_numsRequestFail > MAX_COC_NETWORK_FAIL_NUMS)
			{
                m_urlroot = "";
                CCLOG("----- urlroot null2");
                m_numsRequestFail = 0;
                
				clearRequestList();
				LogicInfo::getSingleton().onNotify(NOTIFY_NETWORK_FAIL);
				return;
			}
            
			sendRequest(pRequest);
			return;
		}
		else
		{
            m_numsRequestFail = 0;
		}
	}
    
    if (pNode->bAsync) {
        pRequest->setUserData(NULL);
        pRequest->release();
        delete pNode;
        return;
    }
    
    nextRequest(pRequest);
}

void HttpClient::nextRequest(CCHttpRequest* pRequest)
{
    _RequestNode* pNode = (_RequestNode*)pRequest->getUserData();
    
    if (pRequest != m_pCurRequest || m_queueRequest.empty()) {
        pRequest->setUserData(NULL);
        pRequest->release();
        delete pNode;
        return;
    }
    
	AutoLock<LockImp> lock(m_lockRequest);
    
	if(m_queueRequest.empty())
		return;
    
    m_queueRequest.pop_front();
    
	bool isSave = pNode->bSave;
    
    pRequest->setUserData(NULL);
	pRequest->release();
	delete pNode;
    
	if(isSave)
		save();
    
	if(m_queueRequest.empty())
	{
		m_pCurRequest = NULL;
		return;
	}
    
	_RequestNode* pNode1 = m_queueRequest.front();
	m_pCurRequest = pNode1->pRequest;
    
	outputLog(m_pCurRequest->getUrl());
    
	m_totalRequestTime = getCurrentTime();// time(NULL);
    sendRequest(m_pCurRequest);
}

void HttpClient::outputLog(const char* str1)
{
	char str[1025];
	int len = strlen(str1);
	if(len > 1024)
	{
		for(int i = 0 ; i < 1024; ++i)
			str[i] = str1[i];
        
		str[1024] = '\0';
	}
	else
	{
		sprintf(str, "%s", str1);
		str[len] = '\0';
	}
    
	int len1 = strlen(str);
	for(int i = 0 ; i < len1; ++i)
	{
		if(str[i] == '%')
			str[i] = ' ';
	}
    
    CCLOG("%s", str);
}

void HttpClient::onHelloWorldRelease()
{
	if(m_pHelloWorld != NULL)
	{
		m_pHelloWorld->release();
        
		m_pHelloWorld = NULL;
	}
}

bool HttpClient::isNeedSaveParam(std::string& key)
{
	if("uid" == key)
		return true;
	if("sid" == key)
		return false;
	if("handle" == key)
		return false;
	if("sign" == key)
		return false;
	if("zlib" == key)
		return false;

	return true;
}

void HttpClient::save()
{
	if(m_curClientState == HTTPCLIENT_STATE_WAITLOGIN)
		return;

	AutoLock<LockImp> lock(m_lockRequest);

	if(HTTPCLIENT_STATE_WAITLOGINEX == m_curClientState)
	{
		float per = m_queueRequest.size() / (float)LogicInfo::getSingleton().m_maxUpdateNums;

		if(per < 0)
			per = 0;
		else if(per > 1)
			per = 1;

//		int iper = (1 - per) * 100;
//		char str[128];
//		sprintf(str, "update... %d%%", iper);

//		coc::LogicInfo::getSingleton().m_strLoadingInfo = str;
	}

	std::string path = CCFileUtils::sharedFileUtils()->getWritablePath();

	std::string name = path + "normal2-1.msg";
	int nums = 0;
	FILE* fp = fopen(name.c_str(), "wb");
	if(fp != NULL)
	{
		for(std::list<_RequestNode*>::iterator it = m_queueRequest.begin(); it != m_queueRequest.end(); ++it)
		{
			_RequestNode* pNode = *it;
			if(pNode->bSave)
			{
				int isPost = pNode->bPost ? 1 : 0;
				fwrite(&isPost, sizeof(int), 1, fp);

				int len = pNode->strRequest.length();
				fwrite(&len, sizeof(int), 1, fp);
				fwrite(pNode->strRequest.c_str(), len, 1, fp);

				int paramnums = 0;
				for(std::list<_RequestParamNode>::iterator it1 = pNode->param.lst.begin(); it1 != pNode->param.lst.end(); ++it1)
				{
					if(isNeedSaveParam(it1->key))
						++paramnums;
				}
				len = paramnums;
				fwrite(&len, sizeof(int), 1, fp);

				for(std::list<_RequestParamNode>::iterator it1 = pNode->param.lst.begin(); it1 != pNode->param.lst.end(); ++it1)
				{
					if(isNeedSaveParam(it1->key))
					{
						len = it1->key.size();
						fwrite(&len, sizeof(int), 1, fp);
						fwrite(it1->key.c_str(), len, 1, fp);


						len = it1->val.size();
						fwrite(&len, sizeof(int), 1, fp);
						fwrite(it1->val.c_str(), len, 1, fp);
					}
				}

				//len = it->strParam.length();
				//fwrite(&len, sizeof(int), 1, fp);
				//fwrite(it->strParam.c_str(), len, 1, fp);

				++nums;
			}
		}

		fclose(fp);
	}

	name = path + "normal2-0.msg";

	fp = fopen(name.c_str(), "wb");
	if(fp != NULL)
	{
		fwrite(&nums, sizeof(int), 1, fp);

		fclose(fp);
	}
}

bool HttpClient::load()
{
	LogicInfo::getSingleton().m_strLoadingInfo = "Connecting...";
    
	AutoLock<LockImp> lock(m_lockRequest);
    
	std::string path = CCFileUtils::sharedFileUtils()->getWritablePath();
    
	std::string name = path + "normal2-0.msg";
	int nums = 0;
	FILE* fp = fopen(name.c_str(), "rb");
	if(fp != NULL)
	{
		fread(&nums, sizeof(int), 1, fp);
        
		fclose(fp);
	}
    
	m_curClientState = HTTPCLIENT_STATE_INIT;
    
	if(nums > 0)
	{
        m_curClientState = HTTPCLIENT_STATE_PROCLAST;
		return true;
	}
    
	return false;
}

void HttpClient::onProcLoad()
{
	AutoLock<LockImp> lock(m_lockRequest);
    
	std::string path = CCFileUtils::sharedFileUtils()->getWritablePath();
    
	std::string name = path + "normal2-0.msg";
	int nums = 0;
	FILE* fp = fopen(name.c_str(), "rb");
	if(fp != NULL)
	{
		fread(&nums, sizeof(int), 1, fp);
        
		fclose(fp);
	}
    
	CCLOG("HttpClient::onProcLoad %d", nums);
    
	LogicInfo::getSingleton().m_maxUpdateNums = nums;
    
//	LogicInfo::getSingleton().m_strLoadingInfo = "Update... ";
    
	if(nums > 0)
	{
        m_curClientState = HTTPCLIENT_STATE_PROCLAST;
        
		name = path + "normal2-1.msg";
		fp = fopen(name.c_str(), "rb");
		if(fp != NULL)
		{
            char* str = (char*)malloc(2049);
			int slen = 2048;
			for(int i = 0; i < nums; ++i)
			{
				int isPost;
				fread(&isPost, sizeof(int), 1, fp);

				int len;
				fread(&len, sizeof(int), 1, fp);
				if(len + 1 > slen)
				{
					str = (char*)realloc(str, len + 1);
					slen = len + 1;
				}
				fread(str, len, 1, fp);
				str[len] = '\0';

				std::string url = str;

				_RequestParam param;
				fread(&len, sizeof(int), 1, fp);
				for(int i = 0; i < len; ++i)
				{
					int len1;
					fread(&len1, sizeof(int), 1, fp);
					if(len1 + 1 > slen)
					{
						str = (char*)realloc(str, len1 + 1);
						slen = len1 + 1;
					}
					fread(str, len1, 1, fp);
					str[len1] = '\0';

					std::string key = str;

					fread(&len1, sizeof(int), 1, fp);
					if(len1 + 1 > slen)
					{
						str = (char*)realloc(str, len1 + 1);
						slen = len1 + 1;
					}
					fread(str, len1, 1, fp);
					str[len1] = '\0';

					std::string val = str;

					param.addParam(key.c_str(), val.c_str());
				}

				if(param.getParamAsInt("uid") == LogicInfo::getSingleton().m_myInfo.uid)
				{
					if(isPost > 0)
						doHttpRequestPostEx(url.c_str(), param, false, true, true, true, true);
					else
						doHttpRequestEx(url.c_str(), param, false, true, true, true, true);
				}
			}
            
            free(str);
            
            m_curClientState = HTTPCLIENT_STATE_WAITLOGINEX;
            
			login();
		}
	}
}

void HttpClient::createClan(const char* clanname, const char* symbol, const char* description, int clantype, int mintrophies, int flag, int national)
{
	std::string strclanname = UrlEncode(clanname);
	std::string strsymbol = UrlEncode(symbol);
	std::string strdescription = UrlEncode(description);
    
	_RequestParam param;

	param.addParam("ctrl", 1);
	param.addParam("clanname", strclanname.c_str());
    param.addParam("national", national);
	param.addParam("symbol", strsymbol.c_str());
	param.addParam("description", strdescription.c_str());
	param.addParam("clantype", clantype);
	param.addParam("mintrophies", mintrophies);
	param.addParam("clanflag", flag);
    
	doHttpRequestPostEx("clanctrl2.php", param, false, false, true);
}

void HttpClient::editClan(ClanInfo clanInfo)
{
	std::string clanname = UrlEncode(clanInfo.name.c_str());
	std::string description = UrlEncode(clanInfo.description.c_str());

	_RequestParam param;

	param.addParam("ctrl", 13);
	param.addParam("clanid", clanInfo.id);
	param.addParam("clanname", clanname.c_str());
    param.addParam("national", clanInfo.national);
	param.addParam("description", description.c_str());
	param.addParam("clantype", clanInfo.jointype);
	param.addParam("mintrophies", clanInfo.needcupnums);
	param.addParam("clanflag", clanInfo.flag);
    
	doHttpRequestPostEx("clanctrl2.php", param, false, false, true);
}

void HttpClient::requestClanData(int aid)
{
	_RequestParam param;

	param.addParam("ctrl", 7);
	param.addParam("clanid", aid);
    
	doHttpRequestPostEx("clanctrl2.php", param, false, false, true, false);
}

void HttpClient::searchClan(const char* chat)
{
	std::string strpuid = UrlEncode(chat);

	_RequestParam param;

	param.addParam("ctrl", 15);
	param.addParam("name", strpuid.c_str());
    
	doHttpRequestPostEx("clanctrl2.php", param, false, false, true);
}

void HttpClient::getClanRanking()
{    
	_RequestParam param;

	param.addParam("ctrl", 12);
    
	doHttpRequestPostEx("clanctrl2.php", param, true, false, true);
}

void HttpClient::requestGetGem(int mask)
{
    _RequestParam param;
    param.addParam("ctrl", 2);
    param.addParam("mask", mask);
    
	doHttpRequestPostEx("payotherctrl.php", param, false, true, true);
}

void HttpClient::requestPaypalPurchase()
{
    _RequestParam param;
    param.addParam("ctrl", 11);
    
	doHttpRequestPostEx("payctrl2.php", param, false, true, true, true);
}

void HttpClient::requestMsgNums()
{
    if (m_urlroot.length() == 0) {
        return;
    }
    
    _RequestParam param;
    param.addParam("ctrl", 22);
    
    doHttpRequestPostEx("otherctrl2.php", param, false, false, true, true);
}

void HttpClient::requestClanWarInfo()
{
    _RequestParam param;
    param.addParam("ctrl", 1);
    
    doHttpRequestPostEx("ctrl_clanwar.php", param, false, false, true, true);
}

void HttpClient::clanWarSignup(int cityid)
{
    _RequestParam param;
    param.addParam("ctrl", 9);
    param.addParam("cityid", cityid);
    
    doHttpRequestPostEx("ctrl_clanwar.php", param, false, false, true, true);
}

void HttpClient::getClanWarSignList(int cityid)
{
    _RequestParam param;
    param.addParam("ctrl", 3);
    param.addParam("cityid", cityid);
    
    doHttpRequestPostEx("ctrl_clanwar.php", param, false, false, true, true);
}

void HttpClient::getClanWarBattleInfo(int cityid, int atkclanid)
{
    _RequestParam param;
    param.addParam("ctrl", 4);
    param.addParam("cityid", cityid);
    param.addParam("clanid", atkclanid);
    
    doHttpRequestPostEx("ctrl_clanwar.php", param, false, false, true, true);
}

void HttpClient::attackClanWar(int uid)
{
    _RequestParam param;
    
	param.addParam("ctrl", 5);
	param.addParam("targetuid", uid);
    
	doHttpRequestPostEx("ctrl_clanwar.php", param, false, false, true);
}

void HttpClient::getClanWarBattleLog(int cityid, int clanid)
{
    _RequestParam param;
    
	param.addParam("ctrl", 8);
	param.addParam("cityid", cityid);
    param.addParam("clanid", clanid);
    
	doHttpRequestPostEx("ctrl_clanwar.php", param, false, false, true, true);
}

void HttpClient::requestClanWarBattleLog(int logid)
{    
    _RequestParam param;
    
	param.addParam("ctrl", 6);
    param.addParam("battleid", logid);
    
	doHttpRequestPostEx("ctrl_clanwar.php", param, false, false, true, true);
}

void HttpClient::getClanWarRewardGems(int rewardid)
{
    _RequestParam param;
    
	param.addParam("ctrl", 11);
    param.addParam("rewardid", rewardid);
    
	doHttpRequestPostEx("ctrl_clanwar.php", param, false, true, true);
}

void HttpClient::checkClanWarAttack(int uid)
{
    _RequestParam param;
    
	param.addParam("ctrl", 12);
    param.addParam("targetuid", uid);
    
	doHttpRequestPostEx("ctrl_clanwar.php", param, false, true, true);
}

void HttpClient::attackGroupWar(int uid)
{
    _RequestParam param;
    
	param.addParam("ctrl", 4);
    param.addParam("cityid", GroupWarMgr::getSingleton().m_pSelectCityInfo->cityid);
	param.addParam("targetuid", uid);
    
	doHttpRequestPostEx("ctrl_groupwar.php", param, false, false, true);
}

void HttpClient::getGroupWarBattleInfo(int cityid, int atkclanid)
{
    _RequestParam param;
    param.addParam("ctrl", 3);
    param.addParam("cityid", cityid);
    param.addParam("clanid", atkclanid);
    
    doHttpRequestPostEx("ctrl_groupwar.php", param, false, false, true, true);
}

void HttpClient::getGroupWarBattleLog(int cityid, int clanid)
{
    _RequestParam param;
    
	param.addParam("ctrl", 7);
	param.addParam("cityid", cityid);
    param.addParam("clanid", clanid);
    
	doHttpRequestPostEx("ctrl_groupwar.php", param, false, false, true, true);
}

void HttpClient::getGroupWarRewardGems(int rewardid, int starrewardid)
{
    _RequestParam param;
    
	param.addParam("ctrl", 8);
    param.addParam("rewardid", rewardid);
    param.addParam("rewardid2", starrewardid);
    
	doHttpRequestPostEx("ctrl_groupwar.php", param, false, true, true);
}

void HttpClient::saveEdit(list<UserBuildingInfo*> lstBuilding)
{
    _RequestParam param;
	param.addParam("ctrl", 7);
    
    Json::Value mapval;
    for (list<UserBuildingInfo*>::iterator it = lstBuilding.begin(); it != lstBuilding.end(); ++it) {
        Json::Value building;
        building["id"] = (*it)->id;
        building["x"] = (*it)->x+1;
        building["y"] = (*it)->y+1;
        
        mapval.append(building);
    }
    
    string mapstr = JsonValueToString(mapval);
    
    int datasize = strlen(mapstr.c_str());
    unsigned long destlen = datasize*2;
    unsigned char* pDest = (unsigned char*)malloc(destlen);
    
    compress(pDest, &destlen, (unsigned char*)mapstr.c_str(), datasize);
    
    string strdata = Base64::Encode(pDest, destlen);
    
    param.addParam("map", strdata.c_str());
    
    free(pDest);
    
	doHttpRequestPostEx("toolctrl2.php", param, false, true, true);
}

void HttpClient::upgradeWalls(vector<Building_Wall*> lstUpgradeWall, int m0, int m1, int m2)
{
    _RequestParam param;
    
	param.addParam("ctrl", 19);
    param.addParam("m0", m0);
    param.addParam("m1", m1);
    param.addParam("m2", m2);
    
    char str[128];
    
    Json::Value val;
    for (vector<Building_Wall*>::iterator it = lstUpgradeWall.begin(); it != lstUpgradeWall.end(); ++it) {
        UserBuildingInfo* info = (*it)->m_pBuildingInfo;
        
        sprintf(str, "%d:%d", info->x + 1, info->y + 1);
        Json::Value valstr(str);
        
        val.append(valstr);
    }
    string jsonstr = JsonValueToString(val);
    param.addParam("walls", jsonstr.c_str());
    
	doHttpRequestPostEx("buildingctrl3.php", param, false, true, true);
}

NS_COC_END
