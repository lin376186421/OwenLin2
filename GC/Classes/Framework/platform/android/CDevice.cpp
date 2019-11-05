//
//  CCDevice.cpp
//  RobinHood
//
//  Created by qsun on 13-1-4.
//
//

#include "CDevice.h"
#include <stdlib.h>
#include "platform/android/jni/JniHelper.h"
#include "md5.h"

using namespace std;
using namespace cocos2d;

static CDevice* s_pSharedDevice = NULL;
CDevice* CDevice::sharedDevice()
{
	if (s_pSharedDevice == NULL) {
		s_pSharedDevice = new CDevice();
		s_pSharedDevice->init();
	}
	return s_pSharedDevice;
}

void CDevice::init()
{
	macAddress_ = "";
	uniqueIdentifier_ = "";
}

string CDevice::macAddress() {

	if (macAddress_ == "") {
		JniMethodInfo minfo;
		if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/common/DeviceInfo", "getMacAddress", "()Ljava/lang/String;")) {

			jstring jstr = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
			macAddress_ = JniHelper::jstring2string(jstr);
			minfo.env->DeleteLocalRef(minfo.classID);
		}
	}

	return macAddress_;
}

string CDevice::md5MacAddress() {
	string macaddress = macAddress();
	string md5Address = MD5(macaddress).toString();

	return md5Address;
}

string CDevice::getUDID()
{
	JniMethodInfo minfo;
	if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/common/DeviceInfo", "getDeviceId", "()Ljava/lang/String;")) {

		jstring jstr = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		string deviceId = JniHelper::jstring2string(jstr);
		minfo.env->DeleteLocalRef(minfo.classID);
		return deviceId;
	}
}

string CDevice::uniqueDeviceIdentifier() {

	if (uniqueIdentifier_ == "") {

		string udid = getUDID();
		uniqueIdentifier_ = MD5(udid).toString();
	}

	return uniqueIdentifier_;
}

string CDevice::getDeviceModel()
{
	string deviceModel = "";

	JniMethodInfo minfo;
	if (JniHelper::getStaticMethodInfo(minfo, "com/mgame/common/DeviceInfo", "getDeviceModel", "()Ljava/lang/String;")) {

		jstring jstr = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		deviceModel = JniHelper::jstring2string(jstr);
		minfo.env->DeleteLocalRef(jstr);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
    
	return deviceModel;
}

int CDevice::getDeviceFamily()
{
	return -1;
}

std::string CDevice::getDeviceZone()
{
    
}
std::string CDevice::getIDFA()
{
    return uniqueDeviceIdentifier();
}
