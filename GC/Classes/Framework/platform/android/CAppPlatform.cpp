#include "CAppPlatform.h"
#include "platform/android/jni/JniHelper.h"
#include "InPurchase.h"
#include "NGNKit.h"
#include "JniUtils.h"
#include "PlayerScene.h"
#include "../../../form/Shop.h"
#include "../../../GameWorldScene.h"
#include "CFacebookService.h"


jobject getNativeAppPlatform()
{
	jobject nativeAppPlatform = NULL;
	JniMethodInfo minfo;
	if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/common/AppPlatform", "getAppPlatform", "()Lcom/mgame/common/AppPlatform;"))
	{
		nativeAppPlatform = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
    
	return nativeAppPlatform;
}

static CAppPlatform* s_pSharedAppPlatform = NULL;
CAppPlatform* CAppPlatform::sharedAppPlatform()
{
    if (s_pSharedAppPlatform == NULL) {
        s_pSharedAppPlatform = new CAppPlatform();
    }
    return s_pSharedAppPlatform;
}

void CAppPlatform::initPlatform()
{

}


void CAppPlatform::getScreenShot()
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    CCRenderTexture *screen = CCRenderTexture::create(size.width, size.height);
    CCScene *scene = CCDirector::sharedDirector()->getRunningScene();
    screen->begin();
    scene->visit();
    screen->end();
    screen->saveToFile(ScreenShotFile, kCCImageFormatPNG);
}
void CAppPlatform::showAnnouncement()
{
	JniMethodInfo minfo;
	bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "showWebView","(Ljava/lang/String;IIIIII)V");
	if (isHave) {
		AnnouncementUI *pAnnouncementUI = PlayerScene::getSingleton().m_pGameWorld->m_pAnnouncementUI;
		CCNode *webNode = pAnnouncementUI->m_pWebNode;
		jobject obj = getNativeAppPlatform();
		CCPoint webPos = webNode->getPosition();
		CCSize size = webNode->getContentSize();
		CCDirector *dir = CCDirector::sharedDirector();

		CCSize dirSize = dir->getWinSize();
		int width = (int)size.width;
		int height = (int)size.height;
		int xPos = (int)(webPos.x-size.width/2);
		int yPos = (int)(dirSize.height - webPos.y -size.height/2);

		jstring webURL = minfo.env->NewStringUTF(LogicInfo::getSingleton().m_webURL.c_str());
		minfo.env->CallVoidMethod(obj, minfo.methodID,webURL, xPos,yPos,width,height,(int)(dirSize.width),(int)(dirSize.height));
		minfo.env->DeleteLocalRef(webURL);
	}
}

void CAppPlatform::webViewDidFinishLoad(int state)
{

	if(PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pAnnouncementUI == NULL) {
		return;
	}

	AnnouncementUI *pAnnouncementUI = PlayerScene::getSingleton().m_pGameWorld->m_pAnnouncementUI;

	if(pAnnouncementUI == NULL)
	        return;

	if(state == 1)
	{
		CCNode *pLoading = pAnnouncementUI->m_pLoading;
		if(pLoading && pLoading->isVisible())
		{
			pLoading->setVisible(false);
		}
	}
	else if(state == 0)
	{
		pAnnouncementUI->menuBackCallback(NULL, CCControlStateNormal);
		LogicInfo::getSingleton().onNotify(NOTIFY_NETWORK_FAIL);
	}
}

void CAppPlatform::hideAnnouncement()
{
	JniMethodInfo minfo;
	bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "hideWebView","()V");
	if (isHave) {
		jobject obj = getNativeAppPlatform();
		minfo.env->CallVoidMethod(obj, minfo.methodID);
	}
}

void CAppPlatform::displayFlurryAds(string pid)
{
	JniMethodInfo minfo;
    
	bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "displayFlurryAds","(Ljava/lang/String;)V");
	if (isHave) {
        
		jobject obj = getNativeAppPlatform();
        
		jstring pidStr = minfo.env->NewStringUTF(pid.c_str());
        
		minfo.env->CallVoidMethod(obj, minfo.methodID, pidStr, NULL);
		minfo.env->DeleteLocalRef(pidStr);
		CCLog("Flurry init successed!");
	}
}

void CAppPlatform::logFlurryEvent(const char* eventKey, CCDictionary* pDict)
{
	JniMethodInfo minfo;
    
	bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "logFlurryEvent","(Ljava/lang/String;Ljava/util/HashMap;)V");
    
	if (isHave) {
        
		jobject obj = getNativeAppPlatform();
        
		jobject key = minfo.env->NewStringUTF(eventKey);
		jobject params = JniUtils::hashMapFromCCDictionary(minfo.env, pDict);
        
		minfo.env->CallVoidMethod(obj, minfo.methodID, key, params, NULL);
		minfo.env->DeleteLocalRef(key);
		minfo.env->DeleteLocalRef(params);
	}
}

void CAppPlatform::logMobileAppEvent(const char* productId, float price)
{
	JniMethodInfo minfo;
    
	bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "logMobileAppEvent","(Ljava/lang/String;F)V");
    
	if (isHave) {
        
		jobject obj = getNativeAppPlatform();
        
		jobject productKey = minfo.env->NewStringUTF(productId);
        
		minfo.env->CallVoidMethod(obj, minfo.methodID, productKey, price, NULL);
		minfo.env->DeleteLocalRef(productKey);
	}
}

void CAppPlatform::showNativeXOfferAds(const char* placementStr)
{
	if(strcmp(placementStr, NATIVE_X_GAMELAUNCH) != 0)
	{
		if(PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pMainUI)
		{
			PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->showAdsLoadingUI();
		}
	}
	JniMethodInfo minfo;
	    bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "showNativeXOfferAds","(Ljava/lang/String;)V");
	    if (isHave) {
	        jobject obj = getNativeAppPlatform();
	        jstring jstr = minfo.env->NewStringUTF(placementStr);
	        minfo.env->CallVoidMethod(obj, minfo.methodID,jstr, NULL);
	        minfo.env->DeleteLocalRef(jstr);
	    }
}
void CAppPlatform::getNativeXPoint()
{
	JniMethodInfo minfo;
	    bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "getNativeXPoint","()V");
	    if (isHave) {
	        jobject obj = getNativeAppPlatform();
	        minfo.env->CallVoidMethod(obj, minfo.methodID);
	    }
}
void CAppPlatform::showFlurryVideoAds()
{
	JniMethodInfo minfo;
    bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "showFlurryVideoAds","()V");
    if (isHave) {
        jobject obj = getNativeAppPlatform();
        minfo.env->CallVoidMethod(obj, minfo.methodID);
    }
}

void CAppPlatform::showChartBoostAds()
{
	JniMethodInfo minfo;
	bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "showChartBoostAds","()V");
	if (isHave) {
		jobject obj = getNativeAppPlatform();
		minfo.env->CallVoidMethod(obj, minfo.methodID);
	}
}

void CAppPlatform::getChartBoostClickGems()
{
	if (LogicInfo::getSingleton().m_isChartboostFreeGemReady && PlayerScene::getSingleton().m_pGameWorld->m_pShop)
	    {
	        if(PlayerScene::getSingleton().m_pGameWorld->m_pShop->m_shopId == SHOP_TYPE_FREEGEMS)
	        {
	            LogicInfo::getSingleton().requestChartboostClickGems();
	            LogicInfo::getSingleton().m_isChartboostFreeGemReady  = false;
	            LogicInfo::getSingleton().m_hasChartboostFreeGem      = true;
	            PlayerScene::getSingleton().m_pGameWorld->m_pShop->refreshShopItem(3);
	        }
	    }
}

void CAppPlatform::requestTapjoyConnect()
{
	JniMethodInfo minfo;
    bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "requestTapjoyConnect","()V");
    if (isHave) {
        jobject obj = getNativeAppPlatform();
        minfo.env->CallVoidMethod(obj, minfo.methodID);
    }
}
void CAppPlatform::getTapJopPoint()
{
	JniMethodInfo minfo;
	    bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "tapjoyGetUpdatePoints","()V");
	    if (isHave) {
	        jobject obj = getNativeAppPlatform();
	        minfo.env->CallVoidMethod(obj, minfo.methodID);
	    }
}

void CAppPlatform::showTapJoyOfferswall()
{
	JniMethodInfo minfo;
    bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "showTapJoyOfferswall","()V");
    if (isHave) {
        jobject obj = getNativeAppPlatform();
        minfo.env->CallVoidMethod(obj, minfo.methodID);
    }
}

void CAppPlatform::getAarkiPoint()
{
    JniMethodInfo minfo;
    bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "getAarkiPoint","()V");
    if (isHave) {
        jobject obj = getNativeAppPlatform();
        minfo.env->CallVoidMethod(obj, minfo.methodID);
    }
}


void CAppPlatform::showAarkiOfferAds()
{
    JniMethodInfo minfo;
    bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "showAarkiOfferAds","()V");
    if (isHave) {
        jobject obj = getNativeAppPlatform();
        minfo.env->CallVoidMethod(obj, minfo.methodID);
    }
}

void CAppPlatform::showSSAOfferAds()
{
    JniMethodInfo minfo;
    bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "showSSAOfferAds","()V");
    if (isHave) {
        jobject obj = getNativeAppPlatform();
        minfo.env->CallVoidMethod(obj, minfo.methodID);
    }
}
void CAppPlatform::getSSAPoint()
{
    HttpClient::getSingleton().requestSupersonicadsData();
}

void CAppPlatform::getMoregame()
{
	JniMethodInfo minfo;
	if (JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "getMoregame", "()V"))
	{
		jobject obj = getNativeAppPlatform();
		minfo.env->CallVoidMethod(obj, minfo.methodID);
	}
}

void CAppPlatform::setInputMode(int mode)
{
	JniMethodInfo minfo;
	if (JniHelper::getStaticMethodInfo(minfo, "org/cocos2dx/lib/Cocos2dxGLSurfaceView", "setInputType", "(I)V"))
	{
        
		int inputMode = getInputMode(mode);
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, inputMode);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
}

int CAppPlatform::getInputMode(int mode)
{
	int inputMode = mode;
    //	switch (mode) {
    //		case kKeyboardTypeASCIICapable:
    //
    //			break;
    //		default:
    //			break;
    //	}
	inputMode = 1 | 32;
	return inputMode;
}

void CAppPlatform::purchasePayment(string productId, int payType){
    
	JniMethodInfo minfo;
	bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "paymentPurchase","(Ljava/lang/String;I)V");
    
	if (isHave) {
		jobject obj = getNativeAppPlatform();
        
		jstring jstr = minfo.env->NewStringUTF(productId.c_str());
        
		minfo.env->CallVoidMethod(obj, minfo.methodID, jstr, payType, NULL);
		minfo.env->DeleteLocalRef(jstr);
	}
}

void CAppPlatform::purchasesResume()
{
	JniMethodInfo minfo;
	bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "resumePurchase","()V");
    
	if (isHave) {
		jobject obj = getNativeAppPlatform();
		minfo.env->CallVoidMethod(obj, minfo.methodID, NULL);
	}
}

void CAppPlatform::consumePurchase(string productId){
	JniMethodInfo minfo;
	bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "consumePurchase","(Ljava/lang/String;)V");
    
	if (isHave) {
		jobject obj = getNativeAppPlatform();
        
		jstring jstr = minfo.env->NewStringUTF(productId.c_str());
        
		minfo.env->CallVoidMethod(obj, minfo.methodID, jstr, NULL);
		minfo.env->DeleteLocalRef(jstr);
	}
}

void CAppPlatform::bindDeviceUserId()
{

	if(CFacebookService::sharedFaceBook()->fbBindActivity == false)
	{

		CCLog("bindDeviceUserId");
		JniMethodInfo minfo;
		if (JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "getGmailName", "()Ljava/lang/String;"))
		{

			jobject obj = getNativeAppPlatform();
			jstring jstr = (jstring)minfo.env->CallObjectMethod(obj, minfo.methodID);
			string playerId = JniHelper::jstring2string(jstr);

			string userName = "";
			if (playerId.length() > 0) {
				userName = playerId.substr(0, playerId.rfind("@"));
			}

			CCLog("user name %s", userName.c_str());

			coc::LogicInfo::getSingleton().m_bindType = BIND_SYSTEM_TYPE;
			verifyLocalPlayer(playerId, userName);
		}
	}
}

string CAppPlatform::getBundleId()
{
	JniMethodInfo minfo;
	if (JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "getPackageName", "()Ljava/lang/String;"))
	{
		jobject obj = getNativeAppPlatform();
		jstring jstr = (jstring)minfo.env->CallObjectMethod(obj, minfo.methodID, NULL);
        
		return JniHelper::jstring2string(jstr);
	}
	return "";
}

string CAppPlatform::getBundleVersion()
{
	JniMethodInfo minfo;
	if (JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "getPackageVersion", "()Ljava/lang/String;"))
	{
        
		jobject obj = getNativeAppPlatform();
		jstring jstr = (jstring)minfo.env->CallObjectMethod(obj, minfo.methodID);
        
		return JniHelper::jstring2string(jstr);
	}
	return "";
}

string CAppPlatform::getAppName()
{
	JniMethodInfo minfo;
	if (JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "getAppName", "()Ljava/lang/String;"))
	{

		jobject obj = getNativeAppPlatform();
		jstring jstr = (jstring)minfo.env->CallObjectMethod(obj, minfo.methodID);

		return JniHelper::jstring2string(jstr);
	}
	return "";
}

void CAppPlatform::updateNewVersion(string message, string newUrl)
{
	JniMethodInfo minfo;
	if (JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "updateNewVersion", "(Ljava/lang/String;Ljava/lang/String;)V"))
	{
		jobject obj = getNativeAppPlatform();
		jstring jstrurl = minfo.env->NewStringUTF(newUrl.c_str());
		jstring jstrmsg = minfo.env->NewStringUTF(message.c_str());

		minfo.env->CallVoidMethod(obj, minfo.methodID, jstrmsg, jstrurl, NULL);
	}
}

void CAppPlatform::rateApp()
{
	int rateCount = CCUserDefault::sharedUserDefault()->getIntegerForKey(RATER_ALERT_COUNT);
    
	if (rateCount < RATER_ALERT_MAX_COUNT) {
        
		JniMethodInfo minfo;
		if (JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "rateApp", "()V"))
		{
			jobject obj = getNativeAppPlatform();
			minfo.env->CallVoidMethod(obj, minfo.methodID);
		}
		CCUserDefault::sharedUserDefault()->setIntegerForKey(RATER_ALERT_COUNT, rateCount++);
	}
}

double CAppPlatform::getElapsedRealtime()
{
    JniMethodInfo minfo;
	if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/common/AppPlatform" , "getRealtime", "()Ljava/lang/String;"))
	{
        jstring retime = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
        
        return strtod(JniHelper::jstring2string(retime).c_str(), NULL);
	}
    
    return 0;
}

bool CAppPlatform::connectedToNet()
{
    JniMethodInfo minfo;
	if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/common/AppPlatform", "connectedToNetwork"
                                       , "()Z"))
	{
        jboolean connected = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
        
        return (bool)connected;
	}
    
    return 0;
}

void CAppPlatform::fireNotification(int delay, string message)
{
    JniMethodInfo minfo;
	if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/common/AppPlatform", "fireNotification"
                                       , "(ILjava/lang/String;)V"))
	{
        jstring jstr = minfo.env->NewStringUTF(message.c_str());
        
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, delay, jstr);
        
		minfo.env->DeleteLocalRef(jstr);
		minfo.env->DeleteLocalRef(minfo.classID);
        
	}
}
void CAppPlatform::openGooglePlusLink()
{
    CCUserDefault::sharedUserDefault()->setBoolForKey(GP_FREE_GEMS, true);
    CCUserDefault::sharedUserDefault()->flush();

    openUrl("https://plus.google.com/communities/102982482049792636737");
}
void CAppPlatform::cancelAllNotifications()
{
    JniMethodInfo minfo;
	if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/common/AppPlatform", "cancelAllNotifications"
                                       , "()V"))
	{
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
        
	}
}

void CAppPlatform::openUrl(const char *url)
{
    JniMethodInfo minfo;
	if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/common/AppPlatform", "openUrl", "(Ljava/lang/String;)V"))
	{
        jstring jstr = minfo.env->NewStringUTF(url);

        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jstr);

		minfo.env->DeleteLocalRef(jstr);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
}

void CAppPlatform::exit()
{
	JniMethodInfo minfo;
    if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/common/AppPlatform", "exit", "()V"))
    {
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
        
    }
}

//jni native function
extern "C"
{


	JNIEXPORT void Java_com_mgame_common_AppPlatform_nativeWebViewDidFinishLoad(JNIEnv* env,jobject obj,jint state)
	{
		CAppPlatform::sharedAppPlatform()->webViewDidFinishLoad(state);
	}

	JNIEXPORT void Java_com_mgame_common_AppPlatform_nativeNativeXDidFinishLoad(JNIEnv* env,jobject obj,jint state)
	{
		if(state == 0)
		{
			coc::LogicInfo::getSingleton().onNotify(NOTIFY_ADS_LOADINF_FAILD);
		}
		else
		{
			if(PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pMainUI)
			{
				PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->hideAdsLoadingUI();
			}
		}
	}
	JNIEXPORT void Java_com_mgame_common_AppPlatform_nativeGetSSAPoint(JNIEnv* env,jobject obj)
	{
		CAppPlatform::sharedAppPlatform()->getSSAPoint();
	}

	JNIEXPORT void Java_com_mgame_common_AppPlatform_nativeGetAdsGemsForNativeX(JNIEnv* env, jobject obj ,jobject amountList)
    {
		jclass cls_id = env->GetObjectClass(amountList);

		jmethodID getMethodID_id = (env)->GetMethodID(cls_id, "get", "(I)Ljava/lang/Object;");

		jmethodID sizeMethodID = (env)->GetMethodID(cls_id, "size", "()I");
		int size = (env)->CallIntMethod(amountList, sizeMethodID);

		int count = 0;
		for (int i = 0; i < size; i++)
		{
			jstring jamount = (jstring)(env)->CallObjectMethod(amountList, getMethodID_id, i);
			string amount = JniHelper::jstring2string(jamount);
			count += atoi(amount.c_str());
		}
		if(count > 0)
		{
			LogicInfo::getSingleton().m_pendingNativeXGem += count;
			CCLOG("aaaaaaaaaaaa");
			if(PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pMainUI)
			{
				CCLOG("aaaaaaaaaaaatttttttttttt");
				LogicInfo::getSingleton().m_addGemType = ADD_GEM_NATIVE_X;
				LogicInfo::getSingleton().requestAdsGem(count);
			}
		}
	}

    JNIEXPORT void Java_com_mgame_common_AppPlatform_nativeGetAdsFreeGems(JNIEnv* env, jobject obj, jint val, jint type)
    {
    	coc::LogicInfo::getSingleton().m_addGemType = type;
        coc::LogicInfo::getSingleton().requestAdsGem(val);
    }

    JNIEXPORT void Java_com_mgame_common_AppPlatform_nativeGetChartBoostClickGem(JNIEnv* env, jobject obj)
    {
        CAppPlatform::sharedAppPlatform()->getChartBoostClickGems();
    }
    
    JNIEXPORT void Java_com_mgame_common_AppPlatform_nativePaymentResult(JNIEnv* env, jobject obj, jstring product, jboolean success, jstring info)
    {
        string productId = JniHelper::jstring2string(product);
        string purchaseInfo = JniHelper::jstring2string(info);
        
        InPurchase::sharedInPurchase()->productId = productId;
        
        InPurchase::sharedInPurchase()->completedPurchase(success, purchaseInfo);
        
    }
    
    JNIEXPORT void Java_com_mgame_common_AppPlatform_nativeRateAppOption(JNIEnv* env, jobject obj, jboolean rate)
    {
        CAppPlatform::sharedAppPlatform()->rateAppOption(rate);
    }
    
    JNIEXPORT void Java_com_mgame_common_AppPlatform_nativeRegisterServerPush(JNIEnv* env, jobject obj, jstring registerId)
    {
        string regId = JniHelper::jstring2string(registerId);
        CAppPlatform::sharedAppPlatform()->registerServerPush(regId);
    }
    
    JNIEXPORT void Java_com_mgame_common_AppPlatform_nativeLoadConfig(JNIEnv* env)
    {
        CCDictionary* pConfig = CCDictionary::create();
        string appDebug = CCStringFormat("%d",APP_DEBUG);
		pConfig->setObject(CCString::create(appDebug), "APP_DEBUG");
        pConfig->setObject(CCString::create(APP_GAME_NAME), "APP_GAME_NAME");
        pConfig->setObject(CCString::create(APP_ID), "APP_ID");
        pConfig->setObject(CCString::create(DEVICE_ID), "DEVICE_ID");
        pConfig->setObject(CCString::create(APP_STORE_URL), "APP_STORE_URL");
        pConfig->setObject(CCString::create(SERVER_URL), "SERVER_URL");
        pConfig->setObject(CCString::create(WEB_URL), "WEB_URL");
        pConfig->setObject(CCString::create(FLURRY_API_KEY), "FLURRY_API_KEY");
        pConfig->setObject(CCString::create(FLURRY_AD_SPACE), "FLURRY_AD_SPACE");
        pConfig->setObject(CCString::create(TAPJOY_APP_ID), "TAPJOY_APP_ID");
        pConfig->setObject(CCString::create(TAPJOY_SECRET_KEY), "TAPJOY_SECRET_KEY");
        pConfig->setObject(CCString::create(CHARTBOOST_APP_ID), "CHARTBOOST_APP_ID");
        pConfig->setObject(CCString::create(CHARTBOOST_APP_SIGNATURE), "CHARTBOOST_APP_SIGNATURE");
        pConfig->setObject(CCString::create(AARKI_APP_ID), "AARKI_APP_ID");
        pConfig->setObject(CCString::create(AARKI_SECRET_KEY), "AARKI_SECRET_KEY");
        pConfig->setObject(CCString::create(ADMOB_APP_ID), "ADMOB_APP_ID");
        pConfig->setObject(CCString::create(INMOBI_APP_ID), "INMOBI_APP_ID");
        pConfig->setObject(CCString::create(ADWHIRL_APP_KEY), "ADWHIRL_APP_KEY");
        pConfig->setObject(CCString::create(MDOTM_APP_KEY), "MDOTM_APP_KEY");
        pConfig->setObject(CCString::create(MOBILE_APP_ID), "MOBILE_APP_ID");
        pConfig->setObject(CCString::create(MOBILE_APP_KEY), "MOBILE_APP_KEY");
        pConfig->setObject(CCString::create(SSA_APP_KEY), "SSA_APP_KEY");
        pConfig->setObject(CCString::create(NATIVE_X_KEY), "NATIVE_X_KEY");
        pConfig->setObject(CCString::create(GETUI_APP_ID), "GETUI_APP_ID");
        pConfig->setObject(CCString::create(GETUI_APP_KEY), "GETUI_APP_KEY");
        pConfig->setObject(CCString::create(GETUI_APP_SECRET), "GETUI_APP_SECRET");

        pConfig->setObject(CCString::create(PURCHASE_KEY_1), "PURCHASE_KEY_1");
		pConfig->setObject(CCString::create(PURCHASE_KEY_2), "PURCHASE_KEY_2");
		pConfig->setObject(CCString::create(PURCHASE_KEY_3), "PURCHASE_KEY_3");
		pConfig->setObject(CCString::create(PURCHASE_KEY_4), "PURCHASE_KEY_4");
		pConfig->setObject(CCString::create(PURCHASE_KEY_5), "PURCHASE_KEY_5");
		pConfig->setObject(CCString::create(PURCHASE_KEY_6), "PURCHASE_KEY_6");

        JniMethodInfo minfo;
        if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/common/AppConfig", "initConfig", "(Ljava/util/HashMap;)V")) {
            jobject params = JniUtils::hashMapFromCCDictionary(minfo.env, pConfig);
            
            minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, params);
            minfo.env->DeleteLocalRef(params);
            minfo.env->DeleteLocalRef(minfo.classID);
        }
        
        CCLog("config load end!");
    }
    
}

