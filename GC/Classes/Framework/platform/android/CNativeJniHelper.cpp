/*
 * CNativeJniHelper.cpp
 *
 *  Created on: 2014-1-16
 *      Author: qsun
 */

#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"
#include "AppDelegate.h"

//jni native function
extern "C"
{

JNIEXPORT void Java_com_mgame_common_AppActivity_nativeApplicationWillTerminate(JNIEnv*  env)
{
	((AppDelegate*)(cocos2d::CCApplication::sharedApplication()))->applicationWillTerminate();
}

}
