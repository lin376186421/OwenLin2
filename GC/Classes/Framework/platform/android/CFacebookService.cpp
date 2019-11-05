//
//  FacebookService.cpp
//  coc
//
//  Created by qsun on 13-11-10.
//
//

#include "CFacebookService.h"
#include "CAppPlatform.h"
#include "../coc/LogicInfo.h"

static CFacebookService* s_pCFacebookService = NULL;

CFacebookService* CFacebookService::sharedFaceBook()
{
    if (s_pCFacebookService == NULL) {
        s_pCFacebookService = new CFacebookService();
    }
    return s_pCFacebookService;
}

CFacebookService::~CFacebookService()
{
    //    [flurryAdDelegate release];
}

jobject getNativeFacebookPlatform()
{
	jobject nativeFacebookPlatform = NULL;
	JniMethodInfo minfo;
	if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/facebook/FacebookMgr", "getFacebookPlatform", "()Lcom/mgame/facebook/FacebookMgr;"))
	{
		nativeFacebookPlatform = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
	}

	return nativeFacebookPlatform;
}

void CFacebookService::init()
{
    this->setFBBindActivity(false);
}

void CFacebookService::setFBBindActivity(bool activity)
{
	if(!FacebookMgr::getSingleton().isOpenState())
	{
		fbBindActivity = false;
		LogicInfo::getSingleton().m_bindType = BIND_SYSTEM_TYPE;

		if(fbBindActivity)
				CCLog("setFBBindActivity---true");
			else
				CCLog("setFBBindActivity---false");
		return;
	}
	fbBindActivity = activity;
	if(fbBindActivity)
		CCLog("setFBBindActivity---true");
	else
		CCLog("setFBBindActivity---false");
	if(fbBindActivity)
		LogicInfo::getSingleton().m_bindType = BIND_FACEBOOK_TYPE;
	else
		LogicInfo::getSingleton().m_bindType = BIND_SYSTEM_TYPE;
}

void CFacebookService::fbInviteFriends(vector<string> friendIds)
{
	string friendStr;
    for(int i = 0; i < friendIds.size(); i++)
    {
		friendStr += friendIds[i];
		if(i < (friendIds.size() -1))
		{
			friendStr += ",";
		}
    }
    JniMethodInfo minfo;

	bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/facebook/FacebookMgr", "fbStartInvite","(Ljava/lang/String;)V");
	if (isHave) {
		jobject obj = getNativeFacebookPlatform();
		jstring fidsString = minfo.env->NewStringUTF(friendStr.c_str());
		minfo.env->CallVoidMethod(obj, minfo.methodID, fidsString, NULL);
		minfo.env->DeleteLocalRef(fidsString);
	}
}

void CFacebookService::fbSetFriends(vector<coc::FBFriendInfo*> friendsList)
{
	coc::FacebookMgr::getSingleton().fbSetFriends(friendsList);
}

void CFacebookService::fbSetUser(string fid, string fname)
{
	CCLog("fbSetUser %s---%s",fid.c_str(),fname.c_str());
    coc::FBUserInfo fbUser;
    fbUser.fid = fid;
    fbUser.fname = fname;
    coc::FacebookMgr::getSingleton().fbSetUser(fbUser);
}

void CFacebookService::fbGetFriends()
{
	JniMethodInfo minfo;
	bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/facebook/FacebookMgr", "fbApiGraphFriends","()V");
	if (isHave) {
		jobject obj = getNativeFacebookPlatform();
		minfo.env->CallVoidMethod(obj, minfo.methodID);
	}
}
void CFacebookService::fbSDKInviteFriends()
{
	JniMethodInfo minfo;
	bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/facebook/FacebookMgr", "fbApiGraphFriends","()V");
	if (isHave) {
		jobject obj = getNativeFacebookPlatform();
		minfo.env->CallVoidMethod(obj, minfo.methodID);
	}

}

void CFacebookService::fbGetInvitableFriends()
{

}

void CFacebookService::fbLogin()
{
	JniMethodInfo minfo;
	bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/facebook/FacebookMgr", "fbLogin","()V");
	if (isHave) {
		jobject obj = getNativeFacebookPlatform();
		minfo.env->CallVoidMethod(obj, minfo.methodID);
	}
}

void CFacebookService::fbLoginOut()
{
	JniMethodInfo minfo;
	bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/facebook/FacebookMgr", "fbLoginOut","()V");
	if (isHave) {
		jobject obj = getNativeFacebookPlatform();
		minfo.env->CallVoidMethod(obj, minfo.methodID);
	}
}

void CFacebookService::fbPostMessage(const string message)
{
	JniMethodInfo minfo;

	bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/facebook/FacebookMgr", "fbPostMessage","(Ljava/lang/String;)V");
	if (isHave) {
		jobject obj = getNativeFacebookPlatform();
		jstring messageStr = minfo.env->NewStringUTF(message.c_str());
		minfo.env->CallVoidMethod(obj, minfo.methodID, messageStr, NULL);
		minfo.env->DeleteLocalRef(messageStr);
	}
}

void CFacebookService::fbPostMessageWithPhoto(const string message)
{
	CAppPlatform::sharedAppPlatform()->getScreenShot();
	JniMethodInfo minfo;

	bool isHave = JniHelper::getMethodInfo(minfo, "com/mgame/facebook/FacebookMgr", "fbPostMessageWithPhoto","(Ljava/lang/String;)V");
	if (isHave) {
		jobject obj = getNativeFacebookPlatform();
		jstring messageStr = minfo.env->NewStringUTF(message.c_str());
		minfo.env->CallVoidMethod(obj, minfo.methodID, messageStr, NULL);
		minfo.env->DeleteLocalRef(messageStr);
	}
}

void CFacebookService::fbRequestFinish(int curApiCall, int state)
{
	 FacebookMgr::getSingleton().fbRequestFinish(curApiCall, state);
}

extern "C"
{

	JNIEXPORT void Java_com_mgame_facebook_FacebookMgr_nativeFBBindActivity(JNIEnv* env, jobject obj, jboolean activity)
    {
		CFacebookService::sharedFaceBook()->setFBBindActivity(activity);
    }

	JNIEXPORT void Java_com_mgame_facebook_FacebookMgr_nativeFBLoginOutSucceed(JNIEnv* env, jobject obj)
    {
		CFacebookService::sharedFaceBook()->fbBindActivity = false;
		coc::FacebookMgr::getSingleton().fbLoginOut();
    }
	JNIEXPORT void Java_com_mgame_facebook_FacebookMgr_nativeFBGetFriendFinish(JNIEnv*  env, jobject obj)
	{
		coc::FacebookMgr::getSingleton().fbGetFBFriendsFinish();
	}
	JNIEXPORT void Java_com_mgame_facebook_FacebookMgr_nativeFBSetUser(JNIEnv* env, jobject obj, jstring fbId, jstring fbName)
    {
		string uid = JniHelper::jstring2string(fbId);
		string uname = JniHelper::jstring2string(fbName);
		CFacebookService::sharedFaceBook()->fbSetUser(uid,uname);
    }

	JNIEXPORT void Java_com_mgame_facebook_FacebookMgr_nativeFBSetFriends(JNIEnv* env, jobject obj, jobject friendIds, jobject friendNames, jobject friendUrls)
	{
		jclass cls_id = env->GetObjectClass(friendIds);
		jclass cls_name = env->GetObjectClass(friendNames);
		jclass cls_url = env->GetObjectClass(friendUrls);

		jmethodID getMethodID_id = env->GetMethodID(cls_id, "get", "(I)Ljava/lang/Object;");
		jmethodID getMethodID_name = env->GetMethodID(cls_name, "get", "(I)Ljava/lang/Object;");
		jmethodID getMethodID_url = env->GetMethodID(cls_url, "get", "(I)Ljava/lang/Object;");

		jmethodID sizeMethodID = (env)->GetMethodID(cls_id, "size", "()I");
		int size = (env)->CallIntMethod(friendIds, sizeMethodID);

		std::vector<coc::FBFriendInfo*> friendsList;
		for (int i = 0; i < size; i++)
		{
			 jstring jfid = (jstring)env->CallObjectMethod(friendIds, getMethodID_id, i);
			 jstring jfname = (jstring)env->CallObjectMethod(friendNames, getMethodID_name, i);
			 jstring jurl = (jstring)env->CallObjectMethod(friendUrls, getMethodID_url, i);
			 string fid = JniHelper::jstring2string(jfid);
			 string fname = JniHelper::jstring2string(jfname);
			 string furl = JniHelper::jstring2string(jurl);
			 FBFriendInfo *fuser = new FBFriendInfo();
			 fuser->fid = fid;
			 fuser->fname = fname;
			 fuser->fphoto = furl;
			 fuser->fSpriteFrame = NULL;
			 fuser->isSelect = false;
			 friendsList.push_back(fuser);
		}
		CFacebookService::sharedFaceBook()->fbSetFriends(friendsList);
	}

	JNIEXPORT void Java_com_mgame_facebook_FacebookMgr_nativeFBRequestResult(JNIEnv* env, jobject obj, jboolean result, jint callType)
    {
		int state;
		if(result)
			state = 1;
		else
			state = 0;
		CFacebookService::sharedFaceBook()->fbRequestFinish(callType,state);
    }

}
