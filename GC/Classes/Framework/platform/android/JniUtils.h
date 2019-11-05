//
//  JniUtils.h
//  Framework
//
//  Created by qsun on 13-1-28.
//
//

#ifndef __Framework__JniUtils__
#define __Framework__JniUtils__

#include "NGNKit.h"
#include "platform/android/jni/JniHelper.h"
#include "cocos2d.h"

using namespace cocos2d;

class JniUtils {
public:

static jobjectArray jobjectArrayFromCCArray(JNIEnv *env, cocos2d::CCArray* pArray) {

    if (pArray == NULL) {
        return NULL;
    }

    jclass jStringCls = 0;

    jStringCls = env->FindClass("java/lang/String");

    jobjectArray objectArray;

    objectArray = env->NewObjectArray(pArray->count(), jStringCls, NULL);

    if (objectArray == NULL) {
        CCLog("failed to create a new jobjectArray");
        return NULL;
    }

    for (int i = 0; i < pArray->count(); i++) {

    		CCObject* obj = pArray->objectAtIndex(i);

        CCString* value = NULL;
        if(isKindOfClass(obj, cocos2d::CCDictionary))
        {
            value = cocos2d::CCString::create("Dictionary");
        }
        else if(isKindOfClass(obj, cocos2d::CCArray))
        {
            value = cocos2d::CCString::create("Array");
        }
        else if (isKindOfClass(obj, cocos2d::CCString))
        {
            value = (cocos2d::CCString*)obj;
        }
        else if (isKindOfClass(obj, cocos2d::CCInteger))
        {
            value = cocos2d::CCString::createWithFormat("%d", ((CCInteger*)obj)->getValue());
        }
        else
        {
            value = cocos2d::CCString::create("Unknown Object");
        }

        jstring objectString = env->NewStringUTF(value->getCString());

        env->SetObjectArrayElement(objectArray, i, objectString);

        env->DeleteLocalRef(objectString);

    }

    return objectArray;
}

static jobject hashMapFromCCDictionary(JNIEnv *env, cocos2d::CCDictionary * pDictionary)
{
	jclass mapClass = env->FindClass("java/util/HashMap");
	if(mapClass == NULL)
	{
	    return NULL;
	}

	jsize map_len = 1;

	jmethodID init =  env->GetMethodID(mapClass, "<init>", "(I)V");
	jobject hashMap = env->NewObject(mapClass, init, map_len);

	jmethodID put = env->GetMethodID(mapClass, "put",
	            "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

	CCDictElement *element = NULL;
	CCDICT_FOREACH(pDictionary, element)
	{
		const char* key = element->getStrKey();
		CCObject* obj = element->getObject();

		CCString* value = NULL;
		if(isKindOfClass(obj, cocos2d::CCDictionary))
		{
			value = cocos2d::CCString::create("Dictionary");
		}
		else if(isKindOfClass(obj, cocos2d::CCArray))
		{
			value = cocos2d::CCString::create("Array");
		}
		else if (isKindOfClass(obj, cocos2d::CCString))
		{
			value = (cocos2d::CCString*)obj;
		}
		else if (isKindOfClass(obj, cocos2d::CCInteger))
		{
			value = cocos2d::CCString::createWithFormat("%d", ((CCInteger*)obj)->getValue());
		}
		else
		{
			value = cocos2d::CCString::create("Unknown Object");
		}

		jstring keyString = env->NewStringUTF(key);
		jstring objectString = env->NewStringUTF(value->getCString());

		env->CallObjectMethod(hashMap, put, keyString, objectString);

		env->DeleteLocalRef(keyString);
		env->DeleteLocalRef(objectString);
	}

	env->DeleteLocalRef(mapClass);

	return hashMap;
}

};

#endif
