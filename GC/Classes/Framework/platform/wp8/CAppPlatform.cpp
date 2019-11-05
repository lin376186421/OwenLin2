#include "CAppPlatform.h"
#include "InPurchase.h"
#include "NGNKit.h"
#include "../coc/LogicInfo.h"


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

void CAppPlatform::displayFlurryAds(string pid)
{
}

void CAppPlatform::logFlurryEvent(const char* eventKey, CCDictionary* pDict)
{
	
}

void CAppPlatform::logMobileAppEvent(const char* purchaseID, float price)
{
}

void CAppPlatform::showTapJoyOfferswall()
{
    
}

void CAppPlatform::requestTapjoyConnect()
{
}

void CAppPlatform::setInputMode(int mode)
{
	
}

int CAppPlatform::getInputMode(int mode)
{
	int inputMode = mode;
	//switch (mode) {
	//	case kKeyboardTypeASCIICapable:

	//		break;
	//	default:
	//		break;
	//}
	inputMode = 1 | 32;
	return inputMode;
}

void CAppPlatform::purchasePayment(string productId, int payType){

	//JniMethodInfo minfo;

	//bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "paymentActivity","(Ljava/lang/String;)V");

	//if (isHave) {

	//	jobject obj = getNativeAppPlatform();

	//	jstring jstr = minfo.env->NewStringUTF(productId.c_str());

	//	minfo.env->CallVoidMethod(obj, minfo.methodID, jstr, NULL);
	//	minfo.env->DeleteLocalRef(jstr);
	//}
	std::string orderid;
	char str[1024];
	sprintf(str, "%d%d", coc::LogicInfo::getSingleton().m_localUID, time(NULL));
	orderid = str;
	//orderid += time(NULL);

	InPurchase::sharedInPurchase()->completedPurchase(true, orderid);
}

void CAppPlatform::bindDeviceUserId()
{
}

//string CAppPlatform::getDeviceUserName()
//{
//	//JniMethodInfo minfo;
//	//if (JniHelper::getMethodInfo(minfo, "com/mgame/common/AppPlatform", "getGmailName", "()Ljava/lang/String;"))
//	//{
//
//	//	jobject obj = getNativeAppPlatform();
//	//	jstring jstr = (jstring)minfo.env->CallObjectMethod(obj, minfo.methodID);
//
//	//	return JniHelper::jstring2string(jstr);
//	//}
//	return "";
//}
//
//void CAppPlatform::verifyLocalPlayer()
//{
//}

//extern "C"
//{
//
//JNIEXPORT void Java_com_mgame_common_AppPlatform_nativePaymentResult(JNIEnv*  env, jobject obj, jboolean success)
//{
//	InPurchase::sharedInPurchase()->completedPurchase(success);
//}
//
//JNIEXPORT void Java_com_mgame_common_AppPlatform_flurryDidFailToReceiveAd(JNIEnv*  env)
//{
////    NGNKit::sharedKit()->getFreeFailed();
//}
//
//JNIEXPORT void Java_com_mgame_common_AppPlatform_flurryDidReceiveAd(JNIEnv*  env)
//{
////    NGNKit::sharedKit()->getFreeSuccess();
//}
//
//JNIEXPORT void Java_com_mgame_common_AppPlatform_nativeLoadConfig(JNIEnv*  env)
//{
//	CCDictionary* pConfig = CCDictionary::create();
//	pConfig->setObject(CCString::createWithFormat("%d", APP_DEBUG), "APP_DEBUG");
//	pConfig->setObject(CCString::create(APP_GAME_NAME), "APP_GAME_NAME");
//	pConfig->setObject(CCString::create(APP_ID), "APP_ID");
//	pConfig->setObject(CCString::create(DEVICE_ID), "DEVICE_ID");
//
//	pConfig->setObject(CCString::create(FLURRY_API_KEY), "FLURRY_API_KEY");
//	pConfig->setObject(CCString::create(TAPJOY_APP_ID), "TAPJOY_APP_ID");
//	pConfig->setObject(CCString::create(TAPJOY_SECRET_KEY), "TAPJOY_SECRET_KEY");
//	pConfig->setObject(CCString::create(CHARTBOOST_APP_ID), "CHARTBOOST_APP_ID");
//	pConfig->setObject(CCString::create(CHARTBOOST_APP_SIGNATURE), "CHARTBOOST_APP_SIGNATURE");
//	pConfig->setObject(CCString::create(AARKI_APP_ID), "AARKI_APP_ID");
//	pConfig->setObject(CCString::create(AARKI_SECRET_KEY), "AARKI_SECRET_KEY");
//	pConfig->setObject(CCString::create(ADMOB_APP_ID), "ADMOB_APP_ID");
//	pConfig->setObject(CCString::create(INMOBI_APP_ID), "INMOBI_APP_ID");
//	pConfig->setObject(CCString::create(ADWHIRL_APP_KEY), "ADWHIRL_APP_KEY");
//
//	JniMethodInfo minfo;
//	if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/common/AppConfig", "initConfig", "(Ljava/util/HashMap;)V")) {
//		jobject params = JniUtils::hashMapFromCCDictionary(minfo.env, pConfig);
//
//		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, params);
//		minfo.env->DeleteLocalRef(params);
//		minfo.env->DeleteLocalRef(minfo.classID);
//	}
//}
//
//}

double CAppPlatform::getElapsedRealtime()
{
 //   JniMethodInfo minfo;
	//if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/common/AppPlatform" , "getRealtime", "()Ljava/lang/String;"))
	//{
 //       jstring retime = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
	//	minfo.env->DeleteLocalRef(minfo.classID);

 //       return strtod(JniHelper::jstring2string(retime).c_str(), NULL);
	//}

    return 0;
}

bool CAppPlatform::connectedToNet()
{
 //   JniMethodInfo minfo;
	//if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/common/AppPlatform", "connectedToNetwork"
 //                                      , "()Z"))
	//{
 //       jboolean connected = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
	//	minfo.env->DeleteLocalRef(minfo.classID);

 //       return (bool)connected;
	//}

    return 0;
}

void CAppPlatform::fireNotification(int delay, string message)
{
 //   JniMethodInfo minfo;
	//if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/common/AppPlatform", "fireNotification"
 //                               , "(ILjava/lang/String;)V"))
	//{
 //       jstring jstr = minfo.env->NewStringUTF(message.c_str());

 //       minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, delay, jstr);

	//	minfo.env->DeleteLocalRef(jstr);
	//	minfo.env->DeleteLocalRef(minfo.classID);

	//}
}

void CAppPlatform::cancelAllNotifications()
{
 //   JniMethodInfo minfo;
	//if (JniHelper::getStaticMethodInfo(minfo , "com/mgame/common/AppPlatform", "cancelAllNotifications"
 //                               , "()V"))
	//{
 //       minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
	//	minfo.env->DeleteLocalRef(minfo.classID);

	//}
}

void CAppPlatform::updateNewVersion(string newUrl)
{
	//ShellExecuteA(NULL, "open", newUrl.c_str(), NULL, NULL, 0);
}

void CAppPlatform::rateApp()
{
}

string CAppPlatform::getBundleId()
{
	std::string bid = "wincoc";

	return bid;
}

string CAppPlatform::getBundleVersion()
{
	std::string bid = "1.33";

	return bid;
}

string CAppPlatform::getAppName()
{
	std::string bid = "coc";

	return bid;
}

void CAppPlatform::showGameCenterLeaderboard(string category)
{
}

void CAppPlatform::showFlurryVideoAds()
{
}

void CAppPlatform::showChartBoostAds()
{
}

void CAppPlatform::consumePurchase(string productId)
{
}

void CAppPlatform::purchasesResume()
{
}

void CAppPlatform::getAarkiPoint()
{
}

void CAppPlatform::showAarkiOfferAds()
{
}

void CAppPlatform::showSSAOfferAds()
{
    
}

void CAppPlatform::getSSAPoint()
{
    
}

void CAppPlatform::getMoregame()
{
}

void CAppPlatform::showAnnouncement(AnnouncementUI *announcementUI)
{
}

void CAppPlatform::hideAnnouncement()
{
}