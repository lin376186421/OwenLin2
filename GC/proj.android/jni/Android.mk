LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := game_shared

LOCAL_MODULE_FILENAME := libgame

MY_FILES := $(wildcard $(LOCAL_PATH)/../../Classes/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../Classes/coc/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../Classes/form/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../Classes/zephyr/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../Classes/zephyr/net/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../Classes/Global/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../Classes/Framework/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../Classes/Framework/platform/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../Classes/Framework/platform/android/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../Classes/Utils/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../Classes/Utils/*.c) \
$(wildcard $(LOCAL_PATH)/../../Classes/Utils/jsoncpp/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../Classes/Utils/jsoncpp/*.inl) \
$(wildcard $(LOCAL_PATH)/../../Classes/Utils/network/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../Classes/Utils/network/http/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../Classes/Utils/network/scoket/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../Classes/Utils/PngPacketDecode/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../Classes/extensions/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../Classes/extensions/android/*.cpp)
MY_FILES := $(MY_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := hellocpp/main.cpp
LOCAL_SRC_FILES += $(MY_FILES)
     
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
$(LOCAL_PATH)/../../Classes/coc \
$(LOCAL_PATH)/../../Classes/zephyr \
$(LOCAL_PATH)/../../Classes/zephyr/net \
$(LOCAL_PATH)/../../Classes/Global \
$(LOCAL_PATH)/../../Classes/Framework \
$(LOCAL_PATH)/../../Classes/Framework/platform/android \
$(LOCAL_PATH)/../../Classes/Utils \
$(LOCAL_PATH)/../../Classes/Utils/jsoncpp \
$(LOCAL_PATH)/../../Classes/Utils/network \
$(LOCAL_PATH)/../../Classes/Utils/network/http \
$(LOCAL_PATH)/../../Classes/Utils/network/scoket \
$(LOCAL_PATH)/../../Classes/Utils/PngPacketDecode \
$(LOCAL_PATH)/../../Classes/extensions \
$(LOCAL_PATH)/../../Classes/extensions/android \

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static cocosdenshion_static cocos_extension_static pngpacket_static
            
include $(BUILD_SHARED_LIBRARY)

$(call import-add-path, $(LOCAL_PATH)/../../libs)\
$(call import-add-path, $(LOCAL_PATH)/../../libs/cocos2dx/platform/third_party/android/prebuilt)\
$(call import-add-path, $(LOCAL_PATH)/../../Classes/Utils)

$(call import-module,CocosDenshion/android) \
$(call import-module,cocos2dx) \
$(call import-module,extensions)
$(call import-module,PngPacket/android)
